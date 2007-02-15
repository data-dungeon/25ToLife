#include "mathUtil/dda.h"

//*************************************************************************
// Convert a real coordinate into a pixel coordinate
//*************************************************************************
inline void VisTool::Convert(
Vector3CRef v,
int &x,
int &z)
{
	Convert(v.x(), v.z(), x, z);
}

//*************************************************************************
// Convert a real coordinate into a pixel coordinate
//*************************************************************************
inline void VisTool::Convert(
float rx,
float rz,
int &x,
int &z)
{
	x = (int)((rx - originX) * scaleX);
	z = (int)((rz - originZ) * scaleZ);
}

//*************************************************************************
// Get a specific pixel in the map (returns NULL if off map)
//*************************************************************************
inline VisColor *VisTool::Pixel(
int x,
int z)
{
	if (x < 0 || x >= width || z < 0 || z >= height)
		return(NULL);
	else
		return(&pixMap[x + (height - 1 - z) * width]);
}

//*************************************************************************
// The real draw
//*************************************************************************
inline void VisTool::Draw(
int x1,
int z1,
int x2,
int z2,
const VisColor &c)
{
	DDA dda(x1, z1, x2, z2);

	// Plot the line
	int x, z;
	while (dda.NextPoint(x, z))
	{
		VisColor *pixel = Pixel(x, z);
		if (pixel)
			*pixel = c;
	}
}




