/**
 * * @file FlyWheel.h
 * * @Synopsis  
 * * @author Mingyu Wu
 * * @version 
 * * @date 2022-03-03
 * */
#ifndef SOLIDLI_H
#define SOLIDLI_H

#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusTcpClient.h"
#include "../../../db/MemDb.h"



#include "../../../etc/socket/TcpClient.h"


class TcpClient;



class SolidLi{
	public:
		SolidLi();
		virtual ~SolidLi();

		void SetTcp(TcpClient* tcpClient);

		int Read(Device::SlaveDev *dev);

		int Preset(Device::SlaveDev* dev);

	private:

		TcpClient* m_tcp;

};
#endif


