/*
 * TcpClient.h
 *
 *   创建日期: 2013-3-27
 *   作         者: mzh
 */

#ifndef TCPCLIENT_H_
#define TCPCLIENT_H_
#include "Socket.h"

/*名称: TcpClient
 *描述: TCP客户端程序,用于与服务器建立连接,
 *     发送和接受服务器数据
 *用法:
  1 连接服务器
    string ip("192.168.1.183");
	int port = 10000;
	string str("发送数据开始");
	Address addrServer(ip, port);
	//连接服务器
	TcpClient client;
	ret=client.Connect(addrServer);
  2 数据读写
  	StringBuffer dataSend(str);
	StringBuffer dataRecv;
    client.Send(dataSend);//发送
    num=client.Recv(dataRecv);//读取
   3 结束断开
	client.Close();
 *创建人:mzh
 *创建日期:2013-3-27
 */
class TcpClient :public Socket
{
public:
	TcpClient();
	virtual ~TcpClient();

	/*
	 * 名称: Connect
	 * 功能: 与服务器建立连接
	 * 输入: addr    :[in]const Address& 服务器地址:IP/端口
	 * 输出: true连接成功/false连接失败
	 */
    bool Connect(const Address& addr);

private:

};

#endif /* TCPCLIENT_H_ */
