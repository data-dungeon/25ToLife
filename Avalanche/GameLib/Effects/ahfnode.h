/////////////////////////////////////////////////////////////////////////////
// Animated height field node
/////////////////////////////////////////////////////////////////////////////
#ifndef AHFNODE_H
#define AHFNODE_H

#include "Math/Vector.h"
#include "Effects/anihf.h"

#define AHFNODE_MAXASSOC	2

class AHFNode
{
public:
	// Contstructor
	inline AHFNode(void);

	// Are we hidden?
	inline bool Shadowed(void);

	// Are we locked?
	inline bool Locked(void);

	// Set our normal
	inline void SetNormal(Vector3CRef n);

	// Get the normal
	inline Vector3Packed *GetNormal(void);

	// Set the color as a normal
	inline void SetColorNormal(Vector3CRef n);

	// Set a set of UV's
	inline void SetUV(int uvIndex, const UVCOORD_TYPE &setUV);

	// Get a set of UV's
	inline UVCOORD_TYPE *GetUV(int uvIndex);


	// Our node flags
	typedef enum
	{
		LOCKED = 0x1,
		SHADOWED = 0x2
	} NODEFLAG;
	inline void SetFlag(NODEFLAG flag);
	inline void ClearFlag(NODEFLAG flag);

	// Associate a point with this node
	inline void AssociatePoint(Vector3Packed *p);

	// Associate a normal with this node
	inline void AssociateNormal(Vector3Packed *n);

	// Associate a color set wiht this node
	inline void AssociateColor(ts_bRGBA *c);

	// Associate a UV set with this node
	inline void AssociateUV(int uvIndex, UVCOORD_TYPE *uvPair);

private:
	// We can refer to several points
	Vector3Packed	*point[AHFNODE_MAXASSOC];

	// We can refer to several normals
	Vector3Packed	*normal[AHFNODE_MAXASSOC];

	// We can refer to multiple sets of colors
	ts_bRGBA	*color[AHFNODE_MAXASSOC];

	// We can refer to multiple sets of UV's
	UVCOORD_TYPE	*uv[AHFNODE_MAXASSOC][ANIHF_MAXTEXTURES];

	// Our flags
	unsigned int	flags;

public:
	// These are all the animation values
	Vector3			basePos;
	UVCOORD_TYPE	baseuv[ANIHF_MAXTEXTURES];
	Vector3			waveOmega;
	Vector2			uvOmega[ANIHF_MAXTEXTURES];
	Vector3			toEye;

	// These are for running physics
	float				psi;
	float				psiV;
	float				psiA;
	float				waveSpeedSquared;
};

#include "Effects/ahfnode.hpp"

#endif
