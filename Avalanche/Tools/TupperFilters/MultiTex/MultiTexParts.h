////////////////////////////////////////////////////////////////////////////
//
// MultiTexParts
//
// Class to handle parts used with multi texturing
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MultiTexParts.h $
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 10/02/02   Time: 11:27a
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * Changed UV transform classes
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 9/30/02    Time: 1:07p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * handle invert alpha
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 9/18/02    Time: 4:35p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * using build uv transform manager
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 9/03/02    Time: 2:06p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * start of hookup phase
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/28/02    Time: 4:38p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * Material Remap Skeleton & MultiTexMesh done
 */

////////////////////////////////////////////////////////////////////////////

#ifndef MULTI_TEX_PARTS
#define MULTI_TEX_PARTS

class ApplyAlphaEntry
{
public:
	float m_constantAlpha;
	UVTransform m_uvTransform;
};

class MaterialRefAlpha
{
public:
	int m_faceMaterialIndex;
	TupArray<ApplyAlphaEntry> m_alphaEntries;
};

class MaterialLayerEntry
{
public:
	int m_materialRef;
	TupArray<ApplyAlphaEntry> m_alphaEntries;
};

class MaterialRemapEntry
{
public:
	TupArray<MaterialLayerEntry> m_layerArray;
};


#endif