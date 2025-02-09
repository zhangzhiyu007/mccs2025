/*
 * PvVolatility.h
 *
 *  创建日期: 2019-8-4
 *  作          者: 刘海涛
 */

#ifndef PVVOLATILITY_H_
#define PVVOLATILITY_H_

#include "../etc/socket/Thread.h"
#include "../util/Util.h"
#include "Device.h"
#include "MemDb.h"

class SysCtrl;

class PvVolatility: public Thread
{
public:
	PvVolatility();
	virtual ~PvVolatility();

	/*
	 * 名称: calculatPvVolatility
	 * 功能: 计算光伏波动率数据
	 * 输入:
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int calculatPvVolatility();

protected:
private:
	/*
	 *  Run
	 *  功能: 线程执行函数
	 *  该线程内运行串口数据采集程序
	 *  输入：无
	 *  输出：无
	 */
	virtual void Run();

protected:
private:
	int m_num;
	FloatArray m_pvArray;
};

#endif /* PVVOLATILITY_H_ */
