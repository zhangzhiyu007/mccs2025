/**
 *    文 件       :			DCXG500FaultSimulator.cpp
 *    功 能       :
 *    作 者       :			Mingyu Wu
 *    version     :			1.0.0
 *    日 期       :			2021-03-23
 */

#include "DCXG500FaultSimulator.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../../util/Util.h"
#include "../../com/Com.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"

DCXG500FaultSimulator::DCXG500FaultSimulator() {
    m_isTcp     = false;
    m_com       = NULL;
    m_tcpClient = NULL;
   
}

DCXG500FaultSimulator::~DCXG500FaultSimulator() {}

void DCXG500FaultSimulator::SetIsTcp(bool isTcp) { m_isTcp = isTcp; }

void DCXG500FaultSimulator::SetTcp(TcpClient *client) { m_tcpClient = client; }

void DCXG500FaultSimulator::SetCom(Com *com) { m_com = com; }

int DCXG500FaultSimulator::Read(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    ModbusRtuMaster rtu;
    rtu.SetIsTcp(true);
    rtu.SetTcpClient(m_tcpClient);

    UshortArray values;
    values.clear();
    int addr     = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;
    int start    = 100;
    int length   = 2;
    bool comm    = false;
    int data     = 0;
    bool tmp;

    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        data = (unsigned short)values[0];

        tmp = data & 0x1;
        MemDb::SetRealData(regStart + 1, tmp,
                           false); // 1 短路加载状态，0卸载状态

        tmp = (data >> 1) & 0x1;
        MemDb::SetRealData(regStart + 2, tmp,
                           false); // 1 短路卸载状态，0卸载状态

        tmp = (data >> 2) & 0x1;
        MemDb::SetRealData(regStart + 3, tmp, false); // 备用

        tmp = (data >> 3) & 0x1;
        MemDb::SetRealData(regStart + 4, tmp, false); // 备用

        tmp = (data >> 4) & 0x1;
        MemDb::SetRealData(regStart + 5, tmp, false); // 1=急停，0=无急停

        tmp = (data >> 5) & 0x1;
        MemDb::SetRealData(
            regStart + 6, tmp,
            false); // 1=记载完毕延时40秒不能再次加载，0可以再次加载

        tmp = (data >> 6) & 0x1;
        MemDb::SetRealData(regStart + 7, tmp, false); // 0选择

        tmp = (data >> 7) & 0x1;
        MemDb::SetRealData(regStart + 8, tmp, false); // PE选择

        tmp = (data >> 8) & 0x1;
        MemDb::SetRealData(regStart + 9, tmp,
                           false); // 1=选择正-负短路，0未选择

        tmp = (data >> 9) & 0x1;
        MemDb::SetRealData(regStart + 10, tmp,
                           false); // 1=选择正-0(PE)短路，0未选择

        tmp = (data >> 10) & 0x1;
        MemDb::SetRealData(regStart + 11, tmp,
                           false); // 1=选择负-0(PE)短路，0未选择

        tmp = (data >> 15) & 0x1;
        MemDb::SetRealData(regStart + 12, tmp, false); // 1=正-0(PE)-负

        tmp = (data >> 11) & 0x1;
        MemDb::SetRealData(regStart + 13, tmp, false); // 选用10Ω短路

        tmp = (data >> 12) & 0x1;
        MemDb::SetRealData(regStart + 14, tmp,
                           false); // 1=短路电阻选择5欧姆，0未选择

        tmp = (data >> 13) & 0x1;
        MemDb::SetRealData(regStart + 15, tmp,
                           false); // 1=短路电阻选择2欧姆，0为选择

        tmp = (data >> 14) & 0x1;
        MemDb::SetRealData(regStart + 16, tmp,
                           false); // 1=短路电阻选择0欧姆，0为选择

        data = ((unsigned short)values[1] & 0xFF) +
               (((unsigned short)values[1] >> 8));
        MemDb::SetRealData(regStart + 17, data / 10, false); // 实际加载秒数

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "故障模拟器通讯故障 %s", dev->name.c_str());
    }

    MemDb::SetRealData(regStart + 0, comm, false);
    return ret;
}

int DCXG500FaultSimulator::Preset(Device::SlaveDev *dev) {
    int ret      = ErrorInfo::ERR_OK;
    int addr     = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;
    int start    = 50;
    int data     = 0;
    RealData realData;
    unsigned short bit_1st, bit_2sec, bit_3rd, bit_4th, bit_5th, bit_6th,
        bit_7th, bit_8th, bit_9th, bit_10th, bit_11th, bit_12th, bit_13th,
        bit_14th, bit_15th, bit_16th = 0;
    if (!m_tcpClient->IsOpened())
        return ErrorInfo::ERR_OPENED;

    ModbusRtuMaster rtu;
    rtu.SetIsTcp(true);
    rtu.SetTcpClient(m_tcpClient);

    // bit_1st = 0;
    if (MemDb::GetRealData(realData, regStart + 21, true)) {
        data    = 0;
        bit_1st = realData.pv.data.iValue; // 1=下发短路加载
        data    = bit_1st & 0x1;
        ret     = rtu.PresetSingleRegister(addr, start, data);
    }

    if (MemDb::GetRealData(realData, regStart + 22, true)) {
        data     = 0;
        bit_2sec = realData.pv.data.iValue; // 1=下发短路卸载
        data     = (bit_2sec & 0x1) << 1;
        ret      = rtu.PresetSingleRegister(addr, start, data);
    }

    if (MemDb::GetRealData(realData, regStart + 23, true)) {
        data    = 0;
        bit_3rd = realData.pv.data.iValue; // 备用
        data    = (bit_3rd & 0x1) << 2;
        ret     = rtu.PresetSingleRegister(addr, start, data);
    }

    if (MemDb::GetRealData(realData, regStart + 24, true)) {
        data    = 0;
        bit_4th = realData.pv.data.iValue; // 备用
        data    = (bit_4th & 0x1) << 3;
        ret     = rtu.PresetSingleRegister(addr, start, data);
    }

    if (MemDb::GetRealData(realData, regStart + 25, true)) {
        data    = 0;
        bit_5th = realData.pv.data.iValue; // 1=急停
        data    = (bit_5th & 0x1) << 4;
        ret     = rtu.PresetSingleRegister(addr, start, data);
    }

    if (MemDb::GetRealData(realData, regStart + 26, true)) {
        data = 0;
        bit_6th = realData.pv.data.iValue; // 1=记载完毕延时40秒不能再次加载
        data = (bit_6th & 0x1) << 5;
        ret  = rtu.PresetSingleRegister(addr, start, data);
    }
    if (MemDb::GetRealData(realData, regStart + 27, true)) {
        data    = 0;
        bit_7th = realData.pv.data.iValue; // 1=选择正-负短路
        data    = (bit_7th & 0x1) << 6;
        ret     = rtu.PresetSingleRegister(addr, start, data);
    }

    if (MemDb::GetRealData(realData, regStart + 28, true)) {
        data    = 0;
        bit_8th = realData.pv.data.iValue; // 1=选择正-0(PE)短路
        data    = (bit_8th & 0x1) << 7;
        ret     = rtu.PresetSingleRegister(addr, start, data);
    }

    if (MemDb::GetRealData(realData, regStart + 29, true)) {
        data    = 0;
        bit_9th = realData.pv.data.iValue; // 1=选择负-0(PE)短路
        data    = (bit_9th & 0x1) << 8;
        ret     = rtu.PresetSingleRegister(addr, start, data);
    }
    if (MemDb::GetRealData(realData, regStart + 30, true)) {
        data     = 0;
        bit_10th = realData.pv.data.iValue; // 1=选择正-负短路
        data     = (bit_10th & 0x1) << 9;
        ret      = rtu.PresetSingleRegister(addr, start, data);
    }
    if (MemDb::GetRealData(realData, regStart + 31, true)) {
        data     = 0;
        bit_11th = realData.pv.data.iValue; // 1=选择正-0(PE)短路
        data     = (bit_11th & 0x1) << 10;
        ret      = rtu.PresetSingleRegister(addr, start, data);
    }
    if (MemDb::GetRealData(realData, regStart + 32, true)) {
        data     = 0;
        bit_12th = realData.pv.data.iValue; // 1=选择负-0(PE)短路
        data     = (bit_12th & 0x1) << 11;
        ret      = rtu.PresetSingleRegister(addr, start, data);
    }

    // if( MemDb::GetRealData(realData, regStart + 33, true) ){
    // 	bit_13th = realData.pv.data.iValue; // 1=短路电阻选择10欧姆

    // }
    // if( MemDb::GetRealData(realData, regStart + 34, true) ){
    // 	bit_13th = realData.pv.data.iValue ; // 1=短路电阻选择5欧姆
    // 	bit_12th = 0;
    // 	bit_14th = 0;
    // 	bit_15th = 0;
    // }

    // if( MemDb::GetRealData(realData, regStart + 35, true) ){
    // 	bit_14th = realData.pv.data.iValue; // 1=短路电路选择2欧姆
    // 	bit_12th = 0;
    // 	bit_13th = 0;
    // 	bit_15th = 0;
    // }
    // if( MemDb::GetRealData(realData, regStart + 36, true) ){
    // 	bit_15th = realData.pv.data.iValue; // 1=短路电阻选择0欧姆
    // 	bit_12th = 0;
    // 	bit_13th = 0;
    // 	bit_14th = 0;
    // }

    // if (MemDb::GetRealData(realData, regStart + 36, true)) {
    //     data = ((bit_3rd & 0x1) << 2) + ((bit_4th & 0x1) << 3) +
    //            ((bit_5th & 0x1) << 4) + ((bit_6th & 0x1) << 5) +
    //            ((bit_7th & 0x1) << 6) + ((bit_8th & 0x1) << 7) +
    //            ((bit_9th & 0x1) << 8) + ((bit_10th & 0x1) << 9) +
    //            ((bit_11th & 0x1) << 10) + ((bit_12th & 0x1) << 11) +
    //            ((bit_13th & 0x1) << 12) + ((bit_14th & 0x1) << 13) +
    //            ((bit_15th & 0x1) << 14) + ((bit_16th & 0x1) << 15);
    //     // zlog_warn(Util::m_zlog, "故障模拟器data: %d, bit1: %d, %d, %d, %d" data,bit_1st, bit_2sec,bit_3rd, bit_4th);
    //     ret = rtu.PresetSingleRegister(addr, start + 0, data);
    // }

    if (MemDb::GetRealData(realData, regStart + 37, true)) {
        data = (realData.pv.data.iValue * 10) & 0xFF;
        rtu.PresetSingleRegister(addr, start + 5, data);
    }

    msleep(300);

    return ret;
}
