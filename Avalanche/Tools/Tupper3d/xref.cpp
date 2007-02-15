////////////////////////////////////////////////////////////////////////////
//
// XRef
//
// Class to handle xrefs
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: xref.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/16/02    Time: 3:44p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added constructors that use the parent as a parameter
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/27/01   Time: 4:31p
 * Updated in $/Avalanche/tools/Tupper3d
 * xrefs and derived objects
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "xref.h"
#include "object.h"

char *TupXRef::m_keyNames[TupXRef::NUM_CHILDREN_ATOMS] = 
{
	"Filename", // FILENAME
	"NodeName", // NODE_NAME
};

TupXRef::TupXRef(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupXRef::TupXRef(TupObject &objectBase)
{
	m_pObject = objectBase.GetBaseAggregate();
	BuildAtomCache();
}

void TupXRef::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupXRef::GetBaseAggregate( void )
{
  return( m_pObject );
}

TupperwareScalar *TupXRef::AddFilename(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[FILENAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[FILENAME],NULL,name);
	m_cachedAtoms[FILENAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupXRef::AddNodeName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NODE_NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[NODE_NAME],NULL,name);
	m_cachedAtoms[NODE_NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

// Get methods

TupperwareScalar *TupXRef::GetFilename(char **name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[FILENAME];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType() == TupperwareScalar::DATA_STRING)
		{
			*name = pScalar->GetAsString();
			return pScalar;
		}
	}
	*name = NULL;
	return NULL;
}

TupperwareScalar *TupXRef::GetNodeName(char **name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NODE_NAME];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType() == TupperwareScalar::DATA_STRING)
		{
			*name = pScalar->GetAsString();
			return pScalar;
		}
	}
	*name = NULL;
	return NULL;
}

