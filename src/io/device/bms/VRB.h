//    文 件       : 		VRB.h
//    功 能       : 		读取全钒液流电池 bms数据
//    作 者       : 		Mingyu Wu
//    version     : 		1.0.0
//    日 期       : 		2022-01-14


#ifndef VRB_H_
#define VRB_H_

#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"


class TcpClient;

class VRB{
	public:
		VRB();
		virtual ~VRB();
		
		void SetTcp(TcpClient* tcpClient);

		int Read(Device::SlaveDev *dev);
	private:
		TcpClient* m_tcp;

};



















#endif 


