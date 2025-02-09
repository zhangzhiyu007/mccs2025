/*
 * ANGridSIm.cpp
 *
 *  Created on: 2022-12-18
 *      Author: 16843
 */

#include "ANGridSIm.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../com/Com.h"

ANGridSIm::ANGridSIm() {
    // TODO Auto-generated constructor stub
    m_com       = NULL;
    m_tcpClient = NULL;
    m_delayTime = 1000;
    m_retries   = 3;
}

ANGridSIm::~ANGridSIm() {
    // TODO Auto-generated destructor stub
}

void ANGridSIm::SetCom(Com *com) {
    assert(NULL != com);
    m_com = com;
}

// 设置TcpClient指针
void ANGridSIm::SetTcp(TcpClient *tcpClient) { m_tcpClient = tcpClient; }

int ANGridSIm::Read(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    // 判断设备名称
    // 判断设备型号来确定具体通信协议
    switch (dev->devModel) {
    case DevModel::Model_JDHN_ANGridSim: // jdhn-angridsim
        ret = this->JDHNAngridRead(dev); // 秦皇岛配电中心
        break;
    default:
        break;
    };

    return ret;
}

int ANGridSIm::Preset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    // 判断设备名称
    // 判断设备型号来确定具体通信协议
    switch (dev->devModel) {
    case DevModel::Model_JDHN_ANGridSim:   //
        ret = this->JDHNAngridPreset(dev); // jdhn-angridsim
        break;
    default:
        break;
    };

    return ret;
}

int ANGridSIm::JDHNAngridRead(Device::SlaveDev *dev) // jdhn-angridsim
{
    zlog_info(Util::m_zlog, "读取60kW艾诺ANBGS系列电网模拟器数据");
    bool comm = false;
    int ret   = ErrorInfo::ERR_OK;
    UshortArray values;
    values.clear();
    int addr     = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;
    float tmp    = 0.0;
    int tmp1     = 0;

    if (NULL == dev) {
        zlog_warn(Util::m_zlog, "从设备为NULL");
        return ErrorInfo::ERR_NULL;
    }

    // 判断串口是否打开
    if (!m_tcpClient->IsOpened()) {
        return ErrorInfo::ERR_OPENED;
    }

    // 查询类
    // msleep(500);
    //     命令字： A4H
    // 命令作用：查询输出测量值：电压、电流、频率、功率、峰值系数、电流峰值等，非启
    // 动态返回全 0 命令格式： 0x7B XX X 0xF0 0xA4 X 0x7D (8 字节) 命令举例： 7B
    // 00 08 01 F0 A4 9D 7D

    // 应答格式： 0x7B XX X 0xF0 0xA4 X(1 字节输出方式,见注1) XXX XX XXX XXX XXX
    // XX XX XX (3 字节电压单位0.01V;2 字节电流单位0.1A;3 字节频率单位0.001Hz;3
    // 字节  有功功率单位1W，其中最高位为符号为1 表示负数;3
    // 字节视在功率单位1VA;2 字节功  率因数单位0.001;2 字节峰值因数单位0.001;2
    // 字节电流峰值单位0.1A，后面2 块相同）
    // XXX XX XXX XXX XXX XX XX XX XXX XX XXX XXX XXX XX XX XX 0x7D (69 字 节)
    ret = AngridRead(addr, 0xF0, 0xA4, 678, values); // 查询输出值
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        tmp1 = values[0];
        MemDb::SetRealData(regStart, tmp1, false); // 输出方式
        tmp = values[1] * 0.01;
        MemDb::SetRealData(regStart + 1, tmp, false); // A相电压
        tmp = values[2] * 0.1;
        MemDb::SetRealData(regStart + 2, tmp, false); // A相电流
        tmp = values[3] * 0.001;
        MemDb::SetRealData(regStart + 3, tmp, false); // A相频率
        tmp = values[4] * 1;
        MemDb::SetRealData(regStart + 4, tmp, false); // A相有功功率
        tmp = values[5] * 1;
        MemDb::SetRealData(regStart + 5, tmp, false); // A相视在功率
        tmp = values[6] * 0.001;
        MemDb::SetRealData(regStart + 6, tmp, false); // A相功率因数
        tmp = values[7] * 0.001;
        MemDb::SetRealData(regStart + 7, tmp, false); // A相峰值因数
        tmp = values[8] * 0.1;
        MemDb::SetRealData(regStart + 8, tmp, false); // A相电流峰值

        tmp = values[9] * 0.01;
        MemDb::SetRealData(regStart + 9, tmp, false); // B相电压
        tmp = values[10] * 0.1;
        MemDb::SetRealData(regStart + 10, tmp, false); // B相电流
        tmp = values[11] * 0.001;
        MemDb::SetRealData(regStart + 11, tmp, false); // B相频率
        tmp = values[12] * 1;
        MemDb::SetRealData(regStart + 12, tmp, false); // B相有功功率
        tmp = values[13] * 1;
        MemDb::SetRealData(regStart + 13, tmp, false); // B相视在功率
        tmp = values[14] * 0.001;
        MemDb::SetRealData(regStart + 14, tmp, false); // B相功率因数
        tmp = values[15] * 0.001;
        MemDb::SetRealData(regStart + 15, tmp, false); // B相峰值因数
        tmp = values[16] * 0.1;
        MemDb::SetRealData(regStart + 16, tmp, false); // B相电流峰值

        tmp = values[17] * 0.01;
        MemDb::SetRealData(regStart + 17, tmp, false); // C相电压
        tmp = values[18] * 0.1;
        MemDb::SetRealData(regStart + 18, tmp, false); // C相电流
        tmp = values[19] * 0.001;
        MemDb::SetRealData(regStart + 19, tmp, false); // C相频率
        tmp = values[20] * 1;
        MemDb::SetRealData(regStart + 20, tmp, false); // C相有功功率
        tmp = values[21] * 1;
        MemDb::SetRealData(regStart + 21, tmp, false); // C相视在功率
        tmp = values[22] * 0.001;
        MemDb::SetRealData(regStart + 22, tmp, false); // C相功率因数
        tmp = values[23] * 0.001;
        MemDb::SetRealData(regStart + 23, tmp, false); // C相峰值因数
        tmp = values[24] * 0.1;
        MemDb::SetRealData(regStart + 24, tmp, false); // C相电流峰值
    } else {
        zlog_warn(Util::m_zlog, "读取%s输出测量值失败，结果=%d",
                  dev->name.c_str(), ret);
    }
    msleep(100);

    //     命令字： 42H
    // 命令作用： 查询零穿某一相参数
    // 命令格式： 0x7B XX X 0xA5 0x42 X（00 代表U 相，01 代表V 相，02 代表W相）
    // X 0x7D (9 字节) 命令举例： 7B 00 09 01 A5 42 00 F1 7D 应答格式： 0x7B XX
    // X 0xA5 0x42 X（1 字节相数)X（1 字节使能开关， 00 代表关， 01 代表开） XXX
    // XXX XXX（3 字节*3：U1、U2、U3 电压，单位0.01V）XXX XXX XXX （3 字节*3
    // ，T1、T2、T3 时间，单位0.001S）XX  （2 字节触发角度,单位0.1°) XX（2
    // 字节相位角度，单位0.1 °) X 0x7D (32 字节)

    ret = AngridRead(addr, 0xA5, 0x42, 32, values, 00); // 查询零穿U相参数
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        tmp1 = values[0];
        MemDb::SetRealData(regStart + 25, tmp1, false); // 相数
        tmp1 = values[1];
        MemDb::SetRealData(regStart + 26, tmp1, false); // 使能开关
        tmp = values[2] * 0.01;
        MemDb::SetRealData(regStart + 27, tmp, false); // U1电压
        tmp = values[3] * 0.01;
        MemDb::SetRealData(regStart + 28, tmp, false); // U2电压
        tmp = values[4] * 0.01;
        MemDb::SetRealData(regStart + 29, tmp, false); // U3电压
        tmp = values[5] * 0.001;
        MemDb::SetRealData(regStart + 30, tmp, false); // T1时间
        tmp = values[6] * 0.001;
        MemDb::SetRealData(regStart + 31, tmp, false); // T2时间
        tmp = values[7] * 0.001;
        MemDb::SetRealData(regStart + 32, tmp, false); // T3时间
        tmp = (short)values[8] * 0.1;
        MemDb::SetRealData(regStart + 33, tmp, false); // 触发角度
        tmp = values[9] * 0.1;
        MemDb::SetRealData(regStart + 34, tmp, false); // 相位角度
    } else {
        zlog_warn(Util::m_zlog, "读取%s零穿U相参数失败，结果=%d",
                  dev->name.c_str(), ret);
    }
    msleep(100);
    ret = AngridRead(addr, 0xA5, 0x42, 32, values, 01); // 查询零穿V相参数
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        tmp1 = values[0];
        MemDb::SetRealData(regStart + 35, tmp1, false); // 相数
        tmp1 = values[1];
        MemDb::SetRealData(regStart + 36, tmp1, false); // 使能开关
        tmp = values[2] * 0.01;
        MemDb::SetRealData(regStart + 37, tmp, false); // U1电压
        tmp = values[3] * 0.01;
        MemDb::SetRealData(regStart + 38, tmp, false); // U2电压
        tmp = values[4] * 0.01;
        MemDb::SetRealData(regStart + 39, tmp, false); // U3电压
        tmp = values[5] * 0.001;
        MemDb::SetRealData(regStart + 40, tmp, false); // T1时间
        tmp = values[6] * 0.001;
        MemDb::SetRealData(regStart + 41, tmp, false); // T2时间
        tmp = values[7] * 0.001;
        MemDb::SetRealData(regStart + 42, tmp, false); // T3时间
        tmp = values[8] * 0.1;
        MemDb::SetRealData(regStart + 43, tmp, false); // 触发角度
        tmp = values[9] * 0.1;
        MemDb::SetRealData(regStart + 44, tmp, false); // 相位角度
    } else {
        zlog_warn(Util::m_zlog, "读取%s零穿V相参数失败，结果=%d",
                  dev->name.c_str(), ret);
    }
    msleep(100);
    ret = AngridRead(addr, 0xA5, 0x42, 32, values, 02); // 查询零穿W相参数
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        tmp1 = values[0];
        MemDb::SetRealData(regStart + 45, tmp1, false); // 相数
        tmp1 = values[1];
        MemDb::SetRealData(regStart + 46, tmp1, false); // 使能开关
        tmp = values[2] * 0.01;
        MemDb::SetRealData(regStart + 47, tmp, false); // U1电压
        tmp = values[3] * 0.01;
        MemDb::SetRealData(regStart + 48, tmp, false); // U2电压
        tmp = values[4] * 0.01;
        MemDb::SetRealData(regStart + 49, tmp, false); // U3电压
        tmp = values[5] * 0.001;
        MemDb::SetRealData(regStart + 50, tmp, false); // T1时间
        tmp = values[6] * 0.001;
        MemDb::SetRealData(regStart + 51, tmp, false); // T2时间
        tmp = values[7] * 0.001;
        MemDb::SetRealData(regStart + 52, tmp, false); // T3时间
        tmp = values[8] * 0.1;
        MemDb::SetRealData(regStart + 53, tmp, false); // 触发角度
        tmp = values[9] * 0.1;
        MemDb::SetRealData(regStart + 54, tmp, false); // 相位角度
    } else {
        zlog_warn(Util::m_zlog, "读取%s零穿W相参数失败，结果=%d",
                  dev->name.c_str(), ret);
    }
    msleep(100);
    // 命令字： 43H
    // 命令作用： 查询低穿某一相参数
    // 命令格式： 0x7B XX X 0xA5 0x43 X（00 代表U 相，01 代表V 相，02 代表W相）
    // X 0x7D (9 字节) 命令举例： 7B 00 09 01 A5 43 00 F2 7D 应答格式： 0x7B XX
    // X 0xA5 0x43 X（1 字节相数)X（1 字节使能开关， 00 代表关， 01  代表开）
    // XXX XXX（3 字节*2：U1 、U2、电压， 单位0.01V）XXX XXX  （3 字节*2，

    // T1 、T2 时间，单位0.001S）XX  （2 字节触发角度，单位0.1°) XX（2
    // 字节相位角度， 单位0.1°) X 0x7D (26 字节)
    ret = AngridRead(addr, 0xA5, 0x43, 26, values, 00); // 查询低穿U相参数
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        tmp1 = values[0];
        MemDb::SetRealData(regStart + 55, tmp1, false); // 相数
        tmp1 = values[1];
        MemDb::SetRealData(regStart + 56, tmp1, false); // 使能开关
        tmp = values[2] * 0.01;
        MemDb::SetRealData(regStart + 57, tmp, false); // U1电压
        tmp = values[3] * 0.01;
        MemDb::SetRealData(regStart + 58, tmp, false); // U2电压
        tmp = values[4] * 0.001;
        MemDb::SetRealData(regStart + 59, tmp, false); // T1时间
        tmp = values[5] * 0.001;
        MemDb::SetRealData(regStart + 60, tmp, false); // T2时间
        tmp = values[6] * 0.1;
        MemDb::SetRealData(regStart + 61, tmp, false); // 触发角度
        tmp = values[7] * 0.1;
        MemDb::SetRealData(regStart + 62, tmp, false); // 相位角度
    } else {
        zlog_warn(Util::m_zlog, "读取%s低穿U相参数失败，结果=%d",
                  dev->name.c_str(), ret);
    }
    msleep(100);
    ret = AngridRead(addr, 0xA5, 0x43, 26, values, 01); // 查询低穿V相参数
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        tmp1 = values[0];
        MemDb::SetRealData(regStart + 63, tmp1, false); // 相数
        tmp1 = values[1];
        MemDb::SetRealData(regStart + 64, tmp1, false); // 使能开关
        tmp = values[2] * 0.01;
        MemDb::SetRealData(regStart + 65, tmp, false); // U1电压
        tmp = values[3] * 0.01;
        MemDb::SetRealData(regStart + 66, tmp, false); // U2电压
        tmp = values[4] * 0.001;
        MemDb::SetRealData(regStart + 67, tmp, false); // T1时间
        tmp = values[5] * 0.001;
        MemDb::SetRealData(regStart + 68, tmp, false); // T2时间
        tmp = values[6] * 0.1;
        MemDb::SetRealData(regStart + 69, tmp, false); // 触发角度
        tmp = values[7] * 0.1;
        MemDb::SetRealData(regStart + 70, tmp, false); // 相位角度
    } else {
        zlog_warn(Util::m_zlog, "读取%s低穿V相参数失败，结果=%d",
                  dev->name.c_str(), ret);
    }
    msleep(100);
    ret = AngridRead(addr, 0xA5, 0x43, 26, values, 02); // 查询低穿W相参数
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        tmp1 = values[0];
        MemDb::SetRealData(regStart + 71, tmp1, false); // 相数
        tmp1 = values[1];
        MemDb::SetRealData(regStart + 72, tmp1, false); // 使能开关
        tmp = values[2] * 0.01;
        MemDb::SetRealData(regStart + 73, tmp, false); // U1电压
        tmp = values[3] * 0.01;
        MemDb::SetRealData(regStart + 74, tmp, false); // U2电压
        tmp = values[4] * 0.001;
        MemDb::SetRealData(regStart + 75, tmp, false); // T1时间
        tmp = values[5] * 0.001;
        MemDb::SetRealData(regStart + 76, tmp, false); // T2时间
        tmp = values[6] * 0.1;
        MemDb::SetRealData(regStart + 77, tmp, false); // 触发角度
        tmp = values[7] * 0.1;
        MemDb::SetRealData(regStart + 78, tmp, false); // 相位角度
    } else {
        zlog_warn(Util::m_zlog, "读取%s低穿W相参数失败，结果=%d",
                  dev->name.c_str(), ret);
    }
    msleep(100);
    // 命令字： 44H
    // 命令作用： 查询高压穿越某一相参数
    // 命令格式： 0x7B XX X 0xA5 0x44 X（00 代表U 相，01 代表V 相，02 代表W相）
    // X 0x7D (9 字节) 命令举例： 7B 00 09 01 A5 44 00 F3 7D 应答格式： 0x7B XX
    // X 0xA5 0x44 X（1 字节相数)X（1 字节使能开关， 0 代表关， 1 代 表开） XXX
    // XXX XXX（3 字节*3：U1 、U2 、U3 电压，单位0.01V）XXX XXX  （3  字节*2
    // ，T1 、T2 时间，单位0.001S）XX  （2 字节触发角度， 单位0.1°) XX（2 字节相
    // 位角度，单位0.1°) X 0x7D (29 字节)
    ret = AngridRead(addr, 0xA5, 0x44, 29, values, 00); // 查询高低穿组合U相参数
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        tmp1 = values[0];
        MemDb::SetRealData(regStart + 79, tmp1, false); // 相数
        tmp1 = values[1];
        MemDb::SetRealData(regStart + 80, tmp1, false); // 使能开关
        tmp = values[2] * 0.01;
        MemDb::SetRealData(regStart + 81, tmp, false); // U1电压
        tmp = values[3] * 0.01;
        MemDb::SetRealData(regStart + 82, tmp, false); // U2电压
        tmp = values[4] * 0.01;
        MemDb::SetRealData(regStart + 83, tmp, false); // U3电压
        tmp = values[5] * 0.001;
        MemDb::SetRealData(regStart + 84, tmp, false); // T1时间
        tmp = values[6] * 0.001;
        MemDb::SetRealData(regStart + 85, tmp, false); // T2时间
        tmp = values[7] * 0.1;
        MemDb::SetRealData(regStart + 86, tmp, false); // 触发角度
        tmp = values[8] * 0.1;
        MemDb::SetRealData(regStart + 87, tmp, false); // 相位角度
    } else {
        zlog_warn(Util::m_zlog, "读取%s高低穿组合U相参数失败，结果=%d",
                  dev->name.c_str(), ret);
    }
    msleep(100);
    ret = AngridRead(addr, 0xA5, 0x44, 29, values, 01); // 查询高低穿组合V相参数
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        tmp1 = values[0];
        MemDb::SetRealData(regStart + 88, tmp1, false); // 相数
        tmp1 = values[1];
        MemDb::SetRealData(regStart + 89, tmp1, false); // 使能开关
        tmp = values[2] * 0.01;
        MemDb::SetRealData(regStart + 90, tmp, false); // U1电压
        tmp = values[3] * 0.01;
        MemDb::SetRealData(regStart + 91, tmp, false); // U2电压
        tmp = values[4] * 0.01;
        MemDb::SetRealData(regStart + 92, tmp, false); // U3电压
        tmp = values[5] * 0.001;
        MemDb::SetRealData(regStart + 93, tmp, false); // T1时间
        tmp = values[6] * 0.001;
        MemDb::SetRealData(regStart + 94, tmp, false); // T2时间
        tmp = values[7] * 0.1;
        MemDb::SetRealData(regStart + 95, tmp, false); // 触发角度
        tmp = values[8] * 0.1;
        MemDb::SetRealData(regStart + 96, tmp, false); // 相位角度
    } else {
        zlog_warn(Util::m_zlog, "读取%s高低穿组合V相参数失败，结果=%d",
                  dev->name.c_str(), ret);
    }
    msleep(100);
    ret = AngridRead(addr, 0xA5, 0x44, 29, values, 02); // 查询高低穿组合W相参数
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        tmp1 = values[0];
        MemDb::SetRealData(regStart + 97, tmp1, false); // 相数
        tmp1 = values[1];
        MemDb::SetRealData(regStart + 98, tmp1, false); // 使能开关
        tmp = values[2] * 0.01;
        MemDb::SetRealData(regStart + 99, tmp, false); // U1电压
        tmp = values[3] * 0.01;
        MemDb::SetRealData(regStart + 100, tmp, false); // U2电压
        tmp = values[4] * 0.01;
        MemDb::SetRealData(regStart + 101, tmp, false); // U3电压
        tmp = values[5] * 0.001;
        MemDb::SetRealData(regStart + 102, tmp, false); // T1时间
        tmp = values[6] * 0.001;
        MemDb::SetRealData(regStart + 103, tmp, false); // T2时间
        tmp = values[7] * 0.1;
        MemDb::SetRealData(regStart + 104, tmp, false); // 触发角度
        tmp = values[8] * 0.1;
        MemDb::SetRealData(regStart + 105, tmp, false); // 相位角度
    } else {
        zlog_warn(Util::m_zlog, "读取%s高低穿组合W相参数失败，结果=%d",
                  dev->name.c_str(), ret);
    }
    msleep(100);
    // 命令字： 45H
    // 命令作用： 查询高低穿组合某一相参数
    // 命令格式： 0x7B XX X 0xA5 0x45 X（0 代表U 相，1 代表V 相，2 代表W相） X
    // 0x7D (9 字节) 命令举例： 7B 00 09 01 A5 45 00 F4 7D 应答格式： 0x7B XX X
    // 0xA5 0x45 X（1 字节相数)X（1 字节使能开关， 00 代表关， 01  代表开） XXX
    // XXX XXX（3 字节*3：U1 、U2 、U3 电压，单位0.01V）XXX XXX  （3  字节*2
    // ，T1 、T2 时间，单位0.001S)  XX  （2 字节触发角度，单位0.1 度）XX（2 字节
    // 相位角度，单位0.1 度）X 0x7D (29 字节)
    ret = AngridRead(addr, 0xA5, 0x45, 29, values, 0); // 查询高低穿组合U相参数
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        tmp1 = values[0];
        MemDb::SetRealData(regStart + 106, tmp1, false); // 相数
        tmp1 = values[1];
        MemDb::SetRealData(regStart + 107, tmp1, false); // 使能开关
        tmp = values[2] * 0.01;
        MemDb::SetRealData(regStart + 108, tmp, false); // U1电压
        tmp = values[3] * 0.01;
        MemDb::SetRealData(regStart + 109, tmp, false); // U2电压
        tmp = values[4] * 0.01;
        MemDb::SetRealData(regStart + 110, tmp, false); // U3电压
        tmp = values[5] * 0.001;
        MemDb::SetRealData(regStart + 111, tmp, false); // T1时间
        tmp = values[6] * 0.001;
        MemDb::SetRealData(regStart + 112, tmp, false); // T2时间
        tmp = values[7] * 0.1;
        MemDb::SetRealData(regStart + 113, tmp, false); // 触发角度
        tmp = values[8] * 0.1;
        MemDb::SetRealData(regStart + 114, tmp, false); // 相位角度
    } else {
        zlog_warn(Util::m_zlog, "读取%s高低穿组合U相参数失败，结果=%d",
                  dev->name.c_str(), ret);
    }
    msleep(100);
    ret = AngridRead(addr, 0xA5, 0x45, 29, values, 1); // 查询高低穿组合V相参数
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        tmp1 = values[0];
        MemDb::SetRealData(regStart + 115, tmp1, false); // 相数
        tmp1 = values[1];
        MemDb::SetRealData(regStart + 116, tmp1, false); // 使能开关
        tmp = values[2] * 0.01;
        MemDb::SetRealData(regStart + 117, tmp, false); // U1电压
        tmp = values[3] * 0.01;
        MemDb::SetRealData(regStart + 118, tmp, false); // U2电压
        tmp = values[4] * 0.01;
        MemDb::SetRealData(regStart + 119, tmp, false); // U3电压
        tmp = values[5] * 0.001;
        MemDb::SetRealData(regStart + 120, tmp, false); // T1时间
        tmp = values[6] * 0.001;
        MemDb::SetRealData(regStart + 121, tmp, false); // T2时间
        tmp = values[7] * 0.1;
        MemDb::SetRealData(regStart + 122, tmp, false); // 触发角度
        tmp = values[8] * 0.1;
        MemDb::SetRealData(regStart + 123, tmp, false); // 相位角度
    } else {
        zlog_warn(Util::m_zlog, "读取%s高低穿组合V相参数失败，结果=%d",
                  dev->name.c_str(), ret);
    }
    msleep(100);
    ret = AngridRead(addr, 0xA5, 0x45, 29, values, 2); // 查询高低穿组合W相参数
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        tmp1 = values[0];
        MemDb::SetRealData(regStart + 124, tmp1, false); // 相数
        tmp1 = values[1];
        MemDb::SetRealData(regStart + 125, tmp1, false); // 使能开关
        tmp = values[2] * 0.01;
        MemDb::SetRealData(regStart + 126, tmp, false); // U1电压
        tmp = values[3] * 0.01;
        MemDb::SetRealData(regStart + 127, tmp, false); // U2电压
        tmp = values[4] * 0.01;
        MemDb::SetRealData(regStart + 128, tmp, false); // U3电压
        tmp = values[5] * 0.001;
        MemDb::SetRealData(regStart + 129, tmp, false); // T1时间
        tmp = values[6] * 0.001;
        MemDb::SetRealData(regStart + 130, tmp, false); // T2时间
        tmp = values[7] * 0.1;
        MemDb::SetRealData(regStart + 131, tmp, false); // 触发角度
        tmp = values[8] * 0.1;
        MemDb::SetRealData(regStart + 132, tmp, false); // 相位角度
    } else {
        zlog_warn(Util::m_zlog, "读取%s高低穿组合W相参数失败，结果=%d",
                  dev->name.c_str(), ret);
    }
    msleep(100);
    //     命令字： 47H
    // 命令作用： 查询不平衡模式 1 参数
    // 命令格式： 0x7B XX X 0xA5 0x47 X 0x7D (8 字节)
    // 命令举例： 7B 00 08 01 A5 47 F5 7D
    // 应答格式： 0x7B XX X 0xA5 0x47 X X（2 字节触发角度， 单位0.1°) XXX XX (U
    // 相参 数， 3 字节电压，单位0.01V，2 字节角度，单位0.1°, 后同） XXXXX（V
    // 相参数） XXX XX（W相参数） X 0x7D (25 字节)
    ret = AngridRead(addr, 0xA5, 0x47, 25, values); // 查询不平衡模式 1 参数
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        tmp  = values[0] * 0.1;
        MemDb::SetRealData(regStart + 133, tmp, false); // 触发角度
        tmp = values[1] * 0.01;
        MemDb::SetRealData(regStart + 134, tmp, false); // U相电压
        tmp = values[2] * 0.1;
        MemDb::SetRealData(regStart + 135, tmp, false); // U相角度
        tmp = values[3] * 0.01;
        MemDb::SetRealData(regStart + 136, tmp, false); // V相电压
        tmp = values[4] * 0.1;
        MemDb::SetRealData(regStart + 137, tmp, false); // V相角度
        tmp = values[5] * 0.01;
        MemDb::SetRealData(regStart + 138, tmp, false); // W相电压
        tmp = values[6] * 0.1;
        MemDb::SetRealData(regStart + 139, tmp, false); // W相角度
    } else {
        zlog_warn(Util::m_zlog, "读取%s不平衡模式 1 参数失败，结果=%d",
                  dev->name.c_str(), ret);
    }
    msleep(100);
    // 命令字： 34H
    // 命令作用：查询谐波主界面参数
    // 命令格式： 0x7B XX X 0xA5 0x34X(1 字节组数， 00-40) X 0x7D (9 字节)
    // 命令举例： 7B 00 08 01 A5 34 E2 7D
    // 应答格式： 0x7B XX X 0xA5 0x34 X(1 字节组数:00-49) XX(2 字节触发角度，
    // 单位°)XXX （1 字节U 相使能开关、1 字节V
    // 相使能开关、1字节W相使能开关单位） X 0x7D (14 字节)
    ret = AngridRead(addr, 0xA5, 0x34, 14, values); // 查询谐波主界面参数
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        tmp1 = values[0];
        MemDb::SetRealData(regStart + 140, tmp1, false); // 组数
        tmp = values[1] * 0.1;
        MemDb::SetRealData(regStart + 141, tmp, false); // 触发角度
        tmp1 = values[2];
        MemDb::SetRealData(regStart + 142, tmp1, false); // U相使能开关
        tmp1 = values[3];
        MemDb::SetRealData(regStart + 143, tmp1, false); // V相使能开关
        tmp1 = values[4];
        MemDb::SetRealData(regStart + 144, tmp1, false); // W相使能开关
    } else {
        zlog_warn(Util::m_zlog, "读取%s谐波主界面参数失败，结果=%d",
                  dev->name.c_str(), ret);
    }
    msleep(100);
    // 命令字： 31H
    // 命令作用：查询某组某次谐波参数
    // 命令格式： 0x7B XX X 0xA5 0x31 X（1 字节组数， 00-49）X(1 字节次数，
    // 02-50) X 0x7D (10 字节) 命令举例： 7B 00 0A 01 A5 31 00 02 E3 7D
    // 应答格式： 0x7B XX X 0xA5 0x31 X(1 字节组数:00-49)X(1 字节次数:02-50)
    // XX(2 字节百 分比0.1)XX（2 字节角度，单位0.1°) X（1 字节使能开关） X 0x7D
    // (15 字节)
    ret = AngridRead(addr, 0xA5, 0x31, 15, values, 00,
                     02); // 查询某组某次谐波参数
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        tmp1 = values[0];
        MemDb::SetRealData(regStart + 145, tmp1, false); // 组数
        tmp1 = values[1];
        MemDb::SetRealData(regStart + 146, tmp1, false); // 次数
        tmp = values[2] * 0.1;
        MemDb::SetRealData(regStart + 147, tmp, false); // 百分比
        tmp = values[3] * 0.1;
        MemDb::SetRealData(regStart + 148, tmp, false); // 角度
        tmp1 = values[4];
        MemDb::SetRealData(regStart + 149, tmp1, false); // 使能开关
    } else {
        zlog_warn(Util::m_zlog, "读取%s某组某次谐波参数失败，结果=%d",
                  dev->name.c_str(), ret);
    }
    msleep(100);
    //     命令字： EBH
    // 命令作用：查询仪表状态/报警代码
    // 命令格式： 0x7B XX X 0xF0 0xEB X 0x7D (8 字节)
    // 命令举例： 7B 00 08 01 F0 EB E4 7D
    // 应答格式： 0x7B XX X 0xF0 0xEB X XX (1
    // 字节状态+2字节数据信息，详细见表6-4） X 0x7D (11 字节)
    ret = AngridRead(addr, 0xF0, 0xEB, 11, values); // 查询仪表状态/报警代码
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        tmp1 = values[0];
        MemDb::SetRealData(regStart + 150, tmp1, false); // 状态
        tmp1 = values[1];
        MemDb::SetRealData(regStart + 151, tmp1, false); // 数据信息
    } else {
        zlog_warn(Util::m_zlog, "读取%s仪表状态/报警代码失败，结果=%d",
                  dev->name.c_str(), ret);
    }
    msleep(100);
    //     命令字： 41H
    // 命令作用： 查询常规参数
    // 命令格式： 0x7B XX X 0xA5 0x41 X 0x7D (8 字节)
    // 命令举例： 7B 00 08 01 A5 41 EF 7D
    // 应答格式： 0x7B XX X 0xA5 0x41 XX (2 字节缓启动，单位0.1 秒) XXX  （3
    // 字节频率， 单位0.001Hz）XXX XX (U 相参数： 3 字节U 相电压单位0.01V、2
    // 字节角度单位0.1°,  后同） XXX XX(V 相参数)XXX XX(W相参数)X 0x7D (28 字节)
    ret = AngridRead(addr, 0xA5, 0x41, 28, values); // 查询常规参数
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        tmp  = values[0] * 0.1;
        MemDb::SetRealData(regStart + 152, tmp, false); // 缓启动
        tmp = values[1] * 0.001;
        MemDb::SetRealData(regStart + 153, tmp, false); // 频率
        tmp = values[2] * 0.01;
        MemDb::SetRealData(regStart + 154, tmp, false); // U相电压
        tmp = values[3] * 0.1;
        MemDb::SetRealData(regStart + 155, tmp, false); // U相角度
        tmp = values[4] * 0.01;
        MemDb::SetRealData(regStart + 156, tmp, false); // V相电压
        tmp = (short)values[5] * 0.1;
        MemDb::SetRealData(regStart + 157, tmp, false); // V相角度
        tmp = values[6] * 0.01;
        MemDb::SetRealData(regStart + 158, tmp, false); // W相电压
        tmp = (short)values[7] * 0.1;
        MemDb::SetRealData(regStart + 159, tmp, false); // W相角度
    } else {
        zlog_warn(Util::m_zlog, "读取%s常规参数失败，结果=%d",
                  dev->name.c_str(), ret);
    }

    MemDb::SetRealData(regStart + 0, comm, false);
    return ret;
}

int ANGridSIm::AngridRead(int addr, int type, int data, short length,
                          UshortArray &values, int bit9) {
    String query; // 查询数组
    String reply; // 响应数组
    int error = ErrorInfo::ERR_OK;
    unsigned short crc16;
    int byte      = 0;
    int i         = 0;
    int regLength = 9;

    // 封装数据帧
    query.clear();
    query.push_back(0x7B);             // 起始帧
    query.push_back(regLength >> 8);   // 总字节数高字节
    query.push_back(regLength & 0xFF); // 总字节数低字节
    query.push_back(addr);             // 从机地址
    query.push_back(type);             // 命令类型
    query.push_back(data);             // 命令字
    query.push_back(bit9);             // 9位

    crc16 = Check::CheckSum(query);
    query.push_back(crc16); // 校验位
    query.push_back(0x7D);  // 结束帧

    zlog_info(Util::m_zlog, "查询命令");

    values.clear();
    values.resize(length - 8);

    // printf("下发数据:");
    // for(int i = 0; i < query.size(); i++)
    // {
    // 	printf("%02x ", query[i]);
    // }
    // printf("\n");

    error = m_tcpClient->TxRxMsg(reply, query, length);

    // printf("上传数据:");
    // for(int i = 0; i < reply.size(); i++)
    // {
    // 	printf("%02x ", reply[i]);
    // }
    // printf("\n");

    if (error == ErrorInfo::ERR_OK) {
        if ((reply[0] != 0x7B)) {
            zlog_warn(Util::m_zlog, "反馈数据错误");
            return ErrorInfo::ERR_INV_RESP;
        }

        if (reply[4] == 0x99) {
            // 错误码
            // 0x01 校验和错误
            // 0x02 命令类型错误
            // 0x03 命令字错误
            // 0x04 状态不符（当前的状态不允许执行接收的命令）
            // 0x05 参数错误
            // 0x06 保护报警
            // 0x07 超量程范围
            // 0x08 命令长度错误
            zlog_warn(Util::m_zlog, "查询命令错误，错误码：%d", reply[6]);
            error = ErrorInfo::ERR_INV_RESP;
        } else {
            if (reply.size() == 9) {
                values[0] = reply[6];
            } else if (reply.size() == 11) {
                values[0] = (reply[6] << 16) | (reply[7] << 8) | (reply[8]);
            }
            // 命令字： 42H
            // 命令作用： 查询零穿某一相参数
            // 命令格式： 0x7B XX X 0xA5 0x42 X（00 代表U 相，01 代表V 相，02
            // 代表W相） X 0x7D (9 字节) 命令举例： 7B 00 09 01 A5 42 00 F1 7D
            // 应答格式： 0x7B XX X 0xA5 0x42 X（1 字节相数)X（1 字节使能开关，
            // 00 代表关， 01 代表开） XXX XXX XXX（3 字节*3：U1、U2、U3
            // 电压，单位0.01V）XXX XXX XXX （3 字节*3 ，T1、T2、T3
            // 时间，单位0.001S）XX  （2 字节触发角度,单位0.1°) XX（2
            // 字节相位角度，单位0.1 °) X 0x7D (32 字节)
            else if (reply[5] == 0x42) {
                values[0] = reply[6]; // 相数
                values[1] = reply[7]; // 使能开关
                values[2] =
                    (reply[8] << 16) | (reply[9] << 8) | reply[10]; // U1电压
                values[3] =
                    (reply[11] << 16) | (reply[12] << 8) | reply[13]; // U2电压
                values[4] =
                    (reply[14] << 16) | (reply[15] << 8) | reply[16]; // U3电压
                values[5] =
                    (reply[17] << 16) | (reply[18] << 8) | reply[19]; // T1时间
                values[6] =
                    (reply[20] << 16) | (reply[21] << 8) | reply[22]; // T2时间
                values[7] =
                    (reply[23] << 16) | (reply[24] << 8) | reply[25]; // T3时间
                values[8] = (reply[26] << 8) | reply[27]; // 触发角度
                values[9] = (reply[28] << 8) | reply[29]; // 相位角度
            }
            // 命令字： 43H
            // 命令作用： 查询低穿某一相参数
            // 命令格式： 0x7B XX X 0xA5 0x43 X（00 代表U 相，01 代表V 相，02
            // 代表W相） X 0x7D (9 字节) 命令举例： 7B 00 09 01 A5 43 00 F2 7D
            // 应答格式： 0x7B XX X 0xA5 0x43 X（1 字节相数)X（1 字节使能开关，
            // 00 代表关， 01  代表开） XXX XXX（3 字节*2：U1 、U2、电压，
            // 单位0.01V）XXX XXX  （3 字节*2， T1 、T2 时间，单位0.001S）XX （2
            // 字节触发角度，单位0.1°) XX（2 字节相位角度， 单位0.1°) X 0x7D (26
            // 字节)
            else if (reply[5] == 0x43) {
                values[0] = reply[6]; // 相数
                values[1] = reply[7]; // 使能开关
                values[2] =
                    (reply[8] << 16) | (reply[9] << 8) | reply[10]; // U1电压
                values[3] =
                    (reply[11] << 16) | (reply[12] << 8) | reply[13]; // U2电压
                values[4] =
                    (reply[14] << 16) | (reply[15] << 8) | reply[16]; // T1时间
                values[5] =
                    (reply[17] << 16) | (reply[18] << 8) | reply[19]; // T2时间
                values[6] = (reply[20] << 8) | reply[21]; // 触发角度
                values[7] = (reply[22] << 8) | reply[23]; // 相位角度
            }
            // 命令字： 44H
            // 命令作用： 查询高压穿越某一相参数
            // 命令格式： 0x7B XX X 0xA5 0x44 X（00 代表U 相，01 代表V 相，02
            // 代表W相） X 0x7D (9 字节) 命令举例： 7B 00 09 01 A5 44 00 F3 7D
            // 应答格式： 0x7B XX X 0xA5 0x44 X（1 字节相数)X（1 字节使能开关，
            // 0 代表关， 1 代 表开） XXX XXX XXX（3 字节*3：U1 、U2 、U3
            // 电压，单位0.01V）XXX XXX  （3  字节*2 ，T1 、T2
            // 时间，单位0.001S）XX  （2 字节触发角度， 单位0.1°) XX（2 字节相
            // 位角度，单位0.1°) X 0x7D (29 字节)
            else if (reply[5] == 0x44) {
                values[0] = reply[6]; // 相数
                values[1] = reply[7]; // 使能开关
                values[2] =
                    (reply[8] << 16) | (reply[9] << 8) | reply[10]; // U1电压
                values[3] =
                    (reply[11] << 16) | (reply[12] << 8) | reply[13]; // U2电压
                values[4] =
                    (reply[14] << 16) | (reply[15] << 8) | reply[16]; // U3电压
                values[5] =
                    (reply[17] << 16) | (reply[18] << 8) | reply[19]; // T1时间
                values[6] =
                    (reply[20] << 16) | (reply[21] << 8) | reply[22]; // T2时间
                values[7] = (reply[23] << 8) | reply[24]; // 触发角度
                values[8] = (reply[25] << 8) | reply[26]; // 相位角度
            }
            // 命令字： 45H
            // 命令作用： 查询高低穿组合某一相参数
            // 命令格式： 0x7B XX X 0xA5 0x45 X（0 代表U 相，1 代表V 相，2
            // 代表W相） X 0x7D (9 字节) 命令举例： 7B 00 09 01 A5 45 00 F4 7D
            // 应答格式： 0x7B XX X 0xA5 0x45 X（1 字节相数)X（1 字节使能开关，
            // 00 代表关， 01  代表开） XXX XXX XXX（3 字节*3：U1 、U2 、U3
            // 电压，单位0.01V）XXX XXX  （3  字节*2 ，T1 、T2 时间，单位0.001S)
            // XX  （2 字节触发角度，单位0.1 度）XX（2 字节 相位角度，单位0.1
            // 度）X 0x7D (29 字节)
            else if (reply[5] == 0x45) {
                values[0] = reply[6]; // 相数
                values[1] = reply[7]; // 使能开关
                values[2] =
                    (reply[8] << 16) | (reply[9] << 8) | reply[10]; // U1电压
                values[3] =
                    (reply[11] << 16) | (reply[12] << 8) | reply[13]; // U2电压
                values[4] =
                    (reply[14] << 16) | (reply[15] << 8) | reply[16]; // U3电压
                values[5] =
                    (reply[17] << 16) | (reply[18] << 8) | reply[19]; // T1时间
                values[6] =
                    (reply[20] << 16) | (reply[21] << 8) | reply[22]; // T2时间
                values[7] = (reply[23] << 8) | reply[24]; // 触发角度
                values[8] = (reply[25] << 8) | reply[26]; // 相位角度
            }

            //             命令字： 34H
            // 命令作用：查询谐波主界面参数
            // 命令格式： 0x7B XX X 0xA5 0x34X(1 字节组数， 00-40) X 0x7D (9
            // 字节) 命令举例： 7B 00 08 01 A5 34 E2 7D 应答格式： 0x7B XX X
            // 0xA5 0x34 X(1 字节组数:00-49) XX(2 字节触发角度， 单位°)XXX （1
            // 字节U 相使能开关、1 字节V 相使能开关、1 字节W相使能开关单位） X
            // 0x7D (14 字节)
            else if (reply[5] == 0x34) {

                values[0] = reply[6];                   // 组数
                values[1] = (reply[7] << 8) | reply[8]; // 触发角度
                values[2] = reply[9];                   // U相使能开关
                values[3] = reply[10];                  // V相使能开关
                values[4] = reply[11];                  // W相使能开关
            }
            //             命令字： 31H
            // 命令作用：查询某组某次谐波参数
            // 命令格式： 0x7B XX X 0xA5 0x31 X（1 字节组数， 00-49）X(1
            // 字节次数， 02-50) X 0x7D (10 字节) 命令举例： 7B 00 0A 01 A5 31
            // 00 02 E3 7D 应答格式： 0x7B XX X 0xA5 0x31 X(1 字节组数:00-49)X(1
            // 字节次数:02-50) XX(2 字节百 分比0.1)XX（2 字节角度，单位0.1°)
            // X（1 字节使能开关） X 0x7D (15 字节)
            else if (reply[5] == 0x31) {
                values[0] = reply[6];                     // 组数
                values[1] = reply[7];                     // 次数
                values[2] = (reply[8] << 8) | reply[9];   // 百分比
                values[3] = (reply[10] << 8) | reply[11]; // 角度
                values[4] = reply[12];                    // 使能开关
            }

            else {
                for (i = 6; i < length - 2; i += 2) {
                    values[byte] = (reply[i] << 8) | (reply[i + 1]);

                    if (i == 12 || i == 30 || i == 48) {
                        for (int j = 0; j < 2; j++) {
                            values[byte] = (reply[i] << 16) |
                                           (reply[i + 1] << 8) | (reply[i + 2]);
                            i = i + 3;
                        }
                        i = i - 2;
                    }

                    byte++;
                }
            }
        }
    }

    return error;
}

int ANGridSIm::AngridRead(int addr, int type, int data, short length,
                          UshortArray &values, int bit9, int bit10) {
    String query; // 查询数组
    String reply; // 响应数组
    int error = ErrorInfo::ERR_OK;
    unsigned short crc16;
    int byte      = 0;
    int i         = 0;
    int regLength = 10;

    // 封装数据帧
    query.clear();
    query.push_back(0x7B);             // 起始帧
    query.push_back(regLength >> 8);   // 总字节数高字节
    query.push_back(regLength & 0xFF); // 总字节数低字节
    query.push_back(addr);             // 从机地址
    query.push_back(type);             // 命令类型
    query.push_back(data);             // 命令字
    query.push_back(bit9);             // 9位
    query.push_back(bit10);            // 10位

    crc16 = Check::CheckSum(query);
    query.push_back(crc16); // 校验位
    query.push_back(0x7D);  // 结束帧

    zlog_info(Util::m_zlog, "查询命令");

    values.clear();            // 清空数组
    values.resize(length - 8); // 重新分配数组大小

    // printf("下发数据:");
    // for(int i = 0; i < query.size(); i++)
    // {
    // 	printf("%02x ", query[i]);
    // }
    // printf("\n");

    error = m_tcpClient->TxRxMsg(reply, query, length);

    // printf("上传数据:");
    // for(int i = 0; i < reply.size(); i++)
    // {
    // 	printf("%02x ", reply[i]);
    // }
    // printf("\n");

    if (error == ErrorInfo::ERR_OK) {
        if ((reply[0] != 0x7B)) {
            zlog_warn(Util::m_zlog, "反馈数据错误");
            return ErrorInfo::ERR_INV_RESP;
        }

        if (reply[4] == 0x99) {
            // 错误码
            // 0x01 校验和错误
            // 0x02 命令类型错误
            // 0x03 命令字错误
            // 0x04 状态不符（当前的状态不允许执行接收的命令）
            // 0x05 参数错误
            // 0x06 保护报警
            // 0x07 超量程范围
            // 0x08 命令长度错误
            zlog_warn(Util::m_zlog, "查询命令错误，错误码：%d", reply[6]);
            error = ErrorInfo::ERR_INV_RESP;
        } else {
            if (reply.size() == 9) {
                values[0] = reply[6];
            } else if (reply.size() == 11) {
                values[0] = (reply[6] << 16) | (reply[7] << 8) | (reply[8]);
            }

            //             命令字： 31H
            // 命令作用：查询某组某次谐波参数
            // 命令格式： 0x7B XX X 0xA5 0x31 X（1 字节组数， 00-49）X(1
            // 字节次数， 02-50) X 0x7D (10 字节) 命令举例： 7B 00 0A 01 A5 31
            // 00 02 E3 7D 应答格式： 0x7B XX X 0xA5 0x31 X(1 字节组数:00-49)X(1
            // 字节次数:02-50) XX(2 字节百 分比0.1)XX（2 字节角度，单位0.1°)
            // X（1 字节使能开关） X 0x7D (15 字节)
            else if (reply[5] == 0x31) {
                values[0] = reply[6];                     // 组数
                values[1] = reply[7];                     // 次数
                values[2] = (reply[8] << 8) | reply[9];   // 百分比
                values[3] = (reply[10] << 8) | reply[11]; // 角度
                values[4] = reply[12];                    // 使能开关
            }

            else {
                for (i = 6; i < length - 2; i += 2) {
                    values[byte] = (reply[i] << 8) | (reply[i + 1]);

                    if (i == 12 || i == 30 || i == 48) {
                        for (int j = 0; j < 2; j++) {
                            values[byte] = (reply[i] << 16) |
                                           (reply[i + 1] << 8) | (reply[i + 2]);
                            i = i + 3;
                        }
                        i = i - 2;
                    }

                    byte++;
                }
            }
        }
    }

    return error;
}

int ANGridSIm::AngridRead(int addr, int type, int data, short length,
                          UshortArray &values) {
    String query; // 查询数组
    String reply; // 响应数组
    int error = ErrorInfo::ERR_OK;
    unsigned short crc16;
    int byte      = 0;
    int i         = 0;
    int regLength = 8;

    // 封装数据帧
    query.clear();
    query.push_back(0x7B);             // 起始帧
    query.push_back(regLength >> 8);   // 总字节数高字节
    query.push_back(regLength & 0xFF); // 总字节数低字节
    query.push_back(addr);             // 从机地址
    query.push_back(type);             // 命令类型
    query.push_back(data);             // 命令字

    crc16 = Check::CheckSum(query);
    query.push_back(crc16); // 校验位
    query.push_back(0x7D);  // 结束帧

    zlog_info(Util::m_zlog, "查询命令");

    values.clear();
    values.resize(length - 8);

    // printf("下发数据:");
    // for(int i = 0; i < query.size(); i++)
    // {
    // 	printf("%02x ", query[i]);
    // }
    // printf("\n");

    error = m_tcpClient->TxRxMsg(reply, query, length);

    // printf("上传数据:");
    // for(int i = 0; i < reply.size(); i++)
    // {
    // 	printf("%02x ", reply[i]);
    // }
    // printf("\n");

    if (error == ErrorInfo::ERR_OK) {
        if ((reply[0] != 0x7B)) {
            zlog_warn(Util::m_zlog, "反馈数据错误");
            return ErrorInfo::ERR_INV_RESP;
        }

        if (reply[4] == 0x99) {
            // 错误码
            // 0x01 校验和错误
            // 0x02 命令类型错误
            // 0x03 命令字错误
            // 0x04 状态不符（当前的状态不允许执行接收的命令）
            // 0x05 参数错误
            // 0x06 保护报警
            // 0x07 超量程范围
            // 0x08 命令长度错误
            zlog_warn(Util::m_zlog, "查询命令错误，错误码：%d", reply[6]);
            error = ErrorInfo::ERR_INV_RESP;
        } else {
            if (reply.size() == 9) {
                values[0] = reply[6];
            } else if (reply.size() == 11) {
                values[0] = (reply[6] << 16) | (reply[7] << 8) | (reply[8]);
            }

            // 命令字： A4H
            //  命令作用：查询输出测量值：电压、电流、频率、功率、峰值系数、电流峰值等，非启
            //  动态返回全 0 命令格式： 0x7B XX X 0xF0 0xA4 X 0x7D (8 字节)
            //  命令举例： 7B 00 08 01 F0 A4 9D 7D
            //  应答格式： 0x7B XX X 0xF0 0xA4 X(1 字节输出方式,见注1) XXX XX
            //  XXX XXX XXX    XX XX XX (3 字节电压单位0.01V;2
            //  字节电流单位0.1A;3 字节频率单位0.001Hz;3 字节
            //  有功功率单位1W，其中最高位为符号为1 表示负数;3
            //  字节视在功率单位1VA;2 字节功  率因数单位0.001;2
            //  字节峰值因数单位0.001;2 字节电流峰值单位0.1A，后面2 块相同）
            //  XXX XX XXX XXX XXX XX XX XX XXX XX XXX XXX XXX XX XX XX 0x7D (69
            //  字 节)
            else if (reply[5] == 0xA4) {
                values[0] = reply[6]; // 输出方式
                values[1] =
                    (reply[7] << 16) | (reply[8] << 8) | (reply[9]); // A相电压
                values[2] = (reply[10] << 8) | (reply[11]); // A相电流
                values[3] = (reply[12] << 16) | (reply[13] << 8) |
                            (reply[14]); // A相频率
                values[4] = (reply[15] << 16) | (reply[16] << 8) |
                            (reply[17]); // A相有功功率
                values[5] = (reply[18] << 16) | (reply[19] << 8) |
                            (reply[20]);                    // A相视在功率
                values[6] = (reply[21] << 8) | (reply[22]); // A相功率因数
                values[7] = (reply[23] << 8) | (reply[24]); // A相峰值因数
                values[8] = (reply[25] << 8) | (reply[26]); // A相电流峰值

                values[9] = (reply[27] << 16) | (reply[28] << 8) |
                            (reply[29]);                     // B相电压
                values[10] = (reply[30] << 8) | (reply[31]); // B相电流
                values[11] = (reply[32] << 16) | (reply[33] << 8) |
                             (reply[34]); // B相频率
                values[12] = (reply[35] << 16) | (reply[36] << 8) |
                             (reply[37]); // B相有功功率
                values[13] = (reply[38] << 16) | (reply[39] << 8) |
                             (reply[40]);                    // B相视在功率
                values[14] = (reply[41] << 8) | (reply[42]); // B相功率因数
                values[15] = (reply[43] << 8) | (reply[44]); // B相峰值因数
                values[16] = (reply[45] << 8) | (reply[46]); // B相电流峰值

                values[17] = (reply[47] << 16) | (reply[48] << 8) |
                             (reply[49]);                    // C相电压
                values[18] = (reply[50] << 8) | (reply[51]); // C相电流
                values[19] = (reply[52] << 16) | (reply[53] << 8) |
                             (reply[54]); // C相频率
                values[20] = (reply[55] << 16) | (reply[56] << 8) |
                             (reply[57]); // C相有功功率
                values[21] = (reply[58] << 16) | (reply[59] << 8) |
                             (reply[60]);                    // C相视在功率
                values[22] = (reply[61] << 8) | (reply[62]); // C相功率因数
                values[23] = (reply[63] << 8) | (reply[64]); // C相峰值因数
                values[24] = (reply[65] << 8) | (reply[66]); // C相电流峰值

            }
            //             命令字： 47H
            // 命令作用： 查询不平衡模式 1 参数
            // 命令格式： 0x7B XX X 0xA5 0x47 X 0x7D (8 字节)
            // 命令举例： 7B 00 08 01 A5 47 F5 7D
            // 应答格式： 0x7B XX X 0xA5 0x47 X X（2 字节触发角度， 单位0.1°)
            // XXX XX (U 相参 数， 3 字节电压，单位0.01V，2 字节角度，单位0.1°,
            // 后同） XXXXX（V 相参数） XXX XX（W相参数） X 0x7D (25 字节)
            else if (reply[5] == 0x47) {
                values[0] = (reply[6] << 8) | reply[7]; // 触发角度
                values[1] =
                    (reply[8] << 16) | (reply[9] << 8) | reply[10]; // U相电压
                values[2] = (reply[11] << 8) | reply[12]; // U相角度
                values[3] =
                    (reply[13] << 16) | (reply[14] << 8) | reply[15]; // V相电压
                values[4] = (reply[16] << 8) | reply[17]; // V相角度
                values[5] =
                    (reply[18] << 16) | (reply[19] << 8) | reply[20]; // W相电压
                values[6] = (reply[21] << 8) | reply[22]; // W相角度
            }
            //     命令字： EBH
            // 命令作用：查询仪表状态/报警代码
            // 命令格式： 0x7B XX X 0xF0 0xEB X 0x7D (8 字节)
            // 命令举例： 7B 00 08 01 F0 EB E4 7D
            // 应答格式： 0x7B XX X 0xF0 0xEB X XX (1字节状态+2字节数据信息，详细见表6-4） X 0x7D (11 字节)
            else if (reply[5] == 0xEB) {
                values[0] = reply[6]; // 状态
                values[1] = (reply[7] << 8) | reply[8]; // 数据信息
            }
            //     命令字： 41H
            // 命令作用： 查询常规参数
            // 命令格式： 0x7B XX X 0xA5 0x41 X 0x7D (8 字节)
            // 命令举例： 7B 00 08 01 A5 41 EF 7D
            // 应答格式： 0x7B XX X 0xA5 0x41 XX (2 字节缓启动，单位0.1 秒) XXX  （3字节频率， 单位0.001Hz）XXX XX (U 相参数： 3 字节U 相电压单位0.01V、2字节角度单位0.1°,  后同） XXX XX(V 相参数)XXX XX(W相参数)X 0x7D (28 字节)
            else if (reply[5] == 0x41) {
                values[0] = (reply[6] << 8) | reply[7]; // 缓启动
                values[1] =
                    (reply[8] << 16) | (reply[9] << 8) | reply[10]; // 频率
                values[2] = (reply[11] << 16) | (reply[12] << 8) |
                            reply[13]; // U相电压
                values[3] = (reply[14] << 8) | reply[15]; // U相角度
                values[4] = (reply[16] << 16) | (reply[17] << 8) |
                            reply[18]; // V相电压
                values[5] = (reply[19] << 8) | reply[20]; // V相角度
                values[6] = (reply[21] << 16) | (reply[22] << 8) |
                            reply[23]; // W相电压
                values[7] = (reply[24] << 8) | reply[25]; // W相角度
            }
             else {
                for (i = 6; i < length - 2; i += 2) {
                    values[byte] = (reply[i] << 8) | (reply[i + 1]);

                    if (i == 12 || i == 30 || i == 48) {
                        for (int j = 0; j < 2; j++) {
                            values[byte] = (reply[i] << 16) |
                                           (reply[i + 1] << 8) | (reply[i + 2]);
                            i = i + 3;
                        }
                        i = i - 2;
                    }

                    byte++;
                }
            }
        }
    }

    return error;
}

int ANGridSIm::JDHNAngridPreset(Device::SlaveDev *dev) {
    zlog_info(Util::m_zlog, "设置60kW艾诺ANBGS系列电网模拟器数据");
    int ret = ErrorInfo::ERR_OK;
    UshortArray values;
    values.clear();
    int addr = atoi(dev->slaveAddr.c_str());
    RealData realData;
    RealData voc;
    RealData isc;
    RealData vmp;
    RealData imp;
    int data     = 0;
    int temp     = 0;
    int regStart = dev->regStart;

    // 判断串口是否打开
    if (!m_tcpClient->IsOpened()) {
        return ErrorInfo::ERR_OPENED;
    }
    // 命令字： 00H
    // 命令作用：停止
    // 命令格式： 0x7B XX X 0x0F 0x00 X 0x7D (8 字节)
    // 命令举例： 7B 00 08 01 0F 00 18 7D
    // 命令返回： 7B 00 09 01 0F 00 00  19
    // 7D（控制类和设置类命令返回格式相同，后面不再 举例） 命令字： FFH
    // 命令作用：启动
    // 命令格式： 0x7B XX X 0x0F 0xFF X 0x7D (8 字节)
    // 命令举例： 7B 00 08 01 0F FF 17 7D
    // 命令字： 01H
    // 命令作用：停止触发
    // 命令格式： 0x7B XX X 0x0F 0x01 X 0x7D (8 字节)
    // 命令举例： 7B 00 08 01 0F 01 19 7D

    // 判断是否有启停命令
    if (MemDb::GetRealDataWrited(realData, regStart + 170, true)) {
        if (realData.pv.type == RealData::INT) {
            values.clear();
            if (realData.pv.data.iValue == 1) // 启动
            {
                ret = this->AngridPreset(addr, 0x0F, 0xFF, 8, values);
            } else if (realData.pv.data.iValue == 0) // 停止
            {
                ret = this->AngridPreset(addr, 0x0F, 0x00, 8, values);
            } else if (realData.pv.data.iValue == 2) // 解除报警
            {
                ret = this->AngridPreset(addr, 0x0F, 0x03, 8, values);
            }

        } else if (realData.pv.type == RealData::DB) {
            zlog_info(Util::m_zlog, "启停命令错误：%f",
                      realData.pv.data.dbValue);
            ret = ErrorInfo::ERR_WRITE;
        }
    }

    // 命令字： 12H
    // 命令作用： 设置输出模式为高低穿的某个模式，非执行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x12    X（1 字节模式选择,0 为零穿模式， 1 为低穿模式， 2 为高穿模式，3 为高低穿模式）X 0x7D (9 字节)
    // 命令举例： 7B 00 09 01 5A 12 00 76 7D（设置为零穿待机态）
    if (MemDb::GetRealDataWrited(realData, regStart + 200, true)) {
        if (realData.pv.type == RealData::INT) {
            values.clear();
            if (realData.pv.data.iValue == 0) // 零穿模式
            {
                ret = this->AngridPreset(addr, 0x5A, 0x12, 9, values);
            } else if (realData.pv.data.iValue == 1) // 低穿模式
            {
                ret = this->AngridPreset(addr, 0x5A, 0x12, 9, values);
            } else if (realData.pv.data.iValue == 2) // 高穿模式
            {
                ret = this->AngridPreset(addr, 0x5A, 0x12, 9, values);
            } else if (realData.pv.data.iValue == 3) // 高低穿模式
            {
                ret = this->AngridPreset(addr, 0x5A, 0x12, 9, values);
            }
        } else if (realData.pv.type == RealData::DB) {
            zlog_info(Util::m_zlog, "输出模式命令错误：%f",
                    realData.pv.data.dbValue);
            ret = ErrorInfo::ERR_WRITE;
        }
    }
    // 命令字： 14H
    // 命令作用： 设置模式为不平衡，非执行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x14 X 0x7D (8 字节)
    // 命令举例： 7B 00 08 01 5A 14 77 7D
    if (MemDb::GetRealDataWrited(realData, regStart + 201, true)) {
        if (realData.pv.type == RealData::INT) {
            values.clear();
            if (realData.pv.data.iValue == 1) // 不平衡模式
            {
                ret = this->AngridPreset(addr, 0x5A, 0x14, 8, values);
            }
        } else if (realData.pv.type == RealData::DB) {
            zlog_info(Util::m_zlog, "不平衡模式命令错误：%f",
                    realData.pv.data.dbValue);
            ret = ErrorInfo::ERR_WRITE;
        }
    }

    // 命令字： 18H
    // 命令作用： 设置模式为谐波， 非执行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x18 X 0x7D (8 字节)
    // 命令举例： 7B 00 08 01 5A 18 7B 7D
    if (MemDb::GetRealDataWrited(realData, regStart + 202, true)) {
        if (realData.pv.type == RealData::INT) {
            values.clear();
            if (realData.pv.data.iValue == 1) // 谐波模式
            {
                ret = this->AngridPreset(addr, 0x5A, 0x18, 8, values);
            }
        } else if (realData.pv.type == RealData::DB) {
            zlog_info(Util::m_zlog, "谐波模式命令错误：%f",
                    realData.pv.data.dbValue);
            ret = ErrorInfo::ERR_WRITE;
        }
    }



    // 命令字： 41H
    // 命令作用： 设置常规参数，非运行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x41 XX (2 字节缓启动，单位0.1S)XXX（3 字节频率，单位0.001Hz） XXX  （U 相参数： 3 字节电压， 单位0.01V、2 字节角度， 单位0.1°, 后 同）XXX XX (V 相参数） XXX XX (W相参数) X 0x7D (28 字节)
    // 命令举例：7B 00 1C 01 5A 41 00 00 00 C3 50 00 55 F0 00 00 00 55 F0 09 60 00 55 F0 04 B0 B7 7D
    // (0 秒缓升、 50Hz、U 相： 220V 、0 °  V 相： 220V 、240 °W 相： 220V 、120°)
    //读取203-210并存入数组
    RealData ang41H[8];
    int ang41Htemp = 0;
    for (int i = 0; i < 8; i++) {
        if (MemDb::GetRealDataWrited(realData, regStart + 203 + i, true)) {
            ang41H[i] = realData;
            temp++;
        }
    }
    if (ang41Htemp != 0) {    
        values.clear();
        // 缓启动时间
        values.push_back(ang41H[0].pv.data.iValue); // 假设这里是2字节整数

        // 频率
        int frequency = ang41H[1].pv.data.iValue; // 假设这里是计算好的频率值，需要3字节
        values.push_back((frequency >> 16) & 0xFF);
        values.push_back((frequency >> 8) & 0xFF);
        values.push_back(frequency & 0xFF);

        // U 相电压和角度
        int u_voltage = ang41H[2].pv.data.iValue; // 假设单位已经是0.01V
        int u_angle = ang41H[3].pv.data.iValue;   // 假设单位已经是0.1°
        values.push_back((u_voltage >> 16) & 0xFF);
        values.push_back((u_voltage >> 8) & 0xFF);
        values.push_back(u_voltage & 0xFF);
        values.push_back((u_angle >> 8) & 0xFF);
        values.push_back(u_angle & 0xFF);

        // V 相电压和角度
        int v_voltage = ang41H[4].pv.data.iValue; // 假设单位已经是0.01V
        int v_angle = ang41H[5].pv.data.iValue;   // 假设单位已经是0.1°
        values.push_back((v_voltage >> 16) & 0xFF);
        values.push_back((v_voltage >> 8) & 0xFF);
        values.push_back(v_voltage & 0xFF);
        values.push_back((v_angle >> 8) & 0xFF);
        values.push_back(v_angle & 0xFF);

        // W 相电压和角度
        int w_voltage = ang41H[6].pv.data.iValue; // 假设单位已经是0.01V
        int w_angle = ang41H[7].pv.data.iValue;   // 假设单位已经是0.1°
        values.push_back((w_voltage >> 16) & 0xFF);
        values.push_back((w_voltage >> 8) & 0xFF);
        values.push_back(w_voltage & 0xFF);
        values.push_back((w_angle >> 8) & 0xFF);
        values.push_back(w_angle & 0xFF);

        ret = this->AngridPreset(addr, 0x5A, 0x41, 28, values);
        ang41Htemp = 0;
    }
   
    // 命令字： 42H
    // 命令作用： 设置高低穿模式：零压穿越其中一项参数，非执行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x42 X（1 字节UVW相设置相选择， 0 代表U 相，1 代表V 相，2 代表W相) X（1 字节使能开关）XX（2 字节触发角度，单位0.1°)XXX XXX XXX （3 字节*3：U1 、U2 、U3 电压，单位0.01V）XXX XXX XXX（3 字节*3：T1 、T2 、T3 时间设置，单位0.001S）XX（2 字节角度，单位0.1°) X 0x7D (32 字节)
    // 命令举例： 7B 00 20 01 5A 42 00 01 00 00 00 00 00 00 11 30 00 4D 58 00 00 96 00 01 DB 00 05 5F 00 00 7A 7D
    // (设置 U 相参数、使能开、触发相位：0 °、U1：0V、U2：44V、U3：198V 、T1：0.15S、 T2：0.475S 、T3：1.375S、角度 0 °)
    //读取211-220并存入数组
    //读取221-230并存入数组
    //读取231-240并存入数组
    RealData ang42H[10];
    RealData ang42H_v[10];
    RealData ang42H_w[10];
    int ang42Htemp = 0;
    int ang42Htemp_v = 0;
    int ang42Htemp_w = 0;


    for (int i = 0; i < 10; i++) {
        if (MemDb::GetRealDataWrited(realData, regStart + 211 + i, true)) {
            ang42H[i] = realData;
            temp++;
        }
    }
    for (int i = 0; i < 10; i++) {
        if (MemDb::GetRealDataWrited(realData, regStart + 221 + i, true)) {
            ang42H_v[i] = realData;
            temp++;
        }
    }
    for (int i = 0; i < 10; i++) {
        if (MemDb::GetRealDataWrited(realData, regStart + 231 + i, true)) {
            ang42H_w[i] = realData;
            temp++;
        }
    }
    if (ang42Htemp != 0) {
        values.clear();
        // U 相设置相选择
        values.push_back(ang42H[0].pv.data.iValue); // 假设这里是1字节整数

        // 使能开关
        values.push_back(ang42H[1].pv.data.iValue); // 假设这里是1字节整数

        // 触发角度
        values.push_back(ang42H[2].pv.data.iValue); // 假设这里是2字节整数

        // U1 、U2 、U3 电压
        int u1_voltage = ang42H[3].pv.data.iValue; // 假设单位已经是0.01V
        int u2_voltage = ang42H[4].pv.data.iValue; // 假设单位已经是0.01V
        int u3_voltage = ang42H[5].pv.data.iValue; // 假设单位已经是0.01V
        values.push_back((u1_voltage >> 16) & 0xFF);
        values.push_back((u1_voltage >> 8) & 0xFF);
        values.push_back(u1_voltage & 0xFF);
        values.push_back((u2_voltage >> 16) & 0xFF);
        values.push_back((u2_voltage >> 8) & 0xFF);
        values.push_back(u2_voltage & 0xFF);
        values.push_back((u3_voltage >> 16) & 0xFF);
        values.push_back((u3_voltage >> 8) & 0xFF);
        values.push_back(u3_voltage & 0xFF);

        // T1 、T2 、T3 时间设置
        int t1_time = ang42H[6].pv.data.iValue; // 假设单位已经是0.001S
        int t2_time = ang42H[7].pv.data.iValue; // 假设单位已经是0.001S
        int t3_time = ang42H[8].pv.data.iValue; // 假设单位已经是0.001S
        values.push_back((t1_time >> 16) & 0xFF);
        values.push_back((t1_time >> 8) & 0xFF);
        values.push_back(t1_time & 0xFF);
        values.push_back((t2_time >> 16) & 0xFF);
        values.push_back((t2_time >> 8) & 0xFF);
        values.push_back(t2_time & 0xFF);
        values.push_back((t3_time >> 16) & 0xFF);
        values.push_back((t3_time >> 8) & 0xFF);
        values.push_back(t3_time & 0xFF);

        // 角度
        values.push_back(ang42H[9].pv.data.iValue); // 假设这里是2字节整数

        ret = this->AngridPreset(addr, 0x5A, 0x42, 32, values);
        ang42Htemp = 0;
    }
    if (ang42Htemp_v != 0) {
        values.clear();
        // V 相设置相选择
        values.push_back(ang42H_v[0].pv.data.iValue); // 假设这里是1字节整数

        // 使能开关
        values.push_back(ang42H_v[1].pv.data.iValue); // 假设这里是1字节整数

        // 触发角度
        values.push_back(ang42H_v[2].pv.data.iValue); // 假设这里是2字节整数

        // U1 、U2 、U3 电压
        int u1_voltage = ang42H_v[3].pv.data.iValue; // 假设单位已经是0.01V
        int u2_voltage = ang42H_v[4].pv.data.iValue; // 假设单位已经是0.01V
        int u3_voltage = ang42H_v[5].pv.data.iValue; // 假设单位已经是0.01V
        values.push_back((u1_voltage >> 16) & 0xFF);
        values.push_back((u1_voltage >> 8) & 0xFF);
        values.push_back(u1_voltage & 0xFF);
        values.push_back((u2_voltage >> 16) & 0xFF);
        values.push_back((u2_voltage >> 8) & 0xFF);
        values.push_back(u2_voltage & 0xFF);
        values.push_back((u3_voltage >> 16) & 0xFF);
        values.push_back((u3_voltage >> 8) & 0xFF);
        values.push_back(u3_voltage & 0xFF);

        // T1 、T2 、T3 时间设置
        int t1_time = ang42H_v[6].pv.data.iValue; // 假设单位已经是0.001S
        int t2_time = ang42H_v[7].pv.data.iValue; // 假设单位已经是0.001S
        int t3_time = ang42H_v[8].pv.data.iValue; // 假设单位已经是0.001S
        values.push_back((t1_time >> 16) & 0xFF);
        values.push_back((t1_time >> 8) & 0xFF);
        values.push_back(t1_time & 0xFF);
        values.push_back((t2_time >> 16) & 0xFF);
        values.push_back((t2_time >> 8) & 0xFF);
        values.push_back(t2_time & 0xFF);
        values.push_back((t3_time >> 16) & 0xFF);
        values.push_back((t3_time >> 8) & 0xFF);
        values.push_back(t3_time & 0xFF);

        // 角度
        values.push_back(ang42H_v[9].pv.data.iValue); // 假设这里是2字节整数

        ret = this->AngridPreset(addr, 0x5A, 0x42, 32, values);
        ang42Htemp_v = 0;

    }
    if (ang42Htemp_w != 0) {
        values.clear();
        // W 相设置相选择
        values.push_back(ang42H_w[0].pv.data.iValue); // 假设这里是1字节整数

        // 使能开关
        values.push_back(ang42H_w[1].pv.data.iValue); // 假设这里是1字节整数

        // 触发角度
        values.push_back(ang42H_w[2].pv.data.iValue); // 假设这里是2字节整数

        // U1 、U2 、U3 电压
        int u1_voltage = ang42H_w[3].pv.data.iValue; // 假设单位已经是0.01V
        int u2_voltage = ang42H_w[4].pv.data.iValue; // 假设单位已经是0.01V
        int u3_voltage = ang42H_w[5].pv.data.iValue; // 假设单位已经是0.01V
        values.push_back((u1_voltage >> 16) & 0xFF);
        values.push_back((u1_voltage >> 8) & 0xFF);
        values.push_back(u1_voltage & 0xFF);
        values.push_back((u2_voltage >> 16) & 0xFF);
        values.push_back((u2_voltage >> 8) & 0xFF);
        values.push_back(u2_voltage & 0xFF);
        values.push_back((u3_voltage >> 16) & 0xFF);
        values.push_back((u3_voltage >> 8) & 0xFF);
        values.push_back(u3_voltage & 0xFF);

        // T1 、T2 、T3 时间设置
        int t1_time = ang42H_w[6].pv.data.iValue; // 假设单位已经是0.001S
        int t2_time = ang42H_w[7].pv.data.iValue; // 假设单位已经是0.001S
        int t3_time = ang42H_w[8].pv.data.iValue; // 假设单位已经是0.001S
        values.push_back((t1_time >> 16) & 0xFF);
        values.push_back((t1_time >> 8) & 0xFF);
        values.push_back(t1_time & 0xFF);
        values.push_back((t2_time >> 16) & 0xFF);
        values.push_back((t2_time >> 8) & 0xFF);
        values.push_back(t2_time & 0xFF);
        values.push_back((t3_time >> 16) & 0xFF);
        values.push_back((t3_time >> 8) & 0xFF);
        values.push_back(t3_time & 0xFF);

        // 角度
        values.push_back(ang42H_w[9].pv.data.iValue); // 假设这里是2字节整数
        ret = this->AngridPreset(addr, 0x5A, 0x42, 32, values);
        ang42Htemp_w = 0;
    }


    // 命令字： 43H
    // 命令作用： 设置高低穿模式：低压穿越其中一项参数，非执行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x43 X（1 字节UVW相设置相选择， 0 代表U 相，1 代表V  相，2 代表W相) X（1 字节使能开关， 0 代表关， 1 代表开） XX（2 字节触发角度，单 位0.1°) XXX XXX（3 字节*2：U1 、U2 电压，单位0.01V）XXX XXX（3 字节*2：T1、 T2 时间设置，单位0.001S）XX（2 字节角度，单位0.1°) X 0x7D (26 字节)
    // 命令举例： 7B 00 1A 01 5A 43 00 01 00 00 00 11 30 00 4D 58 00 02 71 00 05 5F 00 00 76 7D  (设置 U 相参数、使能开、触发相位： 0°、U1：44V、U2：198V、T1：0.625S、T2：1.375S、 角度 0 °)
    //读取241-248并存入数组
    //读取249-256并存入数组
    //读取257-264并存入数组
    RealData ang43H[8];
    RealData ang43H_v[8];
    RealData ang43H_w[8];
    int ang43Htemp = 0;
    int ang43Htemp_v = 0;
    int ang43Htemp_w = 0;

    for (int i = 0; i < 8; i++) {
        if (MemDb::GetRealDataWrited(realData, regStart + 241 + i, true)) {
            ang43H[i] = realData;
            temp++;
        }
    }
    for (int i = 0; i < 8; i++) {
        if (MemDb::GetRealDataWrited(realData, regStart + 249 + i, true)) {
            ang43H_v[i] = realData;
            temp++;
        }
    }
    for (int i = 0; i < 8; i++) {
        if (MemDb::GetRealDataWrited(realData, regStart + 257 + i, true)) {
            ang43H_w[i] = realData;
            temp++;
        }
    }
    if (ang43Htemp != 0) {
        values.clear();
        // U 相设置相选择
        values.push_back(ang43H[0].pv.data.iValue); // 假设这里是1字节整数

        // 使能开关
        values.push_back(ang43H[1].pv.data.iValue); // 假设这里是1字节整数

        // 触发角度
        values.push_back(ang43H[2].pv.data.iValue); // 假设这里是2字节整数

        // U1 、U2 电压
        int u1_voltage = ang43H[3].pv.data.iValue; // 假设单位已经是0.01V
        int u2_voltage = ang43H[4].pv.data.iValue; // 假设单位已经是0.01V
        values.push_back((u1_voltage >> 16) & 0xFF);
        values.push_back((u1_voltage >> 8) & 0xFF);
        values.push_back(u1_voltage & 0xFF);
        values.push_back((u2_voltage >> 16) & 0xFF);
        values.push_back((u2_voltage >> 8) & 0xFF);
        values.push_back(u2_voltage & 0xFF);

        // T1 、T2 时间设置
        int t1_time = ang43H[5].pv.data.iValue; // 假设单位已经是0.001S
        int t2_time = ang43H[6].pv.data.iValue; // 假设单位已经是0.001S
        values.push_back((t1_time >> 16) & 0xFF);
        values.push_back((t1_time >> 8) & 0xFF);
        values.push_back(t1_time & 0xFF);
        values.push_back((t2_time >> 16) & 0xFF);
        values.push_back((t2_time >> 8) & 0xFF);
        values.push_back(t2_time & 0xFF);

        // 角度
        values.push_back(ang43H[7].pv.data.iValue); // 假设这里是2字节整数

        ret = this
            ->AngridPreset(addr, 0x5A, 0x43, 26, values);
        ang43Htemp = 0;
    }
    if (ang43Htemp_v != 0) {
        values.clear();
        // V 相设置相选择
        values.push_back(ang43H_v[0].pv.data.iValue); // 假设这里是1字节整数

        // 使能开关
        values.push_back(ang43H_v[1].pv.data.iValue); // 假设这里是1字节整数

        // 触发角度
        values.push_back(ang43H_v[2].pv.data.iValue); // 假设这里是2字节整数

        // U1 、U2 电压
        int u1_voltage = ang43H_v[3].pv.data.iValue; // 假设单位已经是0.01V
        int u2_voltage = ang43H_v[4].pv.data.iValue; // 假设单位已经是0.01V
        values.push_back((u1_voltage >> 16) & 0xFF);
        values.push_back((u1_voltage >> 8) & 0xFF);
        values.push_back(u1_voltage & 0xFF);
        values.push_back((u2_voltage >> 16) & 0xFF);
        values.push_back((u2_voltage >> 8) & 0xFF);
        values.push_back(u2_voltage & 0xFF);

        // T1 、T2 时间设置
        int t1_time = ang43H_v[5].pv.data.iValue; // 假设单位已经是0.001S
        int t2_time = ang43H_v[6].pv.data.iValue; // 假设单位已经是0.001S
        values.push_back((t1_time >> 16) & 0xFF);
        values.push_back((t1_time >> 8) & 0xFF);
        values.push_back(t1_time & 0xFF);
        values.push_back((t2_time >> 16) & 0xFF);
        values.push_back((t2_time >> 8) & 0xFF);
        values.push_back(t2_time & 0xFF);

        // 角度
        values.push_back(ang43H_v[7].pv.data.iValue); // 假设这里是2

        ret = this->AngridPreset(addr, 0x5A, 0x43, 26, values);
        ang43Htemp_v = 0;
    }
    if (ang43Htemp_w != 0) {
        values.clear();
        // W 相设置相选择
        values.push_back(ang43H_w[0].pv.data.iValue); // 假设这里是1字节整数

        // 使能开关
        values.push_back(ang43H_w[1].pv.data.iValue); // 假设这里是1字节整数

        // 触发角度
        values.push_back(ang43H_w[2].pv.data.iValue); // 假设这里是2字节整数

        // U1 、U2 电压
        int u1_voltage = ang43H_w[3].pv.data.iValue; // 假设单位已经是0.01V
        int u2_voltage = ang43H_w[4].pv.data.iValue; // 假设单位已经是0.01V
        values.push_back((u1_voltage >> 16) & 0xFF);
        values.push_back((u1_voltage >> 8) & 0xFF);
        values.push_back(u1_voltage & 0xFF);
        values.push_back((u2_voltage >> 16) & 0xFF);
        values.push_back((u2_voltage >> 8) & 0xFF);
        values.push_back(u2_voltage & 0xFF);

        // T1 、T2 时间设置
        int t1_time = ang43H_w[5].pv.data.iValue; // 假设单位已经是0.001S
        int t2_time = ang43H_w[6].pv.data.iValue; // 假设单位已经是0.001S
        values.push_back((t1_time >> 16) & 0xFF);
        values.push_back((t1_time >> 8) & 0xFF);
        values.push_back(t1_time & 0xFF);
        values.push_back((t2_time >> 16) & 0xFF);
        values.push_back((t2_time >> 8) & 0xFF);
        values.push_back(t2_time & 0xFF);

        // 角度
        values.push_back(ang43H_w[7].pv.data.iValue); // 假设这里是2

        ret = this->AngridPreset(addr, 0x5A, 0x43, 26, values);
        ang43Htemp_w = 0;
    }

    // 命令字： 44H
    // 命令作用： 设置高低穿模式：高压穿越其中一项参数，非执行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x44 X（1 字节UVW相设置相选择， 0 代表U 相，1 代表V  相，2 代表W相) X（1 字节使能开关， 0 代表关， 1 代表开） XX（2 字节触发角度，单 位0.1°) XXX XXX XXX（3 字节*3：U1 、U2 、U3 电压，单位0.01V）XXX XXX（3  字节*2：T1 、T2 时间设置，单位0.001S）XX（2 字节角度，单位0.1°) X  0x7D  (29 字 节)
    // 命令举例：7B 00 1D 01 5A 44 00 01 00 00 00 6F B8 00 67 20 00 5E 88 00 01 F4 00 25 1C 00 00 87 7D
    // (设置 U 相参数、使能开、触发相位： 0 °、U1：286V、U2：264V、U3：242V、T1：0.5S、 T2：9.5S、角度 0 °)
    //读取265-273并存入数组
    //读取274-282并存入数组
    //读取283-291并存入数组
    RealData ang44H[9];
    RealData ang44H_v[9];
    RealData ang44H_w[9];
    int ang44Htemp = 0;
    int ang44Htemp_v = 0;
    int ang44Htemp_w = 0;

    for (int i = 0; i < 9; i++) {
        if (MemDb::GetRealDataWrited(realData, regStart + 265 + i, true)) {
            ang44H[i] = realData;
            temp++;
        }
    }
    for (int i = 0; i < 9; i++) {
        if (MemDb::GetRealDataWrited(realData, regStart + 274 + i, true)) {
            ang44H_v[i] = realData;
            temp++;
        }
    }
    for (int i = 0; i < 9; i++) {
        if (MemDb::GetRealDataWrited(realData, regStart + 283 + i, true)) {
            ang44H_w[i] = realData;
            temp++;
        }
    }

    if (ang44Htemp != 0) {
        values.clear();
        // U 相设置相选择
        values.push_back(ang44H[0].pv.data.iValue); // 假设这里是1字节整数

        // 使能开关
        values.push_back(ang44H[1].pv.data.iValue); // 假设这里是1字节整数

        // 触发角度
        values.push_back(ang44H[2].pv.data.iValue); // 假设这里是2字节整数

        // U1 、U2 、U3 电压
        int u1_voltage = ang44H[3].pv.data.iValue; // 假设单位已经是0.01V
        int u2_voltage = ang44H[4].pv.data.iValue; // 假设单位已经是0.01V
        int u3_voltage = ang44H[5].pv.data.iValue; // 假设单位已经是0.01V
        values.push_back((u1_voltage >> 16) & 0xFF);
        values.push_back((u1_voltage >> 8) & 0xFF);
        values.push_back(u1_voltage & 0xFF);
        values.push_back((u2_voltage >> 16) & 0xFF);
        values.push_back((u2_voltage >> 8) & 0xFF);
        values.push_back(u2_voltage & 0xFF);
        values.push_back((u3_voltage >> 16) & 0xFF);
        values.push_back((u3_voltage >> 8) & 0xFF);
        values.push_back(u3_voltage & 0xFF);

        // T1 、T2 时间设置
        int t1_time = ang44H[6].pv.data.iValue; // 假设单位已经是0.001S
        int t2_time = ang44H[7].pv.data.iValue; // 假设单位已经是0.001S
        values.push_back((t1_time >> 16) & 0xFF);
        values.push_back((t1_time >> 8) & 0xFF);
        values.push_back(t1_time & 0xFF);
        values.push_back((t2_time >> 16) & 0xFF);
        values.push_back((t2_time >> 8) & 0xFF);
        values.push_back(t2_time & 0xFF);

        // 角度
        values.push_back(ang44H[8].pv.data.iValue); // 假设这里是2字节整数

        ret = this->AngridPreset(addr, 0x5A, 0x44, 29, values);
        ang44Htemp = 0;
    }
    if (ang44Htemp_v != 0) {
        values.clear();
        // V 相设置相选择
        values.push_back(ang44H_v[0].pv.data.iValue); // 假设这里是1字节整数

        // 使能开关
        values.push_back(ang44H_v[1].pv.data.iValue); // 假设这里是1字节整数

        // 触发角度
        values.push_back(ang44H_v[2].pv.data.iValue); // 假设这里是2字节整数

        // U1 、U2 、U3 电压
        int u1_voltage = ang44H_v[3].pv.data.iValue; // 假设单位已经是0.01V
        int u2_voltage = ang44H_v[4].pv.data.iValue; // 假设单位已经是0.01V
        int u3_voltage = ang44H_v[5].pv.data.iValue; // 假设单位已经是0.01V
        values.push_back((u1_voltage >> 16) & 0xFF);
        values.push_back((u1_voltage >> 8) & 0xFF);
        values.push_back(u1_voltage & 0xFF);
        values.push_back((u2_voltage >> 16) & 0xFF);
        values.push_back((u2_voltage >> 8) & 0xFF);
        values.push_back(u2_voltage & 0xFF);
        values.push_back((u3_voltage >> 16) & 0xFF);
        values.push_back((u3_voltage >> 8) & 0xFF);
        values.push_back(u3_voltage & 0xFF);

        // T1 、T2 时间设置
        int t1_time = ang44H_v[6].pv.data.iValue; // 假设单位已经是0.001S
        int t2_time = ang44H_v[7].pv.data.iValue; // 假设单位已经是0.001S
        values.push_back((t1_time >> 16) & 0xFF);
        values.push_back((t1_time >> 8) & 0xFF);
        values.push_back(t1_time & 0xFF);
        values.push_back((t2_time >> 16) & 0xFF);
        values.push_back((t2_time >> 8) & 0xFF);
        values.push_back(t2_time & 0xFF);
        
        // 角度
        values.push_back(ang44H_v[8].pv.data.iValue); // 假设这里是2字节整数

        ret = this->AngridPreset(addr, 0x5A, 0x44, 29, values);
        ang44Htemp_v = 0;
    }
    if (ang44Htemp_w != 0) {
        values.clear();
        // W 相设置相选择
        values.push_back(ang44H_w[0].pv.data.iValue); // 假设这里是1字节整数

        // 使能开关
        values.push_back(ang44H_w[1].pv.data.iValue); // 假设这里是1字节整数

        // 触发角度
        values.push_back(ang44H_w[2].pv.data.iValue); // 假设这里是2字节整数

        // U1 、U2 、U3 电压
        int u1_voltage = ang44H_w[3].pv.data.iValue; // 假设单位已经是0.01V
        int u2_voltage = ang44H_w[4].pv.data.iValue; // 假设单位已经是0.01V
        int u3_voltage = ang44H_w[5].pv.data.iValue; // 假设单位已经是0.01V
        values.push_back((u1_voltage >> 16) & 0xFF);
        values.push_back((u1_voltage >> 8) & 0xFF);
        values.push_back(u1_voltage & 0xFF);
        values.push_back((u2_voltage >> 16) & 0xFF);
        values.push_back((u2_voltage >> 8) & 0xFF);
        values.push_back(u2_voltage & 0xFF);
        values.push_back((u3_voltage >> 16) & 0xFF);
        values.push_back((u3_voltage >> 8) & 0xFF);
        values.push_back(u3_voltage & 0xFF);

        // T1 、T2 、T3 时间设置
        int t1_time = ang44H_w[6].pv.data.iValue; // 假设单位已经是0.001S
        int t2_time = ang44H_w[7].pv.data.iValue; // 假设单位已经是0.001S
        values.push_back((t1_time >> 16) & 0xFF);
        values.push_back((t1_time >> 8) & 0xFF);
        values.push_back(t1_time & 0xFF);
        values.push_back((t2_time >> 16) & 0xFF);
        values.push_back((t2_time >> 8) & 0xFF);
        values.push_back(t2_time & 0xFF);

        // 角度
        values.push_back(ang44H_w[8].pv.data.iValue); // 假设这里是2字节整数

        ret = this->AngridPreset(addr, 0x5A, 0x44, 29, values);
        ang44Htemp_w = 0;
    }

    // 命令字： 45H
    // 命令作用： 设置高低穿模式：高低穿组合其中一项参数，非执行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x45 X（1 字节UVW相设置相选择， 0 代表U 相，1 代表V 相，2 代表W相) X（1 字节使能开关， 00 代表关， 01 代表开） XX（2 字节触发角度， 单位0.1°) XXX XXX XXX（3 字节*3：U1 、U2 、U3 电压，单位0.01V）XXX XXX（3 字节*2：T1 、T2 时间设置，单位0.001S）XX（2 字节角度，单位0.1°) X  0x7D  (29 字 节)
    // 命令举例： 7B 00 1D 01 5A 45 00 01 00 00 00 11 30 00 6F B8 00 5E 88 00 02 71 00 00 C8 00 00 47 7D
    // (设置 U 相参数、使能开、触发相位： 0 °、U1：44V、U2：286V、U3：242V、T1:0.625S、T2：0.2S、角度 0 °)
    //读取292-300并存入数组
    //读取301-309并存入数组
    //读取310-318并存入数组
    RealData ang45H[9];
    RealData ang45H_v[9];
    RealData ang45H_w[9];
    int ang45Htemp = 0;
    int ang45Htemp_v = 0;
    int ang45Htemp_w = 0;

    for (int i = 0; i < 9; i++) {
        if (MemDb::GetRealDataWrited(realData, regStart + 292 + i, true)) {
            ang45H[i] = realData;
            temp++;
        }
    }
    for (int i = 0; i < 9; i++) {
        if (MemDb::GetRealDataWrited(realData, regStart + 301 + i, true)) {
            ang45H_v[i] = realData;
            temp++;
        }
    }
    for (int i = 0; i < 9; i++) {
        if (MemDb::GetRealDataWrited(realData, regStart + 310 + i, true)) {
            ang45H_w[i] = realData;
            temp++;
        }
    }

    if (ang45Htemp != 0) {
        values.clear();
        // U 相设置相选择
        values.push_back(ang45H[0].pv.data.iValue); // 假设这里是1字节整数

        // 使能开关
        values.push_back(ang45H[1].pv.data.iValue); // 假设这里是1字节整数

        // 触发角度
        values.push_back(ang45H[2].pv.data.iValue); // 假设这里是2字节整数

        // U1 、U2 、U3 电压
        int u1_voltage = ang45H[3].pv.data.iValue; // 假设单位已经是0.01V
        int u2_voltage = ang45H[4].pv.data.iValue; // 假设单位已经是0.01V
        int u3_voltage = ang45H[5].pv.data.iValue; // 假设单位已经是0.01V
        values.push_back((u1_voltage >> 16) & 0xFF);
        values.push_back((u1_voltage >> 8) & 0xFF);
        values.push_back(u1_voltage & 0xFF);
        values.push_back((u2_voltage >> 16) & 0xFF);
        values.push_back((u2_voltage >> 8) & 0xFF);
        values.push_back(u2_voltage & 0xFF);
        values.push_back((u3_voltage >> 16) & 0xFF);
        values.push_back((u3_voltage >> 8) & 0xFF);
        values.push_back(u3_voltage & 0xFF);

        // T1 、T2 、T3 时间设置
        int t1_time = ang45H[6].pv.data.iValue; // 假设单位已经是0.001S
        int t2_time = ang45H[7].pv.data.iValue; // 假设单位已经是0.001S
        values.push_back((t1_time >> 16) & 0xFF);
        values.push_back((t1_time >> 8) & 0xFF);
        values.push_back(t1_time & 0xFF);
        values.push_back((t2_time >> 16) & 0xFF);
        values.push_back((t2_time >> 8) & 0xFF);
        values.push_back(t2_time & 0xFF);

        // 角度
        values.push_back(ang45H[8].pv.data.iValue); // 假设这里是2字节整数

        ret = this->AngridPreset(addr, 0x5A, 0x45, 29, values);
        ang45Htemp = 0;
    }
    if (ang45Htemp_v != 0) {
        values.clear();
        // V 相设置相选择
        values.push_back(ang45H_v[0].pv.data.iValue); // 假设这里是1字节整数

        // 使能开关
        values.push_back(ang45H_v[1].pv.data.iValue); // 假设这里是1字节整数

        // 触发角度
        values.push_back(ang45H_v[2].pv.data.iValue); // 假设这里是2字节整数

        // U1 、U2 、U3 电压
        int u1_voltage = ang45H_v[3].pv.data.iValue; // 假设单位已经是0.01V
        int u2_voltage = ang45H_v[4].pv.data.iValue; // 假设单位已经是0.01V
        int u3_voltage = ang45H_v[5].pv.data.iValue; // 假设单位已经是0.01V
        values.push_back((u1_voltage >> 16) & 0xFF);
        values.push_back((u1_voltage >> 8) & 0xFF);
        values.push_back(u1_voltage & 0xFF);
        values.push_back((u2_voltage >> 16) & 0xFF);
        values.push_back((u2_voltage >> 8) & 0xFF);
        values.push_back(u2_voltage & 0xFF);
        values.push_back((u3_voltage >> 16) & 0xFF);
        values.push_back((u3_voltage >> 8) & 0xFF);
        values.push_back(u3_voltage & 0xFF);

        // T1 、T2 、T3 时间设置
        int t1_time = ang45H_v[6].pv.data.iValue; // 假设单位已经是0.001S
        int t2_time = ang45H_v[7].pv.data.iValue; // 假设单位已经是0.001S
        values.push_back((t1_time >> 16) & 0xFF);
        values.push_back((t1_time >> 8) & 0xFF);
        values.push_back(t1_time & 0xFF);
        values.push_back((t2_time >> 16) & 0xFF);
        values.push_back((t2_time >> 8) & 0xFF);
        values.push_back(t2_time & 0xFF);

        // 角度
        values.push_back(ang45H_v[8].pv.data.iValue); // 假设这里是2字节整数

        ret = this->AngridPreset(addr, 0x5A, 0x45, 29, values);
        ang45Htemp_v = 0;
    }
    if (ang45Htemp_w != 0) {
        values.clear();
        // W 相设置相选择
        values.push_back(ang45H_w[0].pv.data.iValue); // 假设这里是1字节整数

        // 使能开关
        values.push_back(ang45H_w[1].pv.data.iValue); // 假设这里是1字节整数

        // 触发角度
        values.push_back(ang45H_w[2].pv.data.iValue); // 假设这里是2字节整数

        // U1 、U2 、U3 电压
        int u1_voltage = ang45H_w[3].pv.data.iValue; // 假设单位已经是0.01V
        int u2_voltage = ang45H_w[4].pv.data.iValue; // 假设单位已经是0.01V
        int u3_voltage = ang45H_w[5].pv.data.iValue; // 假设单位已经是0.01V
        values.push_back((u1_voltage >> 16) & 0xFF);
        values.push_back((u1_voltage >> 8) & 0xFF);
        values.push_back(u1_voltage & 0xFF);
        values.push_back((u2_voltage >> 16) & 0xFF);
        values.push_back((u2_voltage >> 8) & 0xFF);
        values.push_back(u2_voltage & 0xFF);
        values.push_back((u3_voltage >> 16) & 0xFF);
        values.push_back((u3_voltage >> 8) & 0xFF);
        values.push_back(u3_voltage & 0xFF);

        // T1 、T2 、T3 时间设置
        int t1_time = ang45H_w[6].pv.data.iValue; // 假设单位已经是0.001S
        int t2_time = ang45H_w[7].pv.data.iValue; // 假设单位已经是0.001S
        values.push_back((t1_time >> 16) & 0xFF);
        values.push_back((t1_time >> 8) & 0xFF);
        values.push_back(t1_time & 0xFF);
        values.push_back((t2_time >> 16) & 0xFF);
        values.push_back((t2_time >> 8) & 0xFF);
        values.push_back(t2_time & 0xFF);

        // 角度
        values.push_back(ang45H_w[8].pv.data.iValue); // 假设这里是2字节整数

        ret = this->AngridPreset(addr, 0x5A, 0x45, 29, values);
        ang45Htemp_w = 0;
    }


    // 命令字： 47H
    // 命令作用： 设置不平衡模式 1 参数，非执行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x47 XX（2 字节触发角度，单位0.1°) XXX XX（U 相参 数：3 字节电压单位0.01V、2 字节角度单位0.1°, 后同） XXX XX（V 相参数） XXX  XX（W相参数） X 0x7D (25 字节)
    // 命令举例： 7B 00 19 01 5A 47 00 00 00 55 F0 00 00 00 2AF8 09 60 00 55 F0 04 B0 84 7D （触发角度： 0 °、U 相： 220V 、0 °V 相： 110V 、240°W 相： 220V 、120°)
    //读取319-324并存入数组
    RealData ang47H[6];
    int ang47Htemp = 0;
    
    for (int i = 0; i < 6; i++) {
        if (MemDb::GetRealDataWrited(realData, regStart + 319 + i, true)) {
            ang47H[i] = realData;
            temp++;
        }
    }

    if (ang47Htemp != 0) {
        values.clear();
        // 触发角度
        values.push_back(ang47H[0].pv.data.iValue); // 假设这里是2字节整数

        // U 相参数
        int u_voltage = ang47H[1].pv.data.iValue; // 假设单位已经是0.01V
        int u_angle = ang47H[2].pv.data.iValue; // 假设单位已经是0.1°
        values.push_back((u_voltage >> 16) & 0xFF);
        values.push_back((u_voltage >> 8) & 0xFF);
        values.push_back(u_voltage & 0xFF);
        values.push_back((u_angle >> 8) & 0xFF);
        values.push_back(u_angle & 0xFF);

        // V 相参数
        int v_voltage = ang47H[3].pv.data.iValue; // 假设单位已经是0.01V
        int v_angle = ang47H[4].pv.data.iValue; // 假设单位已经是0.1°
        values.push_back((v_voltage >> 16) & 0xFF);
        values.push_back((v_voltage >> 8) & 0xFF);
        values.push_back(v_voltage & 0xFF);
        values.push_back((v_angle >> 8) & 0xFF);
        values.push_back(v_angle & 0xFF);

        // W 相参数
        int w_voltage = ang47H[5].pv.data.iValue; // 假设单位已经是0.01V
        int w_angle = ang47H[6].pv.data.iValue; // 假设单位已经是0.1°
        values.push_back((w_voltage >> 16) & 0xFF);
        values.push_back((w_voltage >> 8) & 0xFF);
        values.push_back(w_voltage & 0xFF);
        values.push_back((w_angle >> 8) & 0xFF);
        values.push_back(w_angle & 0xFF);

        ret = this->AngridPreset(addr, 0x5A, 0x47, 25, values);
        ang47Htemp = 0;
    }


    // 命令字： 34H
    // 命令作用：设置谐波主界面参数,  非执行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x34 X（1 字节组选择， 00-49）XX（2 字节触发角度，单 位0.1°) XXX（1 字节U 相使能开关、1 字节V 相使能开关、1 字节W相使能开关） X  7D(14 字节)
    // 命令举例： 7B 00 0E 01 5A 34 00 04 B0 01 00 00 52 7D
    // （第 0 组、 触发角度 120°、U 相使能开、V 相使能关、 W 相使能关）
    //读取325-327并存入数组
    RealData ang34H[3];
    int ang34Htemp = 0;
    
    for (int i = 0; i < 3; i++) {
        if (MemDb::GetRealDataWrited(realData, regStart + 325 + i, true)) {
            ang34H[i] = realData;
            temp++;
        }
    }

    if (ang34Htemp != 0) {
        values.clear();
        // 组选择
        values.push_back(ang34H[0].pv.data.iValue); // 假设这里是1字节整数

        // 触发角度
        values.push_back(ang34H[1].pv.data.iValue); // 假设这里是2字节整数

        // U 相使能开关、V 相使能开关、W 相使能开关
        values.push_back(ang34H[2].pv.data.iValue); // 假设这里是1字节整数

        ret = this->AngridPreset(addr, 0x5A, 0x34, 14, values);
        ang34Htemp = 0;
    }

    // 命令字： 31H
    // 命令作用：设置谐波组参数,  非执行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x31 X（1 字节组选择， 00-49）X（1 字节次数， 02-50） XX（2 字节百分比） XX（2 字节角度， 单位0.1°) X（1 字节使能开关） X 7D(15 字节)
    // 命令举例： 7B 00 0F 01 5A 31 00 03 00 96 07 08 01 44 7D
    // （第 0 组、第 3 次、 15%、角度： 180°、使能开）
    //读取328-332并存入数组
    RealData ang31H[5];
    int ang31Htemp = 0;

    for (int i = 0; i < 5; i++) {
        if (MemDb::GetRealDataWrited(realData, regStart + 328 + i, true)) {
            ang31H[i] = realData;
            temp++;
        }
    }

    if (ang31Htemp != 0) {
        values.clear();
        // 组选择
        values.push_back(ang31H[0].pv.data.iValue); // 假设这里是1字节整数

        // 次数
        values.push_back(ang31H[1].pv.data.iValue); // 假设这里是1字节整数

        // 百分比
        int percentage = ang31H[2].pv.data.iValue; // 假设单位已经是0.01%
        values.push_back((percentage >> 8) & 0xFF);
        values.push_back(percentage & 0xFF);

        // 角度
        values.push_back(ang31H[3].pv.data.iValue); // 假设这里是2字节整数

        // 使能开关
        values.push_back(ang31H[4].pv.data.iValue); // 假设这里是1字节整数

        ret = this->AngridPreset(addr, 0x5A, 0x31, 15, values);
        ang31Htemp = 0;
    }

    // 命令字： 37H
    // 命令作用：设置谐波清除参数,  非执行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x37 X  （1 字节组选择) X（1 字节模式选择， 1 表示谐波 当前组开关全部为关， 2 表示谐波当前组数据全部清除） X 7D(10 字节)
    // 命令举例： 7B 00 0A 01 5A 37 00 01 9D 7D
    // （谐波第 0 组，当前组开关全部为关）
    //读取333-334并存入数组
    RealData ang37H[2];
    int ang37Htemp = 0;

    for (int i = 0; i < 2; i++) {
        if (MemDb::GetRealDataWrited(realData, regStart + 333 + i, true)) {
            ang37H[i] = realData;
            temp++;
        }
    }

    if (ang37Htemp != 0) {
        values.clear();
        // 组选择
        values.push_back(ang37H[0].pv.data.iValue); // 假设这里是1字节整数

        // 模式选择
        values.push_back(ang37H[1].pv.data.iValue); // 假设这里是1字节整数

        ret = this->AngridPreset(addr, 0x5A, 0x37, 10, values);
        ang37Htemp = 0;
    }

    zlog_info(Util::m_zlog, "设置电网模拟器结束");
    return ret;
}

int ANGridSIm::AngridPreset(int addr, int type, int data, short length,
                            UshortArray values) {
    int error = ErrorInfo::ERR_OK;
    String query;
    String reply;
    unsigned short crc16;
    int retry      = 0;
    unsigned int i = 0;

    // 封装控制类命令
    query.clear();
    query.push_back(0x7B);          // 起始帧
    query.push_back(length >> 8);   // 总字节数高字节
    query.push_back(length & 0xFF); // 总字节数低字节
    query.push_back(addr);          // 从机地址
    query.push_back(type);          // 命令类型
    query.push_back(data);          // 命令字

    switch (data) {
    // 命令字： 00H
    // 命令作用：停止
    // 命令格式： 0x7B XX X 0x0F 0x00 X 0x7D (8 字节)
    // 命令举例： 7B 00 08 01 0F 00 18 7D
    // 命令返回： 7B 00 09 01 0F 00 00  19
    // 7D（控制类和设置类命令返回格式相同，后面不再 举例） 命令字： FFH
    // 命令作用：启动
    // 命令格式： 0x7B XX X 0x0F 0xFF X 0x7D (8 字节)
    // 命令举例： 7B 00 08 01 0F FF 17 7D
    // 命令字： 01H
    // 命令作用：停止触发
    // 命令格式： 0x7B XX X 0x0F 0x01 X 0x7D (8 字节)
    // 命令举例： 7B 00 08 01 0F 01 19 7D
    // 命令字： 14H
    // 命令作用： 设置模式为不平衡，非执行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x14 X 0x7D (8 字节)
    // 命令举例： 7B 00 08 01 5A 14 77 7D
    // 命令字： 18H
    // 命令作用： 设置模式为谐波， 非执行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x18 X 0x7D (8 字节)
    // 命令举例： 7B 00 08 01 5A 18 7B 7D
    case 0xFF:
        // query.push_back(values[0]);
        // break;
    case 0x00:
    case 0x14:
    case 0x18:
    case 0x03:
        for (i = 0; i < values.size(); i++) {
            query.push_back(((int)(values[i])) >> 8);
            query.push_back(((int)(values[i])) & 0xFF);
        }
        break;
    // 命令字： 12H
    // 命令作用： 设置输出模式为高低穿的某个模式，非执行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x12    X（1 字节模式选择,0 为零穿模式， 1 为低穿模式， 2 为高穿模式，3 为高低穿模式）X 0x7D (9 字节)
    // 命令举例： 7B 00 09 01 5A 12 00 76 7D（设置为零穿待机态）
    case 0x12:
        query.push_back(values[0]);
        break;
    // 命令字： 41H
    // 命令作用： 设置常规参数，非运行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x41 XX (2 字节缓启动，单位0.1S)XXX（3 字节频率，单位0.001Hz） XXX  （U 相参数： 3 字节电压， 单位0.01V、2 字节角度， 单位0.1°, 后 同）XXX XX (V 相参数） XXX XX (W相参数) X 0x7D (28 字节)
    // 命令举例：7B 00 1C 01 5A 41 00 00 00 C3 50 00 55 F0 00 00 00 55 F0 09 60 00 55 F0 04 B0 B7 7D
    // (0 秒缓升、 50Hz、U 相： 220V 、0 °  V 相： 220V 、240 °W 相： 220V 、120°)
    case 0x41:
        query.push_back(values[0] >> 8); // 缓启动高字节
        query.push_back(values[0] & 0xFF); // 缓启动低字节
        query.push_back(values[1]); // 频率的第一个字节
        query.push_back(values[2]); // 频率的第二个字节
        query.push_back(values[3]); // 频率的第三个字节
        // U 相电压和角度
        query.push_back(values[4]); // U相电压高字节
        query.push_back(values[5]); // U相电压中字节
        query.push_back(values[6]); // U相电压低字节
        query.push_back(values[7]); // U相角度高字节
        query.push_back(values[8]); // U相角度低字节
        // V 相电压和角度
        query.push_back(values[9]); // V相电压高字节
        query.push_back(values[10]); // V相电压中字节
        query.push_back(values[11]); // V相电压低字节
        query.push_back(values[12]); // V相角度高字节
        query.push_back(values[13]); // V相角度低字节
        // W 相电压和角度
        query.push_back(values[14]); // W相电压高字节
        query.push_back(values[15]); // W相电压中字节
        query.push_back(values[16]); // W相电压低字节
        query.push_back(values[17]); // W相角度高字节
        query.push_back(values[18]); // W相角度低字节

        break;
    // 命令字： 42H
    // 命令作用： 设置高低穿模式：零压穿越其中一项参数，非执行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x42 X（1 字节UVW相设置相选择， 0 代表U 相，1 代表V 相，2 代表W相) X（1 字节使能开关）XX（2 字节触发角度，单位0.1°)XXX XXX XXX （3 字节*3：U1 、U2 、U3 电压，单位0.01V）XXX XXX XXX（3 字节*3：T1 、T2 、T3 时间设置，单位0.001S）XX（2 字节角度，单位0.1°) X 0x7D (32 字节)
    // 命令举例： 7B 00 20 01 5A 42 00 01 00 00 00 00 00 00 11 30 00 4D 58 00 00 96 00 01 DB 00 05 5F 00 00 7A 7D
    // (设置 U 相参数、使能开、触发相位：0 °、U1：0V、U2：44V、U3：198V 、T1：0.15S、 T2：0.475S 、T3：1.375S、角度 0 °)
    case 0x42:
        // U 相设置相选择
        query.push_back(values[0]); // 第1字节

        // 使能开关
        query.push_back(values[1]); // 第2字节

        // 触发角度
        query.push_back(values[2] >> 8); // 第3字节（高位）
        query.push_back(values[2] & 0xFF); // 第4字节（低位）

        // U1 电压
        query.push_back(values[3]); // 第5字节（高位）
        query.push_back(values[4]); // 第6字节（中位）
        query.push_back(values[5]); // 第7字节（低位）

        // U2 电压
        query.push_back(values[6]); // 第8字节（高位）
        query.push_back(values[7]); // 第9字节（中位）
        query.push_back(values[8]); // 第10字节（低位）

        // U3 电压
        query.push_back(values[9]); // 第11字节（高位）
        query.push_back(values[10]); // 第12字节（中位）
        query.push_back(values[11]); // 第13字节（低位）

        // T1 时间
        query.push_back(values[12]); // 第14字节（高位）
        query.push_back(values[13]); // 第15字节（中位）
        query.push_back(values[14]); // 第16字节（低位）

        // T2 时间
        query.push_back(values[15]); // 第17字节（高位）
        query.push_back(values[16]); // 第18字节（中位）
        query.push_back(values[17]); // 第19字节（低位）

        // T3 时间
        query.push_back(values[18]); // 第20字节（高位）
        query.push_back(values[19]); // 第21字节（中位）
        query.push_back(values[20]); // 第22字节（低位）

        // 角度
        query.push_back(values[21] >> 8); // 第23字节（高位）
        query.push_back(values[21] & 0xFF); // 第24字节（低位）
        break;
    // 命令字： 43H
    // 命令作用： 设置高低穿模式：低压穿越其中一项参数，非执行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x43 X（1 字节UVW相设置相选择， 0 代表U 相，1 代表V  相，2 代表W相) X（1 字节使能开关， 0 代表关， 1 代表开） XX（2 字节触发角度，单 位0.1°) XXX XXX（3 字节*2：U1 、U2 电压，单位0.01V）XXX XXX（3 字节*2：T1、 T2 时间设置，单位0.001S）XX（2 字节角度，单位0.1°) X 0x7D (26 字节)
    // 命令举例： 7B 00 1A 01 5A 43 00 01 00 00 00 11 30 00 4D 58 00 02 71 00 05 5F 00 00 76 7D  (设置 U 相参数、使能开、触发相位： 0°、U1：44V、U2：198V、T1：0.625S、T2：1.375S、 角度 0 °)
    case 0x43:
        // 相设置相选择
        query.push_back(values[0]); // 第1字节

        // 使能开关
        query.push_back(values[1]); // 第2字节

        // 触发角度
        query.push_back(values[2] >> 8); // 第3字节（高位）
        query.push_back(values[2] & 0xFF); // 第4字节（低位）

        // U1 电压
        query.push_back(values[3]); // 第5字节（高位）
        query.push_back(values[4]); // 第6字节（中位）
        query.push_back(values[5]); // 第7字节（低位）

        // U2 电压
        query.push_back(values[6]); // 第8字节（高位）
        query.push_back(values[7]); // 第9字节（中位）
        query.push_back(values[8]); // 第10字节（低位）

        // T1 时间
        query.push_back(values[9]); // 第11字节（高位）
        query.push_back(values[10]); // 第12字节（中位）
        query.push_back(values[11]); // 第13字节（低位）

        // T2 时间
        query.push_back(values[12]); // 第14字节（高位）
        query.push_back(values[13]); // 第15字节（中位）
        query.push_back(values[14]); // 第16字节（低位）

        // 角度
        query.push_back(values[15] >> 8); // 第17字节（高位）
        query.push_back(values[15] & 0xFF); // 第18字节（低位）
        break;
    // 命令字： 44H
    // 命令作用： 设置高低穿模式：高压穿越其中一项参数，非执行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x44 X（1 字节UVW相设置相选择， 0 代表U 相，1 代表V  相，2 代表W相) X（1 字节使能开关， 0 代表关， 1 代表开） XX（2 字节触发角度，单 位0.1°) XXX XXX XXX（3 字节*3：U1 、U2 、U3 电压，单位0.01V）XXX XXX（3  字节*2：T1 、T2 时间设置，单位0.001S）XX（2 字节角度，单位0.1°) X  0x7D  (29 字 节)
    // 命令举例：7B 00 1D 01 5A 44 00 01 00 00 00 6F B8 00 67 20 00 5E 88 00 01 F4 00 25 1C 00 00 87 7D
    // (设置 U 相参数、使能开、触发相位： 0 °、U1：286V、U2：264V、U3：242V、T1：0.5S、
    // T2：9.5S、角度 0 °)
    case 0x44:
        // 将U/V/W相的参数以及其他设置添加到query中
        query.push_back(values[0]); // U相设置相选择
        query.push_back(values[1]); // 使能开关
        query.push_back(values[2]); // 触发角度高位
        query.push_back(values[3]); // 触发角度低位

        // U1, U2, U3电压值（每个电压需要3字节）
        for (int i = 4; i <= 14; i++) {
            query.push_back(values[i]); // 添加U1, U2, U3电压值
        }

        // T1, T2时间设置（每个时间也需要3字节）
        for (int i = 15; i <= 20; i++) {
            query.push_back(values[i]); // 添加T1, T2时间值
        }

        // 最后的角度参数
        query.push_back(values[21]); // 角度高位
        query.push_back(values[22]); // 角度低位
        break;


    // 命令字： 45H
    // 命令作用： 设置高低穿模式：高低穿组合其中一项参数，非执行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x45 X（1 字节UVW相设置相选择， 0 代表U 相，1 代表V 相，2 代表W相) X（1 字节使能开关， 00 代表关， 01 代表开） XX（2 字节触发角度， 单位0.1°) XXX XXX XXX（3 字节*3：U1 、U2 、U3 电压，单位0.01V）XXX XXX（3 字节*2：T1 、T2 时间设置，单位0.001S）XX（2 字节角度，单位0.1°) X  0x7D  (29 字 节)
    // 命令举例： 7B 00 1D 01 5A 45 00 01 00 00 00 11 30 00 6F B8 00 5E 88 00 02 71 00 00 C8 00 00 47 7D
    // (设置 U 相参数、使能开、触发相位： 0 °、U1：44V、U2：286V、U3：242V、T1:0.625S、
    // T2：0.2S、角度 0 °)
    case 0x45:
        // 添加U/V/W相的选择
        query.push_back(values[0]); // U/V/W相设置相选择

        // 添加使能开关
        query.push_back(values[1]); // 使能开关

        // 触发角度
        query.push_back(values[2] >> 8); // 触发角度高位
        query.push_back(values[2] & 0xFF); // 触发角度低位

        // U1, U2, U3电压值（每个电压需要3字节）
        for (int i = 3; i <= 11; i++) {
            query.push_back(values[i]); // 添加U1, U2, U3电压值
        }

        // T1, T2时间设置（每个时间需要3字节）
        for (int i = 12; i <= 17; i++) {
            query.push_back(values[i]); // 添加T1, T2时间值
        }

        // 最后的角度参数
        query.push_back(values[18] >> 8); // 角度高位
        query.push_back(values[18] & 0xFF); // 角度低位
        break;


    // 命令字： 47H
    // 命令作用： 设置不平衡模式 1 参数，非执行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x47 XX（2 字节触发角度，单位0.1°) XXX XX（U 相参 数：3 字节电压单位0.01V、2 字节角度单位0.1°, 后同） XXX XX（V 相参数） XXX  XX（W相参数） X 0x7D (25 字节)
    // 命令举例： 7B 00 19 01 5A 47 00 00 00 55 F0 00 00 00 2AF8 09 60 00 55 F0 04 B0 84 7D （触发角度： 0 °、U 相： 220V 、0 °V 相： 110V 、240°W 相： 220V 、120°)
    case 0x47:
        // 触发角度
        query.push_back(values[0] >> 8); // 触发角度高位
        query.push_back(values[0] & 0xFF); // 触发角度低位

        // U相电压和角度
        query.push_back(values[1]); // U相电压高位
        query.push_back(values[2]); // U相电压中位
        query.push_back(values[3]); // U相电压低位
        query.push_back(values[4]); // U相角度高位
        query.push_back(values[5]); // U相角度低位

        // V相电压和角度
        query.push_back(values[6]); // V相电压高位
        query.push_back(values[7]); // V相电压中位
        query.push_back(values[8]); // V相电压低位
        query.push_back(values[9]); // V相角度高位
        query.push_back(values[10]); // V相角度低位

        // W相电压和角度
        query.push_back(values[11]); // W相电压高位
        query.push_back(values[12]); // W相电压中位
        query.push_back(values[13]); // W相电压低位
        query.push_back(values[14]); // W相角度高位
        query.push_back(values[15]); // W相角度低位
        break;
    // 命令字： 34H
    // 命令作用：设置谐波主界面参数,  非执行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x34 X（1 字节组选择， 00-49）XX（2 字节触发角度，单 位0.1°) XXX（1 字节U 相使能开关、1 字节V 相使能开关、1 字节W相使能开关） X  7D(14 字节)
    // 命令举例： 7B 00 0E 01 5A 34 00 04 B0 01 00 00 52 7D
    // （第 0 组、 触发角度 120°、U 相使能开、V 相使能关、 W 相使能关）
    case 0x34:
    // 添加组选择
        query.push_back(values[0]); // 组选择

        // 触发角度
        query.push_back(values[1] >> 8); // 触发角度高位
        query.push_back(values[1] & 0xFF); // 触发角度低位

        // U、V、W相使能开关
        // 假设: values[2]的低三位分别表示U、V、W相的使能开关
        query.push_back(values[2] & 0x01); // U相使能开关
        query.push_back((values[2] >> 1) & 0x01); // V相使能开关
        query.push_back((values[2] >> 2) & 0x01); // W相使能开关
        break;
    // 命令字： 31H
    // 命令作用：设置谐波组参数,  非执行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x31 X（1 字节组选择， 00-49）X（1 字节次数， 02-50） XX（2 字节百分比） XX（2 字节角度， 单位0.1°) X（1 字节使能开关） X 7D(15 字节)
    // 命令举例： 7B 00 0F 01 5A 31 00 03 00 96 07 08 01 44 7D
    // （第 0 组、第 3 次、 15%、角度： 180°、使能开）
    case 0x31:
        // 添加组选择
        query.push_back(values[0]); // 组选择，1字节

        // 添加次数
        query.push_back(values[1]); // 次数，1字节

        // 添加百分比
        query.push_back(values[2]); // 百分比高位
        query.push_back(values[3]); // 百分比低位

        // 添加角度
        query.push_back(values[4] >> 8); // 角度高位
        query.push_back(values[4] & 0xFF); // 角度低位

        // 添加使能开关
        query.push_back(values[5]); // 使能开关，1字节
        break;
    // 命令字： 37H
    // 命令作用：设置谐波清除参数,  非执行/报警/急停态有效
    // 命令格式： 0x7B XX X 0x5A 0x37 X  （1 字节组选择) X（1 字节模式选择， 1 表示谐波 当前组开关全部为关， 2 表示谐波当前组数据全部清除） X 7D(10 字节)
    // 命令举例： 7B 00 0A 01 5A 37 00 01 9D 7D
    // （谐波第 0 组，当前组开关全部为关）
    case 0x37:
        // 添加组选择
        query.push_back(values[0]); // 组选择，1字节

        // 添加模式选择
        query.push_back(values[1]); // 模式选择，1字节
        break;
    default:
        zlog_warn(Util::m_zlog, "未识别的命令字");
        break;
    }

    //	for (i = 0; i < values.size(); i++)
    //	//for (i = 0; i < 1; i++)
    //	{
    //		query.push_back(((int) (values[i])) >> 8);
    //		query.push_back(((int) (values[i])) & 0xFF);
    //	}
    crc16 = Check::CheckSum(query);
    query.push_back(crc16); // 校验和
    query.push_back(0x7D);  // 结束帧

    zlog_info(Util::m_zlog, "查询命令");
    do {
        //		printf("下发数据:");
        //		for(int i = 0; i < query.size(); i++)
        //		{
        //			printf("%02x ", query[i]);
        //		}
        //		printf("\n");

        error = m_tcpClient->TxRxMsg(reply, query, 9);

        //		printf("上传数据:");
        //		for(int i = 0; i < reply.size(); i++)
        //		{
        //			printf("%02x ", reply[i]);
        //		}
        //		printf("\n");

        if (error == ErrorInfo::ERR_OK) {
            // 判断应答地址,命令类型,命令是否与查询一致
            if ((reply[0] != 0x7B)) {
                zlog_warn(Util::m_zlog, "反馈数据错误");
                return ErrorInfo::ERR_INV_RESP;
            }
            if (reply[4] == 0x99) {
                // 错误码
                // 0x01 校验和错误
                // 0x02 命令类型错误
                // 0x03 命令字错误
                // 0x04 状态不符（当前的状态不允许执行接收的命令）
                // 0x05 参数错误
                // 0x06 保护报警
                // 0x07 超量程范围
                // 0x08 命令长度错误
                zlog_warn(Util::m_zlog, "查询命令错误，错误码：%d", reply[6]);
                error = ErrorInfo::ERR_INV_RESP;
                break;
            }
            break;
        } else {
            usleep(m_delayTime * 1000);
        }
        retry++;
    } while ((retry < m_retries) && (ErrorInfo::ERR_OK != error));
    return error;
}
