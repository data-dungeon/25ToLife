/////////////////////////////////////////////////////////////////////////////
// Simple signal generator class
/////////////////////////////////////////////////////////////////////////////
#ifndef SIGGEN_H
#define SIGGEN_H

#include "Effects/pnoise.h"
#include "Effects/fastnoise.h"

class SigGen
{
public:
	// NO VIRTUAL FUNCTIONS!
	typedef enum
	{
		NONE = 0,
		SIN,
		COS,
		NOISE,
		FASTNOISE,
		FASTSMOOTHNOISE
	} TYPE;
	inline SigGen(void);
	inline SigGen(TYPE type, float freq, float amp);
	inline ~SigGen(void);

	// Setup the signal generator
	inline void Setup(TYPE type, float freq, float amp);

	// user can update amplitude directly
	inline void SetAmplitude(float fAmplitude) { amp = fAmplitude; }

	// Are we ready?
	inline bool Ready(void);

	// Generate values
	inline float Generate(float t, float omega = 0.0f);

	static PerlinNoise& GetNoise() { return noise;}
private:
	TYPE						type;
	float						freq;
	float						amp;
	FastNoise				fastNoise;

	static PerlinNoise	noise;
};

// Implement it!
#include "Effects/siggen.hpp"

#endif
