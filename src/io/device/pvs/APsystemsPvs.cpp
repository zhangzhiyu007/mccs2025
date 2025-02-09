/*
 * APsystemsPvs.cpp
 *
 *  创建日期: 2016-8-12
 *  作          者: 刘海涛
 */

#include "APsystemsPvs.h"
#include "../../com/Com.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../../util/Check.h"

APsystemsPvs::APsystemsPvs()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
	m_tcp = NULL;
}

APsystemsPvs::~APsystemsPvs()
{
	// TODO Auto-generated destructor stub
}

void APsystemsPvs::SetCom(Com* com)
{
	m_com = com;
}

//设置TcpClient指针
void APsystemsPvs::SetTcp(TcpClient* tcpClient)
{
	m_tcp = tcpClient;
}


int APsystemsPvs::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_APSYSTEMS_PVS:
		ret = this->APsystemsPvsRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int APsystemsPvs::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_APSYSTEMS_PVS:
		ret = this->APsystemsPreset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int APsystemsPvs::APsystemsPvsRead(Device::SlaveDev* dev)
{
	bool comm = false;
	int ret = ErrorInfo::ERR_OK;
	UshortArray values;
	values.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int error;
	float data = 0.0;
	int value = 0;

	String query;//查询命令数据
	String reply;////应答字符串

	query.clear();
	reply.clear();

	if (NULL == dev)
	{
		zlog_warn(Util::m_zlog,"从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	//判断串口是否打开
	if (!m_tcp->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	unsigned short crc16;

	//modbus message
	query.push_back((char)addr);
	query.push_back(0x02);//功能码
	query.push_back(0x00);//寄存器起始高字节
	query.push_back(0x00);//寄存器起始低字节
	query.push_back(0x00);//寄存器长度高字节
	query.push_back(0x01);//寄存器长度地字节

	crc16 = Check::Crc16(query);
	query.push_back((char)(crc16 & 0xFF));
	query.push_back((char)(crc16 >> 8)&0xFF);

	if(addr == 1)
	{
		error = m_tcp->TxRxMsg(reply, query, 181);
		if (error == ErrorInfo::ERR_OK)
		{
			comm = true;
			if(reply.size() == 31)
			{
				data = ((reply[10]<<24) | (reply[11]<<16) | (reply[12]<<8) | (reply[13])) * 0.01;
				MemDb::SetRealData(regStart + 10, data, false);//系统总功率
				data = ((reply[14]<<24) | (reply[15]<<16) | (reply[16]<<8) | (reply[17])) * 0.0000001;
				MemDb::SetRealData(regStart + 11, data, false);//系统当前发电量
				data = ((reply[18]<<24) | (reply[19]<<16) | (reply[20]<<8) | (reply[21])) * 0.1;
				MemDb::SetRealData(regStart + 12, data, false);//系统历史发电量
			}
			else if(reply.size() == 81)
			{
				data = ((reply[10]<<24) | (reply[11]<<16) | (reply[12]<<8) | (reply[13])) * 0.01;
				MemDb::SetRealData(regStart + 10, data, false);//系统总功率
				data = ((reply[14]<<24) | (reply[15]<<16) | (reply[16]<<8) | (reply[17])) * 0.0000001;
				MemDb::SetRealData(regStart + 11, data, false);//系统当前发电量
				data = ((reply[18]<<24) | (reply[19]<<16) | (reply[20]<<8) | (reply[21])) * 0.1;
				MemDb::SetRealData(regStart + 12, data, false);//系统历史发电量

				//1#微逆1通道
				data = ((reply[36]<<8) | (reply[37])) * 0.1;
				MemDb::SetRealData(regStart + 13, data, false);//直流电压
				data = ((reply[38]<<8) | (reply[39])) * 0.1;
				MemDb::SetRealData(regStart + 14, data, false);//直流电流
				data = ((reply[40]<<8) | (reply[41])) * 0.01;
				MemDb::SetRealData(regStart + 15, data, false);//输出功率
				data = ((reply[42]<<8) | (reply[43])) * 0.1;
				MemDb::SetRealData(regStart + 16, data, false);//电网频率
				data = (short)(reply[44]);
				MemDb::SetRealData(regStart + 17, data, false);//温度
				data = ((reply[45]<<8) | (reply[46]));
				MemDb::SetRealData(regStart + 18, data, false);//电网电压
				data = ((short)(reply[47])) * 0.1;
				MemDb::SetRealData(regStart + 19, data, false);//温度
				value = ((reply[48]<<8) | (reply[49]));
				MemDb::SetRealData(regStart + 20, value, false);//状态位
				data = ((reply[50]<<24) | (reply[51]<<16) | (reply[52]<<8) | (reply[53])) * 0.0000001;
				MemDb::SetRealData(regStart + 21, data, false);//输出电量

				//1#微逆2通道
				data = ((reply[61]<<8) | (reply[62])) * 0.1;
				MemDb::SetRealData(regStart + 22, data, false);//直流电压
				data = ((reply[63]<<8) | (reply[64])) * 0.1;
				MemDb::SetRealData(regStart + 23, data, false);//直流电流
				data = ((reply[65]<<8) | (reply[66])) * 0.01;
				MemDb::SetRealData(regStart + 24, data, false);//输出功率
				data = ((reply[67]<<8) | (reply[68])) * 0.1;
				MemDb::SetRealData(regStart + 25, data, false);//电网频率
				data = (short)(reply[69]);
				MemDb::SetRealData(regStart + 26, data, false);//温度
				data = ((reply[70]<<8) | (reply[71]));
				MemDb::SetRealData(regStart + 27, data, false);//电网电压
				data = ((short)(reply[72])) * 0.1;
				MemDb::SetRealData(regStart + 28, data, false);//电流
				value = ((reply[73]<<8) | (reply[74]));
				MemDb::SetRealData(regStart + 29, value, false);//状态位
				data = ((reply[75]<<24) | (reply[76]<<16) | (reply[77]<<8) | (reply[78])) * 0.0000001;
				MemDb::SetRealData(regStart + 30, data, false);//输出电量
			}
			else if(reply.size() == 131)
			{
				data = ((reply[10]<<24) | (reply[11]<<16) | (reply[12]<<8) | (reply[13])) * 0.01;
				MemDb::SetRealData(regStart + 10, data, false);//系统总功率
				data = ((reply[14]<<24) | (reply[15]<<16) | (reply[16]<<8) | (reply[17])) * 0.0000001;
				MemDb::SetRealData(regStart + 11, data, false);//系统当前发电量
				data = ((reply[18]<<24) | (reply[19]<<16) | (reply[20]<<8) | (reply[21])) * 0.1;
				MemDb::SetRealData(regStart + 12, data, false);//系统历史发电量

				//1#微逆1通道
				data = ((reply[36]<<8) | (reply[37])) * 0.1;
				MemDb::SetRealData(regStart + 13, data, false);//直流电压
				data = ((reply[38]<<8) | (reply[39])) * 0.1;
				MemDb::SetRealData(regStart + 14, data, false);//直流电流
				data = ((reply[40]<<8) | (reply[41])) * 0.01;
				MemDb::SetRealData(regStart + 15, data, false);//输出功率
				data = ((reply[42]<<8) | (reply[43])) * 0.1;
				MemDb::SetRealData(regStart + 16, data, false);//电网频率
				data = (short)(reply[44]);
				MemDb::SetRealData(regStart + 17, data, false);//温度
				data = ((reply[45]<<8) | (reply[46]));
				MemDb::SetRealData(regStart + 18, data, false);//电网电压
				data = ((short)(reply[47])) * 0.1;
				MemDb::SetRealData(regStart + 19, data, false);//温度
				value = ((reply[48]<<8) | (reply[49]));
				MemDb::SetRealData(regStart + 20, value, false);//状态位
				data = ((reply[50]<<24) | (reply[51]<<16) | (reply[52]<<8) | (reply[53])) * 0.0000001;
				MemDb::SetRealData(regStart + 21, data, false);//输出电量

				//1#微逆2通道
				data = ((reply[61]<<8) | (reply[62])) * 0.1;
				MemDb::SetRealData(regStart + 22, data, false);//直流电压
				data = ((reply[63]<<8) | (reply[64])) * 0.1;
				MemDb::SetRealData(regStart + 23, data, false);//直流电流
				data = ((reply[65]<<8) | (reply[66])) * 0.01;
				MemDb::SetRealData(regStart + 24, data, false);//输出功率
				data = ((reply[67]<<8) | (reply[68])) * 0.1;
				MemDb::SetRealData(regStart + 25, data, false);//电网频率
				data = (short)(reply[69]);
				MemDb::SetRealData(regStart + 26, data, false);//温度
				data = ((reply[70]<<8) | (reply[71]));
				MemDb::SetRealData(regStart + 27, data, false);//电网电压
				data = ((short)(reply[72])) * 0.1;
				MemDb::SetRealData(regStart + 28, data, false);//电流
				value = ((reply[73]<<8) | (reply[74]));
				MemDb::SetRealData(regStart + 29, value, false);//状态位
				data = ((reply[75]<<24) | (reply[76]<<16) | (reply[77]<<8) | (reply[78])) * 0.0000001;
				MemDb::SetRealData(regStart + 30, data, false);//输出电量

				//2#微逆1通道
				data = ((reply[86]<<8) | (reply[87])) * 0.1;
				MemDb::SetRealData(regStart + 31, data, false);//直流电压
				data = ((reply[88]<<8) | (reply[89])) * 0.1;
				MemDb::SetRealData(regStart + 32, data, false);//直流电流
				data = ((reply[90]<<8) | (reply[91])) * 0.01;
				MemDb::SetRealData(regStart + 33, data, false);//输出功率
				data = ((reply[92]<<8) | (reply[93])) * 0.1;
				MemDb::SetRealData(regStart + 34, data, false);//电网频率
				data = (short)(reply[94]);
				MemDb::SetRealData(regStart + 35, data, false);//温度
				data = ((reply[95]<<8) | (reply[96]));
				MemDb::SetRealData(regStart + 36, data, false);//电网电压
				data = ((short)(reply[97])) * 0.1;
				MemDb::SetRealData(regStart + 37, data, false);//温度
				value = ((reply[98]<<8) | (reply[99]));
				MemDb::SetRealData(regStart + 38, value, false);//状态位
				data = ((reply[100]<<24) | (reply[101]<<16) | (reply[102]<<8) | (reply[103])) * 0.0000001;
				MemDb::SetRealData(regStart + 39, data, false);//输出电量

				//2#微逆2通道
				data = ((reply[111]<<8) | (reply[112])) * 0.1;
				MemDb::SetRealData(regStart + 40, data, false);//直流电压
				data = ((reply[113]<<8) | (reply[114])) * 0.1;
				MemDb::SetRealData(regStart + 41, data, false);//直流电流
				data = ((reply[115]<<8) | (reply[116])) * 0.01;
				MemDb::SetRealData(regStart + 42, data, false);//输出功率
				data = ((reply[117]<<8) | (reply[118])) * 0.1;
				MemDb::SetRealData(regStart + 43, data, false);//电网频率
				data = (short)(reply[119]);
				MemDb::SetRealData(regStart + 44, data, false);//温度
				data = ((reply[120]<<8) | (reply[121]));
				MemDb::SetRealData(regStart + 45, data, false);//电网电压
				data = ((short)(reply[122])) * 0.1;
				MemDb::SetRealData(regStart + 46, data, false);//温度
				value = ((reply[123]<<8) | (reply[124]));
				MemDb::SetRealData(regStart + 47, value, false);//状态位
				data = ((reply[125]<<24) | (reply[126]<<16) | (reply[127]<<8) | (reply[128])) * 0.0000001;
				MemDb::SetRealData(regStart + 48, data, false);//输出电量
			}
			else if(reply.size() == 181)
			{
				data = ((reply[10] << 24) | (reply[11] << 16)
						| (reply[12] << 8) | (reply[13])) * 0.01;
				MemDb::SetRealData(regStart + 10, data, false);//系统总功率
				data = ((reply[14] << 24) | (reply[15] << 16)
						| (reply[16] << 8) | (reply[17])) * 0.0000001;
				MemDb::SetRealData(regStart + 11, data, false);//系统当前发电量
				data = ((reply[18] << 24) | (reply[19] << 16)
						| (reply[20] << 8) | (reply[21])) * 0.1;
				MemDb::SetRealData(regStart + 12, data, false);//系统历史发电量

				//1#微逆1通道
				data = ((reply[36] << 8) | (reply[37])) * 0.1;
				MemDb::SetRealData(regStart + 13, data, false);//直流电压
				data = ((reply[38] << 8) | (reply[39])) * 0.1;
				MemDb::SetRealData(regStart + 14, data, false);//直流电流
				data = ((reply[40] << 8) | (reply[41])) * 0.01;
				MemDb::SetRealData(regStart + 15, data, false);//输出功率
				data = ((reply[42] << 8) | (reply[43])) * 0.1;
				MemDb::SetRealData(regStart + 16, data, false);//电网频率
				data = (short) (reply[44]);
				MemDb::SetRealData(regStart + 17, data, false);//温度
				data = ((reply[45] << 8) | (reply[46]));
				MemDb::SetRealData(regStart + 18, data, false);//电网电压
				data = ((short) (reply[47])) * 0.1;
				MemDb::SetRealData(regStart + 19, data, false);//温度
				value = ((reply[48] << 8) | (reply[49]));
				MemDb::SetRealData(regStart + 20, value, false);//状态位
				data = ((reply[50] << 24) | (reply[51] << 16)
						| (reply[52] << 8) | (reply[53])) * 0.0000001;
				MemDb::SetRealData(regStart + 21, data, false);//输出电量

				//1#微逆2通道
				data = ((reply[61] << 8) | (reply[62])) * 0.1;
				MemDb::SetRealData(regStart + 22, data, false);//直流电压
				data = ((reply[63] << 8) | (reply[64])) * 0.1;
				MemDb::SetRealData(regStart + 23, data, false);//直流电流
				data = ((reply[65] << 8) | (reply[66])) * 0.01;
				MemDb::SetRealData(regStart + 24, data, false);//输出功率
				data = ((reply[67] << 8) | (reply[68])) * 0.1;
				MemDb::SetRealData(regStart + 25, data, false);//电网频率
				data = (short) (reply[69]);
				MemDb::SetRealData(regStart + 26, data, false);//温度
				data = ((reply[70] << 8) | (reply[71]));
				MemDb::SetRealData(regStart + 27, data, false);//电网电压
				data = ((short) (reply[72])) * 0.1;
				MemDb::SetRealData(regStart + 28, data, false);//电流
				value = ((reply[73] << 8) | (reply[74]));
				MemDb::SetRealData(regStart + 29, value, false);//状态位
				data = ((reply[75] << 24) | (reply[76] << 16)
						| (reply[77] << 8) | (reply[78])) * 0.0000001;
				MemDb::SetRealData(regStart + 30, data, false);//输出电量

				//2#微逆1通道
				data = ((reply[86] << 8) | (reply[87])) * 0.1;
				MemDb::SetRealData(regStart + 31, data, false);//直流电压
				data = ((reply[88] << 8) | (reply[89])) * 0.1;
				MemDb::SetRealData(regStart + 32, data, false);//直流电流
				data = ((reply[90] << 8) | (reply[91])) * 0.01;
				MemDb::SetRealData(regStart + 33, data, false);//输出功率
				data = ((reply[92] << 8) | (reply[93])) * 0.1;
				MemDb::SetRealData(regStart + 34, data, false);//电网频率
				data = (short) (reply[94]);
				MemDb::SetRealData(regStart + 35, data, false);//温度
				data = ((reply[95] << 8) | (reply[96]));
				MemDb::SetRealData(regStart + 36, data, false);//电网电压
				data = ((short) (reply[97])) * 0.1;
				MemDb::SetRealData(regStart + 37, data, false);//温度
				value = ((reply[98] << 8) | (reply[99]));
				MemDb::SetRealData(regStart + 38, value, false);//状态位
				data = ((reply[100] << 24) | (reply[101] << 16) | (reply[102]
						<< 8) | (reply[103])) * 0.0000001;
				MemDb::SetRealData(regStart + 39, data, false);//输出电量

				//2#微逆2通道
				data = ((reply[111] << 8) | (reply[112])) * 0.1;
				MemDb::SetRealData(regStart + 40, data, false);//直流电压
				data = ((reply[113] << 8) | (reply[114])) * 0.1;
				MemDb::SetRealData(regStart + 41, data, false);//直流电流
				data = ((reply[115] << 8) | (reply[116])) * 0.01;
				MemDb::SetRealData(regStart + 42, data, false);//输出功率
				data = ((reply[117] << 8) | (reply[118])) * 0.1;
				MemDb::SetRealData(regStart + 43, data, false);//电网频率
				data = (short) (reply[119]);
				MemDb::SetRealData(regStart + 44, data, false);//温度
				data = ((reply[120] << 8) | (reply[121]));
				MemDb::SetRealData(regStart + 45, data, false);//电网电压
				data = ((short) (reply[122])) * 0.1;
				MemDb::SetRealData(regStart + 46, data, false);//温度
				value = ((reply[123] << 8) | (reply[124]));
				MemDb::SetRealData(regStart + 47, value, false);//状态位
				data = ((reply[125] << 24) | (reply[126] << 16) | (reply[127]
						<< 8) | (reply[128])) * 0.0000001;
				MemDb::SetRealData(regStart + 48, data, false);//输出电量

				//3#微逆1通道
				data = ((reply[136] << 8) | (reply[137])) * 0.1;
				MemDb::SetRealData(regStart + 49, data, false);//直流电压
				data = ((reply[138] << 8) | (reply[139])) * 0.1;
				MemDb::SetRealData(regStart + 50, data, false);//直流电流
				data = ((reply[140] << 8) | (reply[141])) * 0.01;
				MemDb::SetRealData(regStart + 51, data, false);//输出功率
				data = ((reply[142] << 8) | (reply[143])) * 0.1;
				MemDb::SetRealData(regStart + 52, data, false);//电网频率
				data = (short) (reply[144]);
				MemDb::SetRealData(regStart + 53, data, false);//温度
				data = ((reply[145] << 8) | (reply[146]));
				MemDb::SetRealData(regStart + 54, data, false);//电网电压
				data = ((short) (reply[147])) * 0.1;
				MemDb::SetRealData(regStart + 55, data, false);//温度
				value = ((reply[148] << 8) | (reply[149]));
				MemDb::SetRealData(regStart + 56, value, false);//状态位
				data = ((reply[150] << 24) | (reply[151] << 16) | (reply[152]
						<< 8) | (reply[153])) * 0.0000001;
				MemDb::SetRealData(regStart + 57, data, false);//输出电量

				//3#微逆2通道
				data = ((reply[161] << 8) | (reply[162])) * 0.1;
				MemDb::SetRealData(regStart + 58, data, false);//直流电压
				data = ((reply[163] << 8) | (reply[164])) * 0.1;
				MemDb::SetRealData(regStart + 59, data, false);//直流电流
				data = ((reply[165] << 8) | (reply[166])) * 0.01;
				MemDb::SetRealData(regStart + 60, data, false);//输出功率
				data = ((reply[167] << 8) | (reply[168])) * 0.1;
				MemDb::SetRealData(regStart + 61, data, false);//电网频率
				data = (short) (reply[169]);
				MemDb::SetRealData(regStart + 62, data, false);//温度
				data = ((reply[170] << 8) | (reply[171]));
				MemDb::SetRealData(regStart + 63, data, false);//电网电压
				data = ((short) (reply[172])) * 0.1;
				MemDb::SetRealData(regStart + 64, data, false);//温度
				value = ((reply[173] << 8) | (reply[174]));
				MemDb::SetRealData(regStart + 65, value, false);//状态位
				data = ((reply[175] << 24) | (reply[176] << 16) | (reply[177]
						<< 8) | (reply[178])) * 0.0000001;
				MemDb::SetRealData(regStart + 66, data, false);//输出电量
			}
		}
	}
	else if(addr == 2)
	{
		error = m_tcp->TxRxMsg(reply, query, 131);
		if (error == ErrorInfo::ERR_OK)
		{
			comm = true;
			if(reply.size() == 31)
			{
				data = ((reply[10]<<24) | (reply[11]<<16) | (reply[12]<<8) | (reply[13])) * 0.01;
				MemDb::SetRealData(regStart + 10, data, false);//系统总功率
				data = ((reply[14]<<24) | (reply[15]<<16) | (reply[16]<<8) | (reply[17])) * 0.0000001;
				MemDb::SetRealData(regStart + 11, data, false);//系统当前发电量
				data = ((reply[18]<<24) | (reply[19]<<16) | (reply[20]<<8) | (reply[21])) * 0.1;
				MemDb::SetRealData(regStart + 12, data, false);//系统历史发电量
			}
			else if(reply.size() == 81)
			{
				data = ((reply[10]<<24) | (reply[11]<<16) | (reply[12]<<8) | (reply[13])) * 0.01;
				MemDb::SetRealData(regStart + 10, data, false);//系统总功率
				data = ((reply[14]<<24) | (reply[15]<<16) | (reply[16]<<8) | (reply[17])) * 0.0000001;
				MemDb::SetRealData(regStart + 11, data, false);//系统当前发电量
				data = ((reply[18]<<24) | (reply[19]<<16) | (reply[20]<<8) | (reply[21])) * 0.1;
				MemDb::SetRealData(regStart + 12, data, false);//系统历史发电量

				//1#微逆1通道
				data = ((reply[36]<<8) | (reply[37])) * 0.1;
				MemDb::SetRealData(regStart + 13, data, false);//直流电压
				data = ((reply[38]<<8) | (reply[39])) * 0.1;
				MemDb::SetRealData(regStart + 14, data, false);//直流电流
				data = ((reply[40]<<8) | (reply[41])) * 0.01;
				MemDb::SetRealData(regStart + 15, data, false);//输出功率
				data = ((reply[42]<<8) | (reply[43])) * 0.1;
				MemDb::SetRealData(regStart + 16, data, false);//电网频率
				data = (short)(reply[44]);
				MemDb::SetRealData(regStart + 17, data, false);//温度
				data = ((reply[45]<<8) | (reply[46]));
				MemDb::SetRealData(regStart + 18, data, false);//电网电压
				data = ((short)(reply[47])) * 0.1;
				MemDb::SetRealData(regStart + 19, data, false);//温度
				value = ((reply[48]<<8) | (reply[49]));
				MemDb::SetRealData(regStart + 20, value, false);//状态位
				data = ((reply[50]<<24) | (reply[51]<<16) | (reply[52]<<8) | (reply[53])) * 0.0000001;
				MemDb::SetRealData(regStart + 21, data, false);//输出电量

				//1#微逆2通道
				data = ((reply[61]<<8) | (reply[62])) * 0.1;
				MemDb::SetRealData(regStart + 22, data, false);//直流电压
				data = ((reply[63]<<8) | (reply[64])) * 0.1;
				MemDb::SetRealData(regStart + 23, data, false);//直流电流
				data = ((reply[65]<<8) | (reply[66])) * 0.01;
				MemDb::SetRealData(regStart + 24, data, false);//输出功率
				data = ((reply[67]<<8) | (reply[68])) * 0.1;
				MemDb::SetRealData(regStart + 25, data, false);//电网频率
				data = (short)(reply[69]);
				MemDb::SetRealData(regStart + 26, data, false);//温度
				data = ((reply[70]<<8) | (reply[71]));
				MemDb::SetRealData(regStart + 27, data, false);//电网电压
				data = ((short)(reply[72])) * 0.1;
				MemDb::SetRealData(regStart + 28, data, false);//电流
				value = ((reply[73]<<8) | (reply[74]));
				MemDb::SetRealData(regStart + 29, value, false);//状态位
				data = ((reply[75]<<24) | (reply[76]<<16) | (reply[77]<<8) | (reply[78])) * 0.0000001;
				MemDb::SetRealData(regStart + 30, data, false);//输出电量
			}
			else if(reply.size() == 131)
			{
				data = ((reply[10]<<24) | (reply[11]<<16) | (reply[12]<<8) | (reply[13])) * 0.01;
				MemDb::SetRealData(regStart + 10, data, false);//系统总功率
				data = ((reply[14]<<24) | (reply[15]<<16) | (reply[16]<<8) | (reply[17])) * 0.0000001;
				MemDb::SetRealData(regStart + 11, data, false);//系统当前发电量
				data = ((reply[18]<<24) | (reply[19]<<16) | (reply[20]<<8) | (reply[21])) * 0.1;
				MemDb::SetRealData(regStart + 12, data, false);//系统历史发电量

				//1#微逆1通道
				data = ((reply[36]<<8) | (reply[37])) * 0.1;
				MemDb::SetRealData(regStart + 13, data, false);//直流电压
				data = ((reply[38]<<8) | (reply[39])) * 0.1;
				MemDb::SetRealData(regStart + 14, data, false);//直流电流
				data = ((reply[40]<<8) | (reply[41])) * 0.01;
				MemDb::SetRealData(regStart + 15, data, false);//输出功率
				data = ((reply[42]<<8) | (reply[43])) * 0.1;
				MemDb::SetRealData(regStart + 16, data, false);//电网频率
				data = (short)(reply[44]);
				MemDb::SetRealData(regStart + 17, data, false);//温度
				data = ((reply[45]<<8) | (reply[46]));
				MemDb::SetRealData(regStart + 18, data, false);//电网电压
				data = ((short)(reply[47])) * 0.1;
				MemDb::SetRealData(regStart + 19, data, false);//温度
				value = ((reply[48]<<8) | (reply[49]));
				MemDb::SetRealData(regStart + 20, value, false);//状态位
				data = ((reply[50]<<24) | (reply[51]<<16) | (reply[52]<<8) | (reply[53])) * 0.0000001;
				MemDb::SetRealData(regStart + 21, data, false);//输出电量

				//1#微逆2通道
				data = ((reply[61]<<8) | (reply[62])) * 0.1;
				MemDb::SetRealData(regStart + 22, data, false);//直流电压
				data = ((reply[63]<<8) | (reply[64])) * 0.1;
				MemDb::SetRealData(regStart + 23, data, false);//直流电流
				data = ((reply[65]<<8) | (reply[66])) * 0.01;
				MemDb::SetRealData(regStart + 24, data, false);//输出功率
				data = ((reply[67]<<8) | (reply[68])) * 0.1;
				MemDb::SetRealData(regStart + 25, data, false);//电网频率
				data = (short)(reply[69]);
				MemDb::SetRealData(regStart + 26, data, false);//温度
				data = ((reply[70]<<8) | (reply[71]));
				MemDb::SetRealData(regStart + 27, data, false);//电网电压
				data = ((short)(reply[72])) * 0.1;
				MemDb::SetRealData(regStart + 28, data, false);//电流
				value = ((reply[73]<<8) | (reply[74]));
				MemDb::SetRealData(regStart + 29, value, false);//状态位
				data = ((reply[75]<<24) | (reply[76]<<16) | (reply[77]<<8) | (reply[78])) * 0.0000001;
				MemDb::SetRealData(regStart + 30, data, false);//输出电量

				//2#微逆1通道
				data = ((reply[86]<<8) | (reply[87])) * 0.1;
				MemDb::SetRealData(regStart + 31, data, false);//直流电压
				data = ((reply[88]<<8) | (reply[89])) * 0.1;
				MemDb::SetRealData(regStart + 32, data, false);//直流电流
				data = ((reply[90]<<8) | (reply[91])) * 0.01;
				MemDb::SetRealData(regStart + 33, data, false);//输出功率
				data = ((reply[92]<<8) | (reply[93])) * 0.1;
				MemDb::SetRealData(regStart + 34, data, false);//电网频率
				data = (short)(reply[94]);
				MemDb::SetRealData(regStart + 35, data, false);//温度
				data = ((reply[95]<<8) | (reply[96]));
				MemDb::SetRealData(regStart + 36, data, false);//电网电压
				data = ((short)(reply[97])) * 0.1;
				MemDb::SetRealData(regStart + 37, data, false);//温度
				value = ((reply[98]<<8) | (reply[99]));
				MemDb::SetRealData(regStart + 38, value, false);//状态位
				data = ((reply[100]<<24) | (reply[101]<<16) | (reply[102]<<8) | (reply[103])) * 0.0000001;
				MemDb::SetRealData(regStart + 39, data, false);//输出电量

				//2#微逆2通道
				data = ((reply[111]<<8) | (reply[112])) * 0.1;
				MemDb::SetRealData(regStart + 40, data, false);//直流电压
				data = ((reply[113]<<8) | (reply[114])) * 0.1;
				MemDb::SetRealData(regStart + 41, data, false);//直流电流
				data = ((reply[115]<<8) | (reply[116])) * 0.01;
				MemDb::SetRealData(regStart + 42, data, false);//输出功率
				data = ((reply[117]<<8) | (reply[118])) * 0.1;
				MemDb::SetRealData(regStart + 43, data, false);//电网频率
				data = (short)(reply[119]);
				MemDb::SetRealData(regStart + 44, data, false);//温度
				data = ((reply[120]<<8) | (reply[121]));
				MemDb::SetRealData(regStart + 45, data, false);//电网电压
				data = ((short)(reply[122])) * 0.1;
				MemDb::SetRealData(regStart + 46, data, false);//温度
				value = ((reply[123]<<8) | (reply[124]));
				MemDb::SetRealData(regStart + 47, value, false);//状态位
				data = ((reply[125]<<24) | (reply[126]<<16) | (reply[127]<<8) | (reply[128])) * 0.0000001;
				MemDb::SetRealData(regStart + 48, data, false);//输出电量
			}
		}
	}

	MemDb::SetRealData(regStart + 0, comm, false);//通讯状态
	return ret;
}

int APsystemsPvs::APsystemsPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;

	return ret;
}
