
#ifndef VANA_H_
#define VANA_H_

#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"

class TcpClient;



class Vana{
	public:
		Vana();
		virtual ~Vana();


		void SetTcp(TcpClient* tcpClient);

		int Read(Device::SlaveDev* dev);
		int Preset(Device::SlaveDev* dev);
	private:
		TcpClient* m_tcpClient;




};




#endif
