/***************************************************************************/
// Simple class to compute an average using samples
/***************************************************************************/
#ifndef AVGNUM_H
#define AVGNUM_H

#include "platform/BaseType.h"

class AverageNumber
{
public:
	inline AverageNumber(void)
		{ Clear(); }
	inline ~AverageNumber(void) {}

	// add in a sample
	inline void AddSample(float sample)
		{ average += (sample - average) / (++samples); }

	// What is the current average?
	inline float Average(void)
		{ return(average); }


	inline int Samples(void)
		{ return((int)samples); }

	// Reset
	inline void Clear(void)
		{ average = samples = 0.0f; }

private:
	float	average;
	float	samples;
};

class AverageNumberReset : public AverageNumber
{
public:
	inline AverageNumberReset(int resetEvery)
		{ Clear(); reset = resetEvery; }
	inline ~AverageNumberReset(void) {}

	// add in a sample
	inline void AddSample(float sample)
		{
		if (Samples() >= reset)
			Clear();
		AverageNumber::AddSample(sample);
		}

private:
	int	reset;
};

#endif
