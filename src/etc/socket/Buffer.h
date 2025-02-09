/*
 * Buffer.h
 *
 *   创建日期: 2013-3-26
 *   作         者: mzh
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include <errno.h>
#include <netdb.h>
#include <exception>
#include <string.h>
#include <string>
#include <netinet/in.h>
#include <limits.h>
#include <vector>

using std::string;
using std::exception;

#include "../../util/Util.h"


/*名称: Exception
 *描述:网络异常类
 *用法: 由Socket类调用
 *创建人:mzh
 *创建日期:2013-3-26
 */
class Exception: public exception
{
public:
	Exception(const string& _method, const string& _message, int _code) throw() :
		m_method(_method), m_message(_message), m_code(_code)
	{
	}

	virtual ~Exception() throw()
	{
	}

	const string& GetMethod() const throw()
	{
		return m_method;
	}
	const string& GetMessage() const throw()
	{
		return m_message;
	}
	int GetCode() const throw()
	{
		return m_code;
	}

	virtual const char* What() const throw()
	{
		return GetMessage().c_str();
	}

protected:
	string m_method; //抛出异常的方法
	string m_message; //异常错误信息
	int m_code; //错误码
};

/*名称: LibCException
 *描述:网络异常类
 *用法: 由Socket类调用
 *创建人:mzh
 *创建日期:2013-3-26
 */
class LibCException: public Exception
{
public:
	//错误信息
	LibCException(const string& method) throw() :
		Exception(m_method, strerror(errno), errno)
	{
	}
};

/*名称: HLibCException
 *描述:网络异常类
 *用法: 由Socket类调用
 *创建人:mzh
 *创建日期:2013-3-26
 */
class HLibCException: public Exception
{
public:
	//错误信息
	HLibCException(const string& method) throw() :
		Exception(m_method, hstrerror(h_errno), h_errno)
	{
	}
};

/*名称:NetInt32
 *描述:网络字节序到本地字节序的转换
 *用法: 由Socket类调用
 *创建人:mzh
 *创建日期:2013-3-26
 */
class NetInt32
{
public:
	NetInt32(uint32_t val = 0)
	{
		m_value = htonl(val);
	}

	uint32_t operator=(uint32_t val)
	{
		m_value = htonl(val);

		return val;
	}

	operator uint32_t() const
	{
		return ntohl(m_value);
	}

private:
	uint32_t m_value;
};

/*名称:NetInt16
 *描述:网络短字节序到本地字节序的转换
 *用法: 由Socket类调用
 *创建人:mzh
 *创建日期:2013-3-26
 */
class NetInt16
{
public:
	NetInt16(uint16_t val = 0)
	{
		m_value = htons(val);
	}

	uint16_t operator=(uint16_t val)
	{
		m_value = htons(val);
		return val;
	}

	operator uint16_t() const throw()
	{
		return ntohs(m_value);
	}

private:
	uint16_t m_value;
};

/*名称:Address
 *描述:保存socket的IP地址和端口等信息
 *用法: 由Socket类调用
 * string str="192.168.1.249"
 * Address addr(str,1000);
 *创建人:mzh
 *创建日期:2013-3-26
 */
class Address
{
public:
	//错误码
	enum ErrorCode
	{
		//无效IP地址
		NO_IP_CODE = 0x0101,
		//超出有效范围
		OFF_RANGE_CODE = 0x0102
	};

	//socket端口
	typedef in_port_t Port;

	//默认IP地址：00.00.00.00
	static const string ANY_IP;

	/*
	 * 名称: Address
	 * 功能: 构造函数，IP地址，端口等信息
	 * 输入:
	 * ip      :[in]const string& IP地址
	 * port    :[in]Port 端口
	 * lookup  :[in]bool 使能DNS查找功能
	 * 输出: 无explicit
	 */
	explicit Address(const string& ip = ANY_IP, Port port = 0, bool lookup =
			false) ;

	/*
	 * 名称: Address
	 * 功能: 构造函数，IP地址，端口等信息
	 * 输入:
	 * address      :[in]const sockaddr_in& IP地址
	 * 输出: 无
	 */
	Address(const sockaddr_in& address) ;

	/*
	 * 名称: 操作符
	 * 功能: 返回设备地址
	 * 输入:
	 * 输出: sockaddr_in
	 */
	operator sockaddr_in() const ;

	/*
	 * 名称: 操作符=
	 * 功能: 返回设备地址
	 * 输入: address    :[in]const sockaddr_in& ip信息
	 * 输出: 无
	 */
	void operator=(const sockaddr_in& address) ;

	/*
	 * 名称: lookup
	 * 功能: 查找第一个主机
	 * 输入: 无
	 * 输出: 成功/失败
	 */
	bool Lookup();

	/*
	 * 名称: GetName
	 * 功能: 不推荐使用,返回主机标准名称,必须首先调用Lookup查找
	 * 输入: 无
	 * 输出: const string 主机标准名称
	 */
	const string GetName()  ;

	/*
	 * 名称: GetNumAliases
	 * 功能: 返回主机别名数量
	 * 输入: 无
	 * 输出: int 返回主机别名数量
	 */
	int GetNumAliases() const;

	/*
	 * 名称: GetAliasName
	 * 功能: 不推荐使用,返回指定索引的主机的别名,超出范围返回0号索引
	 * 输入: index    :[in]size_t 索引
	 * 输出: const string&  指定索引的网络地址的别名
	 */
	const string& GetAliasName(size_t index) const;

	/*
	 * 名称: GetNumIPs
	 * 功能: 返回设备数量
	 * 输入: 无
	 * 输出: int  ip数量
	 */
	int GetNumIPs() const;

	/*
	 * 名称: GetIPString
	 * 功能: 返回设备指定的ip地址，索引超出范围返回0号索引
	 * 输入: index    :[in]size_t 索引
	 * 输出: string  ip地址，
	 */
	string GetIPString(size_t index = 0) const;

	/*
	 * 名称: GetPort
	 * 功能: 返回端口
	 * 输入: 无
	 * 输出: Port  端口
	 */
	Port GetPort() const;

private:
	typedef std::vector<string> StringVector;
	typedef std::vector<in_addr> IPVector;

	//主机规范名
	string m_name;//

	//主机别名数组
	StringVector m_aliases;

	//网络设备IP地址组
	IPVector m_ipAddresses;

	//端口
	Port m_port;

	//根据IP地址查找设备名称,保存到m_name中
	bool Lookup(const string& ip) ;
};

/*名称:Semaphore
 *描述:信号量同步
 *用法: 由Socket类调用
 *创建人:mzh
 *创建日期:2013-3-26
 */
class Semaphore
{
public:
	//初始化信号量计数为0
	Semaphore() ;
	~Semaphore() ;

	//增加信号量计数即 解锁unlock
	int operator++() ;
	//减小信号量计数及为进程或线程加锁
	int operator--();

	//非阻塞时减小信号量计数
	int operator-=(int dec);

	//返回信号量计数
	operator int();

private:
	int m_value;
	pthread_mutex_t m_mutex;
	pthread_cond_t m_cond;

	void MutexLock();
	void MutexUnlock();
	void CondWait();
	void CondSignal();
};

#endif /* BUFFER_H_ */
