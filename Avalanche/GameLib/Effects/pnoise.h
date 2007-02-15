/////////////////////////////////////////////////////////////////////////////
// coherent noise function over 1, 2 or 3 dimensions
// Adapted from Ken Perlin
/////////////////////////////////////////////////////////////////////////////
#ifndef PNOISE_H
#define PNOISE_H

// We need our base types
#include "Math/Vector.h"

// Default kernel size
#define PERLIN_KERNEL			256
#define PERLIN_KERNEL_MASK  	0xff
#define PERLIN_BASE_SCALE		0x1000

class PerlinNoise
{
public:
	// Initializes the data (watch out!)
	PerlinNoise(void);

	// 1d case
	float Noise(float t);

	// 2d cases
	inline float Noise(Vector2 &v)
		{ return(Noise(v.x(), v.y())); }
	float Noise(float x, float y);

	// 3d cases
	inline float Noise(Vector3 &v)
		{ return(Noise(v.x(), v.y(), v.z())); }
	float Noise(float x, float y, float z);

	// Repeating 1d noise (repeats over period t)
	float LoopNoise(float x, float t);

	// Tileable noise with x repeating over w and y over h
	float TileNoise(float x, float y, float w, float h);

private:
	// Initialization inlines
	static inline float RandomKernel(void);
	static inline Vector2 RandomKernel2(void);
	static inline Vector3 RandomKernel3(void);
	static inline void Swap(int &a, int &b);

	// Other inlines
	inline void SetupInterpolators(float x, int &b0, int &b1, float &r0, float &r1);
	static inline float Dot(const Vector2 &g, float rx, float ry);
	static inline float Dot(Vector3CRef g, float rx, float ry, float rz);
	static inline float SCurve(float t);
	static inline float Lerp(float t, float a, float b);

	// Index offsets
	int		p[PERLIN_KERNEL + PERLIN_KERNEL + 2];

	// Gradient fields for each dimension
	Vector3	g3[PERLIN_KERNEL + PERLIN_KERNEL + 2];
	Vector2	g2[PERLIN_KERNEL + PERLIN_KERNEL + 2];
	float		g1[PERLIN_KERNEL + PERLIN_KERNEL + 2];
};

// Inline implementations
#include "Effects/pnoise.hpp"

// We provide a global noise object, since data requirment is high (14k)
extern PerlinNoise g_perlinNoise;

#endif
