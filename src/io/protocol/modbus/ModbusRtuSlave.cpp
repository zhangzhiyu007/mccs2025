/*
 * ModbusRtuSlave.cpp
 *
 *   创建日期: 2013-4-12
 *   作         者: mzh
 */

#include "ModbusRtuSlave.h"
#include "../../../util/Check.h"

ModbusRtuSlave::ModbusRtuSlave()
{
	// TODO 构造函数
	//设置串口参数
	SetCom(4, 9600, 8, 'N', 1);
	//设置串口超时时间
	SetTimeout(10);
	//设置从设备地址
	SetAddr(1);
	//寄存器起始地址
	SetRegStart(0);
	//寄存器结束地址
	SetRegEnd(MAX_VALUE);

}

ModbusRtuSlave::~ModbusRtuSlave()
{
	// TODO 析构函数
}

//设置串口参数
void ModbusRtuSlave::SetCom(int port,int baudrate,int databits,char parity,int stopbits)
{
	m_com.SetParam(baudrate, databits, parity, stopbits);
	m_com.SetPort(port);
	return;
}


//设置串口超时时间
void ModbusRtuSlave::SetTimeout(int timeout)
{
	m_com.SetTimeout(timeout);
}

//设置从设备地址
void ModbusRtuSlave::SetAddr(int addr)
{
	this->m_addr = addr;
}


//寄存器起始地址
void ModbusRtuSlave::SetRegStart(int regStart)
{
	this->m_regStart = regStart;
}

//寄存器结束地址
void ModbusRtuSlave::SetRegEnd(int regEnd)
{
	this->m_regEnd = regEnd;
}

//线程运行
void ModbusRtuSlave::Run()
{
	String dataRead;
	String dataWrite;
	ModbusRtu modbusData;

	///加上超时判断
	int ret;
	int error;

	if (!m_com.IsOpen())
	{
		//如果串口关闭，则打开串口
		m_com.Open();
	}

	while (true)
	{
		//判断线程是否结束
		if (m_state == Thread::STOPPING)
		{
			zlog_warn(Util::m_zlog, "串口操作失败，串口线程结束");
			break;
		}

		//判断串口是否打开
		if (!m_com.IsOpen())
		{
			//如果串口关闭，则重新打开串口
			sleep(10);
			zlog_warn(Util::m_zlog, "串口关闭，重新打开串口");
			m_com.Open();

			continue;
		}

		//读取串口数据,并进行数据解析
		dataRead.clear();
		modbusData.data.clear();
		//读取串口数据
		//zlog_info(Util::m_zlog, "读取串口数据");
		ret = Read(dataRead);

		//说明收到数据
		if(ErrorInfo::ERR_OK==ret)
		{
			error = UnpackModbus(modbusData, dataRead);
			if(ErrorInfo::ERR_CHECK != error)
			{
				if(ErrorInfo::ERR_OK == error)
				{
					/*返回应答数据*/
					error = PackModbus(dataWrite, modbusData);
				}
				else
				{
					error = PackErrorMessage(dataWrite, dataRead, error);
				}

				if(ErrorInfo::ERR_CHECK != error)
				{
					m_com.Write(dataWrite);
				}
				else
				{
					zlog_warn(Util::m_zlog, "命令执行失败");
				}

			}//end of if(ErrorInfo::ERR_CHECK != error)

		}//end of if(ret>0)

	}
}

int ModbusRtuSlave::Read(String& data)
{
	//Modbus协议数据为不定长度数据
	//协议长度必定大于等于8个字节

	data.clear();
	int i = 0;
	//1.首先读取8个字节数据
	int len = 8;
	int tmp = 0;
	String dataRead;
	int ret;
	ret = m_com.Read(dataRead, len);
	if (ret < len)
	{
		return ErrorInfo::ERR_NO_DATA;
	}

	//如果收到有效数据,判断命令
	for (i = 0; i < len; i++)
	{
		data.push_back(dataRead[i]);
	}

	switch (dataRead[1])
	{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		{
			return ErrorInfo::ERR_OK;
		}
			break;
		case 15:
		{
			//判断寄存器长度
			tmp = dataRead[4];
			tmp = (tmp * 256 )+ dataRead[5];
			tmp = (tmp + 7) / 8;
			if (tmp == dataRead[6])
			{
				len = tmp+1;
				ret = m_com.Read(dataRead, len);

				if (ret < len)
				{
					data.clear();
					return ErrorInfo::ERR_NO_DATA;
				}
				else
				{
					zlog_warn(Util::m_zlog, "收到数据：");
					for (i = 0; i < len; i++)
					{
						data.push_back(dataRead[i]);
					}

					return ErrorInfo::ERR_OK;
				}
			}
		}
			break;

		case 16:
		{
			//15\16号命令需要再次读取后续数据
			//判断寄存器长度
			tmp = (dataRead[4] << 8) + dataRead[5];
			tmp = tmp * 2;
			if (tmp == dataRead[6])
			{
				len = tmp+1;
				ret = m_com.Read(dataRead, len);
				if (ret < len)
				{
					data.clear();
					return ErrorInfo::ERR_NO_DATA;
				}
				else
				{
					for (i = 0; i < len; i++)
					{
						data.push_back(dataRead[i]);
					}

					return ErrorInfo::ERR_OK;
				}
			}
		}
			break;
		default:
			break;
	}
	return ErrorInfo::ERR_NO_DATA;
}

/*
 * 名称: UnpackModbus
 * 功能: 将接收到的Modbus数据进行解析,保存到ModbusRtu&中
 * 输入:
 * modbus    :[out]ModbusRtu& Modbus数据
 * data      :[in]string&  从串口中接收到的数据
 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
 */
int ModbusRtuSlave::UnpackModbus(ModbusRtu& modbus, String& data)
{
	int ret = Check(data);

	zlog_info(Util::m_zlog, "ret = %d ：",ret);

	if (ErrorInfo::ERR_OK == ret)
	{
		int slaveAddr = data[0];//设备地址
		int function = data[1];//功能码
		int startAddr = (data[2] << 8 | data[3]);//起始地址

		switch (function)
		{
			case 1:
			case 2:
			case 3:
			case 4:
			{
				modbus.order = function;
				modbus.addr = slaveAddr;
				modbus.regStart = startAddr;
				modbus.regLength = (data[4] << 8 | data[5]);
			}
				break;

			case 5:
			{
				modbus.data.resize(1);
				modbus.order = function;
				modbus.addr = slaveAddr;
				modbus.regStart = startAddr;
				modbus.regLength = 1;
				if (0xFF00 == (data[4] << 8 | data[5]))
				{
					modbus.data[0] = 1;
				}
				else
				{
					modbus.data[0] = 0;
				}
			}
				break;

			case 6:
			{
				modbus.data.resize(1);
				modbus.order = function;
				modbus.addr = slaveAddr;
				modbus.regStart = startAddr;
				modbus.regLength = 1;
				modbus.data[0] = (data[4] << 8 | data[5]);
			}
				break;

			case 15:
			{
				modbus.order = function;
				modbus.addr = slaveAddr;
				modbus.regStart = startAddr;
				modbus.regLength = (data[4] << 8 | data[5]);
				modbus.data.resize((data[4] << 8 | data[5]));

				int coil = 0;
				int bit = 0;
				int respByte = 7;

				for (coil = 0; coil < (int)modbus.data.size(); coil++)
				{
					if (bit > 7)
					{
						bit = 0;
						respByte++;
					}

					if ((data[respByte] & (1 << bit)) > 0)
					{
						modbus.data[coil] = 1;
					}
					else
					{
						modbus.data[coil] = 0;
					}
					bit++; //next bit

				}//end of for (coil = 0; coil < modbus.data.size(); coil++)

			}
				break;

			case 16:
			{
				modbus.order = function;
				modbus.addr = slaveAddr;
				modbus.regStart = startAddr;
				modbus.regLength = (data[4] << 8 | data[5]);
				modbus.data.resize(modbus.regLength);

				int byte = 0;
				int respByte = 7;
				for (respByte = 7; respByte < data[6] + 7; respByte += 2)
				{
					modbus.data[byte] = (data[respByte]);
					modbus.data[byte] = ((modbus.data[byte] << 8) | (data[respByte
							+ 1]));

					byte++;
				}

			}//end of case 16
				break;

			default:
				break;
		}//end of switch (function)
	}
	else
	{
		if (ErrorInfo::ERR_CHECK == ret)
		{
			return ErrorInfo::ERR_CHECK;//其他错误，不响应
		}
		else
		{
			if(ErrorInfo::ERR_INV_RESP==ret)
			{
				return ErrorInfo::ERR_INV_RESP;//指令错误
			}
			else
			{
				return ErrorInfo::ERR_OUT_OF_RANGE;//寄存器地址越界
			}
		}//end of if(ErrorInfo::ERR_CHECK==ret)

	}// end of if(ErrorInfo::ERR_OK==ret)

	return ErrorInfo::ERR_OK;

}


/*
 * 名称: PackModbus
 * 功能: 将需要返回的ModbusRtu数据进行封装,保存data中
 * 输入:
 * data      :[out]string&  向串口中发送的数据
 * modbus    :[in]ModbusRtu& Modbus数据
 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
 */
int ModbusRtuSlave::PackModbus(String& data, ModbusRtu& modbus)
{
	zlog_info(Util::m_zlog,"地址=%d,命令=%d,起始地址=%d,长度=%d,data=%d",modbus.addr,modbus.order,modbus.regStart,modbus.regLength,modbus.data.size());
	int function = modbus.order;//功能码
	RealData tmpData;//保存临时读取的数据
	int i;
	int crc16;
	data.clear();
	switch (function)
	{
		case 1:
		case 2:
		{
			zlog_info(Util::m_zlog,"%d号命令，读取数据库数据",function);

			modbus.data.clear();
			for (i = 0; i < modbus.regLength; i++)
			{
				if (MemDb::GetRealData(tmpData, modbus.regStart+i))//获取数据
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
					return ErrorInfo::ERR_CHECK;//获取失败，不响应
				}
			}
			zlog_info(Util::m_zlog,"将返回数据进行封装");
			data.resize(3 + (modbus.regLength + 7) / 8);//应答组数据长度
			data[0] = modbus.addr;
			data[1] = modbus.order;
			data[2] = (modbus.regLength + 7) / 8;
			int replyByte = 3;
			int bit = 0;
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

			crc16 = Check::Crc16(data);
			data.push_back((char) (crc16 & 0xFF));
			data.push_back((char) (crc16 >> 8));

		}//end of case 2:
			break;

		case 3:
		case 4:
		{
			zlog_info(Util::m_zlog,"%d号命令，读取数据库数据",function);
			modbus.data.clear();
			for (i = modbus.regStart; i < (modbus.regStart+modbus.regLength); i++)
			{
				if (MemDb::GetRealData(tmpData, modbus.regStart+i))//获取数据
				{
					if (tmpData.pv.type == RealData::INT)
					{
						modbus.data.push_back((USHORT)tmpData.pv.data.iValue);
					}
					else
					{
						modbus.data.push_back((USHORT)tmpData.pv.data.dbValue);
					}
				}
				else
				{
					return ErrorInfo::ERR_CHECK;//获取失败，不响应
				}
			}

			zlog_info(Util::m_zlog,"将返回数据进行封装");
			data.push_back((char) modbus.addr);
			data.push_back((char) modbus.order);
			data.push_back((char) (2* modbus .regLength));
			int bit = 0;
			for (bit = 0; bit < (int)modbus.data.size(); bit++)
			{
				data.push_back((char) (modbus.data[bit] >> 8));
				data.push_back((char) (modbus.data[bit] & 0xff));
			}

			crc16 = Check::Crc16(data);
			data.push_back((char) (crc16 & 0xFF));
			data.push_back((char) (crc16 >> 8));

		}//end of case 4:
			break;

		case 5:
		case 6:
		{
			zlog_info(Util::m_zlog,"%号命令，数据封装",function);
			if (!(MemDb::SetRealDataUnknown(modbus.regStart, modbus.data[0], true)))
			{
				return ErrorInfo::ERR_CHECK;//设置失败，不响应
			}

			data.push_back((char) modbus.addr);
			data.push_back((char) modbus.order);
			data.push_back((char) (modbus.regStart >> 8));
			data.push_back((char) (modbus.regStart & 0xFF));
			if (5 == function)
			{
				if (modbus.data[0])
				{
					data.push_back((char) (0xFF00 >> 8));//开关量高字节
					data.push_back((char) (0xFF00 & 0xFF));//开关量低字节
				}
				else
				{
					data.push_back((char) (0x0000 >> 8));//开关量高字节
					data.push_back((char) (0x0000 & 0xFF));//开关量低字节
				}
			}
			else
			{
				data.push_back((char) (modbus.data[0] >> 8));
				data.push_back((char) (modbus.data[0] & 0xFF));
			}

			crc16 = Check::Crc16(data);
			data.push_back((char) (crc16 & 0xFF));
			data.push_back((char) (crc16 >> 8));

		}//end of case 6:
			break;

		case 15:
		case 16:
		{
			zlog_info(Util::m_zlog,"%d號命令，数据封装",function);

			for (i = 0; i < modbus.data.size(); i++)
			{
				if (!(MemDb::SetRealDataUnknown(modbus.regStart + i, modbus.data[i], true)))
				{
					return ErrorInfo::ERR_CHECK;//设置失败，不响应
				}
			}

			data.push_back((char) modbus.addr);
			data.push_back((char) modbus.order);
			data.push_back((char) (modbus.regStart >> 8));
			data.push_back((char) (modbus.regStart & 0xFF));
			data.push_back((char) (modbus.regLength >> 8));
			data.push_back((char) (modbus.regLength & 0xFF));

			crc16 = Check::Crc16(data);
			data.push_back((char) (crc16 & 0xFF));
			data.push_back((char) (crc16 >> 8));

		}//end of case 16:
			break;

		default:
			break;

	}//end of switch(function)

	return ErrorInfo::ERR_OK;
}


/*
 * 名称: PackErrorMessage,由
 * 功能: 将需要错误信息数据进行封装,保存data中
 * 输入:
 * writeData   :[out]String&  向串口发送的数据
 * readData    :[in]String& 从串口中接收到的数据
 * errorCode :[in]int  判断错误类型
 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
 */
int ModbusRtuSlave::PackErrorMessage(String& writeData, String& readData, int errorCode)
{
	int tmpErrorCrc16 = 0;
	String tmpErrorData;
	tmpErrorData.resize(3);
	writeData.clear();

	tmpErrorData[0] = readData[0];//设备地址
	tmpErrorData[1] = readData[1]+0x80;//功能码

	if (ErrorInfo::ERR_INV_RESP == errorCode)//指令错误
	{
		tmpErrorData[2] = 0x01;
		tmpErrorCrc16 = Check::Crc16(tmpErrorData);
	}//end of if(ErrorInfo::ERR_INV_RESP==ret)
	else
	{
		tmpErrorData[2] = 0x02;
		tmpErrorCrc16 = Check::Crc16(tmpErrorData);
	}//end of if(ErrorInfo::ERR_INV_RESP==ret)//指令错误

	writeData.push_back(tmpErrorData[0]);//设备地址
	writeData.push_back(tmpErrorData[1]);//功能码
	writeData.push_back(tmpErrorData[2]);//错误码
	writeData.push_back(tmpErrorCrc16 & 0xFF);//校验码
	writeData.push_back(tmpErrorCrc16 >> 8);

	return ErrorInfo::ERR_OUT_OF_RANGE;
}



/*
 * 名称: Check
 * 功能: 对modbus命令进行校验,由UnpackModbus调用
 * 1.判断是否是Modbus命令,不是不返回错误信息
 * 2.数据长度校验:校验失败,不返回错误信息
 * 3.地址和命令校验:校验失败,不返回错误信息
 * 4.CRC校验:校验失败,不返回错误信息
 * 5.寄存器地址和长度越界校验:校验失败,返回错误信息1
 * 输入:
 * data      :[in]string&  待校验数据
 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
 */
int ModbusRtuSlave::Check(String& data)
{
//	//校验时使用
//	int m_addr;//设备地址
//	int m_regStart;//寄存器起始地址,用于标示寄存器最大地址范围,防止越界
//	int m_regEnd;//寄存器结束地址
	//1 设备地址   2 功能码    3 起始地址

    if (data.size() >= 8)
    {
    	zlog_info(Util::m_zlog, "data.size() = %d ：",data.size());

		int slaveAddr = data[0];//设备地址
		int function = data[1];//功能码
		int startAddr = (data[2] << 8 | data[3]);//起始地址

		String tmpData;
		tmpData.resize(data.size()-2);
		int i;
		int tmpCrc16;
		for(i=0; i<(data.size()-2); i++)
		{
			tmpData[i] = data[i];
		}
		tmpCrc16 = Check::Crc16(tmpData);

		zlog_info(Util::m_zlog, "slaveAddr = %d   m_addr = %d ：",slaveAddr,m_addr);
		if (slaveAddr == m_addr)
		{
			switch (function)
			{
				case 1:
				case 2:
				case 3:
				case 4:
				case 15:
				case 16:
				{
					if(data[data.size()-2]== (tmpCrc16 & 0xFF)
							&& data[data.size()-1]== (tmpCrc16 >> 8))//crc校验
					{
						int regLength = (data[4] << 8 | data[5]);
						if (startAddr >= m_regStart
								&& (startAddr + regLength) <= m_regEnd)
						{
							return ErrorInfo::ERR_OK;
						}
						else
						{
							return ErrorInfo::ERR_OUT_OF_RANGE;
						}
					}
				}
					break;

				case 5:
				case 6:
				{
					if (data[data.size() - 2] == (tmpCrc16 & 0xFF)
						&& data[data.size()-1] == (tmpCrc16 >> 8))//crc校验
					{
						if (startAddr >= m_regStart
								&& startAddr <= m_regEnd)
						{
							return ErrorInfo::ERR_OK;
						}
						else
						{
							return ErrorInfo::ERR_OUT_OF_RANGE;//寄存器地址越界
						}
					}
				}
					break;

				default:
				{
					return ErrorInfo::ERR_INV_RESP;//指令错误
				}
				break;
			}
		}
	}

    return ErrorInfo::ERR_CHECK;//其他错误，不响应
}


