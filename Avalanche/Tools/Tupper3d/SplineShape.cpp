////////////////////////////////////////////////////////////////////////////
//
// SplineShape
//
// Class for handling spline shapes
//
////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "splineshape.h"
#include "object.h"

char *TupSplineShape::m_keyNames[TupSplineShape::NUM_CHILDREN_ATOMS] = 
{
	"Flags", // FLAGS
	"Splines", // SPLINES
};

TupSplineShape::TupSplineShape(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupSplineShape::TupSplineShape(TupObject &objectBase)
{
	m_pObject = objectBase.GetBaseAggregate();
	BuildAtomCache();
}

void TupSplineShape::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupSplineShape::GetBaseAggregate( void )
{
	return( m_pObject );
}

// add methods
TupperwareScalar *TupSplineShape::AddFlags(int flags)
{
	TupperwareAtom *pAtom = m_cachedAtoms[FLAGS];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	pAtom = m_cachedAtoms[FLAGS] = (TupperwareAtom *)m_pObject->AddScalarInt(m_keyNames[FLAGS],NULL,flags);
	return (TupperwareScalar *)pAtom;
}

TupperwareAggregate *TupSplineShape::AddSpline(int index)
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
TupperwareScalar *TupSplineShape::GetFlags(int &flags)
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

int TupSplineShape::GetNumSplines(void)
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

TupperwareAggregate *TupSplineShape::GetSpline(int index)
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
