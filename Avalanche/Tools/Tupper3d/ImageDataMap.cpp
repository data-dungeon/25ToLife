////////////////////////////////////////////////////////////////////////////
//
// ImageData
//
// Class to handle image data (actual bitmaps)
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImageDataMap.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/27/02    Time: 2:24p
 * Updated in $/Avalanche/tools/Tupper3d
 * Made all Add list functions pass const pointers
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/14/02    Time: 2:00p
 * Updated in $/Avalanche/tools/Tupper3d
 * BuildAtomCache fix (it would help to call it)
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/14/02    Time: 11:21a
 * Updated in $/Avalanche/tools/Tupper3d
 * Added the ImageData Classes to old bitmap data
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "ImageDataMap.h"

char *TupImageDataMap::m_keyNames[NUM_CHILDREN_ATOMS] = 
{
		"Width", // WIDTH
		"Height", // HEIGHT
		"Data", // DATA
};

TupImageDataMap::TupImageDataMap(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupperwareAggregate *TupImageDataMap::GetBaseAggregate(void)
{
	return m_pObject;
}

void TupImageDataMap::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

// add methods
TupperwareScalar *TupImageDataMap::AddWidth(int width)
{
	return AddIntScalar(width,WIDTH);
}

TupperwareScalar *TupImageDataMap::AddHeight(int height)
{
	return AddIntScalar(height,HEIGHT);
}

TupperwareScalar *TupImageDataMap::AddData(const void *pData,int numBytes)
{
	TupperwareAtom *pAtom = m_cachedAtoms[DATA];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarBinary(m_keyNames[DATA],NULL,pData,numBytes,NULL);
	m_cachedAtoms[DATA] = (TupperwareAtom *)scalar;
	return scalar;
}


// get methods
TupperwareScalar *TupImageDataMap::GetWidth(int &width)
{
	return GetIntScalar(width,WIDTH);
}

TupperwareScalar *TupImageDataMap::GetHeight(int &height)
{
	return GetIntScalar(height,HEIGHT);
}

TupperwareScalar *TupImageDataMap::GetData(void **pData,int &numBytes)
{
	TupperwareAtom *pAtom = m_cachedAtoms[DATA];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *scalar = (TupperwareScalar *)pAtom;
		if (scalar->GetDataType()==TupperwareScalar::DATA_BINARY)
		{
			*pData = scalar->GetAsBinary(&numBytes);
			return scalar;
		}
	}
	*pData = NULL;
	numBytes = 0;
	return NULL;
}

TupperwareScalar *TupImageDataMap::AddIntScalar(int value,TupImageDataMap::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupImageDataMap::GetIntScalar(int &value,TupImageDataMap::ChildrenAtoms index)
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
