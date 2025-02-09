/*
 * ModbusTcpServer.h
 *
 *   创建日期: 2013-4-11
 *   作         者: lht
 */

#ifndef MODBUSTCPSERVER_H_
#define MODBUSTCPSERVER_H_
#include "../../../util/Util.h"
#include "../../../etc/socket/Thread.h"
#include "../../../etc/socket/TcpServer.h"
#include "../../../db/MemDb.h"
#include "ModbusTcpClient.h"

/*名称: ModbusTcpServer
 *描述: ModbusTcp服务器端即从站
 *1.接收到以太网数据,进行数据校验,检查命令是否是Modbus命令,
 *长度校验,命令和地址校验等
 *2.数据解析:将Modbus数据包解析
 *3.数据封装:封装成Modbu数据包格式
 *用法:
 *创建人: mzh
 *创建日期: 2013-4-11
 */
class ModbusTcpServer: public TcpService
{
public:
	typedef struct ModbusCheck
	{
		int addr;//设备地址
		int regStart;//寄存器起始地址,用于标示寄存器最大地址范围,防止越界
		int regEnd;//寄存器结束地址
	}ModbusCheck;

	//保存ModbusTcp命令属性
	typedef struct ModbusTcp
	{
		int firstFlag;//事务处理标识符，通常为0,从接收帧中复制
		int secondFlag;//协议标识符=0, 从接收帧中复制
		int order;//命令
		int addr;//从设备地址
		int regStart;//寄存器起始地址
		int regLength;//寄存器长度
		UshortArray data;//表示寄存器值
	} ModbusTcp;

public:
	ModbusTcpServer(Socket* sock,void* param);
	virtual ~ModbusTcpServer();

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
	 * 名称: UnpackModbus
	 * 功能: 将接收到的Modbus数据进行解析,保存到ModbusRtu&中
	 * 输入:
	 * data      :[in]String&  从以太网中接收到的数据
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int UnpackModbus(String& data);

	/*
	 * 名称: PackModbus
	 * 功能: 将需要返回的ModbusRtu数据进行封装,向以太网返回数据
	 * 输入:
	 * modbus    :[in]ModbusTcp& Modbus数据
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int PackModbus(ModbusTcp& modbus);

	/*
	 * 名称: PackErrorMessage,由
	 * 功能: 将需要错误信息数据进行封装,保存data中
	 * 输入:
	 * modbus    :[in]ModbusTcp& Modbus数据
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int PackErrorMessage(ModbusTcp& modbus);

	/*
	 * 名称: Check
	 * 功能: 对modbus命令进行校验,由UnpackModbus调用
	 * 1.判断是否是Modbus命令,不是不返回错误信息
	 * 2.数据长度校验:校验失败,不返回错误信息
	 * 3.地址和命令校验:校验失败,不返回错误信息
	 * 4.寄存器地址和长度越界校验:校验失败,返回错误信息1
	 * 输入:
	 * data      :[in]String&  待校验数据
	 * 输出: 返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
	int Check(String& data);

	//变量
public:
protected:
private:

	//校验时使用
	ModbusCheck* m_modbusCheck;

};

typedef TcpServiceFactoryTemplate<ModbusTcpServer> ModbusTcpFactory;

#endif /* MODBUSTCPSERVER_H_ */
