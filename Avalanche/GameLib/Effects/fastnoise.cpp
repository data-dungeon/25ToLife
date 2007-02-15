/////////////////////////////////////////////////////////////////////////////
// Fast 1d table noise lookup
/////////////////////////////////////////////////////////////////////////////

/******************* includes ****************************/

#include "Effects/EffectsPCH.h"





/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
FastNoise::FastNoise(
float frequency,
int lookups)
{
	lookup = NULL;
	Setup(frequency, lookups);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
FastNoise::~FastNoise(void)
{
	if (lookup)
		memFree(lookup);
}

/////////////////////////////////////////////////////////////////////////////
// Seutp the data
/////////////////////////////////////////////////////////////////////////////
void FastNoise::Setup(
float frequency,
int lookups)
{
	ASSERT(lookups > 0);

	// Get rid if i	
	if (lookup)
		memFree(lookup);

	// Allocate the array
	lookup = (float *)memAlloc(lookups * sizeof(float));
	ASSERT(lookup);

	// Create a perlin noise object
	PerlinNoise noise;

	// Compute the conversion valule
	tConv = (float)(lookups - 1);

	// populate the array
	float sample = 0.0f;
	float sampleinc = frequency / tConv;
	float max = -1.0f;
	float min = 1.0f;
	int i;
	for (i = 0; i < lookups; i++)
	{
		lookup[i] = noise.LoopNoise(sample, frequency);
		if (lookup[i] > max)
			max = lookup[i];
		if (lookup[i] < min)
			min = lookup[i];
		sample += sampleinc;
	}
	
	// Normalize it
	float range = max - min;
	float offset = min + range * 0.5f;
	float n = 2.0f / range;
	
	for (i = 0; i < lookups; i++)
		lookup[i] = (lookup[i] - offset) * n;
}

