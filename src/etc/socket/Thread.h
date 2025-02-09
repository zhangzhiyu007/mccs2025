/*
 * Thread.h
 *
 *  Created on: 2013-3-26
 *      Author: mzh
 */

#ifndef THREAD_H_
#define THREAD_H_
#include <map>
#include <pthread.h>
#include "Buffer.h"

/*名称: Thread
 *描述:通用线程类
 *用法:
 * 1\首先要继承线程函数,并实现Run函数,
 * 该函数为线程运行函数
 * class MyThread :public Thread
 * {
 * virtual void Run();
 * }
 * 2\设置是否自动删除,默认手动
 * MyThread thread;
 * thread.SetAutoDel(false);
 * 3\启动线程
 * thread.Start();
 * 停止线程
 * thread.Stop();
 *创建人:mzh
 *创建日期:2013-3-26
 */
class Thread
{
public:
	Thread();
	virtual ~Thread();

	void Cancel();//关闭线程,线程本身停止线程时使用
	void Kill();//关闭线程,不推荐使用

	virtual void Start();//启动线程
	virtual void Stop();//停止线程,外部线程停止时使用
	virtual void Run() = 0;//线程运行函数，继承该函数

	void Join();//等待线程停止
	static void JoinAll();//等待所有线程停止

	void Detach();//线程运行结束后会自动释放所有资源

	void SetAutoDel(bool autoDel);//设置自动删除Thread指针,否则手动删除对象,默认手动删除对象
	bool IsAlive() const;//线程是否运行
	pthread_t GetID() const;//获得线程句柄

	static Thread* CurrentThread();//当前线程运行

	static int ActiveCount();//线程数量

	enum State
	{
		STOPPED = 1, //线程没有运行
		RUNNING,    //线程正在运行
		STOPPING    //线程即将停止
	};

	//线程状态
	State GetState()
	{
		return m_state;
	}

protected:
	State m_state;

private:
	typedef std::map<pthread_t, Thread*> ThreadMap;
	typedef ThreadMap::iterator ThreadMapIterator;
	typedef ThreadMap::value_type ThreadMapType;

	static ThreadMap m_threads;//线程数组
	static Semaphore m_threadsLock;//信号量

	pthread_t m_handle;//线程句柄
	bool m_autoDel;//自动删除Thread指针

	static void Add(Thread* thread);//添加新线程
	static void Remove(Thread* thread);//删除结束的线程

	static void* Init(void* arg);//初始化
};

#endif /* THREAD_H_ */
