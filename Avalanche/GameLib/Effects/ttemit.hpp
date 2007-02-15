/***************************************************************************/
// Tire track emitter inlines
/***************************************************************************/
#include "Layers/Debug.h"

#include "CollideHelper/XZEntity.h"
#include "stage/EntityCategory.h"

/***************************************************************************/
/***************************************************************************/
inline void TireTrackEmitter::InitSegmentPosDir(
Vector3 &pos,
Vector3 &dir)
{
	Vector3 a, b;

	ComputeVertPair(pos, dir, a, b);
	track->AddVertexData(a, b, 0.0f);
}

/***************************************************************************/
/***************************************************************************/
inline void TireTrackEmitter::InitSegment(
Vector3 &a,
Vector3 &b)
{
	track->AddVertexData(a, b, 0.0f);
}

/***************************************************************************/
/***************************************************************************/
inline void TireTrackEmitter::AddSegment(
Vector3 &pos,
Vector3 &dir)
{
	Vector3 a, b;
	ComputeVertPair(pos, dir, a, b);

	// Add the vertex
	ASSERT(track->VertPairs() < TTRACK_VERTPAIRS);
	track->AddVertexData(a, b, segmentLength);

	// Are we at the end of this track?
	if (track->VertPairs() == TTRACK_VERTPAIRS)
	{
		// Unlock the old one
		track->Unlock();
		ASSERT(!track->Locked());

		// Get a new one
		track = tread.New();
		ASSERT(track);
		track->Lock();
		InitSegment(a, b);
	}
}

/***************************************************************************/
/***************************************************************************/
inline void TireTrackEmitter::ModifyLastSegment(
Vector3 &pos,
Vector3 &dir,
float length)
{
	Vector3 a, b;

	ComputeVertPair(pos, dir, a, b);
	int vert = track->VertPairs();
	if (vert == 1)
		track->AddVertexData(a, b, length);
	else
		track->SetVertexData((vert - 1) << 1, a, b, length);
}

/***************************************************************************/
/***************************************************************************/
inline float TireTrackEmitter::GetHeight(
float x,
float z)
{
	CXZEntity entity(x, z);
	entity.SetPositiveInclude(WORLD_CATEGORY);
	g_collisionSystem.ResetStack();
	if (g_collisionSystem.Test(&entity) != NULL)
		return(0.0f);
	else
		return(entity.YVal());
}

/***************************************************************************/
// Interpolate the point at the segment boundary
/***************************************************************************/
inline void TireTrackEmitter::InterpolateLastPoint(
float lastLength,
float length,
Vector3 &lastPos,
Vector3 &lastDir,
Vector3 &pos,
Vector3 &dir,
Vector3 &ip,
Vector3 &id)
{
	// t is the ratio 0-1
	float t = (segmentLength - lastLength) / (length - lastLength);

	// Figure x, z
	ip = lastPos + (pos - lastPos) * t;

	// Figure out the segment
	Vector3 ddir = (dir - lastDir) * t;
	id = (lastDir + ddir).Normalized();
}
