/**
 * @file GoodWe.cpp
 * @Synopsis GOODWE SDT 8kW  光伏逆变器
 * @author Mingyu Wu
 * @version 1.0
 * @date 2023-12-15
 */

#include "./GoodWe.h"

GoodWe::GoodWe() {
    m_isTcp     = false;
    m_com       = NULL;
    m_tcpClient = NULL;
}
GoodWe::~GoodWe() {
    m_tcpClient = NULL;
    m_com       = NULL;
}
void GoodWe::SetIsTcp(bool isTcp) { m_isTcp = isTcp; }
void GoodWe::SetTcp(TcpClient *tcpClient) { this->m_tcpClient = tcpClient; }
void GoodWe::SetCom(Com *com) { this->m_com = com; }

int GoodWe::Read(Device::SlaveDev *dev) {
    int ret      = ErrorInfo::ERR_OK;
    int addr     = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;
    int start    = 544;
    int length   = 10;
    bool comm    = false;
    int i_data   = 0;
    float f_data = 0.0f;

    UshortArray values;
    values.clear();

    ModbusRtuMaster rtu;
    rtu.SetIsTcp(true);
    rtu.SetTcpClient(this->m_tcpClient);
    // rtu.SetCom(m_com);

    // 30130 0x75B2 Faultcode  故障代码  RO U32 2 1 N/A [0, 0] 详见Table8-2
    values.clear();
    start  = 30130;
    length = 2;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm   = true;
        i_data = (values[0] << 16) + values[1];
        MemDb::SetRealData(regStart + 1, i_data); // 错误代码
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 读取数据失败", dev->name.c_str());
    }
    // 30145 0x75C1 Gross ACenergy 总发电量RO U32 2 10 kWh [0,0]
    values.clear();
    start  = 30145;
    length = 2;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm   = true;
        f_data = (values[0] << 16) + values[1];
        MemDb::SetRealData(regStart + 2, f_data); // 总发电量
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 读取数据失败", dev->name.c_str());
    }
    // 30147 0x75C3 Total generationtime 总发电时间 RO U32 2 1 H [0,0]
    values.clear();
    start  = 30147;
    length = 2;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm   = true;
        i_data = (values[0] << 16) + values[1];
        MemDb::SetRealData(regStart + 3, i_data); // 发电小时
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 读取数据失败", dev->name.c_str());
    }
    // 30103 0x7597 PV1 voltage PV1 电压 RO U16 1 10 V [0, 0]
    // 30105 0x7599 PV2 voltage PV2 电压 RO U16 1 10 V [0, 0]
    // 30106 0x759A PV2 current PV2 电流 RO U16 1 10 A [0, 0]

    values.clear();
    start  = 30105;
    length = 1;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm   = true;
        f_data = values[0] * 0.1;
        MemDb::SetRealData(regStart + 4, f_data); // 1支路光伏侧电压
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 读取数据失败", dev->name.c_str());
    }
    // 30104 0x7598 PV1 current PV1 电流 RO U16 1 10 A [0, 0]
    values.clear();
    start  = 30106;
    length = 1;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm   = true;
        f_data = values[0] * 0.1;
        MemDb::SetRealData(regStart + 5, f_data); // 1支路光伏侧电流
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 读取数据失败", dev->name.c_str());
    }

    // values.clear();
    // start  = 544;
    // length = 10;
    // ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    // if (ErrorInfo::ERR_OK == ret) {
    //     comm = true;
    //     // clang-format off
    //     i_data = (values[0]<< 16) + values[1]; MemDb::SetRealData(regStart +
    //     1, i_data); //错误代码 
    //     f_data = ((values[2] << 16) + values[3]) *0.1;               
    //     MemDb::SetRealData(regStart + 2, f_data); //总发电量 
    //     i_data = (ushort)((values[4] << 16) + values[5]);
    //     MemDb::SetRealData(regStart + 3, i_data); // 发电小时 
    //     f_data =values[6] * 0.1;               
    //     MemDb::SetRealData(regStart + 4,f_data); // 1支路光伏侧电压 
    //     f_data = values[8] * 0.1;
    //     MemDb::SetRealData(regStart + 5, f_data); // 1支路光伏侧电流
    //     // clang-format on
    // } else {
    //     comm = false;
    //     zlog_warn(Util::m_zlog, "%s 读取数据失败", dev->name.c_str());
    // }

    // 30118 0x75A6 Phase voltage AN 电网 A 相电压 RO U16 1 10 V [0, 0]
    // 30119 0x75A7 Phase voltage BN 电网 B 相电压 RO U16 1 10 V [0, 0]
    // 单相机型不支持 
    // 30120 0x75A8 Phase voltage CN 电网 C 相电压 RO U16 1 10 V
    // [0, 0] 单相机型不支持 30121 0x75A9 Phase A current 电网 A 相电流 RO U16 1
    // 10 A [0, 0] 30122 0x75AA Phase B current 电网 B 相电流 RO U16 1 10 A [0,
    // 0] 单相机型不支持 30123 0x75AB Phase C current 电网 C 相电流 RO U16 1 10
    // A [0, 0] 单相机型不支持 30124 0x75AC Phase A frequency 电网A相频率 RO U16
    // 1 100 Hz [0,0] 30125 0x75AD Phase A frequency 电网B相频率 RO U16 1 100 Hz
    // [0,0] 单相机型不支持 30126 0x75AE Phase A frequency 电网C相频率 RO U16 1
    // 100 Hz [0,0] 单相机型不支持
    values.clear();
    start  = 30118;
    length = 9;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        // clang-format off
        f_data = values[0] * 0.1;               MemDb::SetRealData(regStart + 6, f_data); // A相电压
        f_data = values[1] * 0.1;               MemDb::SetRealData(regStart + 7, f_data); // B相电压
        f_data = values[2] * 0.1;               MemDb::SetRealData(regStart + 8, f_data); // C相电压
        f_data = values[3] * 0.1;               MemDb::SetRealData(regStart + 9, f_data); // A相电流
        f_data = values[4] * 0.1;               MemDb::SetRealData(regStart + 10, f_data); // B相电流
        f_data = values[5] * 0.1;               MemDb::SetRealData(regStart + 11, f_data); // C相电流
        f_data = values[6] * 0.01;               MemDb::SetRealData(regStart + 12, f_data); // A相频率
        f_data = values[7] * 0.01;               MemDb::SetRealData(regStart + 13, f_data); // B相频率
        f_data = values[8] * 0.01;               MemDb::SetRealData(regStart + 14, f_data); // C相频率
        // clang-format on
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 读取数据失败", dev->name.c_str());
    }
    // 30127 0x75AF ACactivepower AC 有功功率 RO U32 2 1000 kW [0,0] AC有功功率
    values.clear();
    // start  = 30127;
    // length = 2;

    start  = 563;
    length = 1;

    ret = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm = true;
        // i_data = (values[0] << 16) + values[1];
        i_data = values[0];
        MemDb::SetRealData(regStart + 15, i_data); // 当前输出功率
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 读取数据失败", dev->name.c_str());
    }
    // 30129 0x75B1 Inverter present status 逆变器状态 RO U16 1 1 N/A [0,0]
    // 0:等待1:并网2:故障3:离网4:自检
    values.clear();
    start  = 30129;
    length = 1;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm   = true;
        i_data = values[0];
        if (i_data == 0) {
            MemDb::SetRealData(regStart + 24, 1); // 等待
            MemDb::SetRealData(regStart + 25, 0);
            MemDb::SetRealData(regStart + 26, 0);
        } else if (i_data == 1) {
            MemDb::SetRealData(regStart + 25, 1); // 并网
            MemDb::SetRealData(regStart + 24, 0);
            MemDb::SetRealData(regStart + 26, 0);
        } else if (i_data == 2) {
            MemDb::SetRealData(regStart + 26, 1); // 故障
            MemDb::SetRealData(regStart + 24, 0);
            MemDb::SetRealData(regStart + 25, 0);
        }
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 读取数据失败", dev->name.c_str());
    }
    // 30141 0x75BD Cabinet Temperature 腔体温度 RO S16 1 10 ℃ [0,0]
    values.clear();
    start  = 30141;
    length = 1;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm   = true;
        f_data = values[0] * 0.1;
        MemDb::SetRealData(regStart + 17, f_data); // 逆变器内部温度
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 读取数据失败", dev->name.c_str());
    }
    // 30144 0x75C0 Daily ACenergy 当日发电量 RO U16 1 10 kWh [0,0]
    values.clear();
    start  = 30144;
    length = 1;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm   = true;
        f_data = values[0] * 0.1;
        MemDb::SetRealData(regStart + 18, f_data); // 当日发电量
    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "%s 读取数据失败", dev->name.c_str());
    }

    // values.clear();
    // start  = 554;
    // length = 13;
    // ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    // if (ErrorInfo::ERR_OK == ret) {
    //     comm = true;
    //     // clang-format off
    //     f_data = values[0] * 0.1;               MemDb::SetRealData(regStart +
    //     6, f_data); // A相电压 f_data = values[1] * 0.1;
    //     MemDb::SetRealData(regStart + 7, f_data); // B相电压 f_data =
    //     values[2] * 0.1;               MemDb::SetRealData(regStart + 8,
    //     f_data); // C相电压 f_data = values[3] * 0.1;
    //     MemDb::SetRealData(regStart + 9, f_data); // A相电流 f_data =
    //     values[4] * 0.1;               MemDb::SetRealData(regStart + 10,
    //     f_data); // B相电流 f_data = values[5] * 0.1;
    //     MemDb::SetRealData(regStart + 11, f_data); // C相电流 f_data =
    //     values[6] * 0.01;               MemDb::SetRealData(regStart + 12,
    //     f_data); // A相频率 f_data = values[7] * 0.01;
    //     MemDb::SetRealData(regStart + 13, f_data); // B相频率 f_data =
    //     values[8] * 0.01;               MemDb::SetRealData(regStart + 14,
    //     f_data); // C相频率 i_data = values[9]; MemDb::SetRealData(regStart +
    //     15, i_data); // 当前输出功率 i_data = values[10];
    //     MemDb::SetRealData(regStart + 16, i_data); // 工作模式 if (i_data==0)
    //     {
    //         MemDb::SetRealData(regStart + 24, 1);//等待
    //         MemDb::SetRealData(regStart + 25, 0);
    //         MemDb::SetRealData(regStart + 26, 0);
    //     }else if (i_data==1)
    //     {
    //         MemDb::SetRealData(regStart + 25, 1);//正常
    //         MemDb::SetRealData(regStart + 24, 0);
    //         MemDb::SetRealData(regStart + 26, 0);
    //     }else if (i_data==2)
    //     {
    //         MemDb::SetRealData(regStart + 26, 1);//故障
    //         MemDb::SetRealData(regStart + 24, 0);
    //         MemDb::SetRealData(regStart + 25, 0);
    //     }

    //     f_data = values[11] * 0.1; MemDb::SetRealData(regStart + 17, f_data);
    //     // 逆变器内部温度 f_data = values[12] * 0.1;
    //     MemDb::SetRealData(regStart + 18, f_data); // 当日发电量

    //     // clang-format on
    //     comm = true;
    // } else {
    //     comm = false;
    //     zlog_warn(Util::m_zlog, "< %s >读取数据失败", dev->name.c_str());
    // }

    // 30130 0x75B2 Faultcode  故障代码  RO U32 2 1 N/A [0, 0] 详见Table8-2
    values.clear();
    start  = 30130;
    length = 2;
    ret    = rtu.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret) {
        comm   = true;
        i_data = (values[0] << 16) + values[1];
        // 提取Bit11 2048 0x00000800DC Bus High 母线电压高
        if (i_data & 0x00000800) {
            MemDb::SetRealData(regStart + 20, 1); // 母线电压高
        } else {
            MemDb::SetRealData(regStart + 20, 0); // 母线电压正常
        }
        // 提取 Bit13 8192 0x00002000  Overtemp. 过温保护
        if (i_data & 0x00002000) {
            MemDb::SetRealData(regStart + 19, 1); // 过温保护
        } else {
            MemDb::SetRealData(regStart + 19, 0); // 未过温保护
        }

    } else {
        comm = false;
        zlog_warn(Util::m_zlog, "< %s >读取数据失败", dev->name.c_str());
    }
    MemDb::SetRealData(regStart + 0, comm);
    return ret;
}

int GoodWe::Preset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    RealData realData;
    int addr     = atoi(dev->slaveAddr.c_str());
    int regStart = dev->regStart;

    UshortArray array;
    array.clear();

    ModbusRtuMaster rtu;
    rtu.SetIsTcp(true);
    rtu.SetTcpClient(this->m_tcpClient);

    // 40330 0x9D8A Power On 开机(允许并网自检) RW U16 1 1 N/A [0, 0] 写 0 生效
    // 40331 0x9D8B Power Off 关机(不允许并网自检)RW U16 1 1 N/A [0, 0] 写 0
    // 生效 开机
    if (MemDb::GetRealData(realData, regStart + 21, true)) {
        if (realData.pv.data.iValue == 1) {
            array.clear();
            array.push_back(0);
            // rtu.PresetMultipleRegisters(addr, 0x2C1E, 2, array);
            rtu.PresetSingleRegister(addr, 288, 0);
            rtu.PresetMultipleRegisters(addr, 40330, 2, array);
            zlog_warn(Util::m_zlog, "< %s > 开机", dev->name.c_str());
        } else {
            array.clear();
            array.push_back(0);
            rtu.PresetSingleRegister(addr, 289, 0);
            rtu.PresetMultipleRegisters(addr, 40331, 2, array);
            zlog_warn(Util::m_zlog, "< %s > 关机", dev->name.c_str());
        }
    }
    // 40332 0x9D8C Restart 重启 WO U16 1 1 N/A [0, 0] 写 0 生效
    // 重启
    if (MemDb::GetRealData(realData, regStart + 22, true)) {
        if (realData.pv.data.iValue == 1) {
            array.clear();
            array.push_back(realData.pv.data.iValue);
            rtu.PresetSingleRegister(addr, 290, 0);
            rtu.PresetMultipleRegisters(addr, 40332, 2, array);
            zlog_warn(Util::m_zlog, "< %s > 重启", dev->name.c_str());
        }
    }
    // 40480 0x9E20 Active Controladjust 有功调节量 RW U16 1 1 % [0,100] 对应
    // 0%-100%有功功率

    if (MemDb::GetRealData(realData, regStart + 23, true)) {
        int data = realData.pv.data.iValue;
        if (data >= 0 && data <= 100) {
            array.clear();
            array.push_back(realData.pv.data.iValue);
            // rtu.PresetSingleRegister(addr, 256, data);
            rtu.PresetMultipleRegisters(addr, 256, 1, array);
            zlog_warn(Util::m_zlog, "< %s > 有功调节量 %d", dev->name.c_str(),
                      data);
        }
    }

    return ret;
}
