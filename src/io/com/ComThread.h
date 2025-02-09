/*
 * ComThread.h
 *
 *   创建日期: 2013-4-8
 *   作         者: mzh
 */

#ifndef COMTHREAD_H_
#define COMTHREAD_H_

/*名称: ComThread
 *描述: 用于通过串口读取从设备数据
 *     每个串口产生一个串口线程
 *用法:
 *1.启动线程
 *ComThread thread;
 *thread.Start();
 *2.关闭线程
 *thread.Close();
 *创建人:mzh
 *创建日期:2013-4-8
 */
#include "../../etc/socket/Thread.h"
#include "../../util/Util.h"
#include "../../db/Device.h"
#include "Com.h"

class ComThreads;
class ComThread: public Thread
{
public:
	ComThread();
	virtual ~ComThread();

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

	//变量
public:

	//判断是否进行了功率值设定
	static int m_setPower;//1:设置了功率值     0:功率值设置完成
protected:
private:

	//设备串口配置信息指针
	Device::ComDev* m_comDev;

	//线程父类指针
	ComThreads* m_comThreads;

	//串口指针
	Com* m_com;
};

#endif /* COMTHREAD_H_ */
