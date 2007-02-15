/***************************************************************************/
// A class to help compute different quantities about chords intersecting
// each other
/***************************************************************************/

/******************* includes ****************************/

#include "geomUtil/GeomUtilPCH.h"

bool ChordIntersect::testCrossGrid[3][3] =
{
	{ false, true, true },
	{ true,  true, true },
	{ true,  true, false }
};

