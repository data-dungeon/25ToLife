////////////////////////////////////////////////////////////////////////////
//
// TestMeshes
//
// Class to handle the testing of meshes to determine which instances can be broken up
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TestMeshes.h $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 4/10/03    Time: 4:01p
 * Updated in $/Avalanche/tools/TupperFilters/BreakLargeMeshes
 * handle cut checkbox on vis region
 * handle finding large vis region on instances
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:17p
 * Updated in $/Avalanche/tools/TupperFilters/BreakLargeMeshes
 * breakup meshes based on visibility regions as well as the grid
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/12/02    Time: 4:03p
 * Updated in $/Avalanche/tools/TupperFilters/BreakLargeMeshes
 * Break Up Meshes Complete (for now)
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/08/02    Time: 4:01p
 * Updated in $/Avalanche/tools/TupperFilters/BreakLargeMeshes
 * Test Meshes and Copy objects
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TEST_MESHES_H
#define TEST_MESHES_H

class BreakUpSettings;

class TestMeshesManager
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

	enum PassTypes
	{
		VISIBILITY_SET,
		GRID,
		TEXTURE_USAGE,
	};

	TestMeshesManager(TupNodePool &tupNodePool,
							TupObjectPool &tupObjectPool,
							TupMaterialPool &tupMaterialPool,
							TupMapPool &tupMapPool,
							TupImagePool &tupImagePool,
							TupModifierPool &tupModifierPool,
							int tupRootNodeRef,
							TupConfigRoot &tupConfigRoot,
							BreakUpSettings &settings);

	bool TestAndMarkNodes(int pass);

private:
	int TestAttributes(TupNode &tupNode,bool bVisibilitySet);
	bool TestModifiers(TupArray<int> &modifierReferenceArray,TupArray<TupperwareAggregate *> &modifierContextReferenceArray);
};

#endif
