/***************************************************************************/
// DBL support stuff for Halo stuff
/***************************************************************************/
#ifndef HALODBL_H
#define HALODBL_H

#include "Math/Vector.h"

// Halo source

#define HALOFLAG_HASDIR		0x01
#define HALOFLAG_ROTATE		0x02
#define HALOFLAG_SSE			0x04

#pragma pack(1)
struct ts_EnvHalo
{
	Vector3Packed	pos;							// 12
	Vector3Packed	dir;							// 12
	ts_bRGBA			color;						// 4
	float				size;							// 4
	int16				flags;						// 2
	int16				textureIndex;				// 2

	char				pad[92];						// 92 + above = 128
};

struct ts_EnvBloom
{
	Vector3Packed 	center; 						// 12
	Vector3Packed 	normal;						// 12
	Vector3Packed 	right;						// 12
	Vector3Packed 	up;							// 12
	float				width, height;				// 8
	int32				type;							// 4
	ts_bRGBA			color;						// 4

	char				pad[64];						// 128
};
#pragma pack()

#endif
