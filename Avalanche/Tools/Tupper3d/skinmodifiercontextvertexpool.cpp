////////////////////////////////////////////////////////////////////////////
//
// SkinModifierContextVertexPool
//
// Class to deal with the vertex pools used within the skin modifier context
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: skinmodifiercontextvertexpool.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/19/02    Time: 12:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * Add Skin Modifier
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "skinmodifiercontextvertexpool.h"

TupSkinModifierContextVertexPool::TupSkinModifierContextVertexPool(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	m_pVertexPool = new TupperwarePoolValueString(pObject,"Vertex");
}

TupSkinModifierContextVertexPool::~TupSkinModifierContextVertexPool()
{
	delete m_pVertexPool;
}

int TupSkinModifierContextVertexPool::AddVertex(void)
{
	TupperwareAggregate *pAgg = m_pVertexPool->AddAggregate();
	assert(pAgg);
	return atoi(pAgg->GetName());
}

TupperwareAggregate *TupSkinModifierContextVertexPool::AddVertex(int index)
{
	return m_pVertexPool->AddAggregate(index);
}

TupperwareAggregate *TupSkinModifierContextVertexPool::GetVertex(int index)
{
	return m_pVertexPool->FindAggregate(index);
}

int TupSkinModifierContextVertexPool::GetNumVertices(void)
{
	return m_pVertexPool->GetPoolSize();
}



