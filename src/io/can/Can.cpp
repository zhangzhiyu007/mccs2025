/*
 * Can.cpp
 *
 *   创建日期: 2013-4-8
 *   作         者: mzh
 */

#include "Can.h"

const int Can::m_frameLength = 8;//can帧数据信息长度字节固定为8

Can::Can()
{
	// TODO 构造函数
	m_handle = INVALID_HANDLE;
	m_port = 1;//CAN口号
	m_baudrate = 250;//波特率kbps
	m_timeout =10;//超时时间10ms
}

Can::~Can()
{
	// TODO 析构函数
	m_handle = INVALID_HANDLE;
}

//设置CAN口号
void Can::SetPort(int port)
{
	this->m_port = port;
}

//获取CAN口号
int Can::GetPort()
{
	return this->m_port;
}

//设置CAN口波特率
void Can::SetBaudrate(int baudrate)
{
	this->m_baudrate = baudrate;
}

//获取CAN口波特率
int Can::GetBaudrate()
{
	return this->m_baudrate;
}

//设置超时时间
void Can::SetTimeout(int timeout)
{
	this->m_timeout = timeout;
}

//获取超时时间
int Can::GetTimeout()
{
	return this->m_timeout;

}

//打开CAN口
bool Can::Open(int port, int baudrate)
{
	zlog_info(Util::m_zlog, "打开CAN %d口,波特率：%d",port,baudrate);

	bool isSuccess = true;//CAN是否打开成功

	this->m_port = port;//端口
	this->m_baudrate = baudrate;//波特率

	//打开CAN口
	isSuccess = Open();

	return isSuccess;
}

//打开CAN口
bool Can::Open()
{
	zlog_info(Util::m_zlog, "打开CAN口");

	struct sockaddr_can addr;
	int bytes = 0;
	struct ifreq ifr;

	if (IsOpen())
	{
		zlog_warn(Util::m_zlog,"CAN口已经打开，将CAN口关闭，重新打开");
		Close();
	}

	//初始化可能有问题？
	char device[ARRAY_LENGTH_128] =
	{ '\0' };
	char down[ARRAY_LENGTH_128] =
	{ '\0' };
	char up[ARRAY_LENGTH_128] =
	{ '\0' };
	char canName[ARRAY_LENGTH_32] =
	{ '\0' };

	//1.组装CAN参数
	snprintf(device, ARRAY_LENGTH_128, "/bin/ip link set can%d type can bitrate %d",
			this->m_port, this->m_baudrate*1000);
	snprintf(down, ARRAY_LENGTH_128, "/sbin/ifconfig can%d down", this->m_port);
	snprintf(up, ARRAY_LENGTH_128, "/sbin/ifconfig can%d up", this->m_port);
	snprintf(canName, ARRAY_LENGTH_32, "can%d", this->m_port);

	system(down);
	system(device);
	system(up);

	//2.创建套接字描述符
	m_handle = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (m_handle < 0)
	{
		m_handle = INVALID_HANDLE;
		zlog_warn(Util::m_zlog,"创建CAN%d socket失败",this->m_port);

		return false;
	}

	bytes = strlen(canName);//判断设备名长度
	if (bytes >= IFNAMSIZ)//如果设备名过长，则提示
	{
		Close();
		m_handle = INVALID_HANDLE;
		zlog_warn(Util::m_zlog,"can%d,device=%s太长,length=%d > %d 关闭CAN口",m_port,canName,bytes,IFNAMSIZ);

		return false;
	}

	addr.can_family = AF_CAN;
	memset(&ifr.ifr_name, 0, sizeof(ifr.ifr_name));
	strncpy(ifr.ifr_name, canName, bytes);//拷贝设备名

	if (ioctl(m_handle, SIOCGIFINDEX, &ifr) < 0)//把接口的索引存入ifr_ifrindex
	{
		Close();
		m_handle = INVALID_HANDLE;
		zlog_warn(Util::m_zlog,"can%d,ioctl ifr_ifindex 失败",m_port);

		return false;
	}
	addr.can_ifindex = ifr.ifr_ifindex;

	//绑定
	if (bind(m_handle, (struct sockaddr*) &addr, sizeof(addr)) < 0)//绑定
	{
		Close();
		m_handle = INVALID_HANDLE;
		zlog_warn(Util::m_zlog,"can%d，绑定失败",m_port);
		return false;
	}

	zlog_warn(Util::m_zlog,"can%d打开成功",m_port);

	return true;
}

//关闭CAN口
void Can::Close()
{
	zlog_info(Util::m_zlog,"关闭CAN口");

	if (m_handle == INVALID_HANDLE)
	{
		zlog_warn(Util::m_zlog,"CAN口句柄是无效句柄");
	}

	close(m_handle);

	m_handle = INVALID_HANDLE;
}

//判断CAN口是否打开
bool Can::IsOpen()
{
	zlog_info(Util::m_zlog,"判断CAN口是否打开");
	if (m_handle == INVALID_HANDLE)
	{
		return false;
	}

	return true;
}

//判断CAN口是否可读
int Can::IsReadable()
{
	zlog_info(Util::m_zlog,"判断CAN口是否有数据可读");

	struct timeval tval;
	tval.tv_sec = m_timeout/1000;
	tval.tv_usec = (m_timeout%1000)* 1000 ;

	fd_set	rdfs;
	int ret;
	FD_ZERO(&rdfs);//清空记录集
	FD_SET(m_handle,&rdfs);//把套接字描述符加入记录集中

	ret = select(m_handle+1,&rdfs,NULL,NULL,&tval);

	//可读
	if((ret >0)&&FD_ISSET(m_handle,&rdfs))
	{
		zlog_debug(Util::m_zlog,"缓冲区中有数据可读");

		return ret;
	}
	else
	{
		zlog_info(Util::m_zlog,"缓冲区没有数据可读");

		return ret;
	}
}

//从CAN口读取数据
int Can::Read(FrameInfo& dataRead)
{
	zlog_info(Util::m_zlog,"从can%d口读取数据",m_port);

	unsigned int bytes;
	int i;

	struct can_frame frameTemp;
	FrameType frameType = STANDARD_FRAME;

	memset(&frameTemp,0,sizeof(struct can_frame));

	bytes = read(m_handle, &frameTemp, sizeof(struct can_frame));

	//读取数据失败原因判断
	if (bytes < 0)
	{
		zlog_warn(Util::m_zlog,"读取端口错误，bytes=%d",bytes);
		return ErrorInfo::ERR_READ;
	}
	else if(bytes==0)//没有读到数据
	{
		zlog_warn(Util::m_zlog,"没有读到数据");
		return ErrorInfo::ERR_NO_DATA;
	}
	else if (bytes < sizeof(struct can_frame))//如果读到的数据帧小于can帧的结构，为不完整的数据帧
	{
		zlog_warn(Util::m_zlog,"读到的数据帧小于can帧的结构，bytes=%d",bytes);
		return ErrorInfo::ERR_INV_RESP;
	}
	else if (frameTemp.can_id & CAN_ERR_FLAG)//错误帧
	{
		zlog_debug(Util::m_zlog,"读入的是错误帧");

		frameTemp.can_id &= (CAN_ERR_MASK | CAN_ERR_FLAG);
		frameType = ERROR_FRAME;
	}
	else if (frameTemp.can_id & CAN_EFF_FLAG)//扩展帧
	{
		zlog_debug(Util::m_zlog,"读入的是扩展帧");

		frameTemp.can_id &= CAN_EFF_MASK;
		frameType = EXTENDED_FRAME;
	}
	else
	{
		zlog_debug(Util::m_zlog,"读入的是标准帧");

		frameTemp.can_id &= CAN_SFF_MASK;//标准帧
		frameType = STANDARD_FRAME;
	}//end of if (bytes < sizeof(struct can_frame))

	dataRead.id = frameTemp.can_id;
	dataRead.type = frameType;
	dataRead.data.clear();

	for (i = 0; i < frameTemp.can_dlc; i++)
	{
		dataRead.data.push_back(frameTemp.data[i]);
	}

	return ErrorInfo::ERR_OK;
}

//向CAN口写入数据
int Can::Write(FrameInfo& dataWrite)
{
//	zlog_info(Util::m_zlog,"handle=%ld,ID=%08X 向CAN口写入数据:%02X %02X %02X %02X %02X %02X %02X %02X",
//			m_handle,dataWrite.id,
//			dataWrite.data[0],dataWrite.data[1],
//			dataWrite.data[2],dataWrite.data[3],
//			dataWrite.data[4],dataWrite.data[5],
//			dataWrite.data[6],dataWrite.data[7]);
	zlog_info(Util::m_zlog,"handle=%ld,ID=%08X,字节长度=%d, 向CAN口写入数据",
			m_handle,dataWrite.id,dataWrite.data.size());

	int len = dataWrite.data.size();
	unsigned int bytes;
	int i;
	struct can_frame frameTemp;

	if (STANDARD_FRAME == dataWrite.type)//标准帧
	{
		zlog_warn(Util::m_zlog,"写入标准帧");

		frameTemp.can_id = dataWrite.id;
		frameTemp.can_id &= CAN_SFF_MASK;

		//帧长度处理
		if(len<=m_frameLength)
		{
			frameTemp.can_dlc = len;

			for (i = 0; i < len; i++)
			{
				frameTemp.data[i] = dataWrite.data[i];
				zlog_info(Util::m_zlog,"data[%d]=%02x",i,frameTemp.data[i]);
			}
		}
		else
		{
			frameTemp.can_dlc = m_frameLength;

			for (i = 0; i < m_frameLength; i++)
			{
				frameTemp.data[i] = dataWrite.data[i];
				zlog_info(Util::m_zlog,"data[%d]=%02x",i,frameTemp.data[i]);
			}
		}


	}
	else if (EXTENDED_FRAME == dataWrite.type)//扩展帧
	{
		zlog_info(Util::m_zlog,"写入扩展帧");

		frameTemp.can_id = dataWrite.id;

		//帧ID处理
		frameTemp.can_id &= CAN_EFF_MASK;
		frameTemp.can_id |= CAN_EFF_FLAG;

		//帧长度处理
		if(len<=m_frameLength)
		{
			frameTemp.can_dlc = len;

			for (i = 0; i < len; i++)
			{
				frameTemp.data[i] = dataWrite.data[i];
				zlog_info(Util::m_zlog,"data[%d]=%02x",i,frameTemp.data[i]);
			}
		}
		else
		{
			frameTemp.can_dlc = m_frameLength;

			for (i = 0; i < m_frameLength; i++)
			{
				frameTemp.data[i] = dataWrite.data[i];
				zlog_info(Util::m_zlog,"data[%d]=%02x",i,frameTemp.data[i]);
			}
		}
	}
	else
	{
		zlog_warn(Util::m_zlog,"handle=%ld,无效帧",m_handle);
		return ErrorInfo::ERR_WRITE;
	}

	zlog_info(Util::m_zlog,"向CAN写入数据");
	bytes = write(m_handle, &frameTemp, sizeof(struct can_frame));
	//数据发送错误原因判断
	if (bytes < 0)
	{
		zlog_warn(Util::m_zlog,"写入CAN口错误");
		return ErrorInfo::ERR_WRITE;
	}
	else if (bytes < sizeof(struct can_frame))
	{
		zlog_warn(Util::m_zlog,"写入数据太短");
		return ErrorInfo::ERR_INV_RESP;
	}

	zlog_info(Util::m_zlog,"写入数据成功");

	return ErrorInfo::ERR_OK;
}
