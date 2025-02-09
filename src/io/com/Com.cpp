/*
 * Com.cpp
 *
 *   创建日期: 2013-4-8
 *   作         者: lht
 */

#include "Com.h"

#ifndef CFG_USART_LIB
#define CFG_USART_LIB 1
#endif

#include "libs_emfuture_odm.h"

Com::Com()
{
	// TODO 构造函数
	m_handle = INVALID_HANDLE;//串口句柄

	m_port = 1;//串口号
	m_baudrate = 9600;//波特率
	m_parity = 'N';//校验位,'N'无校验/'O'奇校验/'E'偶校验
	m_databits = 8;//数据位
	m_stopbits = 1;//停止位
	m_timeout = 2000;//超时时间ms
	//按缺省的属性初始化互斥体变量mutex
	pthread_mutex_init(&m_mutex, NULL);
}

Com::~Com()
{
	// TODO 析构函数
	//销毁互斥变量
	m_handle = INVALID_HANDLE;

	pthread_mutex_destroy(&m_mutex);
}

//设置串口号
void Com::SetPort(int port)
{
	this->m_port = port;
}

//读取串口号
int Com::GetPort()
{
	return this->m_port;
}

//设置串口参数
void Com::SetParam(int baudrate, int databits, char parity, int stopbits)
{
	this->m_baudrate = baudrate;
	this->m_databits = databits;
	this->m_parity = parity;
	this->m_stopbits = stopbits;
}

//获取串口参数
void Com::GetParam(int& baudrate,int& databits, char& parity, int& stopbits)
{
	baudrate = this->m_baudrate;
	databits = this->m_databits;
	parity = this->m_parity;
	stopbits = this->m_stopbits;
}

//设置超时时间
void Com::SetTimeout(int timeout)
{
	this->m_timeout = timeout;
}

//获取超时时间
int Com::GetTimeout()
{
	return this->m_timeout;
}

//加锁
void Com::Lock()
{
	// 给互斥体变量加锁
	pthread_mutex_lock(&m_mutex);
}

//解锁
void Com::Unlock()
{
	// 给互斥体变量解除锁
	pthread_mutex_unlock(&m_mutex);
}

//打开串口
bool Com::Open(int port, int baudrate, int databits, char parity, int stopbits)
{
	zlog_info(Util::m_zlog, "打开串口");

	//首先赋值给类变量
	this->m_port = port;
	this->m_baudrate = baudrate;
	this->m_databits = databits;
	this->m_parity = parity;
	this->m_stopbits = stopbits;

	bool isSuccess = Open();//串口是否打开成功

	return isSuccess;
}


//打开串口
bool Com::Open()
{
	zlog_info(Util::m_zlog, "打开串口");

	if(IsOpen())
	{
		zlog_warn(Util::m_zlog, "串口已经打开,将串口关闭，重新打开");
		Close();
	}

	char device[ARRAY_LENGTH_32];
	char config[ARRAY_LENGTH_128];

	bool isSuccess = true;//是否打开成功
	int ret = SUCCESS;//返回值

	//字符串初始化
	memset(device, '0', ARRAY_LENGTH_32);
	memset(config, '0', ARRAY_LENGTH_128);

	snprintf(device, ARRAY_LENGTH_32, "/dev/ttyS%d", m_port);
	snprintf(config, ARRAY_LENGTH_128, "%d,%d,%d,%c",
			this->m_baudrate,
			this->m_databits,
			this->m_stopbits,
			this->m_parity);

	//打开串口
	zlog_info(Util::m_zlog, "打开串口%d操作", m_port);
	ret = OpenCom(device, (int*)&m_handle, config);

	if(SUCCESS != ret)
	{
		isSuccess = false;
		zlog_warn(Util::m_zlog, "串口：%d打开失败", m_port);
	}
	else
	{
		zlog_warn(Util::m_zlog, "串口：%d打开成功", m_port);
	}

	return isSuccess;
}

//关闭串口
void Com::Close()
{
	zlog_info(Util::m_zlog, "关闭串口");

	//判断串口句柄是否存在
	if (m_handle == INVALID_HANDLE)
	{
		zlog_warn(Util::m_zlog, "串口没有打开");
	}
	else
	{
		//关闭句柄
		tcflush(m_handle, TCIOFLUSH);
		close(m_handle);
		m_handle = INVALID_HANDLE;
	}
}

//判断串口是否已经打开
bool Com::IsOpen()
{
	return (m_handle != INVALID_HANDLE);
}

//向串口读取数据
int Com::Read(String& dataRead, int len)
{
	zlog_info(Util::m_zlog, "从串口读取数据");

	fd_set	fRead;
	struct timeval tval;
	int ret = 0;
	int bufPos = 0;
	unsigned int bytesLeft = 0;

	char* buffer = new char[len+1];
	memset(buffer, 0, len+1);

	bytesLeft = len;

	dataRead.clear();

	while(true)
	{
		FD_ZERO(&fRead);
//		FD_CLR(m_handle, &fRead);
		FD_SET(m_handle,&fRead);

		tval.tv_sec = m_timeout / 1000;
		tval.tv_usec = (m_timeout % 1000) * 1000;

		//判断是否有数可读
		if(((ret = select(m_handle + 1, &fRead, NULL, NULL, &tval))== -1)||(!ret))
		{
			if(errno == EINTR)
			{
				//中断错误
				continue;
			}
			else
			{
				zlog_info(Util::m_zlog, "没数可读，结果=%d，错误码=%d", ret, errno);
				delete[] buffer;
				return 0;
			}
		}

		if(ret > 0)
		{
			if(FD_ISSET(m_handle, &fRead))
			{
				if(((ret = read(m_handle, buffer+bufPos, bytesLeft)) == -1)||(!ret))
				{
					m_handle = -1;
					zlog_warn(Util::m_zlog, "读取数据失败");
					delete[] buffer;
					return -1;
				}

				bufPos += ret;
				bytesLeft -= ret;

#if 0
				//242~257 8.22添加，针对串口数据有结束符的协议
				for(int i = 0; i < bufPos; i++)
				{
					if(buffer[i] == 0x0A)//此处为串口数据结束符
					{
						int j;
						for(j=0; j<bufPos; j++)
						{
							dataRead.push_back(buffer[j]);
						}

						delete[] buffer;
						return len;
					}
				}
#endif

				if(len == bufPos)
				{
					zlog_info(Util::m_zlog, "读取数据长度%d", len);
					int i;
					for(i=0; i<len; i++)
					{
						dataRead.push_back(buffer[i]);
					}

					delete[] buffer;
					return len;
				}
			}
		}
	}
	delete[] buffer;
	return 0;
}

int Com::Write(String& dataWrite)
{
	zlog_info(Util::m_zlog, "向串口写入数据,长度%d",dataWrite.size());

	size_t left = dataWrite.size();
	size_t done = 0;
	size_t res = 0;
	fd_set writings;
	struct timeval tval;
	int ret = 0;

	tval.tv_sec = m_timeout / 1000;
	tval.tv_usec = (m_timeout % 1000) * 1000;

	char* tmpDataWrite=new char[dataWrite.size()+1];
	memset(tmpDataWrite, 0, dataWrite.size()+1);
	int i;
	for (i = 0; i < dataWrite.size(); i++)
	{
		tmpDataWrite[i] = dataWrite[i];
	}

	while(true)
	{
		FD_ZERO(&writings);
		FD_SET(m_handle, &writings);

		if(((ret = select(m_handle + 1, NULL, &writings, NULL, &tval)) == -1)||(!ret))
		{
			if(errno == EINTR)
			{
				zlog_warn(Util::m_zlog, "写入数据时IO出错");
				continue;
			}
			else
			{
				delete[] tmpDataWrite;
				zlog_warn(Util::m_zlog, "缓冲区满");
				return -1;
			}
		}

		if(ret > 0)
		{
			if(FD_ISSET(m_handle, &writings))
			{
				zlog_info(Util::m_zlog, "向串口写入数据");

				if(((res = write(m_handle, tmpDataWrite+ done, left)) == -1)||(!res))
				{
					if(errno != EINTR)
					{
						delete[] tmpDataWrite;
						zlog_warn(Util::m_zlog, "向串口写入数据失败%d",errno);
						return -1;
					}
				}

	            done += res;
	            left -= res;

				if(dataWrite.size() == done)
				{
					delete[] tmpDataWrite;
					return 0;
				}
			}
		}
	}

	delete[] tmpDataWrite;

	return left == 0 ? 0 : -1;
}

int Com::TxRxMsg(String& dataRead, String& dataWrite, int len)
{
	zlog_info(Util::m_zlog, "句柄=%ld, 写入长度=%d, 读取长度=%d, 超时时间=%d.",m_handle,
			dataWrite.size(),
			len,
			m_timeout);

	int ret = 0;

	//判断串口句柄是否有效
	if(!IsOpen())
	{
		zlog_warn(Util::m_zlog, "串口没有打开");
		return ErrorInfo::ERR_NULL;
	}

	//写入串口
	Lock();//给互斥变量加锁
	//清除串口缓冲区数据
	tcflush(m_handle, TCIFLUSH);
	zlog_info(Util::m_zlog, "向串口写入数据");
	ret = Write(dataWrite);
	if(ret == -1)
	{
		this->Close();//关闭串口
		Unlock();
		zlog_warn(Util::m_zlog, "向串口写入时，串口发生错误");
		return ErrorInfo::ERR_WRITE;
	}

	//读取串口
	dataRead.clear();

	//读取串口数据
	zlog_info(Util::m_zlog, "读取串口数据");
	ret = Read(dataRead, len);
//	msleep(50);
	if(ret == -1)
	{
		this->Close();//关闭串口
		Unlock();
		zlog_warn(Util::m_zlog, "读取串口时，串口发生错误");
		return ErrorInfo::ERR_READ;
	}
	else if(ret == 0)
	{
		Unlock();
		zlog_info(Util::m_zlog, "没有读到数据");
		return ErrorInfo::ERR_NO_DATA;
	}
	else if(ret < 0)
	{
		Unlock();
		zlog_info(Util::m_zlog, "没有读到数据");
		return ErrorInfo::ERR_READ;
	}
	else if(ret!=len)
	{

		Unlock();
		zlog_info(Util::m_zlog, "读取数据数量不足");
		return ErrorInfo::ERR_INV_RESP;
	}

	Unlock();
	zlog_info(Util::m_zlog, "读写操作完成");

	return 0;
}


