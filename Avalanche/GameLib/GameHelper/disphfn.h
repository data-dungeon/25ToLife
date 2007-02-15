/////////////////////////////////////////////////////////////////////////////
// Display height field node
/////////////////////////////////////////////////////////////////////////////
#ifndef DISPHFN_H
#define DISPHFN_H

#include "Math/Vector.h"
#include "GameHelper/disphf.h"

#define DHFNODE_MAXASSOC	2

class DHFNode
{
public:
	// Contstructor
	inline DHFNode(void);

	// Set out point in space
	inline void SetPoint(Vector3CRef p);

	// Get the point
	inline Vector3 *GetPoint(void);

	// Set our normal
	inline void SetNormal(Vector3CRef n);

	// Get the normal
	inline Vector3 *GetNormal(void);

	// Set a color
	inline void SetColor(const ts_bRGBA &c);

	// Set a set of UV's
	inline void SetUV(int uvIndex, const UVCOORD &setUV);

	// Get a set of UV's
	inline UVCOORD *GetUV(int uvIndex);

	// Associate a point with this node
	inline void AssociatePoint(Vector3 *p);

	// Associate a normal with this node
	inline void AssociateNormal(Vector3 *n);

	// Associate a color set wiht this node
	inline void AssociateColor(ts_bRGBA *c);

	// Associate a UV set with this node
	inline void AssociateUV(int uvIndex, UVCOORD *uvPair);

protected:
	// We can refer to several points
	Vector3	*point[DHFNODE_MAXASSOC];

	// We can refer to several normals
	Vector3	*normal[DHFNODE_MAXASSOC];

	// We can refer to multiple sets of colors
	ts_bRGBA	*color[DHFNODE_MAXASSOC];

	// We can refer to multiple sets of UV's
	UVCOORD	*uv[DHFNODE_MAXASSOC][DISPHF_MAXTEXTURES];
};

#include "GameHelper/disphfn.hpp"

#endif
