////////////////////////////////////////////////////////////////////////////
//
// AudioPool
//
// Class to deal with the audiopools
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: audiopool.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 6/17/03    Time: 2:57p
 * Updated in $/Avalanche/tools/Tupper3d
 * add audio clips
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_AUDIOPOOL_H
#define TUPPER3D_AUDIOPOOL_H

class TupperwarePoolValueString;

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

class TupAudioPool
{
	// Private Default constructor so no one can use it
	TupAudioPool(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the object atoms
	TupperwarePoolValueString *m_pAudioPool; // the pool that knows about all the child atoms

public:
	TupAudioPool(TupperwareAggregate *pObject); // use this one
	~TupAudioPool();

// add methods
	int AddAudio(void); // create a new object in the pool and return the index to this object
	TupperwareAggregate *AddAudio(int index); // create a new object in the pool using the index

// get methods
	TupperwareAggregate *GetAudio(int index); // return the aggregate using a integer index
	int GetNumAudios(void);

// delete methods
	void DeleteAudio(int index); // remove and delete the aggregate from the pool
};


#endif
