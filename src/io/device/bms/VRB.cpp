#include "VRB.h"
#include <functional>

VRB::VRB(){
	m_tcp = NULL;
}

VRB::~VRB(){

}


void VRB::SetTcp(TcpClient *tcpClient){
	m_tcp = tcpClient;

}


int VRB::Read(Device::SlaveDev *dev){
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;

	UshortArray values;

	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;


	float fdata = 0.0;
	int idata = 0;
	bool status = 0;

	int length = 0;

	int start = 0;

	zlog_warn(Util::m_zlog, " 全钒液流读取线程\n");

	if(NULL == dev){
		zlog_warn(Util::m_zlog, "从设备为NULL");
		return ErrorInfo::ERR_NULL;

	}

	if(!m_tcp->IsOpened()){
		return ErrorInfo::ERR_OPENED;
	}


	ModbusRtuMaster rtu;

	length = 9;
	ret = rtu.ReadOutputRegisters(values, addr, start, length);
	if(ret == ErrorInfo::ERR_OK){
		comm = true;

		fdata = values[0] * 0.01; 			MemDb::SetRealData(regStart + 1, fdata, false);
		fdata = values[1] * 0.01; 			MemDb::SetRealData(regStart + 2, fdata, false);
		fdata = values[2] * 0.01; 			MemDb::SetRealData(regStart + 3, fdata, false);
		fdata = values[3] * 0.01; 			MemDb::SetRealData(regStart + 4, fdata, false);
		fdata = values[4] * 0.01; 			MemDb::SetRealData(regStart + 5, fdata, false);
		fdata = values[5] * 0.01; 			MemDb::SetRealData(regStart + 6, fdata, false);

		status  = (values[6] >> 0) & 0b1; 			MemDb::SetRealData(regStart + 7, status, false);
		status  = (values[6] >> 1) & 0b1; 			MemDb::SetRealData(regStart + 8, status, false);
		status  = (values[6] >> 2) & 0b1; 			MemDb::SetRealData(regStart + 9, status, false);
		status  = (values[6] >> 3) & 0b1; 			MemDb::SetRealData(regStart + 10, status, false);
		status  = (values[6] >> 4) & 0b1; 			MemDb::SetRealData(regStart + 11, status, false);
		status  = (values[6] >> 5) & 0b1; 			MemDb::SetRealData(regStart + 12, status, false);
		status  = (values[6] >> 6) & 0b1; 			MemDb::SetRealData(regStart + 13, status, false);
		status  = (values[6] >> 7) & 0b1; 			MemDb::SetRealData(regStart + 14, status, false);
		status  = (values[6] >> 8) & 0b1; 			MemDb::SetRealData(regStart + 15, status, false);
		status  = (values[6] >> 9) & 0b1; 			MemDb::SetRealData(regStart + 16, status, false);
		status  = (values[6] >> 10) & 0b1; 			MemDb::SetRealData(regStart + 17, status, false);
		status  = (values[6] >> 11) & 0b1; 			MemDb::SetRealData(regStart + 18, status, false);
		status  = (values[6] >> 12) & 0b1; 			MemDb::SetRealData(regStart + 19, status, false);
		status  = (values[6] >> 13) & 0b1; 			MemDb::SetRealData(regStart + 20, status, false);
		status  = (values[6] >> 14) & 0b1; 			MemDb::SetRealData(regStart + 21, status, false);
		status  = (values[6] >> 15) & 0b1; 			MemDb::SetRealData(regStart + 22, status, false);

		status  = (values[7] >> 0) & 0b1; 			MemDb::SetRealData(regStart + 23, status, false);
		status  = (values[7] >> 1) & 0b1; 			MemDb::SetRealData(regStart + 24, status, false);
		status  = (values[7] >> 2) & 0b1; 			MemDb::SetRealData(regStart + 25, status, false);
		status  = (values[7] >> 3) & 0b1; 			MemDb::SetRealData(regStart + 26, status, false);
		status  = (values[7] >> 4) & 0b1; 			MemDb::SetRealData(regStart + 27, status, false);
		status  = (values[7] >> 5) & 0b1; 			MemDb::SetRealData(regStart + 28, status, false);
		status  = (values[7] >> 6) & 0b1; 			MemDb::SetRealData(regStart + 29, status, false);
		status  = (values[7] >> 7) & 0b1; 			MemDb::SetRealData(regStart + 30, status, false);
		status  = (values[7] >> 8) & 0b1; 			MemDb::SetRealData(regStart + 31, status, false);
		status  = (values[7] >> 9) & 0b1; 			MemDb::SetRealData(regStart + 32, status, false);
		status  = (values[7] >> 10) & 0b1; 			MemDb::SetRealData(regStart + 33, status, false);
		status  = (values[7] >> 11) & 0b1; 			MemDb::SetRealData(regStart + 34, status, false);
		status  = (values[7] >> 12) & 0b1; 			MemDb::SetRealData(regStart + 35, status, false);
		status  = (values[7] >> 13) & 0b1; 			MemDb::SetRealData(regStart + 36, status, false);
		status  = (values[7] >> 14) & 0b1; 			MemDb::SetRealData(regStart + 37, status, false);
		status  = (values[7] >> 15) & 0b1; 			MemDb::SetRealData(regStart + 38, status, false);

		idata  = values[8]; 						MemDb::SetRealData(regStart + 39, idata, false);
	}
	else{
		comm = false;
		zlog_warn(Util::m_zlog, "%s 通信失败", dev->desc.c_str());

	}


	MemDb::SetRealData(regStart + 0, comm, false);

	return ret;
}



