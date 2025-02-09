/**
 * *    文 件       :		DCXGImpedanceSimulator.h
 * *    功 能       :		
 * *    作 者       :		Mingyu Wu
 * *    version     :		1.0.0
 * *    日 期       :		2021-03-21
 * */

#ifndef _DCXGImpedanceSimulator_H_
#define _DCXGImpedanceSimulator_H_

#include "../../../db/Device.h"
#include "../../../db/MemDb.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"

class Com;
class TcpClient;

class DCXGImpedanceSimulator{
	public:
		DCXGImpedanceSimulator();
		virtual ~DCXGImpedanceSimulator();

		void SetCom(Com* com);
		
		void SetTcp(TcpClient* tcpClient);

		void SetIsTcp(bool isTcp);

		int Read(Device::SlaveDev* dev);

		int Preset(Device::SlaveDev* dev);

		int DCXG500Read(Device::SlaveDev* dev);

		int DCXG500Preset(Device::SlaveDev* dev);



	
	private:
		Com* m_com;
		bool m_isTcp;
		TcpClient* m_tcpClient;

};

#endif
