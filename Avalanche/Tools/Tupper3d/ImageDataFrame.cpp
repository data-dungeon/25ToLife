////////////////////////////////////////////////////////////////////////////
//
// ImageData
//
// Class to handle image data (actual bitmaps)
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImageDataFrame.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 7/25/02    Time: 4:45p
 * Updated in $/Avalanche/tools/Tupper3d
 * added clear palette function
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
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
#include "imagedataframe.h"

char *TupImageDataFrame::m_keyNames[NUM_CHILDREN_ATOMS] = 
{
		"FormatFlags", // FORMAT_FLAGS
		"AuxilaryFlags", // AUXILARY_FLAGS
		"ImageDataPalette", // IMAGE_DATA_PALETTE
		"ImageDataMaps", // IMAGE_DATA_MAPS
};

TupImageDataFrame::TupImageDataFrame(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupperwareAggregate *TupImageDataFrame::GetBaseAggregate(void)
{
	return m_pObject;
}

void TupImageDataFrame::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

// add methods
TupperwareScalar *TupImageDataFrame::AddFormatFlags(int formatFlags)
{
	return AddIntScalar(formatFlags,FORMAT_FLAGS);
}

TupperwareScalar *TupImageDataFrame::AddAuxilaryFlags(int auxilaryFlags)
{
	return AddIntScalar(auxilaryFlags,AUXILARY_FLAGS);
}

TupperwareAggregate *TupImageDataFrame::AddImageDataPalette(void)
{
	TupperwareAtom *pAtom = m_cachedAtoms[IMAGE_DATA_PALETTE];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareAggregate *aggregate = m_pObject->AddAggregate(m_keyNames[IMAGE_DATA_PALETTE],NULL);
	m_cachedAtoms[IMAGE_DATA_PALETTE] = (TupperwareAtom *)aggregate;
	return aggregate;
}

TupperwareAggregate *TupImageDataFrame::AddImageDataMap(int mapIndex)
{
	TupperwareAtom *pAtom = m_cachedAtoms[IMAGE_DATA_MAPS];
	// delete if wrong type
	if (pAtom && pAtom->GetAtomType() != TupperwareAtom::ATOM_AGGREGATE)
	{
		m_pObject->DeleteAtom(pAtom);
		pAtom = NULL;
	}
	// Add if not there
	if (!pAtom)
	{
		pAtom = m_cachedAtoms[IMAGE_DATA_MAPS] = (TupperwareAtom *)m_pObject->AddAggregate(m_keyNames[IMAGE_DATA_MAPS]);
	}
	TupperwareAggregate *pImageDataMapsAgg = (TupperwareAggregate *)pAtom;

	char mapString[20];
	itoa(mapIndex,mapString,10);
	// see if one is already there
	TupperwareAtom *pImageDataMapAtom = pImageDataMapsAgg->FindAtomByKeyAndName("ImageDataMap",mapString);
	// if so then delete it
	if (pImageDataMapAtom)
	{
		pImageDataMapsAgg->DeleteAtom(pImageDataMapAtom);
	}
	return pImageDataMapsAgg->AddAggregate("ImageDataMap",mapString);
}

// get methods
TupperwareScalar *TupImageDataFrame::GetFormatFlags(int &formatFlags)
{
	return GetIntScalar(formatFlags,FORMAT_FLAGS);
}

TupperwareScalar *TupImageDataFrame::GetAuxilaryFlags(int &auxilaryFlags)
{
	return GetIntScalar(auxilaryFlags,AUXILARY_FLAGS);
}

TupperwareAggregate *TupImageDataFrame::GetImageDataPalette(void)
{
	TupperwareAtom *pAtom = m_cachedAtoms[IMAGE_DATA_PALETTE];
	if (pAtom && pAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
	{
		return (TupperwareAggregate *) pAtom;
	}
	return NULL;
}

int TupImageDataFrame::GetNumImageDataMaps(void)
{
	int total = 0;
	TupperwareAtom *pImageDataMapsAtom = m_cachedAtoms[IMAGE_DATA_MAPS];
	// if right type
	if (pImageDataMapsAtom && pImageDataMapsAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
	{
		TupperwareAggregate *pImageDataMapsAgg = (TupperwareAggregate *)pImageDataMapsAtom;
		// make sure it has children
		if (pImageDataMapsAgg->GetFirstChildAtom())
		{
			for (TupperwareIterator i = pImageDataMapsAgg->FindByKey("ImageDataMap"); ! i.Finished(); i.Next() ) 
			{
				// make sure the child is a aggregate
				TupperwareAtom *pImageDataMapAtom = i.GetAsAtom();
				if (pImageDataMapAtom && pImageDataMapAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
				{
					total++;
				}
			}
		}
	}
	return total;
}

TupperwareAggregate *TupImageDataFrame::GetImageDataMap(int mapIndex)
{
	int count = 0;
	TupperwareAtom *pImageDataMapsAtom = m_cachedAtoms[IMAGE_DATA_MAPS];
	// if right type
	if (pImageDataMapsAtom && pImageDataMapsAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
	{
		TupperwareAggregate *pImageDataMapsAgg = (TupperwareAggregate *)pImageDataMapsAtom;
		// make sure it has children
		if (pImageDataMapsAgg->GetFirstChildAtom())
		{
			for (TupperwareIterator i = pImageDataMapsAgg->FindByKey("ImageDataMap"); ! i.Finished(); i.Next() ) 
			{
				// make sure the child is a aggregate
				TupperwareAtom *pImageDataMapAtom = i.GetAsAtom();
				if (pImageDataMapAtom && pImageDataMapAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
				{
					if (mapIndex == count)
						return (TupperwareAggregate *)pImageDataMapAtom;
					count++;
				}
			}
		}
	}
	return NULL;
}

TupperwareScalar *TupImageDataFrame::AddIntScalar(int value,TupImageDataFrame::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupImageDataFrame::GetIntScalar(int &value,TupImageDataFrame::ChildrenAtoms index)
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

// clear methods
void TupImageDataFrame::ClearImageDataPalette(void)
{
	TupperwareAtom *pAtom = m_cachedAtoms[IMAGE_DATA_PALETTE];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	m_cachedAtoms[IMAGE_DATA_PALETTE] = NULL;
}
