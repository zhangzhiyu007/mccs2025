/*
 * IoManager.h
 *
 *   创建日期: 2014-1-10
 *   作          者: 马中华
 */

#ifndef IOMANAGER_H_
#define IOMANAGER_H_

#include "./com/ComThreads.h"
#include "./can/CanThreads.h"
#include "./socket/SocketThreads.h"
#include "../etc/socket/TcpServer.h"
#include "./protocol/modbus/ModbusTcpServer.h"
#include "./protocol/modbus/ModbusRtuSlave.h"

/*名称:IoManager
 *描述:IO通讯管理类，所有读取/控制从设备相关线程由本管理类启动
 *用法:
 *创建人:马中华
 *创建日期:2014-1-10
 */
class IoManager
{
public:
	IoManager();
	virtual ~IoManager();

	/*
	 * 名称: Init
	 * 功能: 初始化IO通讯相关线程
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERROR_OK
	 */
	int Init();

	/*
	 * 名称: Unint
	 * 功能: 停止IO通讯相关线程
	 * 输出: 无
	 */
	void Uninit();
protected:
private:
	//通讯
	ComThreads m_comThreads;//串口通讯线程池
	CanThreads m_canThreads;//CAN通讯线程池
	SocketThreads m_socketThreads;//以太网通讯线程池

	//从站线程
	//从站线程
	ModbusTcpServer::ModbusCheck m_check;
	ModbusTcpFactory m_modbusTcpFactory;//工厂
	TcpThreadedServer m_tcpThreadedServer;//socket线程服务器
};

#endif /* IOMANAGER_H_ */
