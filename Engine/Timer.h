#pragma once

#include <windows.h>

class Timer
{
public:
	Timer(void);
	virtual ~Timer(void);

	bool Init(void);
	void Render(void);

	float GetTime(void) const { return _frameTime; }

private:
	signed long long _frequency;
	float _ticksPerMillisecond;
	signed long long _startTime;
	float _frameTime;
};

