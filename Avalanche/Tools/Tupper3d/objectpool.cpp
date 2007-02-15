////////////////////////////////////////////////////////////////////////////
//
// ObjectPool
//
// Class to handle the objects
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: objectpool.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 2:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Animation Wrapper Classes
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 5/24/02    Time: 3:15p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added a delete function
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 5  *****************
 * User: Shon         Date: 12/18/01   Time: 3:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/06/01   Time: 4:38p
 * Updated in $/Avalanche/tools/Tupper3d
 * GetNumObjects
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

#include "Tupper3dPch.h"
#include "objectpool.h"

TupObjectPool::TupObjectPool(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	m_pObjectPool = new TupperwarePoolValueString(pObject,"Object");
}

TupObjectPool::~TupObjectPool()
{
	delete m_pObjectPool;
}

int TupObjectPool::AddObjectAggregate( void )
{
	TupperwareAggregate *pAgg = m_pObjectPool->AddAggregate();
	assert(pAgg);
	return atoi(pAgg->GetName());
}

TupperwareAggregate *TupObjectPool::AddObjectAggregate(int index)
{
	return m_pObjectPool->AddAggregate(index);
}

TupperwareAggregate *TupObjectPool::GetObjectAggregate(int index)
{
	return m_pObjectPool->FindAggregate(index);
}

int TupObjectPool::GetNumObjects(void)
{
	return m_pObjectPool->GetPoolSize();
}

void TupObjectPool::DeleteObjectAggregate(int index)
{
	m_pObjectPool->DeleteEntry(index);
}




