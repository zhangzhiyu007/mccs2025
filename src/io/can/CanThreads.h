/*
 * CanThreads.h
 *
 *   创建日期: 2013-4-8
 *   作         者: mzh
 */

#ifndef CANTHREADS_H_
#define CANTHREADS_H_

#include "../../db/Device.h"
#include "../../util/Util.h"
#include "CanThread.h"

class Manager;

/*名称: CanThreads
 *描述: 管理多个CAN口线程
 *用法:
 *创建人: mzh
 *创建日期:2013-4-8
 */
class CanThreads
{
public:
	CanThreads();
	virtual ~CanThreads();
	/*
	 *  SetServerManager
	 *  功能:保存IO管理类指针
	 *  输入：
	 *  manager    :[in]ServerManager* 管理类指针
	 *  输出：无
	 */
	void SetManager(Manager* manager);

	/*
	 *  OpenCanThreads
	 *  功能: 根据从设备信息,判断CAN口是否被使用
	 *	           并对使用的每个CAN口启动线程
	 *  输入：无
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int OpenThreads();

	/*
	 *  CloseCanThreads
	 *  功能: 将已经启动的CAN口线程关闭
	 *  输入：无
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int CloseThreads();
protected:
private:

	//变量
public:
protected:
private:
	Manager* m_Manager;//IO管理指针
	PtrArray m_cans;
	PtrArray m_canThread;//线程池指针
};

#endif /* CANTHREADS_H_ */
