////////////////////////////////////////////////////////////////////////////
//
// ConnectionPool
//
// Class to handle connection pool
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: connectionpool.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 6/05/03    Time: 12:02p
 * Updated in $/Avalanche/tools/Tupper3d
 * motion path exporting
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "connectionpool.h"

TupConnectionPool::TupConnectionPool(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	m_pConnectionPool = new TupperwarePoolValueString(pObject,"Connection");
}

TupConnectionPool::~TupConnectionPool()
{
	delete m_pConnectionPool;
}

int TupConnectionPool::AddConnection( void )
{
	TupperwareAggregate *pAgg = m_pConnectionPool->AddAggregate();
	assert(pAgg);
	return atoi(pAgg->GetName());
}

TupperwareAggregate *TupConnectionPool::AddConnection(int index)
{
	return m_pConnectionPool->AddAggregate(index);
}

TupperwareAggregate *TupConnectionPool::GetConnection(int index)
{
	return m_pConnectionPool->FindAggregate(index);
}

int TupConnectionPool::GetNumConnections(void)
{
	return m_pConnectionPool->GetPoolSize();
}

void TupConnectionPool::DeleteConnection(int index)
{
	m_pConnectionPool->DeleteEntry(index);
}




