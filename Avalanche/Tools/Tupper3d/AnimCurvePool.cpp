////////////////////////////////////////////////////////////////////////////
//
// AnimCurvePool
//
// Class to handle a bunch of animation curves
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimCurvePool.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 4:19p
 * Updated in $/Avalanche/tools/Tupper3d
 * first phase of animation export
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "AnimCurvePool.h"

TupAnimCurvePool::TupAnimCurvePool(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	m_pAnimCurvePool = new TupperwarePoolValueString(pObject,"Curve");
}

TupAnimCurvePool::~TupAnimCurvePool()
{
	delete m_pAnimCurvePool;
}

int TupAnimCurvePool::AddAnimCurve( void )
{
	TupperwareAggregate *pAgg = m_pAnimCurvePool->AddAggregate();
	assert(pAgg);
	return atoi(pAgg->GetName());
}

TupperwareAggregate *TupAnimCurvePool::AddAnimCurve(int index)
{
	return m_pAnimCurvePool->AddAggregate(index);
}

TupperwareAggregate *TupAnimCurvePool::GetAnimCurve(int index)
{
	return m_pAnimCurvePool->FindAggregate(index);
}

int TupAnimCurvePool::GetNumAnimCurves(void)
{
	return m_pAnimCurvePool->GetPoolSize();
}

void TupAnimCurvePool::DeleteAnimCurve(int index)
{
	m_pAnimCurvePool->DeleteEntry(index);
}




