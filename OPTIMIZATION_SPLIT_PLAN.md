# MCCS 优化内容拆分入库建议（按阶段逐步入库）

> 目标：把目前已完成的大量优化内容，按“可回归、可回滚、低风险”拆成多个小批次入库。

## 1. 拆分原则

1. **先底座、后业务**：先入库不会改变业务协议行为的底座项（构建、生命周期、线程框架），再入库 DB/socket/device。
2. **每批可独立回归**：每一批都必须能单独编译、单独验证。
3. **每批都可回滚**：按目录拆分，避免跨层耦合改动混在一批。
4. **风险按顺序下降**：先“稳定性必需项”，后“告警和代码洁净项”。

---

## 2. 推荐拆分批次（6 批）

### 批次 A：构建与发布链路（低风险，先入）

**范围**
- `src/Makefile`

**价值**
- 统一 `CROSS`/`USE_API_LIBS`/依赖生成，保证后续所有批次可重复编译。

**最低验证**
1. `make -C src clean`
2. `make -C src USE_API_LIBS=0 -j4`
3. `make -C src print-config`

---

### 批次 B：生命周期与退出链路（中低风险）

**范围**
- `src/main.cpp`
- `src/Manager.cpp`
- `src/Manager.h`
- `src/db/PvVolatility.cpp`

**价值**
- 可优雅退出；初始化失败可回滚，避免半初始化状态。

**最低验证**
1. `make -C src USE_API_LIBS=0 -j4`
2. 运行后发送 `SIGTERM`，检查关闭日志链路完整。

---

### 批次 C：线程基础设施（中风险）

**范围**
- `src/etc/socket/Thread.cpp`
- `src/etc/socket/Thread.h`

**价值**
- 线程创建/Join/线程表访问更稳，减少并发竞态。

**最低验证**
1. `make -C src USE_API_LIBS=0 -j4`
2. 启停线程压测（重复启动/停止）
3. 观察是否出现 Join 卡死/句柄异常日志

---

### 批次 D：DB 连接与语句所有权（中风险，建议单独入库）

**范围**
- `src/db/Db.cpp`
- `src/db/Db.h`
- `src/db/MemDb.cpp`
- `src/util/Util.cpp`
- `src/util/Util.h`

**价值**
- 从全局句柄向实例句柄迁移，降低跨模块互相影响。

**最低验证**
1. `make -C src USE_API_LIBS=0 -j4`
2. DB 读写回归（配置读取、历史记录读写）
3. 重复 open/close 回归

---

### 批次 E：Socket 停机响应与资源释放（中风险）

**范围**
- `src/io/socket/SocketThread.cpp`
- `src/io/socket/SocketCtrlThread.cpp`
- `src/io/socket/SocketThreads.cpp`
- `src/io/socket/SocketThreads.h`

**价值**
- stop 响应更快；关闭顺序更稳；资源释放可观测。

**最低验证**
1. `make -C src USE_API_LIBS=0 -j4`
2. 模拟断网/连不上设备，执行 stop，统计停机时延
3. 重复 open/close 观察 fd/线程是否增长

---

### 批次 F：设备与协议防御性修复（中高风险，最后入）

**范围（示例）**
- `src/io/device/**`
- `src/net/**`
- `src/io/protocol/**`
- `src/io/can/**`, `src/io/com/**`

**价值**
- 修复空指针、返回值、格式化类型、边界校验等历史问题。

**最低验证**
1. `make -C src USE_API_LIBS=0 -j4`
2. 按设备族分组回归（pcs/bms/dcload/acload/...）
3. 关键通信流程冒烟（读/写/Preset）

---

## 3. 入库顺序建议（按阶段而非按天）

- **阶段 1**：批次 A + B（先保证构建稳定与生命周期可控）
- **阶段 2**：批次 C（线程基础设施）
- **阶段 3**：批次 D（DB 所有权与并发安全）
- **阶段 4**：批次 E（Socket 停机响应与资源释放）
- **阶段 5**：批次 F（设备/协议防御性修复，按设备族分多次）

> 如果资源有限，建议至少先完成阶段 1/2，再评估阶段 3/4 的窗口。

---

## 4. 回滚策略（按批次回滚）

- 构建异常：先回滚批次 A。
- 启停异常：回滚批次 B/C。
- DB 行为异常：只回滚批次 D。
- 网络停机卡顿：只回滚批次 E。
- 设备协议异常：按设备族回滚批次 F 的对应提交。

---

## 5. 给项目经理/评审的简版结论

这次优化建议按 **A→F 六批次** 分阶段入库。这样既能保住当前稳定性收益，又能把回归风险控制在可接受范围内。
