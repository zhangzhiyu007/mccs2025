/**
 * *    文 件       :		DCXG500FaultSimulator.h
 * *    功 能       :		
 * *    作 者       :		Mingyu Wu
 * *    version     :		1.0.0
 * *    日 期       :		2021-03-23
 * */

#ifndef _DCXG_FAULT_SIMULATOR_H
#define _DCXG_FAULT_SIMULATOR_H
#include "../../../db/Device.h"
#include "../../../db/MemDb.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"


class Com;
class TcpClient;

class DCXG500FaultSimulator{
	public:
		DCXG500FaultSimulator();
		virtual ~DCXG500FaultSimulator();
		void SetIsTcp(bool isTcp);
		void SetCom(Com* com);
		void SetTcp(TcpClient* client);

		int Read(Device::SlaveDev* dev);
		int Preset(Device::SlaveDev* dev);
	private:
		bool m_isTcp;
		Com* m_com;
		TcpClient* m_tcpClient;
};
#endif
