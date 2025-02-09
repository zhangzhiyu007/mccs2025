/*
 * Com.h
 *
 *   创建日期: 2013-4-8
 *   作         者: lht
 */

#ifndef COM_H_
#define COM_H_

#include "../../util/Util.h"

/*名称:Com
 *描述:实现串口通讯
 *用法:
 *创建人:mzh
 *创建日期:2013-4-8
 */
class Com
{
	//方法
public:
	Com();
	virtual ~Com();

	//设置/读取串口号
	void SetPort(int port);
	int GetPort();
	//设置/读取串口参数
	void SetParam(int baudrate,int databits,char parity,int stopbits);
	void GetParam(int& baudrate,int& databits,char& parity,int& stopbits);
	//设置/读取超时时间
	void SetTimeout(int timeout);
	int GetTimeout();

	/*
	 * 名称: TxRxMsg
	 * 功能: 用于在串口中读写数据，先发送查询数据，然后再读取数据，
	 *       不添加任何校验等
	 * 输入:
	 * dataRead      :[out]String& 从串口读取的数据
	 * dataWrite     :[in]String& 向串口写入的数据
	 * len			 :[in] int 期望从串口读取的数据的长度
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int TxRxMsg(String& dataRead,String& dataWrite, int len);

	/*
	 * 名称: Open
	 * 功能: 打开串口
	 * 输入:
	 * port        :[in]int  串口号
	 * baudrate    :[in]int  波特率 单位bps:1200/2400/4800/9600/19200/38400/57600等
	 * databits    :[in]int  数据位:7/8
	 * parity      :[in]char 校验位:'N'无校验/'O'奇校验/'E'偶校验
	 * stopbits    :[in]int  停止位：1/2
	 * 输出: true成功/false失败
	 */
	bool Open(int port, int baudrate = 9600, int databits = 8, char parity =
			'N', int stopbits = 1);

	/*
	 * 名称: Open
	 * 功能: 打开串口
	 * 输入: 无
	 * 输出: true成功/false失败
	 */
	bool Open();

	/*
	 * 名称: close
	 * 功能: 关闭串口
	 * 输入: 无
	 * 输出: 无
	 */
	void Close();

	/*
	 * 名称: IsComOpen
	 * 功能: 判断串口是否打开
	 * 输入: 无
	 * 输出: true串口已经打开/false串口关闭
	 */
	bool IsOpen();

	/*
	 * 名称: Read
	 * 功能: 向串口读取数据
	 * 输入:
	 * dataRead      :[out]String& 从串口读取的数据
	 * len			 :[in] int 期望从串口读取的数据的长度
	 * 输出: 返回读取数据的实际长度，-1表示串口错误,0表示没有收到数据/>0表示读取到数据的长度
	 */
	int Read(String& dataRead, int len);

	/*
	 * 名称: Write
	 * 功能: 向串口写入数据
	 * 输入:
	 * dataWrite      :[out]String& 向串口写入的数据
	 * 输出: -1表示发送失败,成功返回0
	 */
	int Write(String& dataWrite);


protected:
private:

	//加锁
	void Lock();
	//解锁
	void Unlock();

	//变量
public:
protected:
private:

	//串口句柄,串口描述符
	HANDLE m_handle;

	//串口通讯参数
	int m_port;//串口号
	int m_baudrate;//波特率
	char m_parity;//校验位,'N'无校验/'O'奇校验/'E'偶校验
	int m_databits;//数据位
	int m_stopbits;//停止位

	int m_timeout;//超时时间ms

	//同步
	pthread_mutex_t m_mutex;//互斥

};

#endif /* COM_H_ */
