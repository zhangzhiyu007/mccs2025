# MCCS 稳定性优化：问题发现与修复总清单（详细版）

> 文档目标：面向研发/测试/评审，清晰回答三件事：
> 1) 我们发现了哪些问题；2) 已经修到了什么程度；3) 每类修复如何验证。

---

## 1. 优化边界与原则

- 本轮是 **Phase A/B**：优先修复会导致崩溃、资源泄漏、停机卡死、半初始化状态的高风险问题。
- 对设备协议层采取“保守修复”：优先加防御，不做大规模协议行为重构。
- 构建层面支持 host-only 回归（`USE_API_LIBS=0`），保证在无 vendor 库环境也能持续验证。

---

## 2. 问题与修复明细（按风险域）

## 2.1 生命周期与停机链路（P0）

### 发现的问题
1. 进程主循环不可控退出，清理链路不可达。  
2. 启动中间失败时，已启动模块可能残留（半初始化状态）。  
3. 部分线程/模块启动与停止不对称（尤其波动率线程）。

### 已完成修复
- `main` 增加 `SIGINT/SIGTERM` + `g_stopFlag`，主循环可退出并进入完整 `Uninit` 流程。
- `Manager::Init` 使用统一 `FAIL` 路径，失败时执行 `Uninit` 回滚。
- `Manager` 引入模块启动标志（`m_*Inited/m_*Started`）并按顺序对称关闭。
- `PvVolatility` 启停纳入统一生命周期管理。

### 影响收益
- 从“只能强杀进程”转为“可控停机”。
- 降低半初始化导致的后续异常概率。

---

## 2.2 线程模型与并发契约（P0/P1）

### 发现的问题
1. `pthread_create` 失败路径不统一，线程状态可能误判。  
2. 线程表访问存在并发访问风险。  
3. auto-delete 线程若不 detach，存在 joinable 线程资源泄漏风险。  
4. `Join` 超时直接返回会引入“线程仍运行但对象被释放”的 UAF 风险。  
5. 启动回滚依赖 `IsAlive()` 的判断存在注册时序竞态。

### 已完成修复
- `Start` 检查 `pthread_create` 失败并记录日志。
- 线程表访问加互斥保护，补充 `CurrentThread/ActiveCount` 并发安全接口。
- auto-delete 线程在入口执行 `pthread_detach`。
- `pthread_timedjoin_np` 超时后改为继续 `pthread_join`，消除返回后悬挂线程风险。
- socket 启动回滚路径改为无条件 `Stop` 后删除线程对象，消除 `IsAlive` 异步竞态窗口。

### 影响收益
- 减少线程生命周期竞争条件引发的崩溃和资源泄漏。

---

## 2.3 DB 所有权与语句生命周期（P0/P1）

### 发现的问题
1. SQLite 连接/语句全局共享，跨实例互相影响。  
2. statement 生命周期不完整，prepare/reset/finalize 防护不足。  
3. close 路径顺序及错误处理不完整。  
4. blob 绑定 API 使用错误。

### 已完成修复
- 连接/语句迁移为实例成员：`m_dbConn` / `m_stmt`。
- 引入 `ScopedDbLock`（`pthread_once + recursive mutex`）串行化 DB 操作。
- 增加 `HasStmtLocked` 与空语句保护。
- `Close` 路径调整为先 finalize statement，再 close connection，并处理失败返回。
- `BindBlob` 修正为 `sqlite3_bind_blob`。

### 影响收益
- 降低 DB 并发踩踏和句柄状态错乱问题。

---

## 2.4 IO/Socket 关闭顺序与故障处置（P0）

### 发现的问题
1. socket 关闭失败时仅日志继续，后续继续释放共享资源会导致活线程访问已释放对象。  
2. 关闭顺序不利于阻塞 I/O 及时退出。

### 已完成修复
- `IoManager::Uninit` 改为返回错误码；socket 关闭失败最多重试 3 次，仍失败则 fail-fast。
- `Manager::Uninit` 接入 IO 关闭返回值；若失败则中止后续 `MemDb/Device` 释放，避免 UAF/数据损坏路径。
- `SocketThreads::CloseThreads` 调整为：先 close client（解除阻塞 I/O）→ stop 线程 → delete 对象；并增加关闭耗时日志。

### 影响收益
- 关闭过程从“尽量关”改为“安全优先”，降低停机时崩溃风险。

---

## 2.5 配置路径与历史数据库逻辑（P1）

### 发现的问题
- `HisDbSave/HisDbDel` 中 `db->Open()` 成功/失败判断反转，导致配置读取误判失败。

### 已完成修复
- 修正判断逻辑为 `if (!db->Open())` 才判定失败。

### 影响收益
- 恢复历史配置读取路径的可用性与一致性。

---

## 2.6 构建系统与回归能力（P1）

### 发现的问题
- 原构建流程不利于跨平台复现和无 vendor 库场景回归。

### 已完成修复
- Makefile 支持：`CROSS`、`USE_API_LIBS`、`V`、`print-config`、`-MMD -MP`。
- 支持 object-only 编译路径（`USE_API_LIBS=0`）用于持续编译回归。

### 影响收益
- 将“编译可验证”从环境依赖问题中解耦。

---

## 2.7 设备/协议防御性修复（P1/P2）

### 发现的问题
- 多设备驱动存在：空指针检查不一致、返回值不完整、日志格式与类型不匹配等历史风险。

### 已完成修复（代表性）
- 大量 `m_tcpClient` 判空防护补齐。
- `Com::Write` 使用 `ssize_t` 并补充 EINTR/错误路径处理。
- 多处 `%d/%f/%s` 与实际参数类型不一致修正。
- 若干非 void 函数补齐返回值。

### 影响收益
- 降低现场偶发崩溃概率；提升故障日志可信度。

---

## 3. 最小验收清单（当前版本）

1. `make -C src print-config`
2. `make -C src USE_API_LIBS=0 -j4`
3. 启动后发送 `SIGTERM`，确认有完整停机日志链路。
4. 覆盖历史配置读取（`HisDbSave/HisDbDel`）与 socket 关闭路径。

---

## 4. 版本结论

- 本轮属于“**高风险问题优先修复**”并已完成核心闭环。  
- 仍有后续收敛项（见 `BUG_BACKLOG.md`），但不影响本阶段入库目标：稳定性提升与回归能力增强。
