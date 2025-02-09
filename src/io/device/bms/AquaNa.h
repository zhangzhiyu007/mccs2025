///    文 件       : 		AquaNa.h
/////    功 能       : 		
/////    作 者       : 		Mingyu Wu
/////    version     : 		1.0.0
/////    日 期       : 		2022-01-12


#ifndef AQUANA_H_
#define AQUANA_H_

#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusTcpClient.h"
#include "../../../db/MemDb.h"



#include "../../../etc/socket/TcpClient.h"


class TcpClient;

class AquaNa{
	public:
		AquaNa();
		virtual ~AquaNa();

		void SetTcp(TcpClient* tcpClient);


		int Read(Device::SlaveDev* dev);

	private:

		// ==========================================================================
		///    功 能       : 		BattGroupRealTimeData 读取电池组体信息
		/////    Param       : 		dev 设备指针
		/////    Param       : 		regStart  
		/////    Param       : 		start 从设备寄存器地址
		/////    Returns     : 		
		//// ==========================================================================
		int BattGroupRealTimeData(Device::SlaveDev* dev, int regStart, int start);

		int DeviceStatusInfo(Device::SlaveDev* dev);

		int SingleBattInfo(Device::SlaveDev* dev, int regStart, int start, int singleBattCount);

		// 读取单体电池 告警信息
		// ==========================================================================
		///    功 能       : 		BattWarnInfo 读取单体电池级告警
		/////    Param       : 		value 
		/////    Param       : 		regStart
		//// ==========================================================================
		void  BattWarnInfo(unsigned short value, int  regStart);


		// ==========================================================================
		//
		//    功 能       : 		BattOtherStatus  读取单体电池其他状态 
		//    Param       : 		value
		//    Param       : 		regStart
		// ==========================================================================
		void BattOtherStatus(unsigned short value, int regStart);





		TcpClient* m_tcp;





};








#endif





