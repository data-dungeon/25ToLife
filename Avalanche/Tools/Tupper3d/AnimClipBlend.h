////////////////////////////////////////////////////////////////////////////
//
// AnimClipBlend
//
// Handle a blend between two clips
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimClipBlend.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/20/03    Time: 2:25p
 * Updated in $/Avalanche/tools/Tupper3d
 * add clip blends
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_ANIMCLIPBLEND_H
#define TUPPER3D_ANIMCLIPBLEND_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

class TupAnimClipBlend
{
	// Private Default constructor so no one can use it
	TupAnimClipBlend(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

public:

	enum ChildrenAtoms 
	{
		NAME,
		CLIP_INSTANCE_1_REFERENCE,
		CLIP_INSTANCE_2_REFERENCE,
		CURVES,
		FLAGS,
		NUM_CHILDREN_ATOMS, // last entry
	};

	enum Flags
	{
		FLAGS_ROTATION_MASK					= 3<<0,
		FLAGS_ROTATION_QUATERNION_SHORT	= 0<<0,
		FLAGS_ROTATION_QUATERNION_LONG	= 1<<0,
		FLAGS_ROTATION_LINEAR				= 2<<0,
	};

	// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

	TupAnimClipBlend(TupperwareAggregate *pObject);
	void BuildAtomCache(void);
	TupperwareAggregate *GetBaseAggregate(void);
	
// add methods
	TupperwareScalar *AddName(const char *name);
	TupperwareScalar *AddClipInstance1Reference(int ref);
	TupperwareScalar *AddClipInstance2Reference(int ref);
	TupperwareAggregate *AddCurves(void);
	TupperwareScalar *AddFlags(int flags);
	
// get methods
	TupperwareScalar *GetName(char **name);
	TupperwareScalar *GetClipInstance1Reference(int &ref);
	TupperwareScalar *GetClipInstance2Reference(int &ref);
	TupperwareAggregate *GetCurves(void);
	TupperwareScalar *GetFlags(int &flags);

private:
// add methods
	TupperwareAggregate *AddAggregate(TupAnimClipBlend::ChildrenAtoms index);
	TupperwareScalar *AddStringScalar(const char *pString,TupAnimClipBlend::ChildrenAtoms index);
	TupperwareScalar *AddIntScalar(int value,TupAnimClipBlend::ChildrenAtoms index);
	
// get methods
	TupperwareAggregate *GetAggregate(TupAnimClipBlend::ChildrenAtoms index);
	TupperwareScalar *GetStringScalar(char **pString,TupAnimClipBlend::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupAnimClipBlend::ChildrenAtoms index);
};


#endif
