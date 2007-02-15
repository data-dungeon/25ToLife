////////////////////////////////////////////////////////////////////////////
//
// AnimClipBlend
//
// Class to handle a blend between two instanced clips
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimClipBlend.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/20/03    Time: 2:25p
 * Updated in $/Avalanche/tools/Tupper3d
 * add clip blends
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "AnimClipBlend.h"

char *TupAnimClipBlend::m_keyNames[TupAnimClipBlend::NUM_CHILDREN_ATOMS] = 
{
	"Name", // NAME
	"ClipInstance1Reference", // CLIP_INSTANCE_1_REFERENCE
	"ClipInstance2Reference", // CLIP_INSTANCE_2_REFERENCE
	"Curves", // CURVES
	"Flags", // FLAGS
};

TupAnimClipBlend::TupAnimClipBlend(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupAnimClipBlend::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupAnimClipBlend::GetBaseAggregate( void )
{
	return( m_pObject );
}

TupperwareScalar *TupAnimClipBlend::AddName(const char *name)
{
	return AddStringScalar(name,NAME);
}

TupperwareScalar *TupAnimClipBlend::AddClipInstance1Reference(int ref)
{
	return AddIntScalar(ref,CLIP_INSTANCE_1_REFERENCE);
}

TupperwareScalar *TupAnimClipBlend::AddClipInstance2Reference(int ref)
{
	return AddIntScalar(ref,CLIP_INSTANCE_2_REFERENCE);
}

TupperwareAggregate *TupAnimClipBlend::AddCurves(void)
{
	return AddAggregate(CURVES);
}

TupperwareScalar *TupAnimClipBlend::AddFlags(int flags)
{
	return AddIntScalar(flags,FLAGS);
}

// get methods

TupperwareScalar *TupAnimClipBlend::GetName(char **name)
{
	return GetStringScalar(name,NAME);
}

TupperwareScalar *TupAnimClipBlend::GetClipInstance1Reference(int &ref)
{
	return GetIntScalar(ref,CLIP_INSTANCE_1_REFERENCE);
}

TupperwareScalar *TupAnimClipBlend::GetClipInstance2Reference(int &ref)
{
	return GetIntScalar(ref,CLIP_INSTANCE_2_REFERENCE);
}

TupperwareAggregate *TupAnimClipBlend::GetCurves(void)
{
	return GetAggregate(CURVES);
}

TupperwareScalar *TupAnimClipBlend::GetFlags(int &flags)
{
	return GetIntScalar(flags,FLAGS);
}

// private methods
TupperwareScalar *TupAnimClipBlend::AddStringScalar(const char *pString,TupAnimClipBlend::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *pScalar = m_pObject->AddScalarString(m_keyNames[index],NULL,pString);
	m_cachedAtoms[index] = (TupperwareAtom *)pScalar;
	return pScalar;
}

TupperwareScalar *TupAnimClipBlend::GetStringScalar(char **pString,TupAnimClipBlend::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *) pAtom;
		if (pScalar->GetDataType() == TupperwareScalar::DATA_STRING)
		{
			*pString = pScalar->GetAsString();
			return pScalar;
		}
	}
	*pString = NULL;
	return NULL;
}

TupperwareScalar *TupAnimClipBlend::AddIntScalar(int value,TupAnimClipBlend::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupAnimClipBlend::GetIntScalar(int &value,TupAnimClipBlend::ChildrenAtoms index)
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

TupperwareAggregate *TupAnimClipBlend::AddAggregate(TupAnimClipBlend::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareAggregate *pAgg = m_pObject->AddAggregate(m_keyNames[index],NULL);
	m_cachedAtoms[index] = (TupperwareAtom *)pAgg;
	return pAgg;
}

TupperwareAggregate *TupAnimClipBlend::GetAggregate(TupAnimClipBlend::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
	{
		return (TupperwareAggregate *)pAtom;
	}
	return NULL;
}
