////////////////////////////////////////////////////////////////////////////
//
// Image
//
// Class to handle images
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: image.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 5/08/02    Time: 9:17a
 * Updated in $/Avalanche/tools/Tupper3d
 * getbaseaggregate
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:34p
 * Updated in $/Avalanche/tools/Tupper3d
 * Material - Map stuff
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "image.h"

char *TupImage::m_keyNames[TupImage::NUM_CHILDREN_ATOMS] = 
{
	"FilePathName", // FILE_PATH_NAME
	"UserProperties", // USER_PROPERTIES
};

TupImage::TupImage(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupperwareAggregate *TupImage::GetBaseAggregate(void)
{
	return m_pObject;
}

void TupImage::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareScalar *TupImage::AddFilePathName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[FILE_PATH_NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[FILE_PATH_NAME],NULL,name);
	m_cachedAtoms[FILE_PATH_NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupImage::AddUserProperties(const char *userPropertyString)
{
	TupperwareAtom *pAtom = m_cachedAtoms[USER_PROPERTIES];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[USER_PROPERTIES],NULL,userPropertyString);
	m_cachedAtoms[USER_PROPERTIES] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupImage::GetFilePathName(char **name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[FILE_PATH_NAME];
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

TupperwareScalar *TupImage::GetUserProperties(char **userPropertyString)
{
	TupperwareAtom *pAtom = m_cachedAtoms[USER_PROPERTIES];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType() == TupperwareScalar::DATA_STRING)
		{
			*userPropertyString = pScalar->GetAsString();
			return pScalar;
		}
	}
	*userPropertyString = NULL;
	return NULL;
}
