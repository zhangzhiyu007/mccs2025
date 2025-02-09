#include "SolidLi.h"

SolidLi::SolidLi() {
	m_tcp = NULL;
}

SolidLi::~SolidLi() {

}





void SolidLi::SetTcp(TcpClient* tcpClient) {
	this->m_tcp = tcpClient;
}



int SolidLi::Read(Device::SlaveDev *dev) {
	int ret = ErrorInfo::ERR_OK;
	int start = 0;
	int regStart = dev->regStart;

	bool comm = false;

	UshortArray values;
	values.clear();

	int addr = atoi(dev->slaveAddr.c_str());

	float f_data = 0.0f;
	int i_data = 0;
	bool status = 0;



	int length = 0;

	// zlog_warn(Util::m_zlog, "飞轮bms读线程");


	if( NULL == dev){
		zlog_warn(Util::m_zlog, "从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}
	if(!m_tcp->IsOpened()){
		zlog_warn(Util::m_zlog, "tcp 未打开");
		return ErrorInfo::ERR_OPENED;
	}

	ModbusTcpClient client;
	client.SetTcpClient(this->m_tcp);

	start = 101;
	length = 40;

	values.clear();
	ret = client.ReadInputRegisters(values, addr, start, length);

	if(ErrorInfo::ERR_OK == ret) {
		comm = true;
		// 设备模式
		i_data = values[0] ; 			MemDb::SetRealData(regStart + 1, i_data, false);
		// 工作状态
		i_data = values[1] ; 			MemDb::SetRealData(regStart + 2, i_data, false);

		// 当前充电总量
		i_data = values[2] + (values[3] << 16); 			MemDb::SetRealData(regStart + 3, i_data, false);

		// 当前放电总量
		i_data = values[4] + (values[5] << 16); 			MemDb::SetRealData(regStart + 4, i_data, false);

		// 总充电量
		i_data = values[6] + (values[7] << 16); 			MemDb::SetRealData(regStart + 5, i_data, false);

		// 总放电量
		i_data = values[8] + (values[9] << 16); 			MemDb::SetRealData(regStart + 6, i_data, false);

		// 总有功功率
		f_data = values[10] * 0.1; 							MemDb::SetRealData(regStart + 7, f_data, false);

		// 总无功功率
		f_data = values[11] * 0.1; 							MemDb::SetRealData(regStart + 8, f_data, false);

		// SOC
		f_data = (float)(values[12] * 0.1); 							MemDb::SetRealData(regStart + 9, f_data, false);

		// SOE
		f_data = values[13] * 0.1; 							MemDb::SetRealData(regStart + 10, f_data, false);

		// SOH
		f_data = values[14] * 0.1; 							MemDb::SetRealData(regStart + 11, f_data, false);

		// 最大充电功率
		f_data = values[15] * 0.1; 							MemDb::SetRealData(regStart + 12, f_data, false);

		// 最大放电功率
		f_data = values[16] * 0.1; 							MemDb::SetRealData(regStart + 13, f_data, false);

		// 可放电量
		f_data = values[17] * 0.1; 							MemDb::SetRealData(regStart + 14, f_data, false);

		// 可放充量
		f_data = (signed short)values[18] * 0.1; 							MemDb::SetRealData(regStart + 15, f_data, false);

		for(int i = 0; i < 10 ; i++) {
			for(int j = 0; j < 32; j++){
				status = (values[19 + i] >> j) & 0b1; 						MemDb::SetRealData(regStart + 16 + (i+1)*j, status, false);
			}

		}

	}
	else{
		comm =false;
		zlog_warn(Util::m_zlog,"%s 读取数据失败",dev->name.c_str());
	}







	start = 10001;
	length = 20;

	values.clear();

	// 读取电池簇的信息
	ret = client.ReadInputRegisters(values, addr, start, length);

	if(ErrorInfo::ERR_OK == ret) {
		comm = true;
		// 电池簇总电压
		f_data = (signed short)values[0] * 0.1; 				MemDb::SetRealData(regStart + 401, f_data, false);
		// 总电流
		f_data = (signed short)values[1] * 0.1; 				MemDb::SetRealData(regStart + 402, f_data, false);
		// 总功率
		f_data = (signed short)values[2] * 0.1; 				MemDb::SetRealData(regStart + 403, f_data, false);
		// SOC
		f_data = values[3] * 0.1; 								MemDb::SetRealData(regStart + 404, f_data, false);
		// SOE
		f_data = values[4] * 0.1; 								MemDb::SetRealData(regStart + 405, f_data, false);
		// SOH
		f_data = values[5] * 0.1; 								MemDb::SetRealData(regStart + 406, f_data, false);
		// 最大充电功率
		f_data = values[6] * 0.1; 								MemDb::SetRealData(regStart + 407, f_data, false);
		// 最大放电功率
		f_data = values[7] * 0.1; 								MemDb::SetRealData(regStart + 408, f_data, false);
		// 继电器状状态
		f_data = values[8] * 0.1; 								MemDb::SetRealData(regStart + 409, f_data, false);
		// 最高电压
		f_data = values[9] * 0.001; 							MemDb::SetRealData(regStart + 410, f_data, false);
		// 最大电压
		f_data = values[10] * 0.001; 							MemDb::SetRealData(regStart + 411, f_data, false);
		// 最高温度
		f_data = (signed short)values[11] * 0.1; 				MemDb::SetRealData(regStart + 412, f_data, false);
		// 最低温度
		f_data = (signed short)values[12] * 0.1; 				MemDb::SetRealData(regStart + 413, f_data, false);

		// 电压最高位置
		i_data = values[13]; 									MemDb::SetRealData(regStart + 414, i_data, false);
		// 电压最低位置
		i_data = values[14]; 									MemDb::SetRealData(regStart + 415, i_data, false);

		// 温度最高位置
		i_data = values[15]; 									MemDb::SetRealData(regStart + 416, i_data, false);
		// 温度最低位置
		i_data = values[16]; 									MemDb::SetRealData(regStart + 417, i_data, false);

	}
	else{
		comm =false;
		zlog_warn(Util::m_zlog,"%s 读取数据失败",dev->name.c_str());
	}


	MemDb::SetRealData(regStart + 0, comm, false);


	return ret;

}


int SolidLi::Preset(Device::SlaveDev *dev) {

	int ret = ErrorInfo::ERR_OK;
	int start = 0;
	int regStart = dev->regStart;

	bool comm = false;

	RealData realData;

	int addr = atoi(dev->slaveAddr.c_str());

	int length = 0;



	if( NULL == dev){
		zlog_warn(Util::m_zlog, "从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}
	if(!m_tcp->IsOpened()){
		return ErrorInfo::ERR_OPENED;
	}

	ModbusTcpClient client;
	client.SetTcpClient(this->m_tcp);



	if(MemDb::GetRealData(realData, regStart + 501, true)){
		ret = client.PresetSingleRegister(addr, 102, realData.pv.data.iValue);
	}

	if( ErrorInfo::ERR_OK != ret) {
		zlog_warn(Util::m_zlog, "%s 下发控制指令失败 code:%d", dev->name.c_str(), ret);
		return ErrorInfo::ERR_FAILED;
	}

	return ret;

}






