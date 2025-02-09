/*
 * Device.cpp
 *
 *   创建日期: 2013-3-5
 *   作         者: mzh
 */

#include "Device.h"
#include "Db.h"
#include "MemDb.h"

Device::DevType Device::m_devType[128]; // 设备类型数组
int Device::m_devTypeNum = 0;           // 设备类型数量
Device *Device::m_dev    = NULL;
Device::Device() {
    // TODO 构造函数
    m_slaveDevArray.clear();
    m_controllerType = DevControllerType::SLAVE;

    Init();
}

Device::~Device() {
    // TODO 析构函数
    this->Uninit();
}

Device *Device::GetInstance() {
    if (NULL == Device::m_dev) {
        Device::m_dev = new Device();
    }

    return Device::m_dev;
}

void Device::FreeInstanse() {
    if (NULL != Device::m_dev) {
        delete Device::m_dev;
        Device::m_dev = NULL;
    }

    return;
}

bool Device::Init() {
    zlog_info(Util::m_zlog, "配置信息初始化");
    bool ret = false;

    ret = this->InitSlaveDevConfig();
    ret = this->InitMemDb();

    zlog_info(Util::m_zlog, "配置信息初始化%s", ret ? "成功" : "失败");
    return ret;
}

void Device::Uninit() {
    zlog_info(Util::m_zlog, "反初始化");

    // 1、删除从设备数组
    SlaveDev *slaveDev = NULL; // 从设备指针
    PtrArray::iterator iter;

    for (iter = m_slaveDevArray.begin(); iter != m_slaveDevArray.end();
         iter++) {
        slaveDev = (SlaveDev *)(*iter);

        if (NULL != slaveDev) {
            delete slaveDev;
            slaveDev = NULL;
        }
    }

    m_slaveDevArray.clear();

    zlog_info(Util::m_zlog, "反初始化结束");
    return;
}

bool Device::InitMemDb() {
    zlog_info(Util::m_zlog, "初始化设备设置量地址");

    int i     = 0;
    float tmp = 0.0;

    // 存取从设备信息数组
    Device::SlaveDev *slaveDev = NULL;
    PtrArray::iterator slaveIter;

    for (slaveIter = m_slaveDevArray.begin();
         slaveIter != m_slaveDevArray.end(); slaveIter++) {
        slaveDev = reinterpret_cast<Device::SlaveDev *>(*slaveIter);

        switch (slaveDev->devType) {
        case DeviceType::DEVICE_TYPE_DO:
            for (i = slaveDev->regStart;
                 i < slaveDev->regStart + slaveDev->regLen; i++) {
                MemDb::SetRealData(i, 0, false);
            }
            break;
        case DeviceType::DEVICE_TYPE_DCDC:
            for (i = slaveDev->regStart; i < slaveDev->regStart + 6; i++) {
                MemDb::SetRealData(i, 0, false);
            }
            for (i = slaveDev->regStart + 6; i < slaveDev->regStart + 11; i++) {
                MemDb::SetRealData(i, tmp, false);
            }
            break;
        case DeviceType::DEVICE_TYPE_PCS: // PCS
            for (i = slaveDev->regStart; i < slaveDev->regStart + 6; i++) {
                MemDb::SetRealData(i, 0, false);
            }
            for (i = slaveDev->regStart + 6; i < slaveDev->regStart + 14; i++) {
                MemDb::SetRealData(i, tmp, false);
            }
            break;
        case DeviceType::DEVICE_TYPE_SIMU_PV: // 光伏模拟器
            for (i = slaveDev->regStart; i < slaveDev->regStart + 5; i++) {
                MemDb::SetRealData(i, 0, false);
            }
            for (i = slaveDev->regStart + 5; i < slaveDev->regStart + 12; i++) {
                MemDb::SetRealData(i, tmp, false);
            }
            for (i = slaveDev->regStart + 12; i < slaveDev->regStart + 13;
                 i++) {
                MemDb::SetRealData(i, 0, false);
            }
            for (i = slaveDev->regStart + 13; i < slaveDev->regStart + 22;
                 i++) {
                MemDb::SetRealData(i, tmp, false);
            }
            break;
        case DeviceType::DEVICE_TYPE_SIMU_AC_LOAD: // 可编程交流负载
            for (i = slaveDev->regStart; i < slaveDev->regStart + 3; i++) {
                MemDb::SetRealData(i, 0, false);
            }
            for (i = slaveDev->regStart + 3; i < slaveDev->regStart + 16; i++) {
                MemDb::SetRealData(i, tmp, false);
            }
            break;
        case DeviceType::DEVICE_TYPE_SIMU_DC_LOAD: // 可编程直流负载
            for (i = slaveDev->regStart; i < slaveDev->regStart + 6; i++) {
                MemDb::SetRealData(i, 0, false);
            }
            for (i = slaveDev->regStart + 8; i < slaveDev->regStart + 12; i++) {
                MemDb::SetRealData(i, tmp, false);
            }
            for (i = slaveDev->regStart + 12; i < slaveDev->regStart + 13;
                 i++) {
                MemDb::SetRealData(i, 0, false);
            }
            for (i = slaveDev->regStart + 13; i < slaveDev->regStart + 17;
                 i++) {
                MemDb::SetRealData(i, tmp, false);
            }
            for (i = slaveDev->regStart + 17; i < slaveDev->regStart + 18;
                 i++) {
                MemDb::SetRealData(i, 0, false);
            }
            for (i = slaveDev->regStart + 18; i < slaveDev->regStart + 20;
                 i++) {
                MemDb::SetRealData(i, tmp, false);
            }
            for (i = slaveDev->regStart + 20; i < slaveDev->regStart + 21;
                 i++) {
                MemDb::SetRealData(i, 0, false);
            }
            for (i = slaveDev->regStart + 21; i < slaveDev->regStart + 25;
                 i++) {
                MemDb::SetRealData(i, tmp, false);
            }
            for (i = slaveDev->regStart + 25; i < slaveDev->regStart + 26;
                 i++) {
                MemDb::SetRealData(i, 0, false);
            }
            for (i = slaveDev->regStart + 26; i < slaveDev->regStart + 30;
                 i++) {
                MemDb::SetRealData(i, tmp, false);
            }
            for (i = slaveDev->regStart + 30; i < slaveDev->regStart + 31;
                 i++) {
                MemDb::SetRealData(i, 0, false);
            }
            for (i = slaveDev->regStart + 31; i < slaveDev->regStart + 33;
                 i++) {
                MemDb::SetRealData(i, tmp, false);
            }
            for (i = slaveDev->regStart + 33; i < slaveDev->regStart + 34;
                 i++) {
                MemDb::SetRealData(i, 0, false);
            }
            for (i = slaveDev->regStart + 34; i < slaveDev->regStart + 37;
                 i++) {
                MemDb::SetRealData(i, tmp, false);
            }
            for (i = slaveDev->regStart + 37; i < slaveDev->regStart + 39;
                 i++) {
                MemDb::SetRealData(i, 0, false);
            }
            for (i = slaveDev->regStart + 39; i < slaveDev->regStart + 43;
                 i++) {
                MemDb::SetRealData(i, tmp, false);
            }
            for (i = slaveDev->regStart + 43; i < slaveDev->regStart + 44;
                 i++) {
                MemDb::SetRealData(i, 0, false);
            }
            break;
        case DeviceType::DEVICE_TYPE_SIMU_AC_GRID: // 交流模拟电网
            for (i = slaveDev->regStart; i < slaveDev->regStart + 8; i++) {
                MemDb::SetRealData(i, 0, false);
            }
            for (i = slaveDev->regStart + 8; i < slaveDev->regStart + 15; i++) {
                MemDb::SetRealData(i, tmp, false);
            }
            break;
        case DeviceType::DEVICE_TYPE_SINMU_LIB: // 电池模拟器
            for (i = slaveDev->regStart; i < slaveDev->regStart + 8; i++) {
                MemDb::SetRealData(i, 0, false);
            }
            for (i = slaveDev->regStart + 8; i < slaveDev->regStart + 28; i++) {
                MemDb::SetRealData(i, tmp, false);
            }
            break;
        case DeviceType::DEVICE_TYPE_LIION: // 电池
            for (i = slaveDev->regStart; i < slaveDev->regStart + 2; i++) {
                MemDb::SetRealData(i, 0, false);
            }
            for (i = slaveDev->regStart + 2; i < slaveDev->regStart + 5; i++) {
                MemDb::SetRealData(i, tmp, false);
            }
            break;
        case DeviceType::DEVICE_TYPE_DC_LOAD: // 相变储热
            for (i = slaveDev->regStart; i < slaveDev->regStart + 19; i++) {
                MemDb::SetRealData(i, 0, false);
            }
            for (i = slaveDev->regStart + 19; i < slaveDev->regStart + 25;
                 i++) {
                MemDb::SetRealData(i, tmp, false);
            }
            break;
        case DeviceType::DEVICE_TYPE_BIPOLAR_ACDC: // 双极性AC/DC
            for (i = slaveDev->regStart; i < slaveDev->regStart + 5; i++) {
                MemDb::SetRealData(i, 0, false);
            }
            for (i = slaveDev->regStart + 5; i < slaveDev->regStart + 11; i++) {
                MemDb::SetRealData(i, tmp, false);
            }
            break;
        default:
            break;
        }
    }

    return true;
}

bool Device::InitSlaveDevConfig() {
    zlog_info(Util::m_zlog, "设备信息初始化");
    string str;

    // 1、打开配置数据库
    // 局部变量
    ConfigDb *db = (ConfigDb *)ConfigDb::GetInstance(); // 配置数据库实例

    // 1、打开数据库
    if (!db->IsOpen()) {
        if (!db->Open()) {
            zlog_warn(Util::m_zlog, "配置数据库打开失败");
            return false;
        }
    }

    // 2、读取配置信息
    string sql = "select * from [dev_slave] order by sn";

    if (!db->Prepare(sql)) {
        zlog_warn(Util::m_zlog, "预处理语句失败");
        db->Close();
        return false;
    }

    SlaveDev *slaveDev;
    int start = 0;
    int len   = 0;
    m_slaveDevArray.clear();
    int num = 0;

    while (DB_ROW == db->Step()) {
        slaveDev             = new SlaveDev();
        slaveDev->sn         = db->GetColumnInt(0);
        slaveDev->name       = db->GetColumnText(1);
        slaveDev->devType    = db->GetColumnInt(2);
        slaveDev->devManu    = db->GetColumnInt(3);
        slaveDev->devModel   = db->GetColumnInt(4);
        slaveDev->protocol   = db->GetColumnInt(5);
        slaveDev->commType   = db->GetColumnInt(6);
        slaveDev->commNo     = db->GetColumnInt(7);
        slaveDev->ip         = db->GetColumnText(8);
        slaveDev->port       = db->GetColumnInt(9);
        slaveDev->regStart   = db->GetColumnInt(10);
        slaveDev->regLen     = db->GetColumnInt(11);
        slaveDev->slaveAddr  = db->GetColumnText(12);
        slaveDev->rate       = db->GetColumnInt(13);
        slaveDev->sysType    = db->GetColumnInt(14);
        slaveDev->sysTypeNo  = db->GetColumnInt(15);
        slaveDev->subType1   = db->GetColumnInt(16);
        slaveDev->subType1No = db->GetColumnInt(17);
        slaveDev->subType2   = db->GetColumnInt(18);
        slaveDev->subType2No = db->GetColumnInt(19);
        slaveDev->desc       = db->GetColumnText(20);
        slaveDev->own        = db->GetColumnInt(21);
        slaveDev->group      = db->GetColumnInt(22);
        slaveDev->flag       = db->GetColumnInt(23);
        slaveDev->enable     = db->GetColumnInt(24);

        GetMemAddr(start, len, slaveDev->sysType, slaveDev->sysTypeNo,
                   slaveDev->subType1, slaveDev->subType1No, slaveDev->subType2,
                   slaveDev->subType2No);
        slaveDev->regStart = start;
        slaveDev->regLen   = len;

        zlog_warn(Util::m_zlog, "%s   MemDB起始地址为%d,长度为:%d",
                  slaveDev->name.c_str(), slaveDev->regStart, slaveDev->regLen);
        m_slaveDevArray.push_back(slaveDev);
        num++;
    }

    zlog_warn(Util::m_zlog, "读取从设备信息结束,共计读取从设备%d台", num);
    // 关闭游标
    db->Finalize();

    // 读取串口设备信息
    zlog_info(Util::m_zlog, "从comm_com中读取串口设备信息");
    sql = "select * from [comm_com] order by sn";

    if (!db->Prepare(sql)) {
        zlog_warn(Util::m_zlog, "预处理语句失败");
        db->Close();
        return false;
    }

    ComDev *comDev;
    m_comDevArray.clear();
    num = 0;

    while (DB_ROW == db->Step()) {
        comDev           = new ComDev();
        comDev->port     = db->GetColumnInt(1);
        comDev->baudrate = db->GetColumnInt(2);
        comDev->databits = db->GetColumnInt(3);
        comDev->stopbits = db->GetColumnInt(4);
        str              = db->GetColumnText(5);
        if (str.empty()) {
            comDev->parity = 'N';
        } else {
            comDev->parity = str[0];
        }
        m_comDevArray.push_back(comDev);

        zlog_warn(Util::m_zlog, "串口%d,参数=%d，%c,%d,%d", comDev->port,
                  comDev->baudrate, comDev->parity, comDev->databits,
                  comDev->stopbits);
        num++;
    }
    // 3、关闭游标，关闭数据库
    db->Finalize();

    // 读取CAN设备信息
    zlog_info(Util::m_zlog, "从表comm_can中读取CAN设备信息");
    sql = "select * from [comm_can] order by sn";

    if (!db->Prepare(sql)) {
        zlog_warn(Util::m_zlog, "预处理语句失败");
        db->Close();
        return false;
    }

    CanDev *canDev;
    m_canDevArray.clear();
    num = 0;

    while (DB_ROW == db->Step()) {
        canDev           = new CanDev();
        canDev->port     = db->GetColumnInt(1);
        canDev->baudrate = db->GetColumnInt(2);

        m_canDevArray.push_back(canDev);
        num++;
    }

    // 关闭游标
    db->Finalize();

    // 读取通讯类型信息
    zlog_info(Util::m_zlog, "从表const_common_list中读取通讯类型信息");
    sql = "select * from [const_common_list] where type='comm_type' order by "
          "[sn];";

    if (!db->Prepare(sql)) {
        zlog_warn(Util::m_zlog, "预处理语句失败");
        db->Close();
        return false;
    }

    ConstCommType *constCommType;
    m_constCommTypeArray.clear();
    num = 0;

    while (DB_ROW == db->Step()) {
        constCommType       = new ConstCommType();
        constCommType->type = db->GetColumnInt(2);
        constCommType->name = db->GetColumnText(3);
        constCommType->desc = db->GetColumnText(4);
        m_constCommTypeArray.push_back(constCommType);

        zlog_warn(Util::m_zlog, "通讯类型:编号type=%d，类型name=%s,描述desc=%s",
                  constCommType->type, constCommType->name.c_str(),
                  constCommType->desc.c_str());
        num++;
    }

    // 关闭游标
    db->Finalize();

    // 读取控制器类型
    zlog_info(Util::m_zlog, "从表sys_controller中读取控制器类型信息");
    db->GetValue(m_controllerType, "controller_type", "sys_controller");
    db->GetValue(m_controllerAddr, "controller_addr", "sys_controller");

    db->Close();
    return true;
}

void Device::GetMemAddr(int &start, int &len, int sysType, int sysTypeNo,
                        int subType1, int subType1No, int subType2,
                        int subType2No) {
    int addr  = 0;
    int len1  = 0;
    int addr1 = 0;

    GetType1Addr(addr1, len1, sysType, sysTypeNo, subType1, subType1No,
                 subType2, subType2No);

    switch (sysType) {
    case DeviceType::DEVICE_TYPE_MCCS: // 微电网系统
        start = 0;
        len   = 1000;
        break;
    case DeviceType::DEVICE_TYPE_DGS: // 柴油发电机系统
        addr = 1000;
        if (subType1No <= 0) {
            start = addr;
            len   = 200;
        } else {
            start = addr + addr1;
            len   = len1;
        }

        break;
    case DeviceType::DEVICE_TYPE_LOADS: // 负荷系统
        addr = 5000;
        if (subType1No <= 0) {
            start = addr;
            len   = 200;
        } else {
            start = addr + addr1;
            len   = len1;
        }
        break;
    case DeviceType::DEVICE_TYPE_PVS: // 光伏发电系统
        addr = 9000;
        if (subType1No <= 0) {
            start = addr;
            len   = 200;
        } else {
            start = addr + addr1;
            len   = len1;
        }
        break;
    case DeviceType::DEVICE_TYPE_WTS: // 风电系统
        addr = 13000;
        if (subType1No <= 0) {
            start = addr;
            len   = 200;
        } else {
            start = addr + addr1;
            len   = len1;
        }
        break;
    case DeviceType::DEVICE_TYPE_DCS: // 直流系统
        addr = 21000;
        if (subType1No <= 0) {
            start = addr;
            len   = 200;
        } else {
            start = addr + addr1;
            len   = len1;
        }
        break;
    case DeviceType::DEVICE_TYPE_GRID: // 电网系统
        addr = 2000;
        if (subType1No <= 0) {
            start = addr;
            len   = 100;
        } else {
            start = addr + addr1;
            len   = len1;
        }
        break;
    case DeviceType::DEVICE_TYPE_EVN: // 环境监测装置
        addr = 8800;
        if (subType1No <= 0) {
            start = addr;
            len   = 50;
        } else {
            start = addr + addr1;
            len   = len1;
        }
        break;
    default:
        addr = 0;
        break;
    }

    return;
}

void Device::GetType1Addr(int &start, int &len, int sysType, int sysTypeNo,
                          int subType1, int subType1No, int subType2,
                          int subType2No) {
    int addr  = 0;
    int len1  = 0;
    int addr1 = 0;

    GetType2Addr(addr1, len1, sysType, sysTypeNo, subType1, subType1No,
                 subType2, subType2No);

    if (sysType != DeviceType::DEVICE_TYPE_MCCS) {
        if (sysType == DeviceType::DEVICE_TYPE_EVN) {
            addr = 0x00;
        } else {
            // addr = 200 + (subType1No - 1) * 800;
        }
    } else {
        addr = 0;
    }

    start = addr + addr1;
    len   = len1;
    return;
}

void Device::GetType2Addr(int &start, int &len, int sysType, int sysTypeNo,
                          int subType1, int subType1No, int subType2,
                          int subType2No) {
    int addr    = 0;
    int tmp_len = 0;

    if (subType1 != DeviceType::DEVICE_TYPE_DCS) {
        if ((subType2 == DeviceType::DEVICE_TYPE_LIION)      // 锂电池
            || (subType2 == DeviceType::DEVICE_TYPE_SUPCAP)) // 超级电容
        {
            addr    = 400;
            tmp_len = 400;
        } else if ((subType2 == DeviceType::DEVICE_TYPE_DO) ||
                   (subType2 == DeviceType::DEVICE_TYPE_DC_DO) ||
                   (subType2 == DeviceType::DEVICE_TYPE_DO_OPT) ||
                   (subType2 == DeviceType::DEVICE_TYPE_DC_DO_OPT)) {
            addr    = (subType2No - 1);
            tmp_len = 1;
        } else if ((subType2 == DeviceType::DEVICE_TYPE_DI) ||
                   (subType2 == DeviceType::DEVICE_TYPE_DC_DI)) // DI
        {
            addr    = (subType2No - 1) + 100;
            tmp_len = 1;
        } else if (subType2 == DeviceType::DEVICE_TYPE_EVN) {
            addr    = 0x00;
            tmp_len = 100;
        } else if (subType2 == 62) {
            addr    = 200 * (subType2No - 1);
            tmp_len = 200;
        }
        else if (subType2 == DeviceType::DEVICE_TYPE_SIGNAL_MODULE) {
            addr    = 100 * (subType2No - 1)+3200;
            tmp_len = 100;
        } else if (subType2 == 60) {
            addr    = 50 * (subType2No - 1)+6900;
            tmp_len = 50;
        } else if (subType2 == 61) {
            addr    = 10 * (subType2No - 1)+9250;
            tmp_len = 10;
        }else if (subType2 == 63) {
            addr    = 50 * (subType2No - 1)+9650;
            tmp_len = 50;
        }

        //2025-2-8 设备分块化
        else if (subType2 == DeviceType::DEVICE_TYPE_FJ ) {
            addr    = 200 * (subType2No - 1)+2000;
            tmp_len = 200;
        }else if (subType2 == DeviceType::DEVICE_TYPE_CHROMA_GF_61845_61860 ) {
            addr    = 100 * (subType2No - 1)+4000;
            tmp_len = 100;
        }else if (subType2 == DeviceType::DEVICE_TYPE_CHROMA_CN_62180D  ) {
            addr    = 100 * (subType2No - 1)+6000;
            tmp_len = 100;
        }else if (subType2 == DeviceType::DEVICE_TYPE_CHROMA_GF_62180D  ) {
            addr    = 100 * (subType2No - 1)+8000;
            tmp_len = 100;
        }else if (subType2 == DeviceType::DEVICE_TYPE_WB_CN_DCDC  ) {
            addr    = 100 * (subType2No - 1)+10000;
            tmp_len = 100;
        }else if (subType2 == DeviceType::DEVICE_TYPE_WB_GF_DCDC  ) {
            addr    = 100 * (subType2No - 1)+12000;
            tmp_len = 100;
        }else if (subType2 == DeviceType::DEVICE_TYPE_WB_CN_DCAC ) {
            addr    = 200 * (subType2No - 1)+14000;
            tmp_len = 200;
        }else if (subType2 == DeviceType::DEVICE_TYPE_GDW_GF_DCAC  ) {
            addr    = 100 * (subType2No - 1)+16000;
            tmp_len = 100;
        }else if (subType2 == DeviceType::DEVICE_TYPE_WB_120KWPCS  ) {
            addr    = 200 * (subType2No - 1)+18000;
            tmp_len = 200;
        }else if (subType2 == DeviceType::  DEVICE_TYPE_WB_15KWPCS) {
            addr    = 200 * (subType2No - 1)+20000;
            tmp_len = 200;
        }else if (subType2 == DeviceType::DEVICE_TYPE_HC201  ) {
            addr    = 50 * (subType2No - 1)+22000;
            tmp_len = 50;
        }else if (subType2 == DeviceType:: DEVICE_TYPE_HC202 ) {
            addr    = 50 * (subType2No - 1)+24000;
            tmp_len = 50;
        }else if (subType2 == DeviceType:: DEVICE_TYPE_HC221 ) {
            addr    = 10 * (subType2No - 1)+26000;
            tmp_len = 10;
        }else if (subType2 == DeviceType:: DEVICE_TYPE_HC209 ) {
            addr    = 50 * (subType2No - 1)+28000;
            tmp_len = 50;
        }else if (subType2 == DeviceType:: DEVICE_TYPE_HC33B ) {
            addr    = 50 * (subType2No - 1)+30000;
            tmp_len = 50;
        }else if (subType2 == DeviceType:: DEVICE_TYPE_NGI_N35200 ) {
            addr    = 350 * (subType2No - 1)+32000;
            tmp_len = 350;
        }else if (subType2 == DeviceType:: DEVICE_TYPE_HJ_ACLOAD ) {
            addr    = 50 * (subType2No - 1)+34000;
            tmp_len = 50;
        }else if (subType2 == DeviceType:: DEVICE_TYPE_HJ_DCLOAD ) {
            addr    = 50 * (subType2No - 1)+36000;
            tmp_len = 50;
        } 

        else {
            addr    = 100 * (subType2No + 1);
            tmp_len = 100;
        }
    } else {
        if ((subType2 == DeviceType::DEVICE_TYPE_DO) ||
            (subType2 == DeviceType::DEVICE_TYPE_DC_DO) ||
            (subType2 == DeviceType::DEVICE_TYPE_DO_OPT) ||
            (subType2 == DeviceType::DEVICE_TYPE_DC_DO_OPT)) {
            addr    = (subType2No - 1) + 70;
            tmp_len = 1;
        } else if ((subType2 == DeviceType::DEVICE_TYPE_DI) ||
                   (subType2 == DeviceType::DEVICE_TYPE_DC_DI)) // DI
        {
            addr    = (subType2No - 1) + 80;
            tmp_len = 1;
        }

        /* Mingyu
         * CNCU寄存器地址分配
         */
        else if (subType2 == DeviceType::DEVICE_TYPE_CNCU) {
            addr    = (subType2No - 1) * 300;
            tmp_len = 300;
        }
        /*	Mingyu - 2021-03-21
         *	京测直流线路阻抗模拟器分配地址
         */
        else if (subType2 == DeviceType::DEVICE_TYPE_SIMU_CABLE) {
            addr    = (subType2No - 1) * 40;
            tmp_len = 40;
        }
        /*	Mingyu - 2021-03-24
         *	艾诺光伏模拟器分配地址
         */
        else if (subType2 == DeviceType::DEVICE_TYPE_SIMU_PV) {
            addr    = (subType2No - 1) * 100;
            tmp_len = 100;
            // } else if (subType2 == DeviceType::DEVICE_TYPE_XVJI) {
            //     addr    = (subType2No - 1) * 200;
            //     tmp_len = 200;
        } else // 状态量
        {
            addr    = (subType2No - 1) + 90;
            tmp_len = 10;
        }
    }

    start = addr;
    len   = tmp_len;

    return;
}

PtrArray *Device::GetSlaveDevArray() { return &m_slaveDevArray; }

PtrArray *Device::GetComDevArray() { return &this->m_comDevArray; }

PtrArray *Device::GetCanDevArray() { return &this->m_canDevArray; }

PtrArray *Device::GetConstCommTypeArray() { return &m_constCommTypeArray; }

int Device::GetControllerType() { return Device::m_controllerType; }

int Device::GetControllerAddr() { return Device::m_controllerAddr; }
