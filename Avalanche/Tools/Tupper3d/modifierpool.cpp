////////////////////////////////////////////////////////////////////////////
//
// ModifierPool
//
// Class to handle the modifier pool
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: modifierpool.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 9/05/02    Time: 4:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * added delete method
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 4/19/02    Time: 12:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * Add Skin Modifier
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/28/01   Time: 4:33p
 * Updated in $/Avalanche/tools/Tupper3d
 * added modifiers
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "modifierpool.h"

TupModifierPool::TupModifierPool(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	m_pModifierPool = new TupperwarePoolValueString(pObject,"Modifier");
}

TupModifierPool::~TupModifierPool()
{
	delete m_pModifierPool;
}

int TupModifierPool::AddModifier(void)
{
	TupperwareAggregate *pAgg = m_pModifierPool->AddAggregate();
	assert(pAgg);
	return atoi(pAgg->GetName());
}

TupperwareAggregate *TupModifierPool::AddModifier(int index)
{
	return m_pModifierPool->AddAggregate(index);
}

TupperwareAggregate *TupModifierPool::GetModifier(int index)
{
	return m_pModifierPool->FindAggregate(index);
}

int TupModifierPool::GetNumModifiers(void)
{
	return m_pModifierPool->GetPoolSize();
}

void TupModifierPool::DeleteModifier(int index)
{
	m_pModifierPool->DeleteEntry(index);
}



