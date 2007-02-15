////////////////////////////////////////////////////////////////////////////
//
// Spline
//
// Class for handling a single spline
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: spline.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/29/02    Time: 4:24p
 * Updated in $/Avalanche/tools/Tupper3d
 * Bezier shapes and splines
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "spline.h"

char *TupSpline::m_keyNames[TupSpline::NUM_CHILDREN_ATOMS] = 
{
	"ControlPoints", // CONTROL_POINTS
	"Knots", // KNOTS
	"Flags", // FLAGS
};

TupSpline::TupSpline(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupSpline::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupSpline::GetBaseAggregate( void )
{
  return( m_pObject );
}

// Add Methods
TupperwareList *TupSpline::AddControlPoints(const float *pControlPoints,int numPoints)
{
	return AddFloatList(pControlPoints,numPoints*4,TupSpline::CONTROL_POINTS);
}

TupperwareList *TupSpline::AddKnots(const float *pKnots,int numKnots)
{
	return AddFloatList(pKnots,numKnots,TupSpline::KNOTS);
}

TupperwareScalar *TupSpline::AddFlags(int flags)
{
	TupperwareAtom *pAtom = m_cachedAtoms[FLAGS];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	pAtom = m_cachedAtoms[FLAGS] = (TupperwareAtom *)m_pObject->AddScalarInt(m_keyNames[FLAGS],NULL,flags);
	return (TupperwareScalar *)pAtom;
}

// Get Methods
TupperwareList *TupSpline::GetControlPoints(float **pControlPoints,int &numPoints)
{
	TupperwareList *pList = GetFloatList(pControlPoints,numPoints,TupSpline::CONTROL_POINTS);
	numPoints/=4;
	return pList;
}

TupperwareList *TupSpline::GetKnots(float **pKnots,int &numKnots)
{
	return GetFloatList(pKnots,numKnots,TupSpline::KNOTS);
}

TupperwareScalar *TupSpline::GetFlags(int &flags)
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
TupperwareList *TupSpline::AddFloatList(const float *pFloats,int numFloats,TupSpline::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListFloat(m_keyNames[index],NULL,pFloats,numFloats);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareList *TupSpline::AddIntList(const int *pInts,int numInts,TupSpline::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListInt(m_keyNames[index],NULL,pInts,numInts);
	m_cachedAtoms[index] = list;
	return list;
}

TupperwareList *TupSpline::GetFloatList(float **pFloats,int &numFloats,TupSpline::ChildrenAtoms index)
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

TupperwareList *TupSpline::GetIntList(int **pInts,int &numInts,TupSpline::ChildrenAtoms index)
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
