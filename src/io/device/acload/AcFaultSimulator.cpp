#include "AcFaultSimulator.h"
#include "../../../util/Util.h"

// 交流故障模拟器类

AcFaultSimulator::AcFaultSimulator() : m_tcpClient(NULL) {}

AcFaultSimulator::~AcFaultSimulator() {}

// 设置Modbus通信所需的TCP客户端
void AcFaultSimulator::SetTcpClient(TcpClient *client) { m_tcpClient = client; }

// 读取设备状态的实现
int AcFaultSimulator::Read(Device::SlaveDev *dev) {
    if (!dev)
        return ErrorInfo::ERR_NULL; // 设备指针为空时返回错误
    if (!m_tcpClient->IsOpened())
        return ErrorInfo::ERR_OPENED; // TCP连接未开启时返回错误

    return ModbusRead(dev); // 调用内部函数进行Modbus读取
}

// 设置设备参数的实现
int AcFaultSimulator::Preset(Device::SlaveDev *dev) {
    if (!dev)
        return ErrorInfo::ERR_NULL; // 设备指针为空时返回错误
    if (!m_tcpClient->IsOpened())
        return ErrorInfo::ERR_OPENED; // TCP连接未开启时返回错误

    return ModbusPreset(dev); // 调用内部函数进行Modbus设置
}

// 使用Modbus协议读取寄存器数据
int AcFaultSimulator::ModbusRead(Device::SlaveDev *dev) {
    ModbusRtuMaster rtu;
    rtu.SetTcpClient(m_tcpClient);
    int addr = atoi(dev->slaveAddr.c_str()); // 从机地址，根据实际情况调整
    int start  = 50; // 起始寄存器地址，根据实际情况调整
    int length = 1;  // 读取的寄存器数量，根据实际情况调整
    UshortArray values;
    // 档位输出寄存器定义列表（读写 功能码：03 06）
    // 功能	    写（阻抗寄存器）	读（阻抗寄存器）	备注
    // 短路加载 	    50.0	    100.0	-
    // 短路卸载	        50.1	    100.1	-
    // 选择A相（A-B）	50.2	    100.2
    // 选择B相（B-C）	50.3	    100.3
    // 选择C相（A-C）	50.4	    100.4
    // 短路电阻选择10Ω	50.5	    100.5	单选
    // 短路电阻选择5Ω	50.6	    100.6
    // 短路电阻选择2Ω	50.7	    100.7
    // 短路电阻选择0Ω	50.8	    100.8
    // 选择对地短路	    50.9	    100.9	单选
    // 选择对零短路	    50.10	    100.10
    // 选择相间短路	    50.11	    100.11
    // 急停状态	-	                100.12	-
    // 风机状态	-	                100.13	风机停止后才能再次加载
    // 设置加载时间	    55	        101	    只能设置1秒-10秒
    int result = rtu.ReadOutputRegisters(values, addr, start, length);
    if (result == ErrorInfo::ERR_OK) {
        // 从状态值中提取各个位的值
        bool shortCircuit = (values[0] & 0x0001) == 0x0001;
        bool shortUnload  = (values[0] & 0x0002) == 0x0002;
        bool selectA      = (values[0] & 0x0004) == 0x0004;
        bool selectB      = (values[0] & 0x0008) == 0x0008;
        bool selectC      = (values[0] & 0x0010) == 0x0010;
        bool short10      = (values[0] & 0x0020) == 0x0020;
        bool short5       = (values[0] & 0x0040) == 0x0040;
        bool short2       = (values[0] & 0x0080) == 0x0080;
        bool short0       = (values[0] & 0x0100) == 0x0100;
        bool shortGround  = (values[0] & 0x0200) == 0x0200;
        bool shortZero    = (values[0] & 0x0400) == 0x0400;
        bool shortPhase   = (values[0] & 0x0800) == 0x0800;
        bool emergency    = (values[0] & 0x1000) == 0x1000;
        bool fanStatus    = (values[0] & 0x2000) == 0x2000;

        // 读取到的数据存储到内存数据库
        MemDb::SetRealData(dev->regStart, shortCircuit, false);
        MemDb::SetRealData(dev->regStart + 1, shortUnload, false);
        MemDb::SetRealData(dev->regStart + 2, selectA, false);
        MemDb::SetRealData(dev->regStart + 3, selectB, false);
        MemDb::SetRealData(dev->regStart + 4, selectC, false);
        MemDb::SetRealData(dev->regStart + 5, short10, false);
        MemDb::SetRealData(dev->regStart + 6, short5, false);
        MemDb::SetRealData(dev->regStart + 7, short2, false);
        MemDb::SetRealData(dev->regStart + 8, short0, false);
        MemDb::SetRealData(dev->regStart + 9, shortGround, false);
        MemDb::SetRealData(dev->regStart + 10, shortZero, false);
        MemDb::SetRealData(dev->regStart + 11, shortPhase, false);
        MemDb::SetRealData(dev->regStart + 12, emergency, false);
        MemDb::SetRealData(dev->regStart + 13, fanStatus, false);
    }
    start  = 55; // 加载时间寄存器地址，根据实际情况调整
    result = rtu.ReadInputRegisters(values, addr, start, length);
    if (result == ErrorInfo::ERR_OK) {
        // 读取到的数据存储到内存数据库
        MemDb::SetRealData(dev->regStart + 14, values[0], false);
    }
    return result;
}

// 使用Modbus协议设置寄存器数据
int AcFaultSimulator::ModbusPreset(Device::SlaveDev *dev) {
    ModbusRtuMaster rtu;
    rtu.SetTcpClient(m_tcpClient);
    int addr = atoi(dev->slaveAddr.c_str()); // 从机地址，根据实际情况调整
    // int reg = 100; // 寄存器地址，根据实际情况调整
    UshortArray values;
    bool status = false;
    RealData realData;
    int regStart = dev->regStart;
    int ret      = ErrorInfo::ERR_OK;
    int start    = 100;
    // 档位输出寄存器定义列表（读写 功能码：03 06）
    // 功能	    写（阻抗寄存器）	读（阻抗寄存器）	备注
    // 短路加载 	    50.0	    100.0	-
    // 短路卸载	        50.1	    100.1	-
    // 选择A相（A-B）	50.2	    100.2
    // 选择B相（B-C）	50.3	    100.3
    // 选择C相（A-C）	50.4	    100.4
    // 短路电阻选择10Ω	50.5	    100.5	单选
    // 短路电阻选择5Ω	50.6	    100.6
    // 短路电阻选择2Ω	50.7	    100.7
    // 短路电阻选择0Ω	50.8	    100.8
    // 选择对地短路	    50.9	    100.9	单选
    // 选择对零短路	    50.10	    100.10
    // 选择相间短路	    50.11	    100.11
    // 急停状态	-	                100.12	-
    // 风机状态	-	                100.13	风机停止后才能再次加载
    // 设置加载时间	    55	        101	    只能设置1秒-10秒

    // 短路加载
    if (MemDb::GetRealData(realData, regStart+16, true)) {
        status = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, start, status<<0);
        if (ret == ErrorInfo::ERR_OK) {
            MemDb::SetRealData(regStart + 16, status, true);
        }
    }
    // 短路卸载
    if (MemDb::GetRealData(realData, regStart + 17, true)) {
        status = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, start, status<<1);
        if (ret == ErrorInfo::ERR_OK) {
            MemDb::SetRealData(regStart + 17, status, true);
        }
    }
    // 选择A相（A-B）
    if (MemDb::GetRealData(realData, regStart + 18, true)) {
        status = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, start, status<<2);
        if (ret == ErrorInfo::ERR_OK) {
            MemDb::SetRealData(regStart + 18, status, true);
        }
    }
    // 选择B相（B-C）
    if (MemDb::GetRealData(realData, regStart + 19, true)) {
        status = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, start, status<<3);
        if (ret == ErrorInfo::ERR_OK) {
            MemDb::SetRealData(regStart + 19, status, true);
        }
    }
    // 选择C相（A-C）
    if (MemDb::GetRealData(realData, regStart + 20, true)) {
        status = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, start, status<<4);
        if (ret == ErrorInfo::ERR_OK) {
            MemDb::SetRealData(regStart + 20, status, true);
        }
    }
    // 短路电阻选择10Ω
    if (MemDb::GetRealData(realData, regStart + 21, true)) {
        status = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, start, status<<5);
        if (ret == ErrorInfo::ERR_OK) {
            MemDb::SetRealData(regStart + 21, status, true);
        }
    }
    // 短路电阻选择5Ω
    if (MemDb::GetRealData(realData, regStart + 22, true)) {
        status = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, start, status<<6);
        if (ret == ErrorInfo::ERR_OK) {
            MemDb::SetRealData(regStart + 22, status, true);
        }
    }
    // 短路电阻选择2Ω
    if (MemDb::GetRealData(realData, regStart + 23, true)) {
        status = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, start, status<<7);
        if (ret == ErrorInfo::ERR_OK) {
            MemDb::SetRealData(regStart + 23, status, true);
        }
    }
    // 短路电阻选择0Ω
    if (MemDb::GetRealData(realData, regStart + 24, true)) {
        status = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, start, status<<8);
        if (ret == ErrorInfo::ERR_OK) {
            MemDb::SetRealData(regStart + 24, status, true);
        }
    }
    // 选择对地短路
    if (MemDb::GetRealData(realData, regStart + 25, true)) {
        status = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, start, status<<9);
        if (ret == ErrorInfo::ERR_OK) {
            MemDb::SetRealData(regStart + 25, status, true);
        }
    }
    // 选择对零短路
    if (MemDb::GetRealData(realData, regStart + 26, true)) {
        status = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, start, status<<10);
        if (ret == ErrorInfo::ERR_OK) {
            MemDb::SetRealData(regStart + 26, status, true);
        }
    }
    // 选择相间短路
    if (MemDb::GetRealData(realData, regStart + 27, true)) {
        status = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, start, status<<11);
        if (ret == ErrorInfo::ERR_OK) {
            MemDb::SetRealData(regStart + 27, status, true);
        }
    }
    // 急停状态
    if (MemDb::GetRealData(realData, regStart + 28, true)) {
        status = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, start, status<<12);
        if (ret == ErrorInfo::ERR_OK) {
            MemDb::SetRealData(regStart + 28, status, true);
        }
    }
    // 风机状态
    if (MemDb::GetRealData(realData, regStart + 29, true)) {
        status = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, start, status<<13);
        if (ret == ErrorInfo::ERR_OK) {
            MemDb::SetRealData(regStart + 29, status, true);
        }
    }
    // 设置加载时间
    if (MemDb::GetRealData(realData, regStart + 30, true)) {
        status = realData.pv.data.iValue;
        ret = rtu.PresetSingleRegister(addr, start+1, status);
        if (ret == ErrorInfo::ERR_OK) {
            MemDb::SetRealData(regStart + 30, status, true);
        }
    }
    return ret;
}
