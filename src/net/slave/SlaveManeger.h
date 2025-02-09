/*
 * SlaveManeger.h
 *
 *   创建日期: 2013-4-5
 *   作         者: mzh
 */

#ifndef SLAVEMANEGER_H_
#define SLAVEMANEGER_H_
#include "../Iec103.h"
#include "ControlServer.h"
#include "UdpManagerServer.h"
#include "UploadClient.h"

/*名称:SlaveManager
 *描述:用于站内通讯规约兼容103协议从站管理类
 *用法:
 *创建人:mzh
 *创建日期:2013-4-5
 */
class SlaveManager
{
	//方法
public:
	SlaveManager();
	virtual ~SlaveManager();

	/*
	 *  Init
	 *  功能:初始化MasterManager
	 *  输入：无
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERROR_OK
	 */
	int Init();

	/*
	 *  Init
	 *  功能: 反初始化MasterManager
	 *  输入：无
	 *  输出：无
	 */
	void Uninit();


	//读取主站UDP管理端口
	int GetMasterManagerPort();
	//读取从站UDP管理端口
	int GetSlaveManagerPort();

	//读取主站UDP上传端口
	int GetMasterUploadPort();
	//读取主站UDP上传端口
	int GetSlaveUploadPort();

	//读取本机TCP控制端口
	int GetCtrlPort();

	//读取本机配置信息
	ControllerConfig* GetControllerConfig();
	//读取主站配置信息
	PtrArray* GetMasterConfigs();

protected:
private:
	/*
	 *  InitConfig
	 *  功能:从数据库中读取配置信息
	 *  输入：无
	 *  输出：true成功/false失败
	 */
	bool InitConfig();

	/*
	 *  StartUdpManagerServer
	 *  功能:启动UDP从站管理线程
	 *  输入：无
	 *  输出：true成功/false失败
	 */
	bool StartUdpManagerServer();

	/*
	 *  StartUploadClient
	 *  功能:启动UDP上传线程
	 *  输入：无
	 *  输出：true成功/false失败
	 */
	bool StartUploadClient();

	/*
	 *  StartControlServer
	 *  功能:启动TCP控制服务器
	 *  输入：无
	 *  输出：true成功/false失败
	 */
	bool StartControlServer();

	//变量
public:
protected:
private:
	ControllerConfig m_controllerConfig;//本机网络参数
	PtrArray m_masterConfigs; //主站控制器配置参数MasterConfig*

	int m_masterManagerPort;//主站UDP管理端口
	int m_slaveManagerPort; //从站UDP管理端口
	int m_masterUploadPort; //主站UDP上传端口
	int m_slaveUploadPort;  //从站UDP上传端口
	int m_ctrlPort;         //TCP控制端口

	//103协议从站子类
	TcpThreadedServer m_tcpThreadedServer;//TCP服务器
	ControlServerFactory m_controlServerFactory;//TCP服务工厂

	UdpManagerServer m_udpManagerServer;//站内设备UDP管理端口
	UploadClient m_uploadClient;//从站UDP主动上传数据.

};

#endif /* SLAVEMANEGER_H_ */
