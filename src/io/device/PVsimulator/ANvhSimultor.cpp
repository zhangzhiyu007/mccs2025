/*
 * ANvhSimultor.cpp
 *
 *  Created on: 2023-4-27
 *      Author: 16843
 */

#include "ANvhSimultor.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

ANvhSimultor::ANvhSimultor()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_tcpClient = NULL;
	m_delayTime = 1000;
	m_retries = 3;
}

ANvhSimultor::~ANvhSimultor()
{
	// TODO Auto-generated destructor stub
}

void ANvhSimultor::SetCom(Com* com)
{
	assert(NULL!=com);
	m_com = com;
}

//设置TcpClient指针
void ANvhSimultor::SetTcp(TcpClient* tcpClient)
{
	m_tcpClient = tcpClient;
}


int ANvhSimultor::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_ACLT_anevh:
		ret = this->AcltAnvhRead(dev);
		break;
	default:
		break;
		
	};

	return ret;
}

int ANvhSimultor::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_ACLT_anevh:
		ret = this->AcltAnvhPreset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int ANvhSimultor::AcltAnvhRead(Device::SlaveDev* dev)
{
	zlog_info(Util::m_zlog,"读取艾诺光伏电池模拟器数据");
	bool comm = false;
	int ret = ErrorInfo::ERR_OK;
	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	float tmp = 0;
	int tmp1 = 0;

	if (NULL == dev)
	{
		zlog_warn(Util::m_zlog,"从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	//判断串口是否打开
	if (!m_tcpClient->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	//查询设定的固定电池参数
	msleep(100);
	ret = AnRead(addr, 0xD5, 0x01, 14, values);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		MemDb::SetRealData(regStart + 1, values[0], false);//查询电池类型
		MemDb::SetRealData(regStart + 2, values[1], false);//查询电池串联数量
		MemDb::SetRealData(regStart + 3, values[2], false);//查询电池并联数量
		MemDb::SetRealData(regStart + 4, values[3], false);//查询电池初始SOC

	}
	else
	{
		zlog_warn(Util::m_zlog, "读取%s查询设定的固定电池参数，结果=%d",dev->name.c_str(),ret);
	}

	msleep(100);
	values.clear();
	ret = AnRead(addr, 0xD5, 0x02, 18, values);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = values[0] * 0.01;
		MemDb::SetRealData(regStart + 5, tmp, false);//单体上限电压
		tmp = values[1] * 0.01;
		MemDb::SetRealData(regStart + 6, tmp, false);//单体下限电压
		tmp = values[2] * 0.01;
		MemDb::SetRealData(regStart + 7, tmp, false);//单体标准电压
		tmp = values[3] * 0.1;
		MemDb::SetRealData(regStart + 8, tmp, false);//并联容量
		tmp = values[4] * 0.1;
		MemDb::SetRealData(regStart + 9, tmp, false);//串联电压E0

	}
	else
	{
		zlog_warn(Util::m_zlog, "读取%s查询特征显示参数失败，结果=%d",dev->name.c_str(),ret);
	}
	//msleep(150);


	msleep(100);
	values.clear();
	ret = AnRead(addr, 0xD5, 0x08, 9, values);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = values[0];
		MemDb::SetRealData(regStart + 10, tmp, false);//当前运行的soc
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取%s电池SOC值失败，结果=%d",dev->name.c_str(),ret);
	}

	msleep(100);
	ret = AnRead(addr, 0xD5, 0x09, 21, values);
	if (ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = values[0] * 0.01;//充电电能;
		MemDb::SetRealData(regStart + 11, tmp, false);//充电电能，耗能
		tmp = values[1] * 0.01;//馈能电能;
		MemDb::SetRealData(regStart + 12, tmp, false);//放电电能，馈能
		tmp = values[2] * 0.1;//计算的安时;
		MemDb::SetRealData(regStart + 13, tmp, false);//表计算的安时
		tmp1 = values[3];//运行小时;
		MemDb::SetRealData(regStart + 14, tmp1, false);//表计运行时间：小时
		tmp1 = values[4];//运行分钟;
		MemDb::SetRealData(regStart + 15, tmp1, false);//表计运行时间：分钟
		tmp1 = values[5];//运行秒;
		MemDb::SetRealData(regStart + 16, tmp1, false);//表计运行时间：秒

	}
	else
	{
		zlog_warn(Util::m_zlog, "读取%s电能计量的耗能、馈能、安时和运行时间值失败，结果=%d",dev->name.c_str(),ret);
	}

	msleep(100);
	ret = AnRead(addr, 0xf0, 0x80, 18, values);
	if (ErrorInfo::ERR_OK == ret)
		{
			comm = true;
			tmp = values[0] * 0.1;//实际电压;
			MemDb::SetRealData(regStart + 17, tmp, false);//


			// tmp = values[1] * 100;//实际电流;
			tmp = values[0] * 0.1* values[2] * 0.01;
			MemDb::SetRealData(regStart + 19, tmp, false);//


			tmp = values[2] * 0.01;//实际功率;
			MemDb::SetRealData(regStart + 18, tmp, false);//
		}
		else
		{
			zlog_warn(Util::m_zlog, "读取%s电能计量的耗能、馈能、安时和运行时间值失败，结果=%d",dev->name.c_str(),ret);
		}

	MemDb::SetRealData(regStart + 0, comm, false);
	return ret;
}

int ANvhSimultor::AcltAnvhPreset(Device::SlaveDev* dev)
{
	zlog_info(Util::m_zlog,"设置艾诺储能电池模拟器数据");
	int ret = ErrorInfo::ERR_OK;
	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	RealData realData;
	RealData voc;
	RealData isc;
	RealData vmp;
	RealData imp;
	int data = 0;
	int temp = 0;
	int regStart = dev->regStart;

	//判断串口是否打开
	if (!m_tcpClient->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	//设置储能模拟器页面 00: 常规界面   01:编程界面  02：内阻模式界面  03:固定电池模拟界面
	if (MemDb::GetRealData(realData, regStart + 30, true))
	{
		values.clear();
		if (realData.pv.type == RealData::INT)
		{
		values.push_back(realData.pv.data.iValue);
		}
		else if (realData.pv.type == RealData::DB)
		{
		values.push_back(realData.pv.data.dbValue);
		}
		ret = this->AnPreset(addr, 0x5a, 0x70, 9, values);
	}


	//判断是否有启停命令
	if (MemDb::GetRealDataWrited(realData, regStart + 31, true))
	{
		if (realData.pv.type == RealData::INT)
		{
			values.clear();
			if (realData.pv.data.iValue == 1)//启动
			{
				ret = this->AnPreset(addr, 0x0F, 0xFF, 8, values);
			}
			else if (realData.pv.data.iValue == 0)//停止
			{
				ret = this->AnPreset(addr, 0x0F, 0x00, 8, values);
			}
			else if (realData.pv.data.iValue == 2)//解除报警
			{
				ret = this->AnPreset(addr, 0x0F, 0x03, 8, values);
			}
		}
		else if (realData.pv.type == RealData::DB)
		{
			zlog_info(Util::m_zlog,"启停命令错误：%f",realData.pv.data.dbValue);
			ret = ErrorInfo::ERR_WRITE;
		}
	}

	//判断固定电池参数
	if (MemDb::GetRealDataWrited(realData, regStart + 32, true))
	{
		values.clear();
		int batteytype = 0;
        int seriesNmu = 0;
        int parallelNmu = 0;
        int initSOC = 0;
		if (realData.pv.data.iValue == 1)
		{
	        MemDb::GetRealData(batteytype, regStart + 33);
			values.push_back(batteytype);
	        MemDb::GetRealData(seriesNmu, regStart + 34);
	        values.push_back(seriesNmu);
	        MemDb::GetRealData(parallelNmu, regStart + 35);
	        values.push_back(parallelNmu);
	        MemDb::GetRealData(initSOC, regStart + 36);
	        values.push_back(initSOC);
		}
		ret = this->AnPreset(addr, 0x5D, 0x01, 14, values);
	}

	//开始电能计量
	if (MemDb::GetRealDataWrited(realData, regStart + 37, true))
	{
		values.clear();
		if (realData.pv.data.iValue== 1)
		{
		  ret = this->AnPreset(addr, 0x5D, 0x02, 8, values);
		}

	}
	//停止电能计量
	if (MemDb::GetRealDataWrited(realData, regStart + 38, true))
	{
		values.clear();
		if (realData.pv.data.iValue== 1)
		{
		  ret = this->AnPreset(addr, 0x5D, 0x03, 8, values);
		}
	}


    zlog_info(Util::m_zlog,"设置储能电池模拟器结束");
	return ret;
}

int ANvhSimultor::AnRead(int addr, int type, int data, short length,
		UshortArray& values)
{
	String query;//查询数组
	String reply;//响应数组
	int error = ErrorInfo::ERR_OK;
	unsigned short crc16;
	int byte = 0;
	int i = 0;
	int regLength = 8;

	//封装数据帧
	query.clear();
	query.push_back(0x7B);//起始帧
	query.push_back(regLength >> 8);//总字节数高字节
	query.push_back(regLength & 0xFF);//总字节数低字节
	query.push_back(addr);//从机地址
	query.push_back(type);//命令类型
	query.push_back(data);//命令字

	crc16 = Check::CheckSum(query);
	query.push_back(crc16);//校验位
	query.push_back(0x7D);//结束帧

	zlog_info(Util::m_zlog,"查询命令");

	values.clear();
	values.resize(length - 8);

	printf("下发数据:");
	for(int i = 0; i < query.size(); i++)
	{
		printf("%02x ", query[i]);
	}
	printf("\n");

	error = m_tcpClient->TxRxMsg(reply, query, length);

	printf("上传数据:");
	for(int i = 0; i < reply.size(); i++)
	{
		printf("%02x ", reply[i]);
	}
	printf("\n");


	if (error == ErrorInfo::ERR_OK)
	{
		if((reply[0] != 0x7B))
		{
			zlog_warn(Util::m_zlog,"反馈数据错误");
			return ErrorInfo::ERR_INV_RESP;
		}

		if (reply[4] == 0x99)
		{
			//错误码
			//0x01 校验和错误
			//0x02 命令类型错误
			//0x03 命令字错误
			//0x04 状态不符（当前的状态不允许执行接收的命令）
			//0x05 参数错误
			//0x06 保护报警
			//0x07 超量程范围
			//0x08 命令长度错误
			zlog_warn(Util::m_zlog,"查询命令错误，错误码：%d",reply[6]);
			error = ErrorInfo::ERR_INV_RESP;
		}
		else
		{
			if (reply.size() == 9)
			{
				values[0] = reply[6];
			}
			else if(reply.size() == 11)
			{
				values[0] = (reply[6] << 16) | (reply[7] << 8) | (reply[8]);
			}
			else if (reply.size() == 14)
			{
				values[0] = reply[6];//电池类型
				values[1] = (reply[7] << 8) | (reply[8]) ;//串联数量
				values[2] = (reply[9] << 8) | (reply[10]);//并联数量
				values[3] = (reply[11]);//初始的SOC
			}
			else if (reply.size() == 18)
			{
		    	values[0] = (reply[6] << 8) | (reply[7]) ;//单体上限电压 /100
		    	values[1] = (reply[8] << 8) | (reply[9]);//单体下限电压/100
		    	values[2] = (reply[10] << 8) | (reply[11]) ;//单体标准电压/100
				values[3] = (reply[12] << 8) | (reply[13]);//并联容量Qn/10
		    	values[4] = (reply[14] << 8) | (reply[15]) ;//串联电压E0/10
			}

			else if ((reply.size() == 18)&&( type == 0xf0))
			{
		    	values[0] = (reply[6] << 8) | (reply[7]) ;//单体上限电压 /100
		    	values[1] = (reply[8] << 24) | (reply[9] << 16) | (reply[10] << 8) | (reply[11]);// 电流/100
		      	values[2] = (reply[12] << 24) | (reply[13] << 16) | (reply[14] << 8) | (reply[15]);// power/1000
			}

			else if (reply.size() == 21)
			{
		    	values[0] = (reply[6] << 16) | (reply[7] << 8) | (reply[8]);;//计算的耗能，放电/100
				values[1] = (reply[9] << 16) | (reply[10] << 8) | (reply[11]);;//计算的馈能，充电/100
		    	values[2] = (reply[12] << 16) | (reply[13] << 8) | (reply[14]); ;//计算的安时/10
				values[3] = (reply[15] << 8) | (reply[16]);//运行的时间：时
		    	values[4] = reply[17] ;//运行的时间：分钟
		    	values[5] = reply[18] ;//运行的时间：秒
			}
			else
			{
				for (i = 6; i < length - 2; i += 2)
				{
					values[byte] = (reply[i] << 8) | (reply[i + 1]);
					byte++;
				}
			}
		}
	}

	return error;
}

int ANvhSimultor::AnPreset(int addr, int type, int data, short length,
		UshortArray values)
{
	int error = ErrorInfo::ERR_OK;
	String query;
	String reply;
	unsigned short crc16;
	int retry = 0;
	unsigned int i = 0;

	//封装控制类命令
	query.clear();
	query.push_back(0x7B);//起始帧
	query.push_back(length >> 8);//总字节数高字节
	query.push_back(length & 0xFF);//总字节数低字节
	query.push_back(addr);//从机地址
	query.push_back(type);//命令类型
	query.push_back(data);//命令字

	switch ( length )
	{
	case 9:
		query.push_back(values[0]);
		break;
	case 10:
	case 8:
		for (i = 0; i < values.size(); i++)
	    {
			query.push_back(((int) (values[i])) >> 8);
			query.push_back(((int) (values[i])) & 0xFF);
		}
		break;
	case 14:
			query.push_back((int) values[0]);
			query.push_back(((int) (values[1])) >> 8);
			query.push_back(((int) (values[1])) & 0xFF);
			query.push_back(((int) (values[2])) >> 8);
			query.push_back(((int) (values[2])) & 0xFF);
			query.push_back((int) values[3]);
		break;

	default:
		zlog_warn(Util::m_zlog,"未识别的命令字");
		break;
	}

//	for (i = 0; i < values.size(); i++)
//	//for (i = 0; i < 1; i++)
//	{
//		query.push_back(((int) (values[i])) >> 8);
//		query.push_back(((int) (values[i])) & 0xFF);
//	}
	crc16 = Check::CheckSum(query);
	query.push_back(crc16);//校验和
	query.push_back(0x7D);//结束帧

	zlog_info(Util::m_zlog,"查询命令");
	do
	{
		printf("下发数据:");
		for(int i = 0; i < query.size(); i++)
		{
			printf("%02x ", query[i]);
		}
		printf("\n");

		error = m_tcpClient->TxRxMsg(reply, query, 9);

		printf("上传数据:");
		for(int i = 0; i < reply.size(); i++)
		{
			printf("%02x ", reply[i]);
		}
		printf("\n");

		if (error == ErrorInfo::ERR_OK)
		{
			//判断应答地址,命令类型,命令是否与查询一致
			if((reply[0] != 0x7B))
			{
				zlog_warn(Util::m_zlog,"反馈数据错误");
				return ErrorInfo::ERR_INV_RESP;
			}
			if (reply[4] == 0x99)
			{
				//错误码
				//0x01 校验和错误
				//0x02 命令类型错误
				//0x03 命令字错误
				//0x04 状态不符（当前的状态不允许执行接收的命令）
				//0x05 参数错误
				//0x06 保护报警
				//0x07 超量程范围
				//0x08 命令长度错误
				zlog_warn(Util::m_zlog,"查询命令错误，错误码：%d",reply[6]);
				error = ErrorInfo::ERR_INV_RESP;
				break;
			}
			break;
		}
		else
		{
			usleep(m_delayTime * 1000);
		}
		retry++;
	} while ((retry < m_retries) && (ErrorInfo::ERR_OK != error));
	return error;
}
