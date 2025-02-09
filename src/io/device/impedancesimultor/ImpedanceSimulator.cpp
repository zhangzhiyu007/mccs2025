/*
 * ImpedanceSimulator.cpp
 *
 *  Created on: 2013-10-12
 *      Author: liuhaitao
 */

#include "ImpedanceSimulator.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../../util/Util.h"
#include "../../com/Com.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "DCXGImpedanceSimulator.h"
#include "LineImpedanceSimulator.h"

#include "ParwaImpedanceSimulator.h"
ImpedanceSimulator::ImpedanceSimulator() {
    // TODO Auto-generated constructor stub
    m_com       = NULL;
    m_tcpClient = NULL;
}

ImpedanceSimulator::~ImpedanceSimulator() {
    // TODO Auto-generated destructor stub
}

void ImpedanceSimulator::SetCom(Com *com) { m_com = com; }

// 设置TcpClient指针
void ImpedanceSimulator::SetTcp(TcpClient *tcpClient) {
    m_tcpClient = tcpClient;
}

void ImpedanceSimulator::SetIsTcp(bool IsTcp) { m_isTcp = IsTcp; }

int ImpedanceSimulator::Read(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    if (NULL == dev) {
        return ret;
    }

    // 判断设备名称
    // 判断设备厂商和型号来确定具体通信协议
    switch (dev->devManu) {
    case DevManu::Manu_Parwa:
        ret = this->ParwaImSimRead(dev);
    case DevManu::Manu_JINGCE:
        ret = this->DCXGRead(dev);
        break;
	case DevManu::Manu_XVJI_LISP:
		ret = this->LineImSimRead(dev);
		break;
    default:
        break;
    };

    return ret;
}

int ImpedanceSimulator::Preset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    if (NULL == dev) {
        return ret;
    }

    // 判断设备名称
    // 判断设备厂商和型号来确定具体通信协议
    switch (dev->devManu) {
    case DevManu::Manu_Parwa:
        ret = this->ParwaImSimPreset(dev);
        break;
    case DevManu::Manu_JINGCE:
        ret = this->DCXGPreset(dev);
        break;
	case DevManu::Manu_XVJI_LISP:
		ret = this->LineImSimPreset(dev);
		break;
    default:
        break;
    };

    return ret;
}

int ImpedanceSimulator::LineImSimRead(Device::SlaveDev *dev) {
	int ret = ErrorInfo::ERR_OK;
	LineImpedanceSimulator impSim;
	impSim.SetTcpClient(this->m_tcpClient);
	ret = impSim.Read(dev);
	return ret;
}

int ImpedanceSimulator::LineImSimPreset(Device::SlaveDev *dev) {
	int ret = ErrorInfo::ERR_OK;
	LineImpedanceSimulator impSim;
	impSim.SetTcpClient(this->m_tcpClient);
	ret = impSim.Preset(dev);
	return ret;
}

int ImpedanceSimulator::ParwaImSimRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    ParwaImpedanceSimulator impSim;
    impSim.SetTcp(this->m_tcpClient);
    ret = impSim.Read(dev);
    return ret;
}

int ImpedanceSimulator::ParwaImSimPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    ParwaImpedanceSimulator impSim;
    impSim.SetTcp(this->m_tcpClient);
    ret = impSim.Preset(dev);
    return ret;
}

int ImpedanceSimulator::DCXGRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    DCXGImpedanceSimulator DCXG;
    DCXG.SetTcp(this->m_tcpClient);
    DCXG.SetIsTcp(m_isTcp);
    ret = DCXG.Read(dev);

    return ret;
}

int ImpedanceSimulator::DCXGPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    DCXGImpedanceSimulator DCXG;
    DCXG.SetTcp(this->m_tcpClient);
    DCXG.SetIsTcp(m_isTcp);
    ret = DCXG.Preset(dev);
    return ret;
}
