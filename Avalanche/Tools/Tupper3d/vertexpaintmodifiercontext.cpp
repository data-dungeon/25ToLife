////////////////////////////////////////////////////////////////////////////
//
// VertexPaintModifierContext
//
// Class to handle the vertex paint modifier context
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: vertexpaintmodifiercontext.cpp $
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/05/02    Time: 1:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * Apply Vertex Color Modifiers to Derived objects
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/27/02    Time: 2:24p
 * Updated in $/Avalanche/tools/Tupper3d
 * Made all Add list functions pass const pointers
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/04/02    Time: 1:23p
 * Updated in $/Avalanche/tools/Tupper3d
 * Vertex Paint Modifier
 * Unknown Modifier
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/03/02    Time: 4:31p
 * Updated in $/Avalanche/tools/Tupper3d
 * Vertex Paint Modifier (pass 1)
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "vertexpaintmodifiercontext.h"

char *TupVertexPaintModifierContext::m_keyNames[TupVertexPaintModifierContext::NUM_CHILDREN_ATOMS] = 
{
	"VertexColorChannelColors", // VERTEX_COLOR_CHANNEL_COLORS
	"VertexColorChannelMixFactors", // VERTEX_COLOR_CHANNEL_MIX_FACTORS
	"VertexColorChannelFaceIndices", // VERTEX_COLOR_CHANNEL_FACE_INDICES
	"VertexColorChannelPointIndices", // VERTEX_COLOR_CHANNEL_POINT_INDICES
	"IlluminationChannelColors", // ILLUMINATION_CHANNEL_COLORS
	"IlluminationChannelMixFactors", // ILLUMINATION_COLOR_CHANNEL_MIX_FACTORS
	"IlluminationChannelFaceIndices", // ILLUMINATION_CHANNEL_FACE_INDICES
	"IlluminationChannelPointIndices", // ILLUMINATION_CHANNEL_POINT_INDICES
	"AlphaChannelColors", // ALPHA_CHANNEL_COLORS
	"AlphaChannelMixFactors", // ALPHA_CHANNEL_MIX_FACTORS
	"AlphaChannelFaceIndices", // ALPHA_CHANNEL_FACE_INDICES
	"AlphaChannelPointIndices", // ALPHA_CHANNEL_POINT_INDICES
};

TupVertexPaintModifierContext::TupVertexPaintModifierContext(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupVertexPaintModifierContext::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupVertexPaintModifierContext::GetBaseAggregate(void)
{
	return m_pObject;
}

TupperwareList *TupVertexPaintModifierContext::AddVertexColorChannelColorData(const float *pValues,int numColors)
{
	return AddFloatList(pValues,numColors*3,TupVertexPaintModifierContext::VERTEX_COLOR_CHANNEL_COLORS);
}

TupperwareList *TupVertexPaintModifierContext::AddVertexColorChannelMixFactorData(const float *pValues,int numColors)
{
	return AddFloatList(pValues,numColors,TupVertexPaintModifierContext::VERTEX_COLOR_CHANNEL_MIX_FACTORS);
}

TupperwareList *TupVertexPaintModifierContext::AddVertexColorChannelFaceIndexData(const int *pValues,int numColors)
{
	return AddIntList(pValues,numColors,TupVertexPaintModifierContext::VERTEX_COLOR_CHANNEL_FACE_INDICES);
}

TupperwareList *TupVertexPaintModifierContext::AddVertexColorChannelPointIndexData(const int *pValues,int numColors)
{
	return AddIntList(pValues,numColors,TupVertexPaintModifierContext::VERTEX_COLOR_CHANNEL_POINT_INDICES);
}

TupperwareList *TupVertexPaintModifierContext::AddIlluminationChannelColorData(const float *pValues,int numColors)
{
	return AddFloatList(pValues,numColors*3,TupVertexPaintModifierContext::ILLUMINATION_CHANNEL_COLORS);
}

TupperwareList *TupVertexPaintModifierContext::AddIlluminationChannelMixFactorData(const float *pValues,int numColors)
{
	return AddFloatList(pValues,numColors,TupVertexPaintModifierContext::ILLUMINATION_CHANNEL_MIX_FACTORS);
}

TupperwareList *TupVertexPaintModifierContext::AddIlluminationChannelFaceIndexData(const int *pValues,int numColors)
{
	return AddIntList(pValues,numColors,TupVertexPaintModifierContext::ILLUMINATION_CHANNEL_FACE_INDICES);
}

TupperwareList *TupVertexPaintModifierContext::AddIlluminationChannelPointIndexData(const int *pValues,int numColors)
{
	return AddIntList(pValues,numColors,TupVertexPaintModifierContext::ILLUMINATION_CHANNEL_POINT_INDICES);
}

TupperwareList *TupVertexPaintModifierContext::AddAlphaChannelColorData(const float *pValues,int numColors)
{
	return AddFloatList(pValues,numColors,TupVertexPaintModifierContext::ALPHA_CHANNEL_COLORS);
}

TupperwareList *TupVertexPaintModifierContext::AddAlphaChannelMixFactorData(const float *pValues,int numColors)
{
	return AddFloatList(pValues,numColors,TupVertexPaintModifierContext::ALPHA_CHANNEL_MIX_FACTORS);
}

TupperwareList *TupVertexPaintModifierContext::AddAlphaChannelFaceIndexData(const int *pValues,int numColors)
{
	return AddIntList(pValues,numColors,TupVertexPaintModifierContext::ALPHA_CHANNEL_FACE_INDICES);
}

TupperwareList *TupVertexPaintModifierContext::AddAlphaChannelPointIndexData(const int *pValues,int numColors)
{
	return AddIntList(pValues,numColors,TupVertexPaintModifierContext::ALPHA_CHANNEL_POINT_INDICES);
}

// get methods

TupperwareList *TupVertexPaintModifierContext::GetVertexColorChannelColorData(float **pValues,int &numColors)
{
	TupperwareList *list = GetFloatList(pValues,numColors,TupVertexPaintModifierContext::VERTEX_COLOR_CHANNEL_COLORS);
	numColors/=3;
	return list;
}

TupperwareList *TupVertexPaintModifierContext::GetVertexColorChannelMixFactorData(float **pValues,int &numColors)
{
	return GetFloatList(pValues,numColors,TupVertexPaintModifierContext::VERTEX_COLOR_CHANNEL_MIX_FACTORS);
}

TupperwareList *TupVertexPaintModifierContext::GetVertexColorChannelFaceIndexData(int **pValues,int &numColors)
{
	return GetIntList(pValues,numColors,TupVertexPaintModifierContext::VERTEX_COLOR_CHANNEL_FACE_INDICES);
}

TupperwareList *TupVertexPaintModifierContext::GetVertexColorChannelPointIndexData(int **pValues,int &numColors)
{
	return GetIntList(pValues,numColors,TupVertexPaintModifierContext::VERTEX_COLOR_CHANNEL_POINT_INDICES);
}

TupperwareList *TupVertexPaintModifierContext::GetIlluminationChannelColorData(float **pValues,int &numColors)
{
	TupperwareList *list = GetFloatList(pValues,numColors,TupVertexPaintModifierContext::ILLUMINATION_CHANNEL_COLORS);
	numColors/=3;
	return list;
}

TupperwareList *TupVertexPaintModifierContext::GetIlluminationChannelMixFactorData(float **pValues,int &numColors)
{
	return GetFloatList(pValues,numColors,TupVertexPaintModifierContext::ILLUMINATION_CHANNEL_MIX_FACTORS);
}

TupperwareList *TupVertexPaintModifierContext::GetIlluminationChannelFaceIndexData(int **pValues,int &numColors)
{
	return GetIntList(pValues,numColors,TupVertexPaintModifierContext::ILLUMINATION_CHANNEL_FACE_INDICES);
}

TupperwareList *TupVertexPaintModifierContext::GetIlluminationChannelPointIndexData(int **pValues,int &numColors)
{
	return GetIntList(pValues,numColors,TupVertexPaintModifierContext::ILLUMINATION_CHANNEL_POINT_INDICES);
}

TupperwareList *TupVertexPaintModifierContext::GetAlphaChannelColorData(float **pValues,int &numColors)
{
	return GetFloatList(pValues,numColors,TupVertexPaintModifierContext::ALPHA_CHANNEL_COLORS);
}

TupperwareList *TupVertexPaintModifierContext::GetAlphaChannelMixFactorData(float **pValues,int &numColors)
{
	return GetFloatList(pValues,numColors,TupVertexPaintModifierContext::ALPHA_CHANNEL_MIX_FACTORS);
}

TupperwareList *TupVertexPaintModifierContext::GetAlphaChannelFaceIndexData(int **pValues,int &numColors)
{
	return GetIntList(pValues,numColors,TupVertexPaintModifierContext::ALPHA_CHANNEL_FACE_INDICES);
}

TupperwareList *TupVertexPaintModifierContext::GetAlphaChannelPointIndexData(int **pValues,int &numColors)
{
	return GetIntList(pValues,numColors,TupVertexPaintModifierContext::ALPHA_CHANNEL_POINT_INDICES);
}




TupperwareList *TupVertexPaintModifierContext::AddFloatList(const float *pFloats,int numFloats,TupVertexPaintModifierContext::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListFloat(m_keyNames[index],NULL,pFloats,numFloats);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareList *TupVertexPaintModifierContext::AddIntList(const int *pInts,int numInts,TupVertexPaintModifierContext::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListInt(m_keyNames[index],NULL,pInts,numInts);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareList *TupVertexPaintModifierContext::GetFloatList(float **pFloats,int &numFloats,TupVertexPaintModifierContext::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_LIST)
	{
		TupperwareList *pList = (TupperwareList *)pAtom;
		if (pList->GetDataType()==TupperwareList::DATA_FLOAT)
		{
			numFloats = pList->GetLength();
			*pFloats = pList->GetAsFloat();
			return pList;
		}
	}
	// if we could not get the data then return a null pointer and init the count to 0
	*pFloats = NULL;
	numFloats = 0;
	return NULL;
}

TupperwareList *TupVertexPaintModifierContext::GetIntList(int **pInts,int &numInts,TupVertexPaintModifierContext::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_LIST)
	{
		TupperwareList *pList = (TupperwareList *)pAtom;
		if (pList->GetDataType()==TupperwareList::DATA_INT)
		{
			numInts = pList->GetLength();
			*pInts = pList->GetAsInt();
			return pList;
		}
	}
	// if we could not get the data then return a null pointer and init the count to 0
	*pInts = NULL;
	numInts = 0;
	return NULL;
}

