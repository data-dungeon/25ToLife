////////////////////////////////////////////////////////////////////////////
//
// SkinModifierContextVertexPool
//
// Class to deal with the vertex pools used within the skin modifier context
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: skinmodifiercontextvertexpool.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/19/02    Time: 12:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * Add Skin Modifier
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_SKIN_MODIFIER_CONTEXT_VERTEX_POOL_H
#define TUPPER3D_SKIN_MODIFIER_CONTEXT_VERTEX_POOL_H

class TupperwarePoolValueString;

class TupSkinModifierContextVertexPool
{
	// Private Default constructor so no one can use it
	TupSkinModifierContextVertexPool(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the object atoms
	TupperwarePoolValueString *m_pVertexPool; // the pool that knows about all the child atoms

public:
	TupSkinModifierContextVertexPool(TupperwareAggregate *pObject); // use this one
	~TupSkinModifierContextVertexPool();

// add methods
	int AddVertex(void); // create a new object in the pool and return the index to this object
	TupperwareAggregate *AddVertex(int index); // create a new object in the pool using the index

// get methods
	TupperwareAggregate *GetVertex(int index); // return the aggregate using a integer index
	int GetNumVertices(void);
};

#endif
