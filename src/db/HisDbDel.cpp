/*
 * HisDbDel.cpp
 *
 *   创建日期: 2013-2-27
 *   作         者: mzh
 */

#include "HisDbDel.h"
#include "Db.h"

HisDbDel::HisDbDel()
{
	// TODO 构造函数
	//历史数据配置信息
	m_warningSave = 7;//报警和操作数据保存天数
	m_level1Cycle = 300;//一级数据保存周期，单位s
	m_level1Save = 7;//一级数据保存时间，单位天
	m_level2Cycle = 300;//二级数据保存周期，单位s
	m_level2Save = 7;//二级数据保存时间，单位天
	m_level3Cycle = 300;//三级数据保存周期，单位s
	m_level3Save = 7;//三级数据保存时间，单位天
	memset(m_hisDev, '\0', sizeof(HisDev) * ARRAY_LENGTH_128);

	m_hisDevNum = 15;//目前设备类型15种
	m_hisDev[0].dev = "env";
	m_hisDev[1].dev = "ammete";
	m_hisDev[2].dev = "dido";
	m_hisDev[3].dev = "pvs";
	m_hisDev[4].dev = "pcs";
	m_hisDev[5].dev = "box";
	m_hisDev[6].dev = "dcdc";
	m_hisDev[7].dev = "wt";
	m_hisDev[8].dev = "bms";
	m_hisDev[9].dev = "bms_temp";
	m_hisDev[10].dev = "bms_v";
	m_hisDev[11].dev = "eis";
	m_hisDev[12].dev = "acpower";
	m_hisDev[13].dev = "gen";
	m_hisDev[14].dev = "load";

}

HisDbDel::~HisDbDel()
{
	// TODO 析构函数
}

bool HisDbDel::Init()
{
	zlog_info(Util::m_zlog, "配置信息初始化");
	bool ret = false;

	ret = this->InitHisConfig();

	zlog_info(Util::m_zlog, "配置信息初始化%s",ret?"成功":"失败");
	return ret;
}

bool HisDbDel::InitHisConfig()
{
	zlog_info(Util::m_zlog, "从配置数据库中，读取历史数据配置信息");

	//局部变量
	ConfigDb* db = (ConfigDb*) ConfigDb::GetInstance();//配置数据库实例

	//1、打开数据库
	if (!db->IsOpen())
	{
		if (db->Open())
		{
			zlog_warn(Util::m_zlog, "配置数据库打开失败");
			return false;
		}
	}

	string type;
	const string tb = "sys_his";//历史数据配置信息表

	int value;
	bool ret = true;
	bool ret1 = false;

	//1、报警和操作数据保存天数
	type = "his_warning_save";
	zlog_info(Util::m_zlog, "读取系统参数配置，类型=%s",type.c_str());
	ret1 = db->GetValue(value, type, tb);
	ret = ret && ret1;
	if (ret1)
	{
		m_warningSave = value;
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取系统参数配置，类型=%s失败",type.c_str());
	}

	//2、一级数据保存周期，单位s
	type = "his_level_1_cycle";
	zlog_info(Util::m_zlog, "读取系统参数配置，类型=%s",type.c_str());
	ret1 = db->GetValue(value, type, tb);
	ret = ret && ret1;
	if (ret1)
	{
		m_level1Cycle = value;
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取系统参数配置，类型=%s失败",type.c_str());
	}

	//3、一级数据保存时间，单位天
	type = "his_level_1_save";
	zlog_info(Util::m_zlog, "读取系统参数配置，类型=%s",type.c_str());
	ret1 = db->GetValue(value, type, tb);
	ret = ret && ret1;
	if (ret1)
	{
		m_level1Save = value;
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取系统参数配置，类型=%s失败",type.c_str());
	}

	//4、二级数据保存周期，单位s
	type = "his_level_2_cycle";
	zlog_info(Util::m_zlog, "读取系统参数配置，类型=%s",type.c_str());
	ret1 = db->GetValue(value, type, tb);
	ret = ret && ret1;
	if (ret1)
	{
		m_level2Cycle = value;
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取系统参数配置，类型=%s失败",type.c_str());
	}

	//5、二级数据保存时间，单位天
	type = "his_level_2_save";
	zlog_info(Util::m_zlog, "读取系统参数配置，类型=%s",type.c_str());
	ret1 = db->GetValue(value, type, tb);
	ret = ret && ret1;
	if (ret1)
	{
		m_level2Save = value;
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取系统参数配置，类型=%s失败",type.c_str());
	}

	//6、三级数据保存周期，单位s
	type = "his_level_3_cycle";
	zlog_info(Util::m_zlog, "读取系统参数配置，类型=%s",type.c_str());
	ret1 = db->GetValue(value, type, tb);
	ret = ret && ret1;
	if (ret1)
	{
		m_level3Cycle = value;
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取系统参数配置，类型=%s失败",type.c_str());
	}

	//7、三级数据保存时间，单位天
	type = "his_level_3_save";
	zlog_info(Util::m_zlog, "读取系统参数配置，类型=%s",type.c_str());
	ret1 = db->GetValue(value, type, tb);
	ret = ret && ret1;
	if (ret1)
	{
		m_level3Save = value;
	}
	else
	{
		zlog_warn(Util::m_zlog, "读取系统参数配置，类型=%s失败",type.c_str());
	}

	//读取设备保存级别
	int i = 0;
	for (i = 0; i < m_hisDevNum; i++)
	{
		zlog_info(Util::m_zlog, "读取系统参数配置，类型=%s",m_hisDev[i].dev.c_str());
		ret1 = db->GetValue(value, m_hisDev[i].dev, tb);
		ret = ret && ret1;
		if (ret1)
		{
			m_hisDev[i].level = value;
			switch (m_hisDev[i].level)
			{
			case 1:
				m_hisDev[i].save = m_level1Save;
				break;
			case 2:
				m_hisDev[i].save = m_level2Save;
				break;
			case 3:
				m_hisDev[i].save = m_level3Save;
				break;
			default:
				m_hisDev[i].save = m_level1Save;
				break;
			}
		}
		else
		{
			zlog_warn(Util::m_zlog, "读取系统参数配置，类型=%s失败",m_hisDev[i].dev.c_str());
		}
	}

	//关闭数据库
	db->Close();

	return ret;
}

/*
 * 名称: DelHisData
 * 功能: 删除过期历史数据
 * 输出: bool 成功/失败
 */
bool HisDbDel::DelHisData()
{
	zlog_info(Util::m_zlog, "删除历史数据库超期历史数据");
	bool ret = false;
//	bool ret1 = false;

	//局部变量
	HisDb* db = (HisDb*) HisDb::GetInstance();//配置数据库实例

	//1、打开数据库
	if (!db->IsOpen())
	{
		if (db->Open())
		{
			zlog_warn(Util::m_zlog, "历史数据库打开失败");
			return false;
		}
	}

//	zlog_info(Util::m_zlog, "执行删除历史数据");
//	int i = 0;
//	int save = 0;
//	string tb;
//	for (i = 0; i < Device::m_devTypeNum; i++)
//	{
//		tb=Device::m_devType[i].tb;
//		if(tb.length()>0)//防止控制符串
//		{
//			switch (Device::m_devType[i].level)
//			{
//			case LEVEL_1:
//				save = m_level1Save;
//				break;
//			case LEVEL_2:
//				save = m_level2Save;
//				break;
//			case LEVEL_3:
//				save = m_level3Save;
//				break;
//			default:
//				save = m_level3Save;
//				break;
//			}
//
//			zlog_info(Util::m_zlog, "删除历史数据库中表%s的超过%d天数据",tb.c_str(),save);
//			ret1 = DelHisData(save, tb, db);
//			ret = ret && ret1;
//		}
//	}

	//关闭数据库
	zlog_info(Util::m_zlog, "关闭历史数据库");
	db->Close();

	zlog_info(Util::m_zlog, "删除历史数据库超期历史数据结束");
	return ret;
}

/*
 * 名称: DelHisData
 * 功能: 删除过期历史数据
 * day       :[in]int 保存时间长度
 * tb        :[in]string 表名
 * 输出: bool 成功/失败
 */
bool HisDbDel::DelHisData(const int day, const string tb, Db* db)
{
	zlog_info(Util::m_zlog, "删除历史数据库表%s的超过%d天历史数据",tb.c_str(),day);
	if ((NULL == db) || (!db->IsOpen()))
	{
		zlog_warn(Util::m_zlog, "历史数据库指针为NULL或者没有打开");
		return false;
	}

	bool ret = false;
	string sql;
	char tmp[ARRAY_LENGTH_512];
	sprintf(tmp,"delete from [%s] where time < date('now','localtime','-%d day') ",
			tb.c_str(), day);
	sql=tmp;

	//预处理sql语句\即将sql语句进行解析成字节
	zlog_info(Util::m_zlog, "预处理sql语句=%s",sql.c_str());
	ret = db->Prepare(sql);
	if (!ret)
	{
		zlog_warn(Util::m_zlog, "预处理sql语句失败");
		return false;
	}

	//执行删除语句
	db->Step();

	//关闭stmt
	db->Finalize();

	zlog_info(Util::m_zlog, "删除历史数据库表%s的超过%d天历史数据成功",tb.c_str(),day);

	return true;

}
