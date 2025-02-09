/*
 * Dlt645.cpp
 *
 *   创建日期: 2013-6-17
 *   作         者: mzh
 */

#include "Dlt645.h"
#include "../../com/Com.h"

Dlt645::Dlt645() :
	m_leadByte(0xFE)
{
	// TODO 构造函数
	m_leadBytes = 4; //前导字节数量,默认4字节，0xFE
	m_leadBytesResp = 0;//前导字节数量,默认0字节，0xFE
	m_com = NULL; //串口指针
	m_retries = 1; //查询失败，重复查询次数
	m_delayTime = 2000;//查询出错后，延时该时间，再进行下次查询，ms

}

Dlt645::~Dlt645()
{
	// TODO 析构函数
}

//设置/返回串口指针
void Dlt645::SetCom(Com* com)
{
	m_com = com;
}

//设置返回查询失败的重复查询次数
void Dlt645::SetRetries(int retries)
{
	m_retries = retries;
}

//设置延时时间
void Dlt645::SetDelayTime(int delayTime)
{
	m_delayTime = delayTime;
}

//设置应答数据的前导字节数量
void Dlt645::SetLeadBytesResp(int leadBytesResp)
{
	this->m_leadBytesResp = leadBytesResp;
}

//设置前导字节数量
void Dlt645::SetLeadBytes(int leadBytes)
{
	m_leadBytes = leadBytes;
}

inline BYTE Dlt645::Byte2Bcd(BYTE byte)
{
	BYTE result;
	result = (byte / 10) * 16 + byte % 10;
	return result;
}

inline BYTE Dlt645::Bcd2Byte(BYTE bcd)
{
	BYTE result;
	result = (bcd / 16) * 10 + bcd % 16;
	return result;
}

bool Dlt645::Ascii2Bcd(char *strDest, int destLen, char *strSrc, int srcLen)
{
	int i;

	/* 检查数据长度,源必须是目的的2倍且大于0 */
	if (destLen * 2 != srcLen || srcLen <= 0 || destLen <= 0)
	{
		return false;
	}
	/* 检查源字符串必须是'0'到'9'的字符 */
	for (i = 0; i < srcLen; i++)
	{
		if (*(strSrc + i) < '0' || *(strSrc + i) > '9')
		{
			fprintf(stderr, "invalid source string[%s]\n", strSrc);
			return false;
		}
	}
	for (i = 0; i < srcLen / 2; i++)
	{
		*(strDest + i) = (*(strSrc + i * 2) - '0') << 4;
		*(strDest + i) += *(strSrc + i * 2 + 1) - '0';
	}
	return true;
}

bool Dlt645::Bcd2Ascii(char *strDest, char *strSrc, int srcLen)
{
	short i;
	unsigned char highByte, lowByte;

	for (i = 0; i < srcLen; i++)
	{
		highByte = strSrc[i] >> 4;
		lowByte = strSrc[i] & 0x0F;

		highByte += 0x30;
		strDest[i * 2] = highByte;

		lowByte += 0x30;
		strDest[i * 2 + 1] = lowByte;
	}

	return true;
}

int Dlt645::ASC2BCD(unsigned char *bcd, const char *asc, int len, int fmt)
{
	int i;
	int odd;
	char c;

	odd = len & 0x01;

	if (odd && !fmt)
	{
		*bcd++ = (*asc++) & '\x0F';
	}

	len >>= 1;

	for (i = 0; i < len; i++)
	{
		c = (*asc++) << 4;
		c |= (*asc++) & '\x0F';

		*bcd++ = c;
	}

	if (odd && fmt)
	{
		*bcd = (*asc) << 4;
	}

	return (i + odd);
}

int Dlt645::BCD2ASC(char *asc, const unsigned char *bcd, int len, int fmt)
{
	int i, odd;
	unsigned char c;

	odd = len & 0x01;

	if (odd && !fmt)
	{
		*asc++ = ((*bcd++) & '\x0F') + '0';
	}

	len >>= 1;

	for (i = 0; i < len; i++)
	{
		c = *bcd++;
		*asc++ = (c >> 4) + '0';
		*asc++ = (c & '\x0F') + '0';
	}

	if (odd && fmt)
	{
		*asc = ((*bcd) >> 4) + '0';
	}

	return (i + odd);
}

int Dlt645::BCD2INT(int *plen, const unsigned char *bcd, int len, int fmt)
{
	int l = 0, i, odd;
	unsigned char c;

	odd = len & 0x01;

	if (odd && !fmt)
	{
		l = (*bcd++) & '\x0F';
	}

	len >>= 1;

	for (i = 0; i < len; i++)
	{
		c = *bcd++;
		l *= 10;
		l += c >> 4;
		l *= 10;
		l += c & '\x0F';
	}

	if (odd && fmt)
	{
		l *= 10;
		l += (*bcd) >> 4;
	}

	*plen = l;

	return (i + odd);
}

int Dlt645::TxRxMsg(String& dataRead, String& dataWrite, int len)
{
	int ret = 0;

	if (NULL == m_com)
	{
		return ErrorInfo::ERR_NULL;
	}
	ret = m_com->TxRxMsg(dataRead, dataWrite, len);

	return ret;
}
