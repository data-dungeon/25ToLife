///////////////////////////////////////////////////////////////////////////////
// Simple function generator base class
///////////////////////////////////////////////////////////////////////////////
#ifndef FUNCTIONGEN_H
#define FUNCTIONGEN_H

#include "platform/BaseType.h"

class FunctionGenerator
{
protected:
	// Return the real input time for this function
	inline float Time(float t)
		{ return t - resetTime; }
	inline float Time(uint t)
		{ return Time((float)t * 0.001f); }

public:
	// Most simple of all base classes
	FunctionGenerator()
		{ amplitude = 1.0f; resetTime = 0.0f; }
	FunctionGenerator(float useAmplitude)
		{ amplitude = useAmplitude; resetTime = 0.0f; }
	virtual ~FunctionGenerator() {}

	// Reset the time to "0"
	virtual void Reset(float t)
		{ resetTime = t; }
	void Reset(uint t)
		{ Reset((float)t * 0.001f); }

	// All functions have amplitude
	void SetAmplitude(float newAmplitude)
		{ amplitude = newAmplitude; }
	float Amplitude()
		{ return	amplitude; }

	// Evaluate the function at a time
 	inline float operator()(float t)
		{ return amplitude * Evaluate(Time(t)); }

	// Support MS transparently
 	inline float operator()(uint t)
		{ return amplitude * Evaluate(Time(t)); }

protected:
	// Here is what everyone writes to evaluate the function
	virtual float Evaluate(float t) = 0;

private:
	// Overall scaling amplitude
	float			amplitude;

	// Time we were last reset
	float			resetTime;
};

#endif
