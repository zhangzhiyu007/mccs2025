/**
 * * @file ZJNCLoad.h
 * * @Synopsis  
 * * @author Mingyu Wu
 * * @version 
 * * @date 2022-03-29
 * */

#ifndef _ZJNC_LOAD_H
#define _ZJNC_LOAD_H

#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"



class TcpClient;

class ZJNCLoad {
	public:
		ZJNCLoad();
		virtual ~ZJNCLoad();

		void SetTcp(TcpClient* tcpClient);

		int Read(Device::SlaveDev* dev);
		int DCLoad_a_Read(Device::SlaveDev* dev);
		int DCLoad_b_Read(Device::SlaveDev* dev);

		int Preset(Device::SlaveDev* dev);
		int DCLoad_a_Preset(Device::SlaveDev* dev);
		int DCLoad_b_Preset(Device::SlaveDev* dev);


	private:
		TcpClient* m_tcpClient;



};









#endif




