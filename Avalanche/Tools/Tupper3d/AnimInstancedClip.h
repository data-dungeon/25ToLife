////////////////////////////////////////////////////////////////////////////
//
// AnimInstancedClip
//
// Handle an instanced clip
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimInstancedClip.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 6/06/03    Time: 4:06p
 * Updated in $/Avalanche/tools/Tupper3d
 * instanced clip curves
 * camera lookat
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 2:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Animation Wrapper Classes
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_ANIMINSTANCEDCLIP_H
#define TUPPER3D_ANIMINSTANCEDCLIP_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

class TupAnimInstancedClip
{
	// Private Default constructor so no one can use it
	TupAnimInstancedClip(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

public:

	enum ChildrenAtoms 
	{
		NAME,
		SOURCE_CLIP_REFERENCE,
		START_TIME,
		CYCLE,
		WEIGHT,
		SCALE,
		USER_PROPERTIES,
		ANIMATED_CHANNELS,
		FLAGS,
		NUM_CHILDREN_ATOMS, // last entry
	};

	enum Flags
	{
		FLAGS_ENABLE				= 1<<0,
		FLAGS_ABSOLUTE				= 1<<1,

	};
	// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

	TupAnimInstancedClip(TupperwareAggregate *pObject);
	void BuildAtomCache(void);
	TupperwareAggregate *GetBaseAggregate(void);
	
// add methods
	TupperwareScalar *AddName(const char *name);
	TupperwareScalar *AddSourceClipReference(int ref);
	TupperwareScalar *AddStartTime(float time);
	TupperwareScalar *AddCycle(float cycle);
	TupperwareScalar *AddWeight(float weight);
	TupperwareScalar *AddScale(float scale);
	TupperwareScalar *AddUserProperties(const char *userPropertyString);
	TupperwareAggregate *AddAnimagedChannels(void);
	TupperwareScalar *AddFlags(int flags);
	
// get methods
	TupperwareScalar *GetName(char **name);
	TupperwareScalar *GetSourceClipReference(int &ref);
	TupperwareScalar *GetStartTime(float &time);
	TupperwareScalar *GetCycle(float &cycle);
	TupperwareScalar *GetWeight(float &weight);
	TupperwareScalar *GetScale(float &scale);
	TupperwareScalar *GetUserProperties(char **userPropertyString);
	TupperwareAggregate *GetAnimagedChannels(void);
	TupperwareScalar *GetFlags(int &flags);
	
private:
// add methods
	TupperwareScalar *AddStringScalar(const char *pString,TupAnimInstancedClip::ChildrenAtoms index);
	TupperwareScalar *AddIntScalar(int value,TupAnimInstancedClip::ChildrenAtoms index);
	TupperwareScalar *AddFloatScalar(float value,TupAnimInstancedClip::ChildrenAtoms index);
	TupperwareAggregate *AddAggregate(TupAnimInstancedClip::ChildrenAtoms index);
	
// get methods
	TupperwareScalar *GetStringScalar(char **pString,TupAnimInstancedClip::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupAnimInstancedClip::ChildrenAtoms index);
	TupperwareScalar *GetFloatScalar(float &value,TupAnimInstancedClip::ChildrenAtoms index);
	TupperwareAggregate *GetAggregate(TupAnimInstancedClip::ChildrenAtoms index);
};


#endif
