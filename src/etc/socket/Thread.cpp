#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
/*
 * Thread.cpp
 *
 *  Created on: 2013-3-26
 *      Author: mzh
 */

#include "Thread.h"
#include <errno.h>
#include <signal.h>
#include <iostream>
#include <string.h>
#include <time.h>


Semaphore Thread::m_threadsLock;
Thread::ThreadMap Thread::m_threads;
static pthread_mutex_t g_threadsMutex = PTHREAD_MUTEX_INITIALIZER;

class ScopedThreadsMutex {
public:
	ScopedThreadsMutex() { pthread_mutex_lock(&g_threadsMutex); }
	~ScopedThreadsMutex() { pthread_mutex_unlock(&g_threadsMutex); }
private:
	ScopedThreadsMutex(const ScopedThreadsMutex &);
	ScopedThreadsMutex &operator=(const ScopedThreadsMutex &);
};

Thread::Thread() :
	m_state(STOPPED), m_handle(0),m_autoDel(false)
{
	// TODO Auto-generated constructor stub

}

Thread::~Thread()
{
	// TODO Auto-generated destructor stub
}

void Thread::Start()
{
	//创建线程
	int ret = pthread_create(&m_handle, 0, Thread::Init, this);
	if (ret == 0)
	{
		m_state = RUNNING;
	}
	else
	{
		m_state = STOPPED;
		zlog_warn(Util::m_zlog, "线程创建失败: %s", strerror(ret));
	}
}

void Thread::Stop()
{
	Join();
}

void Thread::Cancel()
{
	pthread_cancel(m_handle);
	m_state = STOPPED;
}

void Thread::Join()
{
	if (m_handle == 0)
	{
		m_state = STOPPED;
		return;
	}

	m_state = STOPPING;
	int* dummy;
	int ret = 0;
#ifdef __linux__
	timespec timeout;
	if (clock_gettime(CLOCK_REALTIME, &timeout) == 0)
	{
		timeout.tv_sec += 5;
		ret = pthread_timedjoin_np(m_handle, (void**) &dummy, &timeout);
		if (ret == ETIMEDOUT)
		{
			zlog_error(Util::m_zlog, "线程等待超时(5s): handle=%lu", (unsigned long)m_handle);
			m_state = RUNNING;
			return;
		}
	}
	else
	{
		ret = pthread_join(m_handle, (void**) &dummy);
	}
#else
	ret = pthread_join(m_handle, (void**) &dummy);
#endif

	if (ret != 0)
	{
		zlog_error(Util::m_zlog, "线程等待失败: %s", strerror(ret));
		m_state = RUNNING;
		return;
	}

	m_state = STOPPED;
	m_handle = 0;
}

void Thread::Kill()
{
	if (IsAlive())
	{
		pthread_kill(m_handle, SIGKILL);
		Remove(this);
	}
}

void Thread::Detach()
{
	pthread_detach(m_handle);
}

void Thread::JoinAll()
{
	while (true)
	{
		Thread* thread = NULL;
		{
			ScopedThreadsMutex lock;
			if (m_threads.empty())
			{
				break;
			}
			thread = m_threads.begin()->second;
		}

		if (thread != NULL)
		{
			thread->Join();
		}
	}
}

//设置自动删除Thread指针
void Thread::SetAutoDel(bool autoDel)
{
	this->m_autoDel = autoDel;
}

bool Thread::IsAlive() const
{
	pthread_t id = GetID();
	ScopedThreadsMutex lock;
	ThreadMapIterator iterator = m_threads.find(id);
	return (iterator != m_threads.end());
}

pthread_t Thread::GetID() const
{
	return m_handle;
}


Thread* Thread::CurrentThread()
{
	pthread_t id = pthread_self();
	ScopedThreadsMutex lock;
	ThreadMapIterator iterator = m_threads.find(id);
	if (iterator != m_threads.end())
	{
		return iterator->second;
	}
	return NULL;
}

int Thread::ActiveCount()
{
	ScopedThreadsMutex lock;
	return m_threads.size();
}

void* Thread::Init(void* arg)
{
	Thread* thread = reinterpret_cast<Thread*> (arg);
	Add(thread);
	if (thread->m_autoDel)
	{
		int ret = pthread_detach(pthread_self());
		if (ret != 0)
		{
			zlog_warn(Util::m_zlog, "自动删除线程detach失败: %s", strerror(ret));
		}
	}
	thread->Run();
	Remove(thread);
	if(thread->m_autoDel)
	{
		delete thread;
	}

	return 0;
}

void Thread::Add(Thread* thread)
{
	pthread_t id = thread->GetID();
	ScopedThreadsMutex lock;
	m_threads.insert(ThreadMapType(id, thread));
}

void Thread::Remove(Thread* thread)
{
	pthread_t id = thread->GetID();
	ScopedThreadsMutex lock;
	ThreadMapIterator iter = m_threads.find(id);
	if (iter != m_threads.end())
	{
		m_threads.erase(iter);
	}
}
