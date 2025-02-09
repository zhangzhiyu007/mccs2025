/*
 * Dlt645_1997.h
 *
 *   创建日期: 2013-6-17
 *   作         者: mzh
 */

#ifndef DLT645_1997_H_
#define DLT645_1997_H_
#include "Dlt645.h"

/*名称:
 *描述:
 *用法:
 *创建人:mzh
 *创建日期:2013-6-17
 */
class Dlt645_1997:public Dlt645
{
public:
	Dlt645_1997();
	virtual ~Dlt645_1997();

	/*
	 * 名称: ReadRegisters
	 * 功能: 获取电表数据
	 * 输入: values : [out] PtrArray  如果没有错误情况下读取的数据，DataTemp*型
	 * 		 strAddr: [in]  string  设备地址
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int ReadRegisters(PtrArray& values, string strAddr);
private:
	/*
	 * 名称: Validate
	 * 功能: 数据校验
	 * 输入: abyQuery :     [in] ByteArray  查询数据
	 * 		 abyResponse:   [in]ByteArray  应答数据
	 *		 replyLength:[in]int	 应答数据长度
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int Validate(ByteArray& abyQuery, ByteArray& abyResponse, int replyLength);

	/*
	 * 名称: ReadData
	 * 功能: 读取电表数据
	 * 输入: regValue		:[out]UINT	 保存读取到的电表数据
	 * 		 strAddr	 	:[in] string 要设置的地址
	 *		 control		：[in]BYTE*	 控制码
	 *		 ctrlCodeLength	：[in]int    控制码长度
	 *		 regLength		：[in]int	 要读取的电表数据字节长度
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int ReadData(UINT& regValue,//读取属性值
			string strAddr,//地址域
			BYTE* control,//控制码
			int ctrlCodeLength,//控制码长度
			int regLength);//属性值字节数
};

#endif /* DLT645_1997_H_ */
