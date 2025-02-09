/*
 * Dlt645.h
 *
 *   创建日期: 2013-6-17
 *   作         者: mzh
 */

#ifndef DLT645_H_
#define DLT645_H_
#include "../../../util/Util.h"

class Com;

/*名称:Dlt645
 *描述:645规约通用方法
 *用法:
 *创建人:mzh
 *创建日期:2013-6-17
 */
class Dlt645
{
public:
	//数据保存结构体
	typedef struct DataTemp
	{
		bool flag;
		UINT data;
	} DataTemp;
public:
	Dlt645();
	virtual ~Dlt645();

	//设置/返回串口指针
	void SetCom(Com* com);
	Com* GetCom();

	//设置返回查询失败的重复查询次数
	void SetRetries(int retries);
	int GetRetries()const;

	//设置应答数据的前导字节数量
	void SetLeadBytesResp(int leadBytesResp);

	//设置延时时间
	void SetDelayTime(int delayTime);
	//设置前导字节数量
	void SetLeadBytes(int leadBytes);

	/*
	 * 名称: Byte2Bcd
	 * 功能: 字节转化为压缩BCD码
	 * 输入: byte	:[in]BYTE	 要转换的值
	 * 输出: 返回转换后的BCD码
	 */
	BYTE Byte2Bcd(BYTE byte);

	/*
	 * 名称: Bcd2Byte
	 * 功能: 压缩BCD转化为字节
	 * 输入: bcd	:[in]BYTE	 要转换的值
	 * 输出: 返回转换后的字节码
	 */
	BYTE Bcd2Byte(BYTE bcd);

	/*
	 * 名称: Ascii2Bcd
	 * 功能: ASCII字符串转BCD码
	 * 输入: strDest	:[out]char*	 BCD目标字符串
	 *		 destLen:[in] int    目标字符串长度
	 *		 strSrc :[in] char*  ASCII源字符串
	 *		 srcLen :[in] int    源字符串长度
	 * 输出:返回成功或失败
	 */
	bool Ascii2Bcd(char *strDest, int destLen, char *strSrc, int srcLen);


	/*
	 * 名称: Bcd2Ascii
	 * 功能: BCD码转为ASCII字符串
	 * 输入: strDest	:[out]char*	 ACSII目标字符串
	 *		 strSrc :[in] char*  BCD源字符串
	 *		 srcLen :[in] int    源字符串长度
	 * 输出:返回成功或失败
	 */
	bool Bcd2Ascii(char *strDest, char *strSrc, int srcLen);

	/*
	 * 名称:ASC2BCD
	 * 功能：ASCII字符串转BCD码
	 * 输入：bcd: [out]unsigned char* 转化后BCD结果
	 * 		 asc: [in]const char* 需转化ASC字符串
	 * 		 len: [in]int 需转化长度
	 * 		 fmt: [in]int 转化后的格式，即长度为奇数时，前补还是后补零
	 * 输出：处理的BCD字节长度
	 */
	int ASC2BCD(unsigned char *bcd, const char *asc, int len, int fmt);

	/*
	 * 名称:BCD2ASC
	 * 功能：BCD码转为ASCII字符串
	 * 输入：asc: [out]char* 转化后ASC结果
	 * 		 bcd: [in]const unsigned char*	需转化BCD字符串
	 *		 len: [in]int需转化长度
	 * 		 fmt: [in]int需转化BCD的格式，即长度为奇数时，前补还是后补零
	 * 输出：处理的ASCII字节长度
	 */
	int BCD2ASC(char *asc, const unsigned char *bcd, int len, int fmt);

	/*
	 * 名称：BCD2INT
	 * 功能：BCD码转为ASCII字符串
	 * 输入：plen: [out]int*		转化后int结果指针
	 * 		 bcd: [in]const unsigned char* 	需转化BCD字符串
	 * 		 len: [in]int 需转化长度
	 * 		 fmt: [in]int 需转化BCD的格式，即长度为奇数时，前补还是后补零
	 * 输出：处理的ASCII字节长度
	 */
	int BCD2INT(int *plen, const unsigned char *bcd, int len, int fmt);

	/*
	 * 名称: TxRxMsg
	 * 功能: 用于在读写数据，先发送查询数据，然后再读取数据，
	 *       不添加任何校验等
	 * 输入:
	 * dataRead      :[out]String& 读取的数据
	 * dataWrite     :[in]String& 写入的数据
	 * len			 :[in] int 期望读取的数据的长度
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int TxRxMsg(String& dataRead, String& dataWrite, int len);

protected:

	const unsigned char m_leadByte;//前导字节，0xFE
	int m_leadBytes;         //前导字节数量,默认4字节，0xFE
	int m_leadBytesResp;     //应答数据前导字节
	Com* m_com;              //串口指针
	int m_retries;           //查询失败，重复查询次数
	int m_delayTime;//查询出错后，延时该时间，再进行下次查询，ms

private:
};

#endif /* DLT645_H_ */
