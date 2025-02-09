/*
 * FgyBMS.cpp
 *
 *  Created on: 2013-10-14
 *      Author: liuhaitao
 */

#include "FgyBMS.h"

FgyBMS::FgyBMS()
{
	// TODO Auto-generated constructor stub
	m_can = NULL;
}

FgyBMS::~FgyBMS()
{
	// TODO Auto-generated destructor stub
}

void FgyBMS::SetCan(Can* can)
{
	assert(can!=NULL);
	m_can = can;
}

int FgyBMS::Read(Device::SlaveDev* dev,Can::FrameInfo& frame)
{
	int ret = ErrorInfo::ERR_OK;
	int addr = atoi(dev->slaveAddr.c_str());

	//设备型号
	int devModel = 0;

	BYTE id[4] =
	{ 0, 0, 0, 0 };

	id[0] = (frame.id >> 24) & 0xFF;
	id[1] = (frame.id >> 16) & 0xFF;
	id[2] = (frame.id >> 8) & 0xFF;
	id[3] = (frame.id >> 0) & 0xFF;

	if ((id[2] == 0x32) && (id[3] == 0xc8+addr))
	{
		devModel = DevModel::Model_HN_Lib_15;//ACLT-LiB
	}
	else
	{

	}

	if (DevModel::Model_HN_Lib_15 == devModel)//型号ACLT-LiB
	{
		ret = this->AcltLibRead(dev, frame);
	}
	else
	{

	}

	return ret;
}

int FgyBMS::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_HN_Lib_15:
		ret = this->AcltLibPreset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int FgyBMS::AcltLibRead(Device::SlaveDev* dev,Can::FrameInfo& frame)
{
	int ret = ErrorInfo::ERR_OK;
	int i = 0x00;
	int tmp = 0;
	float data = 0.0;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;

	//电池模块电压
	for (i = 0x00; i < 0x14; i++)
	{
		if (frame.id == ((0x18003200 + (i * 0x10000)) | (((0x18003200 + (i
				* 0x10000)) & 0xFF) | (0xC8 + addr))))
		{
			zlog_info(Util::m_zlog,"通讯正常");

			//把接收数据存到MemDb里
			MemDb::SetRealData(regStart, 1, false);//通讯状态
			data = (((frame.data[0]) | (frame.data[1] << 8)) & 0x0FFF) * 0.1;
			MemDb::SetRealData(regStart + i * 4 + 255, data, false);
			data = (((frame.data[2]) | (frame.data[3] << 8)) & 0x0FFF) * 0.1;
			MemDb::SetRealData(regStart + i * 4 + 256, data, false);
			data = (((frame.data[4]) | (frame.data[5] << 8)) & 0x0FFF) * 0.1;
			MemDb::SetRealData(regStart + i * 4 + 257, data, false);
			data = (((frame.data[6]) | (frame.data[7] << 8)) & 0x0FFF) * 0.1;
			MemDb::SetRealData(regStart + i * 4 + 258, data, false);
		}
	}

	//电池包温度
	for (i = 4; i < 6; i++)
	{
		if (frame.id == ((0x18603200 + (i * 0x10000)) | (((0x18603200 + (i
				* 0x10000)) & 0xFF) | (0xC8 + addr))))
		{
			zlog_info(Util::m_zlog,"通讯正常");

			//把接收数据存到MemDb里
			MemDb::SetRealData(regStart, 1, false);//通讯状态
			tmp = frame.data[0] - 40;
			MemDb::SetRealData(regStart + (i - 4) * 3 + 80, (float) tmp, false);
			tmp = frame.data[1] - 40;
			MemDb::SetRealData(regStart + (i - 4) * 3 + 81, (float) tmp, false);
			tmp = frame.data[2] - 40;
			MemDb::SetRealData(regStart + (i - 4) * 3 + 82, (float) tmp, false);
		}
	}

	//其余数据
	if (frame.id == ((0x18823200) | ((0x18823200 & 0xFF) | (0xc8 + addr))))
	{
		zlog_info(Util::m_zlog,"通讯正常");

		//把接收数据存到MemDb里
		MemDb::SetRealData(regStart, 1, false);//通讯状态
		data = ((frame.data[1]) | (frame.data[0] << 8)) * 0.1;
		MemDb::SetRealData(regStart + 13, data, false);//最高电压
		data = ((frame.data[3]) | (frame.data[2] << 8)) * 0.1;
		MemDb::SetRealData(regStart + 14, data, false);//最低电压
		tmp = frame.data[4] - 40;
		MemDb::SetRealData(regStart + 15, (float) tmp, false);//最高温度
		tmp = frame.data[5] - 40;
		MemDb::SetRealData(regStart + 16, (float) tmp, false);//最低温度
	}

	if (frame.id == ((0x18833200) | ((0x18833200 & 0xFF) | (0xc8 + addr))))
	{
		zlog_info(Util::m_zlog,"通讯正常");

		//把接收数据存到MemDb里
		MemDb::SetRealData(regStart, 1, false);//通讯状态
		tmp = (((frame.data[0]) & 0xFFFFFFFF) << 16) | ((frame.data[1])
				& 0xFFFFFFFF);
		MemDb::SetRealData(regStart + 17, tmp, false);//最高电压箱号/最高电压箱内位置
		tmp = (((frame.data[2]) & 0xFFFFFFFF) << 16) | ((frame.data[3])
				& 0xFFFFFFFF);
		MemDb::SetRealData(regStart + 18, tmp, false);//最低电压箱号/最低电压箱内位置
		tmp = (((frame.data[4]) & 0xFFFFFFFF) << 16) | ((frame.data[5])
				& 0xFFFFFFFF);
		MemDb::SetRealData(regStart + 19, tmp, false);//最高温度箱号/最低温度箱内位置
		tmp = (((frame.data[6]) & 0xFFFFFFFF) << 16) | ((frame.data[7])
				& 0xFFFFFFFF);
		MemDb::SetRealData(regStart + 20, tmp, false);//最低温度箱号/最低温度箱内位置
	}

	if (frame.id == ((0x18843200) | ((0x18843200 & 0xFF) | (0xc8 + addr))))
	{
		zlog_info(Util::m_zlog,"通讯正常");

		//把接收数据存到MemDb里
		MemDb::SetRealData(regStart, 1, false);//通讯状态
//		data = ((short) (((frame.data[0] & 0xFF) | (frame.data[1] << 8)))) * 0.1;
		data = 60.0;
		MemDb::SetRealData(regStart + 11, data, false);//最大允许充电电流
//		data = ((short) (((frame.data[2] & 0xFF) | (frame.data[3] << 8)))) * 0.1;
		data = 60.0;
		MemDb::SetRealData(regStart + 12, data, false);//最大允许放电电流
		data = ((short) (((frame.data[4] & 0xFF) | (frame.data[5] << 8))))
				* 0.1;
		MemDb::SetRealData(regStart + 6, data, false);//总电压
		data = ((short) (((frame.data[6] & 0xFF) | (frame.data[7] << 8))))
				* 0.1 * (-1);
//		zlog_warn(Util::m_zlog,"data = %.2f,frame.data[6] = %02x, frame.data[7] = %02x", data, frame.data[6],frame.data[7]);
		MemDb::SetRealData(regStart + 7, data, false);//总电流
	}

	if (frame.id == ((0x18853200) | ((0x18853200 & 0xFF) | (0xc8 + addr))))
	{
		zlog_info(Util::m_zlog,"通讯正常");

		//把接收数据存到MemDb里
		tmp = frame.data[0];
		MemDb::SetRealData(regStart + 2, tmp, false);//电池状态位
		tmp = frame.data[1];
		MemDb::SetRealData(regStart + 5, tmp, false);//预充电请求标志位
		tmp = frame.data[2];
		MemDb::SetRealData(regStart + 10, (float) tmp, false);//SOC
		tmp = frame.data[3];
		MemDb::SetRealData(regStart + 8, (float) tmp, false);//系统可充电电量
		tmp = frame.data[4];
		MemDb::SetRealData(regStart + 9, (float) tmp, false);//系统可放电电量
		tmp = 0;
		if ((frame.data[5] != 0) || (frame.data[6] != 0))
		{
			if ((frame.data[5] & 0x01) > 0)//电池过压
			{
				Util::SetBitValue(tmp, 7, true);
			}
			if ((frame.data[5] & 0x02) > 0)//电池欠压
			{
				Util::SetBitValue(tmp, 6, true);
			}
			if ((frame.data[5] & 0x04) > 0)//电池过温
			{
				Util::SetBitValue(tmp, 5, true);
			}
			if ((frame.data[5] & 0x08) > 0)//电池欠温
			{
				Util::SetBitValue(tmp, 4, true);
			}
			if ((frame.data[5] & 0x10) > 0)//充电电流过大
			{
				Util::SetBitValue(tmp, 3, true);
			}
			if ((frame.data[5] & 0x20) > 0)//放电电流过大
			{
				Util::SetBitValue(tmp, 2, true);
			}
			if ((frame.data[5] & 0x40) > 0)//SOC过高
			{
				Util::SetBitValue(tmp, 1, true);
			}
			if ((frame.data[5] & 0x80) > 0)//SOC过低
			{
				Util::SetBitValue(tmp, 0, true);
			}
			if ((frame.data[6] & 0x01) > 0)
			{
				Util::SetBitValue(tmp, 8, true);
			}

			MemDb::SetRealData(regStart + 3, 1, false);//故障状态
			MemDb::SetRealData(regStart + 4, tmp, false);//故障码
		}
		else
		{
			MemDb::SetRealData(regStart + 3, 0, false);
			MemDb::SetRealData(regStart + 4, 0, false);//故障码
		}

	}

	if (frame.id == ((0x18863200) | ((0x18863200 & 0xFF) | (0xc8 + addr))))
	{
		zlog_info(Util::m_zlog,"通讯正常");

		//把接收数据存到MemDb里
//		data = (((frame.data[0]) | (frame.data[1] << 8))) * 0.1;
		data = 3.55;
		MemDb::SetRealData(regStart + 21, data, false);//最高允许充电单体电压
//		data = (((frame.data[2]) | (frame.data[3] << 8))) * 0.1;
		data = 3.0;
		MemDb::SetRealData(regStart + 22, data, false);//最低允许充电单体电压
//		tmp = (frame.data[4] - 40);
		tmp = 55;
		MemDb::SetRealData(regStart + 23, (float)tmp,false);//最高允许工作温度
//		tmp = (frame.data[5] - 40);
		tmp = (-10);
		MemDb::SetRealData(regStart + 24, (float)tmp,false);//最低允许充电温度
	}

	if (frame.id == ((0x18873200) | ((0x18873200 & 0xFF) | (0xc8 + addr))))
	{
		zlog_info(Util::m_zlog,"通讯正常");

		//把接收数据存到MemDb里
		tmp = ((frame.data[0]) | (frame.data[1] << 8) | (frame.data[2] << 16)
				| (frame.data[3] << 24)) * 0.1;
		MemDb::SetRealData(regStart + 25, tmp, false);//累积充电AH
		tmp = ((frame.data[4]) | (frame.data[5] << 8) | (frame.data[6] << 16)
				| (frame.data[7] << 24)) * 0.1;
		MemDb::SetRealData(regStart + 26, tmp, false);//累积放电AH
	}
	return ret;
}

int FgyBMS::AcltLibPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int regStart = dev->regStart;
	int addr = atoi(dev->slaveAddr.c_str());
	RealData realData;
	int tmp = 0;
	Can::FrameInfo frameWrite;//CAN帧信息

	//判断是否有接触器命令
	if(MemDb::GetRealDataWrited(realData,regStart+1,true))
	{
		if(realData.pv.type == RealData::INT)
		{
			if(realData.pv.data.iValue == 1)//闭合接触器
			{
				tmp = 0x55;
			}
			else if(realData.pv.data.iValue == 0)//断开接触器
			{
				tmp = 0xAA;
			}
		}
		else if(realData.pv.type == RealData::DB)
		{
			zlog_warn(Util::m_zlog,"闭合接触器参数错误");
			ret = ErrorInfo::ERR_OK;
		}

		//封装控制数据
		frameWrite.type = Can::EXTENDED_FRAME;//扩展帧
		frameWrite.id = (0x18870032)|((((0x18870032 >> 8)&0xFF)|(0xC8+addr))<<8);
		frameWrite.data.push_back(tmp);
		frameWrite.data.push_back(0x00);
		frameWrite.data.push_back(0x00);
		frameWrite.data.push_back(0x00);
		frameWrite.data.push_back(0x00);
		frameWrite.data.push_back(0x00);
		frameWrite.data.push_back(0x00);

		//发送控制命令
		ret = m_can->Write(frameWrite);

		if (ret != ErrorInfo::ERR_OK)
		{
			zlog_info(Util::m_zlog,"发送数据失败");
		}

		zlog_info(Util::m_zlog,"发送数据结束");
	}
	return ret;
}
