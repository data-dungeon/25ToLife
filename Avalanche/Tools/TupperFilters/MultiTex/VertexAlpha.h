////////////////////////////////////////////////////////////////////////////
//
// VertexAlpha
//
// Class to handle vertex alpha issues with multi texturing
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: VertexAlpha.h $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:40a
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 8/28/02    Time: 4:38p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * Material Remap Skeleton & MultiTexMesh done
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/23/02    Time: 3:44p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * Vertex Alpha Manager
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/21/02    Time: 4:27p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * Vertex Alpha Start
 */

////////////////////////////////////////////////////////////////////////////

#ifndef VERTEX_ALPHA
#define VERTEX_ALPHA

#ifndef MULTI_TEX_PARTS
#include "MultiTexParts.h"
#endif

class VertexAlphaManager
{
	TupImagePool &m_tupImagePool;
	ImageManager &m_imageManager;
	TupTriMesh &m_tupTriMesh;

	int m_numFaces;
	int m_numFacePoints;
	int *m_pFaceMaterialIndices;
	TupArray<float> m_meshAlphaArray;

public:

	VertexAlphaManager(TupImagePool &tupImagePool,ImageManager &imageManager,TupTriMesh &tupTriMesh);

	void ApplyVertexAlpha(const TupArray<MaterialRefAlpha> &materialRefAlphaArray);
	void AddAlphaToMesh(void);

private:
};

#endif
