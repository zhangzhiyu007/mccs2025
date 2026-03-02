# MCCS 稳定性优化入库说明（阶段 A/B 汇总）

> 文档目的：给“可入库”的本轮稳定性优化做统一说明，便于评审、发布和回溯。

## 1. 当前阶段结论（是否可以告一段落）

可以**阶段性告一段落**，建议以“阶段 A 完成 + 阶段 B 关键项完成”入库。

- 已完成可直接支撑上线稳定性的底座项：生命周期可退出、启动失败可回滚、线程基础契约增强、DB 连接/语句实例化、Socket 停机响应优化。
- 仍有后续优化空间（不阻塞本次入库）：Thread 停止超时机制、SocketThreads 关闭超时门限、部分历史变量清理。

## 2. 本轮优化总览（可入库范围）

### 2.1 生命周期与优雅停机

- `main.cpp`
  - 增加 `SIGINT/SIGTERM` 处理与 `g_stopFlag` 退出条件。
  - 主循环可退出，退出后执行 `watchdog close -> manager.Uninit -> Util::Uninit`。
- `Manager.cpp/.h`
  - `Init()` 改为统一失败路径（`goto FAIL`）并在失败时 `Uninit()` 回滚。
  - 增加模块启动标志（`m_memDbInited/m_deviceInited/m_ioStarted/m_netStarted/m_ctrlStarted/m_pvVolStarted`）实现对称清理。

### 2.2 线程框架与并发契约

- `src/etc/socket/Thread.cpp/.h`
  - `Start()` 检查 `pthread_create` 返回值，失败不置 RUNNING。
  - `Join()` 对无效句柄保护并在成功后清零句柄。
  - 全局线程表访问统一互斥保护，补齐 `CurrentThread()` 与 `ActiveCount()` 并发安全。

### 2.3 数据库（SQLite）安全与所有权隔离

- `src/db/Db.cpp/.h`
  - 将 DB 连接和语句从全局改为实例成员：`m_dbConn`、`m_stmt`。
  - 增加 `ScopedDbLock`（`pthread_once` + 递归 mutex）串行化 DB 操作。
  - 修复 statement 生命周期：`Prepare` 前清理旧 statement、`Finalize/Reset/Step` 防御性处理。
  - 新增 `HasStmtLocked()`，统一空语句保护。
  - 修正 `BindBlob` 调用为 `sqlite3_bind_blob`。
  - `Close()` 先 finalize statement，再关闭连接并检查关闭返回值。

### 2.4 历史库配置路径关键修复

- `src/db/HisDbSave.cpp`
- `src/db/HisDbDel.cpp`
  - 修复 `db->Open()` 判断反转问题：改为 `if (!db->Open())` 才判失败，避免“打开成功被误判失败”。

### 2.5 Socket 线程/停机时延优化

- `src/io/socket/SocketThread.cpp`
- `src/io/socket/SocketCtrlThread.cpp`
  - 引入可中断睡眠辅助函数，固定 sleep 改为 stop-aware 等待。
  - 增加 `TcpClient` 空指针保护，避免异常初始化路径崩溃。
- `src/io/socket/SocketThreads.cpp`
  - 优化关闭顺序：先 `Close()` 所有 client 以促使阻塞 I/O 快速返回，再停线程，再释放对象。
  - 增加关闭前资源计数日志与关闭总耗时日志，提升可观测性。

### 2.6 构建系统现代化

- `src/Makefile`
  - 支持 `CROSS`、`USE_API_LIBS`、`V`、依赖文件生成（`-MMD -MP`）、`print-config`。
  - 支持 host/cross 两类路径与 object-only 编译路径。

### 2.7 其他防御性增强（跨模块）

- 多处设备/协议代码增加空指针检查（尤其 `m_tcpClient`）。
- 日志格式与类型转换修正（`size_t/int` 等）。
- 若干返回值路径补全、字符串处理安全性增强。
- `Util` 中日志路径和 `/proc` 读取稳健性增强，`InitMutex` 改为双指针返回并增强错误处理。

## 3. 入库价值（对运行稳定性的直接收益）

- 系统可以被信号优雅停机，避免“只能强杀”。
- 启动失败可回滚，显著降低半启动/半残留状态。
- DB 从共享句柄改为实例句柄后，跨模块互相踩踏风险显著下降。
- Socket 线程 stop 响应更快，停机卡顿概率下降。
- 构建链路更可重复，便于 CI 与目标环境迁移。

## 4. 回归与验证建议（入库前最低门槛）

建议最少执行：

1. `make -C src USE_API_LIBS=0 -j4`
2. 启动后发送 `SIGTERM`，确认日志中出现完整关闭链路（`Manager::Uninit` 与 socket/db 清理日志）。
3. 关键配置读取验证：历史配置能正常加载（覆盖 `HisDbSave/HisDbDel` 的 db open 路径）。

## 5. 已知遗留（不阻塞本次入库）

- `Thread::Stop()/Join()` 已补充超时等待与告警，后续可继续细化分阶段降级策略。
- `SocketThreads::CloseThreads()` 可继续引入“超时告警阈值”与更细粒度阶段耗时。
- `SocketCtrlThread` 未使用状态变量已清理；后续可继续做循环逻辑简化。

## 6. 回滚与兼容性说明

- 本轮变更以“保接口、强健内部行为”为主，外部调用接口基本保持不变。
- 如出现现场回归，建议按模块回滚：
  1) socket 关闭链路变更
  2) db 实例化变更
  3) manager/main 生命周期变更

## 7. 结论

本轮优化已具备**阶段性入库条件**：建议以“稳定性强化阶段 A/B”名义入库，并在下一迭代继续收敛超时治理与观测指标。

## 8. 拆分入库建议（避免一次性大合并）

建议按 `OPTIMIZATION_SPLIT_PLAN.md` 执行分批入库（A→F 六批次）：

- A 构建链路
- B 生命周期
- C 线程基础设施
- D DB 所有权
- E Socket 停机响应
- F 设备/协议防御性修复

每批都应执行独立回归与独立回滚验证，避免一次性大范围合并。

