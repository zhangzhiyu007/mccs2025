/*
 * NetManager.h
 *
 *   创建日期: 2014-1-10
 *   作          者: 马中华
 */

#ifndef NETMANAGER_H_
#define NETMANAGER_H_
#include "./master/MasterManager.h"
#include "./slave/SlaveManeger.h"

/*名称:NetManager
 *描述:站内通讯管理
 *用法:
 *创建人:马中华
 *创建日期:2014-1-10
 */
class NetManager
{
	//方法
public:
	NetManager();
	virtual ~NetManager();

	/*
	 * 名称: Init
	 * 功能: 初始化站内通讯相关线程
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERROR_OK
	 */
	int Init();

	/*
	 * 名称: Uninit
	 * 功能: 停止站内通讯相关线程
	 * 输出: 无
	 */
	void Uninit();

	//变量
public:
protected:
private:
	//站内通讯
	SlaveManager m_slaveManager;//从站管理类
	MasterManager m_masterManger;//主站管理类
};

#endif /* NETMANAGER_H_ */
