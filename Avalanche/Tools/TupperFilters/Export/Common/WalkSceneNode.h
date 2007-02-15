////////////////////////////////////////////////////////////////////////////
//
// WalkSceneNode
//
// Class to handle walking through the scene tree
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: WalkSceneNode.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/12/02    Time: 5:11p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Instance, Scene Tree, etc
 */

#ifndef WALK_SCENE_NODE_H
#define WALK_SCENE_NODE_H

class CSceneNode;
struct ts_BoundingBox;

class WalkSceneNode
{
	static const float BOUNDING_NODE_EXPAND;

	static CSceneNode *m_pBestBoundingParent;
	static int m_nBestBoundingParentDepth;
	static ts_BoundingBox *m_pBestBoundingInstBox;
	static bool m_bBestBoundingExpand;
	static float m_fDivideTargetSize;

public:
	static void SwapEndianInPlace(CSceneNode &sceneNode);
	static void SetDivideTargetSize(float fTargetSize);
	static void InitBestBoundingParent(CSceneNode &pParent, ts_BoundingBox *pInstBox, bool bExpand);
	static CSceneNode *GetBestBoundingParent(void);
	static void WalkToDivideXZ(CSceneNode &sceneNode);
	static void WalkToFindBestBoundingParentXZ(CSceneNode &sceneNode,int nDepth);
	static void WalkToRemoveUnneededBoundingNodes(CSceneNode &sceneNode);
	static void WalkToResizeBoundingNodes(CSceneNode &sceneNode);
	static void WalkToExportData(CSceneNode &sceneNode);
	static void WalkToFixExportData(CSceneNode &sceneNode);
	static void WalkToFree(CSceneNode &sceneNode);

};

#endif