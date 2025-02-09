/*
 * MemDb.h
 *
 *   创建日期: 2013-9-30
 *   作          者: 马中华
 */

#ifndef MEMDB_H_
#define MEMDB_H_

#include "../util/Util.h"
#include <time.h>

//实时数据
typedef struct RealData
{
	//数据类型
	typedef enum DATA_TPYE
	{
		//整形int
		INT = 4,
		//float类型
		DB = 6
	} DATA_TPYE;

	//定义可变变量
	typedef struct Variant
	{
		union Data
		{
			int iValue;
			float dbValue;
		};
		DATA_TPYE type;
		Data data;
		Data data_last;
	}Variant;

	int addr;//地址，从0开始
	time_t now;//当前时间
	Variant pv;//当前值
	bool writed;//被写入
	bool readed;//被读出

	int writed_num;//writed==1，此时即使在WRITED_NUM内读取标志位时不写入

}RealData;

/*名称:MemDb
 *描述:用于内存数据库操作
 *用法:
 *创建人:马中华
 *创建日期:2013-9-30
 */
class MemDb
{
public:
	MemDb();
	virtual ~MemDb();

	/*
	 * 名称: Init
	 * 功能: 完成共享内存数据库初始化工作
	 * 输入: 无
	 * 输出: 成功/失败
	 */
	static bool Init();

	/*
	 * 名称: Uninit
	 * 功能: 完成共享内存数据库反初始化工作
	 * 输入: 无
	 * 输出: 成功/失败
	 */
	static void Uninit();

	/*
	 * 名称: GetRealData
	 * 功能: 读取实时数据
	 * 输入:
	 * data       :[out]RealData& 实时数据
	 * addr       :[in]int 实时数据地址,从0开始
	 * 输出: 成功失败
	 */
	static bool GetRealData(RealData& data,int addr);
	static bool GetRealData(int& data,int addr);
	static bool GetRealData(float& data,int addr);

	/*
	 * 名称: GetRealDataWrited
	 * 功能: 读取设置writed标志位的数据
	 * 输入:
	 * data       :[out]RealData& 实时数据
	 * addr       :[in]int 实时数据地址,从0开始
	 * clearWrite :[in]bool 清空写标志位
	 * 输出: 超出范围和没有设置writed标志位返回失败/读取设置writed标志位数据返回成功
	 */
	static bool GetRealData(RealData& data, int addr,bool clearWrited);//读取写入数据
	static bool GetRealDataWrited(RealData& data, int addr,bool clearWrited);//读取写入数据
	static bool GetRealDataWrited(int addr);//判断是否有写入
	/*
	 * 名称: GetRealData
	 * 功能: 读取实时数据
	 * 输入:
	 * data       :[out]RealData* 读取数组
	 * num        :[in]int data数量
	 * addr       :[in]int 实时数据地址,从0开始
	 * length     :[in]int 读取实时数据长度
	 * 输出: RealData实时数据
	 */
	static bool GetRealData(RealData* data,int num,int addr,int length);

	/*
	 * 名称: SetRealData
	 * 功能: 设置实时数据
	 * 输入:
	 * addr        :[in]int 实时数据地址,从0开始
	 * data        :[in]   保存值
	 * writed      :[in]bool上位机写入数据
	 * 输出: 成功/失败
	 */
	static bool SetRealDataUnknown(int addr,int data,bool writed);//根据已经保存的类型保存数据
	static bool SetRealData(int addr,int data);
	static bool SetRealData(int addr,float data);
	static bool SetRealData(int addr,int data,bool writed);
	static bool SetRealData(int addr,float data,bool writed);
	static bool SetRealData(int addr,RealData& data);

	/*
	 * 名称: SetRealDataBit
	 * 功能: 设置实时数据的某一位
	 * 输入:
	 * addr        :[in]int  实时数据地址,从0开始
	 * bit         :[in]int  某一bit位,0-31之间
	 * data        :[in]bool 该bit位属性值：true/false
	 * 输出: 成功/失败
	 */
	static bool SetRealDataBit(int addr,int bit,bool data);

private:

	/*
	 * 名称: OpenMem
	 * 功能: 开启内存共享
	 * 输入: 无
	 * 输出: 成功/失败
	 */
	static bool OpenMem();

	/*
	 * 名称: CheckAddr
	 * 功能: 检查地址是否越限
	 * 输入: 无
	 * 输出: 成功/失败
	 */
	static bool CheckAddr(int addr);

	/*
	 * 名称: Lock
	 * 功能: 互斥加锁
	 * 输入:无
	 * 输出: 无
	 */
	static void Lock();
	/*
	 * 名称: Lock
	 * 功能: 互斥解锁
	 * 输入: 无
	 * 输出: 无
	 */
	static void Unlock();

public:
private:
	//实时数据表
	static RealData* m_realData;
	static int m_realDataId;
	static pthread_mutex_t m_mutexRealData;
};

#endif /* MEMDB_H_ */
