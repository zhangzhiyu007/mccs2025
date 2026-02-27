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
	if (seconds <= 0)
	{
		return true;
	}

	this->SetEndTime();
	return GetSeconds() >= (unsigned int)seconds;
};

//统计时差，时间到.ms
bool StatTimer::TimeToMilliSeconds(int milliSeconds)
{
	if (milliSeconds <= 0)
	{
		return true;
	}

	this->SetEndTime();
	return GetMilliSeconds() > (unsigned int)milliSeconds;
};
