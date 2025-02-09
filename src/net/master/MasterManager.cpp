/*
 * MasterManager.cpp
 *
 *   创建日期: 2013-4-1
 *   作         者: mzh
 */

#include "MasterManager.h"
#include "../../db/Db.h"
#include "../../db/Device.h"

MasterManager::MasterManager()
{
	// TODO 构造函数
	m_slaveConfigs.clear(); //从站控制器参数
	m_masterManagerPort = 6601;//主站UDP管理端口，客户端绑定端口
	m_slaveManagerPort = 6602; //从站UDP管理端口，服务器绑定端口
	m_masterUploadPort = 6603; //主站UDP上传端口，服务器绑定端口
	m_slaveUploadPort = 6604;  //从站UDP上传端口，客户端绑定端口
	m_ctrlPort = 6600; //本机TCP控制端口

}

MasterManager::~MasterManager()
{
	// TODO 析构函数

}

int MasterManager::Init()
{
	zlog_error(Util::m_zlog, "启动主站站内通讯");

	int ret = ErrorInfo::ERR_OK;
	bool success = false;

	//1.从数据库中读取配置信息
	zlog_error(Util::m_zlog, "初始化主站站内通讯配置信息");
	success = InitConfig();
	if (!success)
	{
		zlog_error(Util::m_zlog, "初始化主站站内通讯配置信息失败");
		return ErrorInfo::ERR_FAILED;
	}

	//2.启动UDP管理线程
	zlog_error(Util::m_zlog, "站内通讯,启动UDP管理线程");
	success = StartUdpManagerClient();
	if (success)
	{
		zlog_error(Util::m_zlog, "主站启动UDP管理线程成功");
	}
	else
	{
		zlog_error(Util::m_zlog, "主站启动UDP管理线程失败");
		return ErrorInfo::ERR_FAILED;
	}

	//3.启动UDP监视线程
	zlog_error(Util::m_zlog, "站内通讯,启动UDP监视线程");
	success = StartUploadServer();
	if (success)
	{
		zlog_error(Util::m_zlog, "主站启动UDP监视线程成功");
	}
	else
	{
		zlog_error(Util::m_zlog, "主站启动UDP监视线程失败");
		return ErrorInfo::ERR_FAILED;
	}

	zlog_error(Util::m_zlog, "启动主站站内通讯结束");
	return ret;
}

bool MasterManager::InitConfig()
{
	zlog_info(Util::m_zlog, "初始化NetServer配置信息");
	bool ret = false;
	string sql;

	//1.打开配置数据库
	//局部变量
	ConfigDb* db = (ConfigDb*) ConfigDb::GetInstance();//配置数据库实例

	//打开数据库
	if (!db->IsOpen())
	{
		if (!db->Open())
		{
			zlog_warn(Util::m_zlog, "配置数据库打开失败");
			return false;
		}
	}

	//2.读取端口信息
	//主站UDP管理端口
	db->GetValue(m_masterManagerPort, "master_manager_port", "sys_controller");
	db->GetValue(m_slaveManagerPort, "slave_manager_port", "sys_controller");
	db->GetValue(m_masterUploadPort, "master_upload_port", "sys_controller");
	db->GetValue(m_slaveUploadPort, "slave_upload_port", "sys_controller");
	db->GetValue(m_ctrlPort, "tcp_port", "sys_controller");
	int networkAUsed = 0;
	int networkBUsed = 0;
	db->GetValue(networkAUsed, "network_a_used", "sys_controller");
	db->GetValue(networkBUsed, "network_b_used", "sys_controller");
	m_controllerConfig.networkAUsed = networkAUsed;
	m_controllerConfig.networkBUsed = networkBUsed;
	int controllerAddr = 0;
	int sysType = 0;
	db->GetValue(controllerAddr, "controller_addr", "sys_controller");
	db->GetValue(sysType, "sys_type", "sys_controller");
	m_controllerConfig.controllerAddr = controllerAddr;
	m_controllerConfig.sysType = sysType;

	//3.读取本机IP地址
	sql = "select [value] from [sys_controller] where [type]='ip_a'";

	//预处理sql语句\即将sql语句进行解析成字节
	zlog_info(Util::m_zlog, "预处理sql语句=%s",sql.c_str());
	ret = db->Prepare(sql);
	if (!ret)
	{
		db->Close();
		return false;
	}

	if (DB_ROW == db->Step())//读取数据行数不为0，数据库中存在
	{
		//读取属性
		m_controllerConfig.ipA = db->GetColumnText(0);
	}

	//关闭stmt
	zlog_info(Util::m_zlog, "读取数据库结束，关闭stmt");
	db->Finalize();

	sql = "select [value] from [sys_controller] where [type]='ip_b'";

	//预处理sql语句\即将sql语句进行解析成字节
	zlog_info(Util::m_zlog, "预处理sql语句=%s",sql.c_str());
	ret = db->Prepare(sql);
	if (!ret)
	{
		db->Close();
		return false;
	}

	if (DB_ROW == db->Step())//读取数据行数不为0，数据库中存在
	{
		//读取属性
		m_controllerConfig.ipB = db->GetColumnText(0);
	}

	//关闭stmt
	zlog_info(Util::m_zlog, "读取数据库结束，关闭stmt");
	db->Finalize();

	//获得广播地址
	m_controllerConfig.ipBroadcastA = Util::GetIpBroadcast(
			m_controllerConfig.ipA);
	m_controllerConfig.ipBroadcastB = Util::GetIpBroadcast(
			m_controllerConfig.ipB);

	//3.读取从站设备配置信息
	sql = "select * from [net_slave]";
	if (!db->Prepare(sql))
	{
		zlog_warn(Util::m_zlog, "预处理语句失败");
		db->Close();
		return false;
	}

	SlaveConfig* slaveConfig = NULL;
	m_slaveConfigs.clear();

	int regStart = 0;
	int regLen = 0;

	while (DB_ROW == db->Step())
	{
		slaveConfig = new SlaveConfig();

		slaveConfig->config.controllerAddr = db->GetColumnInt(1);
		slaveConfig->config.sysType = db->GetColumnInt(2);
		slaveConfig->config.ipA = db->GetColumnText(3);
		slaveConfig->config.ipB = db->GetColumnText(4);
		slaveConfig->config.networkAUsed = db->GetColumnInt(5);
		slaveConfig->config.networkBUsed = db->GetColumnInt(6);
		slaveConfig->config.ipBroadcastA = Util::GetIpBroadcast(
				slaveConfig->config.ipA);
		slaveConfig->config.ipBroadcastB = Util::GetIpBroadcast(
				slaveConfig->config.ipB);
		Device::GetMemAddr(regStart, regLen, slaveConfig->config.sysType, 0, 0,
				0, 0, 0);
		slaveConfig->config.regStart = regStart;
		slaveConfig->config.regLength = regLen;
		m_slaveConfigs.push_back(slaveConfig);

	}

	//3、关闭游标，关闭数据库
	db->Finalize();
	db->Close();

#if USED
	zlog_warn(Util::m_zlog, "主站管理客户端端口：m_masterManagerPort=%d",m_masterManagerPort);
	zlog_warn(Util::m_zlog, "从站管理服务器端口：m_slaveManagerPort=%d",m_slaveManagerPort);
	zlog_warn(Util::m_zlog, "主站上传服务器端口：m_masterUploadPort=%d",m_masterUploadPort);
	zlog_warn(Util::m_zlog, "从站上传客户端端口：m_slaveUploadPort=%d",m_slaveUploadPort);
	zlog_warn(Util::m_zlog, "TCP端口：m_ctrlPort=%d",m_ctrlPort);
	zlog_warn(Util::m_zlog, "本机地址：controllerAddr=%d",m_controllerConfig.controllerAddr);
	zlog_warn(Util::m_zlog, "系统类型：sysType=%d",m_controllerConfig.sysType);
	zlog_warn(Util::m_zlog, "本机A网地址：IPA=%s",m_controllerConfig.ipA.c_str());
	zlog_warn(Util::m_zlog, "本机B网地址：IPB=%s",m_controllerConfig.ipB.c_str());
#endif

	zlog_info(Util::m_zlog, "读取从站设备信息成功");
	return ret;
}

void MasterManager::Uninit()
{
	if (m_slaveConfigs.size() > 0)
	{
		SlaveConfig* config = NULL;
		for (unsigned int i = 0; i < m_slaveConfigs.size(); i++)
		{
			config = (SlaveConfig*) m_slaveConfigs[i];
			if (NULL != config)
			{
				delete config;
				config = NULL;
			}
		}

		m_slaveConfigs.clear();
	}
}

int MasterManager::GetMasterManagerPort()
{
	return this->m_masterManagerPort;
}

int MasterManager::GetSlaveManagerPort()
{
	return this->m_slaveManagerPort;
}

int MasterManager::GetMasterUploadPort()
{
	return this->m_masterUploadPort;
}

int MasterManager::GetSlaveUploadPort()
{
	return this->m_slaveUploadPort;
}

int MasterManager::GetCtrlPort()
{
	return this->m_ctrlPort;
}

PtrArray* MasterManager::GetSlaveConfig()
{
	return &m_slaveConfigs;
}

ControllerConfig* MasterManager::GetControllerConfig()
{
	return &m_controllerConfig;
}

bool MasterManager::StartUdpManagerClient()
{
	zlog_warn(Util::m_zlog, "启动UDP管理线程");
	//启动UDP管理
	m_udpManagerClient.SetMasterManager(this);
	m_udpManagerClient.Start();

	return true;
}

bool MasterManager::StartUploadServer()
{
	zlog_warn(Util::m_zlog, "启动UDP监视线程");

	m_uploadServer.SetMasterManager(this);
	m_uploadServer.Start();
	return true;
}
