/*
 * PvVolatility.cpp
 *
 *  创建日期: 2019-8-4
 *  作          者: 刘海涛
 */

#include "PvVolatility.h"

PvVolatility::PvVolatility() {
    // TODO Auto-generated constructor stub
    m_num = 1;
    m_pvArray.clear();
}

PvVolatility::~PvVolatility() {
    // TODO Auto-generated destructor stub
}

void PvVolatility::Run() {
    zlog_warn(Util::m_zlog, "开启波动率统计线程");
    int setCtrl    = 0;
    int runState   = 0;
    int pvRunState = 0;

    while (true) {
        if (m_state == Thread::STOPPED) {
            zlog_warn(Util::m_zlog, "退出读取数据库线程");
            break;
        }

        //获取系统运行状态
        MemDb::GetRealData(setCtrl, 209);
        MemDb::GetRealData(runState, 200);
        MemDb::GetRealData(pvRunState, 22313);
        zlog_warn(Util::m_zlog, "PV读取数据库线程");

        usleep(5000 * 1000);
    }
}

int PvVolatility::calculatPvVolatility() {
    int ret          = ErrorInfo::ERR_OK;
    float pvPower    = 0.0;
    float battPower  = 0.0;
    float smoth      = 0.0; //平抑后结果
    float averValue  = 0.0; //平均值
    float sumValue   = 0.0;
    float sumSquares = 0.0;
    float sumSqrt    = 0.0;
    float tmp        = 0;
    float volatility = 0.0;

    //获取光伏DCDC高压侧功率
    MemDb::GetRealData(pvPower, 22323);

    //获取储能DCDC高压侧功率
    MemDb::GetRealData(battPower, 26323);

    smoth = (pvPower + battPower) * 0.001;

    if ((m_num > 0) && (m_num <= 10)) {
        m_pvArray.push_back(smoth);
    } else if (m_num > 10) {
        m_pvArray.erase(m_pvArray.begin());
        m_pvArray.push_back(smoth);
        m_num = 11;
    }

    //计算平均值
    for (unsigned int i = 0; i < m_pvArray.size(); i++) {
        sumValue += m_pvArray.at(i);
    }

    averValue = sumValue / m_pvArray.size();

    //计算平方和
    for (unsigned int i = 0; i < m_pvArray.size(); i++) {
        sumSquares += pow((averValue - m_pvArray.at(i)), 2);
    }

    //平方和开平方
    sumSqrt = sqrt(sumSquares);
    tmp     = sumSqrt / averValue;

    volatility = (tmp / m_pvArray.size()) * 100;
    smoth      = smoth * 1000.0;
    MemDb::SetRealData(243, smoth, false);
    MemDb::SetRealData(244, volatility, false);

    m_num++;

    return ret;
}
