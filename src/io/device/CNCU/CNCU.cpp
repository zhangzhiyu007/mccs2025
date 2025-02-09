/**
 *    文件      : CNCU.cpp
 *    Synopsis  : 
 *    作者      : Mingyu Wu
 *    version   : 1.0.0
 *    日期      : 2021-03-02
 *    TODO 完成控制设备控制功能
 */

#include "CNCU.h"
#include "../../../util/Util.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

CNCU::CNCU(){
	m_isTcp = false;
	m_com = NULL;
	m_tcpClient = NULL;
}
CNCU::~CNCU(){

}

void CNCU::SetIsTcp(bool isTcp){
	m_isTcp = isTcp;
}

void CNCU::SetCom(Com *com){
	m_com = com;
}

void CNCU::SetTcp(TcpClient *tcpClient){
	m_tcpClient = tcpClient;
}

int CNCU::Read(Device::SlaveDev *dev){
	int ret = ErrorInfo::ERR_OK;

	switch (dev->devManu) {
		case DevManu::Manu_DEXN: // 德意新能
			ret = DEXNRead(dev);
			break;
		default:
			break;
	}
	return ret;

}

int CNCU::DEXNRead(Device::SlaveDev *dev){
	int ret = ErrorInfo::ERR_OK;
	ACDCRead(dev);
	ESCRead(dev);
	PVCRead(dev);
	OverallRead(dev);

	return ret;
}


int CNCU::OverallRead(Device::SlaveDev* dev){
	int ret = ErrorInfo::ERR_OK;
	/*	Mingyu - 2021-03-01
	 *	系统数据区
	 */

	ModbusRtuMaster rtu;
	rtu.SetIsTcp(m_isTcp);
	rtu.SetTcpClient(m_tcpClient);

	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0x3000; 
	int length = 2;
	int data = 0;
	bool comm = 0;

	ret = rtu.ReadOutputRegisters(values, addr, start, length);

	if( ErrorInfo::ERR_OK == ret){
		comm = true;
		data = (unsigned short)values[0]; //系统工作模式
		MemDb::SetRealData(regStart + 270, data, false);

		data = (unsigned short)values[1] * 0.1; //系统母线电压设定
		MemDb::SetRealData(regStart + 271, data, false);

	}
	else {
		zlog_warn(Util::m_zlog, "读取%s, 整体系统数据区失败", dev->name.c_str());
		comm = false;
	}
	// 系统数据区通信状态
	MemDb::SetRealData(regStart+269, comm, false);
}

/*	Mingyu - 2021-03-01
 */
int CNCU::ACDCRead(Device::SlaveDev* dev){
	int ret = ErrorInfo::ERR_OK;
	
	//2020-02-16
	ModbusRtuMaster rtu;
	rtu.SetIsTcp(m_isTcp);
	rtu.SetTcpClient(m_tcpClient);

	UshortArray values;
	values.clear();

	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0x0000;
	int length = 0;
	bool status = 0;
	int data =0;
	string serialNo;
	bool comm = false;
	start = 0x0000;

	// 功能码01
	length = 2;
	ret = rtu.ReadOutputStatus(values, addr, start, length);
	if( ErrorInfo::ERR_OK == ret){
		comm = true;
		data = (unsigned short)values[0]; //启停控制 0停机 1运行
		MemDb::SetRealData(regStart+1, data, false);

		data = (unsigned short)values[1]; // 故障复位 0无操作 1 复位
		MemDb::SetRealData(regStart+2, data, false);
	}else {
		zlog_warn(Util::m_zlog, "读取%s ACDC 1号 命令失败",dev->name.c_str());
		comm = false;
	}

	values.clear();

	// 功能码02
	length = 5;
	ret = rtu.ReadInputStatus(values, addr, start, length); // 读取停机状态
	//ret = rtu.ReadInputStatusRcl(values, addr, start, length);
	if ( ErrorInfo::ERR_OK == ret) {
		comm = true;
		status = (short)(values[0]); // 运行状态
		MemDb::SetRealData(regStart + 3, status, false); 

		status = (short)(values[1]); // 故障状态
		MemDb::SetRealData(regStart + 4, status, false); 

		status = (short)values[2]; // 告警状态
		MemDb::SetRealData(regStart + 5, status, false);

	
		status = (short)values[3]; // 用电状态
		MemDb::SetRealData(regStart + 6, status, false);

		status = (short)values[4]; // 发电状态
		MemDb::SetRealData(regStart + 7, status, false);



	}
	else {
		zlog_warn(Util::m_zlog, " 读取%s ACDC 2号命令失败", dev->name.c_str());
		comm = false;
	}

	values.clear();
	serialNo.clear();
	length = 19;
	// 功能码04
	ret = rtu.ReadInputRegisters(values, addr, start, length);

	if ( ErrorInfo::ERR_OK == ret) {
		comm = true;
		/* Mingyu
		 * 读取CNCU序列号
		 */

		serialNo.push_back( (char)(values[0] >>  8) ); 
		serialNo.push_back( (char)(values[0] & 0x00FF) );
		serialNo.push_back( (char)(values[1] >>  8) ); 
		serialNo.push_back( (char)(values[1] & 0x00FF) );
		serialNo.push_back( (char)(values[2] >>  8) ); 
		serialNo.push_back( (char)(values[2] & 0x00FF) );
		serialNo.push_back( (char)(values[3] >>  8) ); 
		serialNo.push_back( (char)(values[3] & 0x00FF) );
		serialNo.push_back( (char)(values[4] >>  8) ); 
		serialNo.push_back( (char)(values[4] & 0x00FF) );
		serialNo.push_back( (char)(values[5] >>  8) ); 
		serialNo.push_back( (char)(values[5] & 0x00FF) );
		serialNo.push_back( (char)(values[6] >>  8) ); 
		serialNo.push_back( (char)(values[6] & 0x00FF) );
		serialNo.push_back( (char)(values[7] >>  8) ); 
		serialNo.push_back( (char)(values[7] & 0x00FF) );
		//zlog_warn(Util::m_zlog, "serialNo: %s", serialNo.c_str());

		MemDb::SetRealData(regStart + 8, (unsigned short )values[0],false);
		MemDb::SetRealData(regStart + 9, (unsigned short )values[1],false);
		MemDb::SetRealData(regStart + 10, (unsigned short )values[2],false);
		MemDb::SetRealData(regStart + 11, (unsigned short )values[3],false);
		MemDb::SetRealData(regStart + 12, (unsigned short )values[4],false);
		MemDb::SetRealData(regStart + 13, (unsigned short )values[5],false);
		MemDb::SetRealData(regStart + 14, (unsigned short )values[6],false);
		MemDb::SetRealData(regStart + 15, (unsigned short )values[7],false);

		data = (unsigned short)values[8];
		MemDb::SetRealData(regStart + 16, data, false); // 固件版本
		//zlog_warn(Util::m_zlog, "ESC firmware: %d", data);

		data = (unsigned short)values[9];
		MemDb::SetRealData(regStart + 17, data, false); // 复合节点通信类型
		
		data = (unsigned short)values[10];
		MemDb::SetRealData(regStart + 18, data, false); // 复合节点能源类型
		
		data = (unsigned short)values[11];
		MemDb::SetRealData(regStart + 19, data, false); // 故障代码
		
		data = (unsigned short)values[12] * 0.1;
		MemDb::SetRealData(regStart + 20, data, false); // 交流电压
		
		data = (short)values[13] * 0.1;
		MemDb::SetRealData(regStart + 21, data, false); // 交流电流
		
		data = (short)values[14] * 0.01;
		MemDb::SetRealData(regStart + 22, data, false); // 交流功率

		data = (unsigned short)values[15] * 0.01;
		MemDb::SetRealData(regStart + 23, data, false); // 交流频率

		data = (unsigned short)values[16] * 0.1;
		MemDb::SetRealData(regStart + 24, data, false); // 直流电压
		
		data = (unsigned  short)values[17] * 0.1;
		MemDb::SetRealData(regStart + 25, data, false); // 直流电流
		
		data = (unsigned short)values[18] * 0.01;
		MemDb::SetRealData(regStart + 26, data, false); // 直流功率
	}
	else {
		zlog_warn(Util::m_zlog, " 读取%s ACDC 04 失败", dev->name.c_str());
		comm =false;
	}

	values.clear();
	length = 4;
	// TODO 功能码03
	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if( ErrorInfo::ERR_OK == ret){
		comm = true;

		data = (unsigned short)values[0]; // 工作模式
		MemDb::SetRealData(regStart + 27, data, false);

		data = (unsigned short)values[1] * 0.1; //电压给定
		MemDb::SetRealData(regStart + 28, data, false);

		data = (short)values[2] * 0.1; //电流给定
		MemDb::SetRealData(regStart + 29, data, false);

		data = (short)values[3] * 0.01; //功率给定
		MemDb::SetRealData(regStart + 30, data, false);

	}
	else {
		zlog_warn(Util::m_zlog, "读取%s, ACDC 03 寄存数据失败", dev->name.c_str());
		comm = false;
	}
	MemDb::SetRealData(regStart+0, comm, false); //通讯状态
	return ret;
}

int CNCU::ESCRead(Device::SlaveDev *dev){
	//
	int ret = ErrorInfo::ERR_OK;
	
	//2020-02-09
	ModbusRtuMaster rtu;
	rtu.SetIsTcp(m_isTcp);
	rtu.SetTcpClient(m_tcpClient);

	UshortArray values;
	values.clear();

	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0x1000;
	int length = 0;
	int data =0;
	float f_data = 0.00;
	string serialNo;
	bool comm = false;
	start = 0x1000;


	// 功能码01
	length = 2;
	ret = rtu.ReadOutputStatus(values, addr, start, length);
	if( ErrorInfo::ERR_OK == ret){
		comm = true;
		data = (unsigned short)values[0]; //启停控制 0停机 1运行
		MemDb::SetRealData(regStart + 38, data, false);

		data = (unsigned short)values[1]; // 故障复位 0无操作 1 复位
		MemDb::SetRealData(regStart + 39, data, false);
	}else {
		zlog_warn(Util::m_zlog, "读取%s ESC 01 失败",dev->name.c_str());
		comm = false;
	}

	values.clear();

	
	// 功能码02
	length = 16;

	/*	Mingyu - 2021-03-01
	 *	循环读取ESC每个子模块的02功能
	 */
	SubESCReadInputStatus( regStart+40, start, addr);
	//if(ErrorInfo::ERR_OK  == ret){
		//comm =true;
	//}else{
		//zlog_warn(Util::m_zlog, "读取%s，子系统：%d，02 失败",dev->name.c_str(), ret);		
	//}



	// 功能码 04
	values.clear();
	serialNo.clear();

	length = 31;

	ret = rtu.ReadInputRegisters(values, addr, start, length);

	if ( ErrorInfo::ERR_OK == ret) {
		comm = true;
		/* Mingyu
		 * 读取CNCU序列号
		 */

		serialNo.push_back( (char)(values[0] >>  8) ); 
		serialNo.push_back( (char)(values[0] & 0x00FF) );
		serialNo.push_back( (char)(values[1] >>  8) ); 
		serialNo.push_back( (char)(values[1] & 0x00FF) );
		serialNo.push_back( (char)(values[2] >>  8) ); 
		serialNo.push_back( (char)(values[2] & 0x00FF) );
		serialNo.push_back( (char)(values[3] >>  8) ); 
		serialNo.push_back( (char)(values[3] & 0x00FF) );
		serialNo.push_back( (char)(values[4] >>  8) ); 
		serialNo.push_back( (char)(values[4] & 0x00FF) );
		serialNo.push_back( (char)(values[5] >>  8) ); 
		serialNo.push_back( (char)(values[5] & 0x00FF) );
		serialNo.push_back( (char)(values[6] >>  8) ); 
		serialNo.push_back( (char)(values[6] & 0x00FF) );
		serialNo.push_back( (char)(values[7] >>  8) ); 
		serialNo.push_back( (char)(values[7] & 0x00FF) );
		//zlog_warn(Util::m_zlog, "serialNo: %s", serialNo.c_str());

		MemDb::SetRealData(regStart + 120, (unsigned short )values[0],false);
		MemDb::SetRealData(regStart + 121, (unsigned short )values[1],false);
		MemDb::SetRealData(regStart + 122, (unsigned short )values[2],false);
		MemDb::SetRealData(regStart + 123, (unsigned short )values[3],false);
		MemDb::SetRealData(regStart + 124, (unsigned short )values[4],false);
		MemDb::SetRealData(regStart + 125, (unsigned short )values[5],false);
		MemDb::SetRealData(regStart + 126, (unsigned short )values[6],false);
		MemDb::SetRealData(regStart + 127, (unsigned short )values[7],false);

		data = (unsigned short)values[8];
		MemDb::SetRealData(regStart + 128, data, false); // 固件版本
		//zlog_warn(Util::m_zlog, "ESC firmware: %d", data);
		data = (unsigned short)values[9];
		MemDb::SetRealData(regStart + 129, data, false); // 复合节点通信类型
		
		data = (unsigned short)values[10];
		MemDb::SetRealData(regStart + 130, data, false); // 复合节点能源类型
		
		data = (unsigned short)values[11];
		MemDb::SetRealData(regStart + 131, data, false); // 电站编号
		
		data = (unsigned short)values[12];
		MemDb::SetRealData(regStart + 132, data, false); // 系统类型
		
		data = (unsigned short)values[13];
		MemDb::SetRealData(regStart + 133, data, false); // 装机功率
		
		data = (unsigned short)values[14] * 0.01;
		MemDb::SetRealData(regStart + 134, data, false); // 装机容量
		
		data = (unsigned short)values[15];
		MemDb::SetRealData(regStart + 135, data, false); // 系统规模
		
		data = (unsigned short)values[16];
		MemDb::SetRealData(regStart + 136, data, false); // 额定充电功率
		
		data = (unsigned short)values[17];
		MemDb::SetRealData(regStart + 137, data, false); // 额定放电功率
		
		data = (unsigned short)values[18];
		MemDb::SetRealData(regStart + 138, data, false); // SOC运行上限

		data = (unsigned short)values[19];
		MemDb::SetRealData(regStart + 139, data, false); // SOC运行下限

		data = (unsigned short)values[20];
		MemDb::SetRealData(regStart + 140, data, false); // 循环次数

		data = (unsigned short)values[21];
		MemDb::SetRealData(regStart + 141, data, false); // 电压等级

		data = (unsigned short)values[22];
		MemDb::SetRealData(regStart + 142, data, false); // 储能子系统数量

		data = (unsigned short)values[23];
		MemDb::SetRealData(regStart + 143, data, false); // 储能子系统电池组数量

		data = (unsigned short)values[24];
		MemDb::SetRealData(regStart + 144, data, false); // 电池组单体电压数量

		data = (unsigned short)values[25];
		MemDb::SetRealData(regStart + 145, data, false); // 电池组单体温度

		/*	Mingyu - 2021-03-01
		 *	储能站点实时数据
		 */
		f_data = (unsigned short)values[26] * 0.01;
		MemDb::SetRealData(regStart + 146, f_data, false); // 输出功率

		data = (unsigned short)values[27];
		MemDb::SetRealData(regStart + 147, data, false); // SOC

		data = (unsigned short)values[28];
		MemDb::SetRealData(regStart + 148, data, false); // 故障代码

		data = (unsigned short)values[29] * 0.01; // 最大允放功率
		MemDb::SetRealData(regStart + 274, data, false);

		data = (unsigned short)values[30] * 0.01; // 最大允充功率
		MemDb::SetRealData(regStart + 275, data, false);
	}
	else {
		zlog_warn(Util::m_zlog, " 读取%s ESC 04 失败", dev->name.c_str());
		comm =false;
	}
	/*	Mingyu - 2021-03-02
	 *	读取储能子系统状态区
	 *	功能码04
	 */

	ret = SubESCReadInputRegisters(regStart + 149, start + 0x20, addr);
	SubESCReadInputRegisters(regStart + 168 , start + 0x40, addr);
	SubESCReadInputRegisters(regStart + 187 , start + 0x60, addr);
	SubESCReadInputRegisters(regStart + 206 , start + 0x80, addr);

	//if( ErrorInfo::ERR_OK == ret ){
		//comm =true;
	//}
	//else{
		//zlog_warn(Util::m_zlog, "读取%s，序号：%d ESC 04 失败", dev->name.c_str(), ret);
	//}






	values.clear();
	length = 4;
	// TODO 功能码03
	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if( ErrorInfo::ERR_OK == ret){
		comm = true;

		data = (unsigned short)values[0]; // 工作模式
		MemDb::SetRealData(regStart + 225, data, false);

		data = (unsigned short)values[1] * 0.1; //电压给定
		MemDb::SetRealData(regStart + 226, data, false);

		data = (short)values[2] * 0.1; //电流给定
		MemDb::SetRealData(regStart + 227, data, false);

		data = (short)values[3] * 0.01; //功率给定
		MemDb::SetRealData(regStart + 228, data, false);


	}
	else {
		zlog_warn(Util::m_zlog, "读取%s, ESC 03 数据失败", dev->name.c_str());
		comm = false;
	}

	MemDb::SetRealData(regStart + 37, comm, false); //通讯状态
	return ret;
}
int CNCU::PVCRead(Device::SlaveDev *dev){
	int ret = ErrorInfo::ERR_OK;
	

	//2020-02-16
	ModbusRtuMaster rtu;
	rtu.SetIsTcp(m_isTcp);
	rtu.SetTcpClient(m_tcpClient);

	UshortArray values;
	values.clear();

	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0x2000;
	int length = 0;
	bool status = 0;
	int data =0;
	string serialNo;
	bool comm = false;

	/*	Mingyu - 2021-03-02
	 *	功能码 01
	 */
	length = 2;
	ret = rtu.ReadOutputStatus(values, addr, start, length);
	if(ErrorInfo::ERR_OK == ret){
		comm = true;

		// 启停控制
		data = (unsigned short)		values[0];			MemDb::SetRealData(regStart + 236, data, false);
		// 故障复位
		data = (unsigned short)		values[1];			MemDb::SetRealData(regStart + 237, data, false);
	}
	else {
		zlog_warn(Util::m_zlog, "读取%s PVC 01 失败", dev->name.c_str());
		comm = false;
	}

	/*	Mingyu - 2021-03-02
	 *	功能码 02
	 */

	values.clear();
	length = 3;
	ret = rtu.ReadInputStatus(values, addr, start, length); // 读取停机状态
	if ( ErrorInfo::ERR_OK == ret) {
		comm = true;
		// 运行状态
		status = (short)(values[0]);					MemDb::SetRealData(regStart + 238, status, false); 
		
		// 故障状态
		status = (short)(values[1]);					MemDb::SetRealData(regStart + 239, status, false); 
	}
	else {
		zlog_warn(Util::m_zlog, " 读取PVC %s 02 失败", dev->name.c_str());
		comm = false;
	}


	/*	Mingyu - 2021-03-02
	 *	功能码 04
	 */
	values.clear();
	serialNo.clear();
	length = 18;
	ret = rtu.ReadInputRegisters(values, addr, start, length);
	if ( ErrorInfo::ERR_OK == ret) {
		comm = true;
		/* Mingyu
		 * 读取CNCU序列号
		 */

		serialNo.push_back( (char)(values[0] >>  8) ); 
		serialNo.push_back( (char)(values[0] & 0x00FF) );
		serialNo.push_back( (char)(values[1] >>  8) ); 
		serialNo.push_back( (char)(values[1] & 0x00FF) );
		serialNo.push_back( (char)(values[2] >>  8) ); 
		serialNo.push_back( (char)(values[2] & 0x00FF) );
		serialNo.push_back( (char)(values[3] >>  8) ); 
		serialNo.push_back( (char)(values[3] & 0x00FF) );
		serialNo.push_back( (char)(values[4] >>  8) ); 
		serialNo.push_back( (char)(values[4] & 0x00FF) );
		serialNo.push_back( (char)(values[5] >>  8) ); 
		serialNo.push_back( (char)(values[5] & 0x00FF) );
		serialNo.push_back( (char)(values[6] >>  8) ); 
		serialNo.push_back( (char)(values[6] & 0x00FF) );
		serialNo.push_back( (char)(values[7] >>  8) ); 
		serialNo.push_back( (char)(values[7] & 0x00FF) );
		//zlog_warn(Util::m_zlog, "serialNo: %s", serialNo.c_str());

		MemDb::SetRealData(regStart + 241, (unsigned short )values[0],false);
		MemDb::SetRealData(regStart + 242, (unsigned short )values[1],false);
		MemDb::SetRealData(regStart + 243, (unsigned short )values[2],false);
		MemDb::SetRealData(regStart + 244, (unsigned short )values[3],false);
		MemDb::SetRealData(regStart + 245, (unsigned short )values[4],false);
		MemDb::SetRealData(regStart + 246, (unsigned short )values[5],false);
		MemDb::SetRealData(regStart + 247, (unsigned short )values[6],false);
		MemDb::SetRealData(regStart + 248, (unsigned short )values[7],false);


		// 固件版本
		data = (unsigned short)values[8];				MemDb::SetRealData(regStart + 249, data, false);
		// 复合节点通信类型
		data = (unsigned short)values[9];				MemDb::SetRealData(regStart + 250, data, false);
		// 复合节点能源类型
		data = (unsigned short)values[10];				MemDb::SetRealData(regStart + 251, data, false);
		// 故障代码
		data = (unsigned short)values[11];				MemDb::SetRealData(regStart + 252, data, false);
		// 输入电压
		data = (unsigned short)values[12] * 0.1;		MemDb::SetRealData(regStart + 253, data, false);
		// 输入电流
		data = (		 short)values[13] * 0.1;		MemDb::SetRealData(regStart + 254, data, false);
		// 输入功率
		data = (		 short)values[14] * 0.01;		MemDb::SetRealData(regStart + 255, data, false);
		// 输出电压
		data = (unsigned short)values[15] * 0.1;				MemDb::SetRealData(regStart + 256, data, false);
		// 输出电流
		data = (		 short)values[16] * 0.1;				MemDb::SetRealData(regStart + 257, data, false);
		// 输出功率
		data = (		 short)values[17] * 0.01;				MemDb::SetRealData(regStart + 258, data, false);
		//// 工作模式
		//data = (unsigned short)values[18];				MemDb::SetRealData(regStart + 259, data, false);
		//// 电压给定
		//data = (unsigned short)values[19];				MemDb::SetRealData(regStart + 260, data, false);
		//// 电流给定
		//data = (		 short)values[20];				MemDb::SetRealData(regStart + 261, data, false);
		//// 功率给定
		//data = (		 short)values[21];				MemDb::SetRealData(regStart + 262, data, false);
		


	}
	else {
		zlog_warn(Util::m_zlog, " 读取%s实时数据失败", dev->name.c_str());
		comm =false;
	}



	/*	Mingyu - 2021-03-02
	 *	功能码03
	 */

	values.clear();
	length = 4;
	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if( ErrorInfo::ERR_OK == ret){
		comm = true;
		// 工作模式
		data = (unsigned short)values[0];					MemDb::SetRealData(regStart + 259, data, false);
		// 电压给定
		data = (unsigned short)values[1] * 0.1;					MemDb::SetRealData(regStart + 260, data, false);
		// 电流给定
		data = (unsigned short)values[2] * 0.1;					MemDb::SetRealData(regStart + 261, data, false);
		// 功率给定
		data = (unsigned short)values[3] * 0.01;					MemDb::SetRealData(regStart + 262, data, false);
	}
	else {
		zlog_warn(Util::m_zlog, "读取%s,保持寄存数据失败", dev->name.c_str());
		comm = false;
	}


	MemDb::SetRealData(regStart + 235, comm, false); //通讯状态
	// TODO 根据点表通讯协议完成
	return ret; 
}


int CNCU::Preset(Device::SlaveDev *dev){

	ACDCPreset(dev);
	ESCPreset(dev);
	PVCPreset(dev);
	OverallPreset(dev);

	return ErrorInfo::ERR_OK;

}

int CNCU::ACDCPreset(Device::SlaveDev *dev){
	int ret = ErrorInfo::ERR_OK;

	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0x0000;
	int data = 0;

	UshortArray values;
	RealData realData;

	if(!m_tcpClient->IsOpened()){
		return ErrorInfo::ERR_OPENED;
	}
	
	values.clear();

	ModbusRtuMaster rtu;
	rtu.SetIsTcp(m_isTcp);
	rtu.SetTcpClient(m_tcpClient);

	/*	Mingyu - 2021-03-02
	 *	功能码 05
	 */
	// 启停控制
	if( MemDb::GetRealData(realData, regStart + 31, true) )
		ret = rtu.ForceSingleCoil(addr, start + 0 , realData.pv.data.iValue);

	// 故障复位
	if( MemDb::GetRealData(realData, regStart + 32, true) )
		ret = rtu.ForceSingleCoil(addr, start + 1 , realData.pv.data.iValue);

	if(ErrorInfo::ERR_OK != ret){
		zlog_warn(Util::m_zlog, "下发数据到%s ACDC 5号命令失败", dev->name.c_str());
	}
	/*	Mingyu - 2021-03-02
	 *	功能码 06
	 */
	// 工作模式
	if(MemDb::GetRealData(realData, regStart + 33, true))
		ret = rtu.PresetSingleRegister(addr, start + 0, realData.pv.data.iValue);
	
	// 电压给定
	if(MemDb::GetRealData(realData, regStart + 34, true))
		ret = rtu.PresetSingleRegister(addr, start + 1, realData.pv.data.iValue * 10);

	// 电流给定
	if(MemDb::GetRealData(realData, regStart + 35, true))
		ret = rtu.PresetSingleRegister(addr, start + 2, realData.pv.data.iValue * 10);

	// 功率给定
	if(MemDb::GetRealData(realData, regStart + 36, true))
		ret = rtu.PresetSingleRegister(addr, start + 3, realData.pv.data.iValue * 100);


	if(ErrorInfo::ERR_OK != ret){
		zlog_warn(Util::m_zlog, "下发数据到%s ACDC 6号命令失败", dev->name.c_str());
	}

	
	return ret;

}

int CNCU::ESCPreset(Device::SlaveDev *dev){
	int ret = ErrorInfo::ERR_OK;

	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0x1000;
	int data = 0;

	UshortArray values;
	RealData realData;

	if(!m_tcpClient->IsOpened()){
		return ErrorInfo::ERR_OPENED;
	}
	
	values.clear();

	ModbusRtuMaster rtu;
	rtu.SetIsTcp(m_isTcp);
	rtu.SetTcpClient(m_tcpClient);


	/*	Mingyu - 2021-03-02
	 *	功能码 05
	 */
	// 启停控制
	if( MemDb::GetRealData(realData, regStart + 229, true) )
		ret = rtu.ForceSingleCoil(addr, start + 0 , realData.pv.data.iValue);

	// 故障复位
	if( MemDb::GetRealData(realData, regStart + 230, true) )
		ret = rtu.ForceSingleCoil(addr, start + 1 , realData.pv.data.iValue);

	if(ErrorInfo::ERR_OK != ret){
		zlog_warn(Util::m_zlog, "下发数据到%s ESC 5号命令失败", dev->name.c_str());
	}


	/*	Mingyu - 2021-03-02
	 *	功能码 06
	 */
	// 工作模式
	if(MemDb::GetRealData(realData, regStart + 231, true))
		ret = rtu.PresetSingleRegister(addr, start + 0, realData.pv.data.iValue);
	
	// 电压给定
	if(MemDb::GetRealData(realData, regStart + 232, true))
		ret = rtu.PresetSingleRegister(addr, start + 1, realData.pv.data.iValue * 10);

	// 电流给定
	if(MemDb::GetRealData(realData, regStart + 233, true))
		ret = rtu.PresetSingleRegister(addr, start + 2, realData.pv.data.iValue * 10);

	// 功率给定
	if(MemDb::GetRealData(realData, regStart + 234, true))
		ret = rtu.PresetSingleRegister(addr, start + 3, realData.pv.data.iValue * 100);

	if(ErrorInfo::ERR_OK != ret)
		zlog_warn(Util::m_zlog, "下发数据到%s ESC 6号命令失败", dev->name.c_str());
	
	return ret;



}

int CNCU::PVCPreset(Device::SlaveDev *dev){
	int ret = ErrorInfo::ERR_OK;

	bool comm = false;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0x2000;
	int data = 0;
	float tmp = 0;

	UshortArray values;
	RealData realData;

	if(!m_tcpClient->IsOpened()){
		return ErrorInfo::ERR_OPENED;
	}
	
	values.clear();

	ModbusRtuMaster rtu;
	rtu.SetIsTcp(m_isTcp);
	rtu.SetTcpClient(m_tcpClient);


	/*	Mingyu - 2021-03-02
	 *	功能码 05
	 */
	// 启停控制
	if( MemDb::GetRealData(realData, regStart + 263, true) )
		ret = rtu.ForceSingleCoil(addr, start + 0 , realData.pv.data.iValue);

	// 故障复位
	if( MemDb::GetRealData(realData, regStart + 264, true) )
		ret = rtu.ForceSingleCoil(addr, start + 1 , realData.pv.data.iValue);

	if(ErrorInfo::ERR_OK != ret){
		zlog_warn(Util::m_zlog, "下发数据到%s PVC 5号命令失败", dev->name.c_str());
	}
	/*	Mingyu - 2021-03-02
	 *	功能码 06
	 */
	// 工作模式
	if(MemDb::GetRealData(realData, regStart + 265, true))
		ret = rtu.PresetSingleRegister(addr, start + 0, realData.pv.data.iValue);
	
	// 电压给定
	if(MemDb::GetRealData(realData, regStart + 266, true))
		ret = rtu.PresetSingleRegister(addr, start + 1, realData.pv.data.iValue * 10);

	// 电流给定
	if(MemDb::GetRealData(realData, regStart + 267, true))
		ret = rtu.PresetSingleRegister(addr, start + 2, realData.pv.data.iValue * 10);

	// 功率给定
	if(MemDb::GetRealData(realData, regStart + 268, true)){
		tmp = realData.pv.data.dbValue;
		ret = rtu.PresetSingleRegister(addr, start + 3, tmp * 100);
	}


	if(ErrorInfo::ERR_OK != ret){
		zlog_warn(Util::m_zlog, "下发数据到%s PVC 6号命令失败", dev->name.c_str());
	}

	
	return ret;


}


int CNCU::SubESCReadInputStatus(int regStart, int start, int addr){
	int ret = ErrorInfo::ERR_OK;
	int length = 0;
	bool status = 0;
	int subESCcout = 0;
	bool comm = 0;



	ModbusRtuMaster rtu;
	rtu.SetIsTcp(m_isTcp);
	rtu.SetTcpClient(m_tcpClient);

	UshortArray values;
	for(subESCcout = 0; subESCcout < SUM_CNCU_ESC + 1 ; subESCcout++ ){
		values.clear();

		// 功能码02
		length = 16;
		ret = rtu.ReadInputStatus(values, addr, start, length); // 读取停机状态
		if ( ErrorInfo::ERR_OK == ret) {
			comm = true;
			status = (unsigned short)(values[0]); // 运行状态
			MemDb::SetRealData(regStart + 0, status, false); 

			status = (unsigned short)(values[1]); // 故障状态
			MemDb::SetRealData(regStart + 1, status, false); 

			status = (unsigned short)values[2]; // 告警状态
			MemDb::SetRealData(regStart + 2, status, false);

		
			status = (unsigned short)values[3]; // 充电状态
			MemDb::SetRealData(regStart + 3, status, false);

			status = (unsigned short)values[4]; // 放点电状态
			MemDb::SetRealData(regStart + 4, status, false);

			status = (unsigned short)values[5]; // 急停输入状态
			MemDb::SetRealData(regStart + 5, status, false);

			status = (unsigned short)values[6]; // 主断路器
			MemDb::SetRealData(regStart + 6, status, false);

			status = (unsigned short)values[7]; // 预留
			MemDb::SetRealData(regStart + 7, status, false);

			status = (unsigned short)values[8]; // 预留
			MemDb::SetRealData(regStart + 8, status, false);

			status = (unsigned short)values[9]; // 预留
			MemDb::SetRealData(regStart + 9, status, false);
			
			status = (unsigned short)values[10]; // 预留
			MemDb::SetRealData(regStart + 10, status, false);

			status = (unsigned short)values[11]; // 预留
			MemDb::SetRealData(regStart + 11, status, false);

			status = (unsigned short)values[12]; // 预留
			MemDb::SetRealData(regStart + 12, status, false);
			
			status = (unsigned short)values[13]; // 预留
			MemDb::SetRealData(regStart + 13, status, false);

			status = (unsigned short)values[14]; // 预留
			MemDb::SetRealData(regStart + 14, status, false);

			status = (unsigned short)values[15]; // 预留
			MemDb::SetRealData(regStart + 15, status, false);

		}
		else {
			comm = false;
			zlog_warn(Util::m_zlog,"读取 ESC 子模块 02 序号:%d 失败", subESCcout);
			return subESCcout;
		}

		regStart = regStart + length;
		start = start + 0x10;
	}

	return ErrorInfo::ERR_OK;
}

int CNCU::SubESCReadInputRegisters(int regStart, int start, int addr){
	int ret = ErrorInfo::ERR_OK;
	int length = 19;
	int data = 0;
	float tmp = 0.0;


	ModbusRtuMaster rtu;
	rtu.SetIsTcp(m_isTcp);
	rtu.SetTcpClient(m_tcpClient);

	UshortArray values;
	//for (int subESCcout = 0 ; subESCcout < SUM_CNCU_ESC; subESCcout++) {
	values.clear();

		ret = rtu.ReadInputRegisters(values, addr, start, length);
		if ( ErrorInfo::ERR_OK == ret){
			
			// SOC
			data = (unsigned short)		values[0];			MemDb::SetRealData(regStart + 0, data, false);	 
			
			// 电池侧电压
			tmp = (unsigned short)		values[1] * 0.1;			MemDb::SetRealData(regStart + 1, tmp, false);	 
			
			// 电池侧电流
			tmp = (short)			 	values[2] * 0.1;			MemDb::SetRealData(regStart + 2, tmp, false);	 
			
			// 电池侧功率
			tmp = (short)				values[3] * 0.01;			MemDb::SetRealData(regStart + 3, tmp, false);	 
			
			// BUS侧电压
			tmp = (unsigned short)		values[4] * 0.1;			MemDb::SetRealData(regStart + 4, tmp, false);	 
			
			// BUS侧电流
			tmp = (short)				values[5] * 0.1;			MemDb::SetRealData(regStart + 5, tmp, false);	 
			
			// BUS侧功率
			tmp = (short)				values[6] * 0.01;			MemDb::SetRealData(regStart + 6, tmp, false);	 
			
			// 最高单体电压
			tmp = (unsigned short)		values[7] * 0.001;			MemDb::SetRealData(regStart + 7, tmp, false);	 
			
			// 最高单体电压所在组号		
			data = (unsigned short)		values[8];			MemDb::SetRealData(regStart + 8, data, false);	 
			
			// 最高单体电压所在组号序号
			data = (unsigned short)		values[9];			MemDb::SetRealData(regStart + 9, data, false);	 
			
			// 最低单体电压
			tmp = (unsigned short)		values[10] * 0.001;			MemDb::SetRealData(regStart + 10, tmp, false);	 
			
			// 最低单体电压所在组号		
			data = (unsigned short)		values[11];			MemDb::SetRealData(regStart + 11, data, false);	 
			
			// 最低单体电压所在组号序号
			data = (unsigned short)		values[12];			MemDb::SetRealData(regStart + 12, data, false);	 
			
			// 最高单体温度
			data = (unsigned short)		values[13];			MemDb::SetRealData(regStart + 13, data, false);	 
			
			// 最高单体温度所在组号
			data = (unsigned short)		values[14];			MemDb::SetRealData(regStart + 14, data, false);	 
			
			// 最高单体温度所在组号序号
			data = (unsigned short)		values[15];			MemDb::SetRealData(regStart + 15, data, false);	 
			
			// 最低单体温度
			data = (unsigned short)		values[16];			MemDb::SetRealData(regStart + 16, data, false);	 
			
			// 最低单体温度所在组号
			data = (unsigned short)		values[17];			MemDb::SetRealData(regStart + 17, data, false);	 
			
			// 最低单体温度所在组号序号
			data = (unsigned short)		values[18];			MemDb::SetRealData(regStart + 18, data, false);	 

			//regStart = regStart + length;
			//start = start + 0x20;

		}
		else{
			zlog_warn(Util::m_zlog, "读取 ESC 04 功能 ESC 子模块失败");
			return ret; // 子模块序号从1 开始, 0系统整体。
		}
	//}

	return ErrorInfo::ERR_OK;
}


int CNCU::OverallPreset(Device::SlaveDev *dev){

	int ret = ErrorInfo::ERR_OK;

	//bool comm = false;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0x3000;
	//int data = 0;

	UshortArray values;
	RealData realData;

	if(!m_tcpClient->IsOpened()){
		return ErrorInfo::ERR_OPENED;
	}
	
	values.clear();

	ModbusRtuMaster rtu;
	rtu.SetIsTcp(m_isTcp);
	rtu.SetTcpClient(m_tcpClient);

	/*	Mingyu - 2021-03-02
	 *	功能码 06
	 */
	// 系统工作模式设定
	if(MemDb::GetRealData(realData, regStart + 272, true))
		ret = rtu.PresetSingleRegister(addr, start + 0, realData.pv.data.iValue);
	
	// 系统母线电压设定
	if(MemDb::GetRealData(realData, regStart + 273, true))
		ret = rtu.PresetSingleRegister(addr, start + 1, realData.pv.data.iValue * 10);

	if(ErrorInfo::ERR_OK != ret){
		zlog_warn(Util::m_zlog, "下发数据到%s Overall 6号命令失败", dev->name.c_str());
	}

	
	return ret;

}








