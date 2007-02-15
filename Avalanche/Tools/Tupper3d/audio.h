////////////////////////////////////////////////////////////////////////////
//
// audio
//
// Handle an audio clip
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: audio.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 6/17/03    Time: 2:57p
 * Updated in $/Avalanche/tools/Tupper3d
 * add audio clips
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_AUDIO_H
#define TUPPER3D_AUDIO_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

class TupAudio
{
	// Private Default constructor so no one can use it
	TupAudio(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

public:

	enum ChildrenAtoms 
	{
		NAME,
		START_TIME,
		FILE_NAME,
		FLAGS,
		NUM_CHILDREN_ATOMS, // last entry
	};

	// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

	TupAudio(TupperwareAggregate *pObject);
	void BuildAtomCache(void);
	TupperwareAggregate *GetBaseAggregate(void);
	
// add methods
	TupperwareScalar *AddName(const char *name);
	TupperwareScalar *AddStartTime(float time);
	TupperwareScalar *AddFileName(const char *filename);
	TupperwareScalar *AddFlags(int flags);
	
// get methods
	TupperwareScalar *GetName(char **name);
	TupperwareScalar *GetStartTime(float &time);
	TupperwareScalar *GetFileName(char **filename);
	TupperwareScalar *GetFlags(int &flags);
	
private:
// add methods
	TupperwareAggregate *AddAggregate(TupAudio::ChildrenAtoms index);
	TupperwareScalar *AddStringScalar(const char *pString,TupAudio::ChildrenAtoms index);
	TupperwareScalar *AddIntScalar(int value,TupAudio::ChildrenAtoms index);
	TupperwareScalar *AddFloatScalar(float value,TupAudio::ChildrenAtoms index);
	
// get methods
	TupperwareAggregate *GetAggregate(TupAudio::ChildrenAtoms index);
	TupperwareScalar *GetStringScalar(char **pString,TupAudio::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupAudio::ChildrenAtoms index);
	TupperwareScalar *GetFloatScalar(float &value,TupAudio::ChildrenAtoms index);
};


#endif
