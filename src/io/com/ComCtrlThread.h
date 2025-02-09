/*
 * ComCtrlThread.h
 *
 *   创建日期: 2013-6-25
 *   作         者: mzh
 */

#ifndef COMCTRLTHREAD_H_
#define COMCTRLTHREAD_H_

#include "../../etc/socket/Thread.h"
#include "../../util/Util.h"
#include "../../db/Device.h"
#include "Com.h"

class ComThreads;

/*名称:ComCtrlThread
 *描述:用于串口设备控制
 *用法:
 *创建人:mzh
 *创建日期:2013-6-25
 */
class ComCtrlThread : public Thread
{
public:
	ComCtrlThread();
	virtual ~ComCtrlThread();

	/*
	 *  SetComThreads
	 *  功能:保存串口线程池指针
	 *  输入：
	 *  comThreads    :[in]ComThreads* IO管理类指针
	 *  输出：无
	 */
	void SetComThreads(ComThreads* comThreads);

	/*
	 *  SetCom
	 *  功能:保存串口设备指针
	 *  输入：
	 *  com    	  :[in]Com* 串口指针
	 *  输出：无
	 */
	void SetCom(Com* com);

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

	//设备串口配置信息指针
	Device::ComDev* m_comDev;

	//线程父类指针
	ComThreads* m_comThreads;

	//串口指针
	Com* m_com;
};

#endif /* COMCTRLTHREAD_H_ */
