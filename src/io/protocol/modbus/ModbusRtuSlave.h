/*
 * ModbusRtuSlave.h
 *
 *   创建日期: 2013-4-12
 *   作         者: mzh
 */

#ifndef MODBUSRTUSLAVE_H_
#define MODBUSRTUSLAVE_H_
#include "../../../util/Util.h"
#include "../../../etc/socket/Thread.h"
#include "../../com/Com.h"
#include "../../../db/MemDb.h"

/*名称:ModbusRtuSlave
 *描述: 实现Modbus RTU从站
 *1.接收到串口数据,进行数据校验,检查命令是否是Modbus命令,CRC校验
 *长度校验,命令和地址校验等
 *2.数据解析:将Modbus数据包解析
 *3.数据封装:封装成Modbu数据包格式
 *用法:
 *
 *
 *创建人:mzh
 *创建日期:2013-4-12
 */
class ModbusRtuSlave: public Thread
{
public:
	//保存ModbusRTU命令属性
	typedef struct ModbusRtu
	{
		int order;//命令
		int addr;//从设备地址
		int regStart;//寄存器起始地址
		int regLength;//寄存器长度
		UshortArray data;//表示寄存器值
	}ModbusRtu;

public:
	ModbusRtuSlave();
	virtual ~ModbusRtuSlave();

	//设置串口参数
	void SetCom(int port,int baudrate,int databits,char parity,int stopbits);
	//设置串口超时时间
	void SetTimeout(int timeout);

	//设置从设备地址
	void SetAddr(int addr);
	//寄存器起始地址
	void SetRegStart(int regStart);
	//寄存器结束地址
	void SetRegEnd(int regEnd);

protected:
private:

	/*
	 *  Run
	 *  功能: 线程执行函数
	 *  该线程内运行串口数据采集程序
	 *  输入：无
	 *  输出：无
	 */
    virtual void Run();

	/*
	 * 名称: Read
	 * 功能: 向串口读取数据
	 * 输入:
	 * dataRead      :[out]String& 从串口读取的数据
	 * 输出: 返回读取数据的实际长度，-1表示串口错误,0表示没有收到数据/>0表示读取到数据的长度
	 */
    int Read(String& data);


	/*
	 * 名称: UnpackModbus
	 * 功能: 将接收到的Modbus数据进行解析,保存到ModbusRtu&中
	 * 输入:
	 * modbus    :[out]ModbusRtu& Modbus数据
	 * data      :[in]String&  从串口中接收到的数据
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int UnpackModbus(ModbusRtu& modbus,String& data);

	/*
	 * 名称: PackModbus
	 * 功能: 将需要返回的ModbusRtu数据进行封装,保存data中
	 * 输入:
	 * data      :[out]String&  向串口发送的数据
	 * modbus    :[in]ModbusRtu& Modbus数据
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int PackModbus(String& data,ModbusRtu& modbus);

	/*
	 * 名称: PackErrorMessage,由
	 * 功能: 将需要错误信息数据进行封装,保存data中
	 * 输入:
	 * writeData   :[out]String&  向串口发送的数据
	 * readData    :[in]String& 从串口中接收到的数据
	 * errorCode :[in]int  判断错误类型
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int PackErrorMessage(String& writeData, String& readData, int errorCode);

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
	int Check(String& data);

	//变量
public:
protected:
private:

	//串口操作类
	Com m_com;

	int m_timeout;//超时时间

	//校验时使用
	int m_addr;//设备地址
	int m_regStart;//寄存器起始地址,用于标示寄存器最大地址范围,防止越界
	int m_regEnd;//寄存器结束地址
};

#endif /* MODBUSRTUSLAVE_H_ */
