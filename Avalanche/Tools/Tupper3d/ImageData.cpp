////////////////////////////////////////////////////////////////////////////
//
// ImageData
//
// Class to handle image data (actual bitmaps)
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImageData.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/14/02    Time: 11:21a
 * Updated in $/Avalanche/tools/Tupper3d
 * Added the ImageData Classes to old bitmap data
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "imagedata.h"

TupImageData::TupImageData(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
}

TupperwareAggregate *TupImageData::GetBaseAggregate(void)
{
	return m_pObject;
}

TupperwareAggregate *TupImageData::AddImageDataFrame(int frameNum)
{
	char frameString[20];
	itoa(frameNum,frameString,10);
	// see if one is already there with the frame number
	TupperwareAtom *pImageDataFrameAtom = m_pObject->FindAtomByKeyAndName("ImageDataFrame",frameString);
	// if so then replace it
	if (pImageDataFrameAtom)
	{
		m_pObject->DeleteAtom(pImageDataFrameAtom);
	}
	return m_pObject->AddAggregate("ImageDataFrame",frameString);
}

// get methods
int TupImageData::GetNumImageDataFrames(void)
{
	int total = 0;
	// make sure it has children
	if (m_pObject->GetFirstChildAtom())
	{
		for (TupperwareIterator i = m_pObject->FindByKey("ImageDataFrame"); ! i.Finished(); i.Next() ) 
		{
			// make sure the child is a aggregate
			TupperwareAtom *pImageDataFrameAtom = i.GetAsAtom();
			if (pImageDataFrameAtom && pImageDataFrameAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
			{
				total++;
			}
		}
	}
	return total;
}

TupperwareAggregate *TupImageData::GetImageDataFrame(int index)
{
	int count = 0;
	// make sure it has children
	if (m_pObject->GetFirstChildAtom())
	{
		for (TupperwareIterator i = m_pObject->FindByKey("ImageDataFrame"); ! i.Finished(); i.Next() ) 
		{
			// make sure the child is a aggregate
			TupperwareAtom *pImageDataFrameAtom = i.GetAsAtom();
			if (pImageDataFrameAtom && pImageDataFrameAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
			{
				if (index == count)
					return (TupperwareAggregate *)pImageDataFrameAtom;
				count++;
			}
		}
	}
	return NULL;
}
