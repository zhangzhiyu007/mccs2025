/**
 * @file ZJNCLoad.cpp
 * @Synopsis  
 * @author Mingyu Wu
 * @version 
 * @date 2022-03-29
 */

#include "ZJNCLoad.h"
ZJNCLoad::ZJNCLoad() {
	m_tcpClient = NULL;
}

ZJNCLoad::~ZJNCLoad() {

}

void ZJNCLoad::SetTcp(TcpClient* client){
	m_tcpClient = client;
}


int ZJNCLoad::Read(Device::SlaveDev* dev) {
	int ret = ErrorInfo::ERR_OK;
	if( NULL == dev){
		zlog_warn(Util::m_zlog, "从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}
	if(!m_tcpClient->IsOpened()){
		zlog_warn(Util::m_zlog, "tcp 未打开");
		return ErrorInfo::ERR_OPENED;
	}

	switch(dev->devModel) {
		case DevModel::DCLOAD_A:
			ret = DCLoad_a_Read(dev);
			break;
		case DevModel::DCLOAD_B:
			ret = DCLoad_b_Read(dev);
			break;
		default:
			break;
	}

	return ret;
}

int ZJNCLoad::DCLoad_a_Read(Device::SlaveDev* dev) {
	int ret = ErrorInfo::ERR_OK;

	int start = 0;
	int regStart = dev->regStart;

	bool comm = false;

	UshortArray values;
	values.clear();

	int addr = atoi(dev->slaveAddr.c_str());

	int i_data = 0;
	float f_data =0.0f;
	bool status = 0;



	int length = 0;

	// zlog_warn(Util::m_zlog, "飞轮bms读线程");


	if( NULL == dev){
		zlog_warn(Util::m_zlog, "从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}
	if(!m_tcpClient->IsOpened()){
		zlog_warn(Util::m_zlog, "tcp 未打开");
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetIsTcp(true);
	rtu.SetTcpClient(this->m_tcpClient);


	start = 0x64;
	length = 12;

	values.clear();
	ret = rtu.ReadOutputRegisters(values, addr, start, length);

	if(ErrorInfo::ERR_OK == ret) {
		comm = true;

		// 工作状态
		status = (values[0] >> 3) & 0b1; 			MemDb::SetRealData(regStart + 1, status, false);
		

		//运行模式, 恒流
		i_data = values[0] >> 0 & 0b1;
		if( i_data ) {
			MemDb::SetRealData(regStart + 2, 1, false);
		}

		// 运行模式，恒阻
		i_data = values[0] >> 1 & 0b1;
		if( i_data ) {
			MemDb::SetRealData(regStart + 2, 2, false);
		}

		// 运行模式，恒功率
		i_data = values[0] >> 2 & 0b1;
		if( i_data ) {
			MemDb::SetRealData(regStart + 2, 3, false);
		}

		// 急停
		status = (values[0] >> 5) & 0b1; 		MemDb::SetRealData(regStart + 3, status, false);

		// 过压
		status = (values[0] >> 6) & 0b1; 		MemDb::SetRealData(regStart + 4, status, false);
		// 过温
		status = (values[0] >> 7) & 0b1; 		MemDb::SetRealData(regStart + 5, status, false);
		// 极性反接
		status = (values[0] >> 8) & 0b1; 		MemDb::SetRealData(regStart + 6, status, false);


		// 电压
		f_data = ((values[1] << 16) + values[2]) * 0.1; 	MemDb::SetRealData(regStart + 7, f_data, false);

		// 电流
		f_data = ((values[3] << 16) + values[4]) * 0.1; 	MemDb::SetRealData(regStart + 8, f_data, false);

		// 电阻
		f_data = ((values[5] << 16) + values[6]) * 0.1; 	MemDb::SetRealData(regStart + 9, f_data, false);

		// 功率
		f_data = ((values[7] << 16) + values[8]) * 0.1; 	MemDb::SetRealData(regStart + 10, f_data, false);

		// 加载时间小时
		i_data = values[9]; 							MemDb::SetRealData(regStart + 11, i_data, false);
		i_data = (values[10] >> 8) & 0xFF; 				MemDb::SetRealData(regStart + 12, i_data, false);
		i_data = values[10] & 0xFF; 					MemDb::SetRealData(regStart + 13, i_data, false); 

		MemDb::SetRealData(regStart + 0, comm);
	}
	else {
		comm = false;
		MemDb::SetRealData(regStart + 0, comm);
		zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
	}

}

int ZJNCLoad::DCLoad_b_Read(Device::SlaveDev* dev) {
	int ret = ErrorInfo::ERR_OK;

	int start = 0;
	int regStart = dev->regStart;

	bool comm = false;

	UshortArray values;
	values.clear();

	int addr = atoi(dev->slaveAddr.c_str());

	int i_data = 0;
	float f_data =0.0f;
	bool status = 0;
	int length = 0;

	// zlog_warn(Util::m_zlog, "飞轮bms读线程");


	if( NULL == dev){
		zlog_warn(Util::m_zlog, "从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}
	if(!m_tcpClient->IsOpened()){
		zlog_warn(Util::m_zlog, "tcp 未打开");
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetIsTcp(true);
	rtu.SetTcpClient(this->m_tcpClient);

	// 下发读状态命令格式 （功能码03）	
	//  地址（1字节）+功能码（1字节）+ 起始寄存器地址（2字节） + 寄存器数量（2字节） +校验码（2字节） 
	//        05             03                 00 64                   00 0B                44 56
	// 本机默认1
	// 数据返回格式
	// 地址（1字节）+ 功能码（1字节）+ 数据长度（字节数）+ 数据1 + 数据2 + 数据3 + 数据4 + 数据5 + 数据6 + 数
	//     05             03                 16              XX      XX      XX       XX       XX       XX     
	// 据7 + 数据8 + 数据9 + 数据10 + 数据11 + 数据12 + 数据13 + 数据14 + 数据15 + 数据16 + 数据17 + 数据18 
	// XX      XX      XX       XX        XX       XX        XX       XX       XX        XX        XX        XX 
	// + 数据19 + 数据20 + 数据21 + 数据22 + 校验码（2字节）
	//       XX        XX       XX        XX          XX XX
	// 数据返回数据说明：
	//  		寄存器	数据及状态
	// 数据1	工作状态	15	备用	备用
	// 		14	备用	备用
	// 		13	备用	备用
	// 		12	备用	备用
	// 		11	备用	备用
	// 		10	备用	备用
	// 		9	备用	备用
	// 		8	电压反接报警	0：未反接报警
	// 数据2	工作状态	7	 1：过温报警  	0：未过温报警
	// 		6	1：过压报警(大于820V)	0：未过压报警
	// 		5	1：急停状态	0：未急停状态
	// 		4	  1：定时时间到	 0：定时时间未到
	// 		3	1：=加载状态	0：卸载状态
	// 		2	 1：恒功率状态	    0：未在恒功率状态
	// 		1	1：恒阻状态	 0：未在恒阻状态
	// 		0	1：恒流状态	 0：未在恒流状态
	

	//  数据3 + 数据4 + 数据5 + 数据6 转换10进制（双精度整数）/10 = 采集电压（V）
	//  数据7 + 数据8 + 数据9 + 数据10 转换10进制（双精度整数）/100 = 采集电流（A）
	//  数据11 + 数据12 + 数据13 + 数据14转换10进制（双精度整数）/10 = 采集电阻（Ω）
	//  数据15 + 数据16 + 数据17 + 数据18转换10进制（双精度整数）/10 = 采集功率（W）
	//  数据19 + 数据20 + 数据21 + 数据22 加载时间
	//  数据19	数据20	数据21	数据22
	// 小时（转换10进制）	分（转换10进制）	秒（转换10进制）

	start = 0x64;
	length = 0x0B;

	values.clear();
	ret = rtu.ReadOutputRegisters(values, addr, start, length);

	if(ErrorInfo::ERR_OK == ret) {
		comm = true;

		// 工作状态
		status = (values[0] >> 3) & 0b1; 			MemDb::SetRealData(regStart + 1, status, false);

		// 过压
		status = (values[0] >> 6) & 0b1; 		MemDb::SetRealData(regStart + 2, status, false);
		// 过温
		status = (values[0] >> 7) & 0b1; 		MemDb::SetRealData(regStart + 3, status, false);
		// 急停
		status = (values[0] >> 5) & 0b1; 		MemDb::SetRealData(regStart + 4, status, false);
		// 定时时间到
		status = (values[0] >> 4) & 0b1; 		MemDb::SetRealData(regStart + 5, status, false);
		//恒功率状态
		status = (values[0] >> 2) & 0b1; 		MemDb::SetRealData(regStart + 6, status, false);
		//恒阻状态
		status = (values[0] >> 1) & 0b1; 		MemDb::SetRealData(regStart + 7, status, false);
		//恒流状态
		status = values[0] & 0b1; 				MemDb::SetRealData(regStart + 8, status, false);

		

		// 电压
		f_data = ((values[1] << 16) + values[2]) * 0.1; 	MemDb::SetRealData(regStart + 11, f_data, false);

		// 电流
		f_data = ((values[3] << 16) + values[4]) * 0.01; 	MemDb::SetRealData(regStart + 12, f_data, false);

		// 电阻
		f_data = ((values[5] << 16) + values[6]) * 0.1; 	MemDb::SetRealData(regStart + 13, f_data, false);

		// 功率
		f_data = ((values[7] << 16) + values[8]) * 0.1; 	MemDb::SetRealData(regStart + 14, f_data, false);

		// 加载时间小时
		i_data = values[9]; 							MemDb::SetRealData(regStart + 15, i_data, false);
		i_data = (values[10] >> 8) & 0xFF; 				MemDb::SetRealData(regStart + 16, i_data, false);
		i_data = values[10] & 0xFF; 					MemDb::SetRealData(regStart + 17, i_data, false);
		
		MemDb::SetRealData(regStart + 0, comm);
	}
	else {
		comm = false;
		MemDb::SetRealData(regStart + 0, comm);
		zlog_warn(Util::m_zlog, "%s 设备 读取数据失败", dev->name.c_str());
	}


}


int ZJNCLoad::Preset(Device::SlaveDev *dev) {
	int ret = ErrorInfo::ERR_OK;
	if( NULL == dev){
		zlog_warn(Util::m_zlog, "从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}
	if(!m_tcpClient->IsOpened()){
		return ErrorInfo::ERR_OPENED;
	}

	switch(dev->devModel) {
		case DevModel::DCLOAD_A:
			ret = DCLoad_a_Preset(dev);
			break;
		case DevModel::DCLOAD_B:
			ret = DCLoad_b_Preset(dev);
			break;
		default:
			break;
	}

	return ret;

}

int ZJNCLoad::DCLoad_a_Preset(Device::SlaveDev *dev) {
	int ret = ErrorInfo::ERR_OK;
	int start = 0;
	int regStart = dev->regStart;

	bool comm = false;
	int i_data = 0;

	RealData realData;

	int addr = atoi(dev->slaveAddr.c_str());

	int length = 0;



	if( NULL == dev){
		zlog_warn(Util::m_zlog, "从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}
	if(!m_tcpClient->IsOpened()){
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetIsTcp(true);
	rtu.SetTcpClient(this->m_tcpClient);


	// 工作模式
	if(MemDb::GetRealData(realData, regStart + 31, true)){
		i_data = realData.pv.data.iValue;

		zlog_warn(Util::m_zlog, "工作模式：%i", i_data);

		switch (realData.pv.data.iValue) {
			case 1:
				// 加载
				zlog_warn(Util::m_zlog, "设置直流负载 加载 命令");
				ret = rtu.PresetSingleRegister(addr, 0x32, (1 << 6));
				break;
			case 0:
				zlog_warn(Util::m_zlog, "设置直流负载 卸载 命令");
				ret = rtu.PresetSingleRegister(addr, 0x32, (1 << 7));

			default:
				break;
		}

	}

	if(MemDb::GetRealData(realData, regStart + 32, true)){
		switch (realData.pv.data.iValue) {
			case 1:
				zlog_warn(Util::m_zlog, "设置直流负载 卸载 命令");
				ret = rtu.PresetSingleRegister(addr, 0x32, (1 << 7));
				//恒流
				ret = rtu.PresetSingleRegister(addr, 0x32, (1 << 0));
				break;
			case 2:
				zlog_warn(Util::m_zlog, "设置直流负载 卸载 命令");
				ret = rtu.PresetSingleRegister(addr, 0x32, (1 << 7));
				//恒阻
				ret = rtu.PresetSingleRegister(addr, 0x32, (1 << 1));
				break;
			case 3:
				zlog_warn(Util::m_zlog, "设置直流负载 卸载 命令");
				ret = rtu.PresetSingleRegister(addr, 0x32, (1 << 7));
				//恒功率
				ret = rtu.PresetSingleRegister(addr, 0x32, (1 << 2));
				break;
			default:
				break;
		}
	}

	// 设定电流
	if(MemDb::GetRealData(realData, regStart + 33, true)){
		i_data = realData.pv.data.iValue * 100;
		rtu.PresetSingleRegister(addr, 0x33, (i_data >> 16) & 0xFFFF);
		rtu.PresetSingleRegister(addr, 0x34, i_data  & 0xFFFF);
	}

	// 设定电压
	if(MemDb::GetRealData(realData, regStart + 34, true)){
		i_data = realData.pv.data.iValue * 10;
		rtu.PresetSingleRegister(addr, 0x35, (i_data >> 16) & 0xFFFF);
		rtu.PresetSingleRegister(addr, 0x36, i_data  & 0xFFFF);
	}


	// 设定功率
	if(MemDb::GetRealData(realData, regStart + 35, true)){
		i_data = realData.pv.data.iValue * 10;
		rtu.PresetSingleRegister(addr, 0x37, (i_data >> 16) & 0xFFFF);
		rtu.PresetSingleRegister(addr, 0x38, i_data  & 0xFFFF);
	}

	// 加载时间，小时
	if(MemDb::GetRealData(realData, regStart + 36, true)){
		i_data = realData.pv.data.iValue & 0xFFFF;
		rtu.PresetSingleRegister(addr, 0x39, i_data);
	}

	// 加载时间，分钟
	if(MemDb::GetRealData(realData, regStart + 37, true)){
		i_data = realData.pv.data.iValue & 0xFF;
		rtu.PresetSingleRegister(addr, 0x39, (i_data) << 8);
	}

	// 加载时间，秒
	if(MemDb::GetRealData(realData, regStart + 38, true)){
		i_data = realData.pv.data.iValue & 0xFF;
		rtu.PresetSingleRegister(addr, 0x39, i_data);
	}



	if( ErrorInfo::ERR_OK != ret) {
		zlog_warn(Util::m_zlog, "%s 下发控制指令失败 code:%d", dev->name.c_str(), ret);
		return ErrorInfo::ERR_FAILED;
	}

	return ret;
}

int ZJNCLoad::DCLoad_b_Preset(Device::SlaveDev *dev) {
	int ret = ErrorInfo::ERR_OK;
	int start = 0;
	int regStart = dev->regStart;

	bool comm = false;
	int i_data = 0;

	RealData realData;

	int addr = atoi(dev->slaveAddr.c_str());

	int length = 0;

	if( NULL == dev){
		zlog_warn(Util::m_zlog, "从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}
	if(!m_tcpClient->IsOpened()){
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetIsTcp(true);
	rtu.SetTcpClient(this->m_tcpClient);

	// 工作模式
	if(MemDb::GetRealData(realData, regStart + 31, true)){
		i_data = realData.pv.data.iValue;

		zlog_warn(Util::m_zlog, "工作模式：%i", i_data);

		switch (realData.pv.data.iValue) {
			case 1:
				// 加载
				zlog_warn(Util::m_zlog, "设置直流负载 加载 命令");
				ret = rtu.PresetSingleRegister(addr, 0x32, (1 << 6));
				break;
			case 0:
				zlog_warn(Util::m_zlog, "设置直流负载 卸载 命令");
				ret = rtu.PresetSingleRegister(addr, 0x32, (1 << 7));

			default:
				break;
		}

	}

	if(MemDb::GetRealData(realData, regStart + 32, true)){
		switch (realData.pv.data.iValue) {
			case 1:
				zlog_warn(Util::m_zlog, "设置直流负载 卸载 命令");
				ret = rtu.PresetSingleRegister(addr, 0x32, (1 << 7));
				//恒流
				ret = rtu.PresetSingleRegister(addr, 0x32, (1 << 0));
				break;
			case 2:
				zlog_warn(Util::m_zlog, "设置直流负载 卸载 命令");
				ret = rtu.PresetSingleRegister(addr, 0x32, (1 << 7));
				//恒阻
				ret = rtu.PresetSingleRegister(addr, 0x32, (1 << 1));
				break;
			case 3:
				zlog_warn(Util::m_zlog, "设置直流负载 卸载 命令");
				ret = rtu.PresetSingleRegister(addr, 0x32, (1 << 7));
				//恒功率
				ret = rtu.PresetSingleRegister(addr, 0x32, (1 << 2));
				break;
			default:
				break;
		}
	}

	// 设定电流
	if(MemDb::GetRealData(realData, regStart + 33, true)){
		i_data = realData.pv.data.iValue * 100;
		rtu.PresetSingleRegister(addr, 0x33, (i_data >> 16) & 0xFFFF);
		rtu.PresetSingleRegister(addr, 0x34, i_data  & 0xFFFF);
	}

	// 设定电压
	if(MemDb::GetRealData(realData, regStart + 34, true)){
		i_data = realData.pv.data.iValue * 10;
		rtu.PresetSingleRegister(addr, 0x35, (i_data >> 16) & 0xFFFF);
		rtu.PresetSingleRegister(addr, 0x36, i_data  & 0xFFFF);
	}


	// 设定功率
	if(MemDb::GetRealData(realData, regStart + 35, true)){
		i_data = realData.pv.data.iValue * 10;
		rtu.PresetSingleRegister(addr, 0x37, (i_data >> 16) & 0xFFFF);
		rtu.PresetSingleRegister(addr, 0x38, i_data  & 0xFFFF);
	}

	// 加载时间，小时
	if(MemDb::GetRealData(realData, regStart + 36, true)){
		i_data = realData.pv.data.iValue & 0xFFFF;
		rtu.PresetSingleRegister(addr, 0x39, i_data);
	}

	// 加载时间，分钟
	if(MemDb::GetRealData(realData, regStart + 37, true)){
		i_data = realData.pv.data.iValue & 0xFF;
		rtu.PresetSingleRegister(addr, 0x39, (i_data) << 8);
	}

	// 加载时间，秒
	if(MemDb::GetRealData(realData, regStart + 38, true)){
		i_data = realData.pv.data.iValue & 0xFF;
		rtu.PresetSingleRegister(addr, 0x39, i_data);
	}



	if( ErrorInfo::ERR_OK != ret) {
		zlog_warn(Util::m_zlog, "%s 下发控制指令失败 code:%d", dev->name.c_str(), ret);
		return ErrorInfo::ERR_FAILED;
	}

	return ret;

}