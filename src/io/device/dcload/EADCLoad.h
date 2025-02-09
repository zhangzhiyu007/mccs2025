/**
 * *    文 件       :		EADCLoad.h
 * *    功 能       :		
 * *    作 者       :		Mingyu Wu
 * *    version     :		1.0.0
 * *    日 期       :		2021-04-01
 * */

#ifndef _EADCLOAD_H_
#define _EADCLOAD_H_

#include "../../../db/Device.h"
#include "../../../db/MemDb.h"


class TcpClient;

class EADCLoad{
	public:
		EADCLoad();
		virtual~EADCLoad();
		void SetTcp(TcpClient* tcpClient);
		int Read(Device::SlaveDev* dev);
		int Preset(Device::SlaveDev* dev);
		int ELR9500Read(Device::SlaveDev* dev);
		int ELR9500Preset(Device::SlaveDev* dev);
	private:
		TcpClient* m_tcpClient;
		float m_nominalVolt;
		float m_nominalCur;
		float m_nominalPow;
		float m_internalResMax;
		float m_internalResMin;
};

#endif
