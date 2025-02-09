#include "FlyWheel.h"

FlyWheel::FlyWheel() {
	m_tcp = NULL;
}

FlyWheel::~FlyWheel() {

}


void FlyWheel::SetTcp(TcpClient* tcpClient) {
	this->m_tcp = tcpClient;
}



int FlyWheel::Read(Device::SlaveDev *dev) {
	int ret = ErrorInfo::ERR_OK;
	int start = 0;
	int regStart = dev->regStart;

	bool comm = false;

	UshortArray values;
	values.clear();

	int addr = atoi(dev->slaveAddr.c_str());

	float fdata = 0.0f;
	int idata = 0;
	bool status = 0;



	int length = 0;

	// zlog_warn(Util::m_zlog, "飞轮bms读线程");


	if( NULL == dev){
		zlog_warn(Util::m_zlog, "从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}
	if(!m_tcp->IsOpened()){
		return ErrorInfo::ERR_OPENED;
	}

	ModbusTcpClient client;
	client.SetTcpClient(this->m_tcp);

	start = 2;
	length = 6;

	values.clear();
	ret = client.ReadOutputRegisters(values, addr, start, length);

	if(ErrorInfo::ERR_OK == ret) {
		comm = true;

		idata = values[0]; 			MemDb::SetRealData(regStart + 1, idata, false); // 启动， 停止

		idata = (short) values[1]; 			MemDb::SetRealData(regStart + 2, idata, false); // 功率输入

		idata = values[2]; 			MemDb::SetRealData(regStart + 3, idata, false); // 系统复位

		idata = values[3];  		MemDb::SetRealData(regStart + 4, idata, false); // 预留
		idata = values[4];  		MemDb::SetRealData(regStart + 5, idata, false); // 预留
		idata = values[5];  		MemDb::SetRealData(regStart + 6, idata, false); // 预留
		idata = values[6];  		MemDb::SetRealData(regStart + 7, idata, false); // 预留
		idata = values[7];  		MemDb::SetRealData(regStart + 8, idata, false); // 预留
		idata = values[8];  		MemDb::SetRealData(regStart + 9, idata, false); // 预留
		idata = values[9];  		MemDb::SetRealData(regStart + 10, idata, false); // 预留




	}
	else{
		comm =false;
		zlog_warn(Util::m_zlog,"%s 读取数据失败",dev->name.c_str());
	}

	start = 9;
	length = 25;

	values.clear();
	// 
	ret = client.ReadOutputRegisters(values, addr, start, length);
	if(ErrorInfo::ERR_OK == ret) {
		comm = true;

		// 飞轮通讯状态
		status = values[0] & 0b1; 			MemDb::SetRealData(regStart + 11, status,false);
		// 飞轮状态
		status = (values[1] >> 0) & 0b1;  			MemDb::SetRealData(regStart + 12, status, false);
		status = (values[1] >> 1) & 0b1;  			MemDb::SetRealData(regStart + 13, status, false);
		status = (values[1] >> 2) & 0b1;  			MemDb::SetRealData(regStart + 14, status, false);
		status = (values[1] >> 3) & 0b1;  			MemDb::SetRealData(regStart + 15, status, false);
		status = (values[1] >> 4) & 0b1;  			MemDb::SetRealData(regStart + 16, status, false);
		status = (values[1] >> 5) & 0b1;  			MemDb::SetRealData(regStart + 17, status, false);
		status = (values[1] >> 6) & 0b1;  			MemDb::SetRealData(regStart + 18, status, false);
		status = (values[1] >> 7) & 0b1;  			MemDb::SetRealData(regStart + 19, status, false);
		status = (values[1] >> 8) & 0b1;  			MemDb::SetRealData(regStart + 20, status, false);
		status = (values[1] >> 9) & 0b1;  			MemDb::SetRealData(regStart + 21, status, false);

		// 预留
		idata = 0;  			MemDb::SetRealData(regStart + 22, idata, false);
		idata = 0;  			MemDb::SetRealData(regStart + 23, idata, false);
		idata = 0;  			MemDb::SetRealData(regStart + 24, idata, false);

		//BB1 故障
		status = (values[2] >> 0) & 0b1; 			MemDb::SetRealData(regStart + 25, status, false);
		status = (values[2] >> 1) & 0b1; 			MemDb::SetRealData(regStart + 26, status, false);
		status = (values[2] >> 2) & 0b1; 			MemDb::SetRealData(regStart + 27, status, false);
		status = (values[2] >> 3) & 0b1; 			MemDb::SetRealData(regStart + 28, status, false);
		status = (values[2] >> 5) & 0b1; 			MemDb::SetRealData(regStart + 29, status, false);
		status = (values[2] >> 6) & 0b1; 			MemDb::SetRealData(regStart + 30, status, false);
		status = (values[2] >> 7) & 0b1; 			MemDb::SetRealData(regStart + 31, status, false);
		status = (values[2] >> 9) & 0b1; 			MemDb::SetRealData(regStart + 32, status, false);
		status = (values[2] >> 10) & 0b1; 			MemDb::SetRealData(regStart + 33, status, false);
		status = (values[2] >> 12) & 0b1; 			MemDb::SetRealData(regStart + 34, status, false);


		idata = 0;  								MemDb::SetRealData(regStart + 35, status, false);
		idata = 0;  								MemDb::SetRealData(regStart + 36, status, false);
		idata = 0;  								MemDb::SetRealData(regStart + 37, status, false);


		// BB2 故障
		status = (values[3] >> 0) & 0b1; 			MemDb::SetRealData(regStart + 38, status, false);
		status = (values[3] >> 1) & 0b1; 			MemDb::SetRealData(regStart + 39, status, false);
		status = (values[3] >> 2) & 0b1; 			MemDb::SetRealData(regStart + 40, status, false);
		status = (values[3] >> 3) & 0b1; 			MemDb::SetRealData(regStart + 41, status, false);
		status = (values[3] >> 4) & 0b1; 			MemDb::SetRealData(regStart + 42, status, false);
		status = (values[3] >> 5) & 0b1; 			MemDb::SetRealData(regStart + 43, status, false);
		status = (values[3] >> 6) & 0b1; 			MemDb::SetRealData(regStart + 44, status, false);
		status = (values[3] >> 10) & 0b1; 			MemDb::SetRealData(regStart + 45, status, false);

		idata = 0;  								MemDb::SetRealData(regStart + 46, status, false);
		idata = 0;  								MemDb::SetRealData(regStart + 47, status, false);
		idata = 0;  								MemDb::SetRealData(regStart + 48, status, false);

		// BB3 故障
		status = (values[4] >> 0) & 0b1; 			MemDb::SetRealData(regStart + 49, status, false);
		status = (values[4] >> 1) & 0b1; 			MemDb::SetRealData(regStart + 50, status, false);
		status = (values[4] >> 2) & 0b1; 			MemDb::SetRealData(regStart + 51, status, false);
		status = (values[4] >> 4) & 0b1; 			MemDb::SetRealData(regStart + 52, status, false);
		status = (values[4] >> 6) & 0b1; 			MemDb::SetRealData(regStart + 53, status, false);
		status = (values[4] >> 7) & 0b1; 			MemDb::SetRealData(regStart + 54, status, false);
		status = (values[4] >> 8) & 0b1; 			MemDb::SetRealData(regStart + 55, status, false);
		status = (values[4] >> 11) & 0b1; 			MemDb::SetRealData(regStart + 56, status, false);
		status = (values[4] >> 12) & 0b1; 			MemDb::SetRealData(regStart + 57, status, false);

		idata = 0;  								MemDb::SetRealData(regStart + 58, status, false);
		idata = 0;  								MemDb::SetRealData(regStart + 59, status, false);
		idata = 0;  								MemDb::SetRealData(regStart + 60, status, false);

		// BB3 告警
		status = (values[5] >> 4) & 0b1; 			MemDb::SetRealData(regStart + 61, status, false);
		status = (values[5] >> 6) & 0b1; 			MemDb::SetRealData(regStart + 62, status, false);
		status = (values[5] >> 7) & 0b1; 			MemDb::SetRealData(regStart + 63, status, false);
		status = (values[5] >> 8) & 0b1; 			MemDb::SetRealData(regStart + 64, status, false);
		status = (values[5] >> 9) & 0b1; 			MemDb::SetRealData(regStart + 65, status, false);
		status = (values[5] >> 11) & 0b1; 			MemDb::SetRealData(regStart + 66, status, false);
		status = (values[5] >> 12) & 0b1; 			MemDb::SetRealData(regStart + 67, status, false);
		status = (values[5] >> 13) & 0b1; 			MemDb::SetRealData(regStart + 68, status, false);
		status = (values[5] >> 14) & 0b1; 			MemDb::SetRealData(regStart + 69, status, false);
		status = (values[5] >> 15) & 0b1; 			MemDb::SetRealData(regStart + 70, status, false);

		idata = 0;  								MemDb::SetRealData(regStart + 71, status, false);
		idata = 0;  								MemDb::SetRealData(regStart + 72, status, false);
		idata = 0;  								MemDb::SetRealData(regStart + 73, status, false);

		// 接触器状态
		idata = values[6]; 			MemDb::SetRealData(regStart + 74, idata, false);

		// 转速
		idata = values[7]; 			MemDb::SetRealData(regStart + 75, idata, false);

		idata = values[8];  		MemDb::SetRealData(regStart + 76, idata, false);

	
		fdata = (float)(values[9] * 0.1); 			MemDb::SetRealData(regStart + 77, fdata, false); // DC总线电流
		fdata = (float)(values[10] * 0.1); 			MemDb::SetRealData(regStart + 78, fdata, false); // DC总线电压
		fdata = (float)(values[11] * 0.1); 			MemDb::SetRealData(regStart + 79, fdata, false); // 外界温度
		fdata = (float)(values[12] * 0.1); 			MemDb::SetRealData(regStart + 80, fdata, false); // 柜内温度
		fdata = (float)(values[13] * 0.1); 			MemDb::SetRealData(regStart + 81, fdata, false); // 电机温度
		fdata = (float)(values[14] * 0.1); 			MemDb::SetRealData(regStart + 82, fdata, false); // A相IGBT温度
		fdata = (float)(values[15] * 0.1); 			MemDb::SetRealData(regStart + 83, fdata, false); // B相IGBT温度
		fdata = (float)(values[16] * 0.1); 			MemDb::SetRealData(regStart + 84, fdata, false); // C相IGBT温度
		fdata = (float)(values[17] * 0.1); 			MemDb::SetRealData(regStart + 85, fdata, false); // 真空度
		idata = values[18]; 						MemDb::SetRealData(regStart + 86, idata, false); // 故障计数
		idata = values[19]; 						MemDb::SetRealData(regStart + 87, idata, false); // 运行时间
		idata = values[20]; 						MemDb::SetRealData(regStart + 88, idata, false); // 旋转时间
		idata = values[21]; 						MemDb::SetRealData(regStart + 89, idata, false); // 总放电能量
		idata = values[22]; 						MemDb::SetRealData(regStart + 90, idata, false); // 总循环次数
		idata = values[23]; 						MemDb::SetRealData(regStart + 91, idata, false); // 磁悬浮校正
		idata = values[24]; 						MemDb::SetRealData(regStart + 92, idata, false); // 磁悬浮配置

	}
	else{
		comm =false;
		zlog_warn(Util::m_zlog,"%s 读取数据失败",dev->name.c_str());
	}



	start = 150;
	length = 4;

	values.clear();
	// 读取空调状态
	ret = client.ReadOutputRegisters(values, addr, start, length);

	if(ret == ErrorInfo::ERR_OK) {
		comm = true;

		// 空调1系统状态
		status = (values[0] >> 0) & 0b1; 				MemDb::SetRealData(regStart + 100, status, false);
		status = (values[0] >> 1) & 0b1; 				MemDb::SetRealData(regStart + 101, status, false);
		status = (values[0] >> 2) & 0b1; 				MemDb::SetRealData(regStart + 102, status, false);
		status = (values[0] >> 3) & 0b1; 				MemDb::SetRealData(regStart + 103, status, false);
		status = (values[0] >> 4) & 0b1; 				MemDb::SetRealData(regStart + 104, status, false);
		status = (values[0] >> 5) & 0b1; 				MemDb::SetRealData(regStart + 105, status, false);
		status = (values[0] >> 7) & 0b1; 				MemDb::SetRealData(regStart + 106, status, false);

		//预留
		status = 0; 				MemDb::SetRealData(regStart + 107, status, false);
		status = 0; 				MemDb::SetRealData(regStart + 108, status, false);

		// 空调1 故障01
		status = (values[1] >> 0) & 0b1; 				MemDb::SetRealData(regStart + 109, status, false);
		status = (values[1] >> 1) & 0b1; 				MemDb::SetRealData(regStart + 110, status, false);
		status = (values[1] >> 2) & 0b1; 				MemDb::SetRealData(regStart + 111, status, false);
		status = (values[1] >> 3) & 0b1; 				MemDb::SetRealData(regStart + 112, status, false);
		status = (values[1] >> 4) & 0b1; 				MemDb::SetRealData(regStart + 113, status, false);
		status = (values[1] >> 5) & 0b1; 				MemDb::SetRealData(regStart + 114, status, false);
		status = (values[1] >> 6) & 0b1; 				MemDb::SetRealData(regStart + 115, status, false);
		status = (values[1] >> 7) & 0b1; 				MemDb::SetRealData(regStart + 116, status, false);
		status = (values[1] >> 8) & 0b1; 				MemDb::SetRealData(regStart + 117, status, false);
		status = (values[1] >> 9) & 0b1; 				MemDb::SetRealData(regStart + 118, status, false);
		status = (values[1] >> 10) & 0b1; 				MemDb::SetRealData(regStart + 119, status, false);

		//预留
		status = 0; 				MemDb::SetRealData(regStart + 120, status, false);
		status = 0; 				MemDb::SetRealData(regStart + 121, status, false);


		// 空调1告警02
		status = (values[2] >> 0) & 0b1; 				MemDb::SetRealData(regStart + 122, status, false);
		status = (values[2] >> 1) & 0b1; 				MemDb::SetRealData(regStart + 123, status, false);
		status = (values[2] >> 2) & 0b1; 				MemDb::SetRealData(regStart + 124, status, false);
		status = (values[2] >> 3) & 0b1; 				MemDb::SetRealData(regStart + 125, status, false);
		status = (values[2] >> 4) & 0b1; 				MemDb::SetRealData(regStart + 126, status, false);
		status = (values[2] >> 5) & 0b1; 				MemDb::SetRealData(regStart + 127, status, false);
		status = (values[2] >> 6) & 0b1; 				MemDb::SetRealData(regStart + 128, status, false);
		status = (values[2] >> 7) & 0b1; 				MemDb::SetRealData(regStart + 129, status, false);
		status = (values[2] >> 8) & 0b1; 				MemDb::SetRealData(regStart + 130, status, false);
		status = (values[2] >> 9) & 0b1; 				MemDb::SetRealData(regStart + 131, status, false);
		status = (values[2] >> 10) & 0b1; 				MemDb::SetRealData(regStart + 132, status, false);
		status = (values[2] >> 11) & 0b1; 				MemDb::SetRealData(regStart + 133, status, false);
		status = (values[2] >> 12) & 0b1; 				MemDb::SetRealData(regStart + 134, status, false);
		status = (values[2] >> 13) & 0b1; 				MemDb::SetRealData(regStart + 135, status, false);
		status = (values[2] >> 14) & 0b1; 				MemDb::SetRealData(regStart + 136, status, false);
		status = (values[2] >> 15) & 0b1; 				MemDb::SetRealData(regStart + 137, status, false);

		//预留
		status = 0; 				MemDb::SetRealData(regStart + 138, status, false);
		status = 0; 				MemDb::SetRealData(regStart + 139, status, false);

		// 空调1告警故障03
		status = (values[3] >> 0) & 0b1; 				MemDb::SetRealData(regStart + 140, status, false);
		status = (values[3] >> 1) & 0b1; 				MemDb::SetRealData(regStart + 141, status, false);
		status = (values[3] >> 2) & 0b1; 				MemDb::SetRealData(regStart + 142, status, false);
		status = (values[3] >> 3) & 0b1; 				MemDb::SetRealData(regStart + 143, status, false);
		status = (values[3] >> 4) & 0b1; 				MemDb::SetRealData(regStart + 144, status, false);
		status = (values[3] >> 5) & 0b1; 				MemDb::SetRealData(regStart + 145, status, false);
		status = (values[3] >> 6) & 0b1; 				MemDb::SetRealData(regStart + 146, status, false);
		status = (values[3] >> 7) & 0b1; 				MemDb::SetRealData(regStart + 147, status, false);
		status = (values[3] >> 8) & 0b1; 				MemDb::SetRealData(regStart + 148, status, false);
		status = (values[3] >> 9) & 0b1; 				MemDb::SetRealData(regStart + 149, status, false);
		status = (values[3] >> 10) & 0b1; 				MemDb::SetRealData(regStart + 150, status, false);
		status = (values[3] >> 11) & 0b1; 				MemDb::SetRealData(regStart + 151, status, false);
		status = (values[3] >> 12) & 0b1; 				MemDb::SetRealData(regStart + 152, status, false);

		//预留
		status = 0; 				MemDb::SetRealData(regStart + 153, status, false);
		status = 0; 				MemDb::SetRealData(regStart + 154, status, false);

		


	}
	else{
		comm =false;
		zlog_warn(Util::m_zlog,"%s 读取数据失败",dev->name.c_str());
	}

	start = 160;
	length = 4;

	values.clear();

	// 读取空调状态
	ret = client.ReadOutputRegisters(values, addr, start, length);

	if(ErrorInfo::ERR_OK == ret) {
		comm = true;

		// 空调2 系统状态
		status = (values[0] >> 0) & 0b1; 				MemDb::SetRealData(regStart + 155, status, false);
		status = (values[0] >> 1) & 0b1; 				MemDb::SetRealData(regStart + 156, status, false);
		status = (values[0] >> 2) & 0b1; 				MemDb::SetRealData(regStart + 157, status, false);
		status = (values[0] >> 3) & 0b1; 				MemDb::SetRealData(regStart + 158, status, false);
		status = (values[0] >> 4) & 0b1; 				MemDb::SetRealData(regStart + 159, status, false);
		status = (values[0] >> 5) & 0b1; 				MemDb::SetRealData(regStart + 160, status, false);
		status = (values[0] >> 7) & 0b1; 				MemDb::SetRealData(regStart + 161, status, false);

		// 预留
		status = 0; 				MemDb::SetRealData(regStart + 162, status, false);
		status = 0; 				MemDb::SetRealData(regStart + 163, status, false);

		// 空调2 故障01
		status = (values[1] >> 0) & 0b1; 				MemDb::SetRealData(regStart + 164, status, false);
		status = (values[1] >> 1) & 0b1; 				MemDb::SetRealData(regStart + 165, status, false);
		status = (values[1] >> 2) & 0b1; 				MemDb::SetRealData(regStart + 166, status, false);
		status = (values[1] >> 3) & 0b1; 				MemDb::SetRealData(regStart + 167, status, false);
		status = (values[1] >> 4) & 0b1; 				MemDb::SetRealData(regStart + 168, status, false);
		status = (values[1] >> 5) & 0b1; 				MemDb::SetRealData(regStart + 169, status, false);
		status = (values[1] >> 6) & 0b1; 				MemDb::SetRealData(regStart + 170, status, false);
		status = (values[1] >> 7) & 0b1; 				MemDb::SetRealData(regStart + 171, status, false);
		status = (values[1] >> 8) & 0b1; 				MemDb::SetRealData(regStart + 172, status, false);
		status = (values[1] >> 9) & 0b1; 				MemDb::SetRealData(regStart + 173, status, false);
		status = (values[1] >> 10) & 0b1; 				MemDb::SetRealData(regStart + 174, status, false);

		// 预留
		status = 0; 				MemDb::SetRealData(regStart + 175, status, false);
		status = 0; 				MemDb::SetRealData(regStart + 176, status, false);

		//空调2告警02
		status = (values[2] >> 0) & 0b1; 				MemDb::SetRealData(regStart + 177, status, false);
		status = (values[2] >> 1) & 0b1; 				MemDb::SetRealData(regStart + 178, status, false);
		status = (values[2] >> 2) & 0b1; 				MemDb::SetRealData(regStart + 179, status, false);
		status = (values[2] >> 3) & 0b1; 				MemDb::SetRealData(regStart + 180, status, false);
		status = (values[2] >> 4) & 0b1; 				MemDb::SetRealData(regStart + 181, status, false);
		status = (values[2] >> 5) & 0b1; 				MemDb::SetRealData(regStart + 182, status, false);
		status = (values[2] >> 6) & 0b1; 				MemDb::SetRealData(regStart + 183, status, false);
		status = (values[2] >> 7) & 0b1; 				MemDb::SetRealData(regStart + 184, status, false);
		status = (values[2] >> 8) & 0b1; 				MemDb::SetRealData(regStart + 185, status, false);
		status = (values[2] >> 9) & 0b1; 				MemDb::SetRealData(regStart + 186, status, false);
		status = (values[2] >> 10) & 0b1; 				MemDb::SetRealData(regStart + 187, status, false);
		status = (values[2] >> 11) & 0b1; 				MemDb::SetRealData(regStart + 188, status, false);
		status = (values[2] >> 12) & 0b1; 				MemDb::SetRealData(regStart + 189, status, false);
		status = (values[2] >> 13) & 0b1; 				MemDb::SetRealData(regStart + 190, status, false);
		status = (values[2] >> 14) & 0b1; 				MemDb::SetRealData(regStart + 191, status, false);
		status = (values[2] >> 15) & 0b1; 				MemDb::SetRealData(regStart + 192, status, false);

		// 预留
		status = 0; 				MemDb::SetRealData(regStart + 193, status, false);
		status = 0; 				MemDb::SetRealData(regStart + 194, status, false);

		// 空调2告警03
		status = (values[3] >> 0) & 0b1; 				MemDb::SetRealData(regStart + 195, status, false);
		status = (values[3] >> 1) & 0b1; 				MemDb::SetRealData(regStart + 196, status, false);
		status = (values[3] >> 2) & 0b1; 				MemDb::SetRealData(regStart + 197, status, false);
		status = (values[3] >> 3) & 0b1; 				MemDb::SetRealData(regStart + 198, status, false);
		status = (values[3] >> 4) & 0b1; 				MemDb::SetRealData(regStart + 199, status, false);
		status = (values[3] >> 5) & 0b1; 				MemDb::SetRealData(regStart + 200, status, false);
		status = (values[3] >> 6) & 0b1; 				MemDb::SetRealData(regStart + 201, status, false);
		status = (values[3] >> 7) & 0b1; 				MemDb::SetRealData(regStart + 202, status, false);
		status = (values[3] >> 8) & 0b1; 				MemDb::SetRealData(regStart + 203, status, false);
		status = (values[3] >> 9) & 0b1; 				MemDb::SetRealData(regStart + 204, status, false);
		status = (values[3] >> 10) & 0b1; 				MemDb::SetRealData(regStart + 205, status, false);
		status = (values[3] >> 11) & 0b1; 				MemDb::SetRealData(regStart + 206, status, false);
		status = (values[3] >> 12) & 0b1; 				MemDb::SetRealData(regStart + 207, status, false);

		// 预留
		status = 0; 				MemDb::SetRealData(regStart + 208, status, false);
		status = 0; 				MemDb::SetRealData(regStart + 209, status, false);

	}
	else{
		comm =false;
		zlog_warn(Util::m_zlog,"%s 读取数据失败",dev->name.c_str());
	}


	start = 169;
	length = 2;

	values.clear();

	// 读取空调状态
	ret = client.ReadOutputRegisters(values, addr, start, length);

	if(ErrorInfo::ERR_OK == ret) {
		comm = true;

		// 散热风扇状态
		status = (values[0] >> 0) & 0b1; 				MemDb::SetRealData(regStart + 210, status, false);
		status = (values[0] >> 1) & 0b1; 				MemDb::SetRealData(regStart + 211, status, false);
		status = (values[0] >> 2) & 0b1; 				MemDb::SetRealData(regStart + 212, status, false);

		// 火警控制器状体啊
		status = (values[1] >> 0) & 0b1; 				MemDb::SetRealData(regStart + 213, status, false);
		status = (values[1] >> 1) & 0b1; 				MemDb::SetRealData(regStart + 214, status, false);
		status = (values[1] >> 2) & 0b1; 				MemDb::SetRealData(regStart + 215, status, false);

	}
	else{
		comm =false;
		zlog_warn(Util::m_zlog,"%s 读取数据失败",dev->name.c_str());
	}


	MemDb::SetRealData(regStart + 0, comm, false);


	return ret;

}


int FlyWheel::Preset(Device::SlaveDev *dev) {

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


	if(MemDb::GetRealData(realData, regStart + 221, true)){
		ret = client.PresetSingleRegister(addr, 2, realData.pv.data.iValue);
	}

	if(MemDb::GetRealData(realData, regStart + 222, true)){
		short power = (short)realData.pv.data.iValue;

		zlog_warn(Util::m_zlog, "下发功率指令: %d", power);
		ret = client.PresetSingleRegister(addr, 3, power);
	}

	if(MemDb::GetRealData(realData, regStart + 223, true)){
		ret = client.PresetSingleRegister(addr, 4, realData.pv.data.iValue);
	}

	if( ErrorInfo::ERR_OK != ret) {
		zlog_warn(Util::m_zlog, "%s 下发控制指令失败 code:%d", dev->name.c_str(), ret);
		return ErrorInfo::ERR_FAILED;
	}

	return ret;

}





