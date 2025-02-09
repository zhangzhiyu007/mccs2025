/*
 * MasterManager.h
 *
 *   创建日期: 2013-4-1
 *   作         者: mzh
 */

#ifndef MASTERMANAGER_H_
#define MASTERMANAGER_H_
#include "../../util/Util.h"
#include "../Iec103.h"
#include "UdpManangerClient.h"
#include "UploadServer.h"

class UdpManangerClient;

/*名称:MasterManager
 *描述:用于站内通讯规约兼容103协议主站管理类
 *用法:用于管理站内通讯规约兼容103协议主站
 *创建人:mzh
 *创建日期:2013-4-1
 */
class MasterManager
{

public:
	MasterManager();
	virtual ~MasterManager();

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

	//读取从站配置信息
	PtrArray* GetSlaveConfig();
	//读取本机配置信息
	ControllerConfig* GetControllerConfig();

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
	 *  StartUdpManagerClient
	 *  功能:启动UDP管理线程
	 *  输入：无
	 *  输出：true成功/false失败
	 */
	bool StartUdpManagerClient();

	/*
	 *  StartUploadServer
	 *  功能:启动UDP监视线程
	 *  输入：无
	 *  输出：true成功/false失败
	 */
	bool StartUploadServer();

public:
protected:
private:
	PtrArray m_slaveConfigs;   //从站控制器配置参数SlaveConfig*
	ControllerConfig m_controllerConfig;//本机网络参数

	int m_masterManagerPort;//主站UDP管理端口
	int m_slaveManagerPort; //从站UDP管理端口
	int m_masterUploadPort; //主站UDP上传端口
	int m_slaveUploadPort;  //从站UDP上传端口
	int m_ctrlPort;         //TCP控制端口

	UdpManangerClient m_udpManagerClient;
	UploadServer m_uploadServer;
};

#endif /* MASTERMANAGER_H_ */
