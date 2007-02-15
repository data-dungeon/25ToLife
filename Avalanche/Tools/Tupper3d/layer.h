////////////////////////////////////////////////////////////////////////////
//
// layer
//
// Class deal with layers
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: layer.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 9/11/03    Time: 4:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * import and export of layers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 9/05/03    Time: 4:35p
 * Updated in $/Avalanche/tools/Tupper3d
 * exporting layers
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_LAYER_H
#define TUPPER3D_LAYER_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;
class TupperwareList;

class TupLayer
{
	// Private Default constructor so no one can use it
	TupLayer(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		NAME,
		FLAGS,
		COLOR_INDEX,
		DISPLAY_ORDER,
		NUM_CHILDREN_ATOMS, // last entry
	};
// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:

	enum LayerFlags
	{
		FLAGS_ENABLE_OVERRIDES			= 1<<0,
		FLAGS_VISIBLE						= 1<<1,
		FLAGS_PLAYBACK						= 1<<2,
		FLAGS_TEXTURING					= 1<<3,
		FLAGS_SHADING						= 1<<4,
		FLAGS_DISPLAY_TYPE_MASK			= 3<<5,
		FLAGS_DISPLAY_TYPE_NORMAL		= 0<<5,
		FLAGS_DISPLAY_TYPE_TEMPLATE	= 1<<5,
		FLAGS_DISPLAY_TYPE_REFERENCE	= 2<<5,
		FLAGS_LEVEL_OF_DETAIL_MASK		= 1<<7,
		FLAGS_LEVEL_OF_DETAIL_BOX		= 0<<7,
		FLAGS_LEVEL_OF_DETAIL_FULL		= 1<<7,
	};

	TupLayer(TupperwareAggregate *pObject);

	void BuildAtomCache(void);

  TupperwareAggregate *GetBaseAggregate( void );

// add methods
	TupperwareScalar *AddName(const char *name);
	TupperwareScalar *AddFlags(int flags);
	TupperwareScalar *AddColorIndex(int index);
	TupperwareScalar *AddDisplayOrder(int displayOrder);

// get methods
	TupperwareScalar *GetName(char **name);
	TupperwareScalar *GetFlags(int &flags);
	TupperwareScalar *GetColorIndex(int &index);
	TupperwareScalar *GetDisplayOrder(int &displayOrder);

private:
// add methods
	TupperwareScalar *AddIntScalar(int value,TupLayer::ChildrenAtoms index);

// get methods
	TupperwareScalar *GetIntScalar(int &value,TupLayer::ChildrenAtoms index);
};


#endif
