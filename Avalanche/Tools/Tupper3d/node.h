////////////////////////////////////////////////////////////////////////////
//
// Node
//
// Class deal with nodes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: node.h $
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 9/05/03    Time: 4:35p
 * Updated in $/Avalanche/tools/Tupper3d
 * exporting layers
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 3/22/02    Time: 12:54p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added class references for import export
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 2/28/02    Time: 11:22a
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 2/28/02    Time: 11:21a
 * Updated in $/Avalanche/tools/Tupper3d
 * Node Reference Stuff.  Moved code to use private functions (less space)
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 2/26/02    Time: 11:43a
 * Updated in $/Avalanche/tools/Tupper3d
 * Added the Node Pool and Node Referencing
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 12/28/01   Time: 4:35p
 * Updated in $/Avalanche/tools/Tupper3d
 * user properties and flags
 * 
 * *****************  Version 6  *****************
 * User: Shon         Date: 12/18/01   Time: 3:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 12/10/01   Time: 4:20p
 * Updated in $/Avalanche/tools/Tupper3d
 * Materials - Textures Part 1
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/06/01   Time: 4:38p
 * Updated in $/Avalanche/tools/Tupper3d
 * Get Methods
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/06/01   Time: 1:51p
 * Updated in $/Avalanche/tools/Tupper3d
 * Add and Get functions complete
 * Added new files for handling various parts of the graphics format
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/05/01   Time: 4:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * Level A
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_NODE_H
#define TUPPER3D_NODE_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;
class TupperwareList;

class TupNode
{
	// Private Default constructor so no one can use it
	TupNode(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		NAME,
		OBJECT_REFERENCE,
		MATERIAL_REFERENCE,
		TRANSFORM,
		PARENT_NODE_REFERENCE,
		CHILDREN,
		FLAGS,
		USER_PROPERTIES,
		WIREFRAME_COLOR,
		LAYER_REFERENCE,
		NUM_CHILDREN_ATOMS, // last entry
	};
// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:

	enum NodeFlags
	{
		FLAGS_HIDDEN					= 1<<0,
		FLAGS_FROZEN					= 1<<1,
		FLAGS_FROZEN_WITH_MATERIAL		= 1<<2,
		FLAGS_BOX_DISPLAY				= 1<<3,
		FLAGS_VERTICAL_TICKS_DISPLAY	= 1<<4,
		FLAGS_EDGE_DISPLAY				= 1<<5,
		FLAGS_BACK_CULLED_DISPLAY		= 1<<6,
		FLAGS_TRAJECTORY_DISPLAY		= 1<<7,
		FLAGS_XRAY_DISPLAY				= 1<<8,
	};

	TupNode(TupperwareAggregate *pObject);

	void BuildAtomCache(void);

  TupperwareAggregate *GetBaseAggregate( void );

// add methods
	TupperwareScalar *AddName(const char *name);
	TupperwareScalar *AddObjectReference(int objectReference);
	TupperwareScalar *AddMaterialReference(int materialReference);
	TupperwareList *AddWireframeColor(const float *color);
	TupperwareAggregate *AddTransform(void);
	TupperwareScalar *AddParentNodeReference(int parentNodeReference);
	TupperwareScalar *AddChildNodeReference(int childNodeReference);
	TupperwareScalar *AddFlags(int flags);
	TupperwareScalar *AddUserProperties(const char *userPropertyString);
	TupperwareScalar *AddLayerReference(int layerReference);


// get methods
	TupperwareScalar *GetName(char **name);
	TupperwareScalar *GetObjectReference(int &objectReference);
	TupperwareScalar *GetMaterialReference(int &materialReference);
	TupperwareList *GetWireframeColor(float **color);
	TupperwareAggregate *GetTransform(void);
	int GetNumChildNodes(void);
	TupperwareScalar *GetParentNodeReference(int &parentNodeReference);
	int GetNodeReference(void);
	TupperwareScalar *GetChildNodeReference(int index,int &childNodeReference);
	TupperwareScalar *GetFlags(int &flags);
	TupperwareScalar *GetUserProperties(char **userPropertyString);
	TupperwareScalar *GetLayerReference(int &layerReference);

	bool RemoveChildNodeReference(int childNodeRef);

private:
// add methods
	TupperwareScalar *AddIntScalar(int value,TupNode::ChildrenAtoms index);
	TupperwareList *AddFloatList(const float *pfloats,int numFloats,TupNode::ChildrenAtoms index);
	TupperwareList *AddIntList(const int *pInts,int numInts,TupNode::ChildrenAtoms index);

// get methods
	TupperwareScalar *GetIntScalar(int &value,TupNode::ChildrenAtoms index);
	TupperwareList *GetFloatList(float **pFloats,int &numFloats,TupNode::ChildrenAtoms index);
	TupperwareList *GetIntList(int **pInts,int &numInts,TupNode::ChildrenAtoms index);
};


#endif
