/*
 * ModbusTcpServer.cpp
 *
 *   创建日期: 2013-4-11
 *   作         者: lht
 */

#include "ModbusTcpServer.h"

ModbusTcpServer::ModbusTcpServer(Socket* sock, void* param) :
	TcpService(sock), m_modbusCheck((ModbusCheck*) param)
{
	// TODO 构造函数

}

ModbusTcpServer::~ModbusTcpServer()
{
	// TODO 析构函数
}

//线程运行
void ModbusTcpServer::Run()
{
	zlog_warn(Util::m_zlog, "进入Modbus TCP服务器线程");

	ModbusTcp modbus;
	String dataRead;

	while (true)
	{
		//退出线程
		if (m_state == Thread::STOPPING)
		{
			zlog_warn(Util::m_zlog, "退出TCP服务器控制线程");
			break;
		}

		//判断是否连接
		if (!m_socket.IsOpened())
		{
			//连接断开
			zlog_warn(Util::m_zlog, "客户端断开连接,停止线程工作");
			break;
		}

		//执行读取以太网数据,并进行ModbusTCP解析工作,使用阻塞式读取
		zlog_debug(Util::m_zlog,"读取以太网数据");
		if (m_socket.Recv(dataRead, MAX_VALUE) <= 0)
		{
			//连接断开
			zlog_warn(Util::m_zlog, "客户端断开连接,停止线程工作");
			m_state = Thread::STOPPING;
			continue;
		}
		else
		{
			zlog_debug(Util::m_zlog,"对读取的数据进行解析");
			UnpackModbus(dataRead);
		}
	}

	zlog_warn(Util::m_zlog, "退出Modbus TCP服务器线程");
}

//设置从设备地址
void ModbusTcpServer::SetAddr(int addr)
{
	if ((addr >= 0) && (addr <= 255))
	{
		zlog_debug(Util::m_zlog,"设置从站地址");
		m_modbusCheck->addr = addr;
	}
}

//设置寄存器起始地址
void ModbusTcpServer::SetRegStart(int regStart)
{
	if ((regStart >= 0) && (regStart <= 65535))
	{
		m_modbusCheck->regStart = regStart;
	}
}

//设置寄存器结束地址
void ModbusTcpServer::SetRegEnd(int regEnd)
{
	if ((regEnd >= 0) && (regEnd <= 65535))
	{
		m_modbusCheck->regEnd = regEnd;
	}
}

//将接收到的Modbus数据进行解析,保存到ModbusRtu&中
int ModbusTcpServer::UnpackModbus(String& reqPdu)
{
	ModbusTcp modbus;
	int bit;
	int ret = ErrorInfo::ERR_OK;
	int queryLength;

	zlog_debug(Util::m_zlog,"对接收到的数据进行解析");
	if (ret == Check(reqPdu))
	{
		switch (reqPdu[7])
		{
		case 1:
		case 2:
			modbus.firstFlag = (((UCHAR) (reqPdu[0]) << 8) | (UCHAR) reqPdu[1]);
			modbus.secondFlag = ((UCHAR) (reqPdu[2] << 8) | (UCHAR) reqPdu[3]);
			modbus.order = (UCHAR) reqPdu[7];
			modbus.addr = (UCHAR) reqPdu[6];
			modbus.regStart = (((UCHAR) reqPdu[8] << 8) | (UCHAR) reqPdu[9]);
			modbus.regLength = (((UCHAR) reqPdu[10] << 8) | (UCHAR) reqPdu[11]);
			break;
		case 3:
		case 4:
			zlog_debug(Util::m_zlog,"执行3、4号命令");
			modbus.firstFlag = (((UCHAR) (reqPdu[0]) << 8) | (UCHAR) reqPdu[1]);
			modbus.secondFlag = ((UCHAR) (reqPdu[2] << 8) | (UCHAR) reqPdu[3]);
			modbus.order = (UCHAR) reqPdu[7];
			modbus.addr = (UCHAR) reqPdu[6];
			modbus.regStart = (((UCHAR) reqPdu[8] << 8) | (UCHAR) reqPdu[9]);
			modbus.regLength = (((UCHAR) reqPdu[10] << 8) | (UCHAR) reqPdu[11]);
			break;
		case 5:
			modbus.firstFlag = (((UCHAR) (reqPdu[0]) << 8) | (UCHAR) reqPdu[1]);
			modbus.secondFlag = ((UCHAR) (reqPdu[2] << 8) | (UCHAR) reqPdu[3]);
			modbus.order = (UCHAR) reqPdu[7];
			modbus.addr = (UCHAR) reqPdu[6];
			modbus.regStart = (((UCHAR) (reqPdu[8]) << 8) | (UCHAR) reqPdu[9]);
			if ((((UCHAR) (reqPdu[10]) << 8) | (UCHAR) reqPdu[11]) == 0xFF00)
			{
				modbus.data.push_back(1);
			}
			else
			{
				modbus.data.push_back(0);
			}
			break;
		case 6:
			modbus.firstFlag = (((UCHAR) (reqPdu[0]) << 8) | (UCHAR) reqPdu[1]);
			modbus.secondFlag = (((UCHAR) (reqPdu[2]) << 8) | (UCHAR) reqPdu[3]);
			modbus.order = (UCHAR) reqPdu[7];
			modbus.addr = (UCHAR) reqPdu[6];
			modbus.regStart = (((UCHAR) (reqPdu[8]) << 8) | (UCHAR) reqPdu[9]);
			modbus.data.push_back((((UCHAR) (reqPdu[10]) << 8) | (UCHAR) reqPdu[11]));
			break;
		case 15:
			modbus.firstFlag = (((UCHAR) (reqPdu[0]) << 8) | (UCHAR) reqPdu[1]);
			modbus.secondFlag = ((UCHAR) (reqPdu[2] << 8) | (UCHAR) reqPdu[3]);
			modbus.order = (UCHAR) reqPdu[7];
			modbus.addr = (UCHAR) reqPdu[6];
			modbus.regStart = (((UCHAR) (reqPdu[8]) << 8) | (UCHAR) reqPdu[9]);
			modbus.regLength = (((UCHAR) (reqPdu[10]) << 8) | (UCHAR) reqPdu[11]);
			queryLength = 13;
			bit = 0;
			for (int coil = 0; coil < modbus.regLength; coil++)
			{
				if (bit > 7)
				{
					bit = 0;
					queryLength++;
				}

				if ((reqPdu[queryLength] & (0x01 << bit)) > 0)
				{
					modbus.data.push_back(1);
				}
				else
				{
					modbus.data.push_back(0);
				}
				bit++;
			}
			break;
		case 16:
			modbus.firstFlag = (((UCHAR) (reqPdu[0]) << 8) | (UCHAR) reqPdu[1]);
			modbus.secondFlag = (((UCHAR) (reqPdu[2]) << 8) | (UCHAR) reqPdu[3]);
			modbus.order = (UCHAR) reqPdu[7];
			modbus.addr = (UCHAR) reqPdu[6];
			modbus.regStart = (((UCHAR) (reqPdu[8]) << 8) | (UCHAR) reqPdu[9]);
			modbus.regLength = (((UCHAR) (reqPdu[10]) << 8) | (UCHAR) reqPdu[11]);
			for (int i = 0; i < modbus.regLength; i++)
			{
				modbus.data.push_back(((UCHAR) (reqPdu[13 + 2* i ]) << 8)
						| (UCHAR) reqPdu[13 + 2* i + 1]);
			}
			break;
		default:
			break;
		}

		//对请求命令进行回应
		zlog_debug(Util::m_zlog,"对请求命令进行回应");
		PackModbus(modbus);
	}
	else
	{
		modbus.firstFlag = (((UCHAR) (reqPdu[0]) << 8) | (UCHAR) reqPdu[1]);
		modbus.secondFlag = (((UCHAR) (reqPdu[2]) << 8) | (UCHAR) reqPdu[3]);
		modbus.order = (UCHAR) reqPdu[7];
		modbus.addr = (UCHAR) reqPdu[6];

		zlog_info(Util::m_zlog,"请求命令包不正确，返回错误");
		PackErrorMessage(modbus);
	}

	zlog_debug(Util::m_zlog,"对数据解析完毕，并响应");
	return ErrorInfo::ERR_OK;
}

//将需要返回的ModbusRtu数据进行封装,并向以太网返回数据
int ModbusTcpServer::PackModbus(ModbusTcp& modbus)
{
	zlog_debug(Util::m_zlog,"对请求命令进行回应");

	RealData tmpData;
	ModbusTcpClient modbusServer;
	String data;
	int bit;
	int replyByte;
	int m;
	short tmp=0;
	int temp=0;

	data.clear();

	switch (modbus.order)
	{
	case 1:
	case 2:
		modbus.data.clear();

		zlog_debug(Util::m_zlog,"1、2号命令读取数据库数据");
		for (int i = 0; i < modbus.regLength; i++)
		{
			if (MemDb::GetRealData(tmpData, modbus.regStart + i))
			{
				if (tmpData.pv.type == RealData::INT)
				{
					modbus.data.push_back((USHORT)tmpData.pv.data.iValue);
				}
				else
				{
					modbus.data.push_back((USHORT)0);
				}

			}
			else
			{
				zlog_warn(Util::m_zlog,"读取数据库的开关量失败");
				PackErrorMessage(modbus);
				break;
			}
		}

		//将返回数据进行封装
		zlog_debug(Util::m_zlog,"将返回数据进行封装");
		data.push_back((UCHAR) (modbus.firstFlag >> 8));
		data.push_back((UCHAR) (modbus.firstFlag & 0xff));
		data.push_back((UCHAR) (modbus.secondFlag >> 8));
		data.push_back((UCHAR) (modbus.secondFlag & 0xff));
		data.push_back((UCHAR) 0);
		data.push_back((UCHAR) (3 + (modbus.regLength + 7) / 8));
		data.push_back((UCHAR) (modbus.addr));
		data.push_back((UCHAR) (modbus.order));
		data.push_back((UCHAR) ((modbus.regLength + 7) / 8));
		replyByte = 9;
		data.resize(9 + ((modbus.regLength + 7) / 8));
		bit = 0;
		for (int coil = 0; coil < (int)modbus.data.size(); coil++)
		{
			if (bit > 7)
			{
				bit = 0;
				replyByte++;
			}

			if (modbus.data[coil])
			{
				data[replyByte] = (data[replyByte] | (0x01 << bit));
			}
			bit++;
		}
		zlog_debug(Util::m_zlog,"向以太网发送数据");

		m_socket.Send(data);
		break;
	case 3:
	case 4:
		zlog_debug(Util::m_zlog,"3、4号命令读取数据库数据");
		modbus.data.clear();

		for (int i = 0; i < modbus.regLength; i++)
		{
			if (MemDb::GetRealData(tmpData, modbus.regStart + i))
			{
				if (tmpData.pv.type == RealData::INT)
				{
//					modbus.data.push_back((USHORT)tmpData.pv.data.iValue);
					modbus.data.push_back((short)tmpData.pv.data.iValue);
				}
				else
				{
//					modbus.data.push_back((USHORT)tmpData.pv.data.dbValue);
					modbus.data.push_back((short)tmpData.pv.data.dbValue);
				}
			}
			else
			{
				zlog_warn(Util::m_zlog,"读取数据库的寄存器值失败");
				PackErrorMessage(modbus);
				break;
			}
		}

		//将返回数据进行封装
		zlog_debug(Util::m_zlog,"将返回数据进行封装");

		data.push_back((UCHAR) (modbus.firstFlag >> 8));
		data.push_back((UCHAR) (modbus.firstFlag & 0xff));
		data.push_back((UCHAR) (modbus.secondFlag >> 8));
		data.push_back((UCHAR) (modbus.secondFlag & 0xff));
		data.push_back((UCHAR) 0);
		data.push_back((UCHAR) (3 + 2* modbus .regLength));
		data.push_back((UCHAR) (modbus.addr));
		data.push_back((UCHAR) (modbus.order));
		data.push_back((UCHAR) (2* modbus.regLength));

		m = 0;
		for (int j = 0; j < (int)modbus.data.size(); j++)
		{
			data.push_back((UCHAR) ((modbus.data[m] >> 8) & 0xff));
			data.push_back((UCHAR) (modbus.data[m] & 0xff));
			m++;
		}

		zlog_debug(Util::m_zlog,"向以太网发送数据");
		m_socket.Send(data);
		break;
	case 5:
		zlog_debug(Util::m_zlog,"5号命令设置单个开关量");

		if (MemDb::SetRealDataUnknown(modbus.regStart, modbus.data[0],true))
		{

			//将返回数据进行封装
			zlog_debug(Util::m_zlog,"将返回数据进行封装");
			data.push_back((UCHAR) ((modbus.firstFlag >> 8) & 0xff));
			data.push_back((UCHAR) (modbus.firstFlag & 0xff));
			data.push_back((UCHAR) ((modbus.secondFlag >> 8) & 0xff));
			data.push_back((UCHAR) (modbus.secondFlag & 0xff));
			data.push_back((UCHAR) 0);
			data.push_back(6);
			data.push_back(modbus.addr);
			data.push_back(modbus.order);
			data.push_back((UCHAR) ((modbus.regStart >> 8) & 0xff));
			data.push_back((UCHAR) (modbus.regStart & 0xff));
			data.push_back((UCHAR) ((modbusServer.ModBool(modbus.data[0]) >> 8)
					& 0xff));
			data.push_back(
					(UCHAR) (modbusServer.ModBool(modbus.data[0] & 0xff)));

			zlog_debug(Util::m_zlog,"向以太网发送数据");
			m_socket.Send(data);
		}
		else
		{
			zlog_warn(Util::m_zlog,"设置5号命令失败");
			PackErrorMessage(modbus);
		}
		break;
	case 6:
		tmp = modbus.data[0];
		temp = tmp;
		zlog_warn(Util::m_zlog,"6号命令设置单个寄存器%d=%d",modbus.regStart,temp);

		if (MemDb::SetRealDataUnknown(modbus.regStart,temp,true))
		{
			//将返回数据进行封装
			zlog_debug(Util::m_zlog,"将返回数据进行封装");
			data.push_back((UCHAR) ((modbus.firstFlag >> 8) & 0xff));
			data.push_back((UCHAR) (modbus.firstFlag & 0xff));
			data.push_back((UCHAR) ((modbus.secondFlag >> 8) & 0xff));
			data.push_back((UCHAR) (modbus.secondFlag & 0xff));
			data.push_back((UCHAR) 0);
			data.push_back(6);
			data.push_back(modbus.addr);
			data.push_back(modbus.order);
			data.push_back((UCHAR) ((modbus.regStart >> 8) & 0xff));
			data.push_back((UCHAR) (modbus.regStart & 0xff));
			data.push_back(((modbus.data[0] >> 8) & 0xff));
			data.push_back((UCHAR) (modbus.data[0] & 0xff));

			zlog_debug(Util::m_zlog,"向以太网发送数据");
			m_socket.Send(data);
		}
		else
		{
			zlog_warn(Util::m_zlog,"设置6号命令失败");
			PackErrorMessage(modbus);
		}
		break;
	case 15:
		zlog_debug(Util::m_zlog,"15号命令设置多个开关量");

		for (int i = 0; i < modbus.regLength; i++)
		{
			if (!MemDb::SetRealDataUnknown(modbus.regStart + i, modbus.data[i],true))
			{
				zlog_warn(Util::m_zlog,"设置15号命令失败");
				PackErrorMessage(modbus);
				break;
			}
		}

		//将返回数据进行封装
		zlog_debug(Util::m_zlog,"将返回数据进行封装");
		data.push_back((UCHAR) ((modbus.firstFlag >> 8) & 0xff));
		data.push_back((UCHAR) (modbus.firstFlag & 0xff));
		data.push_back((UCHAR) ((modbus.secondFlag >> 8) & 0xff));
		data.push_back((UCHAR) (modbus.secondFlag & 0xff));
		data.push_back((UCHAR) 0);
		data.push_back(6);
		data.push_back(modbus.addr);
		data.push_back(modbus.order);
		data.push_back((UCHAR) ((modbus.regStart >> 8) & 0xff));
		data.push_back((UCHAR) (modbus.regStart & 0xff));
		data.push_back((UCHAR) ((modbus.regLength >> 8) & 0xff));
		data.push_back((UCHAR) (modbus.regLength & 0xff));

		zlog_debug(Util::m_zlog,"向以太网发送数据");
		m_socket.Send(data);
		break;
	case 16:
		zlog_debug(Util::m_zlog,"16号命令设置单个开关量");

		for (int i = 0; i < modbus.regLength; i++)
		{
			if (!MemDb::SetRealDataUnknown(modbus.regStart + i, modbus.data[i],true))
			{
				zlog_warn(Util::m_zlog,"设置16号命令失败");
				PackErrorMessage(modbus);
				break;
			}
		}

		//将返回数据进行封装
		zlog_debug(Util::m_zlog,"将返回数据进行封装");
		data.push_back((UCHAR) ((modbus.firstFlag >> 8) & 0xff));
		data.push_back((UCHAR) (modbus.firstFlag & 0xff));
		data.push_back((UCHAR) ((modbus.secondFlag >> 8) & 0xff));
		data.push_back((UCHAR) (modbus.secondFlag & 0xff));
		data.push_back((UCHAR) 0);
		data.push_back(6);
		data.push_back(modbus.addr);
		data.push_back(modbus.order);
		data.push_back((UCHAR) ((modbus.regStart >> 8) & 0xff));
		data.push_back((UCHAR) (modbus.regStart & 0xff));
		data.push_back((UCHAR) ((modbus.regLength >> 8) & 0xff));
		data.push_back((UCHAR) (modbus.regLength & 0xff));

		zlog_debug(Util::m_zlog,"向以太网发送数据");
		m_socket.Send(data);
		break;
	default:
		break;
	}

	zlog_debug(Util::m_zlog,"向以太网发送数据完毕");
	return ErrorInfo::ERR_OK;
}

//将需要错误信息数据进行封装,保存data中
int ModbusTcpServer::PackErrorMessage(ModbusTcp& modbus)
{
	String data;
	zlog_debug(Util::m_zlog,"返回错误信息");
	data.clear();

	data.push_back((UCHAR) ((modbus.firstFlag >> 8) & 0xff));
	data.push_back((UCHAR) (modbus.firstFlag & 0xff));
	data.push_back((UCHAR) ((modbus.secondFlag >> 8) & 0xff));
	data.push_back((UCHAR) (modbus.secondFlag & 0xff));
	data.push_back((UCHAR) 0);
	data.push_back(3);
	data.push_back(modbus.addr);
	data.push_back(0x80 + modbus.order);
	data.push_back(0x01);

	zlog_debug(Util::m_zlog,"向以太网发送错误信息");
	m_socket.Send(data);

	zlog_debug(Util::m_zlog,"错误信息返回完毕");
	return ErrorInfo::ERR_OK;
}

//对modbus命令进行校验,由UnpackModbus调用
int ModbusTcpServer::Check(String& data)
{
	zlog_debug(Util::m_zlog,"检查接收的数据");
	int ret = ErrorInfo::ERR_CHECK;
	int length;
	int start;

	if (data.size() >= 6)
	{
		if (data.size() >= (6 + (UCHAR) data[5]))
		{
			length = (((UCHAR) (data[10]) << 8) | (UCHAR) data[11]);//请求数据的数量
			start = (((UCHAR) (data[8]) << 8) | (UCHAR) data[9]);//请求的起始地址

			//if ((UCHAR) data[6] == m_modbusCheck->addr)
			{
				zlog_debug(Util::m_zlog,"校验:%d号命令",data[7]);
				switch ((UCHAR) data[7])
				{
				case 1:
				case 2:
				case 3:
				case 4:
				case 15:
				case 16:
					if ((start >= m_modbusCheck->regStart) && ((start + length)
							<= m_modbusCheck->regEnd))
					{
						zlog_debug(Util::m_zlog,"1、2、3、4、15、16号命令校验成功");
						ret = ErrorInfo::ERR_OK;
					}
					else
					{
						zlog_warn(Util::m_zlog,"1、2、3、4、15、16号命令校验失败，地址越界");
						ret = ErrorInfo::ERR_CHECK;
					}
					break;
				case 5:
				case 6:
					if ((start >= m_modbusCheck->regStart) && (start
							<= m_modbusCheck->regEnd))
					{
						zlog_debug(Util::m_zlog,"5、6号命令校验成功");
						ret = ErrorInfo::ERR_OK;
					}
					else
					{
						zlog_warn(Util::m_zlog,"5、6号命令校验失败，地址越界");
						ret = ErrorInfo::ERR_CHECK;
					}
					break;
				default:
					break;
				}
			}
		}
	}

	zlog_debug(Util::m_zlog,"校验结束");
	return ret;
}

