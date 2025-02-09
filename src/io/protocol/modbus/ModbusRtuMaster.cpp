/*
 * ModbusRtuMaster.cpp
 *
 *   创建日期: 2013-4-11
 *   作         者: mzh
 */

#include "ModbusRtuMaster.h"
#include "../../../util/Check.h"
#include <cstdlib>

ModbusRtuMaster::ModbusRtuMaster()
{
	// TODO 构造函数
	m_com = NULL;
	m_retries = 3;//默认3次
	m_delayTime = 1000;//延时默认1000毫秒
	m_isTcp = false;
	m_tcpClient = NULL;
}

ModbusRtuMaster::~ModbusRtuMaster()
{
	// TODO 析构函数
}

//设置是否采用以太网通讯
void ModbusRtuMaster::SetIsTcp(bool isTcp)
{
	m_isTcp = isTcp;
}

void ModbusRtuMaster::SetCom(Com* com)
{
	m_com = com;
}

Com* ModbusRtuMaster::GetCom()
{
	return m_com;
}

//设置/返回网口指针
void ModbusRtuMaster::SetTcpClient(TcpClient* tcpClient)
{
	//assert(tcpClient != NULL);
	m_tcpClient = tcpClient;
}

TcpClient* ModbusRtuMaster::GetTcpClient()
{
	return m_tcpClient;
}

void ModbusRtuMaster::SetRetries(int retries)
{
	m_retries = retries;
}

int ModbusRtuMaster::GetRetries() const
{
	return m_retries;
}

void ModbusRtuMaster::SetDelayTime(int delayTime)
{
	this->m_delayTime = delayTime;
}

//1号命令，读输出状态量（开关量）
int ModbusRtuMaster::ReadOutputStatus(UshortArray& values,//返回数组，0或1
		int addr,//Modbus设备地址
		int start,//起始地址
		int length)//长度
{
	zlog_info(Util::m_zlog,
			"ModbusRtuMaster::ReadOutputStatus： 命令=1, 从站地址= %d, 起始地址=%d, 其寄存器长度= %d.",
			addr, start, length);

	String query;//查询字符串
	String reply;//应答字符串
	int replyLength=(5 + (length + 7) / 8);//length+7防止丢失比特位
	reply.resize(replyLength+1);

	int error;
	int respByte;
	int coil;
	int bit;
	unsigned short crc16;

	//modbus message
	query.clear();
	query.push_back((char)addr);
	query.push_back((char)1);
	query.push_back((char)(start >> 8)&0xFF);
	query.push_back((char)(start & 0xFF));
	query.push_back((char)(length >> 8)&0xFF);
	query.push_back((char)(length & 0xFF));

	crc16 = Check::Crc16(query);
	query.push_back((char)(crc16 & 0xFF));
	query.push_back((char)(crc16 >> 8));

	error = TxRxMsg(reply, query, replyLength);

	values.clear();
	values.resize(length);
	if (error == ErrorInfo::ERR_OK)
	{
		if ((reply[0] != query[0]) || (reply[1] != query[1]))
		{
			error = ErrorInfo::ERR_INV_RESP;
			zlog_info(Util::m_zlog, "应答错误：%d 无效应答" ,error);
		}
		else
		{
			bit = 0;
			respByte = 3;

			for (coil = 0; coil < length; coil++)
			{
				if (bit > 7)
				{
					bit = 0;
					respByte++;
				}

				if ((reply[respByte] & (1 << bit)) > 0)
				{
					values[coil] = 1;
				}
				else
				{
					values[coil] = 0;
				}
				bit++; //next bit
			}// end of for(coil=0; coil<length; coil++)

		}//end of if ((abyReply[0]!=abyQuery[0])||(abyReply[1]!=abyQuery[1]))

	}//end of if (error==Util::ERR_OK)

	zlog_info(Util::m_zlog, "执行 Modbus RTU协议  ModbusRtuMaster::ReadOutputStatus： 命令=1;错误码= %d~~~~~~" ,error);

	return(error);

}//end of int ModbusRtuMaster::ReadOutputStatus(UshortArray& values,


//2号命令，读输入状态量
int ModbusRtuMaster::ReadInputStatus(UshortArray& values, int addr, int start,	int length)
{
	zlog_info(Util::m_zlog,
			"执行 Modbus RTU协议 ModbusRtuMaster::ReadInputStatus： 命令=2, 从站地址= %d, 起始地址=%d, 其寄存器长度= %d.",
			addr, start, length);

	String query;//查询字符串
	String reply;//应答字符串
	int replyLength=(5 + (length + 7) / 8);
	//	values.resize(length);

	int error;
	int respByte;
	int coil;
	int bit;
	unsigned short crc16 = 0;

	//modbus message
	query.clear();
	query.push_back((char)addr);
	query.push_back((char)2);
	query.push_back((char)(start >> 8)&0xFF);
	query.push_back((char)(start & 0xFF));
	query.push_back((char)(length >> 8)&0xFF);
	query.push_back((char)(length & 0xFF));

	crc16 = Check::Crc16(query);
	query.push_back((char)(crc16 & 0xFF));
	query.push_back((char)(crc16 >> 8)&0xFF);

	error = TxRxMsg(reply, query, replyLength);
	values.clear();
	values.resize(length);
	if (error == ErrorInfo::ERR_OK)
	{
		if ((reply[0] != query[0]) || (reply[1] != query[1]))
		{
			error = ErrorInfo::ERR_INV_RESP;
			zlog_info(Util::m_zlog, "应答错误：%d 无效应答" ,error);
		}
		else
		{
			bit = 0;
			respByte = 3;
			for (coil = 0; coil < length; coil++)
			{
				if (bit > 7)
				{
					bit = 0;
					respByte++;
				}

				if ((reply[respByte] & (0x01 << bit)) > 0)
				{
					values[coil] = 1;
				}
				else
				{
					values[coil] = 0;
				}
				bit++; //next bit

				zlog_info(Util::m_zlog,"%d %02X %d , ",coil, reply[respByte], values[coil]);
			}// end of for(coil=0; coil<length; coil++)

		}// end of if ((reply[0]!=query[0])||(reply[1]!=query[1]))

	}//end of if (error==ErrorInfo::ERR_OK)

	zlog_info(Util::m_zlog, "执行 Modbus RTU协议  ModbusRtuMaster::ReadInputStatus： 命令=1;错误码= %d~~~~~~" ,error);

	return(error);

}//end of int ModbusRtuMaster::ReadInputStatus(UshortArray& values, int addr, int start,	int length)

//2号命令 ，读RCL输入状态量
int ModbusRtuMaster::ReadInputStatusRcl(UshortArray& values, int addr, int start,	int length)
{
	zlog_info(Util::m_zlog,
			"执行 Modbus RTU协议 ModbusRtuMaster::ReadInputStatus： 命令=2 RCL, 从站地址= %d, 起始地址=%d, 其寄存器长度= %d.",
			addr, start, length);

	String query;//查询字符串
	String reply;//应答字符串
	int replyLength=(9 + 5 + (length + 7) / 8);

	int error;
	int respByte;
	int coil;
	int bit;
	unsigned short crc16 = 0;

	//modbus message
	query.clear();
	query.push_back((char)0xBE);
	query.push_back((char)0x09);
	query.push_back((char)0xBE);
	query.push_back((char)0x09);
	query.push_back((char)0xBE);
	query.push_back((char)0x09);
	query.push_back((char)0x55);

	query.push_back((char)addr);
	query.push_back((char)2);
	query.push_back((char)(start >> 8)&0xFF);
	query.push_back((char)(start & 0xFF));
	query.push_back((char)(length >> 8)&0xFF);
	query.push_back((char)(length & 0xFF));

	crc16 = Check::Crc16(query);
	query.push_back((char)(crc16 & 0xFF));
	query.push_back((char)(crc16 >> 8)&0xFF);

	query.push_back((char)0x0D);
	query.push_back((char)0x0A);

	error = TxRxMsg(reply, query, replyLength);

	values.clear();
	values.resize(length);
	if (error == ErrorInfo::ERR_OK)
	{
		if ((reply[7] != query[7]) || (reply[8] != query[8]))
		{
			error = ErrorInfo::ERR_INV_RESP;
		}
		else
		{
			bit = 0;
			respByte = 3 + 7;
			for (coil = 0; coil < length; coil++)
			{
				if (bit > 7)
				{
					bit = 0;
					respByte++;
				}

				if ((reply[respByte] & (0x01 << bit)) > 0)
				{
					values[coil] = 1;
				}
				else
				{
					values[coil] = 0;
				}
				bit++; //next bit

			}// end of for(coil=0; coil<length; coil++)

		}// end of if ((reply[0]!=query[0])||(reply[1]!=query[1]))


	}//end of if (error==ErrorInfo::ERR_OK)

	zlog_info(Util::m_zlog, "执行 Modbus RTU协议  ModbusRtuMaster::ReadInputStatus： 命令=2 RCL; 错误码= %d~~~~~~" ,error);

	return(error);

}//end of int ModbusRtuMaster::ReadInputStatus(UshortArray& values, int addr, int start,	int length)



//读输出寄存器，3号命令
int ModbusRtuMaster::ReadOutputRegisters(UshortArray& values, int addr, int start, int length)
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

	// 查询命令
		// printf("下发数据:");
		// for(int i = 0; i < query.size(); i++)
		// {
		// 	printf("%02x ", query[i]);
		// }
		// printf("\n");
	error = TxRxMsg(reply, query, replyLength);
		// printf("上传数据:");
		// for(int i = 0; i < reply.size(); i++)
		// {
		// 	printf("%02x ", reply[i]);
		// }
		// printf("\n");
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

}//end of int ModbusRtuMaster::ReadOutputRegisters(UshortArray& values, int addr, int start, int length)

//读取直流稳定电源寄存器
int ModbusRtuMaster::ReadDCSourceRegisters(UshortArray& values, int addr, int start, int length)
{
	//局部变量
	String query;//查询命令数据
	String reply;////应答字符串
	//应答长度，设备地址+命令号+字节长度+2×读取寄存器数量 + crc校验
	int replyLength=(5 + 2 * length)+4;

	int error;
	int byte;
	int respByte;
	unsigned short crc16;
	int i =0;

	//modbus message
	query.push_back((char)addr);
	query.push_back((char)0x4C);//功能码
	query.push_back(4);//数据长度
	query.push_back((char)(start >> 8)&0xFF);//寄存器起始高字节
	query.push_back((char)(start & 0xFF));//寄存器起始低字节
	query.push_back((char)(length >> 8)&0xFF);//寄存器长度高字节
	query.push_back((char)(length & 0xFF));//寄存器长度地字节

	crc16 = Check::Crc16(query);
	query.push_back((char)(crc16 & 0xFF));
	query.push_back((char)(crc16 >> 8)&0xFF);

	//查询命令
	error = TxRxMsg(reply, query, replyLength);

	values.clear();
	values.resize(length + 2);
	//查询无错误
	if (error == ErrorInfo::ERR_OK)
	{
		//判断应答地址和命令是否与查询一致
		if ((reply[0] != query[0]) || (reply[1] != query[1]))
		{
			error = ErrorInfo::ERR_INV_RESP;//应答错误
			zlog_warn(Util::m_zlog, "应答错误：%d 无效应答" ,error);
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

	zlog_info(Util::m_zlog, "执行 Modbus RTU协议  ModbusRtuMaster::ReadOutputRegisters： 命令=4c;错误码= %d" ,error);

	return(error);

}//end of int ModbusRtuMaster::ReadOutputRegisters(UshortArray& values, int addr, int start, int length)

//读浮点型输出寄存器，3号命令
int ModbusRtuMaster::ReadOutputFloatRegisters(FloatArray& values, int addr, int start, int length)
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
	error = TxRxMsg(reply, query, replyLength);
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
				tmp = (reply[respByte] << 24) + (reply[respByte + 1] << 16)
				+ (reply[respByte + 2] << 8) + (reply[respByte + 3]);
				memcpy(&temp, &tmp, sizeof(int));
				values[byte] = temp;
				byte++;
			}

		}
	}

	zlog_info(Util::m_zlog, "执行 Modbus RTU协议  ModbusRtuMaster::ReadOutputRegisters： 命令=3;错误码= %d" ,error);
	return(error);
}

int ModbusRtuMaster::ReadRLCFloatRegisters(FloatArray& values, int addr, int start, int length)
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
	error = TxRxMsg(reply, query, replyLength);
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
}

//读双极性AC/DC寄存器，3号命令
int ModbusRtuMaster::ReadBipolarRegisters(String& values, int addr, int start, int length)
{
	zlog_info(Util::m_zlog,
			"执行 Modbus RTU协议 ModbusRtuMaster::ReadOutputRegisters： 命令=3, 从站地址= %d, 起始地址=%d, 其寄存器长度= %d.",
			addr, start, length);

	//局部变量
	String query;//查询命令数据
	String reply;////应答字符串
	//应答长度，设备地址+命令号+字节长度+2×读取寄存器数量 + crc校验
	int replyLength=(7 + length);

	reply.resize(replyLength+1);

	int error;
	int byte;
	int respByte;
	unsigned short crc16;

	//modbus message
	query.push_back((char)addr);
	query.push_back((char)3);//功能码
	query.push_back((char)length);//寄存器长度
	query.push_back((char)(start >> 8)&0xFF);//寄存器起始高字节
	query.push_back((char)(start & 0xFF));//寄存器起始低字节

	crc16 = Check::Crc16(query);
	query.push_back((char)(crc16 & 0xFF));
	query.push_back((char)(crc16 >> 8)&0xFF);

	//查询命令
//			printf("下发数据:");
//			for(int i = 0; i < query.size(); i++)
//			{
//				printf("%02x ", query[i]);
//			}
//			printf("\n");
	error = TxRxMsg(reply, query, replyLength);
//			printf("上传数据:");
//			for(int i = 0; i < reply.size(); i++)
//			{
//				printf("%02x ", reply[i]);
//			}
//			printf("\n");
	values.clear();
	values.resize(length);
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
			for (respByte = 4; respByte < (replyLength - 3); respByte++)
			{
				values[byte] = reply[respByte];
				byte++;
			}

		}
	}//end of if (error==ErrorInfo::ERR_OK)

	zlog_info(Util::m_zlog, "执行 Modbus RTU协议  ModbusRtuMaster::ReadOutputRegisters： 命令=3;错误码= %d" ,error);

	return(error);

}//end of int ModbusRtuMaster::ReadOutputRegisters(UshortArray& values, int addr, int start, int length)

//4号命令,读取输入寄存器值
int ModbusRtuMaster::ReadInputRegisters(UshortArray& values, int addr, int start, int length)
{
	zlog_info(Util::m_zlog,
			"执行 Modbus RTU协议 ModbusRtuMaster::ReadInputRegisters： 命令=4, 从站地址= %d, 起始地址=%d, 其寄存器长度= %d.",
			addr, start, length);

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
	
	
	error = TxRxMsg(reply, query, replyLength);//读取查询消息

	//for(int i = 0 ; i < reply.size(); i++){
		//zlog_warn(Util::m_zlog, "%d", reply[i] );
	//}
	


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

}//end of int ModbusRtuMaster::ReadInputRegisters(UshortArray& values, int addr, int start, int length)

//5号命令,设置单个开关量
int ModbusRtuMaster::ForceSingleCoil(int addr, int regAddr, bool value)
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
					// printf("下发数据:");
					// for(int i = 0; i < query.size(); i++)
					// {
					// 	printf("%02x ", query[i]);
					// }
					// printf("\n");
		error = TxRxMsg(reply, query, replyLength);
					// printf("上传数据:");
					// for(int i = 0; i < reply.size(); i++)
					// {
					// 	printf("%02x ", reply[i]);
					// }
					// printf("\n");
		if (error==ErrorInfo::ERR_OK)
		{
			for (reg=0; reg < QUERY_LENGHT; reg++)
			{

				if (reply[reg]!=query[reg])
				{
					error = ErrorInfo::ERR_INV_RESP;
					zlog_info(Util::m_zlog, "应答错误：%d 无效应答" ,error);
					break;
				}
			}//end of for (reg=0; reg < QUERY_LENGHT; reg++)

		}//end of if (error==ErrorInfo::ERR_OK)

		if(ErrorInfo::ERR_OK != error)
		{
			usleep(m_delayTime*1000);
		}
		retry++;

	} while ((retry < m_retries)&&(ErrorInfo::ERR_OK != error));

	zlog_info(Util::m_zlog, "执行 Modbus RTU协议  ModbusRtuMaster::ForceSingleCoil： 命令=5;错误码= %d, 查询次数=%d~~~~~~" ,error,retry);

	return(error);

}//end ForceSingleCoil

//6号命令,设置单个寄存器
int ModbusRtuMaster::PresetSingleRegister(int addr, int regAddr, unsigned short value)
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
							// printf("下发数据:");
							// for(int i = 0; i < query.size(); i++)
							// {
							// 	printf("%02x ", query[i]);
							// }
							// printf("\n");
		error = TxRxMsg(reply, query, replyLength);
							// printf("上传数据:");
							// for(int i = 0; i < reply.size(); i++)
							// {
							// 	printf("%02x ", reply[i]);
							// }
							// printf("\n");
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
			usleep(m_delayTime*1000);
		}
		retry++;

	} while ((retry<m_retries)&&(ErrorInfo::ERR_OK != error));

	zlog_info(Util::m_zlog, "执行 Modbus RTU协议  ModbusRtuMaster::PresetSingleRegister： 命令=6;错误码= %d, 查询次数=%d~~~~~~" ,error,retry);

	return(error);

}//end PresetSingleRegister

//15号命令,设置多个开关量
int ModbusRtuMaster::ForceMultipleCoils(int addr, int start, int length, UshortArray& values)
{
	zlog_info(Util::m_zlog,
			"执行 Modbus RTU协议 ModbusRtuMaster::ForceMultipleCoils： 命令=15, 从站地址= %d, 起始地址=%d, 其寄存器长度= %d.",
			addr, start, length);

	String query;//查询字符串
	String reply;//应答字符串
	//	int QUERY_LENGHT = (7+(length + 7)/8);
	int replyLength = 8;

	int error;
	int queryByte;
	int coil;
	int bit;
	int reg;
	int retry = 0;
	unsigned short crc16;

	//modbus message
	query.clear();
	query.push_back((char)addr);//设备地址
	query.push_back((char)15);//功能码
	query.push_back((char)(start >> 8)&0xFF);//寄存器起始高字节
	query.push_back((char)(start & 0xFF));//寄存器起始低字节
	query.push_back((char)(length >> 8)&0xFF);//寄存器长度高字节
	query.push_back((char)(length & 0xFF));//寄存器长度地字节
	query.push_back((char)((length + 7)/8));//数据长度字节

	query.resize(9-2+(length + 7)/8);

	queryByte = 7;
	bit=0;
	for (coil=0; coil<length; coil++)
	{
		if (bit>7)
		{
			bit=0;
			queryByte++;
		}

		if (values[coil])
		{
			query[queryByte]=(query[queryByte] | (0x01<<bit));
		}

		bit++;
	}

	crc16 = Check::Crc16(query);
	query.push_back((char)(crc16 & 0xFF));
	query.push_back((char)(crc16 >> 8)&0xFF);

	do
	{
		error = TxRxMsg(reply, query, replyLength);

		if (error==ErrorInfo::ERR_OK)
		{

			for (reg=0; reg<6; reg++)
			{
				if (reply[reg]!=query[reg])
				{
					error = ErrorInfo::ERR_INV_RESP;
					zlog_info(Util::m_zlog, "应答错误：%d 无效应答" ,error);
					break;
				}

			}//end of for (reg=0; reg<6; reg++)

		}//end of if (error==ErrorInfo::ERR_OK)

		if(ErrorInfo::ERR_OK != error)
		{
			usleep(m_delayTime*1000);
		}
		retry++;

	} while ((retry<m_retries)&&(ErrorInfo::ERR_OK != error));

	zlog_info(Util::m_zlog, "执行 Modbus RTU协议  ModbusRtuMaster::ForceMultipleCoils： 命令=15;错误码= %d, 查询次数=%d~~~~~~" ,error,retry);

	return(error);

}//end ForceMultipleCoils

int ModbusRtuMaster::PresetMultipleRegisters(int addr, int start, int length, UshortArray& values)
{
	zlog_info(Util::m_zlog,
			"执行 Modbus RTU协议 ModbusRtuMaster::PresetMultipleRegisters： 命令=16, 从站地址= %d, 起始地址=%d, 其寄存器长度= %d.",
			addr, start, length);

	String query;//查询字符串
	String reply;//应答字符串
	//	const WORD QUERY_LENGHT = (7+2*length);
	int replyLength = 8;

	int error;
	int	reg;
	int retry=0;
	unsigned short crc16;


	//modbus 帧设置
	query.clear();
	query.push_back((char)addr);//设备地址
	query.push_back((char)16);//功能码
	query.push_back((char)(start >> 8)&0xFF);//寄存器起始高字节
	query.push_back((char)(start & 0xFF));//寄存器起始低字节
	query.push_back((char)(length >> 8)&0xFF);//寄存器长度高字节
	query.push_back((char)(length & 0xFF));//寄存器长度地字节
	query.push_back((char)(2*length));//数据长度字节

	//填充数据
	for (reg=0; reg<(length); reg++)
	{
		query.push_back((char)(values[reg] >> 8));
		query.push_back((char)(values[reg] & 0xFF));
	}

	crc16 = Check::Crc16(query);
	query.push_back((char)(crc16 & 0xFF));
	query.push_back((char)(crc16 >> 8)&0xFF);

	do
	{
		error = TxRxMsg(reply, query ,replyLength);

		if (error==ErrorInfo::ERR_OK)
		{
			for (reg=0;reg<6;reg++)
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
			usleep(m_delayTime*1000);
		}
		retry++;

	} while ((retry<m_retries)&&(ErrorInfo::ERR_OK != error));

	zlog_info(Util::m_zlog, "执行 Modbus RTU协议  ModbusRtuMaster::PresetMultipleRegisters： 命令=16;错误码= %d, 查询次数=%d~~~~~~" ,error,retry);

	return(error);

}//end PresetMultipleRegisters

int ModbusRtuMaster::PresetMultipleFloatRegisters(int addr, int start, int length, FloatArray& values)
{
	zlog_info(Util::m_zlog,
			"执行 Modbus RTU协议 ModbusRtuMaster::PresetMultipleRegisters： 命令=16, 从站地址= %d, 起始地址=%d, 其寄存器长度= %d.",
			addr, start, length);

	String query;//查询字符串
	String reply;//应答字符串
	//	const WORD QUERY_LENGHT = (7+2*length);
	int replyLength = 8;

	int error;
	int	reg;
	int retry=0;
	float tmp = 0.0;
	unsigned char array[4];
	unsigned short crc16;


	//modbus 帧设置
	query.clear();
	query.push_back((char)addr);//设备地址
	query.push_back((char)16);//功能码
	query.push_back((char)(start >> 8)&0xFF);//寄存器起始高字节
	query.push_back((char)(start & 0xFF));//寄存器起始低字节
	query.push_back((char)(length >> 8)&0xFF);//寄存器长度高字节
	query.push_back((char)(length & 0xFF));//寄存器长度地字节
	query.push_back((char)(2*length));//数据长度字节

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

	crc16 = Check::Crc16(query);
	query.push_back((char)(crc16 & 0xFF));
	query.push_back((char)(crc16 >> 8)&0xFF);

	do
	{
		//		printf("下发数据:");
		//		for(int i = 0; i < query.size(); i++)
		//		{
		//			printf("%02x ", query[i]);
		//		}
		//		printf("\n");
		error = TxRxMsg(reply, query, replyLength);
		//		printf("上传数据:");
		//		for(int i = 0; i < reply.size(); i++)
		//		{
		//			printf("%02x ", reply[i]);
		//		}
		//		printf("\n");
		if (error==ErrorInfo::ERR_OK)
		{
			for (reg=0;reg<6;reg++)
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
			usleep(m_delayTime*1000);
		}
		retry++;

	} while ((retry<m_retries)&&(ErrorInfo::ERR_OK != error));

	zlog_info(Util::m_zlog, "执行 Modbus RTU协议  ModbusRtuMaster::PresetMultipleRegisters： 命令=16;错误码= %d, 查询次数=%d~~~~~~" ,error,retry);

	return(error);

}//end PresetMultipleRegisters

int ModbusRtuMaster::PresetRLCFloatRegisters(int addr, int start, int length, FloatArray& values)
{
	zlog_info(Util::m_zlog,
			"执行 Modbus RTU协议 ModbusRtuMaster::PresetMultipleRegisters： 命令=16, 从站地址= %d, 起始地址=%d, 其寄存器长度= %d.",
			addr, start, length);

	String query;//查询字符串
	String reply;//应答字符串
	//	const WORD QUERY_LENGHT = (7+2*length);
	int replyLength = 8;

	int error;
	int	reg;
	int retry=0;
	float tmp = 0.0;
	unsigned char array[4];
	unsigned short crc16;


	//modbus 帧设置
	query.clear();
	query.push_back((char)addr);//设备地址
	query.push_back((char)16);//功能码
	query.push_back((char)(start >> 8)&0xFF);//寄存器起始高字节
	query.push_back((char)(start & 0xFF));//寄存器起始低字节
	query.push_back((char)(length >> 8)&0xFF);//寄存器长度高字节
	query.push_back((char)(length & 0xFF));//寄存器长度地字节
	query.push_back((char)(2*length));//数据长度字节

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

	crc16 = Check::Crc16(query);
	query.push_back((char)(crc16 & 0xFF));
	query.push_back((char)(crc16 >> 8)&0xFF);

	do
	{
		error = TxRxMsg(reply, query ,replyLength);
		if (error==ErrorInfo::ERR_OK)
		{
			for (reg=0;reg<6;reg++)
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
			usleep(m_delayTime*1000);
		}
		retry++;

	} while ((retry<m_retries)&&(ErrorInfo::ERR_OK != error));

	zlog_info(Util::m_zlog, "执行 Modbus RTU协议  ModbusRtuMaster::PresetMultipleRegisters： 命令=16;错误码= %d, 查询次数=%d~~~~~~" ,error,retry);

	return(error);

}//end PresetMultipleRegisters

int ModbusRtuMaster::PresetDCSourceRegisters(int addr, int function,int length, UshortArray& values)
{
	String query;//查询字符串
	String reply;//应答字符串
	//	const WORD QUERY_LENGHT = (7+2*length);
	int replyLength = 4;

	int error;
	int retry = 0;
	unsigned short crc16;
	query.clear();
	//	query.resize(length + 5);
	//	reply.clear();

	//modbus 帧设置
	query.clear();
	if (function == 0x41)
	{
		query.push_back((char) addr);//设备地址
		query.push_back((char) function);//功能码
		query.push_back((char) (length));//数据长度字节
		query.push_back((char) values[0] >> 8);//功能标志高字节
		query.push_back((char) (values[0] & 0xFF));//功能标志低字节
		query.push_back((char) (values[2] >> 8)&0xFF);//电压高字节
		query.push_back((char) (values[2] & 0xFF));//电压低字节
		query.push_back((char) (values[1] >> 8)&0xFF);//电流高字节
		query.push_back((char) (values[1] & 0xFF));//电流低字节
		query.push_back((char) 0x00);//运行时间高字节
		query.push_back((char) 0x00);//运行时间中高字节
		query.push_back((char) 0x00);//运行时间中低字节
		query.push_back((char) 0x00);//运行时间低字节

		crc16 = Check::Crc16(query);
		query.push_back((char) (crc16 & 0xFF));
		query.push_back((char) (crc16 >> 8)&0xFF);
	}
	else
	{
		query.push_back((char) addr);//设备地址
		query.push_back((char) function);//功能码
		if (length == 0)
		{
			query.push_back((char) length);//

			crc16 = Check::Crc16(query);
			query.push_back((char) (crc16 & 0xFF));
			query.push_back((char) (crc16 >> 8)&0xFF);
		}
		else
		{
			query.push_back(length);
			query.push_back((char) (values[0] >> 8)&0xFF);
			query.push_back((char) (values[0] & 0xFF));

			crc16 = Check::Crc16(query);
			query.push_back((char) (crc16 & 0xFF));
			query.push_back((char) (crc16 >> 8)&0xFF);
		}
	}

	error = TxRxMsg(reply, query, replyLength);
	if (error == ErrorInfo::ERR_OK)
	{
		if (reply[3] == 0x00)//通讯正常
		{
			error = ErrorInfo::ERR_OK;
		}
		else if (reply[3] == 0x01)//超时错误
		{
			error = ErrorInfo::ERR_TIMEOUT;
		}
		else if (reply[3] == 0x02)//校验错误
		{
			error = ErrorInfo::ERR_CHECK;
		}
		else
		{
			error = ErrorInfo::ERR_FAILED;
		}
	}

	zlog_info(Util::m_zlog, "执行 Modbus RTU协议  ModbusRtuMaster::PresetMultipleRegisters： 命令=16;错误码= %d, 查询次数=%d~~~~~~" ,error,retry);

	return (error);
}

int ModbusRtuMaster::PresetBipolarRegisters(int addr, int start, int length, String& values)
{
	zlog_info(Util::m_zlog,
			"执行 Modbus RTU协议 ModbusRtuMaster::PresetMultipleRegisters： 命令=16, 从站地址= %d, 起始地址=%d, 其寄存器长度= %d.",
			addr, start, length);

	String query;//查询字符串
	String reply;//应答字符串
	//	const WORD QUERY_LENGHT = (7+2*length);
	int replyLength = 7;

	int error;
	int	reg;
	int retry=0;
	unsigned short crc16;


	//modbus 帧设置
	query.clear();
	query.push_back((char)addr);//设备地址
	query.push_back((char)16);//功能码
	query.push_back((char)length);//寄存器长度高字节
	query.push_back((char)(start >> 8)&0xFF);//寄存器起始高字节
	query.push_back((char)(start & 0xFF));//寄存器起始低字节

	//填充数据
	for (reg=0; reg<(length); reg++)
	{
		query.push_back((char)values[reg]);
	}

	crc16 = Check::Crc16(query);
	query.push_back((char)(crc16 & 0xFF));
	query.push_back((char)(crc16 >> 8)&0xFF);

	do
	{
		// printf("下发数据:");
		// for(int i = 0; i < query.size(); i++)
		// {
		// 	printf("%02x ", query[i]);
		// }
		// printf("\n");
		error = TxRxMsg(reply, query, replyLength);
		// printf("上传数据:");
		// for(int i = 0; i < reply.size(); i++)
		// {
		// 	printf("%02x ", reply[i]);
		// }
		// printf("\n");

		if (error==ErrorInfo::ERR_OK)
		{
			if((reply[0] != 0x01) || (reply[1] != 0x10)
					|| (reply[2] != 0x00) || (reply[3] != 0x00)
					|| (reply[4] != 0x00) || (reply[5] != 0x1D))
			{
				error = ErrorInfo::ERR_INV_RESP;
				zlog_warn(Util::m_zlog, "应答错误：%d 无效应答" ,error);
				break;
			}
		}//end of if (error==ErrorInfo::ERR_OK)

		if(ErrorInfo::ERR_OK != error)
		{
			usleep(m_delayTime*1000);
		}
		retry++;

	} while ((retry<m_retries)&&(ErrorInfo::ERR_OK != error));

	zlog_info(Util::m_zlog, "执行 Modbus RTU协议  ModbusRtuMaster::PresetMultipleRegisters： 命令=16;错误码= %d, 查询次数=%d~~~~~~" ,error,retry);

	return(error);

}//end PresetMultipleRegisters

int ModbusRtuMaster::TxRxMsg(String& dataRead, String& dataWrite, int len)
{
	int ret = 0;
	zlog_info(Util::m_zlog, "执行 Modbus RTU协议 TxRxMsg：以太网通讯=%d" ,m_isTcp);
	if (m_isTcp)
	{
		zlog_info(Util::m_zlog, "执行 Modbus RTU协议 TxRxMsg：以太网通讯");
		if (NULL == m_tcpClient)
		{
			return ErrorInfo::ERR_NULL;
		}

		if(!m_tcpClient->IsOpened())
		{
			return ErrorInfo::ERR_OPENED;
		}
		ret = m_tcpClient->TxRxMsg(dataRead, dataWrite, len);
	}
	else
	{
		zlog_info(Util::m_zlog, "执行 Modbus RTU协议 TxRxMsg：485通讯");
		if (NULL == m_com)
		{
			return ErrorInfo::ERR_NULL;
		}

		if(!m_com->IsOpen())
		{
			return ErrorInfo::ERR_OPENED;
		}
		zlog_info(Util::m_zlog, "执行 Modbus RTU协议 TxRxMsg：485通讯");
		ret = m_com->TxRxMsg(dataRead, dataWrite, len);
		zlog_info(Util::m_zlog, "执行 Modbus RTU协议 TxRxMsg：485通讯结果=%d",ret);
	}
	return ret;
}

