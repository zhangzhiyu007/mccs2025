/*
 * CanThread.h
 *
 *   创建日期: 2013-4-8
 *   作         者: mzh
 */

#ifndef CANTHREAD_H_
#define CANTHREAD_H_
#include "../../etc/socket/Thread.h"
#include "Can.h"
#include "../../db/Device.h"
#include "../device/bms/BMS.h"

class CanThreads;

/*名称:CanThread
 *描述:用于通过CAN口读取从设备数据
 *     每个CAN口产生一个CAN口线程
 *用法:
 *创建人:mzh
 *创建日期:2013-4-8
 */
class CanThread: public Thread
{
public:
	CanThread();
	virtual ~CanThread();

	/*
	 *  SetCan
	 *  功能:保存CAN口设备指针
	 *  输入：
	 *  canDev        :[in]Can* CAN口设备指针
	 *  输出：无
	 */
	void SetCan(Can* can);

	/*
	 *  SetCanThreads
	 *  功能:保存CAN口线程池指针
	 *  输入：
	 *  canThreads    :[in]canThreads* CAN线程池指针
	 *  输出：无
	 */
	void SetCanThreads(CanThreads* canThreads);

	/*
	 *  SetCanDev
	 *  功能:保存CAN口设备指针
	 *  输入：
	 *  canDev        :[in]Device::CanDev CAN口设备指针
	 *  输出：无
	 */
	void SetCanDev(Device::CanDev* canDev);
protected:
private:
	/*
	 *  Run
	 *  功能: 线程执行函数
	 *  该线程内运行CAN口数据采集程序
	 *  输入：无
	 *  输出：无
	 */
    virtual void Run();

	//变量
public:
protected:
private:

	//CAN口设备配置信息指针
	Device::CanDev* m_canDev;

	//线程父类指针
	CanThreads* m_canThreads;

	//CAN口指针
	Can* m_can;
};

#endif /* CANTHREAD_H_ */
