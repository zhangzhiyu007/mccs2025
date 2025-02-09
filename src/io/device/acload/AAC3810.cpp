/*
 * VilvaLoad.cpp
 *
 *  创建日期: 2016-5-30
 *  作          者: 刘海涛
 */

#include "AAC3810.h"

#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"
#include <cstdlib>

AAC3810::AAC3810()
{
	// TODO Auto-generated constructor stub
	m_value = 0;
	m_tcpClient = NULL;

	PhaseAResisPower = 0;
	PhaseAIndPower = 0;
	PhaseACapaPower = 0;

	PhaseBResisPower = 0;
	PhaseBIndPower = 0;
	PhaseBCapaPower = 0;

	PhaseCResisPower = 0;
	PhaseCIndPower = 0;
	PhaseCCapaPower = 0;
}

AAC3810::~AAC3810()
{
	// TODO Auto-generated destructor stub
}

void AAC3810::SetTcpClient(TcpClient* client)
{
	this->m_tcpClient = client;
}

int AAC3810::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
		case DevModel::Model_LOAD_AAC3810:
			ret = this->LoadRead(dev);
			break;
		default:
			break;
	};

	return ret;
}

int AAC3810::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
		case DevModel::Model_LOAD_AAC3810:
			ret = this->LoadPreset(dev);
			break;
		default:
			break;
	};

	return ret;
}

int AAC3810::LoadRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0;
	int length = 1;

	this->PhaseAResisPower = 0;
	this->PhaseAIndPower = 0;
	this->PhaseACapaPower = 0;

	this->PhaseBResisPower = 0;
	this->PhaseBIndPower = 0;
	this->PhaseBCapaPower = 0;

	this->PhaseCResisPower = 0;
	this->PhaseCIndPower = 0;
	this->PhaseCCapaPower = 0;

	// zlog_warn(Util::m_zlog, "AAC3810读线程 %d", dev->regStart);

	if (NULL == dev)
	{
		zlog_warn(Util::m_zlog,"从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	if (!m_tcpClient->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetIsTcp(true);
	rtu.SetTcpClient(this->m_tcpClient);
	UshortArray values;
	values.clear();

	start = 719;
	length = 5;

	// 读取A 相数据
	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret){
		comm = true;
		this-> PhaseAResisPower = ( (values[0] >> 12) & 0b1) + ((values[0] >> 13 ) & 0b1) * 2 + ( (values[3] >> 0) & 0b1) * 4 + ( (values[4] >> 0) & 0b1) * 4; // A相R档位
		this-> PhaseAIndPower = ((values[1] >> 10) & 0b1) + ((values[1] >> 11) & 0b1) * 2 + ((values[3] >> 1) & 0b1) * 4 + ((values[4] >> 1) & 0b1) * 4; // A相 档位
		this-> PhaseACapaPower = ((values[2] >> 8) & 0b1) + ((values[2] >> 9) & 0b1) * 2 + ((values[3] >> 2) & 0b1) * 4 + ((values[4] >> 2) & 0b1) * 4; // A相 W档位

		MemDb::SetRealData(regStart + 1, this->PhaseAResisPower, false);
		MemDb::SetRealData(regStart + 2, this->PhaseAIndPower, false);
		MemDb::SetRealData(regStart + 3, this->PhaseACapaPower, false);
	}
	else{
		comm = false;
		zlog_warn(Util::m_zlog, "读取 %s 失败", dev->name.c_str());
	}

	values.clear();
	start = 724;
	length = 5;


	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if( ErrorInfo::ERR_OK == ret) {
		comm = true;
		this-> PhaseBResisPower = ( (values[0] >> 12) & 0b1) + ((values[0] >> 13 ) & 0b1) * 2 + ( (values[3] >> 0) & 0b1) * 4 + ( (values[4] >> 0) & 0b1) * 4; // A相R档位
		this-> PhaseBIndPower = ((values[1] >> 10) & 0b1) + ((values[1] >> 11) & 0b1) * 2 + ((values[3] >> 1) & 0b1) * 4 + ((values[4] >> 1) & 0b1) * 4; // A相 档位
		this-> PhaseBCapaPower = ((values[2] >> 8) & 0b1) + ((values[2] >> 9) & 0b1) * 2 + ((values[3] >> 2) & 0b1) * 4 + ((values[4] >> 2) & 0b1) * 4; // A相 W档位

		MemDb::SetRealData(regStart + 4, this->PhaseBResisPower, false);
		MemDb::SetRealData(regStart + 5, this->PhaseBIndPower, false);
		MemDb::SetRealData(regStart + 6, this->PhaseBCapaPower, false);
	}
	else{
		comm = false;
		zlog_warn(Util::m_zlog, "读取 %s 失败", dev->name.c_str());
	}

	values.clear();
	start = 729;
	length = 5;

	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if( ErrorInfo::ERR_OK == ret) {
		comm = true;
		this-> PhaseCResisPower = ( (values[0] >> 12) & 0b1) + ((values[0] >> 13 ) & 0b1) * 2 + ( (values[3] >> 0) & 0b1) * 4 + ( (values[4] >> 0) & 0b1) * 4; // A相R档位
		this-> PhaseCIndPower = ((values[1] >> 10) & 0b1) + ((values[1] >> 11) & 0b1) * 2 + ((values[3] >> 1) & 0b1) * 4 + ((values[4] >> 1) & 0b1) * 4; // A相 档位
		this-> PhaseCCapaPower = ((values[2] >> 8) & 0b1) + ((values[2] >> 9) & 0b1) * 2 + ((values[3] >> 2) & 0b1) * 4 + ((values[4] >> 2) & 0b1) * 4; // A相 W档位

		MemDb::SetRealData(regStart + 7, this->PhaseCResisPower, false);
		MemDb::SetRealData(regStart + 8, this->PhaseCIndPower, false);
		MemDb::SetRealData(regStart + 9, this->PhaseCCapaPower, false);
	}
	else{
		comm = false;
		zlog_warn(Util::m_zlog, "读取 %s 失败", dev->name.c_str());
	}

	values.clear();
	start = 734;
	length = 1;

	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if( ErrorInfo::ERR_OK == ret) {
		comm = true;
		bool status = 0;
		status = (values[0] >> 0) & 0b1; 			MemDb::SetRealData(regStart + 10, status, false );
		// 报警信息
		status = (values[0] >> 2) & 0b1; 			MemDb::SetRealData(regStart + 11, status, false );
		status = (values[0] >> 3) & 0b1; 			MemDb::SetRealData(regStart + 12, status, false );
		status = (values[0] >> 4) & 0b1; 			MemDb::SetRealData(regStart + 13, status, false );

	}
	else{
		comm = false;
		zlog_warn(Util::m_zlog, "读取 %s 失败", dev->name.c_str());
	}

	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态
	return ret;
}

int AAC3810::LoadPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int regStart = dev->regStart;
	int addr = atoi(dev->slaveAddr.c_str());
	int power = 0;
	RealData realData;

	ModbusRtuMaster rtu;

	rtu.SetIsTcp(true);
	rtu.SetTcpClient(this->m_tcpClient);

	//判断是否有加载/卸载命令下发
	if (MemDb::GetRealData(realData, regStart + 15, true))
	{
		if(realData.pv.data.iValue == 1)
		{
			// 加载
			rtu.PresetSingleRegister(addr, 534, realData.pv.data.iValue);
		}
		else if(realData.pv.data.dbValue == 0)
		{
			// 卸载
			rtu.PresetSingleRegister(addr, 634, realData.pv.data.iValue);
			// 档位清零
			rtu.PresetSingleRegister(addr, 534, 2);
		}

	}

	//判断是否有A相  R L C 功率
	//i = 0 A相 R功率， B相R功率， C相R功率
	for(int i = 0; i < 3; i++) {
		if (MemDb::GetRealData(realData, regStart + 25 + (i*3), true))
		{
			// 设备初始化
			rtu.PresetSingleRegister(addr, 519 +(i*5), 0);
			rtu.PresetSingleRegister(addr, 522+(i*5), 0);
			rtu.PresetSingleRegister(addr, 523+(i*5), 0);
			rtu.PresetSingleRegister(addr, 619+(i*5), 0);
			rtu.PresetSingleRegister(addr, 622+(i*5), 0);
			rtu.PresetSingleRegister(addr, 623+(i*5), 0);

			power =  realData.pv.data.iValue;
			bool bit0, bit1, bit2, bit3 = 0;
			if( power < 12){
				bit0 = (power >> 0) & 0b1;
				bit1 = (power >> 1) & 0b1;
				bit2 = (power >> 2) & 0b1;
				bit3 = (power >> 3) & 0b1;
				zlog_warn(Util::m_zlog, "电阻 %d: %d: %d: %d", bit0, bit1, bit2, bit3);

				if(bit0 == 1) {
					rtu.PresetSingleRegister(addr, 519 +(i*5), (1 << 12 ));
				}
				else {
					rtu.PresetSingleRegister(addr, 619+(i*5), (1 << 12));
				}

				if( bit1 == 1) {
					rtu.PresetSingleRegister(addr, 519+(i*5), (1 << 13));
				}
				else{
					rtu.PresetSingleRegister(addr, 619+(i*5), (1 << 13));
				}

				if( bit2 == 1) {
					rtu.PresetSingleRegister(addr, 522+(i*5), 0);
					rtu.PresetSingleRegister(addr, 522+(i*5), 1);
					rtu.PresetSingleRegister(addr, 623+(i*5), 0);
					rtu.PresetSingleRegister(addr, 623+(i*5), 1);
				}
				else {
					rtu.PresetSingleRegister(addr, 622+(i*5), 0);
					rtu.PresetSingleRegister(addr, 622+(i*5), 1);
				}

				if( bit3 == 1){
					rtu.PresetSingleRegister(addr, 522+(i*5), 1);
					rtu.PresetSingleRegister(addr, 523+(i*5), 1);
				}
				else {
					if( bit2 == 0){
						rtu.PresetSingleRegister(addr, 622+(i*5), 1);
						rtu.PresetSingleRegister(addr, 623+(i*5), 1);
					}
					else{
						rtu.PresetSingleRegister(addr, 623+(i*5), 1);
					}
				}
			}
		}
	}


	//i = 0 A相 L功率， B相L功率， C相L功率
	for(int i = 0; i < 3; i++) {
		if (MemDb::GetRealData(realData, regStart + 26 + (i*3), true))
		{
			rtu.PresetSingleRegister(addr, 520 +(i*5), 0);
			rtu.PresetSingleRegister(addr, 620+(i*5), 0);
			rtu.PresetSingleRegister(addr, 522+(i*5), 0);
			rtu.PresetSingleRegister(addr, 623+(i*5), 0);



			power =  realData.pv.data.iValue;
			int bit0, bit1, bit2, bit3 = 0;
			if( power < 12){
				bit0 = (power >> 0) & 0b1;
				bit1 = (power >> 1) & 0b1;
				bit2 = (power >> 2) & 0b1;
				bit3 = (power >> 3) & 0b1;

				if(bit0 == 1) {
					rtu.PresetSingleRegister(addr, 520 +(i*5), (1 << 10 ));
				}
				else {
					rtu.PresetSingleRegister(addr, 620+(i*5), (1 << 10));
				}

				if( bit1 == 1) {
					rtu.PresetSingleRegister(addr, 520+(i*5), (1 << 11));
				}
				else{
					rtu.PresetSingleRegister(addr, 620+(i*5), (1 << 11));
				}

				if( bit2 == 1) {
					rtu.PresetSingleRegister(addr, 522+(i*5), 1 << 1);
					rtu.PresetSingleRegister(addr, 623+(i*5), 1 << 1);
				}
				else {
					rtu.PresetSingleRegister(addr, 622+(i*5), 1 << 1);
					rtu.PresetSingleRegister(addr, 623+(i*5), 1 << 1);
				}

				if( bit3 == 1) {
					rtu.PresetSingleRegister(addr, 522+(i*5), 0);
					rtu.PresetSingleRegister(addr, 523+(i*5), 0);
					rtu.PresetSingleRegister(addr, 522+(i*5), 1 << 1);
					rtu.PresetSingleRegister(addr, 523+(i*5), 1 << 1);
				}
				else{
					if(bit2 == 0) {
						rtu.PresetSingleRegister(addr, 622+(i*5), 0);
						rtu.PresetSingleRegister(addr, 623+(i*5), 0);
						rtu.PresetSingleRegister(addr, 622+(i*5), 1 << 1);
						rtu.PresetSingleRegister(addr, 623+(i*5), 1 << 1);
					}
					else{
						rtu.PresetSingleRegister(addr, 522+(i*5), 0);
						rtu.PresetSingleRegister(addr, 623+(i*5), 0);
						rtu.PresetSingleRegister(addr, 522+(i*5), 1 << 1);
						rtu.PresetSingleRegister(addr, 623+(i*5), 1 << 1);
					}
				}
			}
		}
	}

	//i = 0 A相 C功率， B相C功率， C相C功率
	for(int i = 0; i < 3; i++) {
		if (MemDb::GetRealData(realData, regStart + 27 + (i*3), true))
		{
			// 设备初始化 
			rtu.PresetSingleRegister(addr, 521 +(i*5), 0);
			rtu.PresetSingleRegister(addr, 521+(i*5), 0);
			rtu.PresetSingleRegister(addr, 522+(i*5), 0);
			rtu.PresetSingleRegister(addr, 523+(i*5), 0);
			rtu.PresetSingleRegister(addr, 524+(i*5), 0);

			rtu.PresetSingleRegister(addr, 621+(i*5), 0);
			rtu.PresetSingleRegister(addr, 622+(i*5), 0);
			rtu.PresetSingleRegister(addr, 623+(i*5), 0);
			power =  realData.pv.data.iValue;
			bool bit0, bit1, bit2, bit3 = 0;
			if( power < 12){
				bit0 = (power >> 0) & 0b1;
				bit1 = (power >> 1) & 0b1;
				bit2 = (power >> 2) & 0b1;
				bit3 = (power >> 3) & 0b1;

				zlog_warn(Util::m_zlog, " %d: %d: %d: %d", bit0, bit1, bit2, bit3);
				if(bit0 == 1) {
					rtu.PresetSingleRegister(addr, 521 +(i*5), (1 << 8 ));
				}
				else {
					rtu.PresetSingleRegister(addr, 621+(i*5), (1 << 8));
				}

				if( bit1 == 1) {
					rtu.PresetSingleRegister(addr, 521+(i*5), (1 << 9));
				}
				else{
					rtu.PresetSingleRegister(addr, 621+(i*5), (1 << 9));
				}

				if( bit2 == 1) {
					zlog_warn(Util::m_zlog, "下发4kW");
					// rtu.PresetSingleRegister(addr, 522+(i*5), (1 << 2));
					ret = rtu.PresetSingleRegister(addr, 523+(i*5), (1 << 2));

					zlog_warn(Util::m_zlog, "下发4kW: %d", ret);
				}
				else {
					// rtu.PresetSingleRegister(addr, 622+(i*5), (1 << 2));
					rtu.PresetSingleRegister(addr, 623+(i*5), (1 << 2));
				}

				if( bit3 == 1){
					ret = rtu.PresetSingleRegister(addr, 522+(i*5), (1 << 2));
					ret = rtu.PresetSingleRegister(addr, 523+(i*5), (1 << 2));
				}
				else {
					if(bit2 == 1) {
						ret = rtu.PresetSingleRegister(addr, 523+(i*5), (1 << 2));
						ret = rtu.PresetSingleRegister(addr, 622+(i*5), (1 << 2));
					}
					else {
						ret = rtu.PresetSingleRegister(addr, 622+(i*5), (1 << 2));
						rtu.PresetSingleRegister(addr, 623+(i*5), (1 << 2));
					}
				}
			}
		}
	}

	if( ret != ErrorInfo::ERR_OK) {
		zlog_warn(Util::m_zlog, "%s 下发指令失败", dev->name.c_str());
	}

	if (MemDb::GetRealData(realData, regStart + 30, true)) {
		rtu.PresetSingleRegister(addr, 522, 0);
		rtu.PresetSingleRegister(addr, 522, 1);

	}

	return ret;
}
