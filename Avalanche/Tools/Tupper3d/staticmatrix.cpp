////////////////////////////////////////////////////////////////////////////
//
// StaticMatrix
//
// Class that deals with a transform using a single matrix
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: staticmatrix.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 5/16/03    Time: 1:50p
 * Updated in $/Avalanche/tools/Tupper3d
 * added more data for matrix components
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/06/01   Time: 1:51p
 * Updated in $/Avalanche/tools/Tupper3d
 * Add and Get functions complete
 * Added new files for handling various parts of the graphics format
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "staticmatrix.h"

char *TupStaticMatrix::m_keyNames[TupStaticMatrix::NUM_CHILDREN_ATOMS] = 
{
	"Matrix4x4", // MATRIX
	"Translation", // TRANSLATION
	"Rotation", // ROTATION
	"Scale", // SCALE
	"Shear", // SHEAR
	"ScalePivot", // SCALE_PIVOT
	"RotationPivot", // ROTATION_PIVOT
	"ScalePivotTranslation", // SCALE_PIVOT_TRANSLATION
	"RotationPivotTranslation", // ROTATION_PIVOT_TRANSLATION
	"RotationOrientation", // ROTATION_ORIENTATION
	"Flags", // FLAGS
};


TupStaticMatrix::TupStaticMatrix(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupStaticMatrix::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupStaticMatrix::GetBaseAggregate( void )
{
	return( m_pObject );
}

// Add Methods

TupperwareList *TupStaticMatrix::AddMatrix(const float *matrix)
{
	return AddFloatList(matrix,16,MATRIX);
}

TupperwareList *TupStaticMatrix::AddTranslation(const float *pValues)
{
	return AddFloatList(pValues,3,TRANSLATION);
}

TupperwareList *TupStaticMatrix::AddRotation(const float *pValues)
{
	return AddFloatList(pValues,3,ROTATION);
}

TupperwareList *TupStaticMatrix::AddScale(const float *pValues)
{
	return AddFloatList(pValues,3,SCALE);
}

TupperwareList *TupStaticMatrix::AddShear(const float *pValues)
{
	return AddFloatList(pValues,3,SHEAR);
}

TupperwareList *TupStaticMatrix::AddScalePivot(const float *pValues)
{
	return AddFloatList(pValues,4,SCALE_PIVOT);
}

TupperwareList *TupStaticMatrix::AddRotationPivot(const float *pValues)
{
	return AddFloatList(pValues,4,ROTATION_PIVOT);
}

TupperwareList *TupStaticMatrix::AddScalePivotTranslation(const float *pValues)
{
	return AddFloatList(pValues,3,SCALE_PIVOT_TRANSLATION);
}

TupperwareList *TupStaticMatrix::AddRotationPivotTranslation(const float *pValues)
{
	return AddFloatList(pValues,3,ROTATION_PIVOT_TRANSLATION);
}

TupperwareList *TupStaticMatrix::AddRotationOrientation(const float *pValues)
{
	return AddFloatList(pValues,3,ROTATION_ORIENTATION);
}

TupperwareScalar *TupStaticMatrix::AddFlags(int flags)
{
	return AddIntScalar(flags,FLAGS);
}

// Get Methods

TupperwareList *TupStaticMatrix::GetMatrix(float **matrix)
{
	int numFloats;
	return GetFloatList(matrix,numFloats,MATRIX);
}

TupperwareList *TupStaticMatrix::GetTranslation(float **pValues)
{
	int numFloats;
	return GetFloatList(pValues,numFloats,TRANSLATION);
}

TupperwareList *TupStaticMatrix::GetRotation(float **pValues)
{
	int numFloats;
	return GetFloatList(pValues,numFloats,ROTATION);
}

TupperwareList *TupStaticMatrix::GetScale(float **pValues)
{
	int numFloats;
	return GetFloatList(pValues,numFloats,SCALE);
}

TupperwareList *TupStaticMatrix::GetShear(float **pValues)
{
	int numFloats;
	return GetFloatList(pValues,numFloats,SHEAR);
}

TupperwareList *TupStaticMatrix::GetScalePivot(float **pValues)
{
	int numFloats;
	return GetFloatList(pValues,numFloats,SCALE_PIVOT);
}

TupperwareList *TupStaticMatrix::GetRotationPivot(float **pValues)
{
	int numFloats;
	return GetFloatList(pValues,numFloats,ROTATION_PIVOT);
}

TupperwareList *TupStaticMatrix::GetScalePivotTranslation(float **pValues)
{
	int numFloats;
	return GetFloatList(pValues,numFloats,SCALE_PIVOT_TRANSLATION);
}

TupperwareList *TupStaticMatrix::GetRotationPivotTranslation(float **pValues)
{
	int numFloats;
	return GetFloatList(pValues,numFloats,ROTATION_PIVOT_TRANSLATION);
}

TupperwareList *TupStaticMatrix::GetRotationOrientation(float **pValues)
{
	int numFloats;
	return GetFloatList(pValues,numFloats,ROTATION_ORIENTATION);
}

TupperwareScalar *TupStaticMatrix::GetFlags(int &flags)
{
	return GetIntScalar(flags,FLAGS);
}


// Private Methods
TupperwareList *TupStaticMatrix::AddFloatList(const float *pFloats,int numFloats,TupStaticMatrix::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListFloat(m_keyNames[index],NULL,pFloats,numFloats);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareList *TupStaticMatrix::AddIntList(const int *pInts,int numInts,TupStaticMatrix::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListInt(m_keyNames[index],NULL,pInts,numInts);
	m_cachedAtoms[index] = list;
	return list;
}

TupperwareList *TupStaticMatrix::GetFloatList(float **pFloats,int &numFloats,TupStaticMatrix::ChildrenAtoms index)
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

TupperwareList *TupStaticMatrix::GetIntList(int **pInts,int &numInts,TupStaticMatrix::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_LIST)
	{
		TupperwareList *pList = (TupperwareList *)pAtom;
		if (pList->GetDataType()==TupperwareList::DATA_INT)
		{
			*pInts = pList->GetAsInt();
			numInts = pList->GetLength();
			return pList;
		}
	}
	*pInts = NULL;
	numInts = 0;
	return NULL;
}

TupperwareScalar *TupStaticMatrix::AddFloatScalar(float value,TupStaticMatrix::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarFloat(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupStaticMatrix::GetFloatScalar(float &value,TupStaticMatrix::ChildrenAtoms index)
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

TupperwareScalar *TupStaticMatrix::AddIntScalar(int value,TupStaticMatrix::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupStaticMatrix::GetIntScalar(int &value,TupStaticMatrix::ChildrenAtoms index)
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
