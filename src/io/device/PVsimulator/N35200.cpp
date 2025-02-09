#include "N35200.h"
#include "../../../db/MemDb.h"
#include "../../../util/Util.h"
#include "../../com/Com.h"
// #include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../protocol/modbus/ModbusTcpClient.h"

#include <iostream>

// 构造函数
N35200::N35200() {
    m_tcpClient = NULL;
    m_isTcp     = false;
    m_com       = NULL;
}

// 析构函数
N35200::~N35200() {
    m_tcpClient = NULL;
    m_com       = NULL;
}

// 设置是否通过TCP进行通信
void N35200::SetIsTcp(bool isTcp) { m_isTcp = isTcp; }

void N35200::SetCom(Com *com) { m_com = com; }

// 设置TCP客户端对象
void N35200::SetTcpClient(TcpClient *tcpClient) { m_tcpClient = tcpClient; }

// 从设备读取数据
int N35200::Read(Device::SlaveDev *dev) {

    int ret   = ErrorInfo::ERR_OK; // 初始化错误信息为“无错误”
    bool comm = false;             // 初始化通信标志为假

    int addr = atoi(dev->slaveAddr.c_str()); // 将设备的从地址从字符串转换为整数

    int regStart  = dev->regStart; // 获取设备的寄存器起始地址
    int start     = 10;            // 初始化起始地址为0
    int length    = 2;             // 初始化长度为0
    uint16_t data = 0;             // 初始化数据为0
    float f_data  = 0.0;           // 初始化浮点数据为0.0
    // bool b_data  = false;         // 初始化布尔数据为假

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "从设备为NULL");
        return ErrorInfo::ERR_NULL;
    }
    if (!m_tcpClient->IsOpened()) {
        return ErrorInfo::ERR_OPENED;
    }
    // ModbusRtuMaster modbus;
    ModbusTcpClient modbus;
    // modbus.SetIsTcp(true);
    modbus.SetTcpClient(this->m_tcpClient);
    // modbus.SetCom(m_com);
    UshortArray values;
    values.clear();

    ret =
        modbus.ReadOutputRegisters(values, addr, start, length); // 读取设备状态
    if (ret != ErrorInfo::ERR_OK) {
        // std::cerr << "读取设备状态失败。\n";
        comm = false;
        return ret;
    } else {
        // std::cout << "读取设备状态成功。\n";
        // 假设values包含正确的寄存器值
        // 根据设备文档处理这些值
        // 地址：10
        // 属性：RO
        // 类型：Uint32
        // 字节：4Byte
        // 位 0 输出指示位，0-关闭，1-开启
        // 位 1 0-电压采样正常，1-电压采样超量程
        // 位 2 0-电流采样正常，1-电流采样超量程
        // 位 3 0-源模式，1-载模式
        // 位 4-6 0-CV，1-CC，2-CP，3-CR
        // 位 7-11 0-STATIC，1-CR，2-SEQ，3-CHARGE，4-DISCHARGE，5-RAMP，6-WAVE
        // 位 12 0-近端，1-远端 位 13 0-本地，1-远程 位 14
        // 0-测试未完成，1-测试完成 位 15 0-测试未进行，1-测试进行中
        // 位
        // 16-21
        // 0-无保护事件，1-MF，2-OTP，3-RV，4-OC，5-OV，6-OP，7-OCP，8-OVP，9-OPP，10-LVP，15-SLA1，16-SLA2，17-SLA3，18-SLA4，19-SLA5，20-SLA6，21-SLA7，22-SLA8，23-SLA9
        // 位 22-26 比特位为 0-未开启，比特位为
        // 1-开启BIT22-电压，BIT23-源电流，BIT24-载电流，BIT25-源功率，BIT26-载功
        // 率 位 27 0-并机未打开，1-并机打开 位 28 0-急停复归，1-急停发生 位 29
        // 预留 位 30 0-校准未进行，1-校准进行中 位 31 0-启动未完成，1-启动完成

        uint32_t status = (values[1] << 16) + values[0]; // 从设备读取的状态值
        // 从状态值中提取各个位的值
        bool output  = status & 0x0001;        // 输出指示位
        bool voltage = status & 0x0002;        // 电压采样超量程
        bool current = status & 0x0004;        // 电流采样超量程
        bool source  = status & 0x0008;        // 源模式
        int mode     = (status & 0x0070) >> 4; // CV/CC/CP/CR
        int func =
            (status & 0x0F80) >> 7; // STATIC/CR/SEQ/CHARGE/DISCHARGE/RAMP/WAVE
        bool remote             = status & 0x4000;        // 远程
        bool test               = status & 0x8000;        // 测试进行中
        bool mf                 = status & 0x10000;       // MF
        bool otp                = status & 0x20000;       // OTP
        bool rv                 = status & 0x40000;       // RV
        bool oc                 = status & 0x80000;       // OC
        bool ov                 = status & 0x100000;      // OV
        bool op                 = status & 0x200000;      // OP
        bool ocp                = status & 0x400000;      // OCP
        bool ovp                = status & 0x800000;      // OVP
        bool opp                = status & 0x1000000;     // OPP
        bool lvp                = status & 0x2000000;     // LVP
        bool sla1               = status & 0x4000000;     // SLA1
        bool sla2               = status & 0x8000000;     // SLA2
        bool sla3               = status & 0x10000000;    // SLA3
        bool sla4               = status & 0x20000000;    // SLA4
        bool sla5               = status & 0x40000000;    // SLA5
        bool sla6               = status & 0x80000000;    // SLA6
        bool sla7               = status & 0x100000000;   // SLA7
        bool sla8               = status & 0x200000000;   // SLA8
        bool sla9               = status & 0x400000000;   // SLA9
        bool voltage_bit        = status & 0x400000000;   // 电压
        bool source_current_bit = status & 0x800000000;   // 源电流
        bool load_current_bit   = status & 0x1000000000;  // 载电流
        bool source_power_bit   = status & 0x2000000000;  // 源功率
        bool load_power_bit     = status & 0x4000000000;  // 载功率
        bool parallel           = status & 0x8000000000;  // 并机打开
        bool emergency_stop     = status & 0x10000000000; // 急停发生
        bool calibration        = status & 0x40000000000; // 校准进行中
        bool startsboot         = status & 0x80000000000; // 启动完成
        // 写入内存
        MemDb::SetRealData(regStart + 1, output, false);
        MemDb::SetRealData(regStart + 2, voltage, false);
        MemDb::SetRealData(regStart + 3, current, false);
        MemDb::SetRealData(regStart + 4, source, false);
        MemDb::SetRealData(regStart + 5, mode, false);
        MemDb::SetRealData(regStart + 6, func, false);
        MemDb::SetRealData(regStart + 7, remote, false);
        MemDb::SetRealData(regStart + 8, test, false);
        MemDb::SetRealData(regStart + 9, mf, false);
        MemDb::SetRealData(regStart + 10, otp, false);
        MemDb::SetRealData(regStart + 11, rv, false);
        MemDb::SetRealData(regStart + 12, oc, false);
        MemDb::SetRealData(regStart + 13, ov, false);
        MemDb::SetRealData(regStart + 14, op, false);
        MemDb::SetRealData(regStart + 15, ocp, false);
        MemDb::SetRealData(regStart + 16, ovp, false);
        MemDb::SetRealData(regStart + 17, opp, false);
        MemDb::SetRealData(regStart + 18, lvp, false);
        MemDb::SetRealData(regStart + 19, sla1, false);
        MemDb::SetRealData(regStart + 20, sla2, false);
        MemDb::SetRealData(regStart + 21, sla3, false);
        MemDb::SetRealData(regStart + 22, sla4, false);
        MemDb::SetRealData(regStart + 23, sla5, false);
        MemDb::SetRealData(regStart + 24, sla6, false);
        MemDb::SetRealData(regStart + 25, sla7, false);
        MemDb::SetRealData(regStart + 26, sla8, false);
        MemDb::SetRealData(regStart + 27, sla9, false);
        MemDb::SetRealData(regStart + 28, voltage_bit, false);
        MemDb::SetRealData(regStart + 29, source_current_bit, false);
        MemDb::SetRealData(regStart + 30, load_current_bit, false);
        MemDb::SetRealData(regStart + 31, source_power_bit, false);
        MemDb::SetRealData(regStart + 32, load_power_bit, false);
        MemDb::SetRealData(regStart + 33, parallel, false);
        MemDb::SetRealData(regStart + 34, emergency_stop, false);
        MemDb::SetRealData(regStart + 35, calibration, false);
        MemDb::SetRealData(regStart + 36, startsboot, false);

        // 5.2. 输出设定
        // 5.2.1. 电压设定
        // 地址：78
        // 属性：WR
        // 类型：Float
        // 字节：4Byte
        // 参数：单位：V
        // 默认值：0
        start  = 78;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        uint32_t combined =
            (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 37, f_data, false); // 电压

        // float tmp = 222.00;
        // // 将电压值缩放并转换为整数

        // uint32_t  int_data;
        // values.clear();

        // memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        // // 按小端格式分割整数，确保低位在前，高位在后
        // uint16_t lowWord  = int_data & 0xFFFF;         // 低16位
        // uint16_t highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // // 确保低位先发送，高位后发送

        // values.push_back(lowWord);
        // values.push_back(highWord);
        // // values.push_back(lowWord);
        // ret = modbus.PresetMultipleRegisters(addr, 78, 2, values);

        // 5.2.2. 源电流设定
        // 地址：80
        // 属性：WR
        // 类型：Float
        // 字节：4Byte
        // 参数：单位：A
        // 默认值：可设定最大值
        start  = 80;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // A
        MemDb::SetRealData(regStart + 38, f_data, false); // 源电流

        // 5.2.3.载电流设定
        // 地址：82
        // 属性：WR
        // 类型：Float
        // 字节：4Byte
        // 参数：单位：A
        start  = 82;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // A
        MemDb::SetRealData(regStart + 39, f_data, false); // 载电流

        //  tmp = 10.00;
        // // 将电压值缩放并转换为整数

        // values.clear();

        // memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        // // 按小端格式分割整数，确保低位在前，高位在后
        //  lowWord  = int_data & 0xFFFF;         // 低16位
        //  highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // // 确保低位先发送，高位后发送

        // values.push_back(lowWord);
        // values.push_back(highWord);
        // // values.push_back(lowWord);
        // ret = modbus.PresetMultipleRegisters(addr, 82, 2, values);

        // 默认值：可设定最大值
        // 5.2.4.源功率设定
        // 地址：84
        // 属性：WR
        // 类型：Float
        // 字节：4Byte
        // 参数：单位：W
        // 默认值：可设定最大值
        start  = 84;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // W
        MemDb::SetRealData(regStart + 40, f_data, false); // 源功率

        //  tmp = 1000.00;
        // // 将电压值缩放并转换为整数

        // values.clear();

        // memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        // // 按小端格式分割整数，确保低位在前，高位在后
        //  lowWord  = int_data & 0xFFFF;         // 低16位
        //  highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // // 确保低位先发送，高位后发送

        // values.push_back(lowWord);
        // values.push_back(highWord);
        // // values.push_back(lowWord);
        // ret = modbus.PresetMultipleRegisters(addr, 84, 2, values);

        // 5.2.5.载功率设定
        // 地址：86
        // 属性：WR
        // 类型：Float
        // 字节：4Byte
        // 参数：单位：W
        // 默认值：可设定最大值
        start  = 86;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // W
        MemDb::SetRealData(regStart + 41, f_data, false); // 载功率

        // 5.2.6.保存测试参数
        // 地址：92
        // 属性：WR
        // 类型：int
        // 字节：4Byte
        // 参数：保存设定的电压、电流、功率参数，范围 1~20
        // 默认值：1
        start  = 92;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        data =
            (values[1] * 65536) + values[0]; // 保存设定的电压、电流、功率参数
        MemDb::SetRealData(regStart + 42, data, false); // 保存测试参数

        // 5.2.7.调用测试参数
        // 地址：94
        // 属性：WR
        // 类型：int
        // 字节：4Byte
        // 参数：调用设定的电压、电流、功率参数，范围 1~20
        // 默认值：1
        start  = 94;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        data =
            (values[1] * 65536) + values[0]; // 调用设定的电压、电流、功率参数
        MemDb::SetRealData(regStart + 43, data, false); // 调用测试参数

        // 读取实时数据
        // 5.3.1. 回显电压
        // 地址：12
        // 属性：RO
        // 类型：Float
        // 字节：4Byte
        // 参数：单位：V
        start  = 12;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // V
        MemDb::SetRealData(regStart + 44, f_data, false); // 电压

        // 5.3.2.回显电流
        // 地址：14
        // 属性：RO
        // 类型：Float
        // 字节：4Byte
        // 参数：单位：A
        start  = 14;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // A
        MemDb::SetRealData(regStart + 45, f_data, false); // 电流

        // 5.3.3.回显功率
        // 地址：16
        // 属性：RO
        // 类型：Float
        // 字节：4Byte
        // 参数：单位：W
        start  = 16;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // W
        MemDb::SetRealData(regStart + 46, f_data, false); // 功率

        // 5.3.4.回显电阻
        // 地址：18
        // 属性：RO
        // 类型：float
        // 字节：4Byte
        // 参数：单位：Ω
        start  = 18;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // Ω
        MemDb::SetRealData(regStart + 47, f_data, false); // 电阻

        // 5.3.5.回显电量
        // 地址：20
        // 属性：RO
        // 类型：Float
        // 字节：4Byte
        // 参数：单位：Ah
        start  = 20;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // Ah
        MemDb::SetRealData(regStart + 48, f_data, false); // 电量

        // 5.3.6.回显能量
        // 地址：22
        // 属性：RO
        // 类型：Float
        // 字节：4Byte
        // 参数：单位：KWh
        start  = 22;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // KWh
        MemDb::SetRealData(regStart + 49, f_data, false); // 能量

        // 5.3.7.带载时间
        // 地址：24
        // 属性：RO
        // 类型：int
        // 字节：4Byte
        // 参数：单位：ms
        start  = 24;
        length = 2;
        values.clear();
        float fdata;
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        fdata = ((values[1] * 65536) + values[0]) * 0.001; // ms
        MemDb::SetRealData(regStart + 50, fdata, false);   // 带载时间

        // 5.3.8.控制板温度
        // 地址：26
        // 属性：RO
        // 类型：Float
        // 字节：4Byte
        // 参数：单位：℃
        start  = 26;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // ℃
        MemDb::SetRealData(regStart + 51, f_data, false); // 控制板温度

        // 5.6. 电源控制

        // 5.6.1. 电源开关 ONOFF 控制
        // 地址：62
        // 属性：WR
        // 类型：int
        // 字节：4Byte
        // 参数：电源开关 ONOFF 控制
        // 0-关闭
        // 1-开启
        start  = 62;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0]; // 电源开关 ONOFF 控制
        MemDb::SetRealData(regStart + 52, data, false); // 电源开关 ONOFF 控制

        // 5.6.2.输出优先模式设置
        // 地址：144
        // 属性：WR
        // 类型：int
        // 字节：4Byte
        // 参数：电压电流优先
        // 0-电压优先
        // 1-电流优先
        // 默认值：0
        start  = 144;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];         // 电压电流优先
        MemDb::SetRealData(regStart + 53, data, false); // 输出优先模式设置

        // 5.6.3.工作模式
        // 地址：60
        // 属性：WR
        // 类型：int
        // 字节：4Byte
        // 参数：设置工作模式
        // 0-普通模式
        // 1-恒阻模式
        // 2-序列模式
        // 3-充电功能
        // 4-放电功能
        // 5-缓升缓降
        // 6-预留
        // 7-内阻模式
        // 默认值：0
        start  = 60;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];         // 工作模式
        MemDb::SetRealData(regStart + 54, data, false); // 工作模式

        // 5.9. 并机设定

        // 5.9.1. 主从选择
        // 地址：308
        // 属性：WR
        // 类型：int
        // 字节：4Byte
        // 参数：设置当前机器主从身份：注：修改主从选择需要先关闭并机控制
        // 0-主机；
        // 1-1 号从机；
        // 2-2 号从机；
        // 3-3 号从机；
        // 4-4 号从机
        // 5-5 号从机
        // 6-6 号从机
        // 7-7 号从机
        // 8-8 号从机
        // 9-9 号从机
        // 默认值：0
        start  = 308;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];         // 主从选择
        MemDb::SetRealData(regStart + 55, data, false); // 主从选择

        // 5.9.2.从机个数
        // 地址：310
        // 属性：WR
        // 类型：int
        // 字节：4Byte
        // 参数：设置当前从机个数，范围：1~9；注：修改从机个数需要先关闭并机控制
        start  = 310;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];         // 从机个数
        MemDb::SetRealData(regStart + 56, data, false); // 从机个数

        // 5.9.3.并机控制
        // 地址：312
        // 属性：WR
        // 类型：int
        // 字节：4Byte
        // 参数：设置当前并机控制
        // 0-关闭
        // 1-开启
        // 默认值：0
        start  = 312;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];         // 并机控制
        MemDb::SetRealData(regStart + 57, data, false); // 并机控制

        // 5.15. 光伏模拟测试

        // 5.15.1. 更新 SAS Sandia Mode
        // 地址：51712
        // 属性：WR
        // 类型：Uint32
        // 字节：4Byte
        // 参数：写 1 有效
        start  = 51712;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0]; // 更新 SAS Sandia Mode
        MemDb::SetRealData(regStart + 58, data, false); // 更新 SAS Sandia Mode

        // 5.15.2. SAS Sandia Mode 之 Voc
        // 地址：51714
        // 属性：WR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：V
        start  = 51714;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // V
        MemDb::SetRealData(regStart + 59, f_data,
                           false); // SAS Sandia Mode 之 Voc

        // 5.15.3. SAS Sandia Mode 之 Isc
        // 地址：51716
        // 属性：WR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：A
        start  = 51716;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // A
        MemDb::SetRealData(regStart + 60, f_data,
                           false); // SAS Sandia Mode 之 Isc

        // 5.15.4. SAS Sandia Mode 之 Vmp
        // 地址：51718
        // 属性：WR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：V
        start  = 51718;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // V
        MemDb::SetRealData(regStart + 61, f_data,
                           false); // SAS Sandia Mode 之 Vmp

        // 5.15.5. SAS Sandia Mode 之 Imp
        // 地址：51720
        // 属性：WR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：A
        start  = 51720;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // A
        MemDb::SetRealData(regStart + 62, f_data,
                           false); // SAS Sandia Mode 之 Imp

        // 5.15.6. 更新 SAS En50530 Mode
        // 地址：51722
        // 属性：WR
        // 类型：Uint32
        // 字节：4Byte
        // 参数：单位：
        start  = 51722;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0]; // 更新 SAS En50530 Mode
        MemDb::SetRealData(regStart + 63, data, false); // 更新 SAS En50530 Mode

        // 5.15.7. SAS En50530 Mode 之 Voc
        // 地址：51724
        // 属性：WR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：V
        start  = 51724;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // V
        MemDb::SetRealData(regStart + 64, f_data,
                           false); // SAS En50530 Mode 之 Voc

        // 5.15.8. SAS En50530 Mode 之 Isc
        // 地址：51726
        // 属性：WR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：A
        start  = 51726;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // A
        MemDb::SetRealData(regStart + 65, f_data,
                           false); // SAS En50530 Mode 之 Isc

        // 5.15.9. SAS En50530 Mode 之 Vmp
        // 地址：51728
        // 属性：WR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：V
        start  = 51728;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // V
        MemDb::SetRealData(regStart + 66, f_data,
                           false); // SAS En50530 Mode 之 Vmp

        // 5.15.10. SAS En50530 Mode 之 Imp
        // 地址：51730
        // 属性：WR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：A
        start  = 51730;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // A
        MemDb::SetRealData(regStart + 67, f_data,
                           false); // SAS En50530 Mode 之 Imp

        // 5.15.11. 更新 Sandia Mode
        // 地址：51732
        // 属性：WR
        // 类型：Uint32
        // 字节：4Byte
        // 参数：单位：
        start  = 51732;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];         // 更新 Sandia Mode
        MemDb::SetRealData(regStart + 68, data, false); // 更新 Sandia Mode

        // 5.15.12. Sandia Mode 之参考照度
        // 地址：51734
        // 属性：WR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：W/m2
        start  = 51734;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // W/m2
        MemDb::SetRealData(regStart + 69, f_data,
                           false); // Sandia Mode 之参考照度

        // 5.15.13. Sandia Mode 之参考温度
        // 地址：51736
        // 属性：WR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：℃
        start  = 51736;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // ℃
        MemDb::SetRealData(regStart + 70, f_data,
                           false); // Sandia Mode 之参考温度

        // 5.15.14. Sandia Mode 之 Beta 系数
        // 地址：51738
        // 属性：WR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：%
        start  = 51738;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // %
        MemDb::SetRealData(regStart + 71, f_data,
                           false); // Sandia Mode 之 Beta 系数

        // 5.15.15. Sandia Mode 之 FF 系数
        // 地址：51740
        // 属性：WR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：
        start  = 51740;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // %
        MemDb::SetRealData(regStart + 72, f_data,
                           false); // Sandia Mode 之 FF 系数

        // 5.15.16. Sandia Mode 之照度
        // 地址：51742
        // 属性：WR
        // 类型：Uint32
        // 字节：4Byte
        // 参数：单位：W/m2
        start  = 51742;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];         // W/m2
        MemDb::SetRealData(regStart + 73, data, false); // Sandia Mode 之照度

        // 5.15.17. Sandia Mode 之温度
        // 地址：51744
        // 属性：WR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：℃
        start  = 51744;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // ℃
        MemDb::SetRealData(regStart + 74, f_data, false); // Sandia Mode 之温度

        // 5.15.18. Sandia Mode 之 Pmp
        // 地址：51746
        // 属性：WR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：W
        start  = 51746;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // W
        MemDb::SetRealData(regStart + 75, f_data, false); // Sandia Mode 之 Pmp

        // 5.15.19. Sandia Mode 之 Vmp
        // 地址：51748
        // 属性：WR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：V
        start  = 51748;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // V
        MemDb::SetRealData(regStart + 76, f_data, false); // Sandia Mode 之 Vmp

        // 5.15.20. 更新 En50530 Mode
        // 地址：51750
        // 属性：WR
        // 类型：Uint32
        // 字节：4Byte
        // 参数：写 1 有效
        start  = 51750;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];         // 更新 En50530 Mode
        MemDb::SetRealData(regStart + 77, data, false); // 更新 En50530 Mode

        // 5.15.21. En50530 Mode 之 Pmp
        // 地址：51752
        // 属性：WR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：W
        start  = 51752;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // W
        MemDb::SetRealData(regStart + 78, f_data, false); // En50530 Mode 之 Pmp

        // 5.15.22. En50530 Mode 之 Vmp
        // 地址：51754
        // 属性：WR
        // 类型：Uint32
        // 字节：4Byte
        // 参数：单位：V
        start  = 51754;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];         // V
        MemDb::SetRealData(regStart + 79, data, false); // En50530 Mode 之 Vmp

        // 5.15.23. En50530 Mode 之 FF
        // 地址：51756
        // 属性：WR
        // 类型：Uint32
        // 字节：4Byte
        // 参数：0-c-Si；1-Thin-Film
        start  = 51756;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];         // 0-c-Si；1-Thin-Film
        MemDb::SetRealData(regStart + 80, data, false); // En50530 Mode 之 FF

        // 5.15.24. En50530 Mode 之 Irr
        // 地址：51758
        // 属性：WR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：W/m2
        start  = 51758;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // W/m2
        MemDb::SetRealData(regStart + 81, f_data, false); // En50530 Mode 之 Irr

        // 5.15.25. 当前 IV 曲线的 Voc
        // 地址：51760
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：V
        start  = 51760;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // V
        MemDb::SetRealData(regStart + 82, f_data, false); // 当前 IV 曲线的 Voc

        // 5.15.26. 当前 IV 曲线的 Isc
        // 地址：51762
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：A
        start  = 51762;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // A
        MemDb::SetRealData(regStart + 83, f_data, false); // 当前 IV 曲线的 Isc

        // 5.15.27. 当前 IV 曲线的 Vmp
        // 地址：51764
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：V
        start  = 51764;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // V
        MemDb::SetRealData(regStart + 84, f_data, false); // 当前 IV 曲线的 Vmp

        // 5.15.28. 当前 IV 曲线的 Imp
        // 地址：51766
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：A
        start  = 51766;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // A
        MemDb::SetRealData(regStart + 85, f_data, false); // 当前 IV 曲线的 Imp

        // 5.15.29. 当前 IV 曲线的 Pmp
        // 地址：51768
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：W
        start  = 51768;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // W
        MemDb::SetRealData(regStart + 86, f_data, false); // 当前 IV 曲线的 Pmp

        // 5.16. 电池模拟测试
        //  5.16.1. 运行状态
        //  地址：49152
        //  属性：OR
        //  类型：Uint32
        //  字节：4Byte
        //  说明：
        //  位 0 机器在输出，0-关闭，1-开启
        //  位 1-2 0-模拟器等待中；1-模拟器运行中；2-模拟器暂停中；3-测试结束
        //  位 3 0-正常，1-当前在充电（吸收）
        //  位 4 0-正常，1-当前在放电（释放）
        //  位 5 0-正常，1-厂家调试中
        //  位 6 0-正常，1-开机后从数据未更新，数据无效
        //  位 7 0-正常，1-非法操作
        //  位 8 0-正常，1-SOC 终止条件已经触达
        //  位 9 0-正常，1-VOLT 终止条件已经触达
        //  位 10 0-正常，1-Ah 终止条件已经触达
        //  位 11 0-正常，1-kWh 终止条件已经触达
        //  位 12 0-正常，1-循环次数终止条件已经触达
        //  位 13 0-正常，1-时间终止条件已经触达
        //  位 14-15 保留
        //  位 16 0-正常，1-参数保存失败
        //  位 17 0-正常，1-当前文件空或错误
        //  位 18 0-正常，1-文件提取失败
        //  位 19 0-正常，1-当前参数写入值超限
        //  位 20 0-正常，1-当前参数不可写
        //  位 21-23 保留
        //  位 24 0-正常，1-电池模拟器配置出错
        //  位 25 0-正常，1-电池模拟器自检失败
        //  位 26 0-正常，1-EEPROM 错误
        //  位 27 0-正常，1-电池模拟器保护告警
        //  位 28 0-正常，1-设备保护告警
        start  = 49152;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        // 从状态值中提取各个位的值
        bool isOutput           = values[0] & 0x01;
        int status49152         = (values[0] >> 1) & 0x03;
        bool isCharging         = (values[0] >> 3) & 0x01;
        bool isDischarging      = (values[0] >> 4) & 0x01;
        bool isDebugging        = (values[0] >> 5) & 0x01;
        bool isDataInvalid      = (values[0] >> 6) & 0x01;
        bool isIllegalOperation = (values[0] >> 7) & 0x01;
        bool isSocTerminated    = (values[0] >> 8) & 0x01;
        bool isVoltTerminated   = (values[0] >> 9) & 0x01;
        bool isAhTerminated     = (values[0] >> 10) & 0x01;
        bool isKwhTerminated    = (values[0] >> 11) & 0x01;
        bool isCycleTerminated  = (values[0] >> 12) & 0x01;
        bool isTimeTerminated   = (values[0] >> 13) & 0x01;
        bool isParamSaveFailed  = (values[0] >> 16) & 0x01;
        bool isFileError        = (values[0] >> 17) & 0x01;
        bool isFileExtractError = (values[0] >> 18) & 0x01;
        bool isParamOverLimit   = (values[0] >> 19) & 0x01;
        bool isParamUnwritable  = (values[0] >> 20) & 0x01;
        bool isConfigError      = (values[0] >> 24) & 0x01;
        bool isSelfTestFailed   = (values[0] >> 25) & 0x01;
        bool isEepromError      = (values[0] >> 26) & 0x01;
        bool isBatteryProtect   = (values[0] >> 27) & 0x01;
        bool isDeviceProtect    = (values[0] >> 28) & 0x01;
        MemDb::SetRealData(regStart + 87, isOutput, false); // 机器在输出
        MemDb::SetRealData(regStart + 88, status49152, false); // 运行状态
        MemDb::SetRealData(regStart + 89, isCharging, false); // 当前在充电
        MemDb::SetRealData(regStart + 90, isDischarging, false); // 当前在放电
        MemDb::SetRealData(regStart + 91, isDebugging, false); // 厂家调试中
        MemDb::SetRealData(regStart + 92, isDataInvalid,
                           false); // 开机后从数据未更新，数据无效
        MemDb::SetRealData(regStart + 93, isIllegalOperation,
                           false); // 非法操作
        MemDb::SetRealData(regStart + 94, isSocTerminated,
                           false); // SOC 终止条件已经触达
        MemDb::SetRealData(regStart + 95, isVoltTerminated,
                           false); // VOLT 终止条件已经触达
        MemDb::SetRealData(regStart + 96, isAhTerminated,
                           false); // Ah 终止条件已经触达
        MemDb::SetRealData(regStart + 97, isKwhTerminated,
                           false); // kWh 终止条件已经触达
        MemDb::SetRealData(regStart + 98, isCycleTerminated,
                           false); // 循环次数终止条件已经触达
        MemDb::SetRealData(regStart + 99, isTimeTerminated,
                           false); // 时间终止条件已经触达
        MemDb::SetRealData(regStart + 100, isParamSaveFailed,
                           false); // 参数保存失败
        MemDb::SetRealData(regStart + 101, isFileError,
                           false); // 当前文件空或错误
        MemDb::SetRealData(regStart + 102, isFileExtractError,
                           false); // 文件提取失败
        MemDb::SetRealData(regStart + 103, isParamOverLimit,
                           false); // 当前参数写入值超限
        MemDb::SetRealData(regStart + 104, isParamUnwritable,
                           false); // 当前参数不可写
        MemDb::SetRealData(regStart + 105, isConfigError,
                           false); // 电池模拟器配置出错
        MemDb::SetRealData(regStart + 106, isSelfTestFailed,
                           false); // 电池模拟器自检失败
        MemDb::SetRealData(regStart + 107, isEepromError, false); // EEPROM 错误
        MemDb::SetRealData(regStart + 108, isBatteryProtect,
                           false); // 电池模拟器保护告警
        MemDb::SetRealData(regStart + 109, isDeviceProtect,
                           false); // 设备保护告警

        // 5.16.2. 当前 SOC 值
        //  地址：49154
        //  属性：OR
        //  类型：float
        //  字节：4Byte
        //  参数：单位：%
        start  = 49154;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // %
        MemDb::SetRealData(regStart + 110, f_data, false);

        // 5.16.3. 当前 VOC 值
        // 地址：49156
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：V
        start  = 49156;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // V
        MemDb::SetRealData(regStart + 111, f_data, false);

        // 5.16.4. 当前 RES 值
        // 地址：49158
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：mΩ
        start  = 49158;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // mΩ
        MemDb::SetRealData(regStart + 112, f_data, false);

        // 5.16.5. 当前温度值
        // 地址：49160
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位：℃
        start  = 49160;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // ℃
        MemDb::SetRealData(regStart + 113, f_data, false);

        // 5.16.6. 当前循环次数
        // 地址：49162
        // 属性：OR
        // 类型：Uint32
        // 字节：4Byte
        // 参数：获取当前循环次数
        start  = 49162;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 114, data, false);

        // 5.16.7. 有效合并带载时长
        // 地址：49164
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 s
        start  = 49164;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // s
        MemDb::SetRealData(regStart + 115, f_data, false);

        // 5.16.8. 有效合并带载的放电时长
        // 地址：49166
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 s
        start  = 49166;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // s
        MemDb::SetRealData(regStart + 116, f_data, false);

        // 5.16.9. 有效合并带载的充电时长
        // 地址：49168
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 s
        start  = 49168;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // s
        MemDb::SetRealData(regStart + 117, f_data, false);

        // 5.16.10. 有效合并电量
        // 地址：49170
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 Ah
        start  = 49170;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // Ah
        MemDb::SetRealData(regStart + 117, f_data, false);

        // 5.16.11. 有效放电电量
        // 地址：49172
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 Ah
        start  = 49172;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // Ah
        MemDb::SetRealData(regStart + 118, f_data, false);

        // 5.16.12. 有效充电电量
        // 地址：49174
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 Ah
        start  = 49174;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // Ah
        MemDb::SetRealData(regStart + 119, f_data, false);

        // 5.16.13. 有效合并能量
        // 地址：49176
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 kWh
        start  = 49176;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // kWh
        MemDb::SetRealData(regStart + 120, f_data, false);

        // 5.16.14. 有效放电能量
        // 地址：49178
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 kWh
        start  = 49178;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // kWh
        MemDb::SetRealData(regStart + 121, f_data, false);

        // 5.16.15. 有效充电能量
        // 地址：49180
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 kWh
        start  = 49180;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // kWh
        MemDb::SetRealData(regStart + 122, f_data, false);

        // 5.16.16. 配置电池类型
        // 地址：49192
        // 属性：RW
        // 类型：Uint32
        // 字节：4Byte
        // 参数： 0-用户自定义电池 1；
        // 1-用户自定义电池 2；
        // 2-磷酸铁锂电池；
        // 3-三元锂电池；
        // 4-铅酸电池；
        // 5-镍氢电池；
        // 6-钛酸锂电池；
        // 7-钴酸锂电池；
        // 8-锰酸锂电池。
        start  = 49192;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 123, data, false);

        // 5.16.17. 所配置电池包的初始 SOC 设定值
        // 地址：49194
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：范围 0~100，单位%
        // 默认值：50
        start  = 49194;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 124, f_data, false);

        // 5.16.18. 所配置电池包的线缆阻抗设定值
        // 地址：49200
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：范围从“电阻范围下/上限”两个寄存器读，单位 mΩ
        // 默认值：10.0
        start  = 49200;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 125, f_data, false);

        // 5.16.19. 所配置电池包的单体电池串联数
        // 地址：49204
        // 属性：RW
        // 类型：Uint32
        // 字节：4Byte
        // 参数：范围从“串联数范围下/上限”两个寄存器读
        // 默认值：40
        start  = 49204;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 126, data, false);

        // 5.16.20. 所配置电池包的单体电池并联数
        // 地址：49206
        // 属性：RW
        // 类型：Uint32
        // 字节：4Byte
        // 参数：范围从“并联数范围下/上限”两个寄存器读
        // 默认值：20
        start  = 49206;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 127, data, false);

        // 5.16.21. 所配置电池包的单体电池容量设定值
        // 地址：49208
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：范围从“容量范围下/上限”两个寄存器读
        // 默认值：20
        start  = 49208;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 128, f_data, false);

        // 5.16.22. 所配置电池包的单体电池在 SOC 为 100%时的 VOC 值
        // 地址：49210
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：范围从“设定电压范围下/上限”两个寄存器读
        start  = 49210;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 129, f_data, false);

        // 5.16.23. 所配置电池包的单体电池的平均 RES 值
        // 地址：49212
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：范围从“设定电阻范围下/上限”两个寄存器读
        start  = 49212;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 130, f_data, false);

        // 5.16.24. 所配置电池包的正向限流值
        // 地址：49214
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：范围从“设定电流范围下/上限”两个寄存器读
        // 默认值：设定电流范围上限值
        start  = 49214;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 131, f_data, false);

        // 5.16.25. 所配置电池包的反向限流值
        // 地址：49216
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：范围从“设定电流范围下/上限”两个寄存器读
        // 默认值：设定电流范围上限值
        start  = 49216;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 132, f_data, false);

        // 5.16.26. 所配置电池包的正向限功率值
        // 地址：49218
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：范围从“设定功率范围下/上限”两个寄存器读
        // 默认值：设定功率范围上限值
        start  = 49218;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 133, f_data, false);

        // 5.16.27. 所配置电池包的反向限功率值
        // 地址：49220
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：范围从“设定功率范围下/上限”两个寄存器读
        // 默认值：设定功率范围上限值
        start  = 49220;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 134, f_data, false);

        // 5.16.28. 所配置电池包的循环次数
        // 地址：49232
        // 属性：RW
        // 类型：Uint32
        // 字节：4Byte
        // 参数：范围从“循环次数范围下/上限”两个寄存器读
        // 默认值：0
        start  = 49232;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 135, data, false);

        // 5.16.29. 所配置电池包循环充电 SOC 切换点
        // 地址：49234
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：0-100。充电时，实时 SOC 如若触达该告警值，计 1
        // 次循环。如该告警值为 0，实 时 SOC 触发 100%时，计 1 次循环
        // 默认值：100
        start  = 49234;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 136, f_data, false);

        // 5.16.30. 所配置电池包循环放电 SOC 切换点
        // 地址：49236
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：0-100。放电时，实时 SOC 如若触达该告警值，计 1 次循环。
        // 默认值：0
        start  = 49236;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 137, f_data, false);

        // 5.16.31. 所配置电池包的循环是否使能
        // 地址：49238
        // 属性：RW
        // 类型：Uint32
        // 字节：4Byte
        // 参数：0|1
        // 默认值：1
        start  = 49238;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 138, data, false);

        // 5.16.32. 所配置电池包的充电 SOC 截止值
        // 地址：49242
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：单位%，0-100，0 表示不限定。
        // 默认值：0
        start  = 49242;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 139, f_data, false);

        // 5.16.33. 所配置电池包的充电 VOC 截止值
        // 地址：49244
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：单位 V，下限固定为 0，上限从“设定电压可设范围上限”读。如若为
        // 0，表示不限 定。 默认值：0
        start  = 49244;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 140, f_data, false);

        // 5.16.34. 所配置电池包的充电电量截止值
        // 地址：49246
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：单位 Ah，下限固定为 0，上限从“容量可设范围上限”读。如若为
        // 0，表示不限定。 默认值：0
        start  = 49246;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 141, f_data, false);

        // 5.16.35. 所配置电池包的充电能量截止值
        // 地址：49248
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：单位 Wh，下限固定为 0，上限从“能量可设范围上限”读。如若为
        // 0，表示不限定。 默认值：0
        start  = 49248;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 142, f_data, false);

        // 5.16.36. 所配置电池包的充电时间设定值
        // 地址：49250
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：单位 s，下限固定为 0，上限从“时间可设范围上限”读。如若为
        // 0，表示不限定。 默认值：0
        start  = 49250;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 143, f_data, false);

        // 5.16.37. 所配置电池包的放电 SOC 截止值
        // 地址：49252
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：0~100，单位%。
        // 默认值：0
        start  = 49252;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 144, f_data, false);

        // 5.16.38. 所配置电池包的放电 VOC 截止值
        // 地址：49254
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：单位 V，下限固定为 0，上限从“设定电压可设范围上限”读。如若为
        // 0，表示不限 定。 默认值：0
        start  = 49254;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 145, f_data, false);

        // 5.16.39. 所配置电池包的放电电量截止值
        // 地址：49256
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：单位 Ah，下限固定为 0，上限从“容量可设范围上限”读。如若为
        // 0，表示不限定。 默认值：0
        start  = 49256;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 146, f_data, false);

        // 5.16.40. 所配置电池包的放电能量截止值
        // 地址：49258
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：单位 Wh，下限固定为 0，上限从“能量可设范围上限”读。如若为
        // 0，表示不限定。 默认值：0
        start  = 49258;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 147, f_data, false);

        // 5.16.41. 所配置电池包的放电时间设定值
        // 地址：49260
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：单位 s，下限固定为 0，上限从“时间可设范围上限”读。如若为
        // 0，表示不限定。 默认值：0
        start  = 49260;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 148, f_data, false);

        // 5.16.42. 更新计算电池包
        // 地址：49272
        // 属性：RW
        // 类型：Uint32
        // 字节：4Byte
        // 参数：1-更新 0-保持
        start  = 49272;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 149, data, false);

        // 5.16.43. 开始模拟
        // 地址：49274
        // 属性：RW
        // 类型：Uint32
        // 字节：4Byte
        // 参数：写 1 有效
        start  = 49274;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 150, data, false);

        // 5.16.44. 中止模拟
        // 地址：49276
        // 属性：RW
        // 类型：Uint32
        // 字节：4Byte
        // 参数：写 1 有效
        start  = 49276;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 151, data, false);

        // 5.16.45. 结束模拟
        // 地址：49278
        // 属性：RW
        // 类型：Uint32
        // 字节：4Byte
        // 参数：写 1 有效
        start  = 49278;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 152, data, false);

        // 5.16.46. 所更新电池的状态
        // 地址：49292
        // 属性：OR
        // 类型：Uint32
        // 字节：4Byte
        // 参数：
        // 位 0-7 0-正常，1-重复；2-SOC 与 VOC
        // 非正相关，3-标准曲线非单调递增，4-SOC
        // 范围未从 0 到 100%，5-文件为空，6-文件只有一行，7-预置文件非协调。
        // 位 8-15 第几项（0-无；1-SOC1；2-VOC；3-SOC2；4-SOC1VOC）
        // 位 16-31 第几行
        start  = 49292;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        // 从状态值中提取各个位的值
        int statusa  = data;
        int bit0_7   = statusa & 0xFF;
        int bit8_15  = (statusa >> 8) & 0xFF;
        int bit16_31 = (statusa >> 16) & 0xFFFF;
        MemDb::SetRealData(regStart + 153, bit0_7, false);
        MemDb::SetRealData(regStart + 154, bit8_15, false);
        MemDb::SetRealData(regStart + 155, bit16_31, false);

        // 5.16.47. 所更新的电池模型具有的点数
        // 地址：49294
        // 属性：OR
        // 类型：Uint32
        // 字节：4Byte
        // 参数：获取所更新的电池模型具有的点数
        start  = 49294;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 156, data, false);

        // 5.16.48. 所更新的电池模型是否含双 SOC
        // 地址：49296
        // 属性：OR
        // 类型：Uint32
        // 字节：4Byte
        // 参数：0-否，1-是
        start  = 49296;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 157, data, false);

        // 5.16.49. 选择所更新的电池模型需要查看的点
        // 地址：49298
        // 属性：RW
        // 类型：Uint32
        // 字节：4Byte
        // 参数：范围从“点数可设范围下/上限”两个寄存器读
        start  = 49298;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 158, data, false);

        // 5.16.50. 获取所更新电池指定点的 SOC1 设定值
        // 地址：49300
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：单位%
        start  = 49300;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // %
        MemDb::SetRealData(regStart + 159, f_data, false);

        // 5.16.51. 获取所更新电池指定点的 VOC 设定值
        // 地址：49302
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：单位 V
        start  = 49302;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // V
        MemDb::SetRealData(regStart + 160, f_data, false);

        // 5.16.52. 获取所更新电池指定点的 SOC2 设定值
        // 地址：49304
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：单位%
        start  = 49304;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // %
        MemDb::SetRealData(regStart + 161, f_data, false);

        // 5.16.53. 获取所更新电池指定点的 RES 设定值
        // 地址：49306
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：单位 mΩ
        start  = 49306;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // mΩ
        MemDb::SetRealData(regStart + 162, f_data, false);

        // 5.16.54. 所更新电池单体的 SOC 有效范围的最小值
        // 地址：49312
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位%
        start  = 49312;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // %
        MemDb::SetRealData(regStart + 163, f_data, false);

        // 5.16.55. 所更新电池单体的 SOC 有效范围的最大值
        // 地址：49314
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位%
        start  = 49314;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // %
        MemDb::SetRealData(regStart + 164, f_data, false);

        // 5.16.56. 所更新电池单体的 VOC 有效范围的最小值
        // 地址：49316
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 V
        start  = 49316;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // V
        MemDb::SetRealData(regStart + 165, f_data, false);

        // 5.16.57. 所更新电池单体的 VOC 有效范围的最大值
        // 地址：49318
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 V
        start  = 49318;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // V
        MemDb::SetRealData(regStart + 166, f_data, false);

        // 5.16.58. 所更新电池单体的 RES 有效范围的平均值
        // 地址：49320
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 mΩ
        start  = 49320;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // mΩ
        MemDb::SetRealData(regStart + 167, f_data, false);

        // 5.16.59. 所更新电池包的 SOC 有效范围的最小值
        // 地址：49332
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位%
        start  = 49332;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // %
        MemDb::SetRealData(regStart + 168, f_data, false);

        // 5.16.60. 所更新电池包的 SOC 有效范围的最大值
        // 地址：49334
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位%
        start  = 49334;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // %
        MemDb::SetRealData(regStart + 169, f_data, false);

        // 5.16.61. 所更新电池包的 VOC 有效范围的最小值
        // 地址：49336
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 V
        start  = 49336;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // V
        MemDb::SetRealData(regStart + 170, f_data, false);

        // 5.16.62. 所更新电池包的 VOC 有效范围的最大值
        // 地址：49338
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 V
        start  = 49338;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // V
        MemDb::SetRealData(regStart + 171, f_data, false);

        // 5.16.63. 所更新电池包的 RES 有效范围的平均值
        // 地址：49340
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 mΩ
        start  = 49340;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // mΩ
        MemDb::SetRealData(regStart + 172, f_data, false);

        // 5.16.64. 所更新电池包的总容量值
        // 地址：49342
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 Ah
        start  = 49342;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // Ah
        MemDb::SetRealData(regStart + 173, f_data, false);

        // 5.16.65. 所更新电池包的 VOC 放大倍数
        // 地址：49344
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        start  = 49344;
        length = 2;
        values.clear();
        ret    = modbus.ReadOutputRegisters(values, addr, start, length);
        f_data = ((values[0] * 65536) + values[1]);
        MemDb::SetRealData(regStart + 174, f_data, false);

        // 5.16.66. 所更新电池包的 RES 放大倍数
        // 地址：49346
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        start  = 49346;
        length = 2;
        values.clear();
        ret    = modbus.ReadOutputRegisters(values, addr, start, length);
        f_data = ((values[0] * 65536) + values[1]);
        MemDb::SetRealData(regStart + 175, f_data, false);

        // 5.16.67. 所更新电池包的熔断电流保护阈值
        // 地址：49352
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：单位 A，范围从“保护电流可设范围下/上限”读。如若为
        // 0，表示不限定。
        // 默认值：0
        start  = 49352;
        length = 2;
        values.clear();
        ret    = modbus.ReadOutputRegisters(values, addr, start, length);
        f_data = ((values[0] * 65536) + values[1]);
        MemDb::SetRealData(regStart + 176, f_data, false);

        // 5.16.68. 自定义编辑电芯-指定查看或编辑的电池
        // 地址：49372
        // 属性：RW
        // 类型：Uint32
        // 字节：4Byte
        // 参数：单位 A，范围从“自定义电池类型下/上限”两个寄存器读。
        start  = 49372;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 177, data, false);

        // 5.16.69. 自定义编辑电芯-电芯的点数
        // 地址：49374
        // 属性：RW
        // 类型：Uint32
        // 字节：4Byte
        // 参数：单位 A，范围从“点数可设范围下/上限”读。
        start  = 49374;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 178, data, false);

        // 5.16.70. 自定义编辑电芯-指定电芯是否含双 SOC
        // 地址：49376
        // 属性：RW
        // 类型：Uint32
        // 字节：4Byte
        // 参数：0-否，1-是。
        start  = 49376;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 179, data, false);

        // 5.16.71. 自定义编辑电芯-指定查看或编辑电芯哪个点
        // 地址：49378
        // 属性：RW
        // 类型：Uint32
        // 字节：4Byte
        // 参数：范围从“点数可设范围下/上限”两个寄存器读。
        start  = 49378;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 180, data, false);

        // 5.16.72. 自定义编辑电芯-指定点的 SOC1 设定值
        // 地址：49380
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：单位%，范围 0-100。
        start  = 49380;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 181, f_data, false);

        // 5.16.73. 自定义编辑电芯-指定点的 VOC 设定值
        // 地址：49382
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：单位 V，范围从“设定电压可设范围下/上限”两个寄存器读。
        start  = 49382;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 182, f_data, false);

        // 5.16.74. 自定义编辑电芯-指定点的 SOC2 设定值
        // 地址：49384
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：单位%，范围 0-100。
        start  = 49384;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 183, f_data, false);

        // 5.16.75. 自定义编辑电芯-指定点的 RES 设定值
        // 地址：49386
        // 属性：RW
        // 类型：float
        // 字节：4Byte
        // 参数：单位 mΩ，范围从“设定电阻可设范围下/上限”两个寄存器读。
        start  = 49386;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 184, f_data, false);

        // 5.16.76. 自定义编辑电芯-检测所编辑数据的正确性
        // 地址：49388
        // 属性：RW
        // 类型：Uint32
        // 字节：4Byte
        // 参数：写 1 触发，其他值不响应。
        start  = 49388;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 185, data, false);

        // 5.16.77. 自定义编辑电芯-保存到 EEPROM
        // 地址：49390
        // 属性：RW
        // 类型：Uint32
        // 字节：4Byte
        // 参数：写 0x5AA5A55A 触发，其他值不响应。
        start  = 49390;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 186, data, false);

        // 5.16.78. 自定义编辑电芯-电芯的错误代码
        // 地址：49392
        // 属性：OR
        // 类型：Uint32
        // 字节：4Byte
        start  = 49392;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 187, data, false);

        // 5.16.79. 自定义编辑电芯-电芯的 SOC 有效范围的最小值
        // 地址：49394
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位%
        start  = 49394;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 188, f_data, false);

        // 5.16.80. 自定义编辑电芯-电芯的 SOC 有效范围的最大值
        // 地址：49396
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位%
        start  = 49396;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 189, f_data, false);

        // 5.16.81. 自定义编辑电芯-电芯的 VOC 有效范围的最小值
        // 地址：49398
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 V
        start  = 49398;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 190, f_data, false);

        // 5.16.82. 自定义编辑电芯-电芯的 VOC 有效范围的最大值
        // 地址：49400
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 V
        start  = 49400;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 191, f_data, false);

        // 5.16.83. 自定义编辑电芯-电芯的 RES 有效范围的平均值
        // 地址：49402
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 mΩ
        start  = 49402;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined, sizeof(f_data)); // 从32位整数转换为浮点数
        MemDb::SetRealData(regStart + 192, f_data, false);

        // 5.16.84. 电池总类型下限
        // 地址：49452
        // 属性：OR
        // 类型：uint32
        // 字节：4Byte
        start  = 49452;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 193, data, false);

        // 5.16.85. 电池总类型上限
        // 地址：49454
        // 属性：OR
        // 类型：uint32
        // 字节：4Byte
        start  = 49454;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 194, data, false);

        // 5.16.86. 自定义电池总类型下限
        // 地址：49456
        // 属性：OR
        // 类型：uint32
        // 字节：4Byte
        start  = 49456;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 195, data, false);

        // 5.16.87. 自定义电池总类型上限
        // 地址：49458
        // 属性：OR
        // 类型：uint32
        // 字节：4Byte
        start  = 49458;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 196, data, false);

        // 5.16.88. 标准电池总类型下限
        // 地址：49460
        // 属性：OR
        // 类型：uint32
        // 字节：4Byte
        start  = 49460;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 197, data, false);

        // 5.16.89. 标准电池总类型上限
        // 地址：49462
        // 属性：OR
        // 类型：uint32
        // 字节：4Byte
        start  = 49462;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 198, data, false);

        // 5.16.90. 容量范围下限
        // 地址：49464
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 Ah
        start  = 49464;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // Ah
        MemDb::SetRealData(regStart + 199, f_data, false);

        // 5.16.91. 容量范围上限
        // 地址：49466
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 Ah
        start  = 49466;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // Ah
        MemDb::SetRealData(regStart + 200, f_data, false);

        // 5.16.92. 能量范围下限
        // 地址：49468
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 kWh
        start  = 49468;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // kWh
        MemDb::SetRealData(regStart + 201, f_data, false);

        // 5.16.93. 能量范围上限
        // 地址：49470
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 kWh
        start  = 49470;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // kWh
        MemDb::SetRealData(regStart + 202, f_data, false);

        // 5.16.94. 串联数范围下限
        // 地址：49472
        // 属性：OR
        // 类型：uint32
        // 字节：4Byte
        start  = 49472;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 203, data, false);

        // 5.16.95. 串联数范围上限
        // 地址：49474
        // 属性：OR
        // 类型：uint32
        // 字节：4Byte
        start  = 49474;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 204, data, false);

        // 5.16.96. 并联数范围下限
        // 地址：49476
        // 属性：OR
        // 类型：uint32
        // 字节：4Byte
        start  = 49476;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 205, data, false);

        // 5.16.97. 并联数范围上限
        // 地址：49478
        // 属性：OR
        // 类型：uint32
        // 字节：4Byte
        start  = 49478;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 206, data, false);

        // 5.16.98. 温度范围下限
        // 地址：49480
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位℃
        start  = 49480;
        length = 2;
        values.clear();
        ret    = modbus.ReadOutputRegisters(values, addr, start, length);
        f_data = ((values[0] * 65536) + values[1]); // ℃
        MemDb::SetRealData(regStart + 207, f_data, false);

        // 5.16.99. 温度范围上限
        // 地址：49482
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位℃
        start  = 49482;
        length = 2;
        values.clear();
        ret    = modbus.ReadOutputRegisters(values, addr, start, length);
        f_data = ((values[0] * 65536) + values[1]); // ℃
        MemDb::SetRealData(regStart + 208, f_data, false);

        // 5.16.100. 设定电压范围下限
        // 地址：49484
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 V
        start  = 49484;
        length = 2;
        values.clear();
        ret    = modbus.ReadOutputRegisters(values, addr, start, length);
        f_data = ((values[0] * 65536) + values[1]); // V
        MemDb::SetRealData(regStart + 209, f_data, false);

        // 5.16.101. 设定电压范围上限
        // 地址：49486
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 V
        start  = 49486;
        length = 2;
        values.clear();
        ret    = modbus.ReadOutputRegisters(values, addr, start, length);
        f_data = ((values[0] * 65536) + values[1]); // V
        MemDb::SetRealData(regStart + 210, f_data, false);

        // 5.16.102. 保护电压范围下限
        // 地址：49488
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 V
        start  = 49488;
        length = 2;
        values.clear();
        ret    = modbus.ReadOutputRegisters(values, addr, start, length);
        f_data = ((values[0] * 65536) + values[1]); // V
        MemDb::SetRealData(regStart + 211, f_data, false);

        // 5.16.103. 保护电压范围上限
        // 地址：49490
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 V
        start  = 49490;
        length = 2;
        values.clear();
        ret    = modbus.ReadOutputRegisters(values, addr, start, length);
        f_data = ((values[0] * 65536) + values[1]); // V
        MemDb::SetRealData(regStart + 212, f_data, false);

        // 5.16.104. 设定电流范围下限
        // 地址：49492
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 A
        start  = 49492;
        length = 2;
        values.clear();
        ret    = modbus.ReadOutputRegisters(values, addr, start, length);
        f_data = ((values[0] * 65536) + values[1]); // A
        MemDb::SetRealData(regStart + 213, f_data, false);

        // 5.16.105. 设定电流范围上限
        // 地址：49494
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 A
        start  = 49494;
        length = 2;
        values.clear();
        ret    = modbus.ReadOutputRegisters(values, addr, start, length);
        f_data = ((values[0] * 65536) + values[1]); // A
        MemDb::SetRealData(regStart + 214, f_data, false);

        // 5.16.106. 保护电流范围下限
        // 地址：49496
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 A
        start  = 49496;
        length = 2;
        values.clear();
        ret    = modbus.ReadOutputRegisters(values, addr, start, length);
        f_data = ((values[0] * 65536) + values[1]); // A
        MemDb::SetRealData(regStart + 215, f_data, false);

        // 5.16.107. 保护电流范围上限
        // 地址：49498
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 A
        start  = 49498;
        length = 2;
        values.clear();
        ret    = modbus.ReadOutputRegisters(values, addr, start, length);
        f_data = ((values[0] * 65536) + values[1]); // A
        MemDb::SetRealData(regStart + 216, f_data, false);

        // 5.16.108. 设定功率范围下限
        // 地址：49500
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 A
        start  = 49500;
        length = 2;
        values.clear();
        ret    = modbus.ReadOutputRegisters(values, addr, start, length);
        f_data = ((values[0] * 65536) + values[1]); // A
        MemDb::SetRealData(regStart + 217, f_data, false);

        // 5.16.109. 设定功率范围上限
        // 地址：49502
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 A
        start  = 49502;
        length = 2;
        values.clear();
        ret    = modbus.ReadOutputRegisters(values, addr, start, length);
        f_data = ((values[0] * 65536) + values[1]); // A
        MemDb::SetRealData(regStart + 218, f_data, false);

        // 5.16.110. 保护功率范围下限
        // 地址：49504
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 A
        start  = 49504;
        length = 2;
        values.clear();
        ret    = modbus.ReadOutputRegisters(values, addr, start, length);
        f_data = ((values[0] * 65536) + values[1]); // A
        MemDb::SetRealData(regStart + 219, f_data, false);

        // 5.16.111. 保护功率范围上限
        // 地址：49506
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 A
        start  = 49506;
        length = 2;
        values.clear();
        ret    = modbus.ReadOutputRegisters(values, addr, start, length);
        f_data = ((values[0] * 65536) + values[1]); // A
        MemDb::SetRealData(regStart + 220, f_data, false);

        // 5.16.112. 内阻范围下限
        // 地址：49508
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 mΩ
        start  = 49508;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // mΩ
        MemDb::SetRealData(regStart + 221, f_data, false);

        // 5.16.113. 内阻范围上限
        // 地址：49510
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 mΩ
        start  = 49510;
        length = 2;
        values.clear();
        ret = modbus.ReadOutputRegisters(values, addr, start, length);
        combined = (values[1] << 16) | values[0]; // 按小端合并为32位整数

        memcpy(&f_data, &combined,
               sizeof(f_data)); // 从32位整数转换为浮点数 // mΩ
        MemDb::SetRealData(regStart + 222, f_data, false);

        // 5.16.114. 效率范围下限
        // 地址：49512
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位%
        start  = 49512;
        length = 2;
        values.clear();
        ret    = modbus.ReadOutputRegisters(values, addr, start, length);
        f_data = ((values[0] * 65536) + values[1]); // %
        MemDb::SetRealData(regStart + 223, f_data, false);

        // 5.16.115. 效率范围上限
        // 地址：49514
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位%
        start  = 49514;
        length = 2;
        values.clear();
        ret    = modbus.ReadOutputRegisters(values, addr, start, length);
        f_data = ((values[0] * 65536) + values[1]); // %
        MemDb::SetRealData(regStart + 224, f_data, false);

        // 5.16.116. 点数范围下限
        // 地址：49516
        // 属性：OR
        // 类型：Uint32
        // 字节：4Byte
        start  = 49516;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 225, data, false);

        // 5.16.117. 点数范围上限
        // 地址：49518
        // 属性：OR
        // 类型：Uint32
        // 字节：4Byte
        start  = 49518;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 226, data, false);

        // 5.16.118. 时间范围下限
        // 地址：49520
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 s
        start  = 49520;
        length = 2;
        values.clear();
        ret    = modbus.ReadOutputRegisters(values, addr, start, length);
        f_data = ((values[0] * 65536) + values[1]);
        MemDb::SetRealData(regStart + 227, f_data, false);

        // 5.16.119. 时间范围上限
        // 地址：49522
        // 属性：OR
        // 类型：float
        // 字节：4Byte
        // 参数：单位 s
        start  = 49522;
        length = 2;
        values.clear();
        ret    = modbus.ReadOutputRegisters(values, addr, start, length);
        f_data = ((values[0] * 65536) + values[1]);
        MemDb::SetRealData(regStart + 228, f_data, false);

        // 5.16.120. 循环次数范围下限
        // 地址：49524
        // 属性：OR
        // 类型：Uint32
        // 字节：4Byte
        start  = 49524;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 229, data, false);

        // 5.16.121. 循环次数范围上限
        // 地址：49526
        // 属性：OR
        // 类型：Uint32
        // 字节：4Byte
        start  = 49526;
        length = 2;
        values.clear();
        ret  = modbus.ReadOutputRegisters(values, addr, start, length);
        data = (values[1] * 65536) + values[0];
        MemDb::SetRealData(regStart + 230, data, false);
        comm = true;
    }
    MemDb::SetRealData(regStart, comm, false);

    return ErrorInfo::ERR_OK;
}

// 向设备发送预设参数
int N35200::Preset(Device::SlaveDev *dev) {
    int ret      = ErrorInfo::ERR_OK;
    int addr     = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;
    // int start    = 0;
    // int length   = 0;
    int data = 0;
    // int mode     = 0;
    float tmp = 0.0;
    RealData realData;
    UshortArray values;
    FloatArray Fvalues;
    values.clear();
    Fvalues.clear();
    UshortArray array;
    array.clear();

    uint16_t lowWord;
    uint16_t highWord;

    uint32_t int_data;

    // ModbusRtuMaster modbus;
    ModbusTcpClient modbus;

    // modbus.SetIsTcp(m_isTcp);
    modbus.SetTcpClient(m_tcpClient);
    // UshortArray values;
    // 案例
    //  if (MemDb::GetRealData(realData, regStart + 41, true)) // 设置参数1
    //  {
    //      data = (realData.pv.data.iValue ) >> 16;
    //      values.clear();
    //      values.push_back(data);
    //      data = (realData.pv.data.iValue ) & 0xFFFF;
    //      values.push_back(data);
    //      ret = client.PresetMultipleRegisters(addr, 0x2015, 2, values);
    //  }
    // 5.2.1. 电压设定
    // 地址：78
    // 属性：WR
    // 类型：Float
    // 字节：4Byte
    // 参数：单位：V
    // 默认值：0
    if (MemDb::GetRealData(realData, regStart + 240, true)) {
        tmp = (float)realData.pv.data.iValue;

        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        // 将电压值缩放并转换为整数

        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位
        // printf("%d+-+--+-+%d", lowWord, highWord);
        // 确保低位先发送，高位后发送

        values.push_back(lowWord);
        values.push_back(highWord);
        // values.push_back(lowWord);
        ret = modbus.PresetMultipleRegisters(addr, 78, 2, values);
    }

    // 5.2.2. 源电流设定
    // 地址：80
    // 属性：WR
    // 类型：Float
    // 字节：4Byte
    // 参数：单位：A
    // 默认值：可设定最大值
    if (MemDb::GetRealData(realData, regStart + 241, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 80, 2, values);
    }

    // 5.2.3.载电流设定
    // 地址：82
    // 属性：WR
    // 类型：Float
    // 字节：4Byte
    // 参数：单位：A
    if (MemDb::GetRealData(realData, regStart + 242, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 82, 2, values);
    }

    // 5.2.4.源功率设定
    // 地址：84
    // 属性：WR
    // 类型：Float
    // 字节：4Byte
    // 参数：单位：W
    // 默认值：可设定最大值
    if (MemDb::GetRealData(realData, regStart + 243, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 84, 2, values);
    }

    // 5.2.5.载功率设定
    // 地址：86
    // 属性：WR
    // 类型：Float
    // 字节：4Byte
    // 参数：单位：W
    // 默认值：可设定最大值
    if (MemDb::GetRealData(realData, regStart + 244, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 86, 2, values);
    }

    // 5.2.6.保存测试参数
    // 地址：92
    // 属性：WR
    // 类型：int
    // 字节：4Byte
    // 参数：保存设定的电压、电流、功率参数，范围 1~20
    // 默认值：1
    if (MemDb::GetRealData(realData, regStart + 245, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 92, 1, values);
    }

    // 5.2.7.调用测试参数
    // 地址：94
    // 属性：WR
    // 类型：int
    // 字节：4Byte
    // 参数：调用设定的电压、电流、功率参数，范围 1~20
    // 默认值：1
    if (MemDb::GetRealData(realData, regStart + 246, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 94, 1, values);
    }

    // 5.6.1. 电源开关 ONOFF 控制
    // 地址：62
    // 属性：WR
    // 类型：int
    // 字节：4Byte
    // 参数：电源开关 ONOFF 控制
    // 0-关闭
    // 1-开启
    if (MemDb::GetRealData(realData, regStart + 247, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 62, 1, values);
    }

    // 5.6.2.输出优先模式设置
    // 地址：144
    // 属性：WR
    // 类型：int
    // 字节：4Byte
    // 参数：电压电流优先
    // 0-电压优先
    // 1-电流优先
    // 默认值：0
    if (MemDb::GetRealData(realData, regStart + 248, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 144, 1, values);
    }

    // 5.6.3.工作模式
    // 地址：60
    // 属性：WR
    // 类型：int
    // 字节：4Byte
    // 参数：设置工作模式
    // 0-普通模式
    // 1-恒阻模式
    // 2-序列模式
    // 3-充电功能
    // 4-放电功能
    // 5-缓升缓降
    // 6-预留
    // 7-内阻模式
    // 默认值：0
    if (MemDb::GetRealData(realData, regStart + 249, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 60, 1, values);
    }

    // 5.9.1. 主从选择
    // 地址：308
    // 属性：WR
    // 类型：int
    // 字节：4Byte
    // 参数：设置当前机器主从身份：注：修改主从选择需要先关闭并机控制
    // 0-主机；
    // 1-1 号从机；
    // 2-2 号从机；
    // 3-3 号从机；
    // 4-4 号从机
    // 5-5 号从机
    // 6-6 号从机
    // 7-7 号从机
    // 8-8 号从机
    // 9-9 号从机
    // 默认值：0
    if (MemDb::GetRealData(realData, regStart + 250, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 308, 1, values);
    }

    // 5.9.2.从机个数
    // 地址：310
    // 属性：WR
    // 类型：int
    // 字节：4Byte
    // 参数：设置当前从机个数，范围：1~9；注：修改从机个数需要先关闭并机控制
    if (MemDb::GetRealData(realData, regStart + 251, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 310, 1, values);
    }

    // 5.9.3.并机控制
    // 地址：312
    // 属性：WR
    // 类型：int
    // 字节：4Byte
    // 参数：设置当前并机控制
    // 0-关闭
    // 1-开启
    // 默认值：0
    if (MemDb::GetRealData(realData, regStart + 252, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 312, 1, values);
    }

    // 5.15.1. 更新 SAS Sandia Mode
    // 地址：51712
    // 属性：WR
    // 类型：Uint32
    // 字节：4Byte
    // 参数：写 1 有效
    if (MemDb::GetRealData(realData, regStart + 253, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 51712, 1, values);
    }

    // 5.15.2. SAS Sandia Mode 之 Voc
    // 地址：51714
    // 属性：WR
    // 类型：float
    // 字节：4Byte
    // 参数：单位：V
    if (MemDb::GetRealData(realData, regStart + 254, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        // values.push_back(lowWord);
        values.push_back(lowWord);
        values.push_back(highWord);
        

        ret = modbus.PresetMultipleRegisters(addr, 51714, 2, values);
    }

    // 5.15.3. SAS Sandia Mode 之 Isc
    // 地址：51716
    // 属性：WR
    // 类型：float
    // 字节：4Byte
    // 参数：单位：A
    if (MemDb::GetRealData(realData, regStart + 255, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 51716, 2, values);
    }

    // 5.15.4. SAS Sandia Mode 之 Vmp
    // 地址：51718
    // 属性：WR
    // 类型：float
    // 字节：4Byte
    // 参数：单位：V
    if (MemDb::GetRealData(realData, regStart + 256, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 51718, 2, values);
    }

    // 5.15.5. SAS Sandia Mode 之 Imp
    // 地址：51720
    // 属性：WR
    // 类型：float
    // 字节：4Byte
    // 参数：单位：A
    if (MemDb::GetRealData(realData, regStart + 257, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 51720, 2, values);
    }

    // 5.15.6. 更新 SAS En50530 Mode
    // 地址：51722
    // 属性：WR
    // 类型：Uint32
    // 字节：4Byte
    // 参数：单位：
    if (MemDb::GetRealData(realData, regStart + 258, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 51722, 1, values);
    }

    // 5.15.7. SAS En50530 Mode 之 Voc
    // 地址：51724
    // 属性：WR
    // 类型：float
    // 字节：4Byte
    // 参数：单位：V
    if (MemDb::GetRealData(realData, regStart + 259, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 51724, 2, values);
    }

    // 5.15.8. SAS En50530 Mode 之 Isc
    // 地址：51726
    // 属性：WR
    // 类型：float
    // 字节：4Byte
    // 参数：单位：A
    if (MemDb::GetRealData(realData, regStart + 260, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 51726, 2, values);
    }

    // 5.15.9. SAS En50530 Mode 之 Vmp
    // 地址：51728
    // 属性：WR
    // 类型：float
    // 字节：4Byte
    // 参数：单位：V
    if (MemDb::GetRealData(realData, regStart + 261, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 51728, 2, values);
    }

    // 5.15.10. SAS En50530 Mode 之 Imp
    // 地址：51730
    // 属性：WR
    // 类型：float
    // 字节：4Byte
    // 参数：单位：A
    if (MemDb::GetRealData(realData, regStart + 262, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 51730, 2, values);
    }

    // 5.15.11. 更新 Sandia Mode
    // 地址：51732
    // 属性：WR
    // 类型：Uint32
    // 字节：4Byte
    // 参数：单位：
    if (MemDb::GetRealData(realData, regStart + 263, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 51732, 1, values);
    }

    // 5.15.12. Sandia Mode 之参考照度
    // 地址：51734
    // 属性：WR
    // 类型：float
    // 字节：4Byte
    // 参数：单位：W/m2
    if (MemDb::GetRealData(realData, regStart + 264, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 51734, 2, values);
    }

    // 5.15.13. Sandia Mode 之参考温度
    // 地址：51736
    // 属性：WR
    // 类型：float
    // 字节：4Byte
    // 参数：单位：℃
    if (MemDb::GetRealData(realData, regStart + 265, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 51736, 2, values);
    }

    // 5.15.14. Sandia Mode 之 Beta 系数
    // 地址：51738
    // 属性：WR
    // 类型：float
    // 字节：4Byte
    // 参数：单位：%
    if (MemDb::GetRealData(realData, regStart + 266, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 51738, 2, values);
    }

    // 5.15.15. Sandia Mode 之 FF 系数
    // 地址：51740
    // 属性：WR
    // 类型：float
    // 字节：4Byte
    // 参数：单位：
    if (MemDb::GetRealData(realData, regStart + 267, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 51740, 2, values);
    }

    // 5.15.16. Sandia Mode 之照度
    // 地址：51742
    // 属性：WR
    // 类型：Uint32
    // 字节：4Byte
    // 参数：单位：W/m2
    if (MemDb::GetRealData(realData, regStart + 268, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 51742, 1, values);
    }

    // 5.15.17. Sandia Mode 之温度
    // 地址：51744
    // 属性：WR
    // 类型：float
    // 字节：4Byte
    // 参数：单位：℃
    if (MemDb::GetRealData(realData, regStart + 269, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 51744, 2, values);
    }

    // 5.15.18. Sandia Mode 之 Pmp
    // 地址：51746
    // 属性：WR
    // 类型：float
    // 字节：4Byte
    // 参数：单位：W
    if (MemDb::GetRealData(realData, regStart + 270, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 51746, 2, values);
    }

    // 5.15.19. Sandia Mode 之 Vmp
    // 地址：51748
    // 属性：WR
    // 类型：float
    // 字节：4Byte
    // 参数：单位：V
    if (MemDb::GetRealData(realData, regStart + 271, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 51748, 2, values);
    }

    // 5.15.20. 更新 En50530 Mode
    // 地址：51750
    // 属性：WR
    // 类型：Uint32
    // 字节：4Byte
    // 参数：写 1 有效
    if (MemDb::GetRealData(realData, regStart + 272, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 51750, 1, values);
    }

    // 5.15.21. En50530 Mode 之 Pmp
    // 地址：51752
    // 属性：WR
    // 类型：float
    // 字节：4Byte
    // 参数：单位：W
    if (MemDb::GetRealData(realData, regStart + 273, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 51752, 2, values);
    }

    // 5.15.22. En50530 Mode 之 Vmp
    // 地址：51754
    // 属性：WR
    // 类型：Uint32
    // 字节：4Byte
    // 参数：单位：V
    if (MemDb::GetRealData(realData, regStart + 274, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 51754, 1, values);
    }

    // 5.15.23. En50530 Mode 之 FF
    // 地址：51756
    // 属性：WR
    // 类型：Uint32
    // 字节：4Byte
    // 参数：0-c-Si；1-Thin-Film
    if (MemDb::GetRealData(realData, regStart + 275, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 51756, 1, values);
    }

    // 5.15.24. En50530 Mode 之 Irr
    // 地址：51758
    // 属性：WR
    // 类型：float
    // 字节：4Byte
    // 参数：单位：W/m2
    if (MemDb::GetRealData(realData, regStart + 276, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 51758, 2, values);
    }

    //     5.16.16. 配置电池类型
    // 地址：49192
    // 属性：RW
    // 类型：Uint32
    // 字节：4Byte
    // 参数： 0-用户自定义电池 1；
    // 1-用户自定义电池 2；
    // 2-磷酸铁锂电池；
    // 3-三元锂电池；
    // 4-铅酸电池；
    // 5-镍氢电池；
    // 6-钛酸锂电池；
    // 7-钴酸锂电池；
    // 8-锰酸锂电池。
    if (MemDb::GetRealData(realData, regStart + 300, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 49192, 1, values);
    }

    // 5.16.17. 所配置电池包的初始 SOC 设定值
    // 地址：49194
    // 属性：RW
    // 类型：float
    // 字节：4Byte
    // 参数：范围 0~100，单位%
    // 默认值：50
    if (MemDb::GetRealData(realData, regStart + 301, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 49194, 2, values);
    }

    // 5.16.18. 所配置电池包的线缆阻抗设定值
    // 地址：49200
    // 属性：RW
    // 类型：float
    // 字节：4Byte
    // 参数：范围从“电阻范围下/上限”两个寄存器读，单位 mΩ
    // 默认值：10.0
    if (MemDb::GetRealData(realData, regStart + 302, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 49200, 2, values);
    }

    // 5.16.19. 所配置电池包的单体电池串联数
    // 地址：49204
    // 属性：RW
    // 类型：Uint32
    // 字节：4Byte
    // 参数：范围从“串联数范围下/上限”两个寄存器读
    // 默认值：40
    if (MemDb::GetRealData(realData, regStart + 303, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 49204, 1, values);
    }

    // 5.16.20. 所配置电池包的单体电池并联数
    // 地址：49206
    // 属性：RW
    // 类型：Uint32
    // 字节：4Byte
    // 参数：范围从“并联数范围下/上限”两个寄存器读
    // 默认值：20
    if (MemDb::GetRealData(realData, regStart + 304, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 49206, 1, values);
    }

    // 5.16.21. 所配置电池包的单体电池容量设定值
    // 地址：49208
    // 属性：RW
    // 类型：float
    // 字节：4Byte
    // 参数：范围从“容量范围下/上限”两个寄存器读
    // 默认值：20
    if (MemDb::GetRealData(realData, regStart + 305, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 49208, 2, values);
    }

    // 5.16.22. 所配置电池包的单体电池在 SOC 为 100%时的 VOC 值
    // 地址：49210
    // 属性：RW
    // 类型：float
    // 字节：4Byte
    // 参数：范围从“设定电压范围下/上限”两个寄存器读
    // 默认值：设定电压范围上限值
    if (MemDb::GetRealData(realData, regStart + 306, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 49210, 2, values);
    }

    // 5.16.23. 所配置电池包的单体电池的平均 RES 值
    // 地址：49212
    // 属性：RW
    // 类型：float
    // 字节：4Byte
    // 参数：范围从“设定电阻范围下/上限”两个寄存器读

    if (MemDb::GetRealData(realData, regStart + 307, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 49212, 2, values);
    }

    // 5.16.24. 所配置电池包的正向限流值
    // 地址：49214
    // 属性：RW
    // 类型：float
    // 字节：4Byte
    // 参数：范围从“设定电流范围下/上限”两个寄存器读
    // 默认值：设定电流范围上限值
    if (MemDb::GetRealData(realData, regStart + 308, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 49214, 2, values);
    }

    // 5.16.25. 所配置电池包的反向限流值
    // 地址：49216
    // 属性：RW
    // 类型：float
    // 字节：4Byte
    // 参数：范围从“设定电流范围下/上限”两个寄存器读
    // 默认值：设定电流范围上限值
    if (MemDb::GetRealData(realData, regStart + 309, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 49216, 2, values);
    }

    // 5.16.26. 所配置电池包的正向限功率值
    // 地址：49218
    // 属性：RW
    // 类型：float
    // 字节：4Byte
    // 参数：范围从“设定功率范围下/上限”两个寄存器读
    // 默认值：设定功率范围上限值
    if (MemDb::GetRealData(realData, regStart + 310, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 49218, 2, values);
    }

    // 5.16.27. 所配置电池包的反向限功率值
    // 地址：49220
    // 属性：RW
    // 类型：float
    // 字节：4Byte
    // 参数：范围从“设定功率范围下/上限”两个寄存器读
    // 默认值：设定功率范围上限值
    if (MemDb::GetRealData(realData, regStart + 311, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 49220, 2, values);
    }

    // 5.16.28. 所配置电池包的循环次数
    // 地址：49232
    // 属性：RW
    // 类型：Uint32
    // 字节：4Byte
    // 参数：范围从“循环次数范围下/上限”两个寄存器读
    // 默认值：0
    if (MemDb::GetRealData(realData, regStart + 312, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 49232, 1, values);
    }

    // 5.16.29. 所配置电池包循环充电 SOC 切换点
    // 地址：49234
    // 属性：RW
    // 类型：float
    // 字节：4Byte
    // 参数：0-100。充电时，实时 SOC 如若触达该告警值，计 1 次循环。如该告警值为
    // 0，实 时 SOC 触发 100%时，计 1 次循环 默认值：100
    if (MemDb::GetRealData(realData, regStart + 313, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 49234, 2, values);
    }

    // 5.16.30. 所配置电池包循环放电 SOC 切换点
    // 地址：49236
    // 属性：RW
    // 类型：float
    // 字节：4Byte
    // 参数：0-100。放电时，实时 SOC 如若触达该告警值，计 1 次循环。
    // 默认值：0
    if (MemDb::GetRealData(realData, regStart + 314, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 49236, 2, values);
    }

    // 5.16.31. 所配置电池包的循环是否使能
    // 地址：49238
    // 属性：RW
    // 类型：Uint32
    // 字节：4Byte
    // 参数：0|1
    // 默认值：1
    if (MemDb::GetRealData(realData, regStart + 315, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 49238, 1, values);
    }

    // 5.16.32. 所配置电池包的充电 SOC 截止值
    // 地址：49242
    // 属性：RW
    // 类型：float
    // 字节：4Byte
    // 参数：单位%，0-100，0 表示不限定。
    // 默认值：0
    if (MemDb::GetRealData(realData, regStart + 316, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 49242, 2, values);
    }

    // 5.16.33. 所配置电池包的充电 VOC 截止值
    // 地址：49244
    // 属性：RW
    // 类型：float
    // 字节：4Byte
    // 参数：单位 V，下限固定为 0，上限从“设定电压可设范围上限”读。如若为
    // 0，表示不限 定。 默认值：0
    if (MemDb::GetRealData(realData, regStart + 317, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 49244, 2, values);
    }

    // 5.16.34. 所配置电池包的充电电量截止值
    // 地址：49246
    // 属性：RW
    // 类型：float
    // 字节：4Byte
    // 参数：单位 Ah，下限固定为 0，上限从“容量可设范围上限”读。如若为
    // 0，表示不限定。 默认值：0
    if (MemDb::GetRealData(realData, regStart + 318, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 49246, 2, values);
    }

    // 5.16.35. 所配置电池包的充电能量截止值
    // 地址：49248
    // 属性：RW
    // 类型：float
    // 字节：4Byte
    // 参数：单位 Wh，下限固定为 0，上限从“能量可设范围上限”读。如若为
    // 0，表示不限定。 默认值：0
    if (MemDb::GetRealData(realData, regStart + 319, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 49248, 2, values);
    }

    // 5.16.36. 所配置电池包的充电时间设定值
    // 地址：49250
    // 属性：RW
    // 类型：float
    // 字节：4Byte
    // 参数：单位 s，下限固定为 0，上限从“时间可设范围上限”读。如若为
    // 0，表示不限定。 默认值：0
    if (MemDb::GetRealData(realData, regStart + 320, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 49250, 2, values);
    }

    // 5.16.37. 所配置电池包的放电 SOC 截止值
    // 地址：49252
    // 属性：RW
    // 类型：float
    // 字节：4Byte
    // 参数：0~100，单位%。
    // 默认值：0
    if (MemDb::GetRealData(realData, regStart + 321, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 49252, 2, values);
    }

    // 5.16.38. 所配置电池包的放电 VOC 截止值
    // 地址：49254
    // 属性：RW
    // 类型：float
    // 字节：4Byte
    // 参数：单位 V，下限固定为 0，上限从“设定电压可设范围上限”读。如若为
    // 0，表示不限 定。 默认值：0
    if (MemDb::GetRealData(realData, regStart + 322, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 49254, 2, values);
    }

    // 5.16.39. 所配置电池包的放电电量截止值
    // 地址：49256
    // 属性：RW
    // 类型：float
    // 字节：4Byte
    // 参数：单位 Ah，下限固定为 0，上限从“容量可设范围上限”读。如若为
    // 0，表示不限定。 默认值：0
    if (MemDb::GetRealData(realData, regStart + 323, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 49256, 2, values);
    }

    // 5.16.40. 所配置电池包的放电能量截止值
    // 地址：49258
    // 属性：RW
    // 类型：float
    // 字节：4Byte
    // 参数：单位 Wh，下限固定为 0，上限从“能量可设范围上限”读。如若为
    // 0，表示不限定。 默认值：0
    if (MemDb::GetRealData(realData, regStart + 324, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 49258, 2, values);
    }

    // 5.16.41. 所配置电池包的放电时间设定值
    // 地址：49260
    // 属性：RW
    // 类型：float
    // 字节：4Byte
    // 参数：单位 s，下限固定为 0，上限从“时间可设范围上限”读。如若为
    // 0，表示不限定。 默认值：0
    if (MemDb::GetRealData(realData, regStart + 325, true)) {
        tmp = realData.pv.data.iValue * 1.0;
        // 将电压值缩放并转换为整数
        memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        lowWord  = int_data & 0xFFFF;         // 低16位
        highWord = (int_data >> 16) & 0xFFFF; // 高16位

        // 确保低位先发送，高位后发送
        values.push_back(lowWord);
        values.push_back(highWord);
        ret = modbus.PresetMultipleRegisters(addr, 49260, 2, values);
    }

    // 5.16.42. 更新计算电池包
    // 地址：49272
    // 属性：RW
    // 类型：Uint32
    // 字节：4Byte
    // 参数：1-更新 0-保持
    // 默认值：0
    if (MemDb::GetRealData(realData, regStart + 326, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 49272, 1, values);
    }

    // 5.16.43. 开始模拟
    // 地址：49274
    // 属性：RW
    // 类型：Uint32
    // 字节：4Byte
    // 参数：写 1 有效

    if (MemDb::GetRealData(realData, regStart + 327, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 49274, 1, values);
    }

    // 5.16.44. 中止模拟
    // 地址：49276
    // 属性：RW
    // 类型：Uint32
    // 字节：4Byte
    // 参数：写 1 有效

    if (MemDb::GetRealData(realData, regStart + 328, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 49276, 1, values);
    }

    // 5.16.45. 结束模拟
    // 地址：49278
    // 属性：RW
    // 类型：Uint32
    // 字节：4Byte
    // 参数：写 1 有效

    if (MemDb::GetRealData(realData, regStart + 329, true)) {
        data = realData.pv.data.iValue;
        values.clear();
        values.push_back(data);
        ret = modbus.PresetMultipleRegisters(addr, 49278, 1, values);
    }

    return ErrorInfo::ERR_OK;
}
