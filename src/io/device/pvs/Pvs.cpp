/*
 * Pvs.cpp
 *
 *   创建日期: 2013-6-24
 *   作         者: mzh
 */

#include "Pvs.h"

#include "../../../db/MemDb.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "APsystemsPvs.h"
#include "GoodWe.h"
#include "HzConverterPvs.h"
#include "SunGrowPvs.h"

Pvs::Pvs() {
    // TODO 构造函数
    m_com       = NULL;
    m_tcpClient = NULL;
	m_isTcp     = false;
}

Pvs::~Pvs() {
    // TODO 析构函数
}

void Pvs::SetCom(Com *com) { m_com = com; }

void Pvs::SetIsTcp(bool isTcp) { m_isTcp = isTcp; }

// 设置TcpClient指针
void Pvs::SetTcp(TcpClient *tcpClient) {
    assert(NULL != tcpClient);
    m_tcpClient = tcpClient;
}

int Pvs::Read(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    if (NULL == dev) {
        return ret;
    }

    // 判断设备名称
    // 判断设备厂商和型号来确定具体通信协议
    switch (dev->devManu) {
    case DevManu::Manu_SUNGROW: // 合肥阳光
        ret = this->SungrowRead(dev);
        break;
    case DevManu::Manu_GOODWE: // 固德威
        ret = this->goodWeRead(dev);
        break;
    case DevManu::Manu_HUAWEI: // 华为
        ret = this->HuaWeiRead(dev);
        break;
    case DevManu::Manu_HZCONVERTER: // 杭州禾迈
        ret = this->HzConverterRead(dev);
        break;
    case DevManu::Manu_APsytems: // APsystems
        ret = this->APsystemsRead(dev);
        break;
    default:
        break;
    };

    return ret;
}

/*
 *  Preset
 *  功能: 设置光伏逆变器命令
 *  输入：
 *  dev    :Device::SlaveDev* 从设备指针
 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
 */
int Pvs::Preset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    if (NULL == dev) {
        zlog_info(Util::m_zlog, "从设备不存在");
        return ret;
    }

    // 判断设备名称
    // 判断设备厂商和型号来确定具体通信协议
    switch (dev->devManu) {
    case DevManu::Manu_SUNGROW: // 合肥阳光
        ret = this->SungrowPreset(dev);
        break;
    case DevManu::Manu_GOODWE: // 固德威
        ret = this->goodWePreset(dev);
        break;
    case DevManu::Manu_HUAWEI: // 华为
        ret = this->HuaWeiPreset(dev);
        break;
    case DevManu::Manu_HZCONVERTER: // 杭州禾迈
        ret = this->HzConverterPreset(dev);
        break;
    case DevManu::Manu_APsytems: // APsystems
        ret = this->APsystemsPreset(dev);
        break;
    default:
        break;
    };

    return ret;
}

int Pvs::SungrowRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    SunGrowPvs pvs;
    pvs.SetCom(this->m_com);
    ret = pvs.Read(dev);
    return ret;
}

int Pvs::SungrowPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    SunGrowPvs pvs;
    pvs.SetCom(this->m_com);
    ret = pvs.Preset(dev);
    return ret;
}

int Pvs::goodWeRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    GoodWe pvs;
    pvs.SetTcp(this->m_tcpClient);
    ret = pvs.Read(dev);
    return ret;
}

int Pvs::goodWePreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    GoodWe pvs;
    pvs.SetTcp(this->m_tcpClient);
    ret = pvs.Preset(dev);
    return ret;
}

int Pvs::HuaWeiRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    m_pvs.SetCom(this->m_com);
    ret = m_pvs.Read(dev);
    return ret;
}

int Pvs::HuaWeiPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    m_pvs.SetCom(this->m_com);
    ret = m_pvs.Preset(dev);
    return ret;
}

int Pvs::APsystemsRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    APsystemsPvs pvs;
    pvs.SetTcp(this->m_tcpClient);
    ret = pvs.Read(dev);
    return ret;
}

int Pvs::APsystemsPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    APsystemsPvs pvs;
    pvs.SetTcp(this->m_tcpClient);
    ret = pvs.Preset(dev);
    return ret;
}

int Pvs::HzConverterRead(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    HzConverterPvs pvs;
    pvs.SetCom(this->m_com);
    ret = pvs.Read(dev);
    return ret;
}

int Pvs::HzConverterPreset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    HzConverterPvs pvs;
    pvs.SetCom(this->m_com);
    ret = pvs.Preset(dev);
    return ret;
}
