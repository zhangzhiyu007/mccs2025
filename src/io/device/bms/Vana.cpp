#include "Vana.h"

#include "../../../db/Device.h"
#include "../../protocol/modbus/ModbusRtuMaster.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"


Vana::Vana() {
	m_tcpClient = NULL;

}

Vana::~Vana(){


}

void Vana::SetTcp(TcpClient *tcpClient) {
	this->m_tcpClient = tcpClient;
}

int Vana::Read(Device::SlaveDev* dev) {
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 1;
	int length = 1;

	float f_data = 1.0f;
	int i_data = 0;


	// zlog_warn(Util::m_zlog, "%s 控制线程启动", dev->name.c_str());


	if (NULL == dev)
	{
		zlog_warn(Util::m_zlog,"从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	if (!m_tcpClient->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}
	
	ModbusRtuMaster rtu;
	rtu.SetIsTcp(true);
	rtu.SetTcpClient(this->m_tcpClient);
	UshortArray values;
	values.clear();

	start = 0;
	length = 14;

	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if(ErrorInfo::ERR_OK == ret) {
		comm = true;
		// 电池电压
		f_data = (float)(values[0] * 0.01); 		MemDb::SetRealData(regStart +1, f_data,false);

		// 电池电流
		f_data = (float)((signed short)values[1] * 0.01); 		MemDb::SetRealData(regStart +2, f_data,false);

		//电解液温度
		f_data = (float)(values[2] * 0.01); 		MemDb::SetRealData(regStart +3, f_data,false);

		// 荷电状态
		f_data = (float)(values[3] * 0.01); 		MemDb::SetRealData(regStart +4, f_data,false);

		// 1#电堆电压
		f_data = (float)(values[4] * 0.01); 		MemDb::SetRealData(regStart +5, f_data,false);

		// 2#电堆电压
		f_data = (float)(values[5] * 0.01); 		MemDb::SetRealData(regStart +6, f_data,false);


		// 系统报警字 
		// 电池电压高
		i_data = (values[6] >> 0) & 0b1; 						MemDb::SetRealData(regStart + 7, i_data, false);
		// 电池电流高
		i_data = (values[6] >> 1) & 0b1; 						MemDb::SetRealData(regStart + 8, i_data, false);
		// 电溶解液高
		i_data = (values[6] >> 2) & 0b1; 						MemDb::SetRealData(regStart + 9, i_data, false);
		// 1#漏夜
		i_data = (values[6] >> 3) & 0b1; 						MemDb::SetRealData(regStart + 10, i_data, false);
		// 2#漏夜
		i_data = (values[6] >> 4) & 0b1; 						MemDb::SetRealData(regStart + 11, i_data, false);
		// 1#电堆电压高
		i_data = (values[6] >> 5) & 0b1; 						MemDb::SetRealData(regStart + 12, i_data, false);
		// 2#电堆电压高
		i_data = (values[6] >> 6) & 0b1; 						MemDb::SetRealData(regStart + 13, i_data, false);
		// 正极管路压力高
		i_data = (values[6] >> 7) & 0b1; 						MemDb::SetRealData(regStart + 14, i_data, false);
		// 负极管路压力高
		i_data = (values[6] >> 8) & 0b1; 						MemDb::SetRealData(regStart + 15, i_data, false);
		// 急停按钮
		i_data = (values[6] >> 9) & 0b1; 						MemDb::SetRealData(regStart + 16, i_data, false);
		// 预留
		i_data = (values[6] >> 10) & 0b1; 						MemDb::SetRealData(regStart + 17, i_data, false);
		i_data = (values[6] >> 11) & 0b1; 						MemDb::SetRealData(regStart + 18, i_data, false);
		i_data = (values[6] >> 12) & 0b1; 						MemDb::SetRealData(regStart + 19, i_data, false);
		i_data = (values[6] >> 13) & 0b1; 						MemDb::SetRealData(regStart + 20, i_data, false);
		i_data = (values[6] >> 14) & 0b1; 						MemDb::SetRealData(regStart + 21, i_data, false);
		i_data = (values[6] >> 15) & 0b1; 						MemDb::SetRealData(regStart + 22, i_data, false);

		// 系统运行状态
		i_data  = (values[7] >> 0) & 0b1; 						MemDb::SetRealData(regStart + 23, i_data, false);
		// 预留
		i_data  = (values[7] >> 1) & 0b1; 						MemDb::SetRealData(regStart + 24, i_data, false);
		i_data  = (values[7] >> 2) & 0b1; 						MemDb::SetRealData(regStart + 25, i_data, false);
		i_data  = (values[7] >> 3) & 0b1; 						MemDb::SetRealData(regStart + 26, i_data, false);
		i_data  = (values[7] >> 4) & 0b1; 						MemDb::SetRealData(regStart + 27, i_data, false);
		i_data  = (values[7] >> 5) & 0b1; 						MemDb::SetRealData(regStart + 28, i_data, false);
		i_data  = (values[7] >> 6) & 0b1; 						MemDb::SetRealData(regStart + 29, i_data, false);
		i_data  = (values[7] >> 7) & 0b1; 						MemDb::SetRealData(regStart + 30, i_data, false);
		i_data  = (values[7] >> 8) & 0b1; 						MemDb::SetRealData(regStart + 31, i_data, false);
		i_data  = (values[7] >> 9) & 0b1; 						MemDb::SetRealData(regStart + 32, i_data, false);
		i_data  = (values[7] >> 10) & 0b1; 						MemDb::SetRealData(regStart + 33, i_data, false);
		i_data  = (values[7] >> 11) & 0b1; 						MemDb::SetRealData(regStart + 34, i_data, false);
		i_data  = (values[7] >> 12) & 0b1; 						MemDb::SetRealData(regStart + 35, i_data, false);
		i_data  = (values[7] >> 13) & 0b1; 						MemDb::SetRealData(regStart + 36, i_data, false);
		i_data  = (values[7] >> 14) & 0b1; 						MemDb::SetRealData(regStart + 37, i_data, false);
		i_data  = (values[7] >> 15) & 0b1; 						MemDb::SetRealData(regStart + 38, i_data, false);







	}
	else {
		comm = false;
		zlog_warn(Util::m_zlog, "读取 %s 失败", dev->name.c_str());
	}


	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态
	return ret;


}



int Vana::Preset(Device::SlaveDev *dev) {


	int ret = ErrorInfo::ERR_OK;
	int regStart = dev->regStart;
	int addr = atoi(dev->slaveAddr.c_str());
	RealData realData;


	ModbusRtuMaster rtu;
	rtu.SetIsTcp(true);
	rtu.SetTcpClient(this->m_tcpClient);

	// zlog_warn(Util::m_zlog, "%s 控制线程启动", dev->name.c_str());
	if(MemDb::GetRealData(realData, regStart + 39, true)) {
		// 邵老师要求，启动发送小于1秒的脉冲
		// 启动
		if(realData.pv.data.iValue == 1) {
			zlog_warn(Util::m_zlog, " %s 下发启动", dev->name.c_str());
			rtu.PresetSingleRegister(addr, 10, 1);
			msleep(1000);
			rtu.PresetSingleRegister(addr, 10, 0);
		}
		// 停止
		else if (realData.pv.data.iValue == 0) {
			rtu.PresetSingleRegister(addr, 10, 1 << 1);
			msleep(1000);
			rtu.PresetSingleRegister(addr, 10, 0);
			// rtu.PresetSingleRegister(addr, 10, 0);
		}
	} 


	// 故障复位
	if(MemDb::GetRealData(realData, regStart + 40, true)) {
		bool status = realData.pv.data.iValue;
		rtu.PresetSingleRegister(addr, 10, status << 2);
			msleep(1000);
			rtu.PresetSingleRegister(addr, 10, 0);

	}

	return ret;
}
