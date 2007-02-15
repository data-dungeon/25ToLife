////////////////////////////////////////////////////////////////////////////
//
// XRefHolder
//
// Class to handle keeping track of data for another scene (used by xrefs)
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: XrefHolder.h $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 7/03/02    Time: 10:31a
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * Check for database version.
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 5/24/02    Time: 4:24p
 * Updated in $/Avalanche/tools/TupperFilters/Annotate
 * Rearranged tupper3d usage on process tup class
 * Merge xref complete (does not do materials, maps, images)
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/23/02    Time: 4:15p
 * Updated in $/Avalanche/tools/TupperFilters/Annotate
 * Merge XRef Phase A
 */

////////////////////////////////////////////////////////////////////////////

#ifndef XREF_HOLDER_H
#define XREF_HOLDER_H

class RefRemap
{
public:
	enum RemapType
	{
		REMAP_NODE,
		REMAP_OBJECT,
		REMAP_MODIFIER,
		REMAP_MATERIAL,
		REMAP_MAP,
		REMAP_IMAGE
	};

	int m_nXRefReference;
	int m_nSceneReference;
	RemapType m_type;

	RefRemap(void);
	RefRemap(int nXRefReference,int nSceneReference,RemapType type);

};

class RemapManager
{
	TupArray<RefRemap> m_remapArray;
public:
	int Find(int nXRefReference,RefRemap::RemapType type) const;
	void Add(int nXRefReference,int nSceneReference,RefRemap::RemapType type);
};

class XRefHolder
{
	TupperwareAggregate *m_pSceneAgg;
	TupScene *m_pTupScene;
	TupNodePool *m_pTupNodePool;
	TupObjectPool *m_pTupObjectPool;
	TupModifierPool *m_pTupModifierPool;
	TupMaterialPool *m_pTupMaterialPool;
	TupMapPool *m_pTupMapPool;
	TupImagePool *m_pTupImagePool;

	RemapManager *m_pRemapManager;

	XRefHolder(void);
public:
	XRefHolder(TupperwareAggregate *pSceneAgg,const char *pFilename);
	~XRefHolder();

	TupperwareAggregate *GetSceneAgg(void);
	TupScene *GetScene(void);
	TupNodePool *GetNodePool(void);
	TupObjectPool *GetObjectPool(void);
	TupModifierPool *GetModifierPool(void);
	TupMaterialPool *GetMaterialPool(void);
	TupMapPool *GetMapPool(void);
	TupImagePool *GetImagePool(void);

	RemapManager *GetRemapManager(void);
};

#endif
