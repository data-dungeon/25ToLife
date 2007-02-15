///////////////////////////////////////////////////////////////////////////////
// Simple LFO's (low frequency oscillator) output [-1..1]
///////////////////////////////////////////////////////////////////////////////
#ifndef LFO_H
#define LFO_H

#include "Audio/function/function.h"

class LFO : public FunctionGenerator
{
public:
	// Construct
	LFO();
	LFO(float amplitude, float period);

	// What is the period?
	float Period();

	// What is the frequency (kenneth?)
	float Frequency();

	// Set new values
	void SetPeriod(float newPeriod);
	void SetFrequency(float newFrequency);

protected:
	float			omega;

	// Evaluate
	float Evaluate(float t);
};

#endif
