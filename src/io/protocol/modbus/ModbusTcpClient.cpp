/*
 * ModbusTcpClient.cpp
 *
 *   创建日期: 2013-4-11
 *   作         者: mzh
 */

#include "ModbusTcpClient.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../../util/Check.h"
//#include "libs_emfuture_odm.h"

ModbusTcpClient::ModbusTcpClient()
{
	// TODO 构造函数
	m_tcpClient = NULL;
	m_retries = 3;

	m_delay = 50;
}

ModbusTcpClient::~ModbusTcpClient()
{
	// TODO 析构函数
	m_tcpClient = NULL;
	m_retries = 0;
}

//设置/返回网口指针
void ModbusTcpClient::SetTcpClient(TcpClient* tcpClient)
{
	assert(tcpClient != NULL);
	m_tcpClient = tcpClient;
}

TcpClient* ModbusTcpClient::GetTcpClient()
{
	return m_tcpClient;
}

//设置返回查询失败的重复查询次数
void ModbusTcpClient::SetRetries(int retries)
{
	if (retries <= 0)
	{
		return;
	}
	m_retries = retries;
}

int ModbusTcpClient::GetRetries()
{
	return m_retries;
}

//设置查询延时
void ModbusTcpClient::SetDelay(int delay)
{
	if (delay <= 0)
	{
		return;
	}
	m_delay = delay;
}

//获取查询延时
int ModbusTcpClient::GetDelay()
{
	return m_delay;
}

//1号命令，读输出状态量（开关量）
int ModbusTcpClient::ReadOutputStatus(UshortArray& values, int addr, int start, int length)
{
	//1、局部变量
	String queryCmdData;//查询命令数据
	//应答长度
	short replyLength = (9 + (7 + length) / 8);
	String replyCmdData;
	int error;
	short coil;
	short bit;
	short rspByte;
	const short QUERY_LENGTH = 12;//查询消息长度
	int i = 0;

	//2、组装请求命令
	//设置查询命令长度
	//标示符6字节+设备地址1字节+功能码1字节+寄存器地址2字节+寄存器长度2字节=12字节
	queryCmdData.resize(QUERY_LENGTH);
	//设置应答长度
	replyCmdData.resize(replyLength);
	values.resize(length);

	//modbus信息
	for (i = 0; i < 5; i++)
	{
		queryCmdData[i] = (UCHAR) 0;
	}

	queryCmdData[5] = 6;//后面字节长度为6
	queryCmdData[6] = (UCHAR) addr;//设备地址
	queryCmdData[7] = 1;//功能码
	queryCmdData[8] = (UCHAR) ((start >> 8) & 0xff);//寄存器起始高字节
	queryCmdData[9] = (UCHAR) (start & 0xff);//寄存器起始低字节
	queryCmdData[10] = (UCHAR) ((length >> 8) & 0xff);//寄存器长度高字节
	queryCmdData[11] = (UCHAR) (length & 0xff);//寄存器长度低字节

	//3、发送数据
	//查询命令
	zlog_info(Util::m_zlog,"查询数据");

	error = m_tcpClient->TxRxMsg(replyCmdData, queryCmdData, replyLength);

	//查询无错误
	if (error == ErrorInfo::ERR_OK)
	{
		//判断应答地址和命令是否与查询一致
		if ((replyCmdData[6] != queryCmdData[6]) || (replyCmdData[7] != queryCmdData[7]))
		{

			zlog_warn(Util::m_zlog,"应答地址和命令与查询不一致");

			error = ErrorInfo::ERR_INV_RESP;
		}
		else
		{
			//无错误，返回查询数组
			zlog_warn(Util::m_zlog,"无错误，返回查询数组");

			rspByte = 9;
			bit = 0;
			for (coil = 0; coil < length; coil++)
			{
				if (bit > 7)
				{
					bit = 0;
					rspByte++;
				}

				if ((replyCmdData[rspByte] & (0x01 << bit)) > 0)
				{
					values[coil] = 1;
				}
				else
				{
					values[coil] = 0;
				}
				bit++;
			}//end for
		}//else
	}//end if(error=ErrorInfo::ERR_OK)

	zlog_info(Util::m_zlog,"查询数据结束");

	return error;
}

//2号命令，读输入状态量（开关量）
int ModbusTcpClient::ReadInputStatus(UshortArray& values, int addr, int start, int length)
{
	//1、局部变量
	String queryCmdData;//查询命令数据
	//应答长度
	short replyLength = (9 + (7 + length) / 8);
	String replyCmdData;
	int error;
	short coil;
	short bit;
	short rspByte;
	const short QUERY_LENGTH = 12;//查询消息长度
	int i = 0;

	//2、组装请求命令
	//设置查询命令长度
	//标示符6字节+设备地址1字节+功能码1字节+寄存器地址2字节+寄存器长度2字节=12字节
	queryCmdData.resize(QUERY_LENGTH);
	//设置应答长度
	replyCmdData.resize(replyLength);
	values.resize(length);

	//modbus信息
	for (i = 0; i < 5; i++)
	{
		queryCmdData[i] = (UCHAR) 0;
	}

	queryCmdData[5] = 6;//后面字节长度为6
	queryCmdData[6] = (UCHAR) addr;//设备地址
	queryCmdData[7] = 2;//功能码
	queryCmdData[8] = (UCHAR) ((start >> 8) & 0xff);//寄存器起始高字节
	queryCmdData[9] = (UCHAR) (start & 0xff);//寄存器起始低字节
	queryCmdData[10] = (UCHAR) ((length >> 8) & 0xff);//寄存器长度高字节
	queryCmdData[11] = (UCHAR) (length & 0xff);//寄存器长度低字节

	//3、发送数据
	//查询命令
	zlog_info(Util::m_zlog,"查询数据");

	error = m_tcpClient->TxRxMsg(replyCmdData, queryCmdData, replyLength);

	//查询无错误
	if (error == ErrorInfo::ERR_OK)
	{
		//判断应答地址和命令是否与查询一致
		if ((replyCmdData[6] != queryCmdData[6]) || (replyCmdData[7] != queryCmdData[7]))
		{

			zlog_warn(Util::m_zlog,"应答地址和命令与查询不一致");

			error = ErrorInfo::ERR_INV_RESP;
		}
		else
		{
			//无错误，返回查询数组
			zlog_warn(Util::m_zlog,"无错误，返回查询数组");

			rspByte = 9;
			bit = 0;
			for (coil = 0; coil < length; coil++)
			{
				if (bit > 7)
				{
					bit = 0;
					rspByte++;
				}

				if ((replyCmdData[rspByte] & (0x01 << bit)) > 0)
				{
					values[coil] = 1;
				}
				else
				{
					values[coil] = 0;
				}
				bit++;
			}//end for
		}//else
	}//end if(error=ErrorInfo::ERR_OK)

	zlog_info(Util::m_zlog,"查询数据结束");

	return error;
}

// 3号命令,读取输出寄存器值
int ModbusTcpClient::ReadOutputRegisters(UshortArray& values, int addr, int start, int length)
{
	zlog_debug(Util::m_zlog,"读取输出寄存器值");
	// printf("读取\n");
	//1、局部变量
	String queryCmdData;//查询命令数据
	//应答长度，9+2*读取寄存器数量
	short replyLength = (9 + 2 * length );
	String replyCmdData;
	int error;
	short byte;
	short rspByte;
	const short QUERY_LENGTH = 12;//查询消息长度
	int i = 0;

	//2、组装请求命令
	//设置查询命令长度
	//标示符6字节+设备地址1字节+功能码1字节+寄存器地址2字节+寄存器长度2字节=12字节
	queryCmdData.resize(QUERY_LENGTH);
	//设置应答长度
	replyCmdData.resize(replyLength);
	values.resize(length);

	//modbus信息
	for (i = 0; i < 5; i++)
	{
		queryCmdData[i] = (UCHAR) 0;
	}

	queryCmdData[5] = 6;//后面字节长度为6
	queryCmdData[6] = (UCHAR) addr;//设备地址
	queryCmdData[7] = 3;//功能码
	queryCmdData[8] = (UCHAR) ((start >> 8) & 0xff);//寄存器起始高字节
	queryCmdData[9] = (UCHAR) (start & 0xff);//寄存器起始低字节
	queryCmdData[10] = (UCHAR) ((length >> 8) & 0xff);//寄存器长度高字节
	queryCmdData[11] = (UCHAR) (length & 0xff);//寄存器长度低字节

	//3、发送数据
	//查询命令

#if 0
		printf("=======%d下发数据：",m_tcpClient->Getsockname().GetPort());
		for(int i = 0; i < queryCmdData.size(); i++)
		{
			printf("%02x ", queryCmdData[i]);
		}
		printf("\n");
#endif
		error = m_tcpClient->TxRxMsg(replyCmdData, queryCmdData, replyLength);
#if 0
		printf("=======%d上传数据：",m_tcpClient->Getsockname().GetPort());
		for(int i = 0; i < replyCmdData.size(); i++)
		{
			printf("%02x ", replyCmdData[i]);
		}
		printf("\n");
#endif



	zlog_debug(Util::m_zlog,"查询数据");
	error = m_tcpClient->TxRxMsg(replyCmdData, queryCmdData, replyLength);


#if 0
		printf("=======%d下发数据：",m_tcpClient->Getsockname().GetPort());
		for(int i = 0; i < queryCmdData.size(); i++)
		{
			printf("%02x ", queryCmdData[i]);
		}
		printf("\n");
#endif
		error = m_tcpClient->TxRxMsg(replyCmdData, queryCmdData, replyLength);
#if 0
		printf("=======%d上传数据：",m_tcpClient->Getsockname().GetPort());
		for(int i = 0; i < replyCmdData.size(); i++)
		{
			printf("%02x ", replyCmdData[i]);
		}
		printf("\n");
#endif



	//查询无错误
	if (error == ErrorInfo::ERR_OK)
	{
		//判断应答地址和命令是否与查询一致
		if ((replyCmdData[6] != queryCmdData[6]) || (replyCmdData[7] != queryCmdData[7]))
		{
			zlog_warn(Util::m_zlog,"应答地址和命令与查询不一致");

			error = ErrorInfo::ERR_INV_RESP;
		}
		else
		{
			//无错误，返回查询数组
			zlog_debug(Util::m_zlog,"无错误，返回查询数组");
			byte = 0;
			for (rspByte = 9; rspByte < replyLength; rspByte = (rspByte + 2))
			{
				values[byte] = (replyCmdData[rspByte]);
				values[byte] = ((values[byte] << 8) | (replyCmdData[rspByte + 1]));
				byte++;
			}
		}
	}//end if(error=ErrorInfo::ERR_OK)

	zlog_debug(Util::m_zlog,"查询数据结束");

	return error;
}

// 3号命令,读取输出寄存器值
int ModbusTcpClient::ReadRs485OutputRegisters(UshortArray& values, int addr, int start, int length)
{
	zlog_info(Util::m_zlog,
	            "执行 Modbus RTU协议 ModbusRtuMaster::ReadOutputRegisters： 命令=3, 从站地址= %d, 起始地址=%d, 其寄存器长度= %d.",
	            addr, start, length);

	//局部变量
	String query;//查询命令数据
	String reply;////应答字符串
	//应答长度，设备地址+命令号+字节长度+2×读取寄存器数量 + crc校验
	int replyLength=(5 + 2 * length);

	reply.resize(replyLength+1);
	query.clear();
	int error;
	int byte;
	int respByte;
	unsigned short crc16;

	//modbus message
	query.push_back((char)addr);
	query.push_back((char)3);//功能码
	query.push_back((char)(start >> 8)&0xFF);//寄存器起始高字节
	query.push_back((char)(start & 0xFF));//寄存器起始低字节
	query.push_back((char)(length >> 8)&0xFF);//寄存器长度高字节
	query.push_back((char)(length & 0xFF));//寄存器长度地字节

	crc16 = Check::Crc16(query);
	query.push_back((char)(crc16 & 0xFF));
	query.push_back((char)(crc16 >> 8)&0xFF);

#if 0
		printf("=======%d下发数据：",m_tcpClient->Getsockname().GetPort());
		for(int i = 0; i < query.size(); i++)
		{
			printf("%02x ", query[i]);
		}
		printf("\n");
#endif
		error = m_tcpClient->TxRxMsg(reply, query, replyLength);
#if 0
		printf("=======%d上传数据：",m_tcpClient->Getsockname().GetPort());
		for(int i = 0; i < reply.size(); i++)
		{
			printf("%02x ", reply[i]);
		}
		printf("\n");
#endif
	values.clear();
	values.resize(length);
	//查询无错误
	if (error == ErrorInfo::ERR_OK)
	{
//		//判断应答地址和命令是否与查询一致
//		if(Check::Crc16(reply))
//		{
//			error = ErrorInfo::ERR_CHECK;
//			zlog_info(Util::m_zlog, "应答错误：%d 校验错误" ,error);
//		}
		if ((reply[0] != query[0]) || (reply[1] != query[1]))
		{
			error = ErrorInfo::ERR_INV_RESP;//应答错误
			zlog_info(Util::m_zlog, "应答错误：%d 无效应答" ,error);
		}
		else
		{
			//无错误，返回查询数组
			byte = 0;
			for (respByte = 3; respByte < (replyLength-2); respByte += 2)
			{
				values[byte] = (reply[respByte]);
				values[byte] = ((values[byte] << 8) | (reply[respByte + 1]));

				//zlog_info(Util::m_zlog,"%d %02X%02X %02X , ",byte,reply[respByte],reply[respByte + 1],values[byte]);

				byte++;
			}

		}
	}//end of if (error==ErrorInfo::ERR_OK)

	zlog_info(Util::m_zlog, "执行 Modbus RTU协议  ModbusRtuMaster::ReadOutputRegisters： 命令=3;错误码= %d" ,error);

	return(error);

}

//读输出寄存器，3号命令
int ModbusTcpClient::ReadRLCFloatRegisters(FloatArray& values, int addr, int start, int length)
{
	zlog_info(Util::m_zlog,
	            "执行 Modbus RTU协议 ModbusRtuMaster::ReadOutputRegisters： 命令=3, 从站地址= %d, 起始地址=%d, 其寄存器长度= %d.",
	            addr, start, length);

	//局部变量
	String query;//查询命令数据
	String reply;////应答字符串
	//应答长度，设备地址+命令号+字节长度+2×读取寄存器数量 + crc校验
	int replyLength=(5 + 2 * length);

	reply.resize(replyLength+1);

	int error;
	int byte;
	int respByte;
	int tmp = 0;
	float temp = 0.0;
	unsigned short crc16;

	//modbus message
	query.push_back((char)addr);
	query.push_back((char)3);//功能码
	query.push_back((char)(start >> 8)&0xFF);//寄存器起始高字节
	query.push_back((char)(start & 0xFF));//寄存器起始低字节
	query.push_back((char)(length >> 8)&0xFF);//寄存器长度高字节
	query.push_back((char)(length & 0xFF));//寄存器长度地字节

	crc16 = Check::Crc16(query);
	query.push_back((char)(crc16 & 0xFF));
	query.push_back((char)(crc16 >> 8)&0xFF);

	//查询命令
#if 0
		printf("=======%d下发数据：",m_tcpClient->Getsockname().GetPort());
		for(int i = 0; i < query.size(); i++)
		{
			printf("%02x ", query[i]);
		}
		printf("\n");
#endif
		error = m_tcpClient->TxRxMsg(reply, query, replyLength);
#if 0
		printf("=======%d上传数据：",m_tcpClient->Getsockname().GetPort());
		for(int i = 0; i < reply.size(); i++)
		{
			printf("%02x ", reply[i]);
		}
		printf("\n");
#endif
	values.clear();
	values.resize(length/2);
	//查询无错误
	if (error == ErrorInfo::ERR_OK)
	{
		if ((reply[0] != query[0]) || (reply[1] != query[1]))
		{
			error = ErrorInfo::ERR_INV_RESP;//应答错误
			zlog_info(Util::m_zlog, "应答错误：%d 无效应答" ,error);
		}
		else
		{
			//无错误，返回查询数组
			byte = 0;
			for (respByte = 3; respByte < (replyLength - 2); respByte += 4)
			{
				tmp = (reply[respByte + 2] << 24) + (reply[respByte + 3] << 16)
						+ (reply[respByte] << 8) + (reply[respByte + 1]);
				memcpy(&temp, &tmp, sizeof(int));
				values[byte] = temp;
				byte++;
			}

		}
	}

	zlog_info(Util::m_zlog, "执行 Modbus RTU协议  ModbusRtuMaster::ReadOutputRegisters： 命令=3;错误码= %d" ,error);
	return(error);
}//end of int ModbusRtuMaster::ReadOutputRegisters(UshortArray& values, int addr, int start, int length)

//4号命令,读取输入寄存器值
int ModbusTcpClient::ReadInputRegisters(UshortArray& values, int addr, int start, int length)
{
	//1、局部变量
	String queryCmdData;//查询命令数据
	//应答长度，9+2*读取寄存器数量
	short replyLength = (9 + 2 * length );
	String replyCmdData;
	int error;
	short byte;
	short rspByte;
	const short QUERY_LENGTH = 12;//查询消息长度
	int i = 0;

	//2、组装请求命令
	//设置查询命令长度
	//标示符6字节+设备地址1字节+功能码1字节+寄存器地址2字节+寄存器长度2字节=12字节
	queryCmdData.resize(QUERY_LENGTH);
	//设置应答长度
	replyCmdData.resize(replyLength);
	values.resize(length);

	//modbus信息
	for (i = 0; i < 5; i++)
	{
		queryCmdData[i] = (UCHAR) 0;
	}

	queryCmdData[5] = 6;//后面字节长度为6
	queryCmdData[6] = (UCHAR) addr;//设备地址
	queryCmdData[7] = 4;//功能码
	queryCmdData[8] = (UCHAR) ((start >> 8) & 0xff);	//寄存器起始高字节
	queryCmdData[9] = (UCHAR) (start & 0xff);			//寄存器起始低字节
	queryCmdData[10] = (UCHAR) ((length >> 8) & 0xff);	//寄存器长度高字节
	queryCmdData[11] = (UCHAR) (length & 0xff);			//寄存器长度低字节

	//3、发送数据
	//查询命令
	zlog_info(Util::m_zlog,"查询数据");

	error = m_tcpClient->TxRxMsg(replyCmdData, queryCmdData, replyLength);

	//查询无错误
	if (error == ErrorInfo::ERR_OK)
	{
		//判断应答地址和命令是否与查询一致
		if ((replyCmdData[6] != queryCmdData[6]) || (replyCmdData[7] != queryCmdData[7]))
		{
			zlog_info(Util::m_zlog,"应答地址和命令与查询不一致");
			error = ErrorInfo::ERR_INV_RESP;
		}
		else
		{
			//无错误，返回查询数组
			// zlog_warn(Util::m_zlog,"无错误，返回查询数组");

			byte = 0;
			for (rspByte = 9; rspByte < replyLength; rspByte = (short) (rspByte + 2))
			{
				values[byte] = (replyCmdData[rspByte]);
				values[byte] = ((values[byte] << 8) | (replyCmdData[rspByte + 1]));
				byte++;
			}
		}
	}//end if(error=ErrorInfo::ERR_OK)

	zlog_info(Util::m_zlog,"查询数据结束");

	return error;
}

//4号命令,读取输入寄存器值
int ModbusTcpClient::ReadRS485InputRegisters(UshortArray& values, int addr, int start, int length)
{
	//1、局部变量
	String query;//查询字符串
	String reply;//应答字符串s
	int replyLength = (5 + 2 * length);
	int error;
	int byte;
	int respByte;
	unsigned short crc16;

	values.clear();
	values.resize(length);

	//modbus 查询消息
	query.clear();
	query.push_back((char)addr);//设备地址
	query.push_back((char)4);//功能码
	query.push_back((char)(start >> 8)&0xFF);//寄存器起始高字节
	query.push_back((char)(start & 0xFF));//寄存器起始低字节
	query.push_back((char)(length >> 8));//寄存器长度高字节
	query.push_back((char)(length & 0xFF)&0xFF);//寄存器长度地字节

	crc16 = Check::Crc16(query);
	query.push_back((char)(crc16 & 0xFF));
	query.push_back((char)(crc16 >> 8)&0xFF);

	error = m_tcpClient->TxRxMsg(reply, query, replyLength);//读取查询消息
	//查询无错误
	if (error == ErrorInfo::ERR_OK)
	{
		if ((reply[0] != query[0]) || (reply[1] != query[1]))
		{
			error = ErrorInfo::ERR_INV_RESP;
			zlog_info(Util::m_zlog, "应答错误：%d 无效应答" ,error);
		}
		else
		{
			byte = 0;
			for (respByte = 3; respByte < (replyLength-2); respByte += 2)
			{
				values[byte] = (reply[respByte]);
				values[byte] = ((values[byte] << 8) | (reply[respByte + 1]));
				byte++;
			}

		}//end of if ((reply[0]!=query[0])||(reply[1]!=query[1]))

	}//end of if (error==ErrorInfo::ERR_OK)

	zlog_info(Util::m_zlog, "执行 Modbus RTU协议  ModbusRtuMaster::ReadInputRegisters： 命令=4;错误码= %d" ,error);

	return (error);
}

//5号命令,设置单个开关量
int ModbusTcpClient::ForceSingleCoil(int addr, int regAddr, bool value)
{
	//1、局部变量
	const short QUERY_LENGTH = 12;//查询长度
	String queryCmdData;//查询数据
	short replyLength = 12;//应答长度
	String replyCmdData;//应答数据
	short error;
	int reg;
	int retry = 0;
	int i;
	//2、组装设置命令
	queryCmdData.resize(QUERY_LENGTH);
	replyCmdData.resize(replyLength);

	//modbus消息
	for (i = 0; i < 5; i++)
	{
		queryCmdData[i] = (UCHAR)0;
	}

	queryCmdData[5] = 6; //后面字节长度为6
	queryCmdData[6] = (UCHAR)addr;
	queryCmdData[7] = 5;
	queryCmdData[8] = (UCHAR)((regAddr >> 8) & 0xff);
	queryCmdData[9] = (UCHAR)(regAddr & 0xff);
	queryCmdData[10] = (UCHAR)((ModbusTcpClient::ModBool(value) >> 8) & 0xff);
	queryCmdData[11] = (UCHAR)(ModbusTcpClient::ModBool(value) & 0xff);

	//进行发送数据操作
	do
	{
		zlog_info(Util::m_zlog,"进行发送数据操作");

		//读取应答命令
		error = m_tcpClient->TxRxMsg(replyCmdData, queryCmdData, replyLength);

		//判读应答数据是否正确
		zlog_info(Util::m_zlog,"判断应答数据是否正确");
		if (error == ErrorInfo::ERR_OK)
		{
			for (reg = 0; reg < QUERY_LENGTH; reg++)
			{
				if (replyCmdData[reg] != queryCmdData[reg])
				{
					zlog_warn(Util::m_zlog,"应答数据错误");
					error = ErrorInfo::ERR_INV_RESP;
					break;
				}
			}
		}//end of if(error == ErrorInfo::ERR_OK)

		if(error != ErrorInfo::ERR_OK)
		{
			retry++;

			zlog_warn(Util::m_zlog,"查询数据结束");
			msleep(m_delay);
		}

	}while((retry < m_retries) && (ErrorInfo::ERR_OK != error));

	return error;
}

//5号命令,设置单个开关量
int ModbusTcpClient::ForceRS485SingleCoil(int addr, int regAddr, bool value)
{
	zlog_info(Util::m_zlog,
	            "执行 Modbus RTU协议 ModbusRtuMaster::ForceSingleCoil： 命令=5, 从站地址= %d, 起始地址=%d.",
	            addr, regAddr);

	String query;//查询字符串
	String reply;//应答字符串
	const int QUERY_LENGHT = 8;
	int replyLength = 8;
	int error;
	int	reg;
	int retry=0;
	unsigned short crc16;

	//modbus 查询消息
	query.clear();
	query.push_back((char)addr);//设备地址
	query.push_back((char)5);//功能码
	query.push_back((char)(regAddr >> 8)&0xFF);//寄存器起始高字节
	query.push_back((char)(regAddr & 0xFF));//寄存器起始低字节
	if(value)
	{
		query.push_back((char)(0xFF00 >> 8)&0xFF);//开关量高字节
		query.push_back((char)(0xFF00 & 0xFF));//开关量低字节
	}
	else
	{
		query.push_back((char)(0x0000 >> 8)&0xFF);//开关量高字节
		query.push_back((char)(0x0000 & 0xFF));//开关量低字节
	}

	crc16 = Check::Crc16(query);
	query.push_back((char)(crc16 & 0xFF));
	query.push_back((char)(crc16 >> 8)&0xFF);

	do
	{
#if 0
		printf("=======%d下发数据：",m_tcpClient->Getsockname().GetPort());
		for(int i = 0; i < query.size(); i++)
		{
			printf("%02x ", query[i]);
		}
		printf("\n");
#endif
		error = m_tcpClient->TxRxMsg(reply, query, replyLength);
#if 0
		printf("=======%d上传数据：",m_tcpClient->Getsockname().GetPort());
		for(int i = 0; i < reply.size(); i++)
		{
			printf("%02x ", reply[i]);
		}
		printf("\n");
#endif
		if (error == ErrorInfo::ERR_OK)
		{
			for (reg = 0; reg < QUERY_LENGHT; reg++)
			{

				if (reply[reg] != query[reg])
				{
					error = ErrorInfo::ERR_INV_RESP;
					zlog_info(Util::m_zlog, "应答错误：%d 无效应答" ,error);
					break;
				}
			}//end of for (reg=0; reg < QUERY_LENGHT; reg++)

		}//end of if (error==ErrorInfo::ERR_OK)

		if (ErrorInfo::ERR_OK != error)
		{
			usleep(m_delay * 1000);
		}
		retry++;

	} while ((retry < m_retries)&&(ErrorInfo::ERR_OK != error));

	zlog_info(Util::m_zlog, "执行 Modbus RTU协议  ModbusRtuMaster::ForceSingleCoil： 命令=5;错误码= %d, 查询次数=%d~~~~~~" ,error,retry);

	return(error);

}//end ForceSingleCoil

//6号命令,设置单个寄存器
int ModbusTcpClient::PresetSingleRegister(int addr, int regAddr, unsigned short value)
{
	//1、局部变量
	const short QUERY_LENGTH = 12;//查询长度
	String queryCmdData;//查询数据
	short replyLength = 12;//应答长度
	String replyCmdData;//应答数据
	short error;
	int reg;
	int retry = 0;
	int i = 0;

	//2、组装设置命令
	queryCmdData.resize(QUERY_LENGTH);
	replyCmdData.resize(replyLength);

	//modbus消息
	for (i = 0; i < 5; i++)
	{
		queryCmdData[i] = (UCHAR)0;
	}

	queryCmdData[5] = 6; //后面字节长度为6
	queryCmdData[6] = (UCHAR)addr;
	queryCmdData[7] = 6;
	queryCmdData[8] = (UCHAR)((regAddr >> 8) & 0xff);
	queryCmdData[9] = (UCHAR)(regAddr & 0xff);
	queryCmdData[10] = (UCHAR)((value >> 8) & 0xff);
	queryCmdData[11] = (UCHAR)(value & 0xff);

	//进行发送数据操作
	do
	{
		//zlog_warn(Util::m_zlog,"进行发送数据操作");

		//读取应答命令
		error = m_tcpClient->TxRxMsg(replyCmdData, queryCmdData, replyLength);
		//判读应答数据是否正确
		//zlog_warn(Util::m_zlog,"判断应答数据是否正确");
		if (error == ErrorInfo::ERR_OK)
		{
			for (reg = 0; reg < QUERY_LENGTH; reg++)
			{
				if (replyCmdData[reg] != queryCmdData[reg])
				{
					zlog_warn(Util::m_zlog,"应答数据错误");
					error = ErrorInfo::ERR_INV_RESP;
					break;
				}
			}
		}//end of if(error == ErrorInfo::ERR_OK)

		if(error != ErrorInfo::ERR_OK)
		{
			retry++;

			//zlog_warn(Util::m_zlog,"查询数据结束");
			msleep(m_delay);
		}

	}while((retry < m_retries) && (ErrorInfo::ERR_OK != error));

	return error;
}

//6号命令,设置单个寄存器
int ModbusTcpClient::PresetRS485SingleRegister(int addr, int regAddr, unsigned short value)
{
	zlog_info(Util::m_zlog,
	            "执行 Modbus RTU协议 ModbusRtuMaster::PresetSingleRegister： 命令=6, 从站地址= %d, 起始地址=%d.",
	            addr, regAddr);

	String query;//查询字符串
	String reply;//应答字符串
	const int QUERY_LENGHT = 8;
	int replyLength = 8;

	int error;
	int	reg;
	int retry = 0;
	unsigned short crc16;

	//modbus message
	query.clear();
	query.push_back((char)addr);//设备地址
	query.push_back((char)6);//功能码
	query.push_back((char)(regAddr >> 8)&0xFF);//寄存器起始高字节
	query.push_back((char)(regAddr & 0xFF));//寄存器起始低字节
	query.push_back((char)(value >> 8)&0xFF);//写入寄存器的值的高字节
	query.push_back((char)(value & 0xFF));//写入寄存器的值的低字节

	crc16 = Check::Crc16(query);
	query.push_back((char)(crc16 & 0xFF));
	query.push_back((char)(crc16 >> 8)&0xFF);

	do
	{
#if 0
		printf("=======%d下发数据：",m_tcpClient->Getsockname().GetPort());
		for(int i = 0; i < query.size(); i++)
		{
			printf("%02x ", query[i]);
		}
		printf("\n");
#endif
		error = m_tcpClient->TxRxMsg(reply, query, replyLength);
#if 0
		printf("=======%d上传数据：",m_tcpClient->Getsockname().GetPort());
		for(int i = 0; i < reply.size(); i++)
		{
			printf("%02x ", reply[i]);
		}
		printf("\n");
#endif
		if (error==ErrorInfo::ERR_OK)
		{
			for (reg=0; reg < QUERY_LENGHT; reg++)
			{
				if (reply[reg]!=query[reg])
				{
					error = ErrorInfo::ERR_INV_RESP;
					zlog_warn(Util::m_zlog, "应答错误：%d 无效应答" ,error);
					break;
				}
			}//end of for (reg=0; reg < QUERY_LENGHT; reg++)

		}//end of if (error==ErrorInfo::ERR_OK)

		if(ErrorInfo::ERR_OK != error)
		{
			usleep(m_delay*1000);
		}
		retry++;

	} while ((retry<m_retries)&&(ErrorInfo::ERR_OK != error));

	zlog_info(Util::m_zlog, "执行 Modbus RTU协议  ModbusRtuMaster::PresetSingleRegister： 命令=6;错误码= %d, 查询次数=%d~~~~~~" ,error,retry);

	return(error);

}//end PresetSingleRegister

//15号命令,设置多个开关量
int ModbusTcpClient::ForceMultipleCoils(int addr, int start, int length, UshortArray& values)
{
	//1、局部变量
	const short QUERY_LENGTH = (13 + (length + 7) / 8);//查询长度
	String queryCmdData;//查询数据
	short replyLength = 12;//应答长度
	String replyCmdData;//应答数据
	short error;
	short queryByte;
	int coil;
	int bit;
	int reg;
	int retry = 0;
	int i;

	//2、组装设置命令
	queryCmdData.resize(QUERY_LENGTH);
	replyCmdData.resize(replyLength);

	//modbus消息
	for (i = 0; i < 5; i++)
	{
		queryCmdData[i] = (UCHAR)0;
	}

	queryCmdData[5] = (length + 7) / 8+7;
	queryCmdData[6] = (UCHAR)addr;
	queryCmdData[7] = 15;
	queryCmdData[8] = (UCHAR)((start >> 8) & 0xff);
	queryCmdData[9] = (UCHAR)(start & 0xff);
	queryCmdData[10] = (UCHAR)((length >> 8) & 0xff);
	queryCmdData[11] = (UCHAR)(length & 0xff);
	queryCmdData[12] = (UCHAR)((length + 7) / 8);
	queryByte = 13;
	bit = 0 ;

	for (coil = 0; coil < length; coil++)
	{
		if (bit > 7)
		{
			bit = 0;
			queryByte++;
		}

		if (values[coil])
		{
			queryCmdData[queryByte] = (queryCmdData[queryByte] | (0x01 << bit));
		}
		bit++;
	}

	//进行发送数据操作
	do
	{
		zlog_info(Util::m_zlog,"进行发送数据操作");

		//读取应答命令
		error = m_tcpClient->TxRxMsg(replyCmdData, queryCmdData, replyLength);

		//判读应答数据是否正确
		zlog_info(Util::m_zlog,"判断应答数据是否正确");
		if (error == ErrorInfo::ERR_OK)
		{
			for (reg = 7; reg < 12; reg++)
			{
				if (replyCmdData[reg] != queryCmdData[reg])
				{
					zlog_warn(Util::m_zlog,"应答数据错误");
					error = ErrorInfo::ERR_INV_RESP;
					break;
				}
			}
		}//end of if(error == ErrorInfo::ERR_OK)

		if(error != ErrorInfo::ERR_OK)
		{
			retry++;

			zlog_warn(Util::m_zlog,"查询数据结束");
			msleep(m_delay);
		}

	}while((retry < m_retries) && (ErrorInfo::ERR_OK != error));

	return error;
}

//16号命令,设置多个寄存器
int ModbusTcpClient::PresetMultipleRegisters(int addr, int start,int length,UshortArray& values)
{
	//1、局部变量
		const short QUERY_LENGTH = (13 + 2 * length);//查询长度
		String queryCmdData;//查询数据
		short replyLength = 12;//应答长度
		String replyCmdData;//应答数据
		short error;
		int reg;
		int retry = 0;
		int i = 0;

		//2、组装设置命令
		queryCmdData.resize(QUERY_LENGTH);
		replyCmdData.resize(replyLength);

		//modbus消息
		for (i = 0; i < 5; i++)
		{
			queryCmdData[i] = (UCHAR)0;
		}

		queryCmdData[5] = length*2+7;
		queryCmdData[6] = (UCHAR)addr;
		queryCmdData[7] = 16;
		queryCmdData[8] = (UCHAR)((start >> 8) & 0xff);
		queryCmdData[9] = (UCHAR)(start & 0xff);
		queryCmdData[10] = (UCHAR)((length >> 8) & 0xff);
		queryCmdData[11] = (UCHAR)(length & 0xff);
		queryCmdData[12] = (2 * length);

		//填充数据
		for (reg = 0; reg < length; reg++)
		{
			queryCmdData[13 + reg * 2] = (UCHAR)((values[reg] >> 8) & 0xff);
			queryCmdData[13 + reg * 2 + 1] = (UCHAR)(values[reg] & 0xff);
		}

		//进行发送数据操作
		do
		{
			zlog_info(Util::m_zlog,"进行发送数据操作");

			//读取应答命令
			error = m_tcpClient->TxRxMsg(replyCmdData, queryCmdData, replyLength);

			//判读应答数据是否正确
			zlog_info(Util::m_zlog,"判断应答数据是否正确");
			if (error == ErrorInfo::ERR_OK)
			{
				for (reg = 7; reg < 12; reg++)
				{
					if (replyCmdData[reg] != queryCmdData[reg])
					{
						zlog_warn(Util::m_zlog,"应答数据错误");
						error = ErrorInfo::ERR_INV_RESP;
						break;
					}
				}
			}//end of if(error == ErrorInfo::ERR_OK)

			if(error != ErrorInfo::ERR_OK)
			{
				retry++;

				zlog_warn(Util::m_zlog,"查询数据结束");
				msleep(m_delay);
			}

		}while((retry < m_retries) && (ErrorInfo::ERR_OK != error));

		return error;
}

//PresetMultipleFloatRegisters--2024-9-5   EcmPcs 北京移动
int ModbusTcpClient::PresetMultipleFloatRegisters(int addr, int start,int length,FloatArray& values)
{
	//1、局部变量    下发： 00+00+00+00+00+字节长度 +地址 +命令字+寄存器起始地址+寄存器长度+数据个数+数据；回复： 00+00+00+00+00+字节长度+地址 +命令字+寄存器起始地址+寄存器长度
		const short QUERY_LENGTH = (13 + 2 * length);//查询长度
		String queryCmdData;//查询数据
		short replyLength = 12;//应答长度 原来是10
		String replyCmdData;//应答数据
		short error;
		int reg;
		int retry = 0;
		int i = 0;
	   float tmp =0;
		//short tmp = 0;
	    unsigned char array[4];
		//unsigned short crc16;

		//2、组装设置命令
		queryCmdData.resize(QUERY_LENGTH);
		replyCmdData.resize(replyLength);

		//modbus消息
		for (i = 0; i < 5; i++)// 改后是3，改回到原来5 了，
		{
			queryCmdData[i] = (UCHAR)0;
		}
		queryCmdData[5] = length*2+7;
		queryCmdData[6] = (UCHAR)addr;
		queryCmdData[7] = 16;
		queryCmdData[8] = (UCHAR)((start >> 8) & 0xff);
		queryCmdData[9] = (UCHAR)(start & 0xff);
		queryCmdData[10] = (UCHAR)((length >> 8) & 0xff);
		queryCmdData[11] = (UCHAR)(length & 0xff);
		queryCmdData[12] = (2 * length);
		tmp = values[0];
		memcpy(array, &tmp, sizeof(float));
		//memcpy(array, &tmp, 4);
		queryCmdData[13] =array[1];//小端   CDAB
		queryCmdData[14] =array[0];
		queryCmdData[15] =array[3];
		queryCmdData[16] =array[2];

		//填充数据
		//length = length - 1;
//		printf("%d ", queryCmdData.size());
//		printf("\n");
//		for (reg = 0; reg < 1; reg++)
//		{
//
//			tmp = values[reg];
//			//memcpy(array, &tmp, sizeof(float));
//			memcpy(array, &tmp, 4);
//			queryCmdData.push_back(array[1]);//小端   CDAB
//			printf("%02x ", array[1]);
//			printf("\n");
//			printf("%d ", queryCmdData.size());
//			printf("\n");
//			queryCmdData.push_back(array[0]);
//			printf("%02x ", array[0]);
//			printf("\n");
//			printf("%d ", queryCmdData.size());
//			printf("\n");
//			queryCmdData.push_back(array[3]);
//			printf("%02x ", array[3]);
//			printf("\n");
//			printf("%d ", queryCmdData.size());
//			printf("\n");
//			queryCmdData.push_back(array[2]);
//			printf("%02x ", array[2]);
//			printf("\n");
//			printf("%d ", queryCmdData.size());
//			printf("\n");
//		}
		//进行发送数据操作
		do
		{
			zlog_info(Util::m_zlog,"进行发送数据操作");

//							printf("下发数据:");
//							for(int i = 0; i < queryCmdData.size(); i++)
//							{
//							printf("%02x ", queryCmdData[i]);
//							}
//							printf("\n");

			//读取应答命令
			error = m_tcpClient->TxRxMsg(replyCmdData, queryCmdData, replyLength);
//							printf("上传数据:");
//							for(int i = 0; i < replyCmdData.size(); i++)
//							{
//								printf("%02x ", replyCmdData[i]);
//							}
//							printf("\n");

			//判读应答数据是否正确
			zlog_info(Util::m_zlog,"判断应答数据是否正确");
			if (error == ErrorInfo::ERR_OK)
			{
				for (reg = 4; reg < 7; reg++)
				{
					if (replyCmdData[reg] != queryCmdData[reg])
					{
						zlog_warn(Util::m_zlog,"应答数据错误");
						error = ErrorInfo::ERR_INV_RESP;
						break;
					}
				}
			}//end of if(error == ErrorInfo::ERR_OK)

			if(error != ErrorInfo::ERR_OK)
			{
				retry++;

				zlog_warn(Util::m_zlog,"查询数据结束");
				msleep(m_delay);
			}

		}while((retry < m_retries) && (ErrorInfo::ERR_OK != error));

		return error;
}

int ModbusTcpClient::PresetRS485MultipleRegisters(int addr, int start,int length,UshortArray& values)
{
	//1、局部变量
		const short QUERY_LENGTH = (13 + 2 * length);//查询长度
		String queryCmdData;//查询数据
		short replyLength = 8;//应答长度
		String replyCmdData;//应答数据
		short error;
		int reg;
		int retry = 0;
		int i = 0;
		unsigned short crc16;

		//2、组装设置命令
		replyCmdData.resize(replyLength);

		//modbus消息
		queryCmdData.clear();
		queryCmdData.push_back((char)addr);//设备地址
		queryCmdData.push_back((char)16);//功能码
		queryCmdData.push_back((char)(start >> 8)&0xFF);//寄存器起始高字节
		queryCmdData.push_back((char)(start & 0xFF));//寄存器起始低字节
		queryCmdData.push_back((char)(length >> 8)&0xFF);//寄存器长度高字节
		queryCmdData.push_back((char)(length & 0xFF));//寄存器长度地字节
		queryCmdData.push_back((char)(2*length));//数据长度字节

		//填充数据
		for (reg=0; reg<(length); reg++)
		{
			queryCmdData.push_back((char)(values[reg] >> 8));
			queryCmdData.push_back((char)(values[reg] & 0xFF));
		}

		crc16 = Check::Crc16(queryCmdData);
		queryCmdData.push_back((char)(crc16 & 0xFF));
		queryCmdData.push_back((char)(crc16 >> 8)&0xFF);

		//进行发送数据操作
		do
		{
			zlog_info(Util::m_zlog,"进行发送数据操作");

			//读取应答命令
			error = m_tcpClient->TxRxMsg(replyCmdData, queryCmdData, replyLength);

			//判读应答数据是否正确
			zlog_info(Util::m_zlog,"判断应答数据是否正确");
			if (error == ErrorInfo::ERR_OK)
			{
				for (reg=0;reg<6;reg++)
				{
					if (replyCmdData[reg]!=queryCmdData[reg])
					{
						error = ErrorInfo::ERR_INV_RESP;
						zlog_warn(Util::m_zlog, "应答错误：%d 无效应答" ,error);
						break;
					}
				}//end of for (reg=0;reg<6;reg++)
			}//end of if(error == ErrorInfo::ERR_OK)

			if(error != ErrorInfo::ERR_OK)
			{
				retry++;

				zlog_warn(Util::m_zlog,"查询数据结束");
				msleep(m_delay);
			}

		}while((retry < m_retries) && (ErrorInfo::ERR_OK != error));

		return error;
}

int ModbusTcpClient::PresetRLCFloatRegisters(int addr, int start, int length, FloatArray& values)
{
	zlog_info(Util::m_zlog,
	            "执行 Modbus RTU协议 ModbusRtuMaster::PresetMultipleRegisters： 命令=16, 从站地址= %d, 起始地址=%d, 其寄存器长度= %d.",
	            addr, start, length);

	String query;//查询字符串
	String reply;//应答字符串
//	const WORD QUERY_LENGHT = (7+2*length);
	int replyLength = 12;

	int error;
	int	reg;
	int retry=0;
	float tmp = 0.0;
	unsigned char array[4];
	unsigned short crc16;


	//modbus 帧设置
	query.clear();

	for(int i = 0; i < 5; i++){
		query.push_back((unsigned char)0);
	}
	query.push_back((char)(2*length + 7));//数据长度字节

	query.push_back((char)addr);//设备地址
	query.push_back((char)16);//功能码
	query.push_back((char)(start >> 8)&0xFF);//寄存器起始高字节
	query.push_back((char)(start & 0xFF));//寄存器起始低字节
	query.push_back((char)(length >> 8)&0xFF);//寄存器长度高字节
	query.push_back((char)(length & 0xFF));//寄存器长度地字节
	query.push_back((char)(2*length));//寄存机字节数


	//填充数据
	for (reg=0; reg<(length / 2); reg++)
	{
		tmp = values[reg];
		memcpy(array, &tmp, sizeof(float));
		query.push_back(array[1]);
		query.push_back(array[0]);
		query.push_back(array[3]);
		query.push_back(array[2]);
	}

	// crc16 = Check::Crc16(query);
	// query.push_back((char)(crc16 & 0xFF));
	// query.push_back((char)(crc16 >> 8)&0xFF);
	

	do
	{
#if 0
		printf("=======%d下发数据：",m_tcpClient->Getsockname().GetPort());
		for(int i = 0; i < query.size(); i++)
		{
			printf("%02X ", query[i]);
		}
		printf("\n");
#endif
		error = m_tcpClient->TxRxMsg(reply, query, replyLength);
#if 0
		printf("=======%d上传数据：",m_tcpClient->Getsockname().GetPort());
		for(int i = 0; i < reply.size(); i++)
		{
			printf("%02x ", reply[i]);
		}
		printf("\n");
#endif
		if (error==ErrorInfo::ERR_OK)
		{
			for (reg=6;reg<12;reg++)
			{
				if (reply[reg]!=query[reg])
				{
					error = ErrorInfo::ERR_INV_RESP;
					zlog_warn(Util::m_zlog, "应答错误：%d 无效应答" ,error);
					break;
				}
			}//end of for (reg=0;reg<6;reg++)
		}//end of if (error==ErrorInfo::ERR_OK)

		if(ErrorInfo::ERR_OK != error)
		{
			usleep(m_delay*1000);
		}
		retry++;

	} while ((retry<m_retries)&&(ErrorInfo::ERR_OK != error));

	zlog_info(Util::m_zlog, "执行 Modbus RTU协议  ModbusRtuMaster::PresetMultipleRegisters： 命令=16;错误码= %d, 查询次数=%d~~~~~~" ,error,retry);

	return(error);

}//end PresetMultipleRegisters

//C++布尔型to modbus开关量类型
int ModbusTcpClient::ModBool(int coil)
{
	if (true == coil)
	{
		return(0xFF00);
	}
	else
	{
		return(0x0000);
	}
}
