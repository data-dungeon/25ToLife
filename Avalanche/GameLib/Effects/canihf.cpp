/////////////////////////////////////////////////////////////////////////////
// Curved animated height field, this height field works off a spline
/////////////////////////////////////////////////////////////////////////////

/******************* includes ****************************/

#include "Effects/EffectsPCH.h"





/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CurvedAnimatedHeightField::CurvedAnimatedHeightField(void)
{
	spline[0] = NULL;
	spline[1] = NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CurvedAnimatedHeightField::~CurvedAnimatedHeightField(void)
{
}

/////////////////////////////////////////////////////////////////////////////
// Build and place around a spline
/////////////////////////////////////////////////////////////////////////////
bool CurvedAnimatedHeightField::SetSplines(
SplinePath &pathA,
SplinePath &pathB)
{
	// What is the origin of this sytem?
	Vector3 origin = (pathA.PointFromT(0.5f) + pathB.PointFromT(0.5f)) * 0.5f;

	// What is our direction vector?
	Vector3 dir = (pathA.TangentFromT(0.5f) + pathB.TangentFromT(0.5f)).Normalized();

	// Pick a right vector
	Vector3 right = (pathA.PointFromT(0.5f) - pathB.PointFromT(0.5f)).Normalized();

	// Compute the up
	Vector3 up = Vector3::Cross(dir, right);
	up.Normalize();

	// Re-figure right
	right = Vector3::Cross(dir, up);

	// Build our matrix
	Graphics4x4 m;
	m.Identity();
	m.SetRow0(right);
	m.SetRow1(dir);
	m.SetRow2(up);
	m.PokeTranslation(origin);

	// Place us
	Place(m);

	// Invert it
	Graphics4x4 mi;
	m.Invert(mi);

	// setup our node info
	AnimatedHeightField::NodeInfo nodeInfo;
	nodeInfo.override = AnimatedHeightField::OVERRIDE_POSITION;

	mVtxBuffer.Lock();

	// Now stretch all the points
	float yt = 0.0f;
	float xtinc = 1.0f / (float)(XPoints() - 1);
	float ytinc = 1.0f / (float)(YPoints() - 1);
	for (int y = 0; y < YPoints(); y++, yt += ytinc)
	{
		Vector3 startPoint = pathA.PointFromT(yt);
		Vector3 endPoint = pathB.PointFromT(yt);
		Vector3 diff = endPoint - startPoint;

		float xt = 0.0f;
		for (int x = 0; x < XPoints(); x++, xt += xtinc)
		{
			nodeInfo.position = (startPoint + diff * xt) * mi;
			ModifyNode(x, y, nodeInfo);
		}
	}

	// We have heavily modified the bounds
	RecomputeBounds();

	mVtxBuffer.UnLock();

	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// Test for collisions
/////////////////////////////////////////////////////////////////////////////
bool CurvedAnimatedHeightField::Test(
Segment &seg,
AnimatedHeightField::Collision &collision)
{
	// Is it in the bounding box?
	if (!aabb->SegmentCrosses(seg.start, seg.end))
		return(false);

	// Put the segment into our local space
	Segment local;
	local.start = seg.start * worldToLocal;
	local.end = seg.end * worldToLocal;

	// setup the bounds (this is just a test here, it will test all :( 
	int startx = 0;
	int starty = 0;
	int endx = XPoints() - 2;
	int endy = YPoints() - 2;

	// Init the collision
	collision.d = 10000.0f;
	collision.hf = NULL;

	mVtxBuffer.Lock();

	// Test triangles
	for (int y = starty; y <= endy; y++)
		for (int x = startx; x <= endx; x++)
		{
			// Should we terminate on first hit?
			if (TestQuad(local, x, y, collision))
				collision.hf = this;
		}

	mVtxBuffer.UnLock();

	// Did we get a hit?
	if (!collision.hf)
		return(false);

	// Convert quantities back into world space
	collision.intersection = collision.intersection * instance->Matrix();
	collision.normal = collision.normal.BasisTransform(instance->Matrix());
	return(true);
}


