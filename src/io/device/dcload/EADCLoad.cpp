/**
 *    文 件       :			EADCLoad.cpp
 *    功 能       :			
 *    作 者       :			Mingyu Wu
 *    version     :			1.0.0
 *    日 期       :			2021-04-06
 */
#include "EADCLoad.h"
#include "../../../util/Util.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"



EADCLoad::EADCLoad(){
	m_tcpClient = NULL;
	m_nominalCur = 60;
	m_nominalVolt = 500;
	m_nominalPow = 10000;
	m_internalResMax = 500;
	m_internalResMin = 0.25;

}
EADCLoad::~EADCLoad(){

}

void EADCLoad::SetTcp(TcpClient *tcpClient){
	m_tcpClient  = tcpClient;
}
 
int EADCLoad::Read(Device::SlaveDev *dev){
	int ret = ErrorInfo::ERR_OK;
	switch (dev->devModel) {
		case DevModel::Model_ELR_9500_60:
		ret = this->ELR9500Read(dev);
		break;
	}

	return ret;
}

int EADCLoad::ELR9500Read(Device::SlaveDev *dev){
	int ret = ErrorInfo::ERR_OK;
	float fdata = 0;
	int data =0;

	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0x0000;
	int length = 0;
	bool comm = false;


	ModbusRtuMaster rtu;
	rtu.SetIsTcp(true);
	rtu.SetTcpClient(m_tcpClient);
	UshortArray values;
	FloatArray fvalues;

	// 开启程控模式
	//rtu.ForceSingleCoil(addr, 402, true);

	
	start = 121;
	length = 10;
	ret = rtu.ReadOutputFloatRegisters(fvalues, addr, start, length);
	if( ErrorInfo::ERR_OK == ret ){
		comm = true;
		fdata = fvalues[ 0 ];			
		MemDb::SetRealData(regStart + 3, fdata, false); // 额定电压
		m_nominalVolt = fdata;
		
		fdata = fvalues[ 1 ];			
		MemDb::SetRealData(regStart + 4, fdata, false); // 额定电流
		m_nominalCur = fdata;
		
		fdata = fvalues[ 2 ];			
		MemDb::SetRealData(regStart + 5, fdata, false); // 额定功率
		m_nominalPow = fdata;

		fdata = fvalues[ 3 ];
		MemDb::SetRealData(regStart + 6, fdata, false); //最大内阻值
		m_internalResMax = fdata;

		fdata = fvalues[ 4 ];
		MemDb::SetRealData(regStart + 7, fdata , false); // 最小内阻值
		m_internalResMin = fdata;

	}

	length = 1;
	start = 405;
	ret = rtu.ReadOutputStatus(values, addr, start, length);
	if(ErrorInfo::ERR_OK == ret){
		comm = true;
		data = values[ 0 ];
		MemDb::SetRealData(regStart + 1, data, false); //负载直流输入状态
	}

	start = 500;
	length = 4;
	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if( ErrorInfo::ERR_OK == ret ){
		comm =true;
		data = values[ 0 ] * m_nominalVolt / 52428;
		MemDb::SetRealData(regStart + 8 , data, false); // 设定电压值

		data = values[ 1 ] * m_nominalCur / 52428;
		MemDb::SetRealData(regStart + 9 , data, false); // 设定电流值

		data = values[ 2 ] * m_nominalPow / 52428;
		MemDb::SetRealData(regStart + 10 , data, false); //  设定功率值

		data = values[ 3 ] * m_internalResMax / 52428;
		MemDb::SetRealData(regStart + 11 , data, false); // 设定电阻值
	}

	start = 505;
	length = 3;
	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret) {
		comm = true;
		data = (values[ 1 ] >> 9 ) & 0b11;
		MemDb::SetRealData(regStart + 2, data, false); // 运行模式, 00 = CV , 01 = CR, 10 = CC 11 = CP;
	}

	start = 507;
	length = 3;
	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if(ErrorInfo::ERR_OK == ret ){
		data = values[ 0 ] * m_nominalVolt / 52428;
		MemDb::SetRealData(regStart + 12, data, false); //当前电压

		data = values[ 1 ] * m_nominalCur / 52428;
		MemDb::SetRealData(regStart + 13, data, false); //当前电流值

		data = values[ 2 ] * m_nominalPow / 52428;
		MemDb::SetRealData(regStart + 14, data, false); // 当前功率

	}

	MemDb::SetRealData(regStart + 0 , comm, false);//通讯状态
	return ret;

}

int EADCLoad::Preset(Device::SlaveDev *dev){
	int ret = ErrorInfo::ERR_OK;
	switch (dev->devModel) {
		case DevModel::Model_ELR_9500_60:
			ret = this->ELR9500Preset(dev);
			break;
	}
	return ret;

}

int EADCLoad::ELR9500Preset(Device::SlaveDev *dev){
	int ret = ErrorInfo::ERR_OK;

	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	RealData realData;
	unsigned short data = 0;
	bool tmp = false;
	ModbusRtuMaster rtu;
	rtu.SetIsTcp(true);
	rtu.SetTcpClient(m_tcpClient);
	
	if( MemDb::GetRealData(realData, regStart + 19, true)){
		tmp = (bool)realData.pv.data.iValue;
		ret = rtu.ForceSingleCoil(addr, 402, tmp);
	}

	if (MemDb::GetRealData(realData, regStart + 20, true)) {
		tmp = (bool)realData.pv.data.iValue;
		ret = rtu.ForceSingleCoil(addr, 405, tmp);
	}

	if( MemDb::GetRealData(realData, regStart + 26, true)){
		data = realData.pv.data.iValue * 52428 / 500;
		zlog_warn(Util::m_zlog,"电压设定值%d\n", data);
		ret = rtu.PresetSingleRegister(addr, 500, data); // 电压设定值
	}

	if( MemDb::GetRealData(realData, regStart + 27, true)){
		data = realData.pv.data.iValue * 52428 / 60;
		ret = rtu.PresetSingleRegister(addr, 501, data); // 电流设定值
	}

	if( MemDb::GetRealData(realData, regStart + 28, true)){
		data = realData.pv.data.iValue * 52428 / 10000;
		ret = rtu.PresetSingleRegister(addr, 502, data); // 功率设定值
	}

	if( MemDb::GetRealData(realData, regStart + 29, true)){
		data = realData.pv.data.iValue * 52428 / 500;
		ret = rtu.PresetSingleRegister(addr, 503, data); // 内阻设定值
	}

	return ret;

}






