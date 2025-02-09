/*
 * StatTimer.h
 *
 *   创建日期: 2013-12-9
 *   作          者: 马中华
 */

#ifndef STATTIMER_H_
#define STATTIMER_H_

#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#if defined(WIN32)
  #define GET_CLOCK_COUNT(x) QueryPerformanceCounter((LARGE_INTEGER *)x)
#else
  #define GET_CLOCK_COUNT(x) gettimeofday(x, NULL)
#endif

#define MILLISECONDS_CONVERSION 1000//毫秒
#define MICROSECONDS_CONVERSION 1000000//微秒

/*名称：StatTimer
 *描述：计算时间差函数
 *用法：
 *StatTimer st;
 *st.Init();//初始化
 *st.SetStartTime();//得到起始时间
 *st.SetEndTime();//得到结束时间
 *st.GetSeconds();//得到时间差
 *创建人：mzh
 *创建日期：2013-2-13
 */
class StatTimer
{
public:
    StatTimer();

    ~StatTimer();

    //初始化
    void Initialize()
    {
        memset(&m_startTime, 0, sizeof(struct timeval));
        memset(&m_endTime, 0, sizeof(struct timeval));
    };

    //得到起始时间
    struct timeval GetStartTime() { return m_startTime; };
    void SetStartTime() { GET_CLOCK_COUNT(&m_startTime); };

    //得到结束时间
    struct timeval GetEndTime() { return m_endTime; };
    void SetEndTime() { GET_CLOCK_COUNT(&m_endTime); };

    //计算时间差
    unsigned int GetMilliSeconds() { return (CalcTotalUSec() / MILLISECONDS_CONVERSION); };//毫秒
    unsigned int GetMicroSeconds() { return (CalcTotalUSec()); }; //微秒
    unsigned int GetSeconds() { return (CalcTotalUSec() / MICROSECONDS_CONVERSION); };//秒

    //得到当前时间
    unsigned int GetCurrentTime()
    {
        struct timeval tmpTime;
        GET_CLOCK_COUNT(&tmpTime);
        return ((tmpTime.tv_sec * MICROSECONDS_CONVERSION) + tmpTime.tv_usec);
    };

    //统计时差，时间到 s
    bool TimeToSeconds(int seconds);

    //统计时差，时间到.ms
    bool TimeToMilliSeconds(int milliSeconds);

protected:
private:
	unsigned int CalcTotalUSec()
	{
		return (((m_endTime.tv_sec - m_startTime.tv_sec)
				* MICROSECONDS_CONVERSION) + (m_endTime.tv_usec
				- m_startTime.tv_usec));
	};
public:
protected:
private:
    struct timeval  m_startTime;
    struct timeval  m_endTime;
};


#endif /* STATTIMER_H_ */
