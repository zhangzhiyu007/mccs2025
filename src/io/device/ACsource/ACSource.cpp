/*
 * ACSource.cpp
 *
 *  Created on: 2013-10-11
 *      Author: liuhaitao
 */

#include "ACSource.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../../util/Util.h"
#include "../../com/Com.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "ANGridSIm.h"
#include "ApcACSource.h"

ACSource::ACSource() {
    // TODO Auto-generated constructor stub
    m_com       = NULL;
    m_tcpClient = NULL;
}

ACSource::~ACSource() {
    // TODO Auto-generated destructor stub
}

void ACSource::SetCom(Com *com) {
    assert(NULL != com);
    m_com = com;
}

// 设置TcpClient指针
void ACSource::SetTcp(TcpClient *tcpClient) {
    assert(NULL != tcpClient);
    m_tcpClient = tcpClient;
}

int ACSource::Read(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    if (NULL == dev) {
        return ret;
    }

    // 判断设备名称
    // 判断设备厂商和型号来确定具体通信协议
    switch (dev->devManu) {
    case DevManu::Manu_CHROMA:
        ret = this->ChromaSourceRead(dev);
        break;
    case DevManu::Manu_XVJI:
        ret = this->ANGridSImRead(dev);
        break;
    default:
        break;
    };

    return ret;
}

int ACSource::Preset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    if (NULL == dev) {
        return ret;
    }

    // 判断设备名称
    // 判断设备厂商和型号来确定具体通信协议
    switch (dev->devManu) {
    case DevManu::Manu_CHROMA:
        ret = this->ChromaSourcePreset(dev);
        break;
	case DevManu::Manu_XVJI:
		ret = this->ANGridSImPreset(dev);
		break;
    default:
        break;
    };

    return ret;
}
int ACSource::ANGridSImRead(Device::SlaveDev *dev) {
	int ret = ErrorInfo::ERR_OK;
	ANGridSIm source;
	source.SetTcp(m_tcpClient);
	ret = source.Read(dev);
	return ret;
}

int ACSource:: ANGridSImPreset(Device::SlaveDev *dev) {
	int ret = ErrorInfo::ERR_OK;
	ANGridSIm source;
	source.SetTcp(m_tcpClient);
	ret = source.Preset(dev);
	return ret;
}
int ACSource::ApcACSourceRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    ApcACSource source;
    source.SetCom(this->m_com);
    ret = source.Read(dev);
    return ret;
}

int ACSource::ApcACSourcePreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    ApcACSource source;
    source.SetCom(this->m_com);
    ret = source.Preset(dev);
    return ret;
}

int ACSource::ChromaSourceRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    m_chromaSource.SetTcp(m_tcpClient);
    ret = m_chromaSource.Read(dev);
    return ret;
}

int ACSource::ChromaSourcePreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    m_chromaSource.SetTcp(m_tcpClient);
    ret = m_chromaSource.Preset(dev);
    return ret;
}
