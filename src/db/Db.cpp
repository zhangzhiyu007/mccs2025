/*
 * Db.cpp
 *
 *   创建日期: 2013-2-22
 *   作         者: mzh
 */

#include "Db.h"
#include "../etc/sqlite3/sqlite3.h"

//数据库指针
//使用全局变量，可以不再.h文件中显示
static sqlite3* g_dbConn=NULL;//数据库句柄指针
static sqlite3_stmt* g_stmt=NULL;//数据集

Db::Db()
{
	// TODO 构造函数
	g_dbConn = NULL;
	g_stmt = NULL;
	m_dbOpen = false;
	m_dbTimeout = 1000;
	m_isKey = false;
	//默认加密密钥
	m_key = "mazhonghua";
}

Db::~Db()
{
	// TODO 析构函数
	Close();
	g_dbConn = NULL;
	g_stmt = NULL;
}

bool Db::Open()
{
	zlog_info(Util::m_zlog, "打开数据库");
	bool isSuccess = true;//是否打开成功

	isSuccess = OpenDb();
//	if (m_isKey)
//	{
//		isSuccess = isSuccess && Key();
//		this->Rekey();
//	}
	return isSuccess;
}

bool Db::OpenDb()
{
	zlog_info(Util::m_zlog, "打开数据库");

	//如果已经打开数据库，就先关闭
	if (IsOpen())
	{
		zlog_warn(Util::m_zlog, "数据库已经打开,将数据库关闭,重新打开");
		Close();
	}

	bool isSuccess = true;//是否打开成功
	char* errmsg = NULL;//错误信息
	int ret = SQLITE_OK;//返回码

	//打开数据库
	zlog_info(Util::m_zlog, "执行打开数据库%s操作",m_db.c_str());
	ret = sqlite3_open(m_db.c_str(), &g_dbConn);
	if (SQLITE_OK != ret)
	{
		isSuccess = false;
		errmsg = (char*) sqlite3_errmsg(g_dbConn);
		zlog_error(Util::m_zlog, "打开数据库失败,错误码:%s.",errmsg);
	}
	else
	{
		ret = sqlite3_busy_timeout(g_dbConn, m_dbTimeout);
		zlog_info(Util::m_zlog, "打开数据库成功");
	}

	m_dbOpen = isSuccess;
	return isSuccess;
}

bool Db::IsOpen()
{
	return m_dbOpen;
}

void Db::Close()
{
	zlog_info(Util::m_zlog, "关闭数据库");
	if (IsOpen())
	{
		sqlite3_close(g_dbConn);
		g_dbConn = NULL;//数据库句柄
		g_stmt = NULL;
		m_dbOpen = false;
	}

	return;
}

bool Db::Rekey()
{
	zlog_info(Util::m_zlog, "加密数据库修改密钥");

	bool isSuccess = true;//是否打开成功
	char* errmsg = NULL;//错误信息
	int ret = SQLITE_OK;//返回码

	//修改密钥，清空密钥为 sqlite3_rekey( db, NULL, 0)。
	ret = sqlite3_rekey(g_dbConn, m_key.c_str(), m_key.length());
	if (SQLITE_OK != ret)
	{
		isSuccess = false;
		errmsg = (char*) sqlite3_errmsg(g_dbConn);
		zlog_warn(Util::m_zlog, "数据库密钥错误,错误码:%s.",errmsg);
	}
	else
	{
		zlog_info(Util::m_zlog, "数据库密钥正确");
	}

	return isSuccess;
}

bool Db::ClearKey()
{
	zlog_info(Util::m_zlog, "加密数据库修改密钥");

	bool isSuccess = true;//是否打开成功
	char* errmsg = NULL;//错误信息
	int ret = SQLITE_OK;//返回码

	ret = sqlite3_rekey(g_dbConn, NULL, 0);
	if (SQLITE_OK != ret)
	{
		isSuccess = false;
		errmsg = (char*) sqlite3_errmsg(g_dbConn);
		zlog_warn(Util::m_zlog, "数据库密钥错误,错误码:%s.",errmsg);
	}
	else
	{
		zlog_info(Util::m_zlog, "数据库密钥正确");
	}

	return isSuccess;
}

bool Db::Key()
{
	zlog_info(Util::m_zlog, "向加密数据库输入密钥");

	bool isSuccess = true;//是否打开成功
	char* errmsg = NULL;//错误信息
	int ret = SQLITE_OK;//返回码

	//输入密钥
	ret = sqlite3_key(g_dbConn, m_key.c_str(), m_key.length());
	if (SQLITE_OK != ret)
	{
		isSuccess = false;
		errmsg = (char*) sqlite3_errmsg(g_dbConn);
		zlog_warn(Util::m_zlog, "数据库密钥错误,错误码:%s.",errmsg);
	}
	else
	{
		zlog_info(Util::m_zlog, "数据库密钥正确");
	}

	return isSuccess;
}

void Db::SetDb(string db)
{
	m_db = db;
	return;
}

void Db::SetIsKey(bool isKey)
{
	m_isKey = isKey;
}

void Db::SetKey(string key)
{
	m_key = key;
	return;
}

void Db::SetTimeout(int timeout)
{
	if (timeout >= 0)
	{
		m_dbTimeout = timeout;
	}
	return;
}

bool Db::Prepare(string sql)
{
	zlog_info(Util::m_zlog, "预处理sql语句=%s.",sql.c_str());

    int ret = SQLITE_OK;
	sqlite3_stmt* stmt = NULL;//预处理语句对象
	char* errmsg = NULL;//错误信息

	//预处理sql语句\即将sql语句进行解析成字节
	ret = sqlite3_prepare(g_dbConn, sql.c_str(), -1, &stmt,
			(const char**) &errmsg);
	if (SQLITE_OK != ret) {
		zlog_info(Util::m_zlog, "预处理sql语句失败，错误=%s.",sqlite3_errmsg(g_dbConn));
		return false;
	}

	g_stmt = stmt;

	zlog_info(Util::m_zlog, "预处理sql语句=%s成功.",sql.c_str());
	return true;
}

string Db::Errmsg()
{
	string errmsg;
	char errmsgtmp[128];
	sprintf(errmsgtmp, "%s", sqlite3_errmsg(g_dbConn));
	errmsg = errmsgtmp;
	return errmsg;
}

int Db::Step()
{
	return sqlite3_step(g_stmt);
}

void Db::Reset()
{
	sqlite3_reset(g_stmt);
	return;
}

void Db::Finalize()
{
	sqlite3_finalize(g_stmt);
	g_stmt = NULL;
}

bool Db::Begin()
{
	zlog_info(Util::m_zlog, "事务处理开始.");
	bool ret = true;
	string sql = "begin";
	if (!Prepare(sql))
	{
		zlog_warn(Util::m_zlog, "事务处理启动失败.");
		return false;
	}

	if (DB_DONE != Step())
	{
		zlog_warn(Util::m_zlog, "提交失败.");
		ret = false;
	}

	Finalize();

	return ret;
}

bool Db::Commit()
{
	zlog_info(Util::m_zlog, "事务提交.");
	bool ret = true;
	string sql = "commit";
	if (!Prepare(sql))
	{
		zlog_warn(Util::m_zlog, "事务处理启动失败.");
		return false;
	}

	if (DB_DONE != Step())
	{
		zlog_warn(Util::m_zlog, "提交失败.");
		ret = false;
	}

	Finalize();

	return ret;
}

int Db::GetDataCount()
{
	return sqlite3_data_count(g_stmt);
}

int Db::GetColumnCount()
{
	return sqlite3_column_count(g_stmt);
}

void* Db::GetColumnBlob(int i)
{
	return (void*)sqlite3_column_blob(g_stmt,i);
}

int Db::GetColumnBytes(int i)
{
	return sqlite3_column_bytes(g_stmt,i);
}

int Db::GetColumnBytes16(int i)
{
	return sqlite3_column_bytes16(g_stmt,i);
}

int Db::GetColumnInt(int i)
{
	return sqlite3_column_int(g_stmt,i);
}

double Db::GetColumnDouble(int i)
{
	return sqlite3_column_double(g_stmt,i);
}

char* Db::GetColumnChar(int i)
{
	return (char*) sqlite3_column_text(g_stmt, i);
}

string Db::GetColumnText(int i)
{
	string str="";
	char* tmp=(char*) sqlite3_column_text(g_stmt, i);
	if(NULL!=tmp)
	{
		str = tmp;
	}
	return str;
}

bool Db::BindText(int i,string data)
{
	int ret = sqlite3_bind_text(g_stmt, i, data.c_str(), data.length(),
			SQLITE_TRANSIENT);
	if (DB_OK == ret)
	{
		return true;
	}

	return false;
}

bool Db::BindText16(int i,void* data,int num)
{
	int ret = sqlite3_bind_text16(g_stmt, i, data, num, SQLITE_TRANSIENT);
	if (DB_OK == ret)
	{
		return true;
	}

	return false;
}

bool Db::BindInt(int i,int data)
{
	int ret = sqlite3_bind_int(g_stmt, i, data);
	if (DB_OK == ret)
	{
		return true;
	}

	return false;
}

bool Db::BindDouble(int i,double data)
{
	int ret = sqlite3_bind_double(g_stmt, i, data);
	if (DB_OK == ret)
	{
		return true;
	}

	return false;
}

bool Db::BindBlob(int i,void* data,int num)
{
	int ret = sqlite3_bind_text16(g_stmt, i, data, num, SQLITE_TRANSIENT);
	if (DB_OK == ret)
	{
		return true;
	}

	return false;
}

//配置数据库操作
Db* ConfigDb::m_configDb = NULL;
ConfigDb::ConfigDb()
{
	string db;
	char tmp[ARRAY_LENGTH_1024];
	//默认数据库
	char curDir[ARRAY_LENGTH_1024] =
	{ 0 };
	memset(curDir,'\0',ARRAY_LENGTH_1024);
	//读取当前目录
	Util::GetCurDir(curDir, ARRAY_LENGTH_1024);
	sprintf(tmp, "%sdb/config.db", curDir);
	db = tmp;
	SetDb(db);
	this->SetIsKey(true);//设置为加密模式
	//this->SetIsKey(false);//设置为加密模式
	string key("mazhonghua");
	this->SetKey(key);//设置默认加密密钥

}

ConfigDb::~ConfigDb()
{
}

Db* ConfigDb::GetInstance()
{
    if (NULL == ConfigDb::m_configDb)
    {
    	ConfigDb::m_configDb = new ConfigDb();
    }

    return ConfigDb::m_configDb;
}

void ConfigDb::FreeInstanse()
{
    if (NULL != ConfigDb::m_configDb)
    {
        delete ConfigDb::m_configDb;
        ConfigDb::m_configDb = NULL;
    }

    return;
}

bool ConfigDb::GetValue(int& value,const string type,const string tb)
{
	zlog_info(Util::m_zlog, "从配置数据库中，从表%s中读取系统参数配置，类型=%s",tb.c_str(),type.c_str());

    if (!this->IsOpen())
    {
    	zlog_warn(Util::m_zlog, "配置数据库没有打开");
        return false;
    }

    bool ret = false;

    string sql;
    char tmp[ARRAY_LENGTH_512];
    sprintf(tmp,"select [value] from [%s] where type='%s' order by [sn]",tb.c_str(),type.c_str());
    sql=tmp;

    //预处理sql语句\即将sql语句进行解析成字节
	zlog_info(Util::m_zlog, "预处理sql语句=%s",sql.c_str());
	ret = Prepare(sql);
	if (!ret)
	{
		return false;
	}

	if(DB_ROW==this->Step())//读取数据行数不为0，数据库中存在
	{
		//读取属性
		value = GetColumnInt(0);
		zlog_info(Util::m_zlog, "读取数据库结束，数据值=%d",value);
	}
	else
	{
		Finalize();
		zlog_warn(Util::m_zlog, "数据库中没有该数据");
		return false;
	}

    //关闭stmt
    zlog_info(Util::m_zlog, "读取数据库结束，关闭stmt");
    Finalize();

    zlog_info(Util::m_zlog, "读取数据库结束，数据值=%d",value);
    return true;
}

//历史数据库操作
HisDb* HisDb::m_hisDb = NULL;
HisDb::HisDb()
{
	string db;
	char tmp[ARRAY_LENGTH_1024];
	char curDir[ARRAY_LENGTH_1024] =
	{ 0 };
	//读取当前目录
	Util::GetCurDir(curDir, ARRAY_LENGTH_1024);
	sprintf(tmp,"%sdb/his.db", curDir);
	db=tmp;
	this->SetDb(db);//数据库
	this->SetIsKey(false);//设置为不加密模式
}

HisDb::~HisDb()
{
}

HisDb* HisDb::GetInstance()
{
    if (NULL == HisDb::m_hisDb)
    {
    	HisDb::m_hisDb = new HisDb();
    }

    return HisDb::m_hisDb;
}

void HisDb::FreeInstanse()
{
    if (NULL != HisDb::m_hisDb)
    {
        delete HisDb::m_hisDb;
        HisDb::m_hisDb = NULL;
    }

    return;
}

