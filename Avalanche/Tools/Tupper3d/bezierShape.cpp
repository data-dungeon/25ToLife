////////////////////////////////////////////////////////////////////////////
//
// BezierShape
//
// Class for handling bezier shapes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: bezierShape.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/29/02    Time: 4:24p
 * Updated in $/Avalanche/tools/Tupper3d
 * Bezier shapes and splines
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "beziershape.h"
#include "object.h"

char *TupBezierShape::m_keyNames[TupBezierShape::NUM_CHILDREN_ATOMS] = 
{
	"Flags", // FLAGS
	"Splines", // SPLINES
};

TupBezierShape::TupBezierShape(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupBezierShape::TupBezierShape(TupObject &objectBase)
{
	m_pObject = objectBase.GetBaseAggregate();
	BuildAtomCache();
}

void TupBezierShape::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupBezierShape::GetBaseAggregate( void )
{
  return( m_pObject );
}

// add methods
TupperwareScalar *TupBezierShape::AddFlags(int flags)
{
	TupperwareAtom *pAtom = m_cachedAtoms[FLAGS];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	pAtom = m_cachedAtoms[FLAGS] = (TupperwareAtom *)m_pObject->AddScalarInt(m_keyNames[FLAGS],NULL,flags);
	return (TupperwareScalar *)pAtom;
}

TupperwareAggregate *TupBezierShape::AddSpline(int index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[SPLINES];
	// delete if wrong type
	if (pAtom && pAtom->GetAtomType() != TupperwareAtom::ATOM_AGGREGATE)
	{
		m_pObject->DeleteAtom(pAtom);
		pAtom = NULL;
	}
	// Add if not there
	if (!pAtom)
	{
		pAtom = m_cachedAtoms[SPLINES] = (TupperwareAtom *)m_pObject->AddAggregate(m_keyNames[SPLINES]);
	}
	TupperwareAggregate *pSplinesAgg = (TupperwareAggregate *)pAtom;

	char indexString[20];
	itoa(index,indexString,10);
	TupperwareAtom *pSplineAtom = pSplinesAgg->FindAtomByKeyAndName("Spline",indexString);
	if (pSplineAtom)
	{
		pSplinesAgg->DeleteAtom(pSplineAtom);
	}
	return pSplinesAgg->AddAggregate("Spline",indexString);
}

// get methods
TupperwareScalar *TupBezierShape::GetFlags(int &flags)
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

int TupBezierShape::GetNumSplines(void)
{
	int total = 0;
	TupperwareAtom *pSplinesAtom = m_cachedAtoms[SPLINES];
	// if right type
	if (pSplinesAtom && pSplinesAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
	{
		TupperwareAggregate *pSplinesAgg = (TupperwareAggregate *)pSplinesAtom;
		// make sure it has children
		if (pSplinesAgg->GetFirstChildAtom())
		{
			for (TupperwareIterator i = pSplinesAgg->FindByKey("Spline"); ! i.Finished(); i.Next() ) 
			{
				// make sure the child is a aggregate
				TupperwareAtom *pSplineAtom = i.GetAsAtom();
				if (pSplineAtom && pSplineAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
				{
					total++;
				}
			}
		}
	}
	return total;
}

TupperwareAggregate *TupBezierShape::GetSpline(int index)
{
	int count = 0;
	TupperwareAtom *pSplinesAtom = m_cachedAtoms[SPLINES];
	// if right type
	if (pSplinesAtom && pSplinesAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
	{
		TupperwareAggregate *pSplinesAgg = (TupperwareAggregate *)pSplinesAtom;
		// make sure it has children
		if (pSplinesAgg->GetFirstChildAtom())
		{
			for (TupperwareIterator i = pSplinesAgg->FindByKey("Spline"); ! i.Finished(); i.Next() ) 
			{
				// make sure the child is a aggregate
				TupperwareAtom *pSplineAtom = i.GetAsAtom();
				if (pSplineAtom && pSplineAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
				{
					if (index == count)
						return (TupperwareAggregate *)pSplineAtom;
					count++;
				}
			}
		}
	}
	return NULL;
}
