/*
 * PowerDetection.cpp
 *
 *  Created on: 2014-3-13
 *      Author: liuhaitao
 */

#include "PowerDetection.h"
#include "../../../util/Util.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"

PowerDetection::PowerDetection()
{
	// TODO Auto-generated constructor stub
	m_tcpClient = NULL;
	m_com = NULL;
	m_isTcp = false;
}

PowerDetection::~PowerDetection()
{
	// TODO Auto-generated destructor stub

}

//设置是否采用以太网通讯
void PowerDetection::SetIsTcp(bool isTcp)
{
	m_isTcp = isTcp;
}

//设置TcpClient指针
void PowerDetection::SetTcp(TcpClient* tcpClient)
{
	m_tcpClient = tcpClient;
}

void PowerDetection::SetCom(Com* com)
{
	assert(com!=NULL);
	m_com = com;
}

int PowerDetection::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;

	if(NULL==dev)
	{
		return ret;
	}

	//判断设备名称
	//判断设备厂商和型号来确定具体通信协议
	switch (dev->devManu)
	{
	case DevManu::Manu_ACREL:
		ret = this->APM810Read(dev);
		break;
	default:
		break;
	};

	return ret;
}

int PowerDetection::APMD730Read(Device::SlaveDev* dev)
{
	zlog_info(Util::m_zlog, "读取%s数据开始",dev->name.c_str());
	bool comm = false;
	int ret = ErrorInfo::ERR_OK;
	int start = 0;
	int length = 8;
	int data = 0;
	float tmp = 0.0;
	int ctI = 0;//电流变比
	int ctV = 0;//电压变比
	int moc = 0;//接线方式（mode of connection）

	if (NULL == dev)
	{
		zlog_warn(Util::m_zlog, "从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	//判断串口是否打开
	if (!m_com->IsOpen())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetCom(m_com);

	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;

	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		moc = (short)values[3];
		MemDb::SetRealData(regStart + 1, moc, false);//接线方式
		ctV = (short)values[6];
		MemDb::SetRealData(regStart + 2, ctV, false);//电压变比
		ctI = (short)values[7];
		MemDb::SetRealData(regStart + 3, ctI, false);//电流变比
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s的地址%d的数据失败",dev->name.c_str(),start);\
		ret = ErrorInfo::ERR_READ;
	}

	start = 243;
	length = 58;
	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		if(moc == 1)//三相三线
		{
			tmp = (short)values[0] * 0.1 * ctV;
			MemDb::SetRealData(regStart + 4, tmp, false);//Uab电压
			tmp = (short)values[1] * 0.1 * ctV;
			MemDb::SetRealData(regStart + 5, tmp, false);//Ubc电压
			tmp = (short)values[2] * 0.1 * ctV;
			MemDb::SetRealData(regStart + 6, tmp, false);//Uca电压
		}
		else if(moc == 2)//三相四线
		{
			tmp = (short)values[3] * 0.1 * ctV;
			MemDb::SetRealData(regStart + 4, tmp, false);//Uab电压
			tmp = (short)values[4] * 0.1 * ctV;
			MemDb::SetRealData(regStart + 5, tmp, false);//Ubc电压
			tmp = (short)values[5] * 0.1 * ctV;
			MemDb::SetRealData(regStart + 6, tmp, false);//Uca电压
		}
		tmp = (short)values[6] * 0.001 * ctI;
		MemDb::SetRealData(regStart + 7, tmp, false);//A相电流
		tmp = (short)values[7] * 0.001 * ctI;
		MemDb::SetRealData(regStart + 8, tmp, false);//B相电流
		tmp = (short)values[8] * 0.001 * ctI;
		MemDb::SetRealData(regStart + 9, tmp, false);//C相电流
		tmp = (short)values[9] * 0.01;
		MemDb::SetRealData(regStart + 10, tmp, false);//频率

		//有功功率
		tmp = ((((short)values[10])<<16)|((short)values[11])) * 0.01 * ctI * ctV;
		MemDb::SetRealData(regStart + 11, tmp, false);//A相有功功率
		tmp = ((((short)values[12])<<16)|((short)values[13])) * 0.01 * ctI * ctV;
		MemDb::SetRealData(regStart + 12, tmp, false);//B相有功功率
		tmp = ((((short)values[14])<<16)|((short)values[15])) * 0.01 * ctI * ctV;
		MemDb::SetRealData(regStart + 13, tmp, false);//C相有功功率

		tmp = ((((short)values[16])<<16)|((short)values[17])) * 0.01 * ctI * ctV;
		MemDb::SetRealData(regStart + 14, tmp, false);//总有功功率

		//无功功率
		tmp = ((((short)values[18])<<16)|((short)values[19])) * 0.01 * ctI * ctV;
		MemDb::SetRealData(regStart + 15, tmp, false);//A相无功功率
		tmp = ((((short)values[20])<<16)|((short)values[21])) * 0.01 * ctI * ctV;
		MemDb::SetRealData(regStart + 16, tmp, false);//B相无功功率
		tmp = ((((short)values[22])<<16)|((short)values[23])) * 0.01 * ctI * ctV;
		MemDb::SetRealData(regStart + 17, tmp, false);//C相无功功率
		tmp = ((((short)values[24])<<16)|((short)values[25])) * 0.01 * ctI * ctV;
		MemDb::SetRealData(regStart + 18, tmp, false);//总无功功率

		//视在功率
		tmp = ((((short)values[26])<<16)|((short)values[27])) * 0.01 * ctI * ctV;
		MemDb::SetRealData(regStart + 19, tmp, false);//A相视在功率
		tmp = ((((short)values[28])<<16)|((short)values[29])) * 0.01 * ctI * ctV;
		MemDb::SetRealData(regStart + 20, tmp, false);//B相视在功率
		tmp = ((((short)values[30])<<16)|((short)values[31])) * 0.01 * ctI * ctV;
		MemDb::SetRealData(regStart + 21, tmp, false);//C相视在功率
		tmp = ((((short)values[32])<<16)|((short)values[33])) * 0.01 * ctI * ctV;
		MemDb::SetRealData(regStart + 22, tmp, false);//总视在功率

		//功率因数
		tmp = (short)values[34] * 0.001;
		MemDb::SetRealData(regStart + 23, tmp, false);//A相功率因数
		tmp = (short)values[35] * 0.001;
		MemDb::SetRealData(regStart + 24, tmp, false);//B相功率因数
		tmp = (short)values[36] * 0.001;
		MemDb::SetRealData(regStart + 25, tmp, false);//C相功率因数
		tmp = (short)values[37] * 0.001;
		MemDb::SetRealData(regStart + 26, tmp, false);//总功率因数

		tmp = (short)values[56] * 0.1;
		MemDb::SetRealData(regStart + 27, tmp, false);//电压不平衡度
		tmp = (short)values[57] * 0.1;
		MemDb::SetRealData(regStart + 28, tmp, false);//电流不平衡度
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s地址%d的数据失败",dev->name.c_str(),start);
		ret = ErrorInfo::ERR_READ;
	}

	start = 365;
	length = 8;
	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		tmp = ((((short)values[0]) << 8)|((short)values[1])) * 0.01 * ctI * ctV;
		MemDb::SetRealData(regStart + 35, tmp, false);//正向有功电能
		tmp = ((((short)values[2]) << 8)|((short)values[3])) * 0.01 * ctI * ctV;
		MemDb::SetRealData(regStart + 36, tmp, false);//反向有功电能
		tmp = ((((short)values[4]) << 8)|((short)values[5])) * 0.01 * ctI * ctV;
		MemDb::SetRealData(regStart + 37, tmp, false);//正向无功电能
		tmp = ((((short)values[6]) << 8)|((short)values[7])) * 0.01 * ctI * ctV;
		MemDb::SetRealData(regStart + 38, tmp, false);//反向无功电能
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s地址%d的数据失败",dev->name.c_str(),start);
		ret = ErrorInfo::ERR_READ;
	}

	start = 553;
	length = 6;
	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;

		tmp = (short)values[0] * 0.01;
		MemDb::SetRealData(regStart + 29, tmp, false);//A相电压总谐波畸变率
		tmp = (short)values[1] * 0.01;
		MemDb::SetRealData(regStart + 30, tmp, false);//B相电压总谐波畸变率
		tmp = (short)values[2] * 0.01;
		MemDb::SetRealData(regStart + 31, tmp, false);//C相电压总谐波畸变率
		tmp = (short)values[3] * 0.01;
		MemDb::SetRealData(regStart + 32, tmp, false);//A相电流总谐波畸变率
		tmp = (short)values[4] * 0.01;
		MemDb::SetRealData(regStart + 33, tmp, false);//B相电流总谐波畸变率
		tmp = (short)values[5] * 0.01;
		MemDb::SetRealData(regStart + 34, tmp, false);//C相电流总谐波畸变率
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s地址%d的数据失败",dev->name.c_str(),start);
		ret = ErrorInfo::ERR_READ;
	}

	MemDb::SetRealData(regStart + 0, comm, false);

	zlog_info(Util::m_zlog,"读取%s数据结束",dev->name.c_str());

	return ret;
}

int PowerDetection::APM810Read(Device::SlaveDev* dev)
{
	zlog_info(Util::m_zlog, "读取%s数据开始",dev->name.c_str());
	bool comm = false;
	int ret = ErrorInfo::ERR_OK;
	int start = 0;
	int length = 0;
	int data = 0;
	float tmp = 0.0;
	float fTmp = 0.0;

	if (NULL == dev)
	{
		zlog_warn(Util::m_zlog, "从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	//判断串口是否打开
	if (!m_tcpClient->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetIsTcp(m_isTcp);
	rtu.SetTcpClient(m_tcpClient);

	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;

	start = 1100;
	length = 84;
	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;

		//电流
		tmp = ((((short)values[0])<<16)|((short)values[1])) * 0.001;
		MemDb::SetRealData(regStart + 4, tmp, false);//A相电流
		tmp = ((((short)values[2])<<16)|((short)values[3])) * 0.001;
		MemDb::SetRealData(regStart + 5, tmp, false);//B相电流
		tmp = ((((short)values[4])<<16)|((short)values[5])) * 0.001;
		MemDb::SetRealData(regStart + 6, tmp, false);//C相电流


		//电压
		tmp = ((((short)values[28])<<16)|((short)values[29])) * 0.1;
		MemDb::SetRealData(regStart + 1, tmp, false);//A相电压
		tmp = ((((short)values[30])<<16)|((short)values[31])) * 0.1;
		MemDb::SetRealData(regStart + 2, tmp, false);//B相电压
		tmp = ((((short)values[32])<<16)|((short)values[33])) * 0.1;
		MemDb::SetRealData(regStart + 3, tmp, false);//C相电压

		tmp = (short)values[79] * 0.01;
		MemDb::SetRealData(regStart + 7, tmp, false);//频率

		//有功功率
		data = ((((short)values[50]) * 65536) + ((short)values[51]));
		memcpy(&fTmp,&data,sizeof(int));
		tmp = fTmp * 0.01;
		MemDb::SetRealData(regStart + 8, tmp, false);//A相有功功率

		data = ((((short)values[52]) * 65536) + ((short)values[53]));
		memcpy(&fTmp,&data,sizeof(int));
		tmp = fTmp * 0.01;
		MemDb::SetRealData(regStart + 9, tmp, false);//B相有功功率

		data = ((((short)values[54]) * 65536) + ((short)values[55]));
		memcpy(&fTmp,&data,sizeof(int));
		tmp = fTmp * 0.01;
		MemDb::SetRealData(regStart + 10, tmp, false);//C相有功功率

		data = ((((short)values[56]) * 65536) + ((short)values[57]));
		memcpy(&fTmp,&data,sizeof(int));
		tmp = fTmp * 0.01;
		MemDb::SetRealData(regStart + 11, tmp, false);//总有功功率

		//无功功率
		data = ((((short)values[58]) * 65536) + ((short)values[59]));
		memcpy(&fTmp,&data,sizeof(int));
		tmp = fTmp * 0.01;
		MemDb::SetRealData(regStart + 12, tmp, false);//A相无功功率
		data = ((((short)values[60]) * 65536) + ((short)values[61]));
		memcpy(&fTmp,&data,sizeof(int));
		tmp = fTmp * 0.01;
		MemDb::SetRealData(regStart + 13, tmp, false);//B相无功功率
		data = ((((short)values[62]) * 65536) + ((short)values[63]));
		memcpy(&fTmp,&data,sizeof(int));
		tmp = fTmp * 0.01;
		MemDb::SetRealData(regStart + 14, tmp, false);//C相无功功率
		data = ((((short)values[64]) * 65536) + ((short)values[65]));
		memcpy(&fTmp,&data,sizeof(int));
		tmp = fTmp * 0.01;
		MemDb::SetRealData(regStart + 15, tmp, false);//总无功功率

		//视在功率
		data = ((((short)values[66]) * 65536) + ((short)values[67]));
		memcpy(&fTmp,&data,sizeof(int));
		tmp = fTmp * 0.01;
		MemDb::SetRealData(regStart + 16, tmp, false);//A相视在功率
		data = ((((short)values[68]) * 65536) + ((short)values[69]));
		memcpy(&fTmp,&data,sizeof(int));
		tmp = fTmp * 0.01;
		MemDb::SetRealData(regStart + 17, tmp, false);//B相视在功率
		data = ((((short)values[70]) * 65536) + ((short)values[71]));
		memcpy(&fTmp,&data,sizeof(int));
		tmp = fTmp * 0.01;
		MemDb::SetRealData(regStart + 18, tmp, false);//C相视在功率
		data = ((((short)values[72]) * 65536) + ((short)values[73]));
		memcpy(&fTmp,&data,sizeof(int));
		tmp = fTmp * 0.01;
		MemDb::SetRealData(regStart + 19, tmp, false);//总视在功率

		//功率因数
		tmp = (short)values[80] * 0.001;
		MemDb::SetRealData(regStart + 20, tmp, false);//A相功率因数
		tmp = (short)values[81] * 0.001;
		MemDb::SetRealData(regStart + 21, tmp, false);//B相功率因数
		tmp = (short)values[82] * 0.001;
		MemDb::SetRealData(regStart + 22, tmp, false);//C相功率因数
		tmp = (short)values[83] * 0.001;
		MemDb::SetRealData(regStart + 23, tmp, false);//总功率因数

		tmp = (short)values[39] * 0.1;
		MemDb::SetRealData(regStart + 24, tmp, false);//电压不平衡度
		tmp = (short)values[13] * 0.1;
		MemDb::SetRealData(regStart + 25, tmp, false);//电流不平衡度
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s地址%d的数据失败",dev->name.c_str(),start);
		ret = ErrorInfo::ERR_READ;
	}

	start = 3000;
	length = 8;
	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;
		data = ((((short)values[0]) * 65536) + ((short)values[1]));
		memcpy(&fTmp,&data,sizeof(int));
		tmp = fTmp * 0.001;
		MemDb::SetRealData(regStart + 32, tmp, false);//正向有功电能
		data = ((((short)values[2]) * 65536) + ((short)values[3]));
		memcpy(&fTmp,&data,sizeof(int));
		tmp = fTmp * 0.001;
		MemDb::SetRealData(regStart + 33, tmp, false);//反向有功电能
		data = ((((short)values[4]) * 65536) + ((short)values[5]));
		memcpy(&fTmp,&data,sizeof(int));
		tmp = fTmp * 0.001;
		MemDb::SetRealData(regStart + 34, tmp, false);//正向无功电能
		data = ((((short)values[6]) * 65536) + ((short)values[7]));
		memcpy(&fTmp,&data,sizeof(int));
		tmp = fTmp * 0.001;
		MemDb::SetRealData(regStart + 35, tmp, false);//反向无功电能
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s地址%d的数据失败",dev->name.c_str(),start);
		ret = ErrorInfo::ERR_READ;
	}

	start = 4872;
	length = 6;
	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if(ret == ErrorInfo::ERR_OK)
	{
		comm = true;

		tmp = (short)values[3] * 0.01;
		MemDb::SetRealData(regStart + 26, tmp, false);//A相电压总谐波畸变率
		tmp = (short)values[4] * 0.01;
		MemDb::SetRealData(regStart + 27, tmp, false);//B相电压总谐波畸变率
		tmp = (short)values[5] * 0.01;
		MemDb::SetRealData(regStart + 28, tmp, false);//C相电压总谐波畸变率
		tmp = (short)values[0] * 0.01;
		MemDb::SetRealData(regStart + 29, tmp, false);//A相电流总谐波畸变率
		tmp = (short)values[1] * 0.01;
		MemDb::SetRealData(regStart + 30, tmp, false);//B相电流总谐波畸变率
		tmp = (short)values[2] * 0.01;
		MemDb::SetRealData(regStart + 31, tmp, false);//C相电流总谐波畸变率
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s地址%d的数据失败",dev->name.c_str(),start);
		ret = ErrorInfo::ERR_READ;
	}

	MemDb::SetRealData(regStart + 0, comm, false);

	zlog_info(Util::m_zlog,"读取%s数据结束",dev->name.c_str());

	return ret;
}
