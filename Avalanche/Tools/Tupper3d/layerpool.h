////////////////////////////////////////////////////////////////////////////
//
// LayerPool
//
// Class to deal with the layers
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: layerpool.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 9/05/03    Time: 4:35p
 * Updated in $/Avalanche/tools/Tupper3d
 * exporting layers
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_LAYERPOOL_H
#define TUPPER3D_LAYERPOOL_H

class TupperwarePoolValueString;

class TupLayerPool
{
	// Private Default constructor so no one can use it
	TupLayerPool(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the object atoms
	TupperwarePoolValueString *m_pLayerPool; // the pool that knows about all the child atoms

public:
	TupLayerPool(TupperwareAggregate *pObject); // use this one
	~TupLayerPool();

// add methods
	int AddLayer(void); // create a new object in the pool and return the index to this object
	TupperwareAggregate *AddLayer(int index); // create a new object in the pool using the index

// get methods
	TupperwareAggregate *GetLayer(int index); // return the aggregate using a integer index
	int GetNumLayers(void);

// delete methods
	void DeleteLayer(int index);
};

#endif
