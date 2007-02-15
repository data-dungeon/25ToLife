//*************************************************************************
// Simple visualization thingie
//*************************************************************************
#ifndef VISTOOL_H
#define VISTOOL_H

#include "collide/obb.h"

#pragma pack(1)
class VisColor
{
public:
	VisColor(void) {}
	VisColor(unsigned char r, unsigned char g, unsigned char b)
		{ this->r = r; this->g = g; this->b = b; }

	inline void Clear(void)
		{ r = g = b = 0; }

	inline void Set(unsigned char r, unsigned char g, unsigned char b)
		{ this->r = r; this->g = g; this->b = b; }

	inline void Gray(unsigned char gray)
		{ this->r = gray; this->g = gray; this->b = gray; }

	inline VisColor &operator=(const VisColor &v)
		{ r = v.r; g = v.g; b = v.b; return(*this); }

	inline VisColor &operator=(unsigned char gray)
		{ r = gray; g = gray; b = gray; return(*this); }

	// Just twidle these directly if you have to
	unsigned char r, g, b;
};
#pragma pack()

// Some nice colors
extern const VisColor VisRED;
extern const VisColor VisGREEN;
extern const VisColor VisBLUE;
extern const VisColor VisCYAN;
extern const VisColor VisMAGENTA;
extern const VisColor VisYELLOW;
extern const VisColor VisWHITE;
extern const VisColor VisBLACK;
extern const VisColor VisGRAY;

class VisTool
{
public:
	// create/destroy
	VisTool(uint wantWidth, uint wantHeight, const VisColor &clearColor);
	~VisTool(void);

	// Set the coordinate system
	void SetCoordinateSystem(Vector3CRef minCoord, Vector3CRef maxCoord);
	void SetCoordinateSystem(const XZRect &rect);

	// Clear the image
	void Clear(const VisColor &clearColor);

	// plot an AABB
	void Plot(const AABB &aabb, const VisColor &c);

	// Plot an OBB
	void Plot(const OBB &obb, const VisColor &c);

	// Plot an XZRect
	void Plot(const XZRect &rect, const VisColor &c);

	// Plot a line
	void Plot(Vector3CRef start, Vector3CRef end, const VisColor &c);
	void Plot(const Segment &s, const VisColor &c);

	// Save to a ppm file
	bool Save(const char *filename);

private:
	VisColor 		*pixMap;
	int				width;
	int				height;
	float				originX, originZ;
	float				scaleX, scaleZ;

	// The real draw
	inline void Draw(int x1, int z1, int x2, int z2, const VisColor &c);

	// Convert a real coordinate into a pixel coordinate
	inline void Convert(Vector3CRef v, int &x, int &z);
	inline void Convert(float rx, float rz, int &x, int &z);

	// Get a specific pixel in the map (returns NULL if off map)
	inline VisColor *Pixel(int x, int z);
};

// include the fast stuff
#include "vistool.hpp"

#endif
