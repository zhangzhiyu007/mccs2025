#include "DCXGImpedanceSimulator.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

DCXGImpedanceSimulator::DCXGImpedanceSimulator(){
    m_com = NULL;
    m_tcpClient = NULL;
    m_isTcp = false;
}

DCXGImpedanceSimulator::~DCXGImpedanceSimulator(){
}

void DCXGImpedanceSimulator::SetCom(Com *com){
    assert(NULL != com);
    m_com = com;
}

void DCXGImpedanceSimulator::SetIsTcp(bool isTcp){
    m_isTcp = isTcp;
}

void DCXGImpedanceSimulator::SetTcp(TcpClient *tcpClient){
    m_tcpClient = tcpClient;
}

int DCXGImpedanceSimulator::Read(Device::SlaveDev *dev){
    int ret = ErrorInfo::ERR_OK;
    bool comm = false;
    int addr = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;

    if (NULL == dev){
        zlog_warn(Util::m_zlog, "从设备地址为NULL");
        return ErrorInfo::ERR_OPENED;
    }

    ModbusRtuMaster rtu;
    rtu.SetIsTcp(m_isTcp);
    rtu.SetTcpClient(this->m_tcpClient);

    UshortArray values;
    values.clear();

    // 读取状态寄存器100
    int start = 0x64; // 寄存器地址100
    int length = 1;

    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ret == ErrorInfo::ERR_OK) {
        comm = true;
        unsigned short data = values[0];
        for (int i = 0; i < 16; ++i) {
            bool tmp = (data >> i) & 0x1;
            MemDb::SetRealData(regStart + i + 1, tmp, false);
        }
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "读取模拟器状态失败，%s", dev->name.c_str());
    }

    MemDb::SetRealData(regStart, comm, false);
    return ret;
}

int DCXGImpedanceSimulator::Preset(Device::SlaveDev *dev){
    int ret = ErrorInfo::ERR_OK;
    int addr = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;

    if(!m_tcpClient->IsOpened())
        return ErrorInfo::ERR_OPENED;

    ModbusRtuMaster rtu;
    rtu.SetIsTcp(m_isTcp);
    rtu.SetTcpClient(m_tcpClient);

    UshortArray values;
    values.clear();
    
    // 写入阻抗寄存器50
    int start = 0x32; // 寄存器地址50
    unsigned short data = 0;
    RealData realData;

    for (int i = 0; i < 16; ++i) { // 假设14个相关的设置
        if (MemDb::GetRealData(realData, regStart + i + 1, true)) {
            if (realData.pv.data.iValue) {
                data |= (1 << i);
            }
        }
    }
    ret = rtu.PresetSingleRegister(addr, start, data);

    return ret;
}
