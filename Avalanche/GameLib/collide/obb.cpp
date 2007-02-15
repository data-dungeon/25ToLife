/***************************************************************************/
// The obb implementations
/***************************************************************************/

/******************* includes ****************************/

/*** pre-compiled headers through .h file (visual studio), can't be inside  ***/
/*** #if statement. see .h file for more info.for codewarrior, pre-compiled ***/
/*** header is handled through xcl compile/link interpreter, and doesn't    ***/
/*** need any explicit include                                              ***/

#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/obb.h"

// Gamelib includes //
#include "obbStatics.h"

#if 0

// Engine includes //
#include "Math/MathClass.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"

// Gamelib includes //
#include "EngineHelper/drawutil.h"

#endif

const uint8 OBB::MASK_XYZ = 1 << (int)OBB::PATH_XYZ;
const uint8 OBB::MASK_YXZ = 1 << (int)OBB::PATH_YXZ;
const uint8 OBB::MASK_YZX = 1 << (int)OBB::PATH_YZX;
const uint8 OBB::MASK_ZYX = 1 << (int)OBB::PATH_ZYX;
const uint8 OBB::MASK_ZXY = 1 << (int)OBB::PATH_ZXY;
const uint8 OBB::MASK_XZY = 1 << (int)OBB::PATH_XZY;

/***************************************************************************/
// Given a matrix that may have scaling and a model AABB that may not be
// "symmetric" return a proper dirCos matrix, center and trueAABB
// that can be used to create an OBB
// Note: This function will be pretty fast, but not super fast.
/***************************************************************************/
void OBB::ComputeBasis(
const Graphics4x4 &m,
const AABB &modelAABB,
DirCos3x3 &dirCos,
Vector3 &center,
AABB &trueAABB)
{
	// create all the vertices of the AABB
	Vector3 p[8];
	p[0] = modelAABB.minV;

	p[1] = modelAABB.minV;
	p[1].z(modelAABB.maxV.z());

	p[2] = modelAABB.minV;
	p[2].y(modelAABB.maxV.y());

	p[3] = modelAABB.maxV;
	p[3].x(modelAABB.minV.x());

	p[4] = modelAABB.minV;
	p[4].x(modelAABB.maxV.x());

	p[5] = modelAABB.maxV;
	p[5].y(modelAABB.minV.y());

	p[6] = modelAABB.maxV;
	p[6].z(modelAABB.minV.z());

	p[7] = modelAABB.maxV;

	// Clear the centroid sum
	center.Clear();
		
	// Transform the model AABB into "world" space
	int i;
	Vector3 pw[8];
	for (i = 0; i < 8; i++)
	{
		pw[i] = p[i] * m;
		center += pw[i];
	}

	// Figure out the center
	center *= 0.125f;

	// Create a proper direction cos matrix
	Vector3 dir;
	dir = m.GetRow2();
	dir.Normalize();

	Vector3 right;
	right = m.GetRow0();

	Vector3 up = Vector3::Cross(dir, right);
	up.Normalize();
	right = Vector3::Cross(up, dir);

	// Setup the matrix
	dirCos.SetRow0(right);
	dirCos.SetRow1(up);
	dirCos.SetRow2(dir);

	// Now figure out the true min/max
	p[0] = (pw[0] - center) / dirCos;
	trueAABB.maxV = p[0];
	for (i = 1; i < 8; i++)
	{
		// Convert back into model space
		p[i] = (pw[i] - center) / dirCos;

		// Setup the bounds
		if (Math::Abs(p[i].x()) > trueAABB.maxV.x())
			trueAABB.maxV.x( Math::Abs(p[i].x()));
		if (Math::Abs(p[i].y()) > trueAABB.maxV.y())
			trueAABB.maxV.y( Math::Abs(p[i].y()));
		if (Math::Abs(p[i].z()) > trueAABB.maxV.z())
			trueAABB.maxV.z( Math::Abs(p[i].z()));
	}
	trueAABB.minV = -trueAABB.maxV;

	// Wee, we're done!!
}

/***************************************************************************/
/***************************************************************************/
OBB::OBB(
const AABB *modelAABB) :
CCollisionObject(OBB_OBJECT)
{
	SetBoundingVolume(modelAABB);
}

/***************************************************************************/
// Setup the obb
/***************************************************************************/
OBBReference::OBBReference(
DirCos3x3 *orientation,
Vector3Packed *position,
AABB *modelAABB) :
OBB(modelAABB)
{
	// These can be null on construction
	parent = orientation;
	pos = position;
}

OBBReference::OBBReference(
DirCos3x3 *orientation,
Vector3Padded *position,
AABB *modelAABB) :
OBB(modelAABB)
{
	// These can be null on construction
	parent = orientation;
	pos = (Vector3Packed*)position;
}
/***************************************************************************/
// Normalize (Note parent & AABB must be set!!!!)
// Make the parent orientation all unit vectors and adjusts aabb to compensate for scaling
// Returns true if the obb had to be normalized
/***************************************************************************/
bool OBB::Normalize(
Vector3 *wantScale)
{
	// Copy these into some nicer vectors
	Vector3 x, y, z;
	x = parent->GetRow0();
	y = parent->GetRow1();
	z = parent->GetRow2();

	// compute the scale
	Vector3 scale;
	scale.x( x.Length());
	x.Normalize();

	// Figure out the XY shear and fix Y
	float shearXY = Vector3::Dot(x, y);
	y -= x * shearXY;
	scale.y( y.Length());
	y.Normalize();
	shearXY /= scale.y();

	// Figure out the XZ & YZ shears
 	float shearXZ = Vector3::Dot(x, z);
	z -= x * shearXZ;
	float shearYZ = Vector3::Dot(y, z);
	z -= y * shearYZ;

 	/* Next, get Z scale and normalize 3rd row. */
 	scale.z( z.Length());
	z.Normalize();
	shearXZ /= scale.z();
	shearYZ /= scale.z();

 	// Check for a coordinate system flip.
	if (Vector3::Dot(x, Vector3::Cross(y, z)) < 0)
	{
		scale *= -1.0f;
		x *= -1.0f;
		y *= -1.0f;
		z *= -1.0f;
	}

	//return the scale factors found, if requested
	if(wantScale)
		*wantScale = scale;

	// Are all of the axes about 1.0 in length?
	if (Math::Zero(scale.x() - 1.0f) && Math::Zero(scale.y() - 1.0f) && Math::Zero(scale.z() - 1.0f))
		return(false);

#if 0
	// Fix this bastard up!
	z = Vector3::Cross(x, y);
	z.Normalize();
	y = Vector3::Cross(z, x);
#endif

	// Slam the normalized vectors back in
	parent->SetRow0(x);
	parent->SetRow1(y);
	parent->SetRow2(z);

	// Adjust the aabb
	aabb.minV = aabb.minV.Diag(scale);
	aabb.maxV = aabb.maxV.Diag(scale);

	return(true);
}

/***************************************************************************/
// Are these OBB's equal?
/***************************************************************************/
#define MIN_DIST   0.01f
#define COS_ALIGN  0.999f   // About 2.5 deg
int OBB::operator==(
const OBB &obb)
{
	// See if the obbs are co-located.
	if ((*pos - *obb.pos).LengthSquared() > (MIN_DIST * MIN_DIST))
		return(false);

	// To be equal, some combination of the axes must be aligned. Alignment
	// can be determined from the matrix that transforms from one to the other.
	DirCos3x3 c = *parent / *obb.parent;

	// Check x-axis/x-axis alignment
	if (Math::Abs(c.e00) > COS_ALIGN)
	{
		if (!Math::Equal(aabb.maxV.x(), obb.aabb.maxV.x(), MIN_DIST))
			return(false);

		// Check y-axis/y-axis, z-axis/z-axis alignment
		if (Math::Abs(c.e11) > COS_ALIGN)
		{
			return(Math::Equal(aabb.maxV.y(), obb.aabb.maxV.y(), MIN_DIST) &&
					Math::Equal(aabb.maxV.z(), obb.aabb.maxV.z(), MIN_DIST));
		}

		// Check y-axis/z-axis, z-axis/y-axis alignment
		return((Math::Abs(c.e12) > COS_ALIGN) &&
				Math::Equal(aabb.maxV.y(), obb.aabb.maxV.z(), MIN_DIST) &&
				Math::Equal(aabb.maxV.z(), obb.aabb.maxV.y(), MIN_DIST));
	}

	// Check x-axis/y-axis alignment
	else if (Math::Abs(c.e01) > COS_ALIGN)
	{
		if (!Math::Equal(aabb.maxV.x(), obb.aabb.maxV.y(), MIN_DIST))
			return(false);

		// Check y-axis/x-axis, z-axis/z-axis alignment
		if (Math::Abs(c.e10) > COS_ALIGN)
		{
			return(Math::Equal(aabb.maxV.y(), obb.aabb.maxV.x(), MIN_DIST) &&
					Math::Equal(aabb.maxV.z(), obb.aabb.maxV.z(), MIN_DIST));
		}

		// Check y-axis/z-axis, z-axis/x-axis alignment
		return((Math::Abs(c.e12) > COS_ALIGN) &&
				Math::Equal(aabb.maxV.y(), obb.aabb.maxV.z(), MIN_DIST) &&
				Math::Equal(aabb.maxV.z(), obb.aabb.maxV.x(), MIN_DIST));
	}

	// Check x-axis/z-axis alignment
	else if (Math::Abs(c.e02) > COS_ALIGN)
	{
		if (!Math::Equal(aabb.maxV.x(), obb.aabb.maxV.x(), MIN_DIST))
			return(false);

		// Check y-axis/x-axis, z-axis/y-axis alignment
		if (Math::Abs(c.e10) > COS_ALIGN)
		{
			return(Math::Equal(aabb.maxV.y(), obb.aabb.maxV.x(), MIN_DIST) &&
					Math::Equal(aabb.maxV.z(), obb.aabb.maxV.y(), MIN_DIST));
		}

		// Check y-axis/y-axis, z-axis/x-axis alignment
		return((Math::Abs(c.e11) > COS_ALIGN) &&
				Math::Equal(aabb.maxV.y(), obb.aabb.maxV.y(), MIN_DIST) &&
				Math::Equal(aabb.maxV.z(), obb.aabb.maxV.x(), MIN_DIST));
	}

	return(false);
}

/***************************************************************************/
// Debugging
/***************************************************************************/
#if !defined(WIN32_TOOLS_DEBUG) && !defined(WIN32_TOOLS_RELEASE) && !defined(CONSUMER_BUILD)
void OBB::QueueDraw(DrawUtility::COLOR color, float lifetime) const
{
	ASSERT(parent && pos);

	Vector3 v[8];
	Transform(*parent, *pos, v);

	for (uint i = 0; i < 12; i++)
	{
		uint v1, v2;
		GetEdge(i, v1, v2);
		DrawUtility::QueueLine(v[v1], v[v2], color, lifetime);
	}
}

void OBB::Draw(
DrawUtility::COLOR color) const
{
	ASSERT(parent && pos);

	Vector3 v[8];
	Transform(*parent, *pos, v);

	for (uint i = 0; i < 12; i++)
	{
		uint v1, v2;
		GetEdge(i, v1, v2);
		DrawUtility::Line(v[v1], v[v2], color);
	}
}

/***************************************************************************/
// Debugging
/***************************************************************************/
void OBB::Draw(
Cartesian4x4 &m,
DrawUtility::COLOR color) const
{
	Vector3 v[8];
	Transform(m, v);

	for (uint i = 0; i < 12; i++)
	{
		uint v1, v2;
		GetEdge(i, v1, v2);
		DrawUtility::Line(v[v1], v[v2], color);
	}
}
#endif

/***************************************************************************/
// If the collision isn't trivally rejected, do the separating
// axis test.  This is reasonably fast if they objects don't
// collide.  If this test passes the boxes are touching.
/***************************************************************************/
bool OBB::TestAABB(
Vector3 &i_center,
Vector3 &i_corner,
float i_radiusSquared)
{
	// Try to trivially reject.
	Vector3 delta = (*pos) - i_center;
	float obbRadiusSquared = radius * radius;
	if (obbRadiusSquared > i_radiusSquared)
	{
		if ((i_radiusSquared + 3.0f * obbRadiusSquared) <
				delta.LengthSquared())
			return(false);
	}
	else if ((3.0f * i_radiusSquared + obbRadiusSquared) <
			delta.LengthSquared())
		return(false);

	// Cast matrix rows as vectors...
	Vector3 bx, by, bz;
	bx = parent->GetRow0();
	by = parent->GetRow1();
	bz = parent->GetRow2();

	Vector3 absBX = Vector3::Abs(bx);
	Vector3 absBY = Vector3::Abs(by);
	Vector3 absBZ = Vector3::Abs(bz);

	// Cache absolute values of matrix elements.
	Matrix3x3 absMat;

	// Alias obb corner.
	Vector3CRef obbCorner = aabb.maxV;

	// World X
	absMat.e00 = absBX.x();
	absMat.e10 = absBY.x();
	absMat.e20 = absBZ.x();
	if (Math::Abs(delta.x()) > (i_corner.x() + obbCorner.x() * absMat.e00 +
			obbCorner.y() * absMat.e10 + obbCorner.z() * absMat.e20))
		 return(false);

	// World Z
	absMat.e02 = absBX.z();
	absMat.e12 = absBY.z();
	absMat.e22 = absBZ.z();
	if (Math::Abs(delta.z()) > (i_corner.z() + obbCorner.x() * absMat.e02 +
			obbCorner.y() * absMat.e12 + obbCorner.z() * absMat.e22))
		 return(false);

	// World Y
	absMat.e01 = absBX.y();
	absMat.e11 = absBY.y();
	absMat.e21 = absBZ.y();
	if (Math::Abs(delta.y()) > (i_corner.y() + obbCorner.x() * absMat.e01 +
			obbCorner.y() * absMat.e11 + obbCorner.z() * absMat.e21))
		 return(false);

	// OBB X
	if (Math::Abs(delta.Dot(bx)) > (i_corner.x() * absMat.e00 +
			i_corner.y() * absMat.e01 + i_corner.z() * absMat.e02 + obbCorner.x()))
		 return(false);

	// OBB Y
	if (Math::Abs(delta.Dot(by)) > (i_corner.x() * absMat.e10 +
			i_corner.y() * absMat.e11 + i_corner.z() * absMat.e12 + obbCorner.y()))
		 return(false);

	// OBB Z
	if (Math::Abs(delta.Dot(bz)) > (i_corner.x() * absMat.e20 +
			i_corner.y() * absMat.e21 + i_corner.z() * absMat.e22 + obbCorner.z()))
		 return(false);

	// (World X) X (OBB X)
	if (Math::Abs(delta.z() * absMat.e01 - delta.y() * absMat.e02) >
			(i_corner.y() * absMat.e02 + i_corner.z() * absMat.e01 +
			obbCorner.y() * absMat.e20 + obbCorner.z() * absMat.e10))
		 return(false);

	// (World X) X (OBB Y)
	if (Math::Abs(delta.z() * absMat.e11 - delta.y() * absMat.e12) >
			(i_corner.y() * absMat.e12 + i_corner.z() * absMat.e11 +
			obbCorner.x() * absMat.e20 + obbCorner.z() * absMat.e00))
		 return(false);

	// (World X) X (OBB Z)
	if (Math::Abs(delta.z() * absMat.e21 - delta.y() * absMat.e22) >
			(i_corner.y() * absMat.e22 + i_corner.z() * absMat.e21 +
			obbCorner.x() * absMat.e10 + obbCorner.y() * absMat.e00))
		 return(false);

	// (World Y) X (OBB X)
	if (Math::Abs(delta.x() * absMat.e02 - delta.z() * absMat.e00) >
			(i_corner.x() * absMat.e02 + i_corner.z() * absMat.e00 +
			obbCorner.y() * absMat.e21 + obbCorner.z() * absMat.e11))
		 return(false);

	// (World Y) X (OBB Y)
	if (Math::Abs(delta.x() * absMat.e12 - delta.z() * absMat.e10) >
			(i_corner.x() * absMat.e12 + i_corner.z() * absMat.e10 +
			obbCorner.x() * absMat.e21 + obbCorner.z() * absMat.e01))
		 return(false);

	// (World Y) X (OBB Z)
	if (Math::Abs(delta.x() * absMat.e22 - delta.z() * absMat.e20) >
			(i_corner.x() * absMat.e22 + i_corner.z() * absMat.e20 +
			obbCorner.x() * absMat.e11 + obbCorner.y() * absMat.e01))
		 return(false);

	// (World Z) X (OBB X)
	if (Math::Abs(delta.y() * absMat.e00 - delta.x() * absMat.e01) >
			(i_corner.x() * absMat.e01 + i_corner.y() * absMat.e00 +
			obbCorner.y() * absMat.e22 + obbCorner.z() * absMat.e12))
		 return(false);

	// (World Z) X (OBB Y)
	if (Math::Abs(delta.y() * absMat.e10 - delta.x() * absMat.e11) >
			(i_corner.x() * absMat.e11 + i_corner.y() * absMat.e10 +
			obbCorner.x() * absMat.e22 + obbCorner.z() * absMat.e02))
		 return(false);

	// (World Z) X (OBB Z)
	if (Math::Abs(delta.y() * absMat.e20 - delta.x() * absMat.e21) >
			(i_corner.x() * absMat.e21 + i_corner.y() * absMat.e20 +
			obbCorner.x() * absMat.e12 + obbCorner.y() * absMat.e02))
		 return(false);

	return(true);
}

void OBB::GetLeadingFaces( int leadingCorner, int &face1, int &face2, int &face3 )
{
	static int LeadingFaces[8][3] = 
	{
		{ 0, 2, 4 }, { 1, 2, 4 }, { 0, 2, 5 }, { 1, 2, 5 },
		{ 0, 3, 4 }, { 1, 3, 4 }, { 0, 3, 5 }, { 1, 3, 5 }
	};
	
	ASSERT( leadingCorner >= 0 && leadingCorner <= 7 );

	face1 = LeadingFaces[leadingCorner][0];
	face2 = LeadingFaces[leadingCorner][1];
	face3 = LeadingFaces[leadingCorner][2];
}

void OBB::GetLeadingEdges( int leadingCorner, int &edge1, int &edge2, int &edge3 )
{
	static int LeadingEdges[8][3] = 
	{
		{ 0, 3, 8 }, { 0, 1, 9 }, { 2, 3, 10 }, { 1, 2, 11 },
		{ 4, 5, 8 }, { 4, 7, 9 }, { 5, 6, 10 }, { 6, 7, 11 }
	};

	ASSERT( leadingCorner >= 0 && leadingCorner <= 7 );

	edge1 = LeadingEdges[leadingCorner][0];
	edge2 = LeadingEdges[leadingCorner][1];
	edge3 = LeadingEdges[leadingCorner][2];
}

void OBB::GetLeadingCorners( int leadingCorner, int &corner1, int &corner2, int &corner3 )
{
	static int LeadingCorners[8][3] = 
	{
		{ 1, 2, 4 }, { 0, 3, 5 }, { 0, 3, 6 }, { 1, 2, 7 },
		{ 0, 5, 6 }, { 1, 4, 7 }, { 2, 4, 7 }, { 3, 5, 6 }
	};

	ASSERT( leadingCorner >= 0 && leadingCorner <= 7 );

	corner1 = LeadingCorners[leadingCorner][0];
	corner2 = LeadingCorners[leadingCorner][1];
	corner3 = LeadingCorners[leadingCorner][2];
}

void OBB::GetAdjacentFaces( int edge, int &face1, int &face2 )
{
	static int AdjacentFaces[12][2] =
	{
		{ 2, 4 }, { 2, 1 }, { 2, 5 }, { 2, 0 },
		{ 3, 4 }, { 3, 1 }, { 3, 5 }, { 3, 0 },
		{ 0, 4 }, { 4, 1 }, { 0, 5 }, { 1, 5 }
	};

	ASSERT( edge >= 0 && edge <= 11 );
	face1 = AdjacentFaces[edge][0];
	face2 = AdjacentFaces[edge][1];
}
