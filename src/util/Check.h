/*
 * Check.h
 *
 *   创建日期: 2014-1-10
 *   作          者: 马中华
 */

#ifndef CHECK_H_
#define CHECK_H_
#include "Util.h"

/*名称:Check
 *描述:数据校验，集成常用校验方法
 *用法:
 *创建人:马中华
 *创建日期:2014-1-10
 */
class Check
{
public:
	Check();
	virtual ~Check();

    /*
     * 名称: Crc16
	 * 功能: CRC校验
	 * 输入：
	 * data      :String  校验字符串
	 * 输出：unsigned short 返回 CRC校验结果
	 */
	static unsigned short Crc16(String data);

	/*
	 * 名称：CheckSum
	 * 功能：艾诺公司CRC校验
	 * 输入：
	 * data		 :String 校验字符串
	 * 输出：unsigned short 返回校验结果
	 */
	static unsigned short CheckSum(String data);

protected:
private:
	//用于CRC16校验
	const static unsigned char m_crcHigh[256];
	const static unsigned char m_crcLower[256];
};

#endif /* CHECK_H_ */
