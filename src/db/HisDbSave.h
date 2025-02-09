/*
 * HisDbSave.h
 *
 *   创建日期: 2013-2-27
 *   作         者: mzh
 */

#ifndef HISDBSAVE_H_
#define HISDBSAVE_H_
#include "Db.h"

/*名称：HisDbSave
 *描述：将实时数据库中数据定时保存到历史数据库中
 *使用方法：
 *由HisDbSaveThread调用
 *
 *创建人：mzh
 *创建日期：2013-2-27
 */
class HisDbSave: public Db
{
public:
	HisDbSave();
	virtual ~HisDbSave();

    /*
     *  GetInstance
     *  功能:得到实例指针
     *  输入：无
     *  输出：返回Db*指针，
     */
    static HisDbSave* GetInstance();

    /*
     *  FreeInstanse
     *  功能:关闭类指针，释放内存
     *  输入：无
     *  输出：无
     */
    void FreeInstanse();

	/*
	 * 名称: Init
	 * 功能: 初始化配置
	 * 输出: 成功/失败
	 */
	bool Init();

	/*
	 * 名称: SaveWarn
	 * 功能: 保存报警/故障数据
	 * 输出: 成功/失败
	 */
	bool SaveWarn();

	/*
	 * 名称: SaveLevel1Data
	 * 功能: 保存一级数据
	 * level   :[in]int 设备级别
	 * 输出: 成功/失败
	 */
	bool SaveLevelData(int level);

	/*
	 * 名称: GetLevel1Cycle
	 * 功能: 返回一级设备保存周期
	 * 输出: 成功/失败
	 */
	int GetLevel1Cycle();

	/*
	 * 名称: GetLevel1Cycle
	 * 功能: 返回一级设备保存周期
	 * 输出: 成功/失败
	 */
	int GetLevel2Cycle();

	/*
	 * 名称: GetLevel1Cycle
	 * 功能: 返回一级设备保存周期
	 * 输出: 成功/失败
	 */
	int GetLevel3Cycle();

protected:
private:

	/*
	 * 名称: InitHisConfig
	 * 功能: 读取历史数据参数配置,
	 * 输出: 成功/失败
	 */
	bool InitHisConfig();

	/*
	 * 名称: SaveData
	 * 功能: 保存历史数据
	 * tb     :[in]保存的表名
	 * sn     :[in]设备编号
	 * start  :[in]实时数据库中起始地址
	 * len    :[in]设备数据长度
	 * 输出: 成功/失败
	 */
	bool SaveData(string tb,int sn,int start,int len);

	//变量
public:
	//历史数据设备保存时间和日期
	typedef struct HisDev
	{
		string dev;//设备名称
		int level;//设备保存级别1、2、3级
		int save;//保存天数
		int cycle;//保存周期
	}HisDev;

protected:
private:
	//历史数据配置信息
	int m_warningSave;//报警和操作数据保存天数
	int m_level1Cycle;//一级数据保存周期，单位s
	int m_level1Save;//一级数据保存时间，单位天
	int m_level2Cycle;//二级数据保存周期，单位s
	int m_level2Save;//二级数据保存时间，单位天
	int m_level3Cycle;//三级数据保存周期，单位s
	int m_level3Save;//三级数据保存时间，单位天

	HisDev m_hisDev[ARRAY_LENGTH_128];//设备
	int m_hisDevNum;//设备数量

	//单例模式
	static HisDbSave* m_hisDb;//配置数据库指针
};

/*名称：HisDbSaveThread
 *描述：线程类，主要用于将实时数据转存到历史数据库中
 *使用方法：
 *1、实例化
 *HisDbSaveThread saveThread;
 *2、启动线程
 *saveThread.Start();
 *3、用完后停止线程
 *saveThread.Stop();
 *创建人：mzh
 *创建日期：2013-2-27
 */
class HisDbSaveThread
{
	//方法
public:
	HisDbSaveThread();
	virtual ~HisDbSaveThread();

    /*
     *  Start
     *  功能:启动线程
     *  输入：无
     *  输出：成功/失败
     */
    bool Start();

    /*
     *  Stop
     *  功能:终止线程运行
     *  输入：无
     *  输出：无
     */
    void Stop();

protected:
private:

    /*
     *  Work
     *  功能:线程运行
     *  输入：无
     *  输出：无
     */
    void Work();

    /*
     *  Work
     *  功能:线程运行
     *  输入：无
     *  输出：无
     */
    static void* Run(void* param);

	//变量
public:

protected:
private:

	//线程操作
	HANDLE m_handle;//线程句柄
	bool m_running;//线程是否正在运行
	bool m_flagExit;//设置线程退出标志
    //用于线程挂起
    pthread_mutex_t    m_mutex;//互斥，防止其他变量唤醒
    pthread_cond_t     m_cond;  //信号
};

#endif /* HISDBSAVE_H_ */
