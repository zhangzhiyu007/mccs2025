/*
 * ChargePileControl.cpp
 *
 * Created on: 2024-04-15
 * Author: zzy
 */

#include "ChargePileControl.h"

// 构造函数
ChargerController::ChargerController() { this->m_tcpClient = NULL; }

// 析构函数
ChargerController::~ChargerController() {
    // m_tcpClient = NULL;
    // m_com       = NULL;
}

// 设置串口通信对象
void ChargerController::SetTcp(TcpClient *tcpClient) {
    this->m_tcpClient = tcpClient;
}

// void ChargerController::SetIsTcp(bool isTcp) { m_isTcp = isTcp; }

// 读取设备数据
int ChargerController::Read(Device::SlaveDev *dev) {
    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "< %s > 读取数据失败", dev->name.c_str());
        return ErrorInfo::ERR_NULL;
    }
    if (!m_tcpClient->IsOpened())
        return ErrorInfo::ERR_OPENED; // TCP连接未开启时返回错误

    return ModbusRead(dev); // 调用内部函数进行Modbus读取
}

int ChargerController::Write(Device::SlaveDev *dev) {
    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "设备空");
        return ErrorInfo::ERR_NULL;
    }
    if (!m_tcpClient->IsOpened())
        return ErrorInfo::ERR_OPENED; // TCP连接未开启时返回错误

    return ModbusPreset(dev); // 调用内部函数进行Modbus设置
}

// 输入寄存器	0032-0033	故障记录	bit0:引导错误
// bit1:过压
// bit2:过流
// 输入寄存器	0064-0065	A相测量电压	单位:0.001V
// 输入寄存器	0066-0067	A相测量电流	单位:0.001A
// 输入寄存器	0068-0069	B相测量电压	单位:0.001V
// 输入寄存器	006A-006B	B相测量电流	单位:0.001A
// 输入寄存器	006C-006D	C相测量电压	单位:0.001V
// 输入寄存器	006E-006F	C相测量电流	单位:0.001A
// 输入寄存器	0070	频率	单位:0.01HZ
// 输入寄存器	0071-0072	总功率	单位: 0.001W
// 输入寄存器	0073-0074	充电时间	单位: 0.001S
// 输入寄存器	0075	充电状态	0:CC未连接
// 1:CC半连接
// 2:CC电阻/PP电压错误
// 3:CP未连接
// 4:完全连接
// 5:CP电压错误
// 6:CP PWM错误
// 7:充电桩就绪
// 8:充电桩未就绪
// 9:车辆就绪
// 10:充电中
// 输入寄存器	0076-0077	CC电阻值	单位: 0.001Ω
// 输入寄存器	0078	S2状态	0:关		1:开
// 输入寄存器	007D-007E	请求电流	单位: 0.001A
// 保持寄存器	00FA-00FB	国标充电桩手动测试模式电流设定	0.001A
// 0x03:数据超出范围 0x0C:禁止操作（不在国标充电桩手动测试模式）
// 保持寄存器	0000	模式	0:系统模式
// 1:国标充电桩老化模式
// 2:国标充电桩手动测试模式
// 3:国标充电桩自动测试模式
// 4:欧标充电桩老化模式
// 5:欧标充电桩手动测试模式
// 6:欧标充电桩自动测试模式
// 7:美标充电桩老化模式
// 8:美标充电桩手动测试模式
// 9:美标充电桩自动测试模式	03:该模式不存在
// 0C:开机不允许切换模式

// 线圈	0000	开关	OFF:关  ON:开	0C:
// 发生错误无法开关、该模式（空闲模式）不允许开机 线圈	0001	复位	ON:复位
// 线圈	0032	国标充电桩手动测试模式S2继电器	OFF:关  ON:开
// 0x0C:不在国标充电桩手动测试模式

// 保持寄存器	0000	模式	0:系统模式
// 1:国标充电桩老化模式
// 2:国标充电桩手动测试模式
// 3:国标充电桩自动测试模式
// 4:欧标充电桩老化模式
// 5:欧标充电桩手动测试模式
// 6:欧标充电桩自动测试模式
// 7:美标充电桩老化模式
// 8:美标充电桩手动测试模式
// 9:美标充电桩自动测试模式	03:该模式不存在
// 0C:开机不允许切换模式
// 保持寄存器	00FA-00FB	国标充电桩手动测试模式电流设定	0.001A
// 0x03:数据超出范围 0x0C:禁止操作（不在国标充电桩手动测试模式）

int ChargerController::ModbusRead(Device::SlaveDev *dev) {
    ModbusRtuMaster rtu;

    rtu.SetIsTcp(true);

    rtu.SetTcpClient(m_tcpClient);
    int addr = atoi(dev->slaveAddr.c_str()); // 从机地址，根据实际情况调整
    int start  = 0x0032; // 起始寄存器地址，根据实际情况调整
    int length = 2;      // 读取的寄存器数量，根据实际情况调整
    UshortArray values;
    values.clear();
    bool comm = false; // 初始化通信标志为假

    int result = rtu.ReadOutputStatus(values, addr, start, length);
    // 输入寄存器	0032-0033	故障记录	bit0:引导错误
    // bit1:过压
    // bit2:过流
    if (result == ErrorInfo::ERR_OK) {
        comm = true; // 通信成功
        // 将两个寄存器的值合并为一个值
        // uint32_t combinedValue = (values[0] << 16) | values[1];
        // // 将整数值转换为单精度浮点数
        // float floatValue = *reinterpret_cast<float *>(&combinedValue);
        // // 将读取到的数据存到内存数据库
        // MemDb::SetRealData(dev->regStart + 1, floatValue);

        uint32_t combinedValue = (values[0] << 16) | values[1];
        float floatValue;
        memcpy(&floatValue, &combinedValue, sizeof(float));
        MemDb::SetRealData(dev->regStart + 1, 0);
    }

    // 输入寄存器	0064-0065	A相测量电压	单位:0.001V
    start  = 0x0064; // 起始寄存器地址，根据实际情况调整
    length = 2;      // 读取的寄存器数量，根据实际情况调整

    result = rtu.ReadInputRegisters(values, addr, start, length);
    if (result == ErrorInfo::ERR_OK) {
        comm = true; // 通信成功
        // 将两个寄存器值组合成一个32位整数
        uint32_t combinedValue = (values[0] << 16) | values[1];

        // 将整数通过应用缩放因子转换为浮点数
        float voltage = combinedValue * 0.001f; // 将毫伏转换为伏特

        // 将计算后的数据存储到内存数据库中
        MemDb::SetRealData(dev->regStart + 2, voltage);
    }
    values.clear();
    // 输入寄存器	0066-0067	A相测量电流	单位:0.001A
    start  = 0x0066; // 起始寄存器地址，根据实际情况调整
    length = 2;      // 读取的寄存器数量，根据实际情况调整
    result = rtu.ReadInputRegisters(values, addr, start, length);
    if (result == ErrorInfo::ERR_OK) {
        comm = true; // 通信成功
        // 将两个寄存器的值合并为一个值
        uint32_t combinedValue = (values[0] << 16) | values[1];

        // 将整数通过应用缩放因子转换为浮点数
        float voltage = combinedValue * 0.001f; // 将毫伏转换为伏特

        // 将计算后的数据存储到内存数据库中
        MemDb::SetRealData(dev->regStart + 3, voltage);
    }
    values.clear();
    // 输入寄存器	0068-0069	B相测量电压	单位:0.001V
    start  = 0x0068; // 起始寄存器地址，根据实际情况调整
    length = 2;      // 读取的寄存器数量，根据实际情况调整
    result = rtu.ReadInputRegisters(values, addr, start, length);
    if (result == ErrorInfo::ERR_OK) {
        comm                   = true; // 通信成功
        uint32_t combinedValue = (values[0] << 16) | values[1];

        // 将整数通过应用缩放因子转换为浮点数
        float voltage = combinedValue * 0.001f; // 将毫伏转换为伏特

        // 将计算后的数据存储到内存数据库中
        MemDb::SetRealData(dev->regStart + 4, voltage);
    }
    values.clear();
    // 输入寄存器	006A-006B	B相测量电流	单位:0.001A
    start  = 0x006A; // 起始寄存器地址，根据实际情况调整
    length = 2;      // 读取的寄存器数量，根据实际情况调整
    result = rtu.ReadInputRegisters(values, addr, start, length);
    if (result == ErrorInfo::ERR_OK) {
        comm = true; // 通信成功
        // 将两个寄存器的值合并为一个值
        uint32_t combinedValue = (values[0] << 16) | values[1];

        // 将整数通过应用缩放因子转换为浮点数
        float voltage = combinedValue * 0.001f; // 将毫伏转换为伏特

        // 将计算后的数据存储到内存数据库中
        MemDb::SetRealData(dev->regStart + 5, voltage);
    }
    values.clear();
    // 输入寄存器	006C-006D	C相测量电压	单位:0.001V
    start  = 0x006C; // 起始寄存器地址，根据实际情况调整
    length = 2;      // 读取的寄存器数量，根据实际情况调整
    result = rtu.ReadInputRegisters(values, addr, start, length);
    if (result == ErrorInfo::ERR_OK) {
        comm = true; // 通信成功
        // 将两个寄存器的值合并为一个值
        uint32_t combinedValue = (values[0] << 16) | values[1];

        // 将整数通过应用缩放因子转换为浮点数
        float voltage = combinedValue * 0.001f; // 将毫伏转换为伏特

        // 将计算后的数据存储到内存数据库中
        MemDb::SetRealData(dev->regStart + 6, voltage);
    }
    values.clear();
    // 输入寄存器	006E-006F	C相测量电流	单位:0.001A
    start  = 0x006E; // 起始寄存器地址，根据实际情况调整
    length = 2;      // 读取的寄存器数量，根据实际情况调整
    result = rtu.ReadInputRegisters(values, addr, start, length);
    if (result == ErrorInfo::ERR_OK) {
        comm = true; // 通信成功
        // 将两个寄存器的值合并为一个值
        uint32_t combinedValue = (values[0] << 16) | values[1];

        // 将整数通过应用缩放因子转换为浮点数
        float voltage = combinedValue * 0.001f; // 将毫伏转换为伏特

        // 将计算后的数据存储到内存数据库中
        MemDb::SetRealData(dev->regStart + 7, voltage);
    }
    values.clear();
    // 输入寄存器	0070	频率	单位:0.01HZ
    start  = 0x0070; // 起始寄存器地址，根据实际情况调整
    length = 1;      // 读取的寄存器数量，根据实际情况调整
    result = rtu.ReadInputRegisters(values, addr, start, length);
    if (result == ErrorInfo::ERR_OK) {
        comm = true; // 通信成功
        // 将读取到的数据存到内存数据库
        float floatValue = values[0] * 0.01;
        MemDb::SetRealData(dev->regStart + 8, floatValue);
    }
    values.clear();
    // 输入寄存器	0071-0072	总功率	单位: 0.001W
    start  = 0x0071; // 起始寄存器地址，根据实际情况调整
    length = 2;      // 读取的寄存器数量，根据实际情况调整
    result = rtu.ReadInputRegisters(values, addr, start, length);
    if (result == ErrorInfo::ERR_OK) {
        comm = true; // 通信成功
        // 将两个寄存器的值合并为一个值
        uint32_t combinedValue = (values[0] << 16) | values[1];

        // 将整数通过应用缩放因子转换为浮点数
        float voltage = combinedValue * 0.001f; // 将毫伏转换为伏特

        // 将计算后的数据存储到内存数据库中
        MemDb::SetRealData(dev->regStart + 9, voltage);
    }
    values.clear();
    // 输入寄存器	0073-0074	充电时间	单位: 0.001S
    start  = 0x0073; // 起始寄存器地址，根据实际情况调整
    length = 2;      // 读取的寄存器数量，根据实际情况调整
    result = rtu.ReadInputRegisters(values, addr, start, length);
    if (result == ErrorInfo::ERR_OK) {
        comm = true; // 通信成功
        // 将两个寄存器的值合并为一个值
        uint32_t combinedValue = (values[0] << 16) | values[1];
        // 将整数值转换为单精度浮点数
        float floatValue;
        memcpy(&floatValue, &combinedValue, sizeof(float));
        floatValue *= 0.001;
        // 将读取到的数据存到内存数据库
        MemDb::SetRealData(dev->regStart + 10, floatValue);
    }
    values.clear();
    // 输入寄存器	0075	充电状态	0:CC未连接
    // 1:CC半连接
    // 2:CC电阻/PP电压错误
    // 3:CP未连接
    // 4:完全连接
    // 5:CP电压错误
    // 6:CP PWM错误
    // 7:充电桩就绪
    // 8:充电桩未就绪
    // 9:车辆就绪
    // 10:充电中
    start  = 0x0075; // 起始寄存器地址，根据实际情况调整
    length = 1;      // 读取的寄存器数量，根据实际情况调整
    result = rtu.ReadInputRegisters(values, addr, start, length);
    if (result == ErrorInfo::ERR_OK) {
        comm = true; // 通信成功
        // 将读取到的数据存到内存数据库
        MemDb::SetRealData(dev->regStart + 11, values[0]);
        if (values[0] == 7) {
            MemDb::SetRealData(dev->regStart + 16, 1);
            MemDb::SetRealData(dev->regStart + 17, 0);
        }
        if (values[0] == 10) {
            MemDb::SetRealData(dev->regStart + 17, 1);
            MemDb::SetRealData(dev->regStart + 16, 0);
        }
    }
    values.clear();
    // 输入寄存器	0078	S2状态	0:关		1:开
    start  = 0x0078; // 起始寄存器地址，根据实际情况调整
    length = 1;      // 读取的寄存器数量，根据实际情况调整
    result = rtu.ReadInputRegisters(values, addr, start, length);
    if (result == ErrorInfo::ERR_OK) {
        comm = true; // 通信成功
        // 将读取到的数据存到内存数据库
        MemDb::SetRealData(dev->regStart + 12, values[0]);
    }
    values.clear();
    // 输入寄存器	007D-007E	请求电流	单位: 0.001A
    start  = 0x007D; // 起始寄存器地址，根据实际情况调整
    length = 2;      // 读取的寄存器数量，根据实际情况调整
    result = rtu.ReadInputRegisters(values, addr, start, length);
    if (result == ErrorInfo::ERR_OK) {
        comm = true; // 通信成功
        // 将两个寄存器的值合并为一个值
        uint32_t combinedValue = (values[0] << 16) | values[1];
        // 将整数值转换为单精度浮点数
        float floatValue;
        memcpy(&floatValue, &combinedValue, sizeof(float));
        floatValue *= 0.001;
        // 将读取到的数据存到内存数据库
        MemDb::SetRealData(dev->regStart + 13, floatValue);
    }
    values.clear();
    // 保持寄存器	00FA-00FB	国标充电桩手动测试模式电流设定	0.001A
    // 0x03:数据超出范围 0x0C:禁止操作（不在国标充电桩手动测试模式）
    start  = 0x00FA; // 起始寄存器地址，根据实际情况调整
    length = 2;      // 读取的寄存器数量，根据实际情况调整
    result = rtu.ReadInputRegisters(values, addr, start, length);
    if (result == ErrorInfo::ERR_OK) {
        comm = true; // 通信成功
        // 将两个寄存器的值合并为一个值
        uint32_t combinedValue = (values[0] << 16) | values[1];
        // 将整数值转换为单精度浮点数
        float floatValue;
        memcpy(&floatValue, &combinedValue, sizeof(float));
        floatValue *= 0.001;
        // 将读取到的数据存到内存数据库
        MemDb::SetRealData(dev->regStart + 14, floatValue);
    }
    values.clear();
    // 保持寄存器	0000	模式	0:系统模式
    // 1:国标充电桩老化模式
    // 2:国标充电桩手动测试模式
    // 3:国标充电桩自动测试模式
    // 4:欧标充电桩老化模式
    // 5:欧标充电桩手动测试模式
    // 6:欧标充电桩自动测试模式
    // 7:美标充电桩老化模式
    // 8:美标充电桩手动测试模式
    // 9:美标充电桩自动测试模式	03:该模式不存在
    // 0C:开机不允许切换模式
    start  = 0x0000; // 起始寄存器地址，根据实际情况调整
    length = 1;      // 读取的寄存器数量，根据实际情况调整
    result = rtu.ReadInputRegisters(values, addr, start, length);
    if (result == ErrorInfo::ERR_OK) {
        comm = true; // 通信成功
        // 将读取到的数据存到内存数据库
        MemDb::SetRealData(dev->regStart + 15, values[0]);
    }
    MemDb::SetRealData(dev->regStart, false);
    values.clear();
    if (comm) {
        MemDb::SetRealData(dev->regStart, comm);
    }

    return result;
}

int ChargerController::ModbusPreset(Device::SlaveDev *dev) {

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "设备空");
        return ErrorInfo::ERR_NULL;
    }

    if (!this->m_tcpClient->IsOpened()) {
        zlog_warn(Util::m_zlog, "网口未打开");
        return ErrorInfo::ERR_OPENED;
    }

    ModbusRtuMaster rtu;
    rtu.SetIsTcp(true);
    rtu.SetTcpClient(m_tcpClient);
    int addr = atoi(dev->slaveAddr.c_str()); // 从机地址，根据实际情况调整
    int start  = 0x0000; // 起始寄存器地址，根据实际情况调整
    int length = 1;      // 写入的寄存器数量，根据实际情况调整
    int status = 0;      // 写入的数据，根据实际情况调整
    int result = ErrorInfo::ERR_OK;
    RealData realData;
    int regStart = dev->regStart;
    FloatArray values;
    UshortArray values1;
    values.clear();
    // 线圈	0000	开关	OFF:关  ON:开	0C:
    // 发生错误无法开关、该模式（空闲模式）不允许开机
    start  = 0x0000; // 起始寄存器地址，根据实际情况调整
    length = 1;      // 写入的寄存器数量，根据实际情况调整
    // 读取内存数据库中的数据
    if (MemDb::GetRealData(realData, regStart + 30, true)) {
        status = realData.pv.data.iValue;
        result = rtu.ForceSingleCoil(addr, start, status);
        // if (result == ErrorInfo::ERR_OK) {
        //     // 读取到的数据存到内存数据库
        //     MemDb::SetRealData(dev->regStart + 30, status, false);
        // }
    }
    // 线圈	0001	复位	ON:复位
    start  = 0x0001; // 起始寄存器地址，根据实际情况调整
    length = 1;      // 写入的寄存器数量，根据实际情况调整
    // 读取内存数据库中的数据
    if (MemDb::GetRealData(realData, regStart + 31, true)) {
        status = realData.pv.data.iValue;
        result = rtu.ForceSingleCoil(addr, start, status);
        // if (result == ErrorInfo::ERR_OK) {
        //     // 读取到的数据存到内存数据库
        //     MemDb::SetRealData(dev->regStart + 31, status, false);
        // }
    }
    // 线圈	0032	国标充电桩手动测试模式S2继电器	OFF:关  ON:开
    // 0x0C:不在国标充电桩手动测试模式
    start  = 0x0032; // 起始寄存器地址，根据实际情况调整
    length = 1;      // 写入的寄存器数量，根据实际情况调整
    // 读取内存数据库中的数据
    if (MemDb::GetRealData(realData, regStart + 32, true)) {
        status = realData.pv.data.iValue;
        result = rtu.ForceSingleCoil(addr, start, status);
        // if (result == ErrorInfo::ERR_OK) {
        //     // 读取到的数据存到内存数据库
        //     MemDb::SetRealData(dev->regStart + 32, status, false);
        // }
    }
    // 保持寄存器	0000	模式	0:系统模式
    // 1:国标充电桩老化模式
    // 2:国标充电桩手动测试模式
    // 3:国标充电桩自动测试模式
    // 4:欧标充电桩老化模式
    // 5:欧标充电桩手动测试模式
    // 6:欧标充电桩自动测试模式
    // 7:美标充电桩老化模式
    // 8:美标充电桩手动测试模式
    // 9:美标充电桩自动测试模式	03:该模式不存在
    // 0C:开机不允许切换模式
    start  = 0x0000; // 起始寄存器地址，根据实际情况调整
    length = 1;      // 写入的寄存器数量，根据实际情况调整
    // 读取内存数据库中的数据
    if (MemDb::GetRealData(realData, regStart + 40, true)) {
        status = realData.pv.data.iValue;
        result = rtu.PresetSingleRegister(addr, start, status);
        // if (result == ErrorInfo::ERR_OK) {
        //     // 读取到的数据存到内存数据库
        //     MemDb::SetRealData(dev->regStart + 40, status, false);
        // }
    }
    // 保持寄存器	00FA-00FB	国标充电桩手动测试模式电流设定	0.001A
    // 0x03:数据超出范围 0x0C:禁止操作（不在国标充电桩手动测试模式）
    start  = 0x00FA; // 起始寄存器地址，根据实际情况调整
    length = 2;      // 写入的寄存器数量，根据实际情况调整
                     // 读取内存数据库中的数据

    if (MemDb::GetRealData(realData, regStart + 41, true)) {
        // 获取单精度浮点数

        //        values.clear();
        //        float tmp = realData.pv.data.iValue * 0.001;
        //        values.push_back(tmp);
        //        result = rtu.PresetRLCFloatRegisters(addr, 0x00FA, 2, values);

        float ftmp = realData.pv.data.dbValue * 1000;
        int tmp;

        memcpy(&tmp, &ftmp, sizeof(ftmp));

        // 将电压值缩放并转换为整数
        //  memcpy(&int_data, &tmp, sizeof(tmp)); // 将浮点数转换为32位整数
        values1.clear();

        // 按小端格式分割整数，确保低位在前，高位在后
        int lowWord  = tmp & 0xFFFF;         // 低16位
        int highWord = (tmp >> 16) & 0xFFFF; // 高16位

        // 确保高位先发送，低位后发送
        values1.push_back(highWord);
        values1.push_back(lowWord);
        result = rtu.PresetMultipleRegisters(addr, 0x00FA, 2, values1);
    }
    return result;
}