/**
 * *    文件      : CNCU.h
 * *    Synopsis  : 
 * *    作者      : Mingyu Wu
 * *    version   : 1.0.0
 * *    日期      : 2021-02-05
 * */


#ifndef CNCU_H_
#define CNCU_H_

#include "../../../db/Device.h"
#define SUM_CNCU_ESC 4
#define IS_TEST_IN__A811 0  // 是否在办公室测试
class Com;
class TcpClient;
class CNCU{
	public:
		CNCU();
		virtual ~CNCU();
		
		//设置是否采用以太网通讯
		void SetIsTcp(bool isTcp);

		//保存串口指针
		void SetCom(Com* com);

		// 设置TcpClient指针
		void SetTcp(TcpClient* tcpClient);

		int Read(Device::SlaveDev* dev); // DEVICE_TYPE_CNCU 复合节点控制器
		int DEXNRead(Device::SlaveDev* dev); // 制造厂家德意新能
		int DEXNSysRead(Device::SlaveDev* dev);
		int ACDCRead(Device::SlaveDev* dev); // ACDC 复合节点控制器
		int ESCRead(Device::SlaveDev* dev); // 储能DCDC读数据 

		/* ==========================================================================*/
		/**
		 *    功 能       :		SubESCReadInputStatus 
		 *						循环读取ESC每个子模块的02功能
		 *
		 *    Param       :		regStart	控制器寄存器地址
		 *    Param       :		start		CNCU寄存器地址
		 *    Param       :		addr		CNCU设备从地址
		 *    Param       :		subESCcout	子系统模块数量
		 *
		 *    Returns     :		成功返回0， 
		 *						失败返回读取失败子模块的序号。	
		 */
		/* ==========================================================================*/
		int SubESCReadInputStatus(int regStart, int start, int addr);


		/* ==========================================================================*/
		/**
		 *    功 能       :		SubESCReadInputStatus 
		 *						循环读取ESC每个子模块的04功能
		 *
		 *    Param       :		regStart	控制器寄存器地址
		 *    Param       :		start		CNCU寄存器地址
		 *    Param       :		addr		CNCU设备从地址
		 *    Param       :		subESCcout	子系统模块数量
		 *
		 *    Returns     :		成功返回0， 
		 *						失败返回读取失败子模块的序号。	
		 */
		/* ==========================================================================*/
		int SubESCReadInputRegisters(int regStart, int start, int addr);
		int PVCRead(Device::SlaveDev* dev); // 光伏DCDC读数据

		int Preset(Device::SlaveDev* dev); 
		int ACDCPreset(Device::SlaveDev* dev); // ACDC 控制
		int ESCPreset(Device::SlaveDev* dev); // ESC 写函数
		int PVCPreset(Device::SlaveDev* dev); // 

		/* ==========================================================================*/
		/**
		 * *    功 能       :		OverallRead 
		 *							读取整体系统数据去
		 * *    Param       :		dev 设备元素
		 * *    Returns     :		成功返回0，失败返回1；		
		 * */
		/* ==========================================================================*/
		int OverallRead(Device::SlaveDev* dev);

		/* ==========================================================================*/
		/**
		 * *    功 能       :		OverallPreset 
		 *							设置整体系统数据区
		 * *    Param       :		dev 设备元素
		 * *    Returns     :		成功返回0， 失败返回1
		 * */
		/* ==========================================================================*/
		int OverallPreset(Device::SlaveDev* dev);
	


	protected:

	private:
		Com* m_com;
		bool m_isTcp;
		TcpClient* m_tcpClient; //以太网用于串口服务器


};
#endif
