////////////////////////////////////////////////////////////////////////////
//
// ImagePool
//
// Class to deal with the image pool
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: imagepool.h $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 9/05/02    Time: 4:25p
 * Updated in $/Avalanche/tools/Tupper3d
 * added delete methods
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 9/05/02    Time: 4:23p
 * Updated in $/Avalanche/tools/Tupper3d
 * added remove function
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:34p
 * Updated in $/Avalanche/tools/Tupper3d
 * Material - Map stuff
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_IMAGEPOOL_H
#define TUPPER3D_IMAGEPOOL_H

class TupperwarePoolValueString;

class TupImagePool
{
	// Private Default constructor so no one can use it
	TupImagePool(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the object atoms
	TupperwarePoolValueString *m_pImagePool; // the pool that knows about all the child atoms

public:
	TupImagePool(TupperwareAggregate *pObject); // use this one
	~TupImagePool();

// add methods
	int AddImage(void); // create a new object in the pool and return the index to this object
	TupperwareAggregate *AddImage(int index); // create a new object in the pool using the index

// get methods
	TupperwareAggregate *GetImage(int index); // return the aggregate using a integer index
	int GetNumImages(void);

// delete methods
	void DeleteImage(int index); // remove and delete the aggregate from the pool

};


#endif
