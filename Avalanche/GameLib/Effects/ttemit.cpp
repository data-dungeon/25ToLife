/***************************************************************************/
// Tire track emitter stuff
/***************************************************************************/

/******************* includes ****************************/

#include "Effects/EffectsPCH.h"





// our statics
float TireTrackEmitter::segmentLength;	// Length of a segment
float TireTrackEmitter::ignoreSquared;	// length of delta's to ignore
float TireTrackEmitter::heightTweak;	 	// adjustment in height over terrain

/***************************************************************************/
// The constructor is private (TireTrackSystem creates us)
/***************************************************************************/
TireTrackEmitter::TireTrackEmitter(
TireTreadType &treadType) :
tread(treadType)
{
	track = 0;
	length = 0.0f;
	lastPos.Clear();
	halfWidth = TTRACK_DEFAULTWIDTH * 0.5f;
}

/***************************************************************************/
// Set the length of the tire track
/***************************************************************************/
void TireTrackEmitter::Initialize(
float totalLength,
float emitterIgnore,
float heightTweakValue)
{
	ignoreSquared = emitterIgnore * emitterIgnore;

	segmentLength = totalLength / (float)(TTRACK_VERTPAIRS - 1);
	heightTweak = heightTweakValue;
}

// kill the current track we're working on
void TireTrackEmitter::Reset( void )
{
	if( track )
	{
		tread.Delete( track );
		track = NULL;
	}
}

/***************************************************************************/
// Add a data point
/***************************************************************************/
void TireTrackEmitter::AddPoint(
Vector3 &pos,
Vector3 &dir,
bool sliding)
{
	// Are we done sliding?
	if (!sliding)
	{
		// End the current track
		if (track)
		{
			// Re-use this one?
			track->Unlock();
			if (track->VertPairs() == 1)
				tread.Delete(track);
			track = 0;
		}

		return;
	}

	// Should we care?
	Vector3 delta = pos - lastPos;
	float deltaSquared = (float)delta.Dot(delta);
	if (deltaSquared < ignoreSquared)
		return;

	// Do the work on the dir
	dir.y(0.0f);
	if (Math::Zero(dir.LengthSquared()))
		return;
	dir.Normalize();

	// Do we have a tire track right now?
	if (!track)
	{
		// Get a new one
		track = tread.New();
		ASSERT(track);
		track->Lock();

		// Setup the first vertex
		InitSegmentPosDir(pos, dir);
		lastPos = pos;
		lastDir = dir;
		length = 0.0f;
		return;
	}

	// We have a new vertex, add in the length
	float lastLength = length;
	length += Math::Sqrt(deltaSquared);

	// Have we gone over a segment boundary?
	while (length > segmentLength)
	{
		Vector3 ip, id;
		InterpolateLastPoint(lastLength, length, lastPos, lastDir, pos, dir, ip, id);
		AddSegment(ip, id);

		// Update
		length -= segmentLength;
		lastPos = ip;
		lastDir = id;
	}

	// Add in the moving point
	ModifyLastSegment(pos, dir, length);

	// Save these for next time
	lastPos = pos;
	lastDir = dir;
}

/***************************************************************************/
/***************************************************************************/
void TireTrackEmitter::ComputeVertPair(
Vector3 &pos,
Vector3 &dir,
Vector3 &a,
Vector3 &b)
{
	a.Set(
		pos.x() + dir.z() * halfWidth,
	//a.y(GetHeight((float)a.x(), (float)a.z()) + heightTweak);
		pos.y() + heightTweak,
		pos.z() - dir.x() * halfWidth);

	b.Set(pos.x() - dir.z() * halfWidth,
	//b.y(GetHeight((float)b.x(), (float)b.z()) + heightTweak);
			pos.y() + heightTweak,
			pos.z() + dir.x() * halfWidth);
}

