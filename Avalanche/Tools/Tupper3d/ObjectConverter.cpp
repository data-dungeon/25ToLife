////////////////////////////////////////////////////////////////////////////
//
// ObjectConverter
//
// Handle the conversion from one object to another
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ObjectConverter.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 7/28/03    Time: 3:10p
 * Updated in $/Avalanche/tools/Tupper3d
 * polymesh and start of object converters
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "ObjectConverter.h"
#include "ObjectPool.h"
#include "ModifierPool.h"
#include "derived.h"
#include "TriMeshToPolyMeshConverter.h"
#include "PolyMeshToTriMeshConverter.h"

TupObjectConverter::TupObjectConverter(TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool) :
	m_tupObjectPool(tupObjectPool),
	m_tupModifierPool(tupModifierPool)
{
	BuildDerivedObjectsRefs();
}

bool TupObjectConverter::SupportedConversion(TupObject::ObjectTypes fromType,TupObject::ObjectTypes toType)
{
	if (fromType==TupObject::TRI_MESH && toType==TupObject::POLY_MESH)
	{
		return true;
	}
	else if (fromType==TupObject::POLY_MESH && toType==TupObject::TRI_MESH)
	{
		return true;
	}
	return false;
}

bool TupObjectConverter::ConvertObjects(TupObject::ObjectTypes fromType,TupObject::ObjectTypes toType)
{
	int numObjects = m_tupObjectPool.GetNumObjects();
	for (int objectIndex=0;objectIndex<numObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_tupObjectPool.GetObjectAggregate(objectIndex);
		if (!pObjectAgg)
			continue;
		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType()==fromType)
		{
			bool bResult = ConvertObject(objectIndex,toType);
			if (!bResult)
				return false;
		}
	}
	return true;
}

bool TupObjectConverter::ConvertObject(int objectRef,TupObject::ObjectTypes toType)
{
	TupperwareAggregate *pObjectAgg = m_tupObjectPool.GetObjectAggregate(objectRef);
	if (!pObjectAgg)
		return false;
	TupObject tupObject(pObjectAgg);
	TupObject::ObjectTypes fromType = tupObject.GetObjectType();

	if (fromType==TupObject::TRI_MESH && toType==TupObject::POLY_MESH)
	{
		return TupTriMeshToPolyMeshConverter::Convert(objectRef,m_tupObjectPool,m_tupModifierPool,m_derivedObjectsRefs);
	}
	else if (fromType==TupObject::POLY_MESH && toType==TupObject::TRI_MESH)
	{
		return TupPolyMeshToTriMeshConverter::Convert(objectRef,m_tupObjectPool,m_tupModifierPool,m_derivedObjectsRefs);
	}
	return false;
}

void TupObjectConverter::BuildDerivedObjectsRefs(void)
{
	int numObjects = m_tupObjectPool.GetNumObjects();
	m_derivedObjectsRefs.SetSize(numObjects);
	for (int objectIndex=0;objectIndex<numObjects;objectIndex++)
	{
		TupperwareAggregate *pObjectAgg = m_tupObjectPool.GetObjectAggregate(objectIndex);
		if (!pObjectAgg)
			continue;
		TupObject tupObject(pObjectAgg);
		if (tupObject.GetObjectType()==TupObject::DERIVED)
		{
			int baseRef = GetBaseObjectRef(objectIndex);
			m_derivedObjectsRefs[baseRef].Add(objectIndex); // make sure the base knows about me
		}
	}
}

int TupObjectConverter::GetBaseObjectRef(int objectRef)
{
	TupperwareAggregate *pObjectAgg = m_tupObjectPool.GetObjectAggregate(objectRef);
	if (!pObjectAgg)
		return objectRef;
	TupObject tupObject(pObjectAgg);
	if (tupObject.GetObjectType()==TupObject::DERIVED)
	{
		TupDerived tupDerived(pObjectAgg);
		int nextObjectRef;
		tupDerived.GetObjectReference(nextObjectRef);
		return GetBaseObjectRef(nextObjectRef);
	}
	else
	{
		return objectRef;
	}
}
