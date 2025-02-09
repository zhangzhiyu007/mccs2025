/*
 * Thread.cpp
 *
 *  Created on: 2013-3-26
 *      Author: mzh
 */

#include "Thread.h"
#include <signal.h>
#include <iostream>


Semaphore Thread::m_threadsLock;
Thread::ThreadMap Thread::m_threads;

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
	m_state = RUNNING;
	//创建线程
	pthread_create(&m_handle, 0, Thread::Init, this);
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
	m_state = STOPPING;
	int* dummy;
	pthread_join(m_handle, (void**) &dummy);
	m_state = STOPPED;
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
	ThreadMapIterator iter;

	while (m_threads.size() > 0)
	{
		iter = m_threads.begin();

		if (iter != m_threads.end())
		{
			iter->second->Join();
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

	ThreadMapIterator iterator = m_threads.find(id);

	if (iterator != m_threads.end())
	{
		return true;
	}

	return false;
}

pthread_t Thread::GetID() const
{
	return m_handle;
}

int Thread::ActiveCount()
{
	return m_threads.size();
}

void* Thread::Init(void* arg)
{
	Thread* thread = reinterpret_cast<Thread*> (arg);
	Add(thread);
	pthread_detach(pthread_self());//设置线程为分离模式
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

	--m_threadsLock;
	m_threads.insert(ThreadMapType(id, thread));
	++m_threadsLock;
}

void Thread::Remove(Thread* thread)
{
	pthread_t id = thread->GetID();
	ThreadMapIterator iter = m_threads.find(id);

	if (iter != m_threads.end())
	{
		--m_threadsLock;
		m_threads.erase(iter);
		++m_threadsLock;
	}
}

