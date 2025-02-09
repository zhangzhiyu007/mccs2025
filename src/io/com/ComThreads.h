/*
 * ComThreads.h
 *
 *   创建日期: 2013-4-8
 *   作         者: mzh
 */

#ifndef COMTHREADS_H_
#define COMTHREADS_H_

#include "../../db/Device.h"
#include "../../util/Util.h"
#include "ComThread.h"

class Manager;

/*名称:ComThreads
 *描述:管理多个串口线程
 *用法:
 *创建人:mzh
 *创建日期:2013-4-8
 */
class ComThreads
{
public:
	ComThreads();
	virtual ~ComThreads();

	/*
	 *  SetServerManager
	 *  功能:保存管理类指针
	 *  输入：
	 *  manager    :[in]Manager* IO管理类指针
	 *  输出：无
	 */
	void SetManager(Manager* manager);

	/*
	 *  OpenThreads
	 *  功能: 为每个串口创建串口线程
	 *  输入：无
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int OpenThreads();

	/*
	 *  CloseThreads
	 *  功能: 将串口线程关闭
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
	Manager* m_Manager;

	PtrArray m_coms;
	//保存线程类指针
	PtrArray m_comThreads;
	//保存线程类指针
	PtrArray m_comCtrlThreads;
};

#endif /* COMTHREADS_H_ */
