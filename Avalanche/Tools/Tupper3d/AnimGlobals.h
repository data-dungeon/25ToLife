////////////////////////////////////////////////////////////////////////////
//
// AnimGlobals
//
// Handle animation global settings
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimGlobals.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 6/02/03    Time: 10:34a
 * Updated in $/Avalanche/tools/Tupper3d
 * animation globals
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_ANIMGLOBALS_H
#define TUPPER3D_ANIMGLOBALS_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

#undef GetCurrentTime

class TupAnimGlobals
{
	// Private Default constructor so no one can use it
	TupAnimGlobals(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

public:
	enum ChildrenAtoms 
	{
		CURRENT_TIME,
		START_TIME,
		END_TIME,
		START_PLAYBACK_TIME,
		END_PLAYBACK_TIME,
		PLAYBACK_BY,
		PLAYBACK_SPEED,
		FLAGS,
		NUM_CHILDREN_ATOMS, // last entry
	};

	enum Flags
	{
		FLAGS_PLAYBACK_MODE_MASK				= 3<<0,
		FLAGS_PLAYBACK_MODE_ONCE				= 0<<0,
		FLAGS_PLAYBACK_MODE_LOOP				= 1<<0,
		FLAGS_PLAYBACK_MODE_OSCILLATE			= 2<<0,

		FLAGS_VIEW_MODE_ALL						= 1<<2,
		FLAGS_AUTO_KEY_MODE						= 1<<3,
 
      FLAGS_USE_MAYA_BONE_TRANSLATION_SPACE = 1<<4,

	};

	// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

	TupAnimGlobals(TupperwareAggregate *pObject);
	void BuildAtomCache(void);
	TupperwareAggregate *GetBaseAggregate(void);
	
// add methods
	TupperwareScalar *AddCurrentTime(float time);
	TupperwareScalar *AddStartTime(float time);
	TupperwareScalar *AddEndTime(float time);
	TupperwareScalar *AddStartPlaybackTime(float time);
	TupperwareScalar *AddEndPlaybackTime(float time);
	TupperwareScalar *AddPlaybackBy(float increment);
	TupperwareScalar *AddPlaybackSpeed(float speed);
	TupperwareScalar *AddFlags(int flags);
	
// get methods
	TupperwareScalar *GetCurrentTime(float &time);
	TupperwareScalar *GetStartTime(float &time);
	TupperwareScalar *GetEndTime(float &time);
	TupperwareScalar *GetStartPlaybackTime(float &time);
	TupperwareScalar *GetEndPlaybackTime(float &time);
	TupperwareScalar *GetPlaybackBy(float &increment);
	TupperwareScalar *GetPlaybackSpeed(float &speed);
	TupperwareScalar *GetFlags(int &flags);
	
private:
// add methods
	TupperwareScalar *AddIntScalar(int value,TupAnimGlobals::ChildrenAtoms index);
	TupperwareScalar *AddFloatScalar(float value,TupAnimGlobals::ChildrenAtoms index);
	
// get methods
	TupperwareScalar *GetIntScalar(int &value,TupAnimGlobals::ChildrenAtoms index);
	TupperwareScalar *GetFloatScalar(float &value,TupAnimGlobals::ChildrenAtoms index);
};


#endif
