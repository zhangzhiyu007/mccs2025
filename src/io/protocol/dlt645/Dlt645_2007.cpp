/*
 * Dlt645_2007.cpp
 *
 *   创建日期: 2013-6-17
 *   作         者: mzh
 */

#include "Dlt645_2007.h"

Dlt645_2007::Dlt645_2007()
{
	// TODO 构造函数

}

Dlt645_2007::~Dlt645_2007()
{
	// TODO 析构函数
}

int Dlt645_2007::Validate(ByteArray& abyQuery, ByteArray& abyResponse,
		int replyLength)
{
	int error = ErrorInfo::ERR_OK;
	int i = 0;
	BYTE cs = 0;

	//1、校验长度
	if (abyResponse.size() < replyLength)
	{
		return ErrorInfo::ERR_INV_RESP;//应答数量太短
	}

	//3、校验地址域
	ByteArray abyResp;
	abyResp.clear();
	for (i = m_leadBytesResp; i < abyResponse.size(); i++)
	{
		abyResp.push_back(abyResponse[i]);
	}

	for (i = 0; i < 7; i++)
	{
		if (abyResp[i] != abyQuery[i + m_leadBytes])
		{
			return ErrorInfo::ERR_INV_RESP;
		}
	}

	//4、校验数据域
	for (i = 10; i < 12; i++)
	{
		if (abyResp[i] != abyQuery[i + m_leadBytes])
		{
			return ErrorInfo::ERR_INV_RESP;
		}
	}

	//5、校验控制码
	if (abyResp[8] != (abyQuery[8 + m_leadBytes] + 0x80))
	{
		return ErrorInfo::ERR_INV_RESP;
	}

	//6、验证校验码
	for (i = 0; i < (replyLength - m_leadBytes - 2); i++)
	{
		cs += abyResp[i];
	}

	if (cs != abyResp[replyLength - m_leadBytes - 2])
	{
		return ErrorInfo::ERR_CHECK;
	}

	return error;
}

int Dlt645_2007::ReadData(UINT& regValue,//读取属性值
		string strAddr,//地址域
		BYTE* control,//控制码
		int ctrlCodeLength,//控制码长度
		int regLength)//属性值字节数
{
	zlog_info(Util::m_zlog, "读取Dlt645_2007数据,设备地址=%s",strAddr.c_str());
	UINT error = ErrorInfo::ERR_OK;
	string strAddrTmp;// = _T("");
	strAddrTmp.clear();
	int i = 0;

	ByteArray abyQuery;//查询命令数据
	short replyLength = 16 + regLength + m_leadBytesResp;//应答长度
	ByteArray abyReply;//应答数据

	const short QUERY_LENGHT = 16 + m_leadBytes;
	int retry = 0;
	int length = 0;
	const int ADDR_LEGNT = 12;
	const int BCD_ADDR_LEGHT = 6;
	bool success = false;
	BYTE btCheck = 0;

	//电量数据有8个
	const int ENERGY_DATA_LENGTH = 8;

	//设置查询命令长度，
	abyQuery.resize(QUERY_LENGHT);
	//设置应答长度，不包括CRC校验
	abyReply.resize(replyLength);

	//组装电表查询信息
	//1、发送m_leadBytes个0xFE
	for (i = 0; i < (int) m_leadBytes; i++)
	{
		abyQuery[i] = m_leadByte;
	}

	//2、发送起始帧0x68
	abyQuery[m_leadBytes] = 0x68;

	//3、组装地址域6字节
	//使用压缩BCD码
	//首先判断电表地址长度是否达到12位，没有补0
	length = strAddr.size();

	if (length < ADDR_LEGNT)
	{
		for (i = length; i < ADDR_LEGNT; i++)
		{
			strAddrTmp.push_back('0');// += _T("0");
		}

		for (i = 0; i < length; i++)
		{
			strAddrTmp.push_back(strAddr[i]);
		}
	}
	else if (length >= ADDR_LEGNT)
	{
		for (i = length - ADDR_LEGNT; i < length; i++)
		{
			strAddrTmp.push_back(strAddr[i]);
		}
	}

	//转化为BCD码Ascii2Bcd(char *strDest,int destLen, char *strSrc,int srcLen)
	char str[ADDR_LEGNT];
	char strBcdAddr[ADDR_LEGNT];
	for(i=0; i<ADDR_LEGNT; i++)
	{
		str[i] = strAddrTmp[i];
	}

	success = Ascii2Bcd(strBcdAddr, BCD_ADDR_LEGHT, str, ADDR_LEGNT);

	for (i = 0; i < BCD_ADDR_LEGHT; i++)
	{
		abyQuery[m_leadBytes + 1 + i] = strBcdAddr[BCD_ADDR_LEGHT - i - 1]; //低字节在前，高字节在后
	}

	abyQuery[m_leadBytes + 7] = 0x68;

	//4、写入控制码
	//主站发出读取数据命令，00010001即0x11
	abyQuery[m_leadBytes + 8] = 0x11;

	//5、写入数据域长度，4
	abyQuery[m_leadBytes + 9] = 0x04;

	//6、写入数据域，出数十按字节加0x33处理，接受方减0x33,所有数据项先送低字节再送高字节
	for (i = 0; i < 4; i++)
	{
		abyQuery[m_leadBytes + 10 + i] = control[4 - i - 1] + 0x33;//低字节在前，高字节在后
	}

	//7、添加校验，从起始帧到校验码之前所有字节和
	btCheck = 0;
	for (i = m_leadBytes; i < (QUERY_LENGHT - 2); i++)
	{
		btCheck += abyQuery[i];
	}
	abyQuery[QUERY_LENGHT - 2] = btCheck;

	//8、添加结束符
	abyQuery[QUERY_LENGHT - 1] = 0x16;

	zlog_info(Util::m_zlog, "读取Dlt645_2007数据,设备地址=%s",strAddr.c_str());

//	for(i=0;i<abyQuery.size();i++)
//	{
//		printf("%02X ",abyQuery[i]);
//	}
//	printf("\r\n");
	error =TxRxMsg(abyReply, abyQuery, replyLength);
	zlog_info(Util::m_zlog, "读取Dlt645_2007数据,设备地址=%s,结果%d",strAddr.c_str(),error);
//	for(i=0;i<abyReply.size();i++)
//	{
//		printf("%02X ",abyReply[i]);
//	}
//	printf("\r\n");

	//返回错误
	if (error != ErrorInfo::ERR_OK)
	{
		zlog_warn(Util::m_zlog, "读取Dlt645_2007数据,设备地址=%s,结果%d,失败",strAddr.c_str(),error);
		return error;//通讯异常
	}

	//校验数据
	error = Validate(abyQuery, abyReply, replyLength);

	if (error != ErrorInfo::ERR_OK)
	{
		zlog_warn(Util::m_zlog, "读取Dlt645_2007数据,设备地址=%s,结果%d,校验失败",strAddr.c_str(),error);
		return error;
	}

	//读取数据域数据
	BYTE btData[10] =
	{ 0 };
	int btDataLen = regLength;//实际长度
	for (i = 0; i < regLength; i++)
	{
		btData[regLength - i - 1] = (BYTE) (abyReply[i + 14+m_leadBytesResp] - 0x33);//接收时高字节在后，低字节在前
	}

	int data = 0;

	int num = BCD2INT(&data, (const unsigned char *) btData, 2* btDataLen , 0);

	regValue = data;

	zlog_info(Util::m_zlog, "读取Dlt645_2007数据,设备地址=%s,返回",strAddr.c_str());

	return error;
}

//读取电表数据
int Dlt645_2007::ReadRegisters(PtrArray& values, //如果没有错误情况下读取的数据，UINT型
		string strAddr)//设备地址
{
	zlog_info(Util::m_zlog, "读取dlt645-2007规约智能电表开始,设备地址=%s",strAddr.c_str());
	int error = ErrorInfo::ERR_OK;

	//局部变量
	BYTE ctrlCode[4] =	{ 0 };//控制码
	const int ctrlCodeLength = 4;//控制码长度
	int regLength = 0;//属性值字节数

	UINT regValue = 0;

	const int data = 30;//需要读取30个数据

	int failCount = 0;//记录通讯失败次数,判断通讯状态

	if (values.size() < data)
	{
		zlog_warn(Util::m_zlog, "读取dlt645-2007规约智能电表开始,设备地址=%s，数据越界",strAddr.c_str());
		return ErrorInfo::ERR_OUT_OF_RANGE;
	}

	int temp=0;

	DataTemp* pData = NULL;

	int nSleep = 5000000;//延时30ms

	//1.故障状态
	pData = (DataTemp*) values[1];
	if (pData != NULL)
	{
		pData->flag = false;
		pData->data = 0;
	}

	//2.系统频率
	pData = (DataTemp*) values[2];
	if (pData != NULL)
	{
		pData->flag = false;
		pData->data = 0;
	}

	//3.相电压V1(A相电压)
	ctrlCode[0] = 0x02;
	ctrlCode[1] = 0x01;
	ctrlCode[2] = 0x01;
	ctrlCode[3] = 0x00;

	regLength = 2;//返回字节长度
	zlog_info(Util::m_zlog, "读取相电压,设备地址=%s",strAddr.c_str());
	error = ReadData(regValue, strAddr, ctrlCode, ctrlCodeLength, regLength);
	zlog_info(Util::m_zlog, "读取相电压,设备地址=%s，error=%d",strAddr.c_str(),error);
	pData = (DataTemp*) values[3];

	if (pData != NULL)
	{
		if (error == ErrorInfo::ERR_OK)
		{
			pData->flag = true;
			pData->data = regValue;
		}
		else
		{
			pData->flag = false;
			pData->data = 0;
			failCount++;
		}
	}
	zlog_info(Util::m_zlog, "读取A相电压=%d,设备地址=%s ",pData->data,strAddr.c_str());

	usleep(nSleep);

	//4.相电压V2(B相电压)
	ctrlCode[0] = 0x02;
	ctrlCode[1] = 0x01;
	ctrlCode[2] = 0x02;
	ctrlCode[3] = 0x00;

	regLength = 2;//返回字节长度

	error = ReadData(regValue, strAddr, ctrlCode, ctrlCodeLength, regLength);

	pData = (DataTemp*) values[4];

	if (pData != NULL)
	{
		if (error == ErrorInfo::ERR_OK)
		{
			pData->flag = true;
			pData->data = regValue;
		}
		else
		{
			pData->flag = false;
			pData->data = 0;
			failCount++;
		}
	}
	zlog_info(Util::m_zlog, "读取B相电压=%d,设备地址=%s ",pData->data,strAddr.c_str());

	usleep(nSleep);

	//5.相电压V3(C相电压)
	ctrlCode[0] = 0x02;
	ctrlCode[1] = 0x01;////////
	ctrlCode[2] = 0x03;
	ctrlCode[3] = 0x00;

	regLength = 2;//返回字节长度

	error = ReadData(regValue, strAddr, ctrlCode, ctrlCodeLength, regLength);

	pData = (DataTemp*) values[5];

	if (pData != NULL)
	{
		if (error == ErrorInfo::ERR_OK)
		{
			pData->flag = true;
			pData->data = regValue;
		}
		else
		{
			pData->flag = false;
			pData->data = 0;
			failCount++;
		}
	}
	zlog_info(Util::m_zlog, "读取C相电压=%d,设备地址=%s ",pData->data,strAddr.c_str());

	usleep(nSleep);

	//6.相电流I1(A相电流)
	ctrlCode[0] = 0x02;
	ctrlCode[1] = 0x02;
	ctrlCode[2] = 0x01;
	ctrlCode[3] = 0x00;

	regLength = 3;//返回字节长度

	error = ReadData(regValue, strAddr, ctrlCode, ctrlCodeLength, regLength);

	pData = (DataTemp*) values[6];

	if (pData != NULL)
	{
		if (error == ErrorInfo::ERR_OK)
		{
			if(regValue>=800000)
			{
				regValue=0;
			}

			pData->flag = true;
			pData->data = regValue;
		}
		else
		{
			pData->flag = false;
			pData->data = 0;
			failCount++;
		}
	}
	zlog_info(Util::m_zlog, "读取A相电流=%d,设备地址=%s ",pData->data,strAddr.c_str());
	usleep(nSleep);

	//7.相电流I2(B相电流)
	ctrlCode[0] = 0x02;
	ctrlCode[1] = 0x02;
	ctrlCode[2] = 0x02;
	ctrlCode[3] = 0x00;

	regLength = 3;//返回字节长度

	error = ReadData(regValue, strAddr, ctrlCode, ctrlCodeLength, regLength);

	pData = (DataTemp*) values[7];

	if (pData != NULL)
	{
		if (error == ErrorInfo::ERR_OK)
		{
			if(regValue>=800000)
			{
				regValue=0;
			}
			pData->flag = true;
			pData->data = regValue;
		}
		else
		{
			pData->flag = false;
			pData->data = 0;
			failCount++;
		}
	}
	zlog_info(Util::m_zlog, "读取B相电流=%d,设备地址=%s ",pData->data,strAddr.c_str());
	usleep(nSleep);

	//8.相电流I3(C相电流)
	ctrlCode[0] = 0x02;
	ctrlCode[1] = 0x02;
	ctrlCode[2] = 0x03;
	ctrlCode[3] = 0x00;

	regLength = 3;//返回字节长度

	error = ReadData(regValue, strAddr, ctrlCode, ctrlCodeLength, regLength);

	pData = (DataTemp*) values[8];

	if (pData != NULL)
	{
		if (error == ErrorInfo::ERR_OK)
		{
			if(regValue>=800000)
			{
				regValue=0;
			}
			pData->flag = true;
			pData->data = regValue;
		}
		else
		{
			pData->flag = false;
			pData->data = 0;
			failCount++;
		}
	}
	zlog_info(Util::m_zlog, "读取C相电流=%d,设备地址=%s ",pData->data,strAddr.c_str());
	usleep(nSleep);

	//9.分相有功功率P1(瞬时A相有功功率)
	ctrlCode[0] = 0x02;
	ctrlCode[1] = 0x03;
	ctrlCode[2] = 0x01;
	ctrlCode[3] = 0x00;

	regLength = 3;//返回字节长度

	error = ReadData(regValue, strAddr, ctrlCode, ctrlCodeLength, regLength);
	pData = (DataTemp*) values[9];

	if (pData != NULL)
	{
		if (error == ErrorInfo::ERR_OK)
		{
			if(regValue>=800000)
			{
				regValue=0;
			}
			pData->flag = true;
			pData->data = regValue;
		}
		else
		{
			pData->flag = false;
			pData->data = 0;
			failCount++;
		}
	}
	zlog_info(Util::m_zlog, "读取A相有功功率=%d,设备地址=%s ",pData->data,strAddr.c_str());
	usleep(nSleep);

	//10.分相有功功率P2(瞬时B相有功功率)
	ctrlCode[0] = 0x02;
	ctrlCode[1] = 0x03;
	ctrlCode[2] = 0x02;
	ctrlCode[3] = 0x00;

	regLength = 3;//返回字节长度

	error = ReadData(regValue, strAddr, ctrlCode, ctrlCodeLength, regLength);

	pData = (DataTemp*) values[10];

	if (pData != NULL)
	{
		if (error == ErrorInfo::ERR_OK)
		{
			if(regValue>=800000)
			{
				regValue=0;
			}
			pData->flag = true;
			pData->data = regValue;
		}
		else
		{
			pData->flag = false;
			pData->data = 0;
			failCount++;
		}
	}
	zlog_info(Util::m_zlog, "读取B相有功功率=%d,设备地址=%s ",pData->data,strAddr.c_str());
	usleep(nSleep);

	//11.分相有功功率P3(瞬时C相有功功率)
	ctrlCode[0] = 0x02;
	ctrlCode[1] = 0x03;
	ctrlCode[2] = 0x03;
	ctrlCode[3] = 0x00;

	regLength = 3;//返回字节长度

	error = ReadData(regValue, strAddr, ctrlCode, ctrlCodeLength, regLength);

	pData = (DataTemp*) values[11];

	if (pData != NULL)
	{
		if (error == ErrorInfo::ERR_OK)
		{
			if(regValue>=800000)
			{
				regValue=0;
			}
			pData->flag = true;
			pData->data = regValue;
		}
		else
		{
			pData->flag = false;
			pData->data = 0;
			failCount++;
		}
	}
	zlog_info(Util::m_zlog, "读取C相有功功率=%d,设备地址=%s ",pData->data,strAddr.c_str());
	usleep(nSleep);

	//12.系统有功功率Psum(瞬时总有功功率)
	ctrlCode[0] = 0x02;
	ctrlCode[1] = 0x03;
	ctrlCode[2] = 0x00;
	ctrlCode[3] = 0x00;

	regLength = 3;//返回字节长度

	error = ReadData(regValue, strAddr, ctrlCode, ctrlCodeLength, regLength);

	pData = (DataTemp*) values[12];

	if (pData != NULL)
	{
		if (error == ErrorInfo::ERR_OK)
		{
			if(regValue>=800000)
			{
				regValue=0;
			}
			pData->flag = true;
			pData->data = regValue;
		}
		else
		{
			pData->flag = false;
			pData->data = 0;
			failCount++;
		}
	}
	zlog_info(Util::m_zlog, "读取总有功功率=%d,设备地址=%s ",pData->data,strAddr.c_str());
	usleep(nSleep);

	//13.分相无功功率Q1(瞬时A相无功功率)
	ctrlCode[0] = 0x02;
	ctrlCode[1] = 0x04;
	ctrlCode[2] = 0x01;
	ctrlCode[3] = 0x00;

	regLength = 3;//返回字节长度

	error = ReadData(regValue, strAddr, ctrlCode, ctrlCodeLength, regLength);
	pData = (DataTemp*) values[13];

	if (pData != NULL)
	{
		if (error == ErrorInfo::ERR_OK)
		{
			if(regValue>=800000)
			{
				regValue=0;
			}
			pData->flag = true;
			pData->data = regValue;
		}
		else
		{
			pData->flag = false;
			pData->data = 0;
			failCount++;
		}
	}
	zlog_info(Util::m_zlog, "读取A相无功功率=%d,设备地址=%s ",pData->data,strAddr.c_str());
	usleep(nSleep);

	//14.分相无功功率Q2(瞬时B相无功功率)
	ctrlCode[0] = 0x02;
	ctrlCode[1] = 0x04;
	ctrlCode[2] = 0x02;
	ctrlCode[3] = 0x00;

	regLength = 3;//返回字节长度

	error = ReadData(regValue, strAddr, ctrlCode, ctrlCodeLength, regLength);

	pData = (DataTemp*) values[14];

	if (pData != NULL)
	{
		if (error == ErrorInfo::ERR_OK)
		{
			if(regValue>=800000)
			{
				regValue=0;
			}
			pData->flag = true;
			pData->data = regValue;
		}
		else
		{
			pData->flag = false;
			pData->data = 0;
			failCount++;
		}
	}
	zlog_info(Util::m_zlog, "读取B相无功功率=%d,设备地址=%s ",pData->data,strAddr.c_str());
	usleep(nSleep);

	//15.分相无功功率Q3(瞬时C相无功功率)
	ctrlCode[0] = 0x02;
	ctrlCode[1] = 0x04;
	ctrlCode[2] = 0x03;
	ctrlCode[3] = 0x00;

	regLength = 3;//返回字节长度

	error = ReadData(regValue, strAddr, ctrlCode, ctrlCodeLength, regLength);

	pData = (DataTemp*) values[15];

	if (pData != NULL)
	{
		if (error == ErrorInfo::ERR_OK)
		{
			if(regValue>=800000)
			{
				regValue=0;
			}
			pData->flag = true;
			pData->data = regValue;
		}
		else
		{
			pData->flag = false;
			pData->data = 0;
			failCount++;
		}
	}
	zlog_info(Util::m_zlog, "读取C相无功功率=%d,设备地址=%s ",pData->data,strAddr.c_str());
	usleep(nSleep);

	//16.系统无功功率Qsum(瞬时总无功功率)
	ctrlCode[0] = 0x02;
	ctrlCode[1] = 0x04;
	ctrlCode[2] = 0x00;
	ctrlCode[3] = 0x00;

	regLength = 3;//返回字节长度

	error = ReadData(regValue, strAddr, ctrlCode, ctrlCodeLength, regLength);

	pData = (DataTemp*) values[16];

	if (pData != NULL)
	{
		if (error == ErrorInfo::ERR_OK)
		{
			if(regValue>=800000)
			{
				regValue=0;
			}
			pData->flag = true;
			pData->data = regValue;
		}
		else
		{
			pData->flag = false;
			pData->data = 0;
			failCount++;
		}
	}
	zlog_info(Util::m_zlog, "读取总无功功率=%d,设备地址=%s ",pData->data,strAddr.c_str());
	usleep(nSleep);

	//17.分相视在功率S1(瞬时A相视在功率)
	ctrlCode[0] = 0x02;
	ctrlCode[1] = 0x05;
	ctrlCode[2] = 0x01;
	ctrlCode[3] = 0x00;

	regLength = 3;//返回字节长度

	error = ReadData(regValue, strAddr, ctrlCode, ctrlCodeLength, regLength);

	pData = (DataTemp*) values[17];

	if (pData != NULL)
	{
		if (error == ErrorInfo::ERR_OK)
		{
			if(regValue>=800000)
			{
				regValue=0;
			}
			pData->flag = true;
			pData->data = regValue;
		}
		else
		{
			pData->flag = false;
			pData->data = 0;
			failCount++;
		}
	}
	zlog_info(Util::m_zlog, "读取A相视在功率=%d,设备地址=%s ",pData->data,strAddr.c_str());
	usleep(nSleep);

	//18.分相视在功率S2(瞬时B相视在功率)
	ctrlCode[0] = 0x02;
	ctrlCode[1] = 0x05;
	ctrlCode[2] = 0x02;
	ctrlCode[3] = 0x00;

	regLength = 3;//返回字节长度

	error = ReadData(regValue, strAddr, ctrlCode, ctrlCodeLength, regLength);

	pData = (DataTemp*) values[18];

	if (pData != NULL)
	{
		if (error == ErrorInfo::ERR_OK)
		{
			if(regValue>=800000)
			{
				regValue=0;
			}
			pData->flag = true;
			pData->data = regValue;
		}
		else
		{
			pData->flag = false;
			pData->data = 0;
			failCount++;
		}
	}
	zlog_info(Util::m_zlog, "读取B相视在功率=%d,设备地址=%s ",pData->data,strAddr.c_str());
	usleep(nSleep);

	//19.分相视在功率S3(瞬时C相视在功率)
	ctrlCode[0] = 0x02;
	ctrlCode[1] = 0x05;
	ctrlCode[2] = 0x03;
	ctrlCode[3] = 0x00;

	regLength = 3;//返回字节长度
	error = ReadData(regValue, strAddr, ctrlCode, ctrlCodeLength, regLength);
	pData = (DataTemp*) values[19];

	if (pData != NULL)
	{
		if (error == ErrorInfo::ERR_OK)
		{
			if(regValue>=800000)
			{
				regValue=0;
			}
			pData->flag = true;
			pData->data = regValue;
		}
		else
		{
			pData->flag = false;
			pData->data = 0;
			failCount++;
		}
	}
	zlog_info(Util::m_zlog, "读取C相视在功率=%d,设备地址=%s ",pData->data,strAddr.c_str());
	usleep(nSleep);

	//20.系统视在功率Ssum(瞬时总视在功率)
	ctrlCode[0] = 0x02;
	ctrlCode[1] = 0x05;
	ctrlCode[2] = 0x00;
	ctrlCode[3] = 0x00;

	regLength = 3;//返回字节长度
	error = ReadData(regValue, strAddr, ctrlCode, ctrlCodeLength, regLength);
	pData = (DataTemp*) values[20];

	if (pData != NULL)
	{
		if (error == ErrorInfo::ERR_OK)
		{
			if(regValue>=800000)
			{
				regValue=0;
			}
			pData->flag = true;
			pData->data = regValue;
		}
		else
		{
			pData->flag = false;
			pData->data = 0;
			failCount++;
		}
	}
	zlog_info(Util::m_zlog, "读取总视在功率=%d,设备地址=%s ",pData->data,strAddr.c_str());
	usleep(nSleep);

	//21.系统功率因数PF(总功率因数)
	ctrlCode[0] = 0x02;
	ctrlCode[1] = 0x06;
	ctrlCode[2] = 0x00;
	ctrlCode[3] = 0x00;

	regLength = 2;//返回字节长度

	error = ReadData(regValue, strAddr, ctrlCode, ctrlCodeLength, regLength);

	pData = (DataTemp*) values[21];

	if (pData != NULL)
	{
		if (error == ErrorInfo::ERR_OK)
		{
			if(regValue>=8000)
			{
				regValue-=8000;
				temp=(-1)*regValue;
			}
			else
			{
				temp=regValue;
			}

			pData->flag = true;
			pData->data = temp;
		}
		else
		{
			pData->flag = false;
			pData->data = 0;
			failCount++;
		}
	}
	zlog_info(Util::m_zlog, "读取总功率因数=%d,设备地址=%s ",pData->data,strAddr.c_str());
	//22.电压不平衡度
	pData = (DataTemp*) values[22];
	if (pData != NULL)
	{
			pData->flag = false;
			pData->data = 0;
	}

	//23.电流不平衡度
	pData = (DataTemp*) values[23];
	if (pData != NULL)
	{
			pData->flag = false;
			pData->data = 0;
	}

	//24.相或线电压平均总谐波畸变率
	pData = (DataTemp*) values[24];
	if (pData != NULL)
	{
			pData->flag = false;
			pData->data = 0;
	}
	//25.相或线电流平均总谐波畸变率
	pData = (DataTemp*) values[25];
	if (pData != NULL)
	{
			pData->flag = false;
			pData->data = 0;
	}

	//26.正向有功电度(正向有功总电能)
	ctrlCode[0] = 0x00;
	ctrlCode[1] = 0x01;
	ctrlCode[2] = 0x00;
	ctrlCode[3] = 0x00;

	regLength = 4;//返回字节长度

	error = ReadData(regValue, strAddr, ctrlCode, ctrlCodeLength, regLength);

	pData = (DataTemp*) values[26];

	if (pData != NULL)
	{
		if (error == ErrorInfo::ERR_OK)
		{
			if(regValue>=80000000)
			{
				regValue=0;
			}
			pData->flag = true;
			pData->data = regValue;
		}
		else
		{
			pData->flag = false;
			pData->data = 0;
			failCount++;
		}
	}
	zlog_info(Util::m_zlog, "读取正向有功总电能=%d,设备地址=%s ",pData->data,strAddr.c_str());
	usleep(nSleep);

	//27.反向有功电度(反向有功总电能)
	ctrlCode[0] = 0x00;
	ctrlCode[1] = 0x02;
	ctrlCode[2] = 0x00;
	ctrlCode[3] = 0x00;

	regLength = 4;//返回字节长度

	error = ReadData(regValue, strAddr, ctrlCode, ctrlCodeLength, regLength);

	pData = (DataTemp*) values[27];

	if (pData != NULL)
	{
		if (error == ErrorInfo::ERR_OK)
		{
			if(regValue>=80000000)
			{
				regValue=0;
			}
			pData->flag = true;
			pData->data = regValue;
		}
		else
		{
			pData->flag = false;
			pData->data = 0;
			failCount++;
		}
	}
	zlog_info(Util::m_zlog, "读取反向有功总电能=%d,设备地址=%s ",pData->data,strAddr.c_str());
	usleep(nSleep);

	//28.正向无功电度 ((当前)组合无功1 总电能)
	ctrlCode[0] = 0x00;
	ctrlCode[1] = 0x03;
	ctrlCode[2] = 0x00;
	ctrlCode[3] = 0x00;

	regLength = 4;//返回字节长度

	error = ReadData(regValue, strAddr, ctrlCode, ctrlCodeLength, regLength);

	pData = (DataTemp*) values[28];

	if (pData != NULL)
	{
		if (error == ErrorInfo::ERR_OK)
		{
			if(regValue>=80000000)
			{
				regValue=0;
			}
			pData->flag = true;
			pData->data = regValue;
		}
		else
		{
			pData->flag = false;
			pData->data = 0;
			failCount++;
		}
	}
	zlog_info(Util::m_zlog, "读取正向无功电度=%d,设备地址=%s ",pData->data,strAddr.c_str());
	usleep(nSleep);

	//29.反向无功电度 ((当前)组合无功2总电能)
	ctrlCode[0] = 0x00;
	ctrlCode[1] = 0x04;
	ctrlCode[2] = 0x00;
	ctrlCode[3] = 0x00;

	regLength = 4;//返回字节长度

	error = ReadData(regValue, strAddr, ctrlCode, ctrlCodeLength, regLength);

	pData = (DataTemp*) values[29];

	if (pData != NULL)
	{
		if (error == ErrorInfo::ERR_OK)
		{
			if(regValue>=80000000)
			{
				regValue=0;
			}
			pData->flag = true;
			pData->data = regValue;
		}
		else
		{
			pData->flag = false;
			pData->data = 0;

			failCount++;
		}
	}
	zlog_info(Util::m_zlog, "读取反向无功电度=%d,设备地址=%s ",pData->data,strAddr.c_str());
	usleep(nSleep);

	//0.通讯状态
	pData = (DataTemp*) values[0];
	if (pData != NULL)
	{
		if (failCount <= 11)//通讯失败次数小于通讯总次数一半即视为通讯正常
		{
			pData->flag = true;
			pData->data = 1;
			error=ErrorInfo::ERR_OK;
		}
		else
		{
			pData->flag = true;
			pData->data = 0;
			error=ErrorInfo::ERR_FAILED;
		}
	}

	return error;
}

