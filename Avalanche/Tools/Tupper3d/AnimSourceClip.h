////////////////////////////////////////////////////////////////////////////
//
// AnimSourceClip
//
// Handle an source clip
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimSourceClip.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 5/20/03    Time: 10:35a
 * Updated in $/Avalanche/tools/Tupper3d
 * oops
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 2:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Animation Wrapper Classes
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_ANIMSOURCECLIP_H
#define TUPPER3D_ANIMSOURCECLIP_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

class TupAnimSourceClip
{
	// Private Default constructor so no one can use it
	TupAnimSourceClip(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

public:

	enum ChildrenAtoms 
	{
		NAME,
		CHANNELS,
		START_TIME,
		DURATION,
		USER_PROPERTIES,
		FLAGS,
		NUM_CHILDREN_ATOMS, // last entry
	};

	enum Flags
	{
		FLAGS_POSE					= 1<<0,
	};
	// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

	TupAnimSourceClip(TupperwareAggregate *pObject);
	void BuildAtomCache(void);
	TupperwareAggregate *GetBaseAggregate(void);
	
// add methods
	TupperwareScalar *AddName(const char *name);
	TupperwareAggregate *AddChannels(void);
	TupperwareScalar *AddStartTime(float time);
	TupperwareScalar *AddDuration(float duration);
	TupperwareScalar *AddUserProperties(const char *userPropertyString);
	TupperwareScalar *AddFlags(int flags);
	
// get methods
	TupperwareScalar *GetName(char **name);
	TupperwareAggregate *GetChannels(void);
	TupperwareScalar *GetStartTime(float &time);
	TupperwareScalar *GetDuration(float &duration);
	TupperwareScalar *GetUserProperties(char **userPropertyString);
	TupperwareScalar *GetFlags(int &flags);
	
private:
// add methods
	TupperwareAggregate *AddAggregate(TupAnimSourceClip::ChildrenAtoms index);
	TupperwareScalar *AddStringScalar(const char *pString,TupAnimSourceClip::ChildrenAtoms index);
	TupperwareScalar *AddIntScalar(int value,TupAnimSourceClip::ChildrenAtoms index);
	TupperwareScalar *AddFloatScalar(float value,TupAnimSourceClip::ChildrenAtoms index);
	
// get methods
	TupperwareAggregate *GetAggregate(TupAnimSourceClip::ChildrenAtoms index);
	TupperwareScalar *GetStringScalar(char **pString,TupAnimSourceClip::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupAnimSourceClip::ChildrenAtoms index);
	TupperwareScalar *GetFloatScalar(float &value,TupAnimSourceClip::ChildrenAtoms index);
};


#endif
