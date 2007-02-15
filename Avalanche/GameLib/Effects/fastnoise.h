/////////////////////////////////////////////////////////////////////////////
// Fast 1d table noise lookup
/////////////////////////////////////////////////////////////////////////////
#ifndef FASTNOISE_H
#define FASTNOISE_H

class FastNoise
{
public:
	// Seutp the data
	FastNoise(void) { lookup = NULL; }
	FastNoise(float frequency, int lookups = 128);
	~FastNoise(void);

	// Are we initialized?	
	bool Ready(void)
		{ return(lookup != NULL); }

	// Setup the stuff
	void Setup(float frequency, int lookups = 128);

	// Get, t [0, 1]
	inline float Get(float t);

	// Get smoothed, t [0, 1]
	inline float GetSmooth(float t);

private:
	float		*lookup;
	float		tConv;
};

// Include the inlines
#include "Effects/fastnoise.hpp"

#endif
