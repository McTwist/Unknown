#pragma once
#ifndef TIMER_HPP
#define TIMER_HPP

// Note: The high definition timer does not work due to library dependencies

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(WIN64) || defined(_WIN64) || defined(__WIN64) && !defined(__CYGWIN__)
#define IS_WINDOWS 1
#endif

// Default value
#ifndef HIGH_DEFINITION_TIMER
#define HIGH_DEFINITION_TIMER 0
#endif

#ifdef IS_WINDOWS
#define TIMER_DISABLED 1
#else
#if HIGH_DEFINITION_TIMER
#include <time.h>
#else
#include <sys/time.h>
#endif
#endif

class Timer
{
public:
	Timer();
	
	void Start();
	void Stop();
	
	float Seconds();
	float MilliSeconds();
	float MicroSeconds();
private:
	
#ifndef TIMER_DISABLED
#if HIGH_DEFINITION_TIMER
	typedef struct timespec Time;
#else
	typedef struct timeval Time;
#endif

	bool IsStopped();
	void GetTime(Time & time);
	
	Time m_start;
	Time m_end;
#endif
};

#endif // TIMER_HPP