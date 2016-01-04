#include "timer.hpp"

#include <string.h>

#ifndef TIMER_DISABLED
Timer::Timer()
{
	memset(&m_start, 0, sizeof(Time));
	memset(&m_end, 0, sizeof(Time));
}

// Start the timer
void Timer::Start()
{
	memset(&m_end, 0, sizeof(Time));
	GetTime(m_start);
}

// Stop the timer
void Timer::Stop()
{
	GetTime(m_end);
}

// Get seconds
float Timer::Seconds()
{
	Time curr;
	if (IsStopped())
		memcpy(&curr, &m_end, sizeof(Time));
	else
		GetTime(curr);
	
	double seconds;
#if HIGH_DEFINITION_TIMER
	long nsec = curr.tv_nsec - m_start.tv_nsec;
	time_t sec = curr.tv_sec - m_start.tv_sec;
	seconds = (sec) + ((double)nsec / 1000000000.0);
#else
	long sec = curr.tv_sec - m_start.tv_sec;
	long usec = curr.tv_usec - m_start.tv_usec;
	seconds = (sec) + ((double)usec / 1000000.0);
#endif
	return seconds;
}

// Get milliseconds
float Timer::MilliSeconds()
{
	Time curr;
	if (IsStopped())
		memcpy(&curr, &m_end, sizeof(Time));
	else
		GetTime(curr);
	
	double ms;
#if HIGH_DEFINITION_TIMER
	long nsec = curr.tv_nsec - m_start.tv_nsec;
	time_t sec = curr.tv_sec - m_start.tv_sec;
	ms = (sec * 1000.0) + ((double)nsec / 1000000.0);
#else
	long sec = curr.tv_sec - m_start.tv_sec;
	long usec = curr.tv_usec - m_start.tv_usec;
	ms = (sec * 1000.0) + ((double)usec / 1000.0);
#endif
	return ms;
}

// Get microseconds
float Timer::MicroSeconds()
{
	Time curr;
	if (IsStopped())
		memcpy(&curr, &m_end, sizeof(Time));
	else
		GetTime(curr);
	
	double us;
#if HIGH_DEFINITION_TIMER
	long nsec = curr.tv_nsec - m_start.tv_nsec;
	time_t sec = curr.tv_sec - m_start.tv_sec;
	us = (sec * 1000000.0) + ((double)nsec / 1000.0);
#else
	long sec = curr.tv_sec - m_start.tv_sec;
	long usec = curr.tv_usec - m_start.tv_usec;
	us = (sec * 1000000.0) + ((double)usec);
#endif
	return us;
}

// Have stopped
bool Timer::IsStopped()
{
	// Not even started
	if (m_start.tv_sec == 0 && m_start.tv_usec == 0)
		return true;
	// Not stopped
	else if (m_end.tv_sec == 0 && m_end.tv_usec == 0)
		return false;
	else
		return true;
}

// Get current time
void Timer::GetTime(Time & time)
{
#if HIGH_DEFINITION_TIMER
	clock_gettime(CLOCK_REALTIME, &time);
#else
	gettimeofday(&time, 0);
#endif
}

#else

// Lazy implementation, we're not running on Windows anyway

Timer::Timer()
{
}

// Start the timer
void Timer::Start()
{
}

// Stop the timer
void Timer::Stop()
{
}

// Get seconds
float Timer::Seconds()
{
	return 0.0f;
}

// Get milliseconds
float Timer::MilliSeconds()
{
	return 0.0f;
}

// Get microseconds
float Timer::MicroSeconds()
{
	return 0.0f;
}

#endif
