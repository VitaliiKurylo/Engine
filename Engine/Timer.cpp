#include "Timer.h"

Timer::Timer(void)
{
}

Timer::~Timer(void)
{
}

bool Timer::Init(void)
{
	// Check to see if this system supports high performance timers.
	QueryPerformanceFrequency((LARGE_INTEGER*)&_frequency);
	if (_frequency == 0)
	{
		return false;
	}

	// Find out how many times the frequency counter ticks every millisecond.
	_ticksPerMillisecond = (float)(_frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&_startTime);

	return true;
}

void Timer::Render(void)
{
	signed long long currentTime;
	float timeDifference;

	// Query the current time.
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	// Calculate the difference in time since the last time we queried for the current time.
	timeDifference = (float)(currentTime - _startTime);

	// Calculate the frame time by the time difference over the timer speed resolution.
	_frameTime = timeDifference / _ticksPerMillisecond;

	// Restart the timer.
	_startTime = currentTime;

	return;
}
