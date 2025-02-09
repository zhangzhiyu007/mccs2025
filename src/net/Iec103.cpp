/*
 * Iec103.cpp
 *
 *  Created on: 2013-4-2
 *      Author: mzh
 */

#include "Iec103.h"

Iec103::Iec103() :
	m_apciHead(0x55), m_asduFixedHead(0x10), m_asduVariableHead(0x68),
			m_asduEnd(0x16)
{


}

Iec103::~Iec103()
{

}

String Iec103::Pack(const PacketHead* head, const PacketData* data)
{
	String str;
	str.clear();

	if (NULL == head)
	{
		return str;
	}

	if (NULL != data)
	{
		switch (data->type)
		{
		case CONFIRM://确认帧
			str = this->PackConfirm((PacketHead*) head, (PacketData*) data);
			break;
		case TIME_SYNC://时间同步帧
			str = this->PackTimeSync((PacketHead*) head, (PacketData*) data);
			break;
		case REAL_DATA://实时数据主动上传
			str = this->PackRealData((PacketHead*) head, (PacketData*) data);
			break;
		case CTRL_DATA://控制数据下发帧
			str = this->PackCtrlData((PacketHead*) head, (PacketData*) data);
			break;
		default:
			break;
		}
	}
	else
	{
		str = this->PackHead(head, NULL);
	}

	return str;
}

bool Iec103::Unpack(PacketHead& head, PacketData& data,
		const String& dataUnpacked)
{
	zlog_debug(Util::m_zlog,"解析数据包");
	bool ret = false;
	String str;
	str.clear();

	//解析包头部分
	ret = UnpackHead(head, str, dataUnpacked);
	if (!ret)
	{
		zlog_warn(Util::m_zlog,"解析ASDU失败");
		return ret;
	}

	//只有包头没有内容
	if (str.empty())
	{
		zlog_debug(Util::m_zlog,"只有包头没有内容");
		data.type = Iec103::TYPE_NULL;
		return true;
	}

	//解析ASDU部分
	zlog_debug(Util::m_zlog,"解析ASDU部分");
	String tmp;
	PacketFormat format;
	ret = UnpackAsdu(tmp, format, str);
	zlog_debug(Util::m_zlog,"解析ASDU部分完毕");
	if (!ret)
	{
		zlog_warn(Util::m_zlog,"解析ASDU失败");
		return ret;
	}

	data.type = Iec103::CONFIRM;

	data.prm = (tmp[0] >> 6) & 0x01;
	data.fcb = (tmp[0] >> 5) & 0x01;
	data.fcv = (tmp[0] >> 4) & 0x01;
	data.fun = tmp[0] & 0x0F;

	int addr = tmp[1];//从设备地址
	data.destAddr = addr;

	data.data.clear();

	int typ = 0;//类型标识
	int i = 0;
	if (VARIABLE_LENGTH == format)//变长格式
	{
		data.destAddr = tmp[5];//地址
		typ = tmp[2];
		if (typ == 0x06)//时间同步
		{
			data.type = Iec103::TIME_SYNC;

			for (i = 8; i < (int)tmp.size(); i++)
			{
				data.data.push_back(tmp[i]);
			}
		}
		else if (typ == 0x09)//测量值上传报文
		{
			data.type = Iec103::REAL_DATA;

			data.continuous = (tmp[3] >> 7) & 0x01;
			data.dataLength = tmp[3] & 0x7F;

			if (Iec103::ALONE_ADDR == data.continuous)//每个数据单独地址
			{
				data.dataStart = tmp[8] + (tmp[9] << 8);
				for (i = 8; i < (int)tmp.size(); i++)
				{
					data.data.push_back(tmp[i]);
				}
			}
			else if(Iec103::CONTINUOUS_ADDR == data.continuous)//连续地址
			{
				data.dataStart = tmp[8] + (tmp[9] << 8);

				for (i = 10; i < (int)tmp.size(); i++)
				{
					data.data.push_back(tmp[i]);
				}
			}
		}
		else if (typ == 0x14)//控制命令下发
		{
			data.type = Iec103::CTRL_DATA;

			data.continuous = (tmp[3] >> 7) & 0x01;
			data.dataLength = tmp[3] & 0x7F;
			data.dataStart = tmp[8] + (tmp[9] << 8);
			for (i = 8; i < (int)tmp.size(); i++)
			{
				data.data.push_back(tmp[i]);
			}
		}
	}

	zlog_debug(Util::m_zlog,"解析ASDU成功");
	//判断是否
	return true;
}

bool Iec103::DataComparison(int dataNumber1, int dataNumber2)
{
	if ((dataNumber1 < 10000) && (dataNumber2 > 60000))
	{
		return true;
	}

	return (dataNumber1 > dataNumber2) ? true : false;

}

//数据编号
void Iec103::DataNumber(int& dataNumber)
{
	if (dataNumber >= 65535)
	{
		dataNumber = 0;
	}
	else
	{
		dataNumber++;
	}
}

String Iec103::PackTimeSync(PacketHead* head, PacketData* data)
{
	zlog_debug(Util::m_zlog, "封装时间同步帧");
	String str;
	str.clear();
	if ((NULL == head) || (NULL == data))
	{
		return str;
	}

	//封装传输帧ASDU
	String asdu;
	int i = 0;
	asdu.resize(8);
	asdu[0] = (data->prm << 6) | (data->fcb << 5) | (data->fcv << 4) | 0x03;//控制域0111 0100
	asdu[1] = data->destAddr;//从站地址
	asdu[2] = 0x06;//类型标识（TYP）
	asdu[3] = 0x81;//可变结构限定词（VSQ）
	asdu[4] = 0x08;//传送原因（COT）
	asdu[5] = data->destAddr;//应用服务单元公共地址
	asdu[6] = ANY_TYPE;//功能类型（FUN）
	asdu[7] = 0;//信息序号（INF）
	//将数据添加到字符串末尾
	for (i = 0; i < (int)data->data.size(); i++)
	{
		asdu.push_back(data->data[i]);
	}

	//添加ASDU部分
	str = this->PackAsdu(&asdu, VARIABLE_LENGTH);
	//添加包头部分
	str = this->PackHead(head, &str);

	zlog_debug(Util::m_zlog, "封装时间同步帧成功");
	return str;
}

String Iec103::PackRealData(PacketHead* head, PacketData* data)
{
	zlog_debug(Util::m_zlog, "封装实时数据帧");
	String str;
	str.clear();
	if ((NULL == head) || (NULL == data))
	{
		return str;
	}

	//封装传输帧ASDU
	String asdu;
	asdu.resize(8);
	asdu[0] = (data->prm << 6) | (data->fcb << 5) | (data->fcv << 4) | 0x04;//控制域0111 0100
	asdu[1] = data->destAddr;//从站地址
	asdu[2] = 0x09;//类型标识（TYP）
	asdu[3] = ((data->continuous & 0x01) << 7) + data->dataLength;//可变结构限定词（VSQ）
	asdu[4] = 0x02;//传送原因（COT）
	asdu[5] = data->destAddr;//应用服务单元公共地址
	asdu[6] = 144;//功能类型（FUN）
	asdu[7] = 0;//信息序号（INF）
	//将数据添加到字符串末尾
	for (int i = 0; i < (int)data->data.size(); i++)
	{
		asdu.push_back(data->data[i]);
	}

	//添加ASDU部分
	str = this->PackAsdu(&asdu, VARIABLE_LENGTH);
	//添加包头部分
	str = this->PackHead(head, &str);

	zlog_debug(Util::m_zlog, "封装实时数据成功");
	return str;
}

String Iec103::PackCtrlData(PacketHead* head, PacketData* data)
{
	zlog_debug(Util::m_zlog, "封装控制数据帧");
	String str;
	str.clear();
	if ((NULL == head) || (NULL == data))
	{
		return str;
	}

	//封装传输帧ASDU
	String asdu;
	asdu.resize(8);
	asdu[0] = (data->prm << 6) | (data->fcb << 5) | (data->fcv << 4) | 0x03;//控制域0111 0100
	asdu[1] = data->destAddr;//从站地址
	asdu[2] = 0x14;//类型标识（TYP）
	asdu[3] = ((data->continuous & 0x01) << 7) + data->dataLength;//可变结构限定词（VSQ）
	asdu[4] = 20;//传送原因（COT）
	asdu[5] = data->destAddr;//应用服务单元公共地址
	asdu[6] = 16;//功能类型（FUN）
	asdu[7] = 0;//信息序号（INF）
	//将数据添加到字符串末尾
	for (int i = 0; i < (int)(data->data.size()); i++)
	{
		asdu.push_back(data->data[i]);
	}

	//添加ASDU部分
	str = this->PackAsdu(&asdu, VARIABLE_LENGTH);
	//添加包头部分
	str = this->PackHead(head, &str);

	zlog_debug(Util::m_zlog, "封装控制数据成功");
	return str;
}

String Iec103::PackConfirm(PacketHead* head, PacketData* data)
{
	zlog_debug(Util::m_zlog, "封装确认帧");
	String str;
	str.clear();
	if ((NULL == head) || (NULL == data))
	{
		return str;
	}

	//封装传输帧ASDU
	String asdu;
	asdu.resize(2);
	asdu[0] = (data->prm << 6) | (data->fcb << 5) | (data->fcv << 4)
			| (data->fun);//控制域0111 0100
	asdu[1] = data->destAddr;//从站地址

	//添加ASDU部分
	str = this->PackAsdu(&asdu, FIXED_LENGTH);

	//添加包头部分
	str = this->PackHead(head, &str);

	zlog_debug(Util::m_zlog, "封装确认帧成功");
	return str;
}

String Iec103::PackAsdu(const String* data, const PacketFormat& format)
{
	zlog_debug(Util::m_zlog, "ASDU数据帧打包");

	String str;
	str.clear();
	//如果字符串为空,则返回空字符串
	if ((NULL == data) || (data->empty()))
	{
		zlog_info(Util::m_zlog, "ASDU数据帧为空");
		return str;
	}

	String dataTmp = *data;

	//设置起始结束符
	int length = data->size();//从控制域到链路用户数据之间字节长度
	int endChar = m_asduEnd;//结束符
	switch (format)
	{
	case FIXED_LENGTH:
		str.push_back(m_asduFixedHead);
		break;
	case VARIABLE_LENGTH:
		str.push_back(m_asduVariableHead);
		str.push_back(length);
		str.push_back(length >> 8);
		str.push_back(m_asduVariableHead);
		break;
	default:
		break;
	}

	int i = 0;
	//封装
	//将字符串添加末尾
	for (i = 0; i < (int)dataTmp.size(); i++)
	{
		str.push_back(dataTmp[i]);
	}

	int checksum = 0;
	for (i = 0; i < length; i++)
	{
		checksum += dataTmp[i];
	}

	str.push_back((char) checksum);
	str.push_back((char) endChar);

	zlog_debug(Util::m_zlog, "ASDU数据帧打包成功");
	return str;
}

bool Iec103::UnpackAsdu(String& data, PacketFormat& format,
		const String& dataUnpacked)
{
	zlog_debug(Util::m_zlog, "ASDU数据帧解析");

	//1\校验字符长度,固定格式为5字节
	if (dataUnpacked.size() < 5)
	{
		zlog_warn(Util::m_zlog, "字节长度太短%d",dataUnpacked.size());
		return false;
	}

	//2\判断起始符和结束符
	int startChar = dataUnpacked[0];
	unsigned char checksum = 0;
	data.clear();
	if (m_asduFixedHead == startChar)//固定格式
	{
		//校验起始字符和结束字符
		if (m_asduEnd != dataUnpacked[4])
		{
			zlog_warn(Util::m_zlog, "固定格式:结束符错误");
			return false;
		}

		//判断校验和
		checksum = dataUnpacked[1] + dataUnpacked[2];
		if (checksum != dataUnpacked[3])
		{
			zlog_warn(Util::m_zlog, "固定格式:校验和错误");
			return false;
		}

		data.push_back(dataUnpacked[1]);
		data.push_back(dataUnpacked[2]);

		format = FIXED_LENGTH;
	}
	else if (m_asduVariableHead == startChar)//变长格式
	{
		//判断字节长度
		if (dataUnpacked.size() < 6)
		{
			zlog_warn(Util::m_zlog, "变长格式:字节数太短");
		}
		//校验起始字符和结束字符

		if ((dataUnpacked[3] != m_asduVariableHead)
				|| (dataUnpacked[dataUnpacked.size() - 1] != m_asduEnd))
		{
			zlog_warn(Util::m_zlog, "变长格式:起始符和结束符错误");
			return false;
		}

		//3\判断长度是否与要求相符
		int len = dataUnpacked[1] + (dataUnpacked[2] << 8);
		if (len != (dataUnpacked.size() - 6))
		{
			zlog_warn(Util::m_zlog, "变长格式:字节长度不符:len=%d,dataUnpacked.size()=%d",len,dataUnpacked.size());
			return false;
		}

		//判断校验和
		int i = 0;
		for (i = 4; i < (dataUnpacked.size() - 2); i++)
		{
			checksum += dataUnpacked[i];
		}
		if (checksum != dataUnpacked[dataUnpacked.size() - 2])
		{
			zlog_warn(Util::m_zlog, "变长格式:校验和错误:checksum=%02X  data=%02X",checksum,dataUnpacked[dataUnpacked.size() - 2]);

			return false;
		}

		//5.复制
		data.clear();
		for (i = 4; i < (dataUnpacked.size() - 2); i++)
		{
			data.push_back(dataUnpacked[i]);
		}

		format = VARIABLE_LENGTH;
	}
	else
	{
		zlog_warn(Util::m_zlog, "起始符错误");
		return false;
	}

	zlog_debug(Util::m_zlog, "ASDU数据帧解析成功");
	return true;
}

String Iec103::PackHead(const PacketHead* head, const String* data)
{
	String str;
	str.clear();
	if (NULL == head)
	{
		return str;
	}

	int len = 10;
	if (data != NULL)
	{
		len += data->size();
	}

	//添加包头
	str.resize(14);
	str[0] = m_apciHead;
	str[1] = (char) len;
	str[2] = (char) (len >> 8);
	str[3] = m_apciHead;
	str[4] = head->srcAddr;
	str[5] = head->srcType;
	str[6] = head->destAddr;
	str[7] = head->destType;
	str[8] = (char) head->dataNumber;
	str[9] = (char) (head->dataNumber >> 8);
	str[10] = head->networkA;
	str[11] = head->networkB;
	str[12] = head->backup1;
	str[13] = head->backup2;

	//添加ASDU部分
	int i = 0;
	if ((data != NULL) && (!data->empty()))
	{
		for (i = 0; i < (data->size()); i++)
		{
			str.push_back((*data)[i]);
		}
	}

	return str;
}

bool Iec103::UnpackHead(PacketHead& head, String& data,
		const String& dataUnpacked)
{
	//1.长度校验
	if (dataUnpacked.size() < 14)
	{
		zlog_warn(Util::m_zlog,"包头长度校验不成功");
		return false;
	}
	//2.包头
	if ((m_apciHead != dataUnpacked[0]) || (m_apciHead != dataUnpacked[3]))
	{
		zlog_warn(Util::m_zlog,"校验包头不成功");
		return false;
	}

	//3.长度
	int length;
	length = dataUnpacked[2];
	length = (length << 8) + dataUnpacked[1];

	if (length > (dataUnpacked.size() - 4))
	{
		zlog_warn(Util::m_zlog,"校验长度不成功");
		return false;
	}

	//4.保存包信息
	head.length = length;
	head.srcAddr = dataUnpacked[4];
	head.srcType = dataUnpacked[5];
	head.destAddr = dataUnpacked[6];
	head.destType = dataUnpacked[7];
	head.dataNumber = dataUnpacked[8] + (dataUnpacked[9] << 8);
	head.networkA = dataUnpacked[10];
	head.networkB = dataUnpacked[11];
	head.backup1 = dataUnpacked[12];
	head.backup2 = dataUnpacked[13];

	//5.ASDU部分
	data.clear();
	//不存在ASDU部分
	if (length <= 10)
	{
		return true;
	}

	//存在ASDU部分,将除APCI部分全部保存到字符串中
	int i = 0;
	for (i = 14; i < (int) (dataUnpacked.size()); i++)
	{
		data.push_back(dataUnpacked[i]);
	}

	return true;
}
