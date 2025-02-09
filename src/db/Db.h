/*
 * Db.h
 *
 *   创建日期: 2013-2-22
 *   作         者: mzh
 */

#ifndef DB_H_
#define DB_H_

#include "../util/Util.h"
#include "Device.h"

//数据库操作错误码
#define DB_OK           0   /* 数据库操作成功 */
/* beginning-of-error-codes */
#define DB_ERROR        1   /* SQL语句错误或者数据库文件丢失 */
#define DB_INTERNAL     2   /* 数据库中逻辑错误 */
#define DB_PERM         3   /* 无权限 */
#define DB_ABORT        4   /* 回调终止 */
#define DB_BUSY         5   /* 数据库文件被锁定 */
#define DB_LOCKED       6   /* 表被锁定 */
#define DB_NOMEM        7   /* 分配内存错误 */
#define DB_READONLY     8   /* 只读数据库 */
#define DB_INTERRUPT    9   /* 操作中断*/
#define DB_IOERR       10   /* 错误 */
#define DB_CORRUPT     11   /* 数据库格式错误 */
#define DB_NOTFOUND    12   /* Unknown opcode in DB3_file_control() */
#define DB_FULL        13   /* 数据库已满 */
#define DB_CANTOPEN    14   /* 无法打开数据库 */
#define DB_PROTOCOL    15   /* 数据库协议错误 */
#define DB_EMPTY       16   /* 数据库为空 */
#define DB_SCHEMA      17   /* 数据库模式发生变化*/
#define DB_TOOBIG      18   /* 字符串或者二进制超过限制 */
#define DB_CONSTRAINT  19   /* 违反约束条件 */
#define DB_MISMATCH    20   /* 数据类型不匹配 */
#define DB_MISUSE      21   /* 库使用错误 */
#define DB_NOLFS       22   /* 操作系统不支持 */
#define DB_AUTH        23   /* 无权操作 */
#define DB_FORMAT      24   /* 数据库格式错误 */
#define DB_RANGE       25   /* 参数错误*/
#define DB_NOTADB      26   /* 文件不是数据库文件 */
#define DB_ROW         100  /* STEP操作还有下一行，选择使用 */
#define DB_DONE        101  /* STEP操作完成，一般是插入/更新/删除使用 */

/*名称：关系型数据库操作
 *描述：
 *创建人：mzh
 *创建日期：2013-2-22
 */
class Db
{
public:
	Db();
	virtual ~Db();

	/*
	 * 名称: Open
	 * 功能: 打开数据库
	 * 输入: 无
	 * 输出: 成功/失败
	 */
	bool Open();

	/*
	 * 名称: IsOpen
	 * 功能: 数据库是否打开
	 * 输入:无
	 * 输出: true打开/false没有打开
	 */
	bool IsOpen();

	/*
	 * 名称: Close
	 * 功能: 关闭数据库
	 * 输入:无
	 * 输出:
	 */
	void Close();

	/*
	 * 名称: Key
	 * 功能: 输入密钥
	 * 输入:
	 * key   :[in]string 加密密钥
	 * 输出: 成功/失败
	 */
	bool Key();

	/*
	 * 名称: Rekey
	 * 功能: 变更密钥或给没有加密的数据库添加密钥或清空密钥，
	 * 变更密钥或清空密钥前必须先正确执行Key
	 * 输入:
	 * 输出: 成功/失败
	 */
	bool Rekey();

	/*
	 * 名称: ClearKey
	 * 功能: 清除密钥，不再加密
	 * 输入:无
	 * 输出: 成功/失败
	 */
	bool ClearKey();

	/*
	 * 名称: SetDb
	 * 功能: 设置数据库
	 * 输入:db  :string 数据库名称，
	 * 输出:无
	 */
	void SetDb(string db);

	/*
	 * 名称: SetIsKey
	 * 功能: 设置是否加密
	 * 输出: 无
	 */
	void SetIsKey(bool isKey);
	/*
	 * 名称: SetKey
	 * 功能: 设置加密字符串
	 * 输出: 无
	 */
	void SetKey(string key);

	/*
	 * 名称: SetTimeout
	 * 功能: 设置超时时间
	 * 输入:timeout  :int 超时时间，ms
	 * 输出:无
	 */
	void SetTimeout(int timeout);

	/*
	 * 名称: Begin
	 * 功能: 事务处理
	 * 输入:无
	 * 输出:true成功/false失败
	 */
	bool Begin();

	/*
	 * 名称: Commit
	 * 功能: 事务提交
	 * 输入:无
	 * 输出:true成功/false失败
	 */
	bool Commit();

	/*
	 * 名称: Prepare
	 * 功能: 预处理语句
	 * 输入:sql  :string sql语句，
	 * 输出:true成功/false失败
	 */
	bool Prepare(string sql);

	/*
	 * 名称: Errmsg
	 * 功能: 失败返回错误信息，紧跟数据库操作失败语句执行该语句，查看失败原因
	 * 输入:无
	 * 输出:string 错误消息
	 */
	string Errmsg();

	/*
	 * 名称: Step
	 * 功能: 读取数据时返回是否还有下一行数据，需执行Prepare预处理语句，
	 * 或者执行插入、删除、更新等操作时执行一次操作
	 * 输入:无
	 * 输出:int DB功能码
	 */
	int Step();

	/*
	 * 名称: Finalize
	 * 功能: 关闭游标
	 * 输入:无
	 * 输出:无
	 */
	void Finalize();

	/*
	 * 名称: Reset
	 * 功能: 复位绑定
	 * 输入:无
	 * 输出:无
	 */
	void Reset();

	/*
	 * 名称: GetDataCount
	 * 功能: 返回结果集行数，需执行Prepare预处理语句
	 * 输入:无
	 * 输出:int 结果集行数
	 */
	int GetDataCount();

	/*
	 * 名称: GetDataCount
	 * 功能: 返回结果集列数，需执行Prepare预处理语句
	 * 输入:无
	 * 输出:int 结果集行数
	 */
	int GetColumnCount();

	/*
	 * 名称: GetColumnBlob
	 * 功能: 返回某行第i列保存的二进制数据，指针首地址
	 * 输入:i   :[in]第i列,从0开始
	 * 输出:void*
	 */
	void* GetColumnBlob(int i);
	/*
	 * 名称: GetColumnBlob
	 * 功能: 返回某行第i列保存的二进制数据字节数
	 * 输入:i   :[in]第i列,从0开始
	 * 输出:int 返回字节数
	 */
	int GetColumnBytes(int i);

	/*
	 * 名称: GetColumnBlob
	 * 功能: 返回某行第i列保存的二进制数据WORD类型数量
	 * 输入:i   :[in]第i列,从0开始
	 * 输出:int 返回双字节类型数量
	 */
	int GetColumnBytes16(int i);

	/*
	 * 名称: GetColumnInt
	 * 功能: 返回某行第i列保存int数据
	 * 输入:i   :[in]第i列,从0开始
	 * 输出:int
	 */
	int GetColumnInt(int i);

	/*
	 * 名称: GetColumnInt
	 * 功能: 返回某行第i列保存int数据
	 * 输入:i   :[in]第i列,从0开始
	 * 输出:double
	 */
	double GetColumnDouble(int i);

	/*
	 * 名称: GetColumnText
	 * 功能: 返回某行第i列保存字符串
	 * 输入:i   :[in]第i列,从0开始
	 * 输出:string
	 */
	string GetColumnText(int i);

	/*
	 * 名称: GetColumnChar
	 * 功能: 返回某行第i列保存字符串
	 * 输入:i   :[in]第i列,从0开始
	 * 输出:char*
	 */
	char* GetColumnChar(int i);

	/*
	 * 名称: GetColumnBlob
	 * 功能: 返回某行第i列保存的二进制数据字节数
	 * 输入:i   :[in]第i列,从0开始
	 * 		data      :[in]字符串
	 * 输出:成功/失败
	 */
	bool BindText(int i,string data);

	/*
	 * 名称: GetColumnBlob
	 * 功能: 返回某行第i列保存的二进制数据WORD类型数量
	 * 输入:i   :[in]第i列,从0开始
	 * 		data     :[in]unicode类型数据
	 * 		num      :[in]data数量
	 * 输出:成功/失败
	 */
	bool BindText16(int i,void* data,int num);

	/*
	 * 名称: GetColumnInt
	 * 功能: 返回某行第i列保存int数据
	 * 输入:i   :[in]第i列,从0开始
	 * 		data:[in]数据
	 * 输出:成功/失败
	 */
	bool BindInt(int i,int data);

	/*
	 * 名称: GetColumnInt
	 * 功能: 返回某行第i列保存int数据
	 * 输入:i   :[in]第i列,从0开始
	 * 		data:[in]数据
	 * 输出:成功/失败
	 */
	bool BindDouble(int i,double data);

	/*
	 * 名称: GetColumnText
	 * 功能: 返回某行第i列保存字符串
	 * 输入:i         :[in]第i列,从0开始
	 * 		data     :[in]二进制数据
	 * 		num      :[in]data数量
	 * 输出:成功/失败
	 */
	bool BindBlob(int i,void* data,int num);

protected:
private:

	/*
	 * 名称: OpenDb
	 * 功能: 打开数据库
	 * 输入: 无
	 * 输出: 成功/失败
	 */
	bool OpenDb();

	//变量
public:
protected:
private:

	string m_errmsg;//错误信息
	string m_db;//数据库
	string m_key;//加密密钥
	bool m_dbOpen;//数据库是否打开
	bool m_isKey;//数据库是否加密
	int m_dbTimeout;//设置数据库操作超时时间，ms

};

/*名称：配置数据库操作
 *描述：用于读取、更新、删除、插入配置数据库表
 *创建人：mzh
 *创建日期：2013-2-22
 */
class ConfigDb: public Db
{
	//方法
public:
	ConfigDb();
	virtual ~ConfigDb();

    /*
     *  getInstance
     *  功能:得到实例指针
     *  输入：无
     *  输出：返回Db*指针，
     */
    static Db* GetInstance();

    /*
     *  freeInstanse
     *  功能:关闭类指针，释放内存
     *  输入：无
     *  输出：无
     */
    void FreeInstanse();

	/*
	 * 名称: GetValue
	 * 功能: 读取系统参数配置,返回成功或者失败
	 * value     :[out]string 属性值
	 * type      :[in]string 类型
	 * tb        :[in]int 表名
	 * 输出: bool 成功/失败
	 */
	bool GetValue(int& value,const string type,const string tb);

protected:

private:


	//变量
public:

protected:
private:

	//单例模式
	static Db* m_configDb;//配置数据库指针

};

/*名称：HisDb
 *描述：历史数据库操作
 *创建人：mzh
 *创建日期：2013-2-27
 */
class HisDb: public Db
{
public:
	HisDb();
	virtual ~HisDb();

    /*
     *  GetInstance
     *  功能:得到实例指针
     *  输入：无
     *  输出：返回Db*指针，
     */
    static HisDb* GetInstance();

    /*
     *  FreeInstanse
     *  功能:关闭类指针，释放内存
     *  输入：无
     *  输出：无
     */
    void FreeInstanse();

protected:
private:


	//变量
public:

protected:
private:

	//单例模式
	static HisDb* m_hisDb;//配置数据库指针
};

#endif /* DB_H_ */

//用法ConfigDb和HisDb用法一样
//1、得到配置数据库实例
//ConfigDb* db = (ConfigDb*) ConfigDb::GetInstance();
//2、打开数据库
//	if (!db->IsOpen())
//	{
//		if (db->Open())
//		{
//			zlog_warn(Util::m_zlog, "配置数据库打开失败");
//			return false;
//		}
//	}
//	该部分已经包括输入密钥/数据库名称，如果修改密钥则调用
//	string key="mzh"
//	db->SetKey(key);
//  db->Rekey(key);修改密钥
//3、执行sql语句
//string sql="select * from [sys_controller]";
//db->Prepare(sql);//预编译sql语句
//while(DB_ROW==db->Step())
//{
////读取一行数据，addr是该行的第几个元素
//int data=db->GetColumnInt(addr=0);//addr是某一行的第addr列，从0开始
//double data=db->GetColumnDouble(addr=1);//addr是某一行的第addr列，从0开始
//string data=db->GetColumnText(addr=3);//addr是某一行的第addr列，从0开始
//}
//
//如果是插入或者更新、删除则
//if(DB_DONE==db->Step())
//{
////成功
//}
//4、结束语句,关闭游标
//db->Finalize();
//5、关闭数据库
//db->Close();
//
//另外如果需要进行事务处理
//则第3步如下
//事物开始
//if(!db->Begin())
//{
//	zlog_warn(Util::m_zlog, "事务开始失败");
//	db->Close();
//	return false;
//}
////预处理
//string sql="insert into [data_warn]([time],[time_t],[addr],[fault],[name]) values(datetime( 'now', 'localtime' ),0,%d,%d,'%s'),";
//if(!db->Prepare(sql))
//{
//	zlog_warn(Util::m_zlog, "预处理失败");
//	db->Commit();
//	db->Close();
//	return false;
//}
//
////循环读取报警/故障数据
//while (MemDb::GetWarnData(warnData))
//{
//	name.format("%s",warnData.name);
//	db->BindInt(0,warnData.addr);
//	db->BindInt(1,warnData.fault);
//	db->BindText(2,name);
//	db->Step();
//}
//
//if (!db->Commit())
//{
//	zlog_warn(Util::m_zlog, "事务提交失败");
//	ret=false;
//}
//
//db->Close();
