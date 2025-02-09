/*
 * Manager.h
 *
 *   创建日期: 2013-9-27
 *   作          者: 马中华
 */

#ifndef MANAGER_H_
#define MANAGER_H_

#include "./io/IoManager.h"
#include "./net/NetManager.h"
#include "./ctrl/CtrlManager.h"
#include "./db/PvVolatility.h"

/*名称: Manager
 *描述: 管理类，用于系统启动、运行
 *用法:
 *创建人:马中华
 *创建日期:2013-9-27
 */
class Manager
{
public:
	Manager();
	virtual ~Manager();

	/*
	 * 名称: Init
	 * 功能: 系统启动
	 * 输出: 成功/失败
	 */
	int Init();

	/*
	 * 名称: Init
	 * 功能: 停止系统
	 * 输出: 无
	 */
	void Uninit();

protected:
private:


	//变量
public:
protected:
private:
	//IO通讯
	IoManager m_io;

	//站内通讯
	NetManager m_net;

	//控制管理
	CtrlManager m_ctrl;

	PvVolatility m_pvVolatility;//光伏波动率线程
};

#endif /* MANAGER_H_ */
