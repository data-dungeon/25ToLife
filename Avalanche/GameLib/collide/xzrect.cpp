/*************************************************************************/
// A really simple XZRect class
/*************************************************************************/
#include "collide/CollidePCH.h"
#include "collide/xzrect.h"
#include "collide/aabb.h"

/*************************************************************************/
/*************************************************************************/
XZRect::XZRect(
const AABB &from)
{
	// Project this onto the XZ plane
	Set(from.minV.x(), from.minV.z(), from.maxV.x(), from.maxV.z());
}

/*************************************************************************/
// Set from AABB
/*************************************************************************/
void XZRect::Set(
const AABB &from)
{
	// Project this onto the XZ plane
	Set(from.minV.x(), from.minV.z(), from.maxV.x(), from.maxV.z());
}

/*************************************************************************/
// Make the rect square
/*************************************************************************/
void XZRect::MakeSquare(void)
{
	float width = Width();
	float height = Height();

	if (width > height)
	{
		float delta = (width - height) * 0.5f;
		minz -= delta;
		maxz += delta;
	}
	else if (width < height)
	{
		float delta = (height - width) * 0.5f;
		minx -= delta;
		maxx += delta;
	}
}

/*************************************************************************/
// Inflate the XZRect so it contains the point
/*************************************************************************/
void XZRect::InFlateForPoint(
float x,
float z)
{
	if (x < minx)
		minx = x;
	else if (x > maxx)
		maxx = x;
	if (z < minz)
		minz = z;
	else if (z > maxz)
		maxz = z;
}

/*************************************************************************/
/*************************************************************************/
void XZRect::Draw(
DrawUtility::COLOR color,
float y)
{
	const float deflate = 0.1f;
	Vector3 a(minx + deflate, y, minz + deflate);
	Vector3 b(maxx - deflate, y, minz + deflate);
	Vector3 c(maxx - deflate, y, maxz - deflate);
	Vector3 d(minx + deflate, y, maxz - deflate);

	DrawUtility::Line(a, b, color);
	DrawUtility::Line(b, c, color);
	DrawUtility::Line(c, d, color);
	DrawUtility::Line(d, a, color);
}
