#include "MovingAverage.h"
MovingAverage::MovingAverage(unsigned short filterLength)
{
	FilterLength = filterLength;
	init();
}

MovingAverage::MovingAverage()
{
	FilterLength = default_filter_length;
	init();
}

MovingAverage::~MovingAverage()
{
	delete[] Array;
}

void MovingAverage::init()
{
	FilterComplete = false;
	Index = -1;
	Sum = 0;
	Average = 0;
	Array = new float[FilterLength];
	clear();
}

void MovingAverage::clear()
{
	for (unsigned short i = 0; i < FilterLength; i++)
	{
		Array[i] = 0;
	}
}

void MovingAverage::Add(float x)
{
	Index = (Index + 1) % FilterLength;
	Sum -= Array[Index];
	Array[Index] = x;
	Sum += x;
	if (!FilterComplete && Index == (FilterLength - 1))
	{
		FilterComplete = true;
	}
	if (FilterComplete)
	{
		Average = Sum / FilterLength;
	}
	else
	{
		Average = Sum / (Index + 1);
	}
}

float MovingAverage::GetCurrentAverage()
{
	return Average;
}

float* MovingAverage::GetArray()
{
	return Array;
}

float MovingAverage::GetRaw()
{
	if (Index == -1)
	{
		return 0.0f;
	}
	return Array[Index];
}

unsigned short MovingAverage::GetFilterLength()
{
	return FilterLength;
}
