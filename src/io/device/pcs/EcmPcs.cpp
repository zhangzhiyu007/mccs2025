/*
 * EcmPcs.cpp
 *
 *  Created on: 2019-4-24
 *      Author: JDHN
 */

#include "EcmPcs.h"
#include "../../../db/MemDb.h"
#include "../../../etc/socket/TcpClient.h"
#include "../../../util/Util.h"
#include "../../com/Com.h"
#include "../../protocol/modbus/ModbusTcpClient.h"

EcmPcs::EcmPcs() {
    // TODO Auto-generated constructor stub
    m_isTcp     = false;
    m_com       = NULL;
    m_tcpClient = NULL;
    m_state     = 0;
}

EcmPcs::~EcmPcs() {
    // TODO Auto-generated destructor stub
}

//设置是否采用以太网通讯
void EcmPcs::SetIsTcp(bool isTcp) { m_isTcp = isTcp; }

void EcmPcs::SetCom(Com *com) { m_com = com; }

//设置TcpClient指针
void EcmPcs::SetTcp(TcpClient *tcpClient) { m_tcpClient = tcpClient; }

int EcmPcs::Read(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;

    if (NULL == dev) {
        return ret;
    }

    //判断设备名称
    //判断设备厂商和型号来确定具体通信协议
    switch (dev->devModel) {
    case DevModel::Model_ECM_DCAC: //PCS
		// printf("44\n");
        ret = this->EcmPcsRead(dev);
        break;
    case DevModel::Model_ECM_DCAC_kaifang: //PCS
		// printf("72\n");

        ret = this->EcmPcskaifangRead(dev);
        // ret = this->EcmPcsRead(dev);

        break;
    default:
        break;
    };

    return ret;
}

int EcmPcs::Preset(Device::SlaveDev *dev) {
    int ret = ErrorInfo::ERR_OK;
    if (NULL == dev) {
        return ret;
    }

    //判断设备名称
    //判断设备厂商和型号来确定具体通信协议
    switch (dev->devModel) {
    case DevModel::Model_ECM_DCAC: //PCS
        ret = this->EcmPcsPreset(dev);
        break;
    case DevModel::Model_ECM_DCAC_kaifang: //PCS
        ret = this->EcmPcskaifangPreset(dev);
        break;
    default:
        break;
    };

    return ret;
}

int EcmPcs::EcmPcskaifangRead(Device::SlaveDev *dev)
{
	int ret           = ErrorInfo::ERR_OK;
	bool comm         = false;
	int addr          = atoi(dev->slaveAddr.c_str());
	int regStart      = dev->regStart;
	int start         = 0x0000;
	int length        = 0;
	int data          = 0;
	float f_data      = 0.0;
	float vol         = 0.0;
	float cur         = 0.0;
	float power       = 0.0;
	int fault         = 0;
	float gridVoltage = 0.0;
	float gridFreq    = 50.0;

	if (NULL == dev) {
		zlog_warn(Util::m_zlog, "从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	//判断串口是否打开
	if (!m_tcpClient->IsOpened()) {
		return ErrorInfo::ERR_OPENED;
	}

	ModbusTcpClient client;
	client.SetTcpClient(m_tcpClient);

	UshortArray values;
	values.clear();

	start  = 0;
	length = 28;
	ret    = client.ReadOutputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret) {
		comm = true;

		f_data = (unsigned short)values[0] * 0.1;
		MemDb::SetRealData(regStart + 1, f_data, false); //电池电压

		f_data = (unsigned short)values[1] * 0.1;
		MemDb::SetRealData(regStart + 2, f_data, false); //上母线电压

		f_data = (unsigned short)values[2] * 0.1;
		MemDb::SetRealData(regStart + 3, f_data, false); //下母线电压

		f_data = (unsigned short)values[3] * 0.1;
		MemDb::SetRealData(regStart + 4, f_data, false); //母线电压

		f_data = (short)values[4] * 0.01;
		MemDb::SetRealData(regStart + 5, f_data, false); //直流电流

		f_data = (short)values[5] * 0.01;
		MemDb::SetRealData(regStart + 6, f_data, false); //直流功率 kw

		f_data = (unsigned short)values[6] * 0.01;
		MemDb::SetRealData(regStart + 7, f_data, false); //电网电压A

		f_data = (unsigned short)values[7] * 0.01;
		MemDb::SetRealData(regStart + 8, f_data, false); //电网电压B

		f_data = (unsigned short)values[8] * 0.01;
		MemDb::SetRealData(regStart + 9, f_data, false); //电网电压C

        f_data = (unsigned short)values[9] * 0.01;
        MemDb::SetRealData(regStart + 10, f_data, false); //逆变电压A

        f_data = (unsigned short)values[10] * 0.01;
        MemDb::SetRealData(regStart + 11, f_data, false); //逆变电压B

        f_data = (unsigned short)values[11] * 0.01;
        MemDb::SetRealData(regStart + 12, f_data, false); //逆变电压C

        f_data = (unsigned short)values[15] * 0.01;
        MemDb::SetRealData(regStart + 13, f_data, false); //电网电流A

        f_data = (unsigned short)values[16] * 0.01;
        MemDb::SetRealData(regStart + 14, f_data, false); //电网电流B

        f_data = (unsigned short)values[17] * 0.01;
        MemDb::SetRealData(regStart + 15, f_data, false); //电网电流C

        f_data = (unsigned short)values[18] * 0.01;
        MemDb::SetRealData(regStart + 16, f_data, false); //电感电流A

        f_data = (unsigned short)values[19] * 0.01;
        MemDb::SetRealData(regStart + 17, f_data, false); //电感电流B

        f_data = (unsigned short)values[20] * 0.01;
        MemDb::SetRealData(regStart + 18, f_data, false); //电感电流C

        f_data = (short)values[21] * 0.01;
        MemDb::SetRealData(regStart + 19, f_data, false); //电网有功交换功率

        f_data = (short)values[22] * 0.01;
        MemDb::SetRealData(regStart + 20, f_data, false); //电网无功交换功率

        f_data = (unsigned short)values[23] * 0.01;
        MemDb::SetRealData(regStart + 21, f_data, false); //电网视在交换功率

        f_data = (short)values[24] * 0.01;
        MemDb::SetRealData(regStart + 22, f_data, false); //中点电流

        f_data = (short)values[25] * 0.01;
        MemDb::SetRealData(regStart + 23, f_data, false); //逆变器频率

        f_data = (short)values[26] * 0.01;
        MemDb::SetRealData(regStart + 24, f_data, false); //电网频率
	}
	else {
	        comm = false;
	        zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
	    }

	start  = 50;
	length = 13;
	values.clear();
	ret    = client.ReadOutputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret) {
		comm = true;
		data = (unsigned short)values[0];
		MemDb::SetRealData(regStart + 25, data, false); //单相计算使能
		f_data = (short)values[1] * 0.1;
		MemDb::SetRealData(regStart + 26, f_data, false); //A相有功输出
		f_data = (short)values[2] * 0.1;
		MemDb::SetRealData(regStart + 27, f_data, false); //B相有功输出
		f_data = (short)values[3] * 0.1;
		MemDb::SetRealData(regStart + 28, f_data, false); //C相有功输出
		f_data = (short)values[4] * 0.1;
		MemDb::SetRealData(regStart + 29, f_data, false); //A相无功输出
		f_data = (short)values[5] * 0.1;
		MemDb::SetRealData(regStart + 30, f_data, false); //B相无功输出
		f_data = (short)values[6] * 0.1;
		MemDb::SetRealData(regStart + 31, f_data, false); //C相无功输出
		f_data = (unsigned short)values[7] * 0.1;
		MemDb::SetRealData(regStart + 32, f_data, false); //A相视在输出
		f_data = (unsigned short)values[8] * 0.1;
		MemDb::SetRealData(regStart + 33, f_data, false); //B相视在输出
		f_data = (unsigned short)values[9] * 0.1;
		MemDb::SetRealData(regStart + 34, f_data, false); //C相视在输出
		f_data = (short)values[10] * 0.01;
		MemDb::SetRealData(regStart + 35, f_data, false); //A相功率因数
		f_data = (short)values[11] * 0.01;
		MemDb::SetRealData(regStart + 36, f_data, false); //B相功率因数
		f_data = (short)values[12] * 0.01;
		MemDb::SetRealData(regStart + 37, f_data, false); //C相功率因数

	}
	else {
	        comm = false;
	        zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
	    }

	start  = 202;
	length = 6;
	values.clear();
	ret    = client.ReadOutputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret) {
			comm = true;
			data =(short) values[0];
			MemDb::SetRealData(regStart + 38, data, false); //本地实测相序 -1 负序 1 正序
			data = (short)values[1];
			MemDb::SetRealData(regStart + 39, data, false); //电网实测相序 -1 负序 1 正序
			data = (unsigned short)values[3];
			MemDb::SetRealData(regStart + 40, data, false); //电网状态 0异常， 1 正常
			data = (unsigned short)values[4];
			MemDb::SetRealData(regStart + 41, data, false); //锁相同步状态 0未同步， 1 已同步
			data =(unsigned short) values[5];
			MemDb::SetRealData(regStart + 42, data, false); //并网状态 0未并网， 1 已并网
	}
	else {
		    comm = false;
		    zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
		 }

	start  = 239;
	length = 1;
	values.clear();
	ret    = client.ReadOutputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret) {
			comm = true;
			data = (unsigned short)values[0];
			MemDb::SetRealData(regStart + 43, data, false); //工作状态   0待机 ；1 开机中;2关机中； 3 故障;4离网充电;5离网放电;6并网充电;7并网放电;8	直流稳压。
	}
	else {
		    comm = false;
		    zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
		 }


	start  = 42027;
	length = 4;
	values.clear();
	ret    = client.ReadOutputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret) {
			comm = true;
			f_data =(unsigned short) values[0] * 0.01 -40;
			MemDb::SetRealData(regStart + 44, f_data, false); //IGBTA相温度
			f_data = (unsigned short)values[1] * 0.01 -40;
			MemDb::SetRealData(regStart + 45, f_data, false); //IGBTB相温度
			f_data = (unsigned short)values[2] * 0.01 -40;
			MemDb::SetRealData(regStart + 46, f_data, false); //IGBTC相温度
			f_data = (unsigned short)values[3] * 0.01 -40;
			MemDb::SetRealData(regStart + 47, f_data, false); //IGBTN相温度
	}
	else {
		    comm = false;
		    zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
		 }

	start  = 100;
	length = 6;
	values.clear();
	ret    = client.ReadOutputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret) {
		comm = true;
		data = (unsigned short)values[0];
		MemDb::SetRealData(regStart + 50, data, false); //设备故障
		data = (unsigned short)values[1];
		MemDb::SetRealData(regStart + 51, data, false); //硬件故障
		data = (unsigned short)values[2];
		MemDb::SetRealData(regStart + 52, data, false); //软件故障
		data = (unsigned short)values[3];
		MemDb::SetRealData(regStart + 53, data, false); //设备告警
		data = (unsigned short)values[4];
		MemDb::SetRealData(regStart + 54, data, false); //告警清除
		data = (unsigned short)values[5];
		MemDb::SetRealData(regStart + 55, data, false); //故障代码   0正常,10急停,11并网开关断开失败,12电池过压,13电池欠压,14直流稳压模式下并网开关未闭合,15孤岛时过压,16孤岛时欠压
	}
	else {
	        comm = false;
	        zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
	    }
	start  = 245;
	length = 3;
	values.clear();
	ret   = client.ReadInputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret) {
			comm = true;
			data = (unsigned short)values[0];
			MemDb::SetRealData(regStart + 56, data, false); //电池模拟器模式
			f_data = (unsigned short)values[1] * 0.1;
			MemDb::SetRealData(regStart + 57, f_data, false); //电池模拟器模式输出电压
			f_data = (unsigned short)values[2] * 0.001;
			MemDb::SetRealData(regStart + 58, f_data, false); //电池模拟器模式内阻
		}
		else {
		        comm = false;
		        zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
		    }

	start  = 255;
	length = 2;
	values.clear();
	ret   = client.ReadInputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret) {
			comm = true;
			data = (unsigned short)values[0];
			MemDb::SetRealData(regStart + 59, data, false); //电池模拟下垂模式  0电压下垂  1功率下垂
			f_data = (unsigned short)values[1] * 0.0001;
			MemDb::SetRealData(regStart + 60, f_data, false); //电池模拟功率下垂基准值
		}
		else {
				comm = false;
				zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
			}

	start  = 1200;
	length = 5;
	values.clear();
	ret   = client.ReadInputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret) {
			comm = true;
			data = (short)values[0];
			MemDb::SetRealData(regStart + 61, data, false); //交流电压基准
			data = (short)values[1];
			MemDb::SetRealData(regStart + 62, data, false); //交流电流基准
			data = (short)values[2];
			MemDb::SetRealData(regStart + 63, data, false); //直流电压基准
			data = (short)values[3];
			MemDb::SetRealData(regStart + 64, data, false); //直流电流基准
			data = (short)values[4];
			MemDb::SetRealData(regStart + 65, data, false); //功率给定限制

		}
		else {
				comm = false;
				zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
			}



	start  = 250;
	length = 1;
	values.clear();
	ret   = client.ReadInputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret) {
			comm = true;
			data = (short)values[0];
			MemDb::SetRealData(regStart + 66, data, false); //有功功率反馈值
		}
		else {
				comm = false;
				zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
			}





	start  = 345;
	length = 2;
	values.clear();
	ret   = client.ReadInputRegisters(values, addr, start, length);
	if (ErrorInfo::ERR_OK == ret) {
			comm = true;
			data = (short)values[0];
			MemDb::SetRealData(regStart + 48, data, false); //并网与电池模拟链接模式
			data = (short)values[1];
			MemDb::SetRealData(regStart + 49, data, false); //并网与电池模拟自动切换模式
		}
		else {
				comm = false;
				zlog_warn(Util::m_zlog, "读取%s实时数据失败", dev->name.c_str());
		}


	MemDb::SetRealData(regStart + 0, comm, false);
}


int EcmPcs::EcmPcskaifangPreset(Device::SlaveDev *dev)
{
	int ret           = ErrorInfo::ERR_OK;
	bool comm         = false;
	int addr          = atoi(dev->slaveAddr.c_str());
	int regStart      = dev->regStart;
	int start         = 0x0000;
	int length        = 0;
	int data          = 0;
	float tmp         = 0.0;
	int fault         = 0;
    RealData realData;


	if (NULL == dev) {
		zlog_warn(Util::m_zlog, "从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	//判断串口是否打开
	if (!m_tcpClient->IsOpened()) {
		return ErrorInfo::ERR_OPENED;
	}

	ModbusTcpClient client;
	client.SetTcpClient(m_tcpClient);

 //=======================判断 遥控
	//设备故障清除 置1清除故障，设定2秒后，下发0转为0；
    if (MemDb::GetRealData(realData, regStart + 69, true)) {
        ret = client.PresetSingleRegister(addr, 104, realData.pv.data.iValue);
        sleep(2);
        ret = client.PresetSingleRegister(addr, 104, 0);
    }
	//离网电压给定方式选择 0是线电压，1是相电压
    if (MemDb::GetRealData(realData, regStart + 70, true)) {
        ret = client.PresetSingleRegister(addr, 240, realData.pv.data.iValue);
    }

    //离网线电压给定
    if (MemDb::GetRealData(realData, regStart + 71, true)) {
    	data = realData.pv.data.iValue *10;
    	zlog_warn(Util::m_zlog, "读取data离线电压给定=%d", data);
        ret = client.PresetSingleRegister(addr, 241, data);
    }
    //离网相电压给定
    if (MemDb::GetRealData(realData, regStart + 72, true)) {
    	data = realData.pv.data.iValue *10;
    	zlog_warn(Util::m_zlog, "读取data离网相电压给定=%d", data);
        ret = client.PresetSingleRegister(addr, 242, data);
    }

    //设备工频频率
    if (MemDb::GetRealData(realData, regStart + 73, true)) {
    	data = realData.pv.data.iValue *100;
    	zlog_warn(Util::m_zlog, "读取data设备工频频率=%d", data);
        ret = client.PresetSingleRegister(addr, 243, data);
    }
    //交流电压变化速率 ms
    if (MemDb::GetRealData(realData, regStart + 74, true)) {
        ret = client.PresetSingleRegister(addr, 244, realData.pv.data.iValue);
    }

    //电池模拟器模式
    if (MemDb::GetRealData(realData, regStart + 75, true)) {
        ret = client.PresetSingleRegister(addr, 245, realData.pv.data.iValue);
    }

    //电池模拟器模式输出电压
     if (MemDb::GetRealData(realData, regStart + 76, true)) {
    	 data = realData.pv.data.iValue *10;
    	 zlog_warn(Util::m_zlog, "读取data电池模拟器模式输出电压=%d", data);
         ret = client.PresetSingleRegister(addr, 246, data);
     }
     //电池模拟器模式内阻
      if (MemDb::GetRealData(realData, regStart + 77, true)) {
    	  data = realData.pv.data.iValue *100;//只用poll调试用
    	//  data = realData.pv.data.dbValue *1000;//后面换成上位机用这个
    	  zlog_warn(Util::m_zlog, "读取data电池模拟器模式内阻=%d", data);
          ret = client.PresetSingleRegister(addr, 247, data);
      }

     //有功无功给定保存选择 0不可保存,1可保存
      if (MemDb::GetRealData(realData, regStart + 78, true)) {
          ret = client.PresetSingleRegister(addr, 249, realData.pv.data.iValue);
      }
      //有功功率给定
       if (MemDb::GetRealData(realData, regStart + 79, true)) {
    	   data = realData.pv.data.iValue*100;
    	   zlog_warn(Util::m_zlog, "读取data有功功率给定=%d", data);
           ret = client.PresetSingleRegister(addr, 250, data);
      }
       //无功功率给定
        if (MemDb::GetRealData(realData, regStart + 80, true)) {
        	data = realData.pv.data.iValue *100;
            zlog_warn(Util::m_zlog, "读取data无功功率给定=%d", data);
            ret = client.PresetSingleRegister(addr, 251, data);
      }

//=======================判断 遥调
        // //设备启停 1#和2#联动
        // if(regStart == 2300)
        // {
        // if (MemDb::GetRealData(realData, regStart + 95, true)) {
        //    ret = client.PresetSingleRegister(addr, 220, realData.pv.data.iValue);//0停机，1开机
        //    MemDb::SetRealData(50, 5, false); //用50放1#pcs停机标志位
        //    MemDb::SetRealData(51, realData.pv.data.iValue, false); //
        // }
        // }

        // if(regStart == 2400)
        // {
        // 	int data = 0;
        // 	int flag = 0;

        // 	MemDb::GetRealData(flag, 50);
        // 	MemDb::GetRealData(data, 51);
        // 	if(flag == 5){
        //     ret = client.PresetSingleRegister(addr, 220, data);}//0停机，1开机
        // }
//======================================


//=======================判断 遥调
        //设备启停
         if (MemDb::GetRealData(realData, regStart + 81, true)) {
             ret = client.PresetSingleRegister(addr, 220, realData.pv.data.iValue);//0停机，1开机
       }
         //离并网控制
          if (MemDb::GetRealData(realData, regStart + 82, true)) {
              ret = client.PresetSingleRegister(addr, 221, realData.pv.data.iValue);//0只能离网运行，1只能并网运行
        }
          //低电压穿越功能使能
           if (MemDb::GetRealData(realData, regStart + 83, true)) {
               ret = client.PresetSingleRegister(addr, 321, realData.pv.data.iValue);//0禁止，1使能
        }
           //自动并网时间
            if (MemDb::GetRealData(realData, regStart + 84, true)) {
                ret = client.PresetSingleRegister(addr, 309, realData.pv.data.iValue);//
        }
            //并机模式
            if (MemDb::GetRealData(realData, regStart + 85, true)) {
                 ret = client.PresetSingleRegister(addr, 364, realData.pv.data.iValue);//0独立运行； 1主从 ； 2下垂二次调节

         }

	    //并网与电池模拟自动切换模式
		  if (MemDb::GetRealData(realData, regStart + 86, true)) {
		  ret = client.PresetSingleRegister(addr, 346, realData.pv.data.iValue);//0：禁止； 1:使能 ；

	     }
	     //并网与电池模拟链接模式
		  if (MemDb::GetRealData(realData, regStart + 87, true)) {
		  ret = client.PresetSingleRegister(addr, 345, realData.pv.data.iValue);//0：禁止； 1:使能 ；

		 }

        //并网自动切换电池模拟电压上限
	     if (MemDb::GetRealData(realData, regStart + 88, true)) {
	     ret = client.PresetSingleRegister(addr, 347, realData.pv.data.iValue);//设385

	     }
	    //并网自动切换电池模拟电压下限
		  if (MemDb::GetRealData(realData, regStart + 89, true)) {
		  ret = client.PresetSingleRegister(addr, 348, realData.pv.data.iValue);// 设365

		 }


	    //电池模拟下垂模式  0电压下垂  1功率下垂
		 if (MemDb::GetRealData(realData, regStart + 90, true)) {
		 ret = client.PresetSingleRegister(addr, 255, realData.pv.data.iValue);//设255

		 }
	    //电池模拟功率下垂基准值
		  if (MemDb::GetRealData(realData, regStart + 91, true)) {
			//  ret = client.PresetSingleRegister(addr, 256, realData.pv.data.iValue * 1000);// 设256  poll下
			  data = realData.pv.data.dbValue * 10000;
		      ret = client.PresetSingleRegister(addr, 256, data);// 设256//上位机下
		      zlog_warn(Util::m_zlog, "读取池模拟功率data下垂基准值=%d", data);

		 }

}


int EcmPcs::EcmPcsRead(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	bool comm = false;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0x0000;
	int length = 0;
	int data = 0;
	float tmp = 0.0;
	float vol = 0.0;
	float cur = 0.0;
	float power = 0.0;
	int fault = 0;
//	float gridVoltage = 0.0;
//	float gridFreq = 50.0;
//	float fdata = 0;

	if (NULL == dev)
	{
		zlog_warn(Util::m_zlog,"从设备为NULL");
		return ErrorInfo::ERR_NULL;
	}

	//判断串口是否打开
	if (!m_tcpClient->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusTcpClient client;
	client.SetTcpClient(this->m_tcpClient);

	UshortArray values;
	values.clear();

	//读取实时数据
	start = 100;
	length = 60;
    ret = client.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret)
    {
    	comm = true;

    	//电池直流
    	data = (((short)values[1]) * 65536) + ((short)values[0]);
    	memcpy(&vol,&data,sizeof(int));
    	MemDb::SetRealData(regStart + 36, vol, false);//电池直流电压36

    	data = (((short)values[3]) * 65536) + ((short)values[2]);
    	memcpy(&cur,&data,sizeof(int));
    	MemDb::SetRealData(regStart + 37, cur, false);//电池直流电流37

    	power = vol * cur * 0.001;//单位是kW
    	MemDb::SetRealData(regStart + 38, power, false);//直流功率38

    	//本地交流电压
    	data = (((short)values[9]) * 65536) + ((short)values[8]);
    	memcpy(&tmp,&data,sizeof(int));
    	MemDb::SetRealData(regStart + 23, tmp, false);//A相电压

    	data = (((short)values[11]) * 65536) + ((short)values[10]);
    	memcpy(&tmp,&data,sizeof(int));
    	MemDb::SetRealData(regStart + 24, tmp, false);//B相电压

    	data = (((short)values[13]) * 65536) + ((short)values[12]);
    	memcpy(&tmp,&data,sizeof(int));
    	MemDb::SetRealData(regStart + 25, tmp, false);//C相电压

    	//本地交流电流
    	data = (((short)values[15]) * 65536) + ((short)values[14]);
    	memcpy(&tmp,&data,sizeof(int));
    	MemDb::SetRealData(regStart + 26, tmp, false);//A相电流 26
    	data = (((short)values[17]) * 65536) + ((short)values[16]);
    	memcpy(&tmp,&data,sizeof(int));
    	MemDb::SetRealData(regStart + 27, tmp, false);//B相电流27
    	data = (((short)values[19]) * 65536) + ((short)values[18]);
    	memcpy(&tmp,&data,sizeof(int));
    	MemDb::SetRealData(regStart + 28, tmp, false);//C相电流28

    	//网侧交流电压
    	data = (((short)values[21]) * 65536) + ((short)values[20]);
    	memcpy(&tmp,&data,sizeof(int));
//    	if(addr == 1)
//    	{
//    		//gridVoltage = tmp;
//    	}
    	MemDb::SetRealData(regStart + 29, tmp, false);//A相电压29
    	data = (((short)values[23]) * 65536) + ((short)values[22]);
    	memcpy(&tmp,&data,sizeof(int));
    	MemDb::SetRealData(regStart + 30, tmp, false);//B相电压30
    	data = (((short)values[25]) * 65536) + ((short)values[24]);
    	memcpy(&tmp,&data,sizeof(int));
    	MemDb::SetRealData(regStart + 31, tmp, false);//C相电压31

    	//网侧交流电流
    	data = (((short)values[27]) * 65536) + ((short)values[26]);
    	memcpy(&tmp,&data,sizeof(int));
    	MemDb::SetRealData(regStart + 32, tmp, false);//A相电流32
    	data = (((short)values[29]) * 65536) + ((short)values[28]);
    	memcpy(&tmp,&data,sizeof(int));
    	MemDb::SetRealData(regStart + 33, tmp, false);//B相电流33
    	data = (((short)values[31]) * 65536) + ((short)values[30]);
    	memcpy(&tmp,&data,sizeof(int));
    	MemDb::SetRealData(regStart + 34, tmp, false);//C相电流34

    	//运行状态
    	data = (short)values[32];
    	switch(data)
    	{
    	case 0://待机
    	case 1://开机中
    		MemDb::SetRealData(regStart + 35, 2, false);//运行状态-待机
    		break;
    	case 2://独立运行充电
    	case 3://独立运行放电
    	case 4://并网运行充电
    	case 5://并网运行放电
    	case 9://电池模拟器
    		MemDb::SetRealData(regStart + 35, 1, false);//运行状态-运行
    		break;
    	case 6://关机中
    	case 7://关机放电
    	case 8://故障
    		MemDb::SetRealData(regStart + 35, 0, false);//运行状态-停止
    		break;
    	default:
    		MemDb::SetRealData(regStart + 35, 0, false);//运行状态-停止35
    		break;
    	}

    	//交流侧功率
    	data = (((short)values[35]) * 65536) + ((short)values[34]);
    	memcpy(&tmp,&data,sizeof(int));
       	MemDb::SetRealData(regStart + 39, tmp, false);//有功功率39 //单位kW
    	data = (((short)values[37]) * 65536) + ((short)values[36]);
    	memcpy(&tmp,&data,sizeof(int));
    	MemDb::SetRealData(regStart + 40, tmp, false);//无功功率40//单位kVar
    	data = (((short)values[39]) * 65536) + ((short)values[38]);
    	memcpy(&tmp,&data,sizeof(int));
       	MemDb::SetRealData(regStart + 41, tmp, false);//视在功率41//单位kVA

    	//状态
    	data = (short)values[40];
    	if(Util::GetBitValue(data, 2))
    	{
    		MemDb::SetRealData(regStart + 42, 0, false);//并网42
    	}
    	else
    	{
    		MemDb::SetRealData(regStart + 42, 1, false);//离网
    	}

    	//故障 43
    	if(((short)values[42] == 1) || ((short)values[43] == 1)
    			|| ((short)values[44] == 1) || ((short)values[45] == 1)
    			|| ((short)values[49] == 1) || ((short)values[50] == 1)
    			|| ((short)values[51] == 1) || ((short)values[52] == 1))
    	{
    		Util::SetBitValue(fault, 0, true);//继电器故障
    	}
    	else
    	{
    		Util::SetBitValue(fault, 0, false);//继电器故障
    	}

    	if(((short)values[46] == 1) || ((short)values[53] == 1))
    	{
    		Util::SetBitValue(fault, 1, true);//并网开关故障
    	}
    	else
    	{
    		Util::SetBitValue(fault, 1, false);//并网开关故障
    	}

    	if(((short)values[47] == 1))
    	{
    		Util::SetBitValue(fault, 2, true);//直流断路器故障
    	}
    	else
    	{
    		Util::SetBitValue(fault, 2, false);//直流断路器故障
    	}

    	if(((short)values[48] == 1))
    	{
    		Util::SetBitValue(fault, 3, true);//交流断路器故障
    	}
    	else
    	{
    		Util::SetBitValue(fault, 3, false);//交流断路器故障
    	}

    	if(((short)values[54] == 1))
    	{
    		Util::SetBitValue(fault, 4, true);//直流侧欠压
    	}
    	else
    	{
    		Util::SetBitValue(fault, 4, false);//直流侧欠压
    	}

    	if(((short)values[55] == 1))
    	{
    		Util::SetBitValue(fault, 5, true);//直流侧过压
    	}
    	else
    	{
    		Util::SetBitValue(fault, 5, false);//直流侧过压
    	}

    	if(((short)values[56] == 1))
    	{
    		Util::SetBitValue(fault, 6, true);//电池模拟器模式下电网异常
    	}
    	else
    	{
    		Util::SetBitValue(fault, 6, false);//电池模拟器模式下电网异常
    	}

    	if(((short)values[57] == 1))
    	{
    		Util::SetBitValue(fault, 7, true);//电池模拟器模式下并网开关未闭合
    	}
    	else
    	{
    		Util::SetBitValue(fault, 7, false);//电池模拟器模式下并网开关未闭合
    	}

    	if(((short)values[58] == 1))
    	{
    		Util::SetBitValue(fault, 8, true);//开机时电网异常但并网开关闭合
    	}
    	else
    	{
    		Util::SetBitValue(fault, 8, false);//开机时电网异常但并网开关闭合
    	}
    }
    else
    {
        comm = false;
        zlog_warn(Util::m_zlog,"读取%s实时数据失败",dev->name.c_str());
    }

//    if(addr == 1)
//    {
//        if(gridVoltage < 180)
//        {
//        	gridFreq = 0;
//        }
//    	//用于电网分析是否有电
//    	Grid::Analysis(gridVoltage, gridFreq);
//    }


	//读取故障数据
	start = 300;
	length = 6;
    ret = client.ReadOutputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret)
    {
    	comm = true;
    	data = (((short)values[1]) * 65536) + ((short)values[0]);
    	memcpy(&tmp,&data,sizeof(int));
    	MemDb::SetRealData(regStart + 44, tmp, false);//母线电压 44   300 301 302 303

    	data = (short)values[3];
    	if(Util::GetBitValue(data, 0) || Util::GetBitValue(data, 1)
			||Util::GetBitValue(data, 2))
    	{
    		Util::SetBitValue(fault, 9, true);//IGBT过流
    	}
    	else
    	{
    		Util::SetBitValue(fault, 9, false);//IGBT过流
    	}

    	if(Util::GetBitValue(data, 3) || Util::GetBitValue(data, 4)
			||Util::GetBitValue(data, 5))
    	{
    		Util::SetBitValue(fault, 10, true);//电网过流
    	}
    	else
    	{
    		Util::SetBitValue(fault, 10, false);//电网过流
    	}

    	if(Util::GetBitValue(data, 6) || Util::GetBitValue(data, 7)
			||Util::GetBitValue(data, 8))
    	{
    		Util::SetBitValue(fault, 11, true);//IGBT过温
    	}
    	else
    	{
    		Util::SetBitValue(fault, 11, false);//IGBT过温
    	}

    	Util::SetBitValue(fault, 12, Util::GetBitValue(data, 9));//Bat_OV
    	Util::SetBitValue(fault, 13, Util::GetBitValue(data, 10));//Bus_OV
    	Util::SetBitValue(fault, 14, Util::GetBitValue(data, 12));//Bat_Charge_OC
    	Util::SetBitValue(fault, 15, Util::GetBitValue(data, 13));//Bat_Discha_OC
    	Util::SetBitValue(fault, 16, Util::GetBitValue(data, 14));//L_OT
    	Util::SetBitValue(fault, 17, Util::GetBitValue(data, 15));//Tran_OT

    	data = (short)values[4];
    	Util::SetBitValue(fault, 18, Util::GetBitValue(data, 0));//Hardware_Fault
    	Util::SetBitValue(fault, 19, Util::GetBitValue(data, 1));//SPD_Fault
    	Util::SetBitValue(fault, 20, Util::GetBitValue(data, 7));//VacOut_OV
    	Util::SetBitValue(fault, 21, Util::GetBitValue(data, 8));//VacOut_LV
    }
    else
    {
        comm = false;
        zlog_warn(Util::m_zlog,"读取%s故障数据失败",dev->name.c_str());
    }

    if(fault != 0)
    {
    	MemDb::SetRealData(regStart + 45, 1, false);//故障 45
    	MemDb::SetRealData(regStart + 46, fault, false);//故障码46
    }
    else
    {
    	MemDb::SetRealData(regStart + 45, 0, false);//故障
    	MemDb::SetRealData(regStart + 46, fault, false);//故障码
    }


	//读取累计数据
	start = 770;
	length = 4;
    ret = client.ReadInputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret)
    {
    	data = (((short)values[1]) * 65536) + ((short)values[0]);
    	memcpy(&tmp,&data,sizeof(int));
    	tmp = tmp / (3600 * 1000);
    	MemDb::SetRealData(regStart + 47, tmp, false);//累计充电量
    	data = (((short)values[3]) * 65536) + ((short)values[2]);
    	memcpy(&tmp,&data,sizeof(int));
    	tmp = tmp / (3600 * 1000);
    	MemDb::SetRealData(regStart + 48, tmp, false);//累计放电量
    }
    else
    {
        comm = false;
        zlog_warn(Util::m_zlog,"读取%s累计数据失败",dev->name.c_str());
    }

	//读取给定值
	start = 504;
	length = 8;
    ret = client.ReadInputRegisters(values, addr, start, length);
    if (ErrorInfo::ERR_OK == ret)
    {
    	data = (((short)values[1]) * 65536) + ((short)values[0]);
    	memcpy(&tmp,&data,sizeof(int));
    	MemDb::SetRealData(regStart + 49, tmp, false);//有功功率给定
    	data = (((short)values[3]) * 65536) + ((short)values[2]);
    	memcpy(&tmp,&data,sizeof(int));
       	MemDb::SetRealData(regStart + 50, tmp, false);//无功功率给定
      	data = (((short)values[5]) * 65536) + ((short)values[4]);
		memcpy(&tmp,&data,sizeof(int));
		MemDb::SetRealData(regStart + 51, tmp, false);//离网电压给定
      	data = (((short)values[7]) * 65536) + ((short)values[6]);
		memcpy(&tmp,&data,sizeof(int));
		MemDb::SetRealData(regStart + 52, tmp, false);//离网频率给定
    }
    else
    {
        comm = false;
        zlog_warn(Util::m_zlog,"读取%s给定值失败",dev->name.c_str());
    }


    MemDb::SetRealData(regStart + 0, comm, false);//通讯状态

    return ret;
}

int EcmPcs::EcmPcsPreset(Device::SlaveDev* dev)
{
	int ret = ErrorInfo::ERR_OK;
	int addr = atoi(dev->slaveAddr.c_str());
	int regStart = dev->regStart;
	int start = 0;
	int length = 0;
	int data = 0;
	int mode = 0;
	float tmp = 0;
	//short tmp = 0;
	RealData realData;
	FloatArray values;
	//ShortArray values;
//	RealData realData;
//	UshortArray values;
	values.clear();
	UshortArray array;
	array.clear();

	//判断串口是否打开
	if (!m_tcpClient->IsOpened())
	{
		return ErrorInfo::ERR_OPENED;
	}

	ModbusTcpClient client;
	client.SetTcpClient(this->m_tcpClient);

	start = 512;
	length = 1;
    ret = client.ReadInputRegisters(array, addr, start, length);
    if (ErrorInfo::ERR_OK == ret)
    {
    	m_state = (short)array[0];
    }

	start = 701;
	length = 1;
    ret = client.ReadInputRegisters(array, addr, start, length);
    if (ErrorInfo::ERR_OK == ret)
    {
    	m_mode = (short)array[0];
    }

    //判断是否有启动停止命令下发
    if (MemDb::GetRealData(realData, regStart + 1, true))
    {
    	Util::SetBitValue(m_state, 0, realData.pv.data.iValue);
        ret = client.PresetSingleRegister(addr, 512, m_state);
    }

    //判断是否有故障复位命令下发
    if (MemDb::GetRealData(realData, regStart + 2, true))
    {
    	Util::SetBitValue(m_state, 14, realData.pv.data.iValue);
        ret = client.PresetSingleRegister(addr, 512, m_state);
    }

	//判断是否有电网并离网切换命令下发
	if (MemDb::GetRealData(realData, regStart + 14, true))
    {
    	Util::SetBitValue(m_state, 1, realData.pv.data.iValue);
        ret = client.PresetSingleRegister(addr, 512, m_state);
    }


     //判断是否有自动电池模拟模式命令下发
    if (MemDb::GetRealData(realData, regStart + 5, true))
    {
    	Util::SetBitValue(m_mode, 11, realData.pv.data.iValue);
        ret = client.PresetSingleRegister(addr, 701, m_mode);
    }
	
	    //判断是否有电池模拟器模式命令下发
  //  if (MemDb::GetRealData(realData, regStart + 14, true))
   // {
   // 	Util::SetBitValue(m_mode, 6, realData.pv.data.iValue);//
  //      ret = client.PresetSingleRegister(addr, 701, m_mode);
  //  }


    //判断是否有有功功率设定值下发 kw
    if (MemDb::GetRealData(realData, regStart + 6, true))
    {
    	values.clear();
        tmp = realData.pv.data.dbValue;
        values.push_back(tmp);
//    	printf("%02x ", tmp);
//    	printf("\n");
        ret = client.PresetMultipleFloatRegisters(addr, 504, 2, values);
        //ret = client.PresetMultipleRegisters(addr, 504, 2, values);
    }

    //判断是否有无功功率设定值下发 kVar
    if (MemDb::GetRealData(realData, regStart + 7, true))
    {
    	values.clear();
        tmp = realData.pv.data.dbValue;
        values.push_back(tmp);
        ret = client.PresetMultipleFloatRegisters(addr, 506, 2, values);
    }

    //判断是否有离网电压设定值下发
    if (MemDb::GetRealData(realData, regStart + 8, true))
    {
    	values.clear();
        tmp = realData.pv.data.dbValue;
        values.push_back(tmp);
        ret = client.PresetMultipleFloatRegisters(addr, 508, 2, values);
    }

    //判断是否有离网频率设定值下发
    if (MemDb::GetRealData(realData, regStart + 9, true))
    {
    	values.clear();
        tmp = realData.pv.data.dbValue;
        values.push_back(tmp);
        ret = client.PresetMultipleFloatRegisters(addr, 510, 2, values);
    }

    //判断是否有电池模拟器输出电压设定值下发
    if (MemDb::GetRealData(realData, regStart + 10, true))
    {
    	values.clear();
        tmp = realData.pv.data.dbValue;
        values.push_back(tmp);
        ret = client.PresetMultipleFloatRegisters(addr, 810, 2, values);
    }

    //判断是否有电池模拟器内阻设定值下发
    if (MemDb::GetRealData(realData, regStart + 11, true))
    {
    	values.clear();
        tmp = realData.pv.data.dbValue;
        values.push_back(tmp);
       ret = client.PresetMultipleFloatRegisters(addr, 812, 2, values);
    }

    //判断是否有自动电池模拟母线电压上限设定值下发
    if (MemDb::GetRealData(realData, regStart + 12, true))
    {
        data = (int)realData.pv.data.dbValue;
        ret = client.PresetSingleRegister(addr, 552, data);
    }

    //判断是否有自动电池模拟母线电压下限设定值下发
    if (MemDb::GetRealData(realData, regStart + 13, true))
    {
        data = (int)realData.pv.data.dbValue;
        ret = client.PresetSingleRegister(addr, 553, data);
    }

    //判断是否有工作模式设定值下发
    if (MemDb::GetRealData(realData, regStart + 3, true))
    {
        data = (int)realData.pv.data.iValue;
        if((data == 1) || (data == 2))
        {
        	Util::SetBitValue(m_mode, 6, false);
        }
        else if((data == 3) || (data == 4))
        {
        	Util::SetBitValue(m_mode, 6, true);
        }
        ret = client.PresetSingleRegister(addr, 701, m_mode);
    }

    MemDb::GetRealData(mode, regStart + 3);
    MemDb::SetRealData(regStart + 4, mode, false);//读工作模式

    return ret;
}
