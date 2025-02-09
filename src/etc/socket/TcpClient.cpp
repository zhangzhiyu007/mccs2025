/*
 * TcpClient.cpp
 *
 *   创建日期: 2013-3-27
 *   作         者: mzh
 */

#include "TcpClient.h"

TcpClient::TcpClient()
{
	// TODO 构造函数

}

TcpClient::~TcpClient()
{
	// TODO 析构函数
}

/*
 * 名称: Connect
 * 功能: 与服务器建立连接
 * 输入: addr    :[in]const Address& 服务器地址:IP/端口
 * 输出: true连接成功/false连接失败
 */
bool TcpClient::Connect(const Address& addr)
{
	zlog_info(Util::m_zlog, "与服务器建立连接");
	bool ret = false;

	//打开socket
	ret = this->Open(TcpClient::TCP);
	if(!ret)
	{
		zlog_warn(Util::m_zlog, "打开socket失败");
		return ret;
	}

	//建立连接
	ret = Socket::Connect(addr);
	if(!ret)
	{
		zlog_warn(Util::m_zlog, "建立连接失败,关闭socket");
		this->Close();
	}

	return ret;
}


