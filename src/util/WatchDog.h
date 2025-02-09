/*
 * WatchDog.h
 *   Copyright (c) 2012-2014,北京中电建投微电网科技有限公司
 *   All rights reserved.
 *   创建日期: 2014-2-23
 *   作          者: 马中华
 *
 *   版          本：1.1
 *   作          者：马中华
 *   修改内容: 封装看门狗功能，将看门狗功能以类的形式封装
 */

#ifndef WATCHDOG_H_
#define WATCHDOG_H_

/*名称:WatchDog
 *描述:看门狗功能
 *用法:
 *1、使能看门狗
 *WatchDog dog;
 *dog.Open();
 *2、在运行过程中，喂狗
 *dog.Feed();
 *3、程序结束时，关闭看门狗功能
 *dog.Close();
 *创建人:马中华
 *创建日期:2014-2-23
 */
class WatchDog
{
public:
	WatchDog();
	virtual ~WatchDog();

	/*
	* 名称: Open
	* 功能: 打开看门狗
	* 输入:
	* timeout    :[in]int   看门狗超时时间，单位s
	* 输出: true成功/false失败
	*/
	bool Open(int timeout);

	/*
	* 名称: Close
	* 功能: 关闭看门狗
	* 输入: 无
	* 输出: true成功/false失败
	*/
	bool Close();

	/*
	* 名称: Feed
	* 功能: 喂狗操作
	* 输入: 无
	* 输出: true成功/false失败
	*/
	bool Feed();
};

#endif /* WATCHDOG_H_ */
