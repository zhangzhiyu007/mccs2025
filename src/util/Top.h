/*
 * Top.h
 *
 *   创建日期: 2013-11-29
 *   作          者: 马中华
 */

#ifndef TOP_H_
#define TOP_H_

#include "../etc/socket/Thread.h"

/*名称:
 *描述:
 *用法:
 *创建人:马中华
 *创建日期:2013-11-29
 */
class Top: public Thread
{
	//方法
public:
	Top();
	virtual ~Top();

	/*
	* 名称: GetCpuUsage
	* 功能: 读取CPU占用率
	* 输入: cpu:[out]float& CPU占用率%
	* 输出: 返回成功/失败
	*/
	bool GetCpuUsage(float& cpu);

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
protected:
private:
	const int m_interval;//计算CPU占用率的间隔时间
	bool m_success;//读取CPU占用率成功/失败
	float m_cpuUsage;//CPU占用率，%
};

#endif /* TOP_H_ */
