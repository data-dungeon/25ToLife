/////////////////////////////////////////////////////////////////////////////
// coherent noise function over 1, 2 or 3 dimensions
// Adapted from Ken Perlin
/////////////////////////////////////////////////////////////////////////////

/******************* includes ****************************/

#include "Effects/EffectsPCH.h"





// We provide a global noise object, since data requirment is high (14k)
PerlinNoise g_perlinNoise;

/////////////////////////////////////////////////////////////////////////////
// Construct the tables
/////////////////////////////////////////////////////////////////////////////
PerlinNoise::PerlinNoise(void)
{
	int i;

	for (i = 0; i < PERLIN_KERNEL; i++)
	{
		p[i] = i;

		// 1d gradient field
		g1[i] = RandomKernel();

		// 2d gradient field
		g2[i] = RandomKernel2();

		// 3d gradient field
		g3[i] = RandomKernel3();
	}

	// Mix up the index table
	while (--i)
		Swap(p[i], p[rand() % PERLIN_KERNEL]);

	// Copy stuff around
	for (i = 0 ; i < PERLIN_KERNEL + 2 ; i++)
	{
		p[PERLIN_KERNEL + i] = p[i];
		g1[PERLIN_KERNEL + i] = g1[i];
		g2[PERLIN_KERNEL + i] = g2[i];
		g3[PERLIN_KERNEL + i] = g3[i];
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float PerlinNoise::Noise(
float t)
{
	int bx0, bx1;
	float rx0, rx1;

	SetupInterpolators(t, bx0, bx1, rx0, rx1);
	float sx = SCurve(rx0);
	float u = rx0 * g1[p[bx0]];
	float v = rx1 * g1[p[bx1]];
	return(Lerp(sx, u, v));
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float PerlinNoise::Noise(
float x,
float y)
{
	int bx0, bx1, by0, by1;
	float rx0, rx1, ry0, ry1;

	SetupInterpolators(x, bx0, bx1, rx0, rx1);
	SetupInterpolators(y, by0, by1, ry0, ry1);

	const int &i = p[bx0];
	const int &j = p[bx1];

	const int &b00 = p[i + by0];
	const int &b10 = p[j + by0];
	const int &b01 = p[i + by1];
	const int &b11 = p[j + by1];

	float sx = SCurve(rx0);
	float sy = SCurve(ry0);

	float u = Dot(g2[b00], rx0, ry0);
	float v = Dot(g2[b10], rx1, ry0);
	float a = Lerp(sx, u, v);

	u = Dot(g2[b01], rx0, ry1);
	v = Dot(g2[b11], rx1, ry1);
	float b = Lerp(sx, u, v);

	return(Lerp(sy, a, b));
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float PerlinNoise::Noise(
float x,
float y,
float z)
{
	int bx0, bx1, by0, by1, bz0, bz1;
	float rx0, rx1, ry0, ry1, rz0, rz1;

	SetupInterpolators(x, bx0, bx1, rx0, rx1);
	SetupInterpolators(y, by0, by1, ry0, ry1);
	SetupInterpolators(z, bz0, bz1, rz0, rz1);

	const int &i = p[bx0];
	const int &j = p[bx1];

	const int &b00 = p[i + by0];
	const int &b10 = p[j + by0];
	const int &b01 = p[i + by1];
	const int &b11 = p[j + by1];

	float t = SCurve(rx0);
	float sy = SCurve(ry0);
	float sz = SCurve(rz0);

	float u = Dot(g3[b00 + bz0], rx0, ry0, rz0);
	float v = Dot(g3[b10 + bz0], rx1, ry0, rz0);
	float a = Lerp(t, u, v);

	u = Dot(g3[b01 + bz0], rx0, ry1, rz0);
	v = Dot(g3[b11 + bz0], rx1, ry1, rz0);
	float b = Lerp(t, u, v);

	float c = Lerp(sy, a, b);

	u = Dot(g3[b00 + bz1], rx0, ry0, rz1);
	v = Dot(g3[b10 + bz1], rx1, ry0, rz1);
	a = Lerp(t, u, v);

	u = Dot(g3[b01 + bz1], rx0, ry1, rz1);
	v = Dot(g3[b11 + bz1], rx1, ry1, rz1);
	b = Lerp(t, u, v);

	float d = Lerp(sy, a, b);

	return(Lerp(sz, c, d));
}

/////////////////////////////////////////////////////////////////////////////
// Repeating 1d noise (repeats over period t)
/////////////////////////////////////////////////////////////////////////////
float PerlinNoise::LoopNoise(float x, float t)
{
	return(((t - x) * Noise(x) + x * Noise(x - t)) / t);
}

/////////////////////////////////////////////////////////////////////////////
// Tileable noise with x repeating over w and y over h
/////////////////////////////////////////////////////////////////////////////
float PerlinNoise::TileNoise(
float x,
float y,
float w,
float h)
{
	return(((Noise(x, y) * (w - x) * (h - y)) +
			  (Noise(x - w, y) * x * (h - y)) +
			  (Noise(x - w, y - h) * x * y) +
			  (Noise(x, y - h) * (w - x) * y)) / (w * h));
}
