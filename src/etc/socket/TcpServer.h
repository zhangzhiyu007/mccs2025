/*
 * TcpServer.h
 *
 *   创建日期: 2013-3-28
 *   作         者: mzh
 */

#ifndef TCPSERVER_H_
#define TCPSERVER_H_
#include "Socket.h"
#include "Thread.h"

/*名称: TcpServer
 *描述: 用于服务器端TCP通讯
 *用法:
 *创建人:mzh
 *创建日期:2013-3-28
 */
class TcpServer :public Socket
{
public:
	TcpServer();
	virtual ~TcpServer();

	/*
	 * 名称: Start
	 * 功能: 启动服务器
	 * 输入:
	 * port    :[in]const Address::Port 服务器地址端口,端口=0自动分配,默认所有IP监听
	 * queue   :[in]int 客户端排队最大数量
	 * 输出: true启动成功/false启动失败
	 */
	bool Start(const int port = 0, int queue = 5);

	/*
	 * 名称: Start
	 * 功能: 启动服务器
	 * 输入:
	 * addr    :[in]const Address& 服务器地址:IP/端口
	 * queue   :[in]int 客户端排队最大数量
	 * 输出: true启动成功/false启动失败
	 */
	bool Start(const Address& addr, int queue = 5);

	/*
	 * 名称: Stop
	 * 功能: 停止服务器
	 * 输入: 无
	 * 输出: true启动成功/false启动失败
	 */
	void Stop();

	/*
	 * 名称: GetConnetion
	 * 功能: 阻塞式等待客户端的连接
	 * 输入:
	 * addr    :[out]const Address& 客户端主机地址
	 * 输出: Socket* 新建连接的socket指针
	 */
	Socket* GetConnection(Address& addr);

	/*
	 * 名称: Wait4Connetion
	 * 功能: 超时时间内是否有新连接
	 * 输入:
	 * seconds    :[in]int 超时时间s
	 * useconds   :[in]int 超时时间us
	 * 输出:bool true在超时时间内有新建连接/false没有
	 */
	bool Wait4Connection(int seconds=0, int useconds=0);

protected:
private:

};

class SlaveManager;
class TcpServiceFactory;//工厂类
class TcpThreadedServer;//线程类

/*名称: TcpThreadedServer
 *描述: TCP线程服务类
 *用法:
 *创建人:mzh
 *创建日期:2013-3-28
 */
class TcpThreadedServer
{
public:
	TcpThreadedServer();
	virtual ~TcpThreadedServer();

	//设置从站管理线程
	void SetParam(void* param);

	/*
	 * 名称: Start
	 * 功能: 启动监听服务
	 * 输入:
	 * services    :[in]TCPServiceFactory& 监听工厂
	 * port        :[in]Address::Port 监听端口号
	 * 输出:bool true在超时时间内有新建连接/false没有
	 */
    void Start(TcpServiceFactory& services, Address::Port port = 0);

    //停止监听
    void Stop() ;

    //获得服务器的地址
    Address GetAddress() ;

    //返回客户端数量,包括已经终止的
    int GetClients() ;

protected:
	//监听线程
    class TcpListen: public Thread
    {
    public:
        TcpThreadedServer* m_server;
        bool m_listen;

        //线程运行方法
        void Run();
    };
    friend class TcpListen;
    friend class TcpService;

    void* m_param;//外部

    //监听线程
    TcpListen* m_listener;
    //监听服务器.
    TcpServer m_server;
    //TCP服务工厂
    TcpServiceFactory* m_serviceFactory;
    //监听端口
    Address::Port m_port;
    //信号量
    Semaphore m_counterSemaphore;
    //客户端数量
    int m_clientCounter;
private:

};

/*名称: TcpService
 *描述: TCP服务
 *用法:
 *创建人:mzh
 *创建日期:2013-3-28
 */
class TcpService : public Thread
{
public:
  TcpService(Socket* sock )   : m_socket(*sock) {}
  ~TcpService()   { delete &m_socket; }

protected:
	//新建连接
	Socket& m_socket;
};


/*名称: TcpServiceFactory
 *描述: TcpService类工厂
 *用法:
 *创建人:mzh
 *创建日期:2013-3-28
 */
class TcpServiceFactory
{
public:
  virtual ~TcpServiceFactory() {}

  //由TCPListen创建新的TCP服务
  virtual TcpService* CreateService(Socket* sock,void* param) = 0;
};

/*名称: TcpServiceFactoryTemplate
 *描述: TcpServiceFactoryTemplate工厂类模板
 *用法:
 * 1\继承自TCPService得TCP线程类
 * class TcpServiceThread:public TcpService
 * 该类实现新建与客户端连接的socket通讯
 * 2\
 * typedef TcpServiceFactoryTemplate<TcpServiceThread> TcpServiceThreadFactory;
 *创建人:mzh
 *创建日期:2013-3-28
 */
template<class SERVICE>
class TcpServiceFactoryTemplate : public TcpServiceFactory
{
public:
  virtual TcpService* CreateService(Socket* sock,void* param)
  {
    return new SERVICE(sock,param);
  }
};

#endif /* TCPSERVER_H_ */
