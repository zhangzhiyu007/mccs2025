/*
 * Buffer.cpp
 *
 *   创建日期: 2013-3-26
 *   作         者: mzh
 */

#include "Buffer.h"
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../../util/Util.h"

//Semaphore
Semaphore::Semaphore()
{
	pthread_mutex_init(&m_mutex, 0);
	pthread_cond_init(&m_cond, 0);
	m_value = 0;
}

Semaphore::~Semaphore()
{
  pthread_mutex_destroy(&m_mutex);
  pthread_cond_destroy(&m_cond);
}

int Semaphore::operator++()
{
	int valueAfterOp;

	MutexLock();
	++m_value;
	valueAfterOp = m_value;
	MutexUnlock();

	CondSignal();

	return valueAfterOp;
}

int Semaphore::operator--()
{
	int valueAfterOp;

	MutexLock();
	while (m_value < 0)
	{
		CondWait();
	}

	--m_value;
	valueAfterOp = m_value;
	MutexUnlock();

	return valueAfterOp;
}

int Semaphore::operator-=(int dec)
{
	int valueAfterOp;

	MutexLock();
	m_value -= dec;
	valueAfterOp = m_value;
	MutexUnlock();

	return valueAfterOp;
}

Semaphore::operator int()
{
	int valueAfterOp;

	MutexLock();
	valueAfterOp = m_value;
	MutexUnlock();

	return valueAfterOp;
}

void Semaphore::MutexLock()
{
  int ret;
  ret = pthread_mutex_lock(&m_mutex);
  if(ret < 0)
  {
	  zlog_warn(Util::m_zlog, "信号量加锁失败");
  }
}

void Semaphore::MutexUnlock()
{
  int ret;
  ret = pthread_mutex_unlock(&m_mutex);
  if(ret < 0)
  {
	  zlog_warn(Util::m_zlog, "信号量解锁失败");
  }
}

void Semaphore::CondWait()
{
  int ret;
  ret = pthread_cond_wait(&m_cond, &m_mutex);
  if(ret < 0)
  {
	  zlog_warn(Util::m_zlog, "等待同步变量失败");
  }
}

void Semaphore::CondSignal()
{
  int ret;
  ret = pthread_cond_signal(&m_cond);
  if(ret < 0)
  {
	  zlog_warn(Util::m_zlog, "发送同步信号失败");
  }
}

const string Address::ANY_IP = "0.0.0.0";

Address::Address(const string& ip, Port _port, bool doLookup)
{
    in_addr address;

    if (doLookup)
    {
        Lookup(ip);
    }
    else
    {
        // 将字符串形式IP地址转化成网络字节序IP
        if (inet_aton(ip.c_str(), &address) == 0)
        {
            // 如果IP有问题查找一下,看看是不是已经保存了
            Lookup(ip);
        }
        else
        {
            //成功，保存
            m_ipAddresses.push_back(address);
        }
    }

    m_port = htons(_port);//转换短字节端口
}

Address::Address(const sockaddr_in & address)
{
	m_ipAddresses.push_back(address.sin_addr);

    m_port = address.sin_port;
}

Address::operator sockaddr_in() const
{
	sockaddr_in address;
	memset(&address,0,sizeof(sockaddr_in) );

	if (m_ipAddresses.empty())
	{
		return address;
	}

    address.sin_family = AF_INET;
    address.sin_port = m_port;
    address.sin_addr = m_ipAddresses.front();

    return address;
}

void Address::operator=(const sockaddr_in & address)
{
    m_name = "";
    m_aliases.clear();

    m_ipAddresses.clear();
    m_ipAddresses.push_back(address.sin_addr);

    m_port = address.sin_port;
}

bool Address::Lookup(const string & ip)
{
    hostent * hostinfo = NULL;

    //通过域名或者主机名得到主机
    hostinfo = gethostbyname(ip.c_str());

    if (hostinfo == NULL)
    {
        return false;
    }

    m_name = hostinfo->h_name;

    m_aliases.clear();
    // 保存设备名称
    if (hostinfo->h_aliases[0] != NULL)
    {
        for (char ** alias = hostinfo->h_aliases; *alias != NULL; alias++)
        {
        	m_aliases.push_back(*alias);
        }
    }

    m_ipAddresses.clear();
    //保存IP地址
    for (in_addr ** addr = reinterpret_cast<in_addr**> (hostinfo->h_addr_list); *addr
            != NULL; addr++)
    {
    	m_ipAddresses.push_back(**addr);
    }

    return true;
}

bool Address::Lookup()
{
    hostent * hostinfo = NULL;

    if (m_ipAddresses.empty())
    {
        return  false;
    }

    // just using first IP in list
    hostinfo = gethostbyaddr(reinterpret_cast<char*> (&m_ipAddresses.front()),
            sizeof(in_addr), AF_INET);

    if (hostinfo == NULL)
    {
        return false;
    }

    m_name = hostinfo->h_name;

    m_aliases.clear();

    if (hostinfo->h_aliases[0] != NULL)
    {
        for (char ** alias = hostinfo->h_aliases; *alias != NULL; alias++)
        {
        	m_aliases.push_back(*alias);
        }
    }

    m_ipAddresses.clear();

    for (in_addr ** addr = reinterpret_cast<in_addr**> (hostinfo->h_addr_list); *addr
            != NULL; addr++)
    {
    	m_ipAddresses.push_back(**addr);
    }
    return true;
}

const string  Address::GetName()
{
    return m_name;
}

int Address::GetNumAliases() const
{
    return m_aliases.size();
}

const string & Address::GetAliasName(size_t index) const
{
    if (index >= m_aliases.size())
    {
        return m_aliases[0];
    }

    return m_aliases[index];
}

int Address::GetNumIPs() const
{
    return m_ipAddresses.size();
}

string Address::GetIPString(size_t index) const
{
	string ip;
    if (index >= m_ipAddresses.size())
    {
    	ip = inet_ntoa(m_ipAddresses[0]);
    }
    else
    {
    	ip = inet_ntoa(m_ipAddresses[index]);
    }

    return ip;
}

Address::Port Address::GetPort() const
{
    return ntohs(m_port);
}
