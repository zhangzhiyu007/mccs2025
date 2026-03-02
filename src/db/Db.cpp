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

static pthread_once_t g_dbLockOnce = PTHREAD_ONCE_INIT;
static pthread_mutex_t g_dbLock;

static void InitDbLockOnce()
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&g_dbLock, &attr);
	pthread_mutexattr_destroy(&attr);
}

class ScopedDbLock {
public:
	ScopedDbLock() {
		pthread_once(&g_dbLockOnce, InitDbLockOnce);
		pthread_mutex_lock(&g_dbLock);
	}
	~ScopedDbLock() { pthread_mutex_unlock(&g_dbLock); }
private:
	ScopedDbLock(const ScopedDbLock &);
	ScopedDbLock &operator=(const ScopedDbLock &);
};

Db::Db()
{
	// TODO 构造函数
	m_dbOpen = false;
	m_dbTimeout = 1000;
	m_isKey = false;
	m_dbConn = NULL;
	m_stmt = NULL;
	//默认加密密钥
	m_key = "mazhonghua";
}

Db::~Db()
{
	// TODO 析构函数
	Close();
}

bool Db::Open()
{
	ScopedDbLock dbLock;
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
	ScopedDbLock dbLock;
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
	ret = sqlite3_open(m_db.c_str(), &m_dbConn);
	if (SQLITE_OK != ret)
	{
		isSuccess = false;
		errmsg = (char*) sqlite3_errmsg(m_dbConn);
		zlog_error(Util::m_zlog, "打开数据库失败,错误码:%s.",errmsg);
	}
	else
	{
		ret = sqlite3_busy_timeout(m_dbConn, m_dbTimeout);
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
	ScopedDbLock dbLock;
	zlog_info(Util::m_zlog, "关闭数据库");
	if (IsOpen())
	{
		if (NULL != m_stmt)
		{
			sqlite3_finalize(m_stmt);
			m_stmt = NULL;
		}
		int ret = sqlite3_close(m_dbConn);
		if (SQLITE_OK != ret)
		{
			zlog_warn(Util::m_zlog, "关闭数据库失败,错误=%s", sqlite3_errmsg(m_dbConn));
			return;
		}
		m_dbConn = NULL;//数据库句柄
		m_dbOpen = false;
	}

	return;
}

bool Db::Rekey()
{
	ScopedDbLock dbLock;
	zlog_info(Util::m_zlog, "加密数据库修改密钥");

	bool isSuccess = true;//是否打开成功
	char* errmsg = NULL;//错误信息
	int ret = SQLITE_OK;//返回码

	//修改密钥，清空密钥为 sqlite3_rekey( db, NULL, 0)。
	ret = sqlite3_rekey(m_dbConn, m_key.c_str(), m_key.length());
	if (SQLITE_OK != ret)
	{
		isSuccess = false;
		errmsg = (char*) sqlite3_errmsg(m_dbConn);
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
	ScopedDbLock dbLock;
	zlog_info(Util::m_zlog, "加密数据库修改密钥");

	bool isSuccess = true;//是否打开成功
	char* errmsg = NULL;//错误信息
	int ret = SQLITE_OK;//返回码

	ret = sqlite3_rekey(m_dbConn, NULL, 0);
	if (SQLITE_OK != ret)
	{
		isSuccess = false;
		errmsg = (char*) sqlite3_errmsg(m_dbConn);
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
	ScopedDbLock dbLock;
	zlog_info(Util::m_zlog, "向加密数据库输入密钥");

	bool isSuccess = true;//是否打开成功
	char* errmsg = NULL;//错误信息
	int ret = SQLITE_OK;//返回码

	//输入密钥
	ret = sqlite3_key(m_dbConn, m_key.c_str(), m_key.length());
	if (SQLITE_OK != ret)
	{
		isSuccess = false;
		errmsg = (char*) sqlite3_errmsg(m_dbConn);
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
	ScopedDbLock dbLock;
	zlog_info(Util::m_zlog, "预处理sql语句=%s.",sql.c_str());

    int ret = SQLITE_OK;
	sqlite3_stmt* stmt = NULL;//预处理语句对象
	char* errmsg = NULL;//错误信息

	if (NULL != m_stmt)
	{
		sqlite3_finalize(m_stmt);
		m_stmt = NULL;
	}

	//预处理sql语句\即将sql语句进行解析成字节
	ret = sqlite3_prepare(m_dbConn, sql.c_str(), -1, &stmt,
			(const char**) &errmsg);
	if (SQLITE_OK != ret) {
		zlog_info(Util::m_zlog, "预处理sql语句失败，错误=%s.",sqlite3_errmsg(m_dbConn));
		return false;
	}

	m_stmt = stmt;

	zlog_info(Util::m_zlog, "预处理sql语句=%s成功.",sql.c_str());
	return true;
}

string Db::Errmsg()
{
	ScopedDbLock dbLock;
	string errmsg;
	char errmsgtmp[128];
	sprintf(errmsgtmp, "%s", sqlite3_errmsg(m_dbConn));
	errmsg = errmsgtmp;
	return errmsg;
}

int Db::Step()
{
	ScopedDbLock dbLock;
	if (!HasStmtLocked("Step"))
	{
		return DB_ERROR;
	}
	return sqlite3_step(m_stmt);
}

void Db::Reset()
{
	ScopedDbLock dbLock;
	if (NULL != m_stmt)
	{
		sqlite3_reset(m_stmt);
	}
	return;
}

void Db::Finalize()
{
	ScopedDbLock dbLock;
	if (NULL != m_stmt)
	{
		sqlite3_finalize(m_stmt);
		m_stmt = NULL;
	}
}

bool Db::Begin()
{
	ScopedDbLock dbLock;
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
	ScopedDbLock dbLock;
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

bool Db::HasStmtLocked(const char* caller) const
{
	if (NULL == m_stmt)
	{
		zlog_warn(Util::m_zlog, "%s失败: m_stmt为空", caller);
		return false;
	}
	return true;
}

int Db::GetDataCount()
{
	ScopedDbLock dbLock;
	if (!HasStmtLocked("GetDataCount"))
	{
		return 0;
	}
	return sqlite3_data_count(m_stmt);
}

int Db::GetColumnCount()
{
	ScopedDbLock dbLock;
	if (!HasStmtLocked("GetColumnCount"))
	{
		return 0;
	}
	return sqlite3_column_count(m_stmt);
}

void* Db::GetColumnBlob(int i)
{
	ScopedDbLock dbLock;
	if (!HasStmtLocked("GetColumnBlob"))
	{
		return NULL;
	}
	return (void*)sqlite3_column_blob(m_stmt,i);
}

int Db::GetColumnBytes(int i)
{
	ScopedDbLock dbLock;
	if (!HasStmtLocked("GetColumnBytes"))
	{
		return 0;
	}
	return sqlite3_column_bytes(m_stmt,i);
}

int Db::GetColumnBytes16(int i)
{
	ScopedDbLock dbLock;
	if (!HasStmtLocked("GetColumnBytes16"))
	{
		return 0;
	}
	return sqlite3_column_bytes16(m_stmt,i);
}

int Db::GetColumnInt(int i)
{
	ScopedDbLock dbLock;
	if (!HasStmtLocked("GetColumnInt"))
	{
		return 0;
	}
	return sqlite3_column_int(m_stmt,i);
}

double Db::GetColumnDouble(int i)
{
	ScopedDbLock dbLock;
	if (!HasStmtLocked("GetColumnDouble"))
	{
		return 0;
	}
	return sqlite3_column_double(m_stmt,i);
}

char* Db::GetColumnChar(int i)
{
	ScopedDbLock dbLock;
	if (!HasStmtLocked("GetColumnChar"))
	{
		return NULL;
	}
	return (char*) sqlite3_column_text(m_stmt, i);
}

string Db::GetColumnText(int i)
{
	ScopedDbLock dbLock;
	string str="";
	if (!HasStmtLocked("GetColumnText"))
	{
		return str;
	}
	char* tmp=(char*) sqlite3_column_text(m_stmt, i);
	if(NULL!=tmp)
	{
		str = tmp;
	}
	return str;
}

bool Db::BindText(int i,string data)
{
	ScopedDbLock dbLock;
	if (!HasStmtLocked("BindText"))
	{
		return false;
	}
	int ret = sqlite3_bind_text(m_stmt, i, data.c_str(), data.length(),
			SQLITE_TRANSIENT);
	if (DB_OK == ret)
	{
		return true;
	}

	return false;
}

bool Db::BindText16(int i,void* data,int num)
{
	ScopedDbLock dbLock;
	if (!HasStmtLocked("BindText16"))
	{
		return false;
	}
	int ret = sqlite3_bind_text16(m_stmt, i, data, num, SQLITE_TRANSIENT);
	if (DB_OK == ret)
	{
		return true;
	}

	return false;
}

bool Db::BindInt(int i,int data)
{
	ScopedDbLock dbLock;
	if (!HasStmtLocked("BindInt"))
	{
		return false;
	}
	int ret = sqlite3_bind_int(m_stmt, i, data);
	if (DB_OK == ret)
	{
		return true;
	}

	return false;
}

bool Db::BindDouble(int i,double data)
{
	ScopedDbLock dbLock;
	if (!HasStmtLocked("BindDouble"))
	{
		return false;
	}
	int ret = sqlite3_bind_double(m_stmt, i, data);
	if (DB_OK == ret)
	{
		return true;
	}

	return false;
}

bool Db::BindBlob(int i,void* data,int num)
{
	ScopedDbLock dbLock;
	if (!HasStmtLocked("BindBlob"))
	{
		return false;
	}
	int ret = sqlite3_bind_blob(m_stmt, i, data, num, SQLITE_TRANSIENT);
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

