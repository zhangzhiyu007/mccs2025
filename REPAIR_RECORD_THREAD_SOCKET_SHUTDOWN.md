# Thread/Socket Shutdown 修复记录（整理版）

> 目的：对近期围绕线程停止、socket 线程池关闭、系统反初始化顺序的修复做一次统一梳理，便于评审、回溯与发布说明引用。

## 1. 背景问题

在系统关闭阶段，曾出现以下高风险路径：

1. `Thread::Join()` 在 `pthread_timedjoin_np` 超时后把状态从 `STOPPING` 改回 `RUNNING`，导致停止意图被“撤销”。
2. `SocketThreads::CloseThreads()` 在 `Stop()` 后仍可能直接释放线程关联对象（线程对象、`TcpClient`、地址对象），与未退出线程并发，形成 UAF 风险。
3. `IoManager::Uninit()` 早期仅记录 socket 关闭失败并继续拆除，可能让存活线程继续访问后续被释放/去初始化的全局资源。
4. `Manager::Uninit()` 在 IO 关闭失败时仍可能继续做 `MemDb::Uninit()` 与 `Device::FreeInstanse()`，放大崩溃与数据破坏风险。

---

## 2. 已实施修复（按组件）

### A. `src/etc/socket/Thread.cpp`

- 调整 `Thread::Join()`：
  - `pthread_timedjoin_np` 超时（`ETIMEDOUT`）时，保持 `m_state = STOPPING`。
  - 其他 join 失败时，同样保持 `m_state = STOPPING`。
- 结果：停止请求不会被回滚，线程循环仍能持续观察到停止态并尽快协作退出。

### B. `src/io/socket/SocketThreads.cpp`

- 调整 `CloseThreads()` 资源释放策略：
  - 先关闭 TCP 连接，帮助阻塞 I/O 返回；
  - 对控制线程/采集线程执行 `Stop()` 后，若线程仍存活则**保留对象**（不 delete）；
  - 若存在存活线程，保留 `m_tcpClients` 和 `m_socketTypeDevs`，返回 `ERR_FAILED`；
  - 仅当线程全部退出后，再释放 `TcpClient` 与地址对象。
- 结果：避免“线程尚在运行但其依赖对象被释放”的悬挂访问路径。

### C. `src/io/IoManager.h` + `src/io/IoManager.cpp`

- 将 `IoManager::Uninit()` 签名从 `void` 改为 `int`，向上层传播关闭状态。
- 增加 socket 线程池关闭重试（最多 3 次，间隔 1s）。
- 在重试耗尽仍失败时，执行**快速失败**：
  - 返回 `ERR_FAILED`；
  - 阻断后续 CAN/COM 拆除流程，避免在 socket 线程可能仍存活时继续拆除共享资源。
- 同步修正文档注释与返回逻辑，确保接口语义一致。

### D. `src/Manager.cpp`

- `Manager::Uninit()` 检查 `m_io.Uninit()` 返回值。
- 当 IO 关闭失败时，记录错误并提前返回，不继续执行 `MemDb::Uninit()` 和 `Device::FreeInstanse()`。
- 结果：在“IO 未完全关闭”场景中避免进入更危险的全局反初始化阶段。

---

## 3. 关闭流程的新行为（摘要）

1. 优先发起 socket 线程停止并尝试回收。
2. 若存在超时/失败，线程状态保持 `STOPPING`，且对象保留，不做危险释放。
3. IO 层重试关闭 socket 线程池；若仍失败，IO 反初始化立即失败返回。
4. 系统管理层收到失败后停止进一步全局资源反初始化。

---

## 4. 风险与后续建议

### 当前已降低的风险

- UAF（线程持有悬空 `TcpClient`/地址/线程对象）。
- 关闭时 stop 状态丢失导致线程继续跑。
- IO 关闭失败后继续拆全局资源导致的崩溃链。

### 建议后续项

- 增加专门的关闭阶段集成测试（模拟 socket 线程卡住/超时场景）。
- 评估在失败返回后由上层调度“延迟重试关闭”的策略（例如后台 watchdog 触发二次清理）。
- 统一 `Uninit()` 系列接口返回码风格（当前已有部分仍为 `void`）。

---

## 5. 验证记录（本轮整理）

- 本次提交为“修复记录重整文档”更新，不引入功能代码变更。
- 相关功能修复的编译验证已在此前各修复提交中通过（`make -C src USE_API_LIBS=0 -j4`）。
