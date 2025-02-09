/*
 * ComThread.cpp
 *
 *   创建日期: 2013-4-8
 *   作         者: mzh
 */

#include "ComThread.h"
#include "../protocol/modbus/ModbusRtuMaster.h"
#include "../../db/MemDb.h"
#include "../../db/Device.h"
#include "../device/pvs/Pvs.h"
#include "../device/env/Env.h"
#include "../device/watt/Wattmeter.h"
#include "../device/powerDetection/PowerDetection.h"
#include "../device/signalmodule/SignalModule.h"
#include "../device/dcdc/DCDC.h"
#include "../device/acload/AcLoad.h"
#include "../device/APFSVG/ApfSvg.h"
#include "../device/pcs/Pcs.h"
#include "../device/PVsimulator/PVSimulator.h"
#include "../device/faultSimultor/FaultSimultor.h"

int ComThread::m_setPower = 0;

ComThread::ComThread()
{
	// TODO 构造函数
	m_comDev = NULL;
	m_comThreads = NULL;
	m_com = NULL;
}

ComThread::~ComThread()
{
	// TODO 析构函数
	m_comDev = NULL;
	m_comThreads = NULL;

	//释放内存
	if (NULL != m_com)
	{
		delete m_com;
		m_com = NULL;
	}
}

//保存串口线程池指针
void ComThread::SetComThreads(ComThreads* comThreads)
{
	assert(comThreads != NULL);
	m_comThreads = comThreads;
}

//保存串口设备指针
void ComThread::SetCom(Com *com)
{
	assert(com !=NULL);
	m_com = com;
}

//线程运行
void ComThread::Run()
{
	zlog_info(Util::m_zlog, "串口%d查询线程启动",m_com->GetPort());

	//得到实例化并初始化
	Device* device = Device::GetInstance();

	//获取从设备信息指针
	PtrArray* slaveArray = device->GetSlaveDevArray();
	//读取通讯类型指针
	PtrArray* constCommTypeArray = device->GetConstCommTypeArray();
	//串口485类型
	Device::ConstCommType* commType =
			(Device::ConstCommType*) constCommTypeArray->at(0);

	//获取本控制器地址
	int controllerAddr = device->GetControllerAddr();

	PtrArray runArray;
	runArray.clear();
	//存取从设备信息数组
	Device::SlaveDev* slaveDev = NULL;
	PtrArray::iterator slaveIter;
	zlog_info(Util::m_zlog, "串口%d查询线程读取串口设备,通讯=%s",m_com->GetPort(),commType->name.c_str());
	for (slaveIter = slaveArray->begin(); slaveIter != slaveArray->end(); slaveIter++)
	{
		slaveDev = reinterpret_cast<Device::SlaveDev*> (*slaveIter);

		if ((NULL != slaveDev)
				&& (slaveDev->commType == commType->type)
				&& (m_com->GetPort() == slaveDev->commNo)
				&& (slaveDev->own == controllerAddr))//Com口通信
		{
			runArray.push_back(slaveDev);
			zlog_warn(Util::m_zlog, "串口%d查询线程:从设备=%s,设备地址=%d",m_com->GetPort(),slaveDev->name.c_str(),atoi(slaveDev->slaveAddr.c_str()));

		}
	}
	zlog_warn(Util::m_zlog, "串口%d查询线程:从设备数量=%d,该串口从设备数量=%d",m_com->GetPort(),slaveArray->size(),runArray.size());

	int ret = ErrorInfo::ERR_OK;
	//通讯状态
	int uNoResponse = 0;//当前读取失败的次数

	//设备
	//PVS
	Pvs pvs;
	pvs.SetCom(this->m_com);

	//环境监测仪
	Env env;
	env.SetCom(this->m_com);


	//智能电表
	Wattmeter watt;
	watt.SetCom(this->m_com);

	//电能模块
	SignalModule signalModule;
	signalModule.SetCom(this->m_com);

	//电能在线监测仪
	PowerDetection power;
	power.SetCom(this->m_com);

	//DCDC
	DCDC dcdc;
	dcdc.SetCom(this->m_com);

	//交流负载
	AcLoad acload;
	acload.SetCom(this->m_com);

	//APF/SVG
	ApfSvg apf;
	apf.SetCom(this->m_com);

	//pcs
	Pcs pcs;
	pcs.SetCom(this->m_com);


	//光伏模拟器
	PVSimulator simulator;
	simulator.SetCom(this->m_com);

	//故障模拟器
	FaultSimultor faultSim;
	faultSim.SetCom(this->m_com);

	

	zlog_warn(Util::m_zlog, "串口%d查询线程主循环启动",m_com->GetPort());
	while (true)
	{
		//判断线程是否结束
		if (m_state == Thread::STOPPING)
		{
			zlog_warn(Util::m_zlog, "串口操作失败，串口线程结束");
			break;
		}

		//判断是否超过计算次数，认为串口有问题
		if (uNoResponse > ARRAY_LENGTH_128)
		{
			zlog_warn(Util::m_zlog, "超过计算次数，串口有问题，关闭串口");
			uNoResponse = 0;
			m_com->Close();
		}

		//判断串口是否打开
		if (!m_com->IsOpen())
		{
			//如果串口关闭，则重新打开串口
			sleep(10);
			zlog_warn(Util::m_zlog, "串口关闭，重新打开串口");
			m_com->Open();
			continue;
		}

		zlog_info(Util::m_zlog, "获取从设备信息");
		for (slaveIter = runArray.begin(); slaveIter != runArray.end(); slaveIter++)
		{
			slaveDev = reinterpret_cast<Device::SlaveDev*> (*slaveIter);
			//判断设备名称
			//判断设备厂商和型号来确定具体通信协议
			zlog_info(Util::m_zlog,"devType = %d",slaveDev->devType);
			switch (slaveDev->devType)
			{
			case DeviceType::DEVICE_TYPE_WATT://智能电表
				zlog_info(Util::m_zlog,"智能电表");
				ret = watt.Read(slaveDev);
				break;
			case DeviceType::DEVICE_TYPE_PV://PVS
				zlog_info(Util::m_zlog,"PVS设备");
				ret = pvs.Read(slaveDev);
				break;
			case DeviceType::DEVICE_TYPE_EVN://环境监测仪
				zlog_info(Util::m_zlog,"环境监测仪设备");
				ret = env.Read(slaveDev);
				break;
			case DeviceType::DEVICE_TYPE_SIGNAL_MODULE://电能模块
				zlog_info(Util::m_zlog,"电能模块");
				ret = signalModule.Read(slaveDev);
				break;
			case DeviceType::DEVICE_TYPE_POWER_DETEC://电能质量在线监测
				zlog_info(Util::m_zlog,"电能质量在线监测仪");
				ret = power.Read(slaveDev);
				break;
			case DeviceType::DEVICE_TYPE_DCDC://DCDC
				zlog_info(Util::m_zlog,"DCDC");
				ret = dcdc.Read(slaveDev);
				break;
			case DeviceType::DEVICE_TYPE_AC_LOAD://交流负载
			case DeviceType::DEVICE_TYPE_SIMU_AC_LOAD://交流负载
				ret = acload.Read(slaveDev);
				break;
			case DeviceType::DEVICE_TYPE_APF://APF
				ret = apf.Read(slaveDev);
				break;
			case DeviceType::DEVICE_TYPE_PCS://PCS
				ret = pcs.Read(slaveDev);
				break;
			case DeviceType::DEVICE_TYPE_SIMU_PV://光伏模拟器
				ret = simulator.Read(slaveDev);
				break;
			case DeviceType::DEVICE_TYPE_SINMU_GZ://故障模拟器
				ret = faultSim.Read(slaveDev);
				break;
			default:
				break;
			};

			//通讯失败
			if (ret != ErrorInfo::ERR_OK)
			{
				zlog_info(Util::m_zlog, "通讯失败");
				uNoResponse++;
			}
			else
			{
				uNoResponse = 0;
			}

			if((slaveDev->devModel == DevModel::Model_SPC030KTL)
					|| (slaveDev->devModel == DevModel::Model_WSTF_LDJ_RLC))
			{
				msleep(500);
			}
			else
			{
				msleep(200);
			}
		}

		if((slaveDev->devModel == DevModel::Model_SPC030KTL)
				|| (slaveDev->devModel == DevModel::Model_WSTF_LDJ_RLC))
		{
			msleep(800);
		}
		else
		{
			msleep(500);
		}
	}
}

