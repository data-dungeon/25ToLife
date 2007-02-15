////////////////////////////////////////////////////////////////////////////
//
// Lookat
//
// Class for lookats
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: lookat.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 6/09/03    Time: 2:55p
 * Updated in $/Avalanche/tools/Tupper3d
 * camera lookat node references
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 6/06/03    Time: 4:06p
 * Updated in $/Avalanche/tools/Tupper3d
 * instanced clip curves
 * camera lookat
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 2:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Animation Wrapper Classes
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "lookat.h"
#include "object.h"

char *TupLookAt::m_keyNames[TupLookAt::NUM_CHILDREN_ATOMS] = 
{
	"DistanceBetween", // DISTANCE_BETWEEN
	"Twist", // TWIST
	"WorldUpVector", // WORLD_UP_VECTOR
	"AimVector", // AIM_VECTOR
	"UpVector", // UP_VECTOR
	"CameraNodeReference", // CAMERA_NODE_REFERENCE
	"AimNodeReference", // AIM_NODE_REFERENCE
	"UpNodeReference", // UP_NODE_REFERENCE
	"Flags", // FLAGS
};

TupLookAt::TupLookAt(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupLookAt::TupLookAt(TupObject &objectBase)
{
	m_pObject = objectBase.GetBaseAggregate();
	BuildAtomCache();
}

void TupLookAt::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

// add methods
TupperwareScalar *TupLookAt::AddDistanceBetween(float value)
{
	return AddFloatScalar(value,TupLookAt::DISTANCE_BETWEEN);
}

TupperwareScalar *TupLookAt::AddTwist(float value)
{
	return AddFloatScalar(value,TupLookAt::TWIST);
}

TupperwareList *TupLookAt::AddWorldUpVector(float *pValues)
{
	return AddFloatList(pValues,3,TupLookAt::WORLD_UP_VECTOR);
}

TupperwareList *TupLookAt::AddAimVector(float *pValues)
{
	return AddFloatList(pValues,3,TupLookAt::AIM_VECTOR);
}

TupperwareList *TupLookAt::AddUpVector(float *pValues)
{
	return AddFloatList(pValues,3,TupLookAt::UP_VECTOR);
}

TupperwareScalar *TupLookAt::AddCameraNodeReference(int ref)
{
	return AddIntScalar(ref,TupLookAt::CAMERA_NODE_REFERENCE);
}

TupperwareScalar *TupLookAt::AddUpNodeReference(int ref)
{
	return AddIntScalar(ref,TupLookAt::UP_NODE_REFERENCE);
}

TupperwareScalar *TupLookAt::AddAimNodeReference(int ref)
{
	return AddIntScalar(ref,TupLookAt::AIM_NODE_REFERENCE);
}

TupperwareScalar *TupLookAt::AddFlags(int value)
{
	return AddIntScalar(value,TupLookAt::FLAGS);
}



// get methods
TupperwareScalar *TupLookAt::GetDistanceBetween(float &value)
{
	return GetFloatScalar(value,TupLookAt::DISTANCE_BETWEEN);
}

TupperwareScalar *TupLookAt::GetTwist(float &value)
{
	return GetFloatScalar(value,TupLookAt::TWIST);
}

TupperwareList *TupLookAt::GetWorldUpVector(float **pValues)
{
	int numFloats;
	return GetFloatList(pValues,numFloats,TupLookAt::WORLD_UP_VECTOR);
}

TupperwareList *TupLookAt::GetAimVector(float **pValues)
{
	int numFloats;
	return GetFloatList(pValues,numFloats,TupLookAt::AIM_VECTOR);
}

TupperwareList *TupLookAt::GetUpVector(float **pValues)
{
	int numFloats;
	return GetFloatList(pValues,numFloats,TupLookAt::UP_VECTOR);
}

TupperwareScalar *TupLookAt::GetCameraNodeReference(int &ref)
{
	return GetIntScalar(ref,TupLookAt::CAMERA_NODE_REFERENCE);
}

TupperwareScalar *TupLookAt::GetUpNodeReference(int &ref)
{
	return GetIntScalar(ref,TupLookAt::UP_NODE_REFERENCE);
}

TupperwareScalar *TupLookAt::GetAimNodeReference(int &ref)
{
	return GetIntScalar(ref,TupLookAt::AIM_NODE_REFERENCE);
}

TupperwareScalar *TupLookAt::GetFlags(int &value)
{
	return GetIntScalar(value,TupLookAt::FLAGS);
}

// private methods							

TupperwareScalar *TupLookAt::AddFloatScalar(float value,TupLookAt::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarFloat(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupLookAt::GetFloatScalar(float &value,TupLookAt::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *scalar = (TupperwareScalar *)pAtom;
		if (scalar->GetDataType()==TupperwareScalar::DATA_FLOAT)
		{
			value = scalar->GetAsFloat();
			return scalar;
		}
	}
	value = 0;
	return NULL;
}

TupperwareScalar *TupLookAt::AddIntScalar(int value,TupLookAt::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareList *TupLookAt::AddFloatList(const float *pFloats,int numFloats,TupLookAt::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListFloat(m_keyNames[index],NULL,pFloats,numFloats);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}


TupperwareScalar *TupLookAt::GetIntScalar(int &value,TupLookAt::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *scalar = (TupperwareScalar *)pAtom;
		if (scalar->GetDataType()==TupperwareScalar::DATA_INT)
		{
			value = scalar->GetAsInt();
			return scalar;
		}
	}
	value = 0;
	return NULL;
}

TupperwareList *TupLookAt::GetFloatList(float **pFloats,int &numFloats,TupLookAt::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_LIST)
	{
		TupperwareList *pList = (TupperwareList *)pAtom;
		if (pList->GetDataType()==TupperwareList::DATA_FLOAT)
		{
			*pFloats = pList->GetAsFloat();
			numFloats = pList->GetLength();
			return pList;
		}
	}
	*pFloats = NULL;
	numFloats = 0;
	return NULL;
}
