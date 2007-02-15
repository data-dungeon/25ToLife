////////////////////////////////////////////////////////////////////////////
//
// MaterialPool
//
// Class to deal with the material pools
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: materialpool.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 9/05/02    Time: 4:25p
 * Updated in $/Avalanche/tools/Tupper3d
 * added delete methods
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/10/01   Time: 4:20p
 * Updated in $/Avalanche/tools/Tupper3d
 * Materials - Textures Part 1
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_MATERIALPOOL_H
#define TUPPER3D_MATERIALPOOL_H

class TupperwarePoolValueString;

class TupMaterialPool
{
	// Private Default constructor so no one can use it
	TupMaterialPool(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the object atoms
	TupperwarePoolValueString *m_pMaterialPool; // the pool that knows about all the child atoms

public:
	TupMaterialPool(TupperwareAggregate *pObject); // use this one
	~TupMaterialPool();

// add methods
	int AddMaterial(void); // create a new object in the pool and return the index to this object
	TupperwareAggregate *AddMaterial(int index); // create a new object in the pool using the index

// get methods
	TupperwareAggregate *GetMaterial(int index); // return the aggregate using a integer index
	int GetNumMaterials(void);

// delete methods
	void DeleteMaterial(int index); // remove and delete the aggregate from the pool

};


#endif
