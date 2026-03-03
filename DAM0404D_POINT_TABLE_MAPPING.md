# DAM0404D 点表映射说明（新设备：Model_DAM0404D_PT）

> 说明：该点表设备与基础继电器卡 `Model_DAM0404D` 分开建模。
>
> - `Model_DAM0404D`：基础DO/DI设备（读线圈+读离散输入，按通道控制线圈）
> - `Model_DAM0404D_PT`：点表版电量/电能/开关量设备（03H读 06H~3FH，05H STATUS写）

## 读数据（03H）

- 一次读取保持寄存器区间：`06H ~ 3FH`（长度 `0x3A`）
- `regStart + 0`：通讯状态（0/1）

### 电量信息 float（IEEE754，高字在前）

- `regStart + 1..13` 对应：
  - Ua, Ub, Uc, Uab, Ubc, Uca, Ia, Ib, Ic, PA, PB, PC, PS
- `regStart + 14..20` 对应：
  - QA, QB, QC, QS, SS, PFS, FF

### 电能信息 float

- `regStart + 21` <- EP (2EH,2FH)
- `regStart + 22` <- EQ (32H,33H)
- `regStart + 23` <- WP (36H,37H)
- `regStart + 24` <- WQ (3AH,3BH)

### 开关量

- `regStart + 25` <- D0 (3EH)
- `regStart + 26` <- DI (3FH)

## 写数据

- 使用 `regStart + 25` 作为输出控制命令源。
- 写 `05H STATUS`：仅取低 4 位（bit0~bit3），对应 DO1~DO4。
- 写入命令：`PresetSingleRegister(addr, 0x0005, status)`。
