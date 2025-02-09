/*
 * Can.h
 *
 *   创建日期: 2013-4-8
 *   作         者: mzh
 */

#ifndef CAN_H_
#define CAN_H_

#include "../../util/Util.h"

/*名称:Can
 *描述:CAN接口通讯
 *用法:
 *创建人:mzh
 *创建日期:2013-4-8
 */
class Can
{
public:
	//CAN接口帧类型
	typedef enum FrameType
	{
	    STANDARD_FRAME = 1,   //标准帧
	    EXTENDED_FRAME,       //扩展帧
	    ERROR_FRAME,          //错误帧
	}FrameType;

	//帧信息
	typedef struct FrameInfo
	{
		int id;//ID
		String data;//数据
		FrameType type;//帧类型1表示标准帧，2表示扩展帧，3表示错误帧
	} FrameInfo;

	//方法
public:
	Can();
	virtual ~Can();

	//设置/读取CAN口号
	void SetPort(int port);
	int GetPort();
	//设置/读取CAN口参数
	void SetBaudrate(int baudrate);
	int GetBaudrate();
	//设置/读取超时时间
	void SetTimeout(int timeout);
	int GetTimeout();

	/*
	 * 名称: Open
	 * 功能: 打开CAN口
	 * 输入:
	 * port        :[in]int  CAN口号
	 * baudrate    :[in]int  波特率 单位kbps
	 * 输出: true成功/false失败
	 */
	bool Open(int port, int baudrate = 250);

	/*
	 * 名称: Open
	 * 功能: 打开CAN口
	 * 输入: 无
	 * 输出: true成功/false失败
	 */
	bool Open();

	/*
	 * 名称: close
	 * 功能: 关闭CAN口
	 * 输入: 无
	 * 输出: 无
	 */
	void Close();

	/*
	 * 名称: IsOpen
	 * 功能: 判断CAN口是否打开
	 * 输入: 无
	 * 输出: true CAN口已经打开/false CAN口关闭
	 */
	bool IsOpen();


	/*
	 * 名称: IsReadable
	 * 功能: 判断CAN口是否可读
	 * 输入: 无
	 * 输出: int 0表示没有数据,>0表示可读帧数
	 */
	int IsReadable();

	/*
	 * 名称: Read
	 * 功能: 从CAN口读取数据
	 * 输入:
	 * dataRead      :[out]FrameInfo& 从CAN读取的数据帧，必须是8bytes
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int Read(FrameInfo& dataRead);

	/*
	 * 名称: Write
	 * 功能: 向CAN口写入数据
	 * 输入:
	 * dataWrite      :[in]FrameInfo& 向CAN写入数据帧
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int Write(FrameInfo& dataWrite);
protected:
private:

	//变量
public:
	const static int m_frameLength;//can帧数据信息长度字节固定为8
protected:
private:
	//CAN口句柄,CAN口描述符,
	HANDLE m_handle;

	//CAN口通讯参数
	int m_port;//串口号
	int m_baudrate;//波特率

	int m_timeout;//超时时间ms
};

#endif /* CAN_H_ */
