/***************************************************************************/
// DBL support stuff for Stationary Cam stuff
/***************************************************************************/
#ifndef STATIONARYCAMDBL_H
#define STATIONARYCAMDBL_H

#include "Math/Vector.h"

struct ts_EnvStationaryCam
{
	Vector3Packed	pos;							// 12
	Vector3Packed	dir;							// 12
	float				fov;							// 4
	float				x;								// 4
	float				y;								// 4
	float				speed;						// 4
	float				time;							// 4
	char				name[12];					// 12

	char				pad[84];						// 84 + above = 128
};

#endif
