/*
 * Env.h
 *
 *   创建日期: 2013-6-24
 *   作         者: mzh
 */

#ifndef ENV_H_
#define ENV_H_

#include "../../../util/Util.h"
#include "../../../db/Device.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

/*名称:Env
 *描述:用于读取环境监测装置数据
 *用法:
 *创建人:mzh
 *创建日期:2013-6-24
 */
class Env
{
public:
	Env();
	virtual ~Env();

	//保存串口指针
	void SetCom(Com* com);

	//设置TcpClient指针
	void SetTcp(TcpClient* tcpClient);

	/*
	 *  EnvRead
	 *  功能: 读取环境监测装置数据
	 *
	 *  输入：
	 *  dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Read(Device::SlaveDev* dev);

protected:
private:

	/*
	 *  Jz322Read
	 *  功能: 读取锦州阳光环境监测装置数据,地址1，命令03，串口参数：19200/8/N/1
	 *  输入：dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Jz322Read(Device::SlaveDev* dev);

	/*
	 *  Pc4Read
	 *  功能: 读取锦州阳光环境监测装置数据,地址1，命令03，串口参数：9600/8/N/1
	 *  输入：dev    :Device::SlaveDev* 从设备指针
	 *  输出：返回错误码ErrorInfo,成功返回ErrorInfo::ERR_OK
	 */
    int Pc4Read(Device::SlaveDev* dev);

protected:
private:
    Com* m_com;
	TcpClient* m_tcpClient;//以太网，用于串口服务器

};

#endif /* ENV_H_ */
