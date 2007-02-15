///////////////////////////////////////////////////////////////////////////////
// Simple ramp generators
///////////////////////////////////////////////////////////////////////////////
#ifndef RAMP_H
#define RAMP_H

#include "Audio/function/function.h"

// Base class for a ramp
class Ramp : public FunctionGenerator
{
public:
	Ramp()
		{ p = 1.0f; }
	Ramp(float amplitude, float period) : FunctionGenerator(amplitude)
		{ p = period; }
	virtual ~Ramp() {}

	float Period()
		{ return p; }
	void SetPeriod(float newPeriod)
		{ p = newPeriod; }

	bool Done(float t)
		{ return(Time(t) - p > 0.0f); }
	bool Done(uint t)
		{ return(Time(t) - p > 0.0f); }


protected:
	float			a;
	float			p;
};

// Output [1..0] in the specified period
class RampDown : public Ramp
{
public:
	RampDown() {}
	RampDown(float amplitude, float period) : Ramp(amplitude, period) {}

protected:
	// Evaluate
	float Evaluate(float t);
};

// Output [0..1] in the specified period
class RampUp : public Ramp
{
public:
	RampUp() {}
	RampUp(float amplitude, float period) : Ramp(amplitude, period) {}

protected:
	// Evaluate
	float Evaluate(float t);
};

#endif
