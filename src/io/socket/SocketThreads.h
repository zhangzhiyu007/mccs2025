/*
 * SocketThreads.h
 *
 *  Created on: 2013-4-10
 *      Author: liuhaitao
 */

#ifndef SOCKETTHREADS_H_
#define SOCKETTHREADS_H_

#include "../../db/Device.h"
#include "../../util/Util.h"


class Manager;

/*
 * 名称：SocketThreads
 * 描述：管理多个Socket口线程
 * 用法：
 * 创建人：lht
 * 创建日期：2013-4-10
 */
class SocketThreads
{
public:
	SocketThreads();
	virtual ~SocketThreads();

	/*
	 * SetServerManager
	 * 功能：保存IO管理类指针
	 * 输入：
	 * manager			:[in] Manager* IO管理类指针
	 * 输出：无
	 */
	void SetServerManager(Manager* manager);

	/*
	 * OpenSocketThreads
	 * 功能：根据从设备信息，判断Socket IP是否被使用
	 * 				并对使用的每个Socket IP启动线程
	 * 输入：无
	 * 输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int OpenThreads();

	/*
	 * CloseSocketThreads
	 * 功能：将已经启动的Socket线程关闭
	 * 输入：无
	 * 输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int CloseThreads();
protected:
private:

	//变量
public:
protected:
private:

	Manager* m_serverManager;//IO管理指针
	PtrArray m_socketThreads;//保存线程类指针
	PtrArray m_socketCtrlThreads;//保存控制线程类指针
	PtrArray m_socketTypeDevs;//以太网设备，查找相同IP和端口设备
};

#endif /* SOCKETTHREADS_H_ */


























































































































