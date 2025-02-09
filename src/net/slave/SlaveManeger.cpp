/*
 * SlaveManeger.cpp
 *
 *   创建日期: 2013-4-5
 *   作         者: mzh
 */

#include "SlaveManeger.h"
#include "../../db/Db.h"

SlaveManager::SlaveManager()
{
	// TODO 构造函数
	m_masterManagerPort = 6601;//主站UDP管理端口，客户端绑定端口
	m_slaveManagerPort = 6602; //从站UDP管理端口，服务器绑定端口
	m_masterUploadPort = 6603; //主站UDP上传端口，服务器绑定端口
	m_slaveUploadPort = 6604;  //从站UDP上传端口，客户端绑定端口
	m_ctrlPort = 6600; //本机TCP控制端口
	m_masterConfigs.clear();
}

SlaveManager::~SlaveManager()
{
	// TODO 析构函数
	Uninit();
}

int SlaveManager::Init()
{
	//1、从站站内通讯,从站设备信息初始化
	zlog_warn(Util::m_zlog, "从站站内通讯,从站设备信息初始化");
	if(!InitConfig())
	{
		zlog_warn(Util::m_zlog, "从站站内通讯,从站设备信息初始化失败");
		return ErrorInfo::ERR_FAILED;
	}

	//2、从站站内通讯,开启UDP管理进程
	zlog_warn(Util::m_zlog,"开启UDP管理进程");
	bool success = StartUdpManagerServer();
	if(success)
	{
		zlog_error(Util::m_zlog, "开启UDP管理进程成功");
	}
	else
	{
		zlog_error(Util::m_zlog, "开启UDP管理进程失败");
		return ErrorInfo::ERR_FAILED;
	}

	//3、开启UDP上传进程
	zlog_warn(Util::m_zlog,"开启UDP上传进程");
	success = StartUploadClient();
	if(success)
	{
		zlog_error(Util::m_zlog, "开启UDP上传进程成功");
	}
	else
	{
		zlog_error(Util::m_zlog, "开启UDP上传进程失败");
		return ErrorInfo::ERR_FAILED;
	}

	//4、开启TCP控制线程
	zlog_warn(Util::m_zlog,"开启TCP控制线程");
	success = StartControlServer();
	if(success)
	{
		zlog_error(Util::m_zlog, "开启TCP控制线程成功");
	}
	else
	{
		zlog_error(Util::m_zlog, "开启TCP控制线程失败");
		return ErrorInfo::ERR_FAILED;
	}

	zlog_warn(Util::m_zlog, "从站站内通讯,从站设备信息初始化结束");

	return true;
}

void SlaveManager::Uninit()
{
	if (m_masterConfigs.size() > 0)
	{
		MasterConfig* config = NULL;
		for (unsigned int i = 0; i < m_masterConfigs.size(); i++)
		{
			config = (MasterConfig*) m_masterConfigs[i];
			if(NULL!=config)
			{
				delete config;
			}
		}

		m_masterConfigs.clear();
	}
}

bool SlaveManager::InitConfig()
{
	zlog_warn(Util::m_zlog, "初始化NetServer配置信息");
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
	int controllerType = 0;
	db->GetValue(controllerAddr, "controller_addr", "sys_controller");
	db->GetValue(controllerType, "controller_type", "sys_controller");
	db->GetValue(sysType, "sys_type", "sys_controller");
	m_controllerConfig.controllerAddr = controllerAddr;
	m_controllerConfig.controllerType = controllerType;
	m_controllerConfig.sysType = sysType;

	//3.读取本机IP地址
	sql = "select [value] from [sys_controller] where [type]='ip_a'";

	//预处理sql语句\即将sql语句进行解析成字节
	zlog_warn(Util::m_zlog, "预处理sql语句=%s",sql.c_str());
	ret = db->Prepare(sql);
	if (!ret)
	{
		zlog_warn(Util::m_zlog, "预处理sql语句失败");
		db->Close();
		return false;
	}

	if (DB_ROW == db->Step())//读取数据行数不为0，数据库中存在
	{
		//读取属性
		m_controllerConfig.ipA = db->GetColumnText(0);
		zlog_info(Util::m_zlog, "A网段IP=%s",m_controllerConfig.ipA.c_str());
	}

	//关闭stmt
	zlog_warn(Util::m_zlog, "读取数据库结束，关闭stmt");
	db->Finalize();

	sql = "select [value] from [sys_controller] where [type]='ip_b'";

	//预处理sql语句\即将sql语句进行解析成字节
	zlog_warn(Util::m_zlog, "预处理sql语句=%s",sql.c_str());
	ret = db->Prepare(sql);
	if (!ret)
	{
		zlog_warn(Util::m_zlog, "预处理sql语句失败");
		db->Close();
		return false;
	}

	if (DB_ROW == db->Step())//读取数据行数不为0，数据库中存在
	{
		//读取属性
		m_controllerConfig.ipB = db->GetColumnText(0);
		zlog_info(Util::m_zlog, "B网段IP=%s",m_controllerConfig.ipB.c_str());
	}

	//关闭stmt
	zlog_warn(Util::m_zlog, "读取数据库结束，关闭stmt");
	db->Finalize();
	db->Close();

	//获得广播地址
	m_controllerConfig.ipBroadcastA = Util::GetIpBroadcast(
			m_controllerConfig.ipA);
	m_controllerConfig.ipBroadcastB = Util::GetIpBroadcast(
			m_controllerConfig.ipB);

#if USED
	zlog_warn(Util::m_zlog, "主站管理客户端端口：m_masterManagerPort=%d",m_masterManagerPort);
	zlog_warn(Util::m_zlog, "从站管理服务器端口：m_slaveManagerPort=%d",m_slaveManagerPort);
	zlog_warn(Util::m_zlog, "主站上传服务器端口：m_masterUploadPort=%d",m_masterUploadPort);
	zlog_warn(Util::m_zlog, "从站上传客户端端口：m_slaveUploadPort=%d",m_slaveUploadPort);
	zlog_warn(Util::m_zlog, "TCP端口：m_ctrlPort=%d",m_ctrlPort);
	zlog_warn(Util::m_zlog, "控制器类型：controllerType=%d",controllerType);
	zlog_warn(Util::m_zlog, "本机地址：controllerAddr=%d",controllerAddr);
	zlog_warn(Util::m_zlog, "系统类型：sysType=%d",m_controllerConfig.sysType);
	zlog_warn(Util::m_zlog, "本机A网地址：IPA=%s",m_controllerConfig.ipA.c_str());
	zlog_warn(Util::m_zlog, "本机B网地址：IPB=%s",m_controllerConfig.ipB.c_str());
#endif

	return true;
}

int SlaveManager::GetMasterManagerPort()
{
	return this->m_masterManagerPort;
}

int SlaveManager::GetSlaveManagerPort()
{
	return this->m_slaveManagerPort;
}

int SlaveManager::GetMasterUploadPort()
{
	return this->m_masterUploadPort;
}

int SlaveManager::GetSlaveUploadPort()
{
	return this->m_slaveUploadPort;
}

int SlaveManager::GetCtrlPort()
{
	return this->m_ctrlPort;
}

ControllerConfig* SlaveManager::GetControllerConfig()
{
	return &m_controllerConfig;
}

PtrArray* SlaveManager::GetMasterConfigs()
{
	return &m_masterConfigs;
}

bool SlaveManager::StartUdpManagerServer()
{
	m_udpManagerServer.SetSlaveManager(this);
	if (!m_udpManagerServer.IsAlive())
	{
		m_udpManagerServer.Start();
	}
	return true;
}

bool SlaveManager::StartUploadClient()
{
	m_uploadClient.SetMasterManager(this);
	if(!m_uploadClient.IsAlive())
	{
		m_uploadClient.Start();
	}
	return true;
}

bool SlaveManager::StartControlServer()
{
	m_tcpThreadedServer.SetParam(this);
	m_tcpThreadedServer.Start(m_controlServerFactory,m_ctrlPort);
	return true;
}

