/*
 * HisDbDel.h
 *
 *   创建日期: 2013-2-27
 *   作         者: mzh
 */

#ifndef HISDBDEL_H_
#define HISDBDEL_H_

#include "../util/Util.h"
class Db;

/*名称：HisDbDel
 *描述：将过期历史数据删除
 *使用方法：
 *1、初始化
 *HisDbDel del;
 *del.Init();
 *2、执行删除程序
 *del.DelHisData();
 *创建人：mzh
 *创建日期：2013-2-27
 */
class HisDbDel
{
public:
	HisDbDel();
	virtual ~HisDbDel();

	/*
	 * 名称: Init
	 * 功能: 初始化配置
	 * 输出: 成功/失败
	 */
	bool Init();

	/*
	 * 名称: DelHisData
	 * 功能: 删除过期历史数据
	 * 输出: bool 成功/失败
	 */
	bool DelHisData();

protected:
private:
	/*
	 * 名称: InitHisConfig
	 * 功能: 读取历史数据参数配置,
	 * 输出: 成功/失败
	 */
	bool InitHisConfig();

	/*
	 * 名称: DelHisData
	 * 功能: 删除过期历史数据
	 * day       :[in]int 保存时间长度
	 * tb        :[in]string 表名
	 * 输出: bool 成功/失败
	 */
	bool DelHisData(const int day,const string tb,Db* db);

	//变量
public:
	//历史数据设备保存时间和日期
	typedef struct HisDev
	{
		string dev;//设备名称
		int level;//设备保存级别1、2、3级
		int save;//保存天数
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
};

#endif /* HISDBDEL_H_ */
