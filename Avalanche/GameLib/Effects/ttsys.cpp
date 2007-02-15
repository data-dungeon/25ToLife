/***************************************************************************/
// Tire track pool
/***************************************************************************/

/******************* includes ****************************/

#include "Effects/EffectsPCH.h"



TireTrackSystem* g_trackSystemPtr = NULL;

/***************************************************************************/
// constructor
/***************************************************************************/
TireTrackSystem::TireTrackSystem(void) :
pool(256),
treads(true)
{
	// clear
	maxTracks = 0;
	tracksLeft = 0;

	time = 0;

	render = true;

	// Dumb default
	SetMaxTracks(1000);
	SetMaxDrawDistance(100.0f);
}

/***************************************************************************/
// Clean up stuff
/***************************************************************************/
TireTrackSystem::~TireTrackSystem(void)
{
}

/***************************************************************************/
// Setup global junk
/***************************************************************************/
void TireTrackSystem::Setup(
float emitterIgnore,
float heightTweak,
float trackLength,
uint textureRepeat)
{
	// setup the tire track
	TireTrack::Setup(trackLength, textureRepeat);

	// Do the emitter stuff
	TireTrackEmitter::Initialize(trackLength, emitterIgnore, heightTweak);
}

/***************************************************************************/
// Set the total number of trax
/***************************************************************************/
void TireTrackSystem::SetMaxTracks(
uint maximumTracks)
{
	int currentInUse = maxTracks - tracksLeft;
	maxTracks = maximumTracks;
	tracksLeft = maxTracks - currentInUse;
}

/***************************************************************************/
// Load in a tire track (returns index)
/***************************************************************************/
TireTrackSystem::TREADTYPE TireTrackSystem::AddTreadType(
TREADTEXTUREID texture,
float baseAlpha)
{
	// Is it already here?
	TREADTYPE find = FindTreadType(texture);
	if (find >= 0)
		return(find);

	// Create the model
	TireTreadType *tread = new TireTreadType(*this, texture, baseAlpha);
	if (!tread)
		return(-1);

	// Is the model good, and the group added to the groups?
	if (!treads.Add(tread))
	{
		delete tread;
		return(-1);
	}

	// Return the index
	return(treads.Count() - 1);
}

/***************************************************************************/
// find a given model name
/***************************************************************************/
TireTrackSystem::TREADTYPE TireTrackSystem::FindTreadType(
TREADTEXTUREID texture)
{
	if (!texture || !(*texture))
		return(-1);

	int count = treads.Count();
	for (int i = 0; i < count; i++)
	{
		TireTreadType *tread = treads[i];
		if (tread->IsEqual(texture))
			return(i);
	}

	// No luck here
	return(-1);
}

/***************************************************************************/
// Create a new emitter
/***************************************************************************/
TireTrackEmitter *TireTrackSystem::CreateEmitter(
TREADTYPE tread)
{
	ASSERT(tread >= 0 && tread < (int)treads.Count());

	return(new TireTrackEmitter(*treads[tread]));
}

/***************************************************************************/
// Clear all tracks!
/***************************************************************************/
void TireTrackSystem::ClearAll(void)
{
	for (int i = 0; i < treads.Count(); i++)
	{
		TireTreadType *tread = treads[i];
		tread->Clear();
	}
}

/***************************************************************************/
// These calls provide the interface for the TireTread to allocate new tracks
/***************************************************************************/
TireTrack *TireTrackSystem::New(
TireTreadType &parent)
{
	// Are there any tracks left?
	if (tracksLeft <= 0)
		ReclaimTracks();

	// Create a track
	TireTrack *track = pool.New();
	if (track)
	{
		++time;
		track->SetBirth(time);
		--tracksLeft;
	}

	return(track);
}

/***************************************************************************/
/***************************************************************************/
void TireTrackSystem::Delete(
TireTrack *killMe)
{
	if (killMe)
	{
		// Is this really safe?
		if(killMe->Locked())
			killMe->Unlock();
		pool.Delete((TireTrackPad16 *)killMe);
		++tracksLeft;
	}
}

/***************************************************************************/
// Reclaim some tracks, the pool is full
/***************************************************************************/
void TireTrackSystem::ReclaimTracks(void)
{
	// wtf?  you can't use an unsigned int as a counter if you want it to go below zero....
	// ...............
	int deleteCount = 4;

	while (--deleteCount >= 0)
	{
		// What is the oldest in each of the lists
		TireTrack *oldest = NULL;
		TireTreadType *owner = NULL;
		for (int i = 0; i < treads.Count(); i++)
		{
			// What is the oldest one here?
			TireTreadType *tread = treads[i];
			TireTrack *lookAt = tread->Oldest();
			if (lookAt)
			{
				if (oldest == NULL || lookAt->Birth() < oldest->Birth())
				{
					oldest = lookAt;
					owner = tread;
				}
			}
		}

		// Do we have one to delete?
		if (!oldest)
			break;

		// Kill it
		if (!oldest->Locked())
			owner->Delete(oldest);
	}
}

