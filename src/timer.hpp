#pragma once
#ifndef TIMER_HPP
#define TIMER_HPP

// Note: The high definition timer does not work due to library dependencies

// Default value
#ifndef HIGH_DEFINITION_TIMER
#define HIGH_DEFINITION_TIMER 0
#endif

#if HIGH_DEFINITION_TIMER
#include <time.h>
#else
#include <sys/time.h>
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
	
#if HIGH_DEFINITION_TIMER
	typedef struct timespec Time;
#else
	typedef struct timeval Time;
#endif

	bool IsStopped();
	void GetTime(Time & time);
	
	Time m_start;
	Time m_end;
};

#endif // TIMER_HPP