/*
 * MemDb.cpp
 *
 *   创建日期: 2013-9-30
 *   作          者: 马中华
 */

#include "MemDb.h"

#define MAX_LENGTH 0xFFFF
#define REAL_DATA_ID 0x44
#define WARN_DATA_ID 0x55

RealData *MemDb::m_realData = NULL; // 实时数据表，
int MemDb::m_realDataId     = -1;   //
pthread_mutex_t MemDb::m_mutexRealData;

#define WRITED_NUM 5
MemDb::MemDb() {
    // TODO 构造函数
}

MemDb::~MemDb() {
    // TODO 析构函数
}

bool MemDb::Init() {
    zlog_info(Util::m_zlog, "内存共享数据库初始化");
    bool ret = false;

    // 1、初始化内存共享
    ret = OpenMem();

    zlog_info(Util::m_zlog, "内存共享数据库初始化%s", ret ? "成功" : "失败");
    return ret;
}

void MemDb::Uninit() {
    zlog_info(Util::m_zlog, "内存共享数据库反初始化");
    // Util::DelMem(m_realDataId);//释放内存共享
    if (NULL != m_realData) {
        delete[] m_realData;
        m_realData = NULL;
    }

    // 销毁互斥变量
    pthread_mutex_destroy(&m_mutexRealData);
    return;
}

bool MemDb::OpenMem() {
    zlog_info(Util::m_zlog, "内存共享数据库:开启内存共享");
    bool ret = false;

    // 采用内存共享时采用
    //	RealData* tmp = NULL;
    //	int shmid = -1;
    //	int id = 0;
    //
    //	//开启实时数据内存共享
    //	id = REAL_DATA_ID;
    //	ret = Util::CreateMem((void**)&tmp, shmid, id, sizeof(RealData) *
    //MAX_LENGTH); 	if (ret)
    //	{
    //		m_realData = (RealData*)tmp;
    //		zlog_info(Util::m_zlog,"id = %d",id);
    //		m_realDataId = id;
    //		zlog_info(Util::m_zlog, "内存共享数据库开启");
    //	}
    //	else
    //	{
    //		zlog_warn(Util::m_zlog,
    //"内存共享数据库开启实时数据内存共享失败"); 		return false;
    //	}
    //
    //	//打开互斥
    //	ret = Util::InitMutex(&MemDb::m_mutexRealData);
    //	if (!ret)
    //	{
    //		zlog_warn(Util::m_zlog, "内存共享数据库开启实时数据互斥失败");
    //		return false;
    //	}

    // 开启数据共享
    if (NULL == m_realData) {
        m_realData = new RealData[MAX_LENGTH];
        memset(m_realData, 0, sizeof(RealData) * MAX_LENGTH);
    }
    // 互斥初始化
    pthread_mutex_init(&m_mutexRealData, NULL);

    // 初始化数据
    for (int i = 0; i < MAX_LENGTH; i++) {
        m_realData[i].addr       = i;
        m_realData[i].readed     = false;
        m_realData[i].writed     = false;
        m_realData[i].pv.type    = RealData::INT;
        m_realData[i].writed_num = 0;
    }

    return true;
}

bool MemDb::GetRealData(RealData &data, int addr) {
    if (!CheckAddr(addr)) {
        zlog_warn(Util::m_zlog, "内存共享数据库实时数据地址越限");
        return false;
    }

    Lock();
    memcpy(&data, &m_realData[addr], sizeof(RealData));
    Unlock();

    return true;
}

bool MemDb::GetRealData(int &data, int addr) {
    bool ret = false;
    RealData realData;
    ret = GetRealData(realData, addr);
    if (ret) {
        if (realData.pv.type == RealData::INT) {
            data = realData.pv.data.iValue;
        } else {
            ret = false;
        }
    }
    return ret;
}

bool MemDb::GetRealData(float &data, int addr) {
    bool ret = false;
    RealData realData;
    ret = GetRealData(realData, addr);
    if (ret) {
        if (realData.pv.type == RealData::DB) {
            data = realData.pv.data.dbValue;
        } else {
            data = realData.pv.data.iValue;
        }
    }
    return ret;
}
bool MemDb::GetRealData(RealData &data, int addr, bool clearWrited) {
    return GetRealDataWrited(data, addr, clearWrited);
}

bool MemDb::GetRealDataWrited(RealData &data, int addr, bool clearWrited) {
    bool ret = false;
    if (!CheckAddr(addr)) {
        zlog_warn(Util::m_zlog, "内存共享数据库实时数据地址%d越限", addr);
        return ret;
    }

    Lock();
    if (m_realData[addr].writed) {
        memcpy(&data, &m_realData[addr], sizeof(RealData));
        if (clearWrited) {
            m_realData[addr].writed = false;
        }
        ret = true;
    }
    Unlock();

    return ret;
}

bool MemDb::GetRealDataWrited(int addr) {
    bool ret = false;
    if (!CheckAddr(addr)) {
        zlog_warn(Util::m_zlog, "内存共享数据库实时数据地址越限");
        return ret;
    }

    Lock();
    ret = m_realData[addr].writed;
    Unlock();

    return ret;
}

bool MemDb::GetRealData(RealData *data, int num, int addr, int length) {
    if ((NULL == data) || (num <= 0) || (num < length) || (!CheckAddr(addr)) ||
        (!CheckAddr(addr + length))) {
        return false;
    }

    int i = 0;
    if (m_realData[addr].writed) {
        for (i = 0; i < length; i++) {
            GetRealData(data[i], (addr + i));
        }
    }
    return true;
}

bool MemDb::SetRealDataUnknown(int addr, int data, bool writed) {
    bool ret = false;
    RealData realData;
    ret = GetRealData(realData, addr);
    if (ret) {
        if (realData.pv.type == RealData::DB) {
            ret = SetRealData(addr, (float)data, writed);
        } else {
            ret = SetRealData(addr, data, writed);
        }
    }

    return ret;
}

bool MemDb::SetRealData(int addr, int data) {
    return SetRealData(addr, data, false);
}

bool MemDb::SetRealData(int addr, float data) {
    return SetRealData(addr, data, false);
}

bool MemDb::SetRealData(int addr, int data, bool writed) {
    RealData realData;
    realData.addr = addr;
    // time(&realData.now);
    realData.pv.type             = RealData::INT;
    realData.pv.data_last.iValue = realData.pv.data.iValue;
    realData.pv.data.iValue      = data;
    realData.readed              = !writed;
    realData.writed              = writed;

    return SetRealData(addr, realData);
}

bool MemDb::SetRealData(int addr, float data, bool writed) {
    RealData realData;
    realData.addr = addr;
    // time(&realData.now);
    realData.pv.type              = RealData::DB;
    realData.pv.data_last.dbValue = realData.pv.data.dbValue;
    realData.pv.data.dbValue      = data;
    realData.readed               = !writed;
    realData.writed               = writed;

    return SetRealData(addr, realData);
}

bool MemDb::SetRealData(int addr, RealData &data) {
    if (!CheckAddr(addr)) {
        zlog_warn(Util::m_zlog, "内存共享数据库实时数据地址越限");
        return false;
    }

    Lock();
    // memcpy(&m_realData[addr],&data,sizeof(RealData));
    if ((m_realData[addr].writed) && (!data.writed) &&
        (m_realData[addr].writed_num < WRITED_NUM)) {
        m_realData[addr].writed_num++;
    } else {
        memcpy(&m_realData[addr], &data, sizeof(RealData));
        m_realData[addr].writed_num = 0;
    }

    Unlock();

    return true;
}

bool MemDb::SetRealDataBit(int addr, int bit, bool data) {
    int value    = 0;
    bool success = MemDb::GetRealData(value, addr);
    if (success) {
        Util::SetBitValue(value, bit, data);
        success = MemDb::SetRealData(addr, value);
    }

    return success;
}

bool MemDb::CheckAddr(int addr) {
    if ((addr >= 0) && (addr < MAX_LENGTH)) {
        return true;
    }

    return false;
}

void MemDb::Lock() {
    // 给互斥体变量加锁
    pthread_mutex_lock(&m_mutexRealData);

    // 内存共享时采用
    // Util::Lock(&m_mutexRealData);
}

void MemDb::Unlock() {
    // 给互斥体变量解除锁
    pthread_mutex_unlock(&m_mutexRealData);

    // 内存共享时采用
    // Util::Unlock(&m_mutexRealData);
}
