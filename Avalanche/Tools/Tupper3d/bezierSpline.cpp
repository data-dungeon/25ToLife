////////////////////////////////////////////////////////////////////////////
//
// BezierSpline
//
// Class for handling a single bezier spline
//
////////////////////////////////////////////////////////////////////////////


#include "Tupper3dPch.h"
#include "bezierspline.h"

char *TupBezierSpline::m_keyNames[TupBezierSpline::NUM_CHILDREN_ATOMS] = 
{
	"PointLocations", // POINT_LOCATIONS
	"InVectors", // IN_VECTORS
	"OutVectors", // OUT_VECTORS
	"PointFlags", // POINT_FLAGS
	"Flags", // FLAGS
};

TupBezierSpline::TupBezierSpline(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupBezierSpline::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupBezierSpline::GetBaseAggregate( void )
{
	return( m_pObject );
}

// Add Methods
TupperwareList *TupBezierSpline::AddPointLocations(const float *pPointLocations,int numPoints)
{
	return AddFloatList(pPointLocations,numPoints*3,TupBezierSpline::POINT_LOCATIONS);
}

TupperwareList *TupBezierSpline::AddInVectors(const float *pInVectors,int numPoints)
{
	return AddFloatList(pInVectors,numPoints*3,TupBezierSpline::IN_VECTORS);
}

TupperwareList *TupBezierSpline::AddOutVectors(const float *pOutVectors,int numPoints)
{
	return AddFloatList(pOutVectors,numPoints*3,TupBezierSpline::OUT_VECTORS);
}

TupperwareList *TupBezierSpline::AddPointFlags(const int *pPointFlags,int numPoints)
{
	return AddIntList(pPointFlags,numPoints,TupBezierSpline::POINT_FLAGS);
}

TupperwareScalar *TupBezierSpline::AddFlags(int flags)
{
	TupperwareAtom *pAtom = m_cachedAtoms[FLAGS];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	pAtom = m_cachedAtoms[FLAGS] = (TupperwareAtom *)m_pObject->AddScalarInt(m_keyNames[FLAGS],NULL,flags);
	return (TupperwareScalar *)pAtom;
}

// Get Methods
TupperwareList *TupBezierSpline::GetPointLocations(float **pPointLocations,int &numPoints)
{
	TupperwareList *pList = GetFloatList(pPointLocations,numPoints,TupBezierSpline::POINT_LOCATIONS);
	numPoints/=3;
	return pList;
}

TupperwareList *TupBezierSpline::GetInVectors(float **pInVectors,int &numPoints)
{
	TupperwareList *pList = GetFloatList(pInVectors,numPoints,TupBezierSpline::IN_VECTORS);
	numPoints/=3;
	return pList;
}

TupperwareList *TupBezierSpline::GetOutVectors(float **pOutVectors,int &numPoints)
{
	TupperwareList *pList = GetFloatList(pOutVectors,numPoints,TupBezierSpline::OUT_VECTORS);
	numPoints/=3;
	return pList;
}

TupperwareList *TupBezierSpline::GetPointFlags(int **pPointFlags,int &numPoints)
{
	return GetIntList(pPointFlags,numPoints,TupBezierSpline::POINT_FLAGS);
}

TupperwareScalar *TupBezierSpline::GetFlags(int &flags)
{
	TupperwareAtom *pAtom = m_cachedAtoms[FLAGS];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *scalar = (TupperwareScalar *)pAtom;
		if (scalar->GetDataType()==TupperwareScalar::DATA_INT)
		{
			flags = scalar->GetAsInt();
			return scalar;
		}
	}
	flags = 0;
	return NULL;
}

// Private Methods
TupperwareList *TupBezierSpline::AddFloatList(const float *pFloats,int numFloats,TupBezierSpline::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListFloat(m_keyNames[index],NULL,pFloats,numFloats);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareList *TupBezierSpline::AddIntList(const int *pInts,int numInts,TupBezierSpline::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListInt(m_keyNames[index],NULL,pInts,numInts);
	m_cachedAtoms[index] = list;
	return list;
}

TupperwareList *TupBezierSpline::GetFloatList(float **pFloats,int &numFloats,TupBezierSpline::ChildrenAtoms index)
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

TupperwareList *TupBezierSpline::GetIntList(int **pInts,int &numInts,TupBezierSpline::ChildrenAtoms index)
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
