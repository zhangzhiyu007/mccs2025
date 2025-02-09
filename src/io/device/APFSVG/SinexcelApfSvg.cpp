/*
 * SinexcelApfSvg.cpp
 *
 *  创建日期: 2016-6-20
 *  作          者: 刘海涛
 */

#include "SinexcelApfSvg.h"

#include "../../com/Com.h"

SinexcelApfSvg::SinexcelApfSvg()
{
	// TODO Auto-generated constructor stub
	m_com = NULL;
}

SinexcelApfSvg::~SinexcelApfSvg()
{
	// TODO Auto-generated destructor stub
}

void SinexcelApfSvg::SetCom(Com* com)
{
	m_com = com;
}

int SinexcelApfSvg::Read(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_SINEXCEL_APF://深圳盛弘APF/SVG
		ret = this->SinexcelApfSvgRead(dev);
		break;
	default:
		break;
	};

	return ret;
}

int SinexcelApfSvg::Preset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	//判断设备名称
	//判断设备型号来确定具体通信协议
	switch (dev->devModel)
	{
	case DevModel::Model_SINEXCEL_APF://深圳盛弘APF/SVG
		ret = this->SinexcelApfSvgPreset(dev);
		break;
	default:
		break;
	};

	return ret;
}

int SinexcelApfSvg::SinexcelApfSvgRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0;
	int length = 0;
	float tmp = 0.0;
	int value = 0;

	if (NULL == dev)
	{
		zlog_warn(Util::m_zlog,"从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	//判断串口是否打开
	if (!m_com->IsOpen())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetCom(m_com);
	FloatArray values;
	values.clear();
	UshortArray array;
	array.clear();

	//读取运行状态
	start = 0x0000;
	length = 61;
	ret = rtu.ReadInputStatus(array, addr, start, length);
	if(ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		MemDb::SetRealData(regStart + 13, array[0], false);//运行状态

		for(unsigned int i = 40; i < array.size(); i++)
		{
			Util::SetBitValue(value, (i - 40), array[i]);
		}

		if(value > 0)
		{
			MemDb::SetRealData(regStart + 15, 1, false);//故障状态
			MemDb::SetRealData(regStart + 16, value, false);//故障码
		}
		else
		{
			MemDb::SetRealData(regStart + 15, 0, false);//故障状态
			MemDb::SetRealData(regStart + 16, value, false);//故障码
		}
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s设备，地址=%x数据失败", dev->name.c_str(), start);
	}
	msleep(200);

	//读取实时数据
	start = 0x0000;
	length = 100;
	ret = rtu.ReadOutputFloatRegisters(values, addr, start, length);
	if(ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		MemDb::SetRealData(regStart + 38, values[0], false);//负载A相电流
		MemDb::SetRealData(regStart + 39, values[1], false);//负载B相电流
		MemDb::SetRealData(regStart + 40, values[2], false);//负载C相电流
		MemDb::SetRealData(regStart + 54, values[3], false);//负载A相THDI
		MemDb::SetRealData(regStart + 55, values[4], false);//负载B相THDI
		MemDb::SetRealData(regStart + 56, values[5], false);//负载C相THDI
		MemDb::SetRealData(regStart + 51, values[6], false);//负载A相功率因数
		MemDb::SetRealData(regStart + 52, values[7], false);//负载B相功率因数
		MemDb::SetRealData(regStart + 53, values[8], false);//负载C相功率因数
		MemDb::SetRealData(regStart + 27, values[9], false);//A相电感电流
		MemDb::SetRealData(regStart + 28, values[10], false);//B相电感电流
		MemDb::SetRealData(regStart + 29, values[11], false);//C相电感电流
		tmp = values[12] * 1000;
		MemDb::SetRealData(regStart + 73, tmp, false);//源侧A相视在功率
		tmp = values[13] * 1000;
		MemDb::SetRealData(regStart + 74, tmp, false);//源侧B相视在功率
		tmp = values[14] * 1000;
		MemDb::SetRealData(regStart + 75, tmp, false);//源侧C相视在功率
		tmp = values[15] * 1000;
		MemDb::SetRealData(regStart + 67, tmp, false);//源侧A相有功功率
		tmp = values[16] * 1000;
		MemDb::SetRealData(regStart + 68, tmp, false);//源侧B相有功功率
		tmp = values[17] * 1000;
		MemDb::SetRealData(regStart + 69, tmp, false);//源侧C相有功功率
		MemDb::SetRealData(regStart + 66, values[18], false);//源侧N线电流
		MemDb::SetRealData(regStart + 41, values[19], false);//负载N线电流
		MemDb::SetRealData(regStart + 63, values[20], false);//源侧A相电流
		MemDb::SetRealData(regStart + 64, values[21], false);//源侧B相电流
		MemDb::SetRealData(regStart + 65, values[22], false);//源侧C相电流
		MemDb::SetRealData(regStart + 85, values[23], false);//源侧A相THDI
		MemDb::SetRealData(regStart + 86, values[24], false);//源侧B相THDI
		MemDb::SetRealData(regStart + 87, values[25], false);//源侧C相THDI
		MemDb::SetRealData(regStart + 76, values[26], false);//源侧A相功率因数
		MemDb::SetRealData(regStart + 77, values[27], false);//源侧B相功率因数
		MemDb::SetRealData(regStart + 78, values[28], false);//源侧C相功率因数
		MemDb::SetRealData(regStart + 20, values[29], false);//温度1
		MemDb::SetRealData(regStart + 21, values[30], false);//温度2
		MemDb::SetRealData(regStart + 22, values[31], false);//温度3
		tmp = values[32] * 1000;
		MemDb::SetRealData(regStart + 70, tmp, false);//源侧A相无功功率
		tmp = values[33] * 1000;
		MemDb::SetRealData(regStart + 71, tmp, false);//源侧B相无功功率
		tmp = values[34] * 1000;
		MemDb::SetRealData(regStart + 72, tmp, false);//源侧C相无功功率
		MemDb::SetRealData(regStart + 88, values[35], false);//源侧A相COSPHI
		MemDb::SetRealData(regStart + 89, values[36], false);//源侧B相COSPHI
		MemDb::SetRealData(regStart + 90, values[37], false);//源侧C相COSPHI
		tmp = values[38] * 1000;
		MemDb::SetRealData(regStart + 45, tmp, false);//负载A相无功功率
		tmp = values[39] * 1000;
		MemDb::SetRealData(regStart + 46, tmp, false);//负载B相无功功率
		tmp = values[40] * 1000;
		MemDb::SetRealData(regStart + 47, tmp, false);//负载C相无功功率
		MemDb::SetRealData(regStart + 30, values[41], false);//A相补偿电流
		MemDb::SetRealData(regStart + 31, values[42], false);//B相补偿电流
		MemDb::SetRealData(regStart + 32, values[43], false);//C相补偿电流
		MemDb::SetRealData(regStart + 33, values[44], false);//A相补偿电流负载率
		MemDb::SetRealData(regStart + 34, values[45], false);//B相补偿电流负载率
		MemDb::SetRealData(regStart + 35, values[46], false);//C相补偿电流负载率
		MemDb::SetRealData(regStart + 23, values[47], false);//温度4
		MemDb::SetRealData(regStart + 24, values[48], false);//温度5
		MemDb::SetRealData(regStart + 25, values[49], false);//温度6
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s设备，地址=%x数据失败", dev->name.c_str(), start);
	}
	msleep(200);

	start = 0x0064;
	length = 60;
	ret = rtu.ReadOutputFloatRegisters(values, addr, start, length);
	if(ErrorInfo::ERR_OK == ret)
	{
		comm = true;
		tmp = values[0] * 1000;
		MemDb::SetRealData(regStart + 48, tmp, false);//负载A相视在功率
		tmp = values[1] * 1000;
		MemDb::SetRealData(regStart + 49, tmp, false);//负载B相视在功率
		tmp = values[2] * 1000;
		MemDb::SetRealData(regStart + 50, tmp, false);//负载C相视在功率
		tmp = values[3] * 1000;
		MemDb::SetRealData(regStart + 42, tmp, false);//负载A相有功功率
		tmp = values[4] * 1000;
		MemDb::SetRealData(regStart + 43, tmp, false);//负载B相有功功率
		tmp = values[5] * 1000;
		MemDb::SetRealData(regStart + 44, tmp, false);//负载C相有功功率
		MemDb::SetRealData(regStart + 57, values[6], false);//负载A相COSPHI
		MemDb::SetRealData(regStart + 58, values[7], false);//负载B相COSPHI
		MemDb::SetRealData(regStart + 59, values[8], false);//负载C相COSPHI
		MemDb::SetRealData(regStart + 60, values[9], false);//源侧A相电压
		MemDb::SetRealData(regStart + 61, values[10], false);//源侧B相电压
		MemDb::SetRealData(regStart + 62, values[11], false);//源侧C相电压
		MemDb::SetRealData(regStart + 79, values[12], false);//源侧A相频率
		MemDb::SetRealData(regStart + 80, values[13], false);//源侧B相频率
		MemDb::SetRealData(regStart + 81, values[14], false);//源侧C相频率
		MemDb::SetRealData(regStart + 82, values[15], false);//源侧A相THDU
		MemDb::SetRealData(regStart + 83, values[16], false);//源侧B相THDU
		MemDb::SetRealData(regStart + 84, values[17], false);//源侧C相THDU
		MemDb::SetRealData(regStart + 17, values[24], false);//整机运行时间
		MemDb::SetRealData(regStart + 18, values[25], false);//大于50%载运行时间
		MemDb::SetRealData(regStart + 19, values[26], false);//小于50%载运行时间
		MemDb::SetRealData(regStart + 36, values[27], false);//正直流母线电压
		MemDb::SetRealData(regStart + 37, values[28], false);//负直流母线电压
		MemDb::SetRealData(regStart + 26, values[29], false);//电感温度
	}
	else
	{
		zlog_warn(Util::m_zlog,"读取%s设备，地址=%x数据失败", dev->name.c_str(), start);
	}

	MemDb::SetRealData(regStart + 0, comm ,false);//通讯状态

	return ret;
}

int SinexcelApfSvg::SinexcelApfSvgPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int tmp = 0;

	//判断串口是否打开
	if (!m_com->IsOpen())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusRtuMaster rtu;
	rtu.SetCom(m_com);

	FloatArray values;
	values.clear();
	UshortArray array;
	array.clear();
	int addr = atoi(dev->slaveAddr.c_str());
	RealData realData;
	int regStart = dev->regStart;
	int start = 0;

	//判断是否有启动/停止命令
	if (MemDb::GetRealDataWrited(realData, regStart + 1, true))
	{
		if(realData.pv.data.iValue == 1)//启动
		{
			array.clear();
			array.push_back(realData.pv.data.iValue);
			rtu.PresetMultipleRegisters(addr, 0x2C1E, 2, array);
		}
		else if(realData.pv.data.iValue == 0)//停止
		{
			array.clear();
			array.push_back(realData.pv.data.iValue);
			rtu.PresetMultipleRegisters(addr, 0x2C20, 2, array);
		}
	}

	//判断是否有故障清除命令
	if (MemDb::GetRealDataWrited(realData, regStart + 4, true))
	{
		if(realData.pv.data.iValue == 1)//启动
		{
			array.clear();
			array.push_back(realData.pv.data.iValue);
			rtu.PresetMultipleRegisters(addr, 0x2C1A, 2, array);
		}
	}

	//判断是否有启动方式命令
	if (MemDb::GetRealDataWrited(realData, regStart + 2, true))
	{
		array.clear();
		array.push_back(realData.pv.data.iValue);
		rtu.PresetMultipleRegisters(addr, 0x201E, 2, array);
	}

	//判断是否有工作模式命令
	if (MemDb::GetRealDataWrited(realData, regStart + 3, true))
	{
		array.clear();
		array.push_back(realData.pv.data.iValue);
		rtu.PresetMultipleRegisters(addr, 0x201C, 2, array);
		MemDb::SetRealData(regStart + 14, realData.pv.data.iValue, false);
	}

	//判断是否有谐波补偿率设置命令
	if (MemDb::GetRealDataWrited(realData, regStart + 5, true))
	{
		values.clear();
		values.push_back(realData.pv.data.dbValue);
		rtu.PresetMultipleFloatRegisters(addr, 0x200C, 2, values);
	}

	//判断是否有目标功率因数命令
	if (MemDb::GetRealDataWrited(realData, regStart + 6, true))
	{
		values.clear();
		values.push_back(realData.pv.data.dbValue);
		rtu.PresetMultipleFloatRegisters(addr, 0x200E, 2, values);
	}

	//判断是否有3次谐波相角偏置命令
	if (MemDb::GetRealDataWrited(realData, regStart + 7, true))
	{
		values.clear();
		values.push_back(realData.pv.data.dbValue);
		rtu.PresetMultipleFloatRegisters(addr, 0x2502, 2, values);
	}

	//判断是否有谐波补偿次数命令
	if (MemDb::GetRealDataWrited(realData, regStart + 9, true))
	{
		MemDb::GetRealData(tmp, regStart + 8);//谐波补偿次数

		start = 0x2A00 + ((tmp * 2) - 4);
		values.clear();
		values.push_back(realData.pv.data.dbValue);
		rtu.PresetMultipleFloatRegisters(addr, start, 2, values);
	}

	return ret;
}
