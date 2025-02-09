#include "LineImpedanceSimulator.h"
#include "../../../db/MemDb.h"
#include "../../../util/Util.h"
#include "../../com/Com.h"
#include "../../protocol/modbus/ModbusTcpClient.h"

// 线路阻抗模拟器

LineImpedanceSimulator::LineImpedanceSimulator()
    : m_tcpClient(NULL), m_isTcp(true) {}

LineImpedanceSimulator::~LineImpedanceSimulator() {
    // 资源清理
}

void LineImpedanceSimulator::SetTcpClient(TcpClient *tcpClient) {
    m_tcpClient = tcpClient;
}

int LineImpedanceSimulator::Read(Device::SlaveDev *dev) {
    if (!m_tcpClient || !m_tcpClient->IsOpened()) {
        return ErrorInfo::ERR_OPENED;
    }

    ModbusTcpClient modbus;
    modbus.SetTcpClient(m_tcpClient);

    int addr = atoi(dev->slaveAddr.c_str());
    UshortArray values;

    int ret = modbus.ReadOutputRegisters(values, addr, 0x0064,
                                         16); // 根据实际起始寄存器和数量修改
    //     据返回数据表格说明：
    //  		寄存器	功能说明	数据及状态
    // 工作状态	15	备用	备用	备用
    // 		14	备用	备用	备用
    // 		13	备用	备用	备用
    // 		12	备用	备用	备用
    // 		11	备用	备用	备用
    // 		10	备用	备用	备用
    // 		9	备用	备用	备用
    // 		8	备用	备用	备用
    // 工作状态	7	线缆连接故障	数据1=线缆连接故障数据0=无线缆连接故障
    // 		6	备用	备用	备用
    // 		5	备用	备用	备用
    // 		4	2000米	数据1=2000米	数据0=无
    // 		3	1500米	数据1=1500米	数据0=无
    // 		2	1000米	数据1=1000米	数据0=无
    // 		1	500米	数据1=500米	    数据0=无
    // 		0	0米（直通）	数据1=0米	数据0=无
    // 读取到的数据存储到内存数据库
    if (ret == ErrorInfo::ERR_OK && !values.empty()) {
        int status = values[0];
        MemDb::SetRealData(dev->regStart, status, false);
        status = values[1];
        MemDb::SetRealData(dev->regStart + 1, status, false);
        status = values[2];
        MemDb::SetRealData(dev->regStart + 2, status, false);
        status = values[3];
        MemDb::SetRealData(dev->regStart + 3, status, false);
        status = values[4];
        MemDb::SetRealData(dev->regStart + 4, status, false);
        status = values[5];
        MemDb::SetRealData(dev->regStart + 5, status, false);
        status = values[6];
        MemDb::SetRealData(dev->regStart + 6, status, false);
        status = values[7];
        MemDb::SetRealData(dev->regStart + 7, status, false);
    }
    return ret;
}

int LineImpedanceSimulator::Preset(Device::SlaveDev *dev) {
    if (!m_tcpClient || !m_tcpClient->IsOpened()) {
        return ErrorInfo::ERR_OPENED;
    }
    int ret = ErrorInfo::ERR_OK;

    int start = 0x32;
    int data  = 0;
    UshortArray values;
    bool status = false;
    RealData realData;
    int regStart = dev->regStart;
    ModbusTcpClient modbus;
    modbus.SetTcpClient(m_tcpClient);

    int addr = atoi(dev->slaveAddr.c_str());
    //     功能及寄存器地址	地址（1字节）本机默认5	功能码（1字节）
    //     起始寄存器（2字节）	寄存器位数（2字节）	校验码（2字节）
    // 50.0	0米	05	06	00	32	00	01	E8
    // 41 50.1	500米	05	06	00	32	00	02	A8
    // 40 50.2	1000米	05	06	00	32	00	04	28
    // 42 50.3	1500米	05	06	00	32	00	08	28
    // 47 50.4	2000米	05	06	00	32	00	10	28
    // 4D

    // if( MemDb::GetRealData(realData, regStart + 19, true) ) {
    // 	status  = realData.pv.data.iValue;
    // 	rtu.PresetSingleRegister
    // 	return ret;
    // }
    if (MemDb::GetRealData(realData, regStart + 10, true)) {
        status = realData.pv.data.iValue;
        ret    = modbus.PresetSingleRegister(addr, start, status << 0);
        if (ret == ErrorInfo::ERR_OK) {
            MemDb::SetRealData(regStart + 10, status, true);
        }
    }
    if (MemDb::GetRealData(realData, regStart + 11, true)) {
        status = realData.pv.data.iValue;
        ret    = modbus.PresetSingleRegister(addr, start, status << 1);
        if (ret == ErrorInfo::ERR_OK) {
            MemDb::SetRealData(regStart + 11, status, true);
        }
    }
    if (MemDb::GetRealData(realData, regStart + 12, true)) {
        status = realData.pv.data.iValue;
        ret    = modbus.PresetSingleRegister(addr, start, status << 2);
        if (ret == ErrorInfo::ERR_OK) {
            MemDb::SetRealData(regStart + 12, status, true);
        }
    }
    if (MemDb::GetRealData(realData, regStart + 13, true)) {
        status = realData.pv.data.iValue;
        ret    = modbus.PresetSingleRegister(addr, start, status << 3);
        if (ret == ErrorInfo::ERR_OK) {
            MemDb::SetRealData(regStart + 13, status, true);
        }
    }
    if (MemDb::GetRealData(realData, regStart + 14, true)) {
        status = realData.pv.data.iValue;
        ret    = modbus.PresetSingleRegister(addr, start, status << 4);
        if (ret == ErrorInfo::ERR_OK) {
            MemDb::SetRealData(regStart + 14, status, true);
        }
    }
    if  (MemDb::GetRealData(realData, regStart + 15,true)){
        status = realData.pv.data.iValue;
        ret    = modbus.PresetSingleRegister(addr, start, status << 5);
        if (ret == ErrorInfo::ERR_OK) {
            MemDb::SetRealData(regStart + 15, status, true);
        }
    }
    if  (MemDb::GetRealData(realData, regStart + 16,true)){
        status = realData.pv.data.iValue;
        ret    = modbus.PresetSingleRegister(addr, start, status << 6);
        if (ret == ErrorInfo::ERR_OK) {
            MemDb::SetRealData(regStart + 16, status, true);
        }
    }
    
    return ret;
}