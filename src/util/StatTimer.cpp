/*
 * StatTimer.cpp
 *
 *   创建日期: 2013-12-9
 *   作          者: 马中华
 */

#include "StatTimer.h"

StatTimer::StatTimer()
{
	// TODO 构造函数

}

StatTimer::~StatTimer()
{
	// TODO 析构函数
}

//统计时差，时间到,s
bool StatTimer::TimeToSeconds(int seconds)
{
	bool ret=false;
	this->SetEndTime();
	if(GetSeconds()>=seconds)
	{
		ret=true;
	}

	return ret;
};

//统计时差，时间到.ms
bool StatTimer::TimeToMilliSeconds(int milliSeconds)
{
	bool ret = false;
	this->SetEndTime();
	if (GetMilliSeconds() > milliSeconds)
	{
		ret = true;
	}

	return ret;
};
