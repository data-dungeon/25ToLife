////////////////////////////////////////////////////////////////////////////
//
// BreakMeshes
//
// Class to handle the breaking up of meshes into smaller ones
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: BreakMeshes.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:11a
 * Updated in $/Avalanche/tools/TupperFilters/BreakLargeMeshes
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:17p
 * Updated in $/Avalanche/tools/TupperFilters/BreakLargeMeshes
 * breakup meshes based on visibility regions as well as the grid
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/12/02    Time: 4:03p
 * Updated in $/Avalanche/tools/TupperFilters/BreakLargeMeshes
 * Break Up Meshes Complete (for now)
 */

////////////////////////////////////////////////////////////////////////////

#ifndef BREAK_MESHES_H
#define BREAK_MESHES_H

class BreakUpSettings;

class BreakMeshesManager
{
	TupNodePool &m_tupNodePool;
	TupObjectPool &m_tupObjectPool;
	TupModifierPool &m_tupModifierPool;
	TupMaterialPool &m_tupMaterialPool;
	TupMapPool &m_tupMapPool;
	TupImagePool &m_tupImagePool;
	TupConfigRoot &m_tupConfigRoot;
	BreakUpSettings &m_settings;

	int m_tupRootNodeRef;

public:
	BreakMeshesManager(TupNodePool &tupNodePool,
						    TupObjectPool &tupObjectPool,
							 TupMaterialPool &tupMaterialPool,
							 TupMapPool &tupMapPool,
							 TupImagePool &tupImagePool,
							 TupModifierPool &tupModifierPool,
							 int tupRootNodeRef,
							 TupConfigRoot &tupConfigRoot,
							 BreakUpSettings &settings);

	void BreakUpMarkedNodesWithVisibilitySets(void);
	void BreakUpMarkedNodesOnGrid(void);
	void BreakUpMarkedNodesOnTextureUsage(void);

private:
	bool TestMesh(TupNode &tupNode,TupTriMesh &tupTriMesh);
};

#endif
