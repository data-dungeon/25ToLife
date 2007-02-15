////////////////////////////////////////////////////////////////////////////
//
// AnimSet
//
// Handle a animation set
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimChannel.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 5/20/03    Time: 10:35a
 * Updated in $/Avalanche/tools/Tupper3d
 * Add character Index
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 4:19p
 * Updated in $/Avalanche/tools/Tupper3d
 * first phase of animation export
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 2:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Animation Wrapper Classes
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_ANIMCHANNEL_H
#define TUPPER3D_ANIMCHANNEL_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

class TupAnimChannel
{
	// Private Default constructor so no one can use it
	TupAnimChannel(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

public:

	enum ChildrenAtoms 
	{
		TYPE,
		NAME,
		ATTRIBUTE,
		REFERENCE,
		CHARACTER_INDEX,
		CURVES,
		FLAGS,
		NUM_CHILDREN_ATOMS, // last entry
	};
	// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

	enum ChannelTypes
	{
		NODE,
		UNKNOWN,
		LAST_ENTRY,
	};
	static char *m_typeNames[LAST_ENTRY]; // names for the children atoms

	TupAnimChannel(TupperwareAggregate *pObject);
	void BuildAtomCache(void);
	TupperwareAggregate *GetBaseAggregate(void);
	
// add methods
	void SetAnimChannelType(TupAnimChannel::ChannelTypes type);
	TupperwareScalar *AddName(const char *name);
	TupperwareScalar *AddAttribute(const char *attribute);
	TupperwareScalar *AddReference(int ref);
	TupperwareScalar *AddCharacterIndex(int index);
	TupperwareAggregate *AddCurves(void);
	TupperwareScalar *AddFlags(int flags);

// get methods
	TupAnimChannel::ChannelTypes GetAnimChannelType(void);
	TupperwareScalar *GetName(char **name);
	TupperwareScalar *GetAttribute(char **attribute);
	TupperwareScalar *GetReference(int &ref);
	TupperwareScalar *GetCharacterIndex(int &index);
	TupperwareAggregate *GetCurves(void);
	TupperwareScalar *GetFlags(int &flags);
	
private:
// add methods
	TupperwareAggregate *AddAggregate(TupAnimChannel::ChildrenAtoms index);
	TupperwareScalar *AddStringScalar(const char *pString,TupAnimChannel::ChildrenAtoms index);
	TupperwareScalar *AddIntScalar(int value,TupAnimChannel::ChildrenAtoms index);
   TupperwareList *AddFloatList(const float *pfloats,int numFloats,TupAnimChannel::ChildrenAtoms index);
	
// get methods
	TupperwareAggregate *GetAggregate(TupAnimChannel::ChildrenAtoms index);
	TupperwareScalar *GetStringScalar(char **pString,TupAnimChannel::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupAnimChannel::ChildrenAtoms index);
   TupperwareList *GetFloatList(float **pFloats,int &numFloats,TupAnimChannel::ChildrenAtoms index);
};


#endif
