/*
 * Iec103.h
 *
 *  Created on: 2013-4-2
 *      Author: mzh
 */

#ifndef IEC103_H_
#define IEC103_H_
#include "../util/Util.h"

#define ANY_TYPE 255
#define BROADCAST_ADDR 255

class ControlClient;
class TcpClient;
class Thread;

//微网控制器配置信息
typedef struct ControllerConfig
{
	int controllerAddr;//微电网控制器地址
	int controllerType;//控制器类型
	int sysType;//集中器/柴油发电系统/负荷系统/光伏系统/风电系统等
	string ipA;//A网段IP地址
	string ipB;//B网段IP地址

	bool networkAUsed;//A网true使用/false未使用
	bool networkBUsed;//B网true使用/false未使用

	//通过ipA/ipB计算得出
	string ipBroadcastA; //A网广播地址
	string ipBroadcastB; //B网广播地址

	int regStart;//保存寄存器起始地址
	int regLength;//保存寄存器长度

	ControllerConfig()
	{
		controllerAddr = 0;//微电网控制器地址
		sysType = 0;//集中器/柴油发电系统/负荷系统/光伏系统/风电系统等
		ipA = "0.0.0.0";//A网段IP地址
		ipB = "0.0.0.0";//B网段IP地址

		networkAUsed = true;//A网正在使用
		networkBUsed = true;//B网正在使用

		ipBroadcastA = "0.0.0.0"; //A网广播地址
		ipBroadcastB = "0.0.0.0"; //B网广播地址
	}

} ControllerConfig;

//从站配置信息
typedef struct SlaveConfig
{
	ControllerConfig config;

	Thread* thread;//线程指针

	bool isRun;//主站是否与从站建立通讯
	bool flag; //收到从站返回的状态信息
	unsigned short dataNumber;//数据编号

	int networkA;//A网通讯状态，1代表正常,2表示异常，0表示本网未使用
	int networkB;//B网通讯状态，1代表正常,2表示异常，0表示本网未使用
	SlaveConfig()
	{
		isRun = false;
		flag = false;
		dataNumber = 0;
		thread = NULL;
		networkA = 0;
		networkB = 0;
	}

} SlaveConfig;

//主站配置信息
typedef struct MasterConfig
{
	ControllerConfig config;

	Thread* thread;//线程指针

	bool isRun;//从站是否与主站建立通讯,以TCP控制为准
	bool flag; //收到主站的状态信息
	unsigned short dataNumber;//数据编号

	int networkA;//A网通讯状态，1代表正常,2表示异常，0表示本网未使用
	int networkB;//B网通讯状态，1代表正常,2表示异常，0表示本网未使用
	MasterConfig()
	{
		isRun = false;
		flag = false;
		dataNumber = 0;
		thread = NULL;
		networkA = 0;
		networkB = 0;
	}

} MasterConfig;

/*名称:Iec103
 *描述:站内通讯协议兼容103规约解析
 *    本规约采用固定和可变帧长帧格式
 *用法:
 *创建人:mzh
 *创建日期:2013-4-1
 */
class Iec103
{
public:

	typedef enum NetworkState
	{
		NO_USED,//没有使用
		NORMAL,//通讯正常
		ABNORMAL
	//通讯异常
	} NetworkState;

	//帧类型
	typedef enum PacketType
	{
		TYPE_NULL = 0,//未知类型
		CONFIRM,//确认帧
		TIME_SYNC,//时间同步帧
		REAL_DATA,//实时数据主动上传
		CTRL_DATA
	//控制数据下发帧
	} PacketType;

	//帧格式
	typedef enum PacketFormat
	{
		FIXED_LENGTH = 1,//固定格式
		VARIABLE_LENGTH,
	//	变长格式
	} PacketFormat;

	//数据格式,连续地址/单独地址
	typedef enum AddressContinuous
	{
		CONTINUOUS_ADDR = 0,//连续地址
		ALONE_ADDR,
	//	每个数据单独地址
	} AddressContinuous;

	typedef struct PacketHead
	{
		int length;//字节上长度

		int srcAddr;//源设备地址
		int srcType;//源设备类型
		int destAddr;//目的设备地址
		int destType;//目的设备类型
		unsigned short dataNumber;//数据编号
		int networkA;//A网通讯状态
		int networkB;//B网通讯状态

		int backup1;//备用1
		int backup2;//备用2

		PacketHead()
		{
			length = 0;
			srcAddr = 0;
			srcType = 255;//不限类型
			destAddr = 0;
			destType = 255;//不限类型
			dataNumber = 0;
			networkA = 0;
			networkB = 0;
			backup1 = 0;
			backup2 = 0;
		}
	} PacketHead;

	typedef struct PacketData
	{
		PacketType type;//帧类型

		int destAddr;//目的地址
		int dataStart;//数据起始地址
		int dataLength;//数据长度

		int continuous;//0连续地址/1表示单独地址
		int prm;//启动报文位,1由主站向从站传输/0由从站设备向主站传输
		int fcb;//帧记数位:主站每向从站发送新一轮的“发送/确认”或“请求/响应”传输服务时，将FCB取反
		int fcv;//帧记数有效位
		int fun;//功能码
		String data;//数据

		PacketData()
		{
			type = TYPE_NULL;
			destAddr = 0;
			dataStart = 0;
			dataLength = 0;
			continuous = CONTINUOUS_ADDR;
			prm = 0;
			fcb = 0;
			fcv = 0;
			fun = 0;
			data.clear();
		}
	} PacketData;

public:
	Iec103();
	virtual ~Iec103();

	/*
	 * 名称: Pack
	 * 功能: 封装帧
	 * 输入:
	 * head         :[in]const PacketHead* 包头部分
	 * data         :[in],const PacketData* 数据体部分,为NULL表示只有包头没有数据体
	 * 输出: String 封装后的数据,失败字符串为空
	 */
	String Pack(const PacketHead* head, const PacketData* data);

	/*
	 * 名称: Unpack
	 * 功能: 解析数据帧
	 * 输入:
	 * head              :[out]const PacketHead& 包头部分
	 * data              :[out]PacketData& 数据体部分
	 * dataUnpacked      :[in]const String& 被解析时间同步帧
	 * 输出: true解包成功/false解包失败(校验没有通过等原因)
	 */
	bool Unpack(PacketHead& head, PacketData& data, const String& dataUnpacked);

	/*
	 * 名称: DataComparison
	 * 功能: 用于数据编号比较
	 * 输入:
	 * dataNumber1       :[in]数据编号1
	 * dataNumber2       :[in]数据编号2
	 * 输出: true表示dataNumber1大于dataNumber2/false小于等于
	 */
	bool DataComparison(int dataNumber1, int dataNumber2);

	//数据编号自动加1
	void DataNumber(int& dataNumber);
protected:

	/*
	 * 名称: PackTimeSync
	 * 功能: 封装时间同步帧
	 * 输入:
	 * head         :[in]const PacketHead* 包头部分
	 * data         :[in]PacketData* 数据体部分
	 * 输出: String 封装后的数据,失败字符串为空
	 */
	String PackTimeSync(PacketHead* head, PacketData* data);

	/*
	 * 名称: PackRealData
	 * 功能: 封装实时数据
	 * 输入:
	 * head         :[in]const PacketHead* 包头部分
	 * data         :[in]PacketData* 数据体部分
	 * 输出: String 封装后的数据,失败字符串为空
	 */
	String PackRealData(PacketHead* head, PacketData* data);

	/*
	 * 名称: PackCtrlData
	 * 功能: 封装控制数据
	 * 输入:
	 * head         :[in]const PacketHead* 包头部分
	 * data         :[in]PacketData* 数据体部分
	 * 输出: String 封装后的数据,失败字符串为空
	 */
	String PackCtrlData(PacketHead* head, PacketData* data);

	/*
	 * 名称: PackConfirm
	 * 功能: 封装确认帧,固定字节
	 * 输入:
	 * head         :[in]const PacketHead* 包头部分
	 * data         :[in]PacketData* 数据体部分
	 * 输出: String 封装后的数据,失败字符串为空
	 */
	String PackConfirm(PacketHead* head, PacketData* data);

	/*
	 * 名称: PackAsdu
	 * 功能: 对ASDU包进行封装
	 * 输入:
	 * data     :[in]const String*被封装数据
	 * format   :[in]const PacketFormat&封装格式:固定格式/变长格式
	 * 输出: String 封装后的数据,失败字符串为空
	 */
	String PackAsdu(const String* data, const PacketFormat& format);

	/*
	 * 名称: UnpackAsdu
	 * 功能: 对ASDU包校验/解析数据,返回控制域/地址域和链路用户数据域
	 * 输入:
	 * data              :[out]String& 解析后的数据
	 * format            :[out]PacketFormat&封装格式:固定格式/变长格式
	 * dataUnpacked      :[in]const string& 被解析数据
	 * 输出: true解包成功/false解包失败(校验没有通过等原因)
	 */
	bool UnpackAsdu(String& data, PacketFormat& format,
			const String& dataUnpacked);

	/*
	 * 名称: PackHead
	 * 功能: 添加包头,即APCI部分
	 * 输入:
	 * head         :[in]const PacketHead* 包头部分
	 * data         :[in]const String*被封装数据
	 * 输出: String 封装后的数据
	 */
	String PackHead(const PacketHead* head, const String* data);

	/*
	 * 名称: PackHead
	 * 功能: 解析包头部分,并进行校验,即APCI部分
	 * 输入:
	 * head         :[out]PacketHead& 解析的包头部分
	 * data         :[out]String&解析后的数据
	 * dataUnpacked :[in]const String& 被解析数据
	 * 输出: String 封装后的数据
	 */
	bool UnpackHead(PacketHead& head, String& data, const String& dataUnpacked);

private:
	//定义起始字节
	const int m_apciHead;//数据包的起始字节
	const int m_asduFixedHead;//固定格式ASDU数据起始字节
	const int m_asduVariableHead;//变长格式ASDU数据起始字节
	const int m_asduEnd;//ASDU数据结束符
};

#endif /* IEC103_H_ */
