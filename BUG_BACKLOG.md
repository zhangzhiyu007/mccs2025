# MCCS 遗漏问题清单（详细版）

> 目标：只列剩余问题，不掺杂已完成内容；按优先级给出建议动作与验收方式。

---

## P1（高优先级，建议近期完成）

### P1-1 线程停止策略可观测性与可控性仍不足
- **现状**：`Join` 超时后已改为阻塞 `pthread_join`，安全性提升，但停机时长上限不可控。
- **风险**：极端 I/O 阻塞时，停机耗时不可预测。
- **建议**：
  1) 增加分级告警（5s/15s/30s）；
  2) 输出线程名/模块名/handle；
  3) 明确最终兜底策略（仅告警或人工干预）。
- **验收**：构造阻塞场景，验证日志分级与停机策略符合预期。

### P1-2 Socket 关闭链路缺少分段耗时指标
- **现状**：已有总耗时日志。
- **风险**：定位慢关闭时不知道卡在 close client、ctrl stop 还是 io stop。
- **建议**：补充分段耗时统计与阈值告警。
- **验收**：断网/设备失联场景下能定位具体慢点。

### P1-3 IoManager 失败上报机制可继续强化
- **现状**：已 fail-fast 返回错误码。
- **风险**：上层只能看到错误，缺少结构化失败原因（阶段、重试次数、残留资源数量）。
- **建议**：统一错误码细分或结构化日志字段。
- **验收**：日志可直接用于故障归因。

---

## P2（中优先级，分批治理）

### P2-1 设备/协议历史告警仍较多
- **现状**：仍有 signed/unsigned、unused、格式化类型等警告。
- **建议**：按设备族分批清理（pcs/bms/dcload/acload/net/protocol）。
- **验收**：每批 warning 数可量化下降，行为不回归。

### P2-2 历史 TODO/UNUSED 标记清理
- **现状**：核心模块仍有 TODO/UNUSED 注释路径。
- **建议**：结合真实运行模式清理或补注释（保留原因、计划时间）。
- **验收**：关键路径不再存在“语义不明确”注释。

### P2-3 文档归档一致性
- **现状**：旧文档已归档指向新清单。
- **建议**：后续只维护 `BUG_FIX_SUMMARY.md` 与 `BUG_BACKLOG.md` 两份源文档。
- **验收**：评审入口统一，无重复内容冲突。

---

## 执行建议（务实版）

- 每次只处理 1~2 个 P1/P2 项，避免再次形成大爆改。
- 每次固定验收：
  1. `make -C src USE_API_LIBS=0 -j4`
  2. 对应模块最小冒烟（停机、重连、配置读取）
  3. 回归通过再推进下一项。



## 附：遗留问题关注位置（便于直接排查）

- P1-1 线程停止策略：`src/etc/socket/Thread.cpp`（`Thread::Join`）
- P1-2 Socket 分段耗时：`src/io/socket/SocketThreads.cpp`（`CloseThreads`）
- P1-3 IoManager 失败上报：`src/io/IoManager.cpp`（`Uninit`）/ `src/Manager.cpp`（`Uninit`）
- P2-1 设备告警收敛：`src/io/device/**`、`src/net/**`、`src/io/protocol/**`
- P2-2 TODO/UNUSED 清理：`src/main.cpp`、`src/Manager.cpp`、`src/io/IoManager.cpp`、`src/io/socket/SocketThreads.cpp`

