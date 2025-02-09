#include "AquaNa.h"



AquaNa::AquaNa(){
	m_tcp = NULL;
}


AquaNa::~AquaNa(){

}

void AquaNa::SetTcp(TcpClient *tcpClient){
	m_tcp = tcpClient;
}


int AquaNa::Read(Device::SlaveDev *dev){
	int ret = ErrorInfo::ERR_OK;

	int start = 0;
	// 第一组 实时数据
	ret = BattGroupRealTimeData(dev, dev->regStart + 0, start);

	// 第二组 实时数据
	ret = BattGroupRealTimeData(dev, dev->regStart + 200, start + 100);

	// 第三组 实时数据
	ret = BattGroupRealTimeData(dev, dev->regStart + 400, start + 200);

	// 第四组 实时数据
	ret = BattGroupRealTimeData(dev, dev->regStart + 600, start + 300);

	// 第五组 实时数据
	ret = BattGroupRealTimeData(dev, dev->regStart + 800, start + 400);

	// 读取设备状态信息 regStart起始1000， start 600
	ret = DeviceStatusInfo(dev);

	// 读取组一，单体电池信息,
	int singleBattCount = 420;
	ret = SingleBattInfo(dev, dev->regStart + 2000, start + 1000, singleBattCount);

	// 读取组二，单体电池信息,
	singleBattCount = 210;
	ret = SingleBattInfo(dev, dev->regStart + 3000, start + 7000, singleBattCount);

	// 读取组三，单体电池信息,
	singleBattCount = 140;
	ret = SingleBattInfo(dev, dev->regStart + 4000, start + 10000, singleBattCount);

	// 读取组四，单体电池信息
	singleBattCount = 70;
	ret = SingleBattInfo(dev, dev->regStart + 5000, start + 12000, singleBattCount);

	// 读取组五，单体电池信息
	singleBattCount = 70;
	ret = SingleBattInfo(dev, dev->regStart + 6000, start + 13000, singleBattCount);


	//设备状态信息
	ret = DeviceStatusInfo(dev);




	return ret;
}

int AquaNa::BattGroupRealTimeData(Device::SlaveDev* dev, int regStart, int start){
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;

	UshortArray values;
	values.clear();

	int addr = atoi(dev->slaveAddr.c_str());

	float fdata = 0.0f;
	int idata = 0;
	bool status = 0;


	int length = 0;

	zlog_warn(Util::m_zlog, "水离子电池读线程");


	if( NULL == dev){
		zlog_warn(Util::m_zlog, "从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}
	if(!m_tcp->IsOpened()){
		return ErrorInfo::ERR_OPENED;
	}

	ModbusTcpClient client;
	client.SetTcpClient(this->m_tcp);

	length = 60;

	ret = client.ReadOutputRegisters(values, addr, start, length);

	if(ErrorInfo::ERR_OK == ret){
		comm = true;

		fdata = (((unsigned short)values[1] << 16) + (unsigned short)values[0]) * 0.01;
		zlog_warn(Util::m_zlog, "组体电压: %f, [0]: %d, [1]: %d\n", fdata, values[0], values[1]);
		MemDb::SetRealData(regStart + 1, fdata, false); // 组体电压

		fdata = 0;
		MemDb::SetRealData(regStart + 2, fdata, false); // 预留
		MemDb::SetRealData(regStart + 3, fdata, false); // 预留
		MemDb::SetRealData(regStart + 4, fdata, false); // 预留

		fdata = (signed int)((values[7] << 16) + values[6]) * 0.01; // 组体电流;
		zlog_warn(Util::m_zlog, "组体电流: %f\n", fdata);

		MemDb::SetRealData(regStart + 5, fdata, false);

		fdata = (signed int)((values[8] << 16) + values[9]) * 0.01;
		MemDb::SetRealData(regStart + 6, fdata, false);  // 纹波电流

		fdata = (unsigned int)((values[10] << 16) + values[11]) * 0.01;
		MemDb::SetRealData(regStart + 7, fdata, false); // 纹波电压

		idata = values[12];
		MemDb::SetRealData(regStart + 8, idata, false); //SOC

		idata = values[13];
		MemDb::SetRealData(regStart + 9, idata, false); //预留

		idata = values[14] * 0.1;
		MemDb::SetRealData(regStart + 10, idata, false); //均衡度

		idata = values[15];
		MemDb::SetRealData(regStart + 11, idata, false); // 组体电压偏差度

		idata = values[16];
		MemDb::SetRealData(regStart + 12, idata, false); //电池组状态


		idata = values[17]; 			MemDb::SetRealData(regStart + 13, idata, false); //组模块连接状态

		idata = values[18]; 			MemDb::SetRealData(regStart + 14, idata, false); //预留

		fdata = values[19] * 0.1; 			MemDb::SetRealData(regStart + 15, fdata, false); //组温度

		idata = values[20]; 			MemDb::SetRealData(regStart + 16, idata, false); // 电池数量

		status = values[21] >> 0 & 0b1; 			MemDb::SetRealData(regStart + 17, status, false); //一组体告警1
		status = values[21] >> 1 & 0b1; 			MemDb::SetRealData(regStart + 18, status, false); //一组体告警2
		status = values[21] >> 2 & 0b1; 			MemDb::SetRealData(regStart + 19, status, false); //一组体告警3
		status = values[21] >> 3 & 0b1; 			MemDb::SetRealData(regStart + 20, status, false); //一组体告警4
		status = values[21] >> 4 & 0b1; 			MemDb::SetRealData(regStart + 21, status, false); //一组体告警5
		status = values[21] >> 5 & 0b1; 			MemDb::SetRealData(regStart + 22, status, false); //一组体告警6
		status = values[21] >> 6 & 0b1; 			MemDb::SetRealData(regStart + 23, status, false); //一组体告警7
		status = values[21] >> 7 & 0b1; 			MemDb::SetRealData(regStart + 24, status, false); //一组体告警8
		status = values[21] >> 8 & 0b1; 			MemDb::SetRealData(regStart + 25, status, false); //一组体告警9
		status = values[21] >> 9 & 0b1; 			MemDb::SetRealData(regStart + 26, status, false); //一组体告警10
		status = values[21] >> 10 & 0b1; 			MemDb::SetRealData(regStart + 27, status, false); //一组体告警11
		status = values[21] >> 11 & 0b1; 			MemDb::SetRealData(regStart + 28, status, false); //一组体告警12
		status = values[21] >> 12 & 0b1; 			MemDb::SetRealData(regStart + 29, status, false); //一组体告警13
		status = values[21] >> 13 & 0b1; 			MemDb::SetRealData(regStart + 30, status, false); //一组体告警14
		status = values[21] >> 14 & 0b1; 			MemDb::SetRealData(regStart + 31, status, false); //一组体告警15
		status = values[21] >> 15 & 0b1; 			MemDb::SetRealData(regStart + 32, status, false); //一组体告警16

		status = values[22] >> 0 & 0b1; 			MemDb::SetRealData(regStart + 33, status, false); //二组体告警01
		status = values[22] >> 1 & 0b1; 			MemDb::SetRealData(regStart + 34, status, false); //二组体告警02
		status = values[22] >> 2 & 0b1; 			MemDb::SetRealData(regStart + 35, status, false); //二组体告警03
		status = values[22] >> 3 & 0b1; 			MemDb::SetRealData(regStart + 36, status, false); //二组体告警04
		status = values[22] >> 4 & 0b1; 			MemDb::SetRealData(regStart + 37, status, false); //二组体告警05
		status = values[22] >> 5 & 0b1; 			MemDb::SetRealData(regStart + 38, status, false); //二组体告警06
		status = values[22] >> 6 & 0b1; 			MemDb::SetRealData(regStart + 39, status, false); //二组体告警07
		status = values[22] >> 7 & 0b1; 			MemDb::SetRealData(regStart + 40, status, false); //二组体告警08
		status = values[22] >> 8 & 0b1; 			MemDb::SetRealData(regStart + 41, status, false); //二组体告警09
		status = values[22] >> 9 & 0b1; 			MemDb::SetRealData(regStart + 42, status, false); //二组体告警10
		status = values[22] >> 10 & 0b1; 			MemDb::SetRealData(regStart + 43, status, false); //二组体告警11
		status = values[22] >> 11 & 0b1; 			MemDb::SetRealData(regStart + 44, status, false); //二组体告警12
		status = values[22] >> 12 & 0b1; 			MemDb::SetRealData(regStart + 45, status, false); //二组体告警13
		status = values[22] >> 13 & 0b1; 			MemDb::SetRealData(regStart + 46, status, false); //二组体告警14
		status = values[22] >> 14 & 0b1; 			MemDb::SetRealData(regStart + 47, status, false); //二组体告警15
		status = values[22] >> 15 & 0b1; 			MemDb::SetRealData(regStart + 48, status, false); //二组体告警16

		status = values[24] >> 0 & 0b1; 			MemDb::SetRealData(regStart + 49, status, false); //  一级单体告警状态01;
		status = values[24] >> 1 & 0b1; 			MemDb::SetRealData(regStart + 50, status, false); //  一级单体告警状态02;
		status = values[24] >> 2 & 0b1; 			MemDb::SetRealData(regStart + 51, status, false); //  一级单体告警状态03;
		status = values[24] >> 3 & 0b1; 			MemDb::SetRealData(regStart + 52, status, false); //  一级单体告警状态04;
		status = values[24] >> 4 & 0b1; 			MemDb::SetRealData(regStart + 53, status, false); //  一级单体告警状态05;
		status = values[24] >> 5 & 0b1; 			MemDb::SetRealData(regStart + 54, status, false); //  一级单体告警状态06;
		status = values[24] >> 6 & 0b1; 			MemDb::SetRealData(regStart + 55, status, false); //  一级单体告警状态07;
		status = values[24] >> 7 & 0b1; 			MemDb::SetRealData(regStart + 56, status, false); //  一级单体告警状态08;
		status = values[24] >> 8 & 0b1; 			MemDb::SetRealData(regStart + 57, status, false); //  一级单体告警状态09;
		status = values[24] >> 9 & 0b1; 			MemDb::SetRealData(regStart + 58, status, false); //  一级单体告警状态10;
		status = values[24] >> 10 & 0b1; 			MemDb::SetRealData(regStart + 59, status, false); //  一级单体告警状态11;
		status = values[24] >> 11 & 0b1; 			MemDb::SetRealData(regStart + 60, status, false); //  一级单体告警状态12;
		status = values[24] >> 12 & 0b1; 			MemDb::SetRealData(regStart + 61, status, false); //  一级单体告警状态13;
		status = values[24] >> 13 & 0b1; 			MemDb::SetRealData(regStart + 62, status, false); //  一级单体告警状态14;
		status = values[24] >> 14 & 0b1; 			MemDb::SetRealData(regStart + 63, status, false); //  一级单体告警状态15;
		status = values[24] >> 15 & 0b1; 			MemDb::SetRealData(regStart + 64, status, false); //  一级单体告警状态16;

		status = values[25] >> 0 & 0b1; 			MemDb::SetRealData(regStart + 65, status, false); // 二级单体告警状态01;
		status = values[25] >> 1 & 0b1; 			MemDb::SetRealData(regStart + 66, status, false); // 二级单体告警状态02;
		status = values[25] >> 2 & 0b1; 			MemDb::SetRealData(regStart + 67, status, false); // 二级单体告警状态03;
		status = values[25] >> 3 & 0b1; 			MemDb::SetRealData(regStart + 68, status, false); // 二级单体告警状态04;
		status = values[25] >> 4 & 0b1; 			MemDb::SetRealData(regStart + 69, status, false); // 二级单体告警状态05;
		status = values[25] >> 5 & 0b1; 			MemDb::SetRealData(regStart + 70, status, false); // 二级单体告警状态06;
		status = values[25] >> 6 & 0b1; 			MemDb::SetRealData(regStart + 71, status, false); // 二级单体告警状态07;
		status = values[25] >> 7 & 0b1; 			MemDb::SetRealData(regStart + 72, status, false); // 二级单体告警状态08;
		status = values[25] >> 8 & 0b1; 			MemDb::SetRealData(regStart + 73, status, false); // 二级单体告警状态09;
		status = values[25] >> 9 & 0b1; 			MemDb::SetRealData(regStart + 74, status, false); // 二级单体告警状态10;
		status = values[25] >> 10 & 0b1; 			MemDb::SetRealData(regStart + 75, status, false); // 二级单体告警状态11;
		status = values[25] >> 11 & 0b1; 			MemDb::SetRealData(regStart + 76, status, false); // 二级单体告警状态12;
		status = values[25] >> 12 & 0b1; 			MemDb::SetRealData(regStart + 77, status, false); // 二级单体告警状态13;
		status = values[25] >> 13 & 0b1; 			MemDb::SetRealData(regStart + 78, status, false); // 二级单体告警状态14;
		status = values[25] >> 14 & 0b1; 			MemDb::SetRealData(regStart + 79, status, false); // 二级单体告警状态15;
		status = values[25] >> 15 & 0b1; 			MemDb::SetRealData(regStart + 80, status, false); // 二级单体告警状态16;

		idata = values[27]; 						MemDb::SetRealData(regStart + 81, idata, false); // 内阻测量状态

		idata = values[28]; 						MemDb::SetRealData(regStart + 82, idata, false); // 预留
		idata = values[29]; 						MemDb::SetRealData(regStart + 83, idata, false); // 预留
		idata = values[30]; 						MemDb::SetRealData(regStart + 84, idata, false); // 预留
		idata = values[31]; 						MemDb::SetRealData(regStart + 85, idata, false); // 预留
		idata = values[32]; 						MemDb::SetRealData(regStart + 86, idata, false); // 预留
		idata = values[33]; 						MemDb::SetRealData(regStart + 87, idata, false); // 预留
	
		fdata = (((values[35] << 16) + values[34] ) * 0.001); // 电池内阻平均值
		MemDb::SetRealData(regStart + 88, fdata, false);
		zlog_warn(Util::m_zlog, "电池内阻平均值:%f, [0]: %d, [1]: %d", fdata, values[34], values[35]);

		fdata = ((values[37] << 16) + values[36] ) * 0.001; // 电池内阻最大值
		MemDb::SetRealData(regStart + 89, fdata, false);
		zlog_warn(Util::m_zlog, "内阻最大值:%f, [0]: %d, [1]: %d", fdata, values[36], values[37]);

		fdata = ((values[39] << 16) + values[38] ) * 0.001; // 电池内阻最小值
		MemDb::SetRealData(regStart + 90, fdata, false);
		zlog_warn(Util::m_zlog, "内阻最小值:%f, [0]: %d, [1]: %d", fdata, values[38], values[39]);


		idata = values[40];
		MemDb::SetRealData(regStart + 91, idata, false); //内阻最大 电池标识

		idata = values[41];
		MemDb::SetRealData(regStart + 92, idata, false); //内阻最小 电池标识

		fdata = values[42]; 		MemDb::SetRealData(regStart + 93, fdata, false); // 电池电压平均值
		fdata = values[43]; 		MemDb::SetRealData(regStart + 94, fdata, false); // 电池电压最大值
		fdata = values[44]; 		MemDb::SetRealData(regStart + 95, fdata, false); // 电池电压最小值
		fdata = values[45]; 		MemDb::SetRealData(regStart + 96, fdata, false); // 电池电压最大值编号
		fdata = values[46]; 		MemDb::SetRealData(regStart + 97, fdata, false); // 电池电压最小值编号

		idata = (signed short)values[47]; 		MemDb::SetRealData(regStart + 98, idata, false); // 电池温度平均值
		idata = (signed short)values[48]; 		MemDb::SetRealData(regStart + 99, idata, false); // 电池温度最大值
		idata = (signed short)values[49]; 		MemDb::SetRealData(regStart + 100, idata, false); // 电池温度最小值

		idata = (unsigned short)values[50]; 		MemDb::SetRealData(regStart + 101, idata, false); // 电池温度最小值,电池编号
		idata = (unsigned short)values[51]; 		MemDb::SetRealData(regStart + 102, idata, false); // 电池温度最小值,电池编号

	}
	else{

		zlog_warn(Util::m_zlog, "%s通讯失败, erro: %d\n", dev->desc.c_str(), ret);
	}

	MemDb::SetRealData(dev->regStart + 0, comm, false);
	return ret;

}

int AquaNa::DeviceStatusInfo(Device::SlaveDev* dev){
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;

	UshortArray values;
	values.clear();

	int addr = atoi(dev->slaveAddr.c_str());

	float fdata = 0.0f;
	int idata = 0;
	bool status = false;
	int regStart = dev->regStart;

	int start = 600;
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

	length = 30;

	ret = client.ReadOutputRegisters(values, addr, start, length);
	if(ret == ErrorInfo::ERR_OK){
		comm = true;
		// 内阻测量状态
		status = (values[0] >> 0) & 0b1; 		MemDb::SetRealData(regStart + 1000, status, false);
		status = (values[0] >> 1) & 0b1; 		MemDb::SetRealData(regStart + 1001, status, false);
		status = (values[0] >> 2) & 0b1; 		MemDb::SetRealData(regStart + 1002, status, false);
		status = (values[0] >> 3) & 0b1; 		MemDb::SetRealData(regStart + 1003, status, false);
		status = (values[0] >> 4) & 0b1; 		MemDb::SetRealData(regStart + 1004, status, false);
		status = (values[0] >> 5) & 0b1; 		MemDb::SetRealData(regStart + 1005, status, false);
		status = (values[0] >> 6) & 0b1; 		MemDb::SetRealData(regStart + 1006, status, false);
		status = (values[0] >> 7) & 0b1; 		MemDb::SetRealData(regStart + 1007, status, false);

		//预留
		status = values[1]; 					MemDb::SetRealData(regStart + 1008, status, false);
		status = values[2]; 					MemDb::SetRealData(regStart + 1009, status, false);

		// PBMS60000 指示灯
		status = (values[3] >> 0) & 0b1; 		MemDb::SetRealData(regStart + 1010, status, false);
		status = (values[3] >> 1) & 0b1; 		MemDb::SetRealData(regStart + 1011, status, false);

		// 故障状态
		status = (values[4] >> 0) & 0b1; 		MemDb::SetRealData(regStart + 1012, status, false);
		status = (values[4] >> 1) & 0b1; 		MemDb::SetRealData(regStart + 1013, status, false);
		status = (values[4] >> 2) & 0b1; 		MemDb::SetRealData(regStart + 1014, status, false);


		// 蜂鸣器状态
		status = values[5]; 					MemDb::SetRealData(regStart + 1015, status, false);

		//继电器状态
		status = (values[6] >> 0) & 0b1; 		MemDb::SetRealData(regStart + 1016, status, false);
		status = (values[6] >> 1) & 0b1; 		MemDb::SetRealData(regStart + 1017, status, false);
		status = (values[6] >> 2) & 0b1; 		MemDb::SetRealData(regStart + 1018, status, false);
		status = (values[6] >> 3) & 0b1; 		MemDb::SetRealData(regStart + 1019, status, false);
		status = (values[6] >> 4) & 0b1; 		MemDb::SetRealData(regStart + 1020, status, false);
		status = (values[6] >> 5) & 0b1; 		MemDb::SetRealData(regStart + 1021, status, false);
		status = (values[6] >> 6) & 0b1; 		MemDb::SetRealData(regStart + 1022, status, false);
		status = (values[6] >> 7) & 0b1; 		MemDb::SetRealData(regStart + 1023, status, false);

		// 开关量状态
		status = (values[7] >> 0) & 0b1; 		MemDb::SetRealData(regStart + 1024, status, false);
		status = (values[7] >> 1) & 0b1; 		MemDb::SetRealData(regStart + 1025, status, false);
		status = (values[7] >> 2) & 0b1; 		MemDb::SetRealData(regStart + 1026, status, false);

		status = values[8]; 					MemDb::SetRealData(regStart + 1027, status, false);
		status = values[9]; 					MemDb::SetRealData(regStart + 1028, status, false);

		// 第一路模拟量输入
		idata = (unsigned short)values[10]; 	MemDb::SetRealData(regStart + 1029, idata, false);

		// 第二路模拟量输入
		idata = (unsigned short)values[11]; 	MemDb::SetRealData(regStart + 1030, idata, false);


		// 模拟量输入转换值
		fdata = ((signed short)(values[12])) * 0.001;  			MemDb::SetRealData(regStart + 1031, fdata, false);
		fdata = ((signed short)(values[13])) * 0.001;  			MemDb::SetRealData(regStart + 1032, fdata, false);

		idata = (unsigned short)values[14]; 					MemDb::SetRealData(regStart + 1033, idata, false);
		idata = (unsigned short)values[15]; 					MemDb::SetRealData(regStart + 1034, idata, false);

		// 环境温度
		fdata = ((signed short)values[16]) * 0.1; 	MemDb::SetRealData(regStart + 1035, fdata, false);
		zlog_warn(Util::m_zlog, "环境温度: %f\n", fdata);

		//预留
		idata = (unsigned short)values[17]; 		MemDb::SetRealData(regStart + 1036, idata, false);
		idata = (unsigned short)values[18]; 		MemDb::SetRealData(regStart + 1037, idata, false);
		idata = (unsigned short)values[19]; 		MemDb::SetRealData(regStart + 1038, idata, false);

		// COM正在测量内阻电池标识
		idata = (unsigned short)values[20]; 		MemDb::SetRealData(regStart + 1039, idata, false);
		idata = (unsigned short)values[21]; 		MemDb::SetRealData(regStart + 1040, idata, false);
		idata = (unsigned short)values[22]; 		MemDb::SetRealData(regStart + 1041, idata, false);
		idata = (unsigned short)values[23]; 		MemDb::SetRealData(regStart + 1042, idata, false);
		idata = (unsigned short)values[24]; 		MemDb::SetRealData(regStart + 1043, idata, false);
		idata = (unsigned short)values[25]; 		MemDb::SetRealData(regStart + 1044, idata, false);

	}else{
		zlog_warn(Util::m_zlog, "读取设备状态信息失败: %s\n", dev->desc.c_str());
		comm = false;
		ret = ErrorInfo::ERR_FAILED;
	}

	MemDb::SetRealData(dev->regStart + 0, comm, false);


	return ret;

}


int AquaNa::SingleBattInfo(Device::SlaveDev *dev, int regStart, int start, int singleBattCount){

	int ret = ErrorInfo::ERR_OK;
	bool comm = false;

	UshortArray values;
	values.clear();

	int addr = atoi(dev->slaveAddr.c_str());

	float fdata = 0.0f;
	int idata = 0;
	bool status = false;

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

	length = 30;

	// 读取单体电池标识
	values.clear();
	ret = client.ReadOutputRegisters(values, addr, start + (singleBattCount * 0), length);
	if(ret == ErrorInfo::ERR_OK){
		comm = true;

		idata = values[0];  		MemDb::SetRealData(regStart + 1, idata, false);
		idata = values[1];  		MemDb::SetRealData(regStart + 2, idata, false);
		idata = values[2];  		MemDb::SetRealData(regStart + 3, idata, false);
		idata = values[3];  		MemDb::SetRealData(regStart + 4, idata, false);
		idata = values[4];  		MemDb::SetRealData(regStart + 5, idata, false);
		idata = values[5];  		MemDb::SetRealData(regStart + 6, idata, false);
		idata = values[6];  		MemDb::SetRealData(regStart + 7, idata, false);
		idata = values[7];  		MemDb::SetRealData(regStart + 8, idata, false);
		idata = values[8];  		MemDb::SetRealData(regStart + 9, idata, false);
		idata = values[9];  		MemDb::SetRealData(regStart + 10, idata, false);
		idata = values[10];  		MemDb::SetRealData(regStart + 11, idata, false);
		idata = values[11];  		MemDb::SetRealData(regStart + 12, idata, false);
		idata = values[12];  		MemDb::SetRealData(regStart + 13, idata, false);
		idata = values[13];  		MemDb::SetRealData(regStart + 14, idata, false);
		idata = values[14];  		MemDb::SetRealData(regStart + 15, idata, false);
		idata = values[15];  		MemDb::SetRealData(regStart + 16, idata, false);
		idata = values[16];  		MemDb::SetRealData(regStart + 17, idata, false);
		idata = values[17];  		MemDb::SetRealData(regStart + 18, idata, false);
		idata = values[18];  		MemDb::SetRealData(regStart + 19, idata, false);
		idata = values[19];  		MemDb::SetRealData(regStart + 20, idata, false);
		idata = values[20];  		MemDb::SetRealData(regStart + 21, idata, false);
		idata = values[21];  		MemDb::SetRealData(regStart + 22, idata, false);
		idata = values[22];  		MemDb::SetRealData(regStart + 23, idata, false);
		idata = values[23];  		MemDb::SetRealData(regStart + 24, idata, false);
		idata = values[24];  		MemDb::SetRealData(regStart + 25, idata, false);
		idata = values[25];  		MemDb::SetRealData(regStart + 26, idata, false);
		idata = values[26];  		MemDb::SetRealData(regStart + 27, idata, false);
		idata = values[27];  		MemDb::SetRealData(regStart + 28, idata, false);
		idata = values[28];  		MemDb::SetRealData(regStart + 29, idata, false);
		idata = values[29];  		MemDb::SetRealData(regStart + 30, idata, false);

	}
	else{
		zlog_warn(Util::m_zlog, "读取设备状态信息失败: %s\n", dev->desc.c_str());
		comm = false;
		ret = ErrorInfo::ERR_FAILED;

	}

	length = 30;
	values.clear();
	ret = client.ReadOutputRegisters(values, addr, start + (singleBattCount * 1), length);
	if(ret == ErrorInfo::ERR_OK){
		comm = true;
		// 读取 单体电池通讯状态
		idata = values[0]; 		MemDb::SetRealData(regStart + 31, idata, false);
		idata = values[1]; 		MemDb::SetRealData(regStart + 32, idata, false);
		idata = values[2]; 		MemDb::SetRealData(regStart + 33, idata, false);
		idata = values[3]; 		MemDb::SetRealData(regStart + 34, idata, false);
		idata = values[4]; 		MemDb::SetRealData(regStart + 35, idata, false);
		idata = values[5]; 		MemDb::SetRealData(regStart + 36, idata, false);
		idata = values[6]; 		MemDb::SetRealData(regStart + 37, idata, false);
		idata = values[7]; 		MemDb::SetRealData(regStart + 38, idata, false);
		idata = values[8]; 		MemDb::SetRealData(regStart + 39, idata, false);
		idata = values[9]; 		MemDb::SetRealData(regStart + 40, idata, false);
		idata = values[10]; 		MemDb::SetRealData(regStart + 41, idata, false);
		idata = values[11]; 		MemDb::SetRealData(regStart + 42, idata, false);
		idata = values[12]; 		MemDb::SetRealData(regStart + 43, idata, false);
		idata = values[13]; 		MemDb::SetRealData(regStart + 44, idata, false);
		idata = values[14]; 		MemDb::SetRealData(regStart + 45, idata, false);
		idata = values[15]; 		MemDb::SetRealData(regStart + 46, idata, false);
		idata = values[16]; 		MemDb::SetRealData(regStart + 47, idata, false);
		idata = values[17]; 		MemDb::SetRealData(regStart + 48, idata, false);
		idata = values[18]; 		MemDb::SetRealData(regStart + 49, idata, false);
		idata = values[19]; 		MemDb::SetRealData(regStart + 50, idata, false);
		idata = values[20]; 		MemDb::SetRealData(regStart + 51, idata, false);
		idata = values[21]; 		MemDb::SetRealData(regStart + 52, idata, false);
		idata = values[22]; 		MemDb::SetRealData(regStart + 53, idata, false);
		idata = values[23]; 		MemDb::SetRealData(regStart + 54, idata, false);
		idata = values[24]; 		MemDb::SetRealData(regStart + 55, idata, false);
		idata = values[25]; 		MemDb::SetRealData(regStart + 56, idata, false);
		idata = values[26]; 		MemDb::SetRealData(regStart + 57, idata, false);
		idata = values[27]; 		MemDb::SetRealData(regStart + 58, idata, false);
		idata = values[28]; 		MemDb::SetRealData(regStart + 59, idata, false);
		idata = values[29]; 		MemDb::SetRealData(regStart + 60, idata, false);

	}
	else{
		zlog_warn(Util::m_zlog, "读取设备状态信息失败: %s\n", dev->desc.c_str());
		comm = false;
		ret = ErrorInfo::ERR_FAILED;

	}


	// 读取单体电池电压
	length = 30;
	values.clear();
	ret = client.ReadOutputRegisters(values, addr, start + (singleBattCount * 2), length);
	if(ret == ErrorInfo::ERR_OK){
		comm = true;
		fdata = values[0] * 0.001; 			MemDb::SetRealData(regStart + 61, fdata, false);
		fdata = values[1] * 0.001; 			MemDb::SetRealData(regStart + 62, fdata, false);
		fdata = values[2] * 0.001; 			MemDb::SetRealData(regStart + 63, fdata, false);
		fdata = values[3] * 0.001; 			MemDb::SetRealData(regStart + 64, fdata, false);
		fdata = values[4] * 0.001; 			MemDb::SetRealData(regStart + 65, fdata, false);
		fdata = values[5] * 0.001; 			MemDb::SetRealData(regStart + 66, fdata, false);
		fdata = values[6] * 0.001; 			MemDb::SetRealData(regStart + 67, fdata, false);
		fdata = values[7] * 0.001; 			MemDb::SetRealData(regStart + 68, fdata, false);
		fdata = values[8] * 0.001; 			MemDb::SetRealData(regStart + 69, fdata, false);
		fdata = values[9] * 0.001; 			MemDb::SetRealData(regStart + 70, fdata, false);
		fdata = values[10] * 0.001; 			MemDb::SetRealData(regStart + 71, fdata, false);
		fdata = values[11] * 0.001; 			MemDb::SetRealData(regStart + 72, fdata, false);
		fdata = values[12] * 0.001; 			MemDb::SetRealData(regStart + 73, fdata, false);
		fdata = values[13] * 0.001; 			MemDb::SetRealData(regStart + 74, fdata, false);
		fdata = values[14] * 0.001; 			MemDb::SetRealData(regStart + 75, fdata, false);
		fdata = values[15] * 0.001; 			MemDb::SetRealData(regStart + 76, fdata, false);
		fdata = values[16] * 0.001; 			MemDb::SetRealData(regStart + 77, fdata, false);
		fdata = values[17] * 0.001; 			MemDb::SetRealData(regStart + 78, fdata, false);
		fdata = values[18] * 0.001; 			MemDb::SetRealData(regStart + 79, fdata, false);
		fdata = values[19] * 0.001; 			MemDb::SetRealData(regStart + 80, fdata, false);
		fdata = values[20] * 0.001; 			MemDb::SetRealData(regStart + 81, fdata, false);
		fdata = values[21] * 0.001; 			MemDb::SetRealData(regStart + 82, fdata, false);
		fdata = values[22] * 0.001; 			MemDb::SetRealData(regStart + 83, fdata, false);
		fdata = values[23] * 0.001; 			MemDb::SetRealData(regStart + 84, fdata, false);
		fdata = values[24] * 0.001; 			MemDb::SetRealData(regStart + 85, fdata, false);
		fdata = values[25] * 0.001; 			MemDb::SetRealData(regStart + 86, fdata, false);
		fdata = values[26] * 0.001; 			MemDb::SetRealData(regStart + 87, fdata, false);
		fdata = values[27] * 0.001; 			MemDb::SetRealData(regStart + 88, fdata, false);
		fdata = values[28] * 0.001; 			MemDb::SetRealData(regStart + 89, fdata, false);
		fdata = values[29] * 0.001; 			MemDb::SetRealData(regStart + 90, fdata, false);
	}
	else{
		zlog_warn(Util::m_zlog, "读取单体电池电压失败: %s\n", dev->desc.c_str());
		comm = false;
		ret = ErrorInfo::ERR_FAILED;

	}


	//读取单体电池温度
	ret = client.ReadOutputRegisters(values, addr, start + (singleBattCount * 3), length);
	if(ret == ErrorInfo::ERR_OK){
		comm = true;
		fdata = (signed)values[0] * 0.1; 		MemDb::SetRealData(regStart + 91, fdata, false);
		fdata = (signed)values[1] * 0.1; 		MemDb::SetRealData(regStart + 92, fdata, false);
		fdata = (signed)values[2] * 0.1; 		MemDb::SetRealData(regStart + 93, fdata, false);
		fdata = (signed)values[3] * 0.1; 		MemDb::SetRealData(regStart + 94, fdata, false);
		fdata = (signed)values[4] * 0.1; 		MemDb::SetRealData(regStart + 95, fdata, false);
		fdata = (signed)values[5] * 0.1; 		MemDb::SetRealData(regStart + 96, fdata, false);
		fdata = (signed)values[6] * 0.1; 		MemDb::SetRealData(regStart + 97, fdata, false);
		fdata = (signed)values[7] * 0.1; 		MemDb::SetRealData(regStart + 98, fdata, false);
		fdata = (signed)values[8] * 0.1; 		MemDb::SetRealData(regStart + 99, fdata, false);
		fdata = (signed)values[9] * 0.1; 		MemDb::SetRealData(regStart + 100, fdata, false);
		fdata = (signed)values[10] * 0.1; 		MemDb::SetRealData(regStart + 101, fdata, false);
		fdata = (signed)values[11] * 0.1; 		MemDb::SetRealData(regStart + 102, fdata, false);
		fdata = (signed)values[12] * 0.1; 		MemDb::SetRealData(regStart + 103, fdata, false);
		fdata = (signed)values[13] * 0.1; 		MemDb::SetRealData(regStart + 104, fdata, false);
		fdata = (signed)values[14] * 0.1; 		MemDb::SetRealData(regStart + 105, fdata, false);
		fdata = (signed)values[15] * 0.1; 		MemDb::SetRealData(regStart + 106, fdata, false);
		fdata = (signed)values[16] * 0.1; 		MemDb::SetRealData(regStart + 107, fdata, false);
		fdata = (signed)values[17] * 0.1; 		MemDb::SetRealData(regStart + 108, fdata, false);
		fdata = (signed)values[18] * 0.1; 		MemDb::SetRealData(regStart + 109, fdata, false);
		fdata = (signed)values[19] * 0.1; 		MemDb::SetRealData(regStart + 110, fdata, false);
		fdata = (signed)values[20] * 0.1; 		MemDb::SetRealData(regStart + 111, fdata, false);
		fdata = (signed)values[21] * 0.1; 		MemDb::SetRealData(regStart + 112, fdata, false);
		fdata = (signed)values[22] * 0.1; 		MemDb::SetRealData(regStart + 113, fdata, false);
		fdata = (signed)values[23] * 0.1; 		MemDb::SetRealData(regStart + 114, fdata, false);
		fdata = (signed)values[24] * 0.1; 		MemDb::SetRealData(regStart + 115, fdata, false);
		fdata = (signed)values[25] * 0.1; 		MemDb::SetRealData(regStart + 116, fdata, false);
		fdata = (signed)values[26] * 0.1; 		MemDb::SetRealData(regStart + 117, fdata, false);
		fdata = (signed)values[27] * 0.1; 		MemDb::SetRealData(regStart + 118, fdata, false);
		fdata = (signed)values[28] * 0.1; 		MemDb::SetRealData(regStart + 119, fdata, false);
		fdata = (signed)values[29] * 0.1; 		MemDb::SetRealData(regStart + 120, fdata, false);

	}
	else{
		zlog_warn(Util::m_zlog, "读取 单体电池温度 失败: %s\n", dev->desc.c_str());
		comm = false;
		ret = ErrorInfo::ERR_FAILED;

	}



	// 读取单体电池内阻
	values.clear();
	length = 60;
	ret = client.ReadOutputRegisters(values, addr, start + (singleBattCount * 4), length);
	if(ret == ErrorInfo::ERR_OK){
		comm = true;
		fdata = ((values[1] << 16) + values[0]) * 0.001; 			MemDb::SetRealData(regStart + 121, fdata, false);
		fdata = ((values[3] << 16) + values[2]) * 0.001; 			MemDb::SetRealData(regStart + 122, fdata, false);
		fdata = ((values[5] << 16) + values[4]) * 0.001; 			MemDb::SetRealData(regStart + 123, fdata, false);
		fdata = ((values[7] << 16) + values[6]) * 0.001; 			MemDb::SetRealData(regStart + 124, fdata, false);
		fdata = ((values[9] << 16) + values[8]) * 0.001; 			MemDb::SetRealData(regStart + 125, fdata, false);
		fdata = ((values[11] << 16) + values[10]) * 0.001; 			MemDb::SetRealData(regStart + 126, fdata, false);
		fdata = ((values[13] << 16) + values[12]) * 0.001; 			MemDb::SetRealData(regStart + 127, fdata, false);
		fdata = ((values[15] << 16) + values[14]) * 0.001; 			MemDb::SetRealData(regStart + 128, fdata, false);
		fdata = ((values[17] << 16) + values[16]) * 0.001; 			MemDb::SetRealData(regStart + 129, fdata, false);
		fdata = ((values[19] << 16) + values[18]) * 0.001; 			MemDb::SetRealData(regStart + 130, fdata, false);
		fdata = ((values[21] << 16) + values[20]) * 0.001; 			MemDb::SetRealData(regStart + 131, fdata, false);
		fdata = ((values[23] << 16) + values[22]) * 0.001; 			MemDb::SetRealData(regStart + 132, fdata, false);
		fdata = ((values[25] << 16) + values[24]) * 0.001; 			MemDb::SetRealData(regStart + 133, fdata, false);
		fdata = ((values[27] << 16) + values[26]) * 0.001; 			MemDb::SetRealData(regStart + 134, fdata, false);
		fdata = ((values[29] << 16) + values[28]) * 0.001; 			MemDb::SetRealData(regStart + 135, fdata, false);
		fdata = ((values[31] << 16) + values[30]) * 0.001; 			MemDb::SetRealData(regStart + 136, fdata, false);
		fdata = ((values[33] << 16) + values[32]) * 0.001; 			MemDb::SetRealData(regStart + 137, fdata, false);
		fdata = ((values[35] << 16) + values[34]) * 0.001; 			MemDb::SetRealData(regStart + 138, fdata, false);
		fdata = ((values[37] << 16) + values[36]) * 0.001; 			MemDb::SetRealData(regStart + 139, fdata, false);
		fdata = ((values[39] << 16) + values[38]) * 0.001; 			MemDb::SetRealData(regStart + 140, fdata, false);
		fdata = ((values[41] << 16) + values[40]) * 0.001; 			MemDb::SetRealData(regStart + 141, fdata, false);
		fdata = ((values[43] << 16) + values[42]) * 0.001; 			MemDb::SetRealData(regStart + 142, fdata, false);
		fdata = ((values[45] << 16) + values[44]) * 0.001; 			MemDb::SetRealData(regStart + 143, fdata, false);
		fdata = ((values[47] << 16) + values[46]) * 0.001; 			MemDb::SetRealData(regStart + 144, fdata, false);
		fdata = ((values[49] << 16) + values[48]) * 0.001; 			MemDb::SetRealData(regStart + 145, fdata, false);
		fdata = ((values[51] << 16) + values[50]) * 0.001; 			MemDb::SetRealData(regStart + 146, fdata, false);
		fdata = ((values[53] << 16) + values[52]) * 0.001; 			MemDb::SetRealData(regStart + 147, fdata, false);
		fdata = ((values[55] << 16) + values[54]) * 0.001; 			MemDb::SetRealData(regStart + 148, fdata, false);
		fdata = ((values[57] << 16) + values[56]) * 0.001; 			MemDb::SetRealData(regStart + 149, fdata, false);
		fdata = ((values[59] << 16) + values[58]) * 0.001; 			MemDb::SetRealData(regStart + 150, fdata, false);
	}
	else{
		zlog_warn(Util::m_zlog, "读取 电池内阻 失败: %s\n", dev->desc.c_str());
		comm = false;
		ret = ErrorInfo::ERR_FAILED;

	}

	// SOC
	values.clear();
	length = 31;
	ret = client.ReadOutputRegisters(values, addr, start + (singleBattCount * 5), length);
	if(ret == ErrorInfo::ERR_OK){
		comm = true;
		idata = values[0]; 				MemDb::SetRealData(regStart + 151, idata, false);
		idata = values[1]; 				MemDb::SetRealData(regStart + 152, idata, false);
		idata = values[2]; 				MemDb::SetRealData(regStart + 153, idata, false);
		idata = values[3]; 				MemDb::SetRealData(regStart + 154, idata, false);
		idata = values[4]; 				MemDb::SetRealData(regStart + 155, idata, false);
		idata = values[5]; 				MemDb::SetRealData(regStart + 156, idata, false);
		idata = values[6]; 				MemDb::SetRealData(regStart + 157, idata, false);
		idata = values[7]; 				MemDb::SetRealData(regStart + 158, idata, false);
		idata = values[8]; 				MemDb::SetRealData(regStart + 159, idata, false);
		idata = values[9]; 				MemDb::SetRealData(regStart + 160, idata, false);
		idata = values[10]; 				MemDb::SetRealData(regStart + 161, idata, false);
		idata = values[11]; 				MemDb::SetRealData(regStart + 162, idata, false);
		idata = values[12]; 				MemDb::SetRealData(regStart + 163, idata, false);
		idata = values[13]; 				MemDb::SetRealData(regStart + 164, idata, false);
		idata = values[14]; 				MemDb::SetRealData(regStart + 165, idata, false);
		idata = values[15]; 				MemDb::SetRealData(regStart + 166, idata, false);
		idata = values[16]; 				MemDb::SetRealData(regStart + 167, idata, false);
		idata = values[17]; 				MemDb::SetRealData(regStart + 168, idata, false);
		idata = values[18]; 				MemDb::SetRealData(regStart + 169, idata, false);
		idata = values[19]; 				MemDb::SetRealData(regStart + 170, idata, false);
		idata = values[20]; 				MemDb::SetRealData(regStart + 171, idata, false);
		idata = values[21]; 				MemDb::SetRealData(regStart + 172, idata, false);
		idata = values[22]; 				MemDb::SetRealData(regStart + 173, idata, false);
		idata = values[23]; 				MemDb::SetRealData(regStart + 174, idata, false);
		idata = values[24]; 				MemDb::SetRealData(regStart + 175, idata, false);
		idata = values[25]; 				MemDb::SetRealData(regStart + 176, idata, false);
		idata = values[26]; 				MemDb::SetRealData(regStart + 177, idata, false);
		idata = values[27]; 				MemDb::SetRealData(regStart + 178, idata, false);
		idata = values[28]; 				MemDb::SetRealData(regStart + 179, idata, false);
		idata = values[29]; 				MemDb::SetRealData(regStart + 180, idata, false);

	}
	else{
		zlog_warn(Util::m_zlog, "读取 SOC 信息失败: %s\n", dev->desc.c_str());
		comm = false;
		ret = ErrorInfo::ERR_FAILED;

	}


	// SOHO
	
	values.clear();
	length = 30;	
	ret = client.ReadOutputRegisters(values, addr, start + (singleBattCount * 6), length);
	if(ret == ErrorInfo::ERR_OK){
		comm = true;
		idata = values[0]; 				MemDb::SetRealData(regStart + 181, idata, false);
		idata = values[1]; 				MemDb::SetRealData(regStart + 182, idata, false);
		idata = values[2]; 				MemDb::SetRealData(regStart + 183, idata, false);
		idata = values[3]; 				MemDb::SetRealData(regStart + 184, idata, false);
		idata = values[4]; 				MemDb::SetRealData(regStart + 185, idata, false);
		idata = values[5]; 				MemDb::SetRealData(regStart + 186, idata, false);
		idata = values[6]; 				MemDb::SetRealData(regStart + 187, idata, false);
		idata = values[7]; 				MemDb::SetRealData(regStart + 188, idata, false);
		idata = values[8]; 				MemDb::SetRealData(regStart + 189, idata, false);
		idata = values[9]; 				MemDb::SetRealData(regStart + 190, idata, false);
		idata = values[10]; 				MemDb::SetRealData(regStart + 191, idata, false);
		idata = values[11]; 				MemDb::SetRealData(regStart + 192, idata, false);
		idata = values[12]; 				MemDb::SetRealData(regStart + 193, idata, false);
		idata = values[13]; 				MemDb::SetRealData(regStart + 194, idata, false);
		idata = values[14]; 				MemDb::SetRealData(regStart + 195, idata, false);
		idata = values[15]; 				MemDb::SetRealData(regStart + 196, idata, false);
		idata = values[16]; 				MemDb::SetRealData(regStart + 197, idata, false);
		idata = values[17]; 				MemDb::SetRealData(regStart + 198, idata, false);
		idata = values[18]; 				MemDb::SetRealData(regStart + 199, idata, false);
		idata = values[19]; 				MemDb::SetRealData(regStart + 200, idata, false);
		idata = values[20]; 				MemDb::SetRealData(regStart + 201, idata, false);
		idata = values[21]; 				MemDb::SetRealData(regStart + 202, idata, false);
		idata = values[22]; 				MemDb::SetRealData(regStart + 203, idata, false);
		idata = values[23]; 				MemDb::SetRealData(regStart + 204, idata, false);
		idata = values[24]; 				MemDb::SetRealData(regStart + 205, idata, false);
		idata = values[25]; 				MemDb::SetRealData(regStart + 206, idata, false);
		idata = values[26]; 				MemDb::SetRealData(regStart + 207, idata, false);
		idata = values[27]; 				MemDb::SetRealData(regStart + 208, idata, false);
		idata = values[28]; 				MemDb::SetRealData(regStart + 209, idata, false);
		idata = values[29]; 				MemDb::SetRealData(regStart + 210, idata, false);

	}
	else{
		zlog_warn(Util::m_zlog, "读取 SOH 失败: %s\n", dev->desc.c_str());
		comm = false;
		ret = ErrorInfo::ERR_FAILED;

	}

	// TODO 单体电池一级告警
	values.clear();
	length= 30;
	
	ret = client.ReadOutputRegisters(values, addr, start + (singleBattCount * 7), length);
	if(ret == ErrorInfo::ERR_OK){
		comm = true;
#if 0
		status = (values[0] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 211, status, false);
		status = (values[0] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 212, status, false);
		status = (values[0] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 213, status, false);
		status = (values[0] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 214, status, false);
		status = (values[0] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 215, status, false);
		status = (values[0] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 216, status, false);
		status = (values[0] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 217, status, false);
		status = (values[0] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 218, status, false);

		status = (values[1] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 219, status, false);
		status = (values[1] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 220, status, false);
		status = (values[1] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 221, status, false);
		status = (values[1] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 222, status, false);
		status = (values[1] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 223, status, false);
		status = (values[1] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 224, status, false);
		status = (values[1] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 225, status, false);
		status = (values[1] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 226, status, false);

		status = (values[2] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 227, status, false);
		status = (values[2] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 228, status, false);
		status = (values[2] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 229, status, false);
		status = (values[2] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 230, status, false);
		status = (values[2] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 231, status, false);
		status = (values[2] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 232, status, false);
		status = (values[2] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 233, status, false);
		status = (values[2] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 234, status, false);

		status = (values[3] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 235, status, false);
		status = (values[3] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 236, status, false);
		status = (values[3] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 237, status, false);
		status = (values[3] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 238, status, false);
		status = (values[3] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 239, status, false);
		status = (values[3] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 240, status, false);
		status = (values[3] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 241, status, false);
		status = (values[3] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 242, status, false);

		status = (values[4] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 243, status, false);
		status = (values[4] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 244, status, false);
		status = (values[4] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 245, status, false);
		status = (values[4] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 246, status, false);
		status = (values[4] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 247, status, false);
		status = (values[4] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 248, status, false);
		status = (values[4] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 249, status, false);
		status = (values[4] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 250, status, false);

		status = (values[5] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 251, status, false);
		status = (values[5] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 252, status, false);
		status = (values[5] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 253, status, false);
		status = (values[5] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 254, status, false);
		status = (values[5] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 255, status, false);
		status = (values[5] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 256, status, false);
		status = (values[5] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 257, status, false);
		status = (values[5] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 258, status, false);

		status = (values[6] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 259, status, false);
		status = (values[6] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 260, status, false);
		status = (values[6] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 261, status, false);
		status = (values[6] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 262, status, false);
		status = (values[6] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 263, status, false);
		status = (values[6] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 264, status, false);
		status = (values[6] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 265, status, false);
		status = (values[6] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 266, status, false);

		status = (values[7] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 267, status, false);
		status = (values[7] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 268, status, false);
		status = (values[7] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 269, status, false);
		status = (values[7] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 270, status, false);
		status = (values[7] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 271, status, false);
		status = (values[7] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 272, status, false);
		status = (values[7] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 273, status, false);
		status = (values[7] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 274, status, false);

		status = (values[8] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 275, status, false);
		status = (values[8] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 276, status, false);
		status = (values[8] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 277, status, false);
		status = (values[8] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 278, status, false);
		status = (values[8] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 279, status, false);
		status = (values[8] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 280, status, false);
		status = (values[8] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 281, status, false);
		status = (values[8] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 282, status, false);

		status = (values[9] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 283, status, false);
		status = (values[9] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 284, status, false);
		status = (values[9] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 285, status, false);
		status = (values[9] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 286, status, false);
		status = (values[9] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 287, status, false);
		status = (values[9] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 288, status, false);
		status = (values[9] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 289, status, false);
		status = (values[9] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 290, status, false);

		status = (values[10] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 291, status, false);
		status = (values[10] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 292, status, false);
		status = (values[10] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 293, status, false);
		status = (values[10] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 294, status, false);
		status = (values[10] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 295, status, false);
		status = (values[10] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 296, status, false);
		status = (values[10] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 297, status, false);
		status = (values[10] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 298, status, false);

		status = (values[11] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 299, status, false);
		status = (values[11] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 300, status, false);
		status = (values[11] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 301, status, false);
		status = (values[11] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 302, status, false);
		status = (values[11] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 303, status, false);
		status = (values[11] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 304, status, false);
		status = (values[11] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 305, status, false);
		status = (values[11] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 306, status, false);

		status = (values[12] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 307, status, false);
		status = (values[12] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 308, status, false);
		status = (values[12] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 309, status, false);
		status = (values[12] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 310, status, false);
		status = (values[12] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 311, status, false);
		status = (values[12] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 312, status, false);
		status = (values[12] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 313, status, false);
		status = (values[12] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 314, status, false);

		status = (values[13] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 315, status, false);
		status = (values[13] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 316, status, false);
		status = (values[13] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 317, status, false);
		status = (values[13] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 318, status, false);
		status = (values[13] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 319, status, false);
		status = (values[13] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 320, status, false);
		status = (values[13] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 321, status, false);
		status = (values[13] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 322, status, false);

		status = (values[14] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 323, status, false);
		status = (values[14] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 324, status, false);
		status = (values[14] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 325, status, false);
		status = (values[14] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 326, status, false);
		status = (values[14] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 327, status, false);
		status = (values[14] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 328, status, false);
		status = (values[14] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 329, status, false);
		status = (values[14] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 330, status, false);

		status = (values[15] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 331, status, false);
		status = (values[15] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 332, status, false);
		status = (values[15] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 333, status, false);
		status = (values[15] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 334, status, false);
		status = (values[15] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 335, status, false);
		status = (values[15] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 336, status, false);
		status = (values[15] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 337, status, false);
		status = (values[15] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 338, status, false);

		status = (values[16] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 339, status, false);
		status = (values[16] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 340, status, false);
		status = (values[16] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 341, status, false);
		status = (values[16] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 342, status, false);
		status = (values[16] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 343, status, false);
		status = (values[16] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 344, status, false);
		status = (values[16] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 345, status, false);
		status = (values[16] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 346, status, false);

		status = (values[17] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 347, status, false);
		status = (values[17] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 348, status, false);
		status = (values[17] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 349, status, false);
		status = (values[17] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 350, status, false);
		status = (values[17] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 351, status, false);
		status = (values[17] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 352, status, false);
		status = (values[17] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 353, status, false);
		status = (values[17] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 354, status, false);

		status = (values[18] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 355, status, false);
		status = (values[18] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 356, status, false);
		status = (values[18] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 357, status, false);
		status = (values[18] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 358, status, false);
		status = (values[18] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 359, status, false);
		status = (values[18] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 360, status, false);
		status = (values[18] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 361, status, false);
		status = (values[18] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 362, status, false);

		status = (values[19] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 363, status, false);
		status = (values[19] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 364, status, false);
		status = (values[19] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 365, status, false);
		status = (values[19] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 366, status, false);
		status = (values[19] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 367, status, false);
		status = (values[19] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 368, status, false);
		status = (values[19] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 369, status, false);
		status = (values[19] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 370, status, false);

		status = (values[20] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 371, status, false);
		status = (values[20] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 372, status, false);
		status = (values[20] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 373, status, false);
		status = (values[20] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 374, status, false);
		status = (values[20] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 375, status, false);
		status = (values[20] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 376, status, false);
		status = (values[20] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 377, status, false);
		status = (values[20] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 378, status, false);

		status = (values[21] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 379, status, false);
		status = (values[21] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 380, status, false);
		status = (values[21] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 381, status, false);
		status = (values[21] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 382, status, false);
		status = (values[21] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 383, status, false);
		status = (values[21] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 384, status, false);
		status = (values[21] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 385, status, false);
		status = (values[21] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 386, status, false);

		status = (values[22] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 387, status, false);
		status = (values[22] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 388, status, false);
		status = (values[22] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 389, status, false);
		status = (values[22] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 390, status, false);
		status = (values[22] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 391, status, false);
		status = (values[22] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 392, status, false);
		status = (values[22] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 393, status, false);
		status = (values[22] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 394, status, false);

		status = (values[23] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 395, status, false);
		status = (values[23] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 396, status, false);
		status = (values[23] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 397, status, false);
		status = (values[23] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 398, status, false);
		status = (values[23] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 399, status, false);
		status = (values[23] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 400, status, false);
		status = (values[23] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 401, status, false);
		status = (values[23] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 402, status, false);

		status = (values[24] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 403, status, false);
		status = (values[24] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 404, status, false);
		status = (values[24] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 405, status, false);
		status = (values[24] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 406, status, false);
		status = (values[24] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 407, status, false);
		status = (values[24] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 408, status, false);
		status = (values[24] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 409, status, false);
		status = (values[24] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 410, status, false);

		status = (values[25] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 411, status, false);
		status = (values[25] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 412, status, false);
		status = (values[25] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 413, status, false);
		status = (values[25] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 414, status, false);
		status = (values[25] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 415, status, false);
		status = (values[25] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 416, status, false);
		status = (values[25] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 417, status, false);
		status = (values[25] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 418, status, false);

		status = (values[26] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 419, status, false);
		status = (values[26] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 420, status, false);
		status = (values[26] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 421, status, false);
		status = (values[26] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 422, status, false);
		status = (values[26] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 423, status, false);
		status = (values[26] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 424, status, false);
		status = (values[26] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 425, status, false);
		status = (values[26] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 426, status, false);

		status = (values[27] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 427, status, false);
		status = (values[27] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 428, status, false);
		status = (values[27] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 429, status, false);
		status = (values[27] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 430, status, false);
		status = (values[27] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 431, status, false);
		status = (values[27] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 432, status, false);
		status = (values[27] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 433, status, false);
		status = (values[27] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 434, status, false);
		
		status = (values[28] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 435, status, false);
		status = (values[28] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 436, status, false);
		status = (values[28] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 437, status, false);
		status = (values[28] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 438, status, false);
		status = (values[28] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 439, status, false);
		status = (values[28] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 440, status, false);
		status = (values[28] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 441, status, false);
		status = (values[28] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 442, status, false);

		status = (values[29] >> 0 ) & 0b1; 		MemDb::SetRealData(regStart + 443, status, false);
		status = (values[29] >> 1 ) & 0b1; 		MemDb::SetRealData(regStart + 444, status, false);
		status = (values[29] >> 2 ) & 0b1; 		MemDb::SetRealData(regStart + 445, status, false);
		status = (values[29] >> 3 ) & 0b1; 		MemDb::SetRealData(regStart + 446, status, false);
		status = (values[29] >> 4 ) & 0b1; 		MemDb::SetRealData(regStart + 447, status, false);
		status = (values[29] >> 5 ) & 0b1; 		MemDb::SetRealData(regStart + 448, status, false);
		status = (values[29] >> 6 ) & 0b1; 		MemDb::SetRealData(regStart + 449, status, false);
		status = (values[29] >> 7 ) & 0b1; 		MemDb::SetRealData(regStart + 450, status, false);
#endif 

		// 一级告警
		BattWarnInfo(values[1], regStart + 219);
		BattWarnInfo(values[2], regStart + 227);
		BattWarnInfo(values[3], regStart + 235);
		BattWarnInfo(values[4], regStart + 243);
		BattWarnInfo(values[5], regStart + 251);
		BattWarnInfo(values[6], regStart + 259);
		BattWarnInfo(values[7], regStart + 267);
		BattWarnInfo(values[8], regStart + 275);
		BattWarnInfo(values[9], regStart + 283);
		BattWarnInfo(values[10], regStart + 291);
		BattWarnInfo(values[11], regStart + 299);
		BattWarnInfo(values[12], regStart + 307);
		BattWarnInfo(values[13], regStart + 315);
		BattWarnInfo(values[14], regStart + 323);
		BattWarnInfo(values[15], regStart + 331);
		BattWarnInfo(values[16], regStart + 339);
		BattWarnInfo(values[17], regStart + 347);
		BattWarnInfo(values[18], regStart + 355);
		BattWarnInfo(values[19], regStart + 363);
		BattWarnInfo(values[20], regStart + 371);
		BattWarnInfo(values[21], regStart + 379);
		BattWarnInfo(values[22], regStart + 387);
		BattWarnInfo(values[23], regStart + 395);
		BattWarnInfo(values[24], regStart + 403);
		BattWarnInfo(values[25], regStart + 411);
		BattWarnInfo(values[26], regStart + 419);
		BattWarnInfo(values[27], regStart + 427);
		BattWarnInfo(values[28], regStart + 435);
		BattWarnInfo(values[29], regStart + 443);

	}
	else{
		zlog_warn(Util::m_zlog, "读取 一级告警 信息失败: %s\n", dev->desc.c_str());
		comm = false;
		ret = ErrorInfo::ERR_FAILED;

	}


	//  单体电池二级告警
	values.clear();
	length= 30;
	
	ret = client.ReadOutputRegisters(values, addr, start + (singleBattCount * 8), length);
	if(ret == ErrorInfo::ERR_OK){
		comm = true;

		BattWarnInfo(values[0], regStart + 451);
		BattWarnInfo(values[1], regStart + 459);
		BattWarnInfo(values[2], regStart + 467);
		BattWarnInfo(values[3], regStart + 475);
		BattWarnInfo(values[4], regStart + 483);
		BattWarnInfo(values[5], regStart + 491);
		BattWarnInfo(values[6], regStart + 499);
		BattWarnInfo(values[7], regStart + 507);
		BattWarnInfo(values[8], regStart + 515);
		BattWarnInfo(values[9], regStart + 523);
		BattWarnInfo(values[10], regStart + 531);
		BattWarnInfo(values[11], regStart + 539);
		BattWarnInfo(values[12], regStart + 547);
		BattWarnInfo(values[13], regStart + 555);
		BattWarnInfo(values[14], regStart + 563);
		BattWarnInfo(values[15], regStart + 571);
		BattWarnInfo(values[16], regStart + 579);
		BattWarnInfo(values[17], regStart + 587);
		BattWarnInfo(values[18], regStart + 595);
		BattWarnInfo(values[19], regStart + 603);
		BattWarnInfo(values[20], regStart + 611);
		BattWarnInfo(values[21], regStart + 619);
		BattWarnInfo(values[22], regStart + 627);
		BattWarnInfo(values[23], regStart + 635);
		BattWarnInfo(values[24], regStart + 643);
		BattWarnInfo(values[25], regStart + 651);
		BattWarnInfo(values[26], regStart + 659);
		BattWarnInfo(values[27], regStart + 667);
		BattWarnInfo(values[28], regStart + 675);
		BattWarnInfo(values[29], regStart + 683);

	}
	else{
		zlog_warn(Util::m_zlog, "读取 二级告警 信息失败: %s\n", dev->desc.c_str());
		comm = false;
		ret = ErrorInfo::ERR_FAILED;

	}



	// 单体电池其他状态
	values.clear();
	length= 30;
	
	ret = client.ReadOutputRegisters(values, addr, start + (singleBattCount * 9), length);
	if(ret == ErrorInfo::ERR_OK){
		comm = true;
		BattOtherStatus(values[0], regStart + 691);
		BattOtherStatus(values[1], regStart + 695);
		BattOtherStatus(values[2], regStart + 699);
		BattOtherStatus(values[3], regStart + 703);
		BattOtherStatus(values[4], regStart + 707);
		BattOtherStatus(values[5], regStart + 711);
		BattOtherStatus(values[6], regStart + 715);
		BattOtherStatus(values[7], regStart + 719);
		BattOtherStatus(values[8], regStart + 723);
		BattOtherStatus(values[9], regStart + 727);
		BattOtherStatus(values[10], regStart + 731);
		BattOtherStatus(values[11], regStart + 735);
		BattOtherStatus(values[12], regStart + 739);
		BattOtherStatus(values[13], regStart + 743);
		BattOtherStatus(values[14], regStart + 747);
		BattOtherStatus(values[15], regStart + 751);
		BattOtherStatus(values[16], regStart + 755);
		BattOtherStatus(values[17], regStart + 759);
		BattOtherStatus(values[18], regStart + 763);
		BattOtherStatus(values[19], regStart + 767);
		BattOtherStatus(values[20], regStart + 771);
		BattOtherStatus(values[21], regStart + 775);
		BattOtherStatus(values[22], regStart + 779);
		BattOtherStatus(values[23], regStart + 783);
		BattOtherStatus(values[24], regStart + 787);
		BattOtherStatus(values[25], regStart + 791);
		BattOtherStatus(values[26], regStart + 795);
		BattOtherStatus(values[27], regStart + 799);
		BattOtherStatus(values[28], regStart + 803);
		BattOtherStatus(values[29], regStart + 807);

	}
	else{
		zlog_warn(Util::m_zlog, "读取 单体电池其他状态  失败: %s\n", dev->desc.c_str());
		comm = false;
		ret = ErrorInfo::ERR_FAILED;

	}

	MemDb::SetRealData(dev->regStart + 0, comm, false);


	return ret;
}

void AquaNa::BattWarnInfo(unsigned short value, int regStart){
	bool status = 0;

	status = (value >> 0) & 0b1; 		 MemDb::SetRealData(regStart + 0, status, false);
	status = (value >> 1) & 0b1; 		 MemDb::SetRealData(regStart + 1, status, false);
	status = (value >> 2) & 0b1; 		 MemDb::SetRealData(regStart + 2, status, false);
	status = (value >> 3) & 0b1; 		 MemDb::SetRealData(regStart + 3, status, false);
	status = (value >> 4) & 0b1; 		 MemDb::SetRealData(regStart + 4, status, false);
	status = (value >> 5) & 0b1; 		 MemDb::SetRealData(regStart + 5, status, false);
	status = (value >> 6) & 0b1; 		 MemDb::SetRealData(regStart + 6, status, false);
	status = (value >> 7) & 0b1; 		 MemDb::SetRealData(regStart + 7, status, false);



}

void AquaNa::BattOtherStatus(unsigned short value, int regStart){
	bool status = 0;
	status = (value >> 0) & 0b1; 		MemDb::SetRealData(regStart + 0, status, false);
	status = (value >> 1) & 0b1; 		MemDb::SetRealData(regStart + 1, status, false);
	status = (value >> 2) & 0b1; 		MemDb::SetRealData(regStart + 2, status, false);
	status = (value >> 15) & 0b1; 		MemDb::SetRealData(regStart + 3, status, false);


}






