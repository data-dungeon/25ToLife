/**********************************************/
/*                                            */
/*   VisRegion.h                              */
/*   big juju  7/15/02                        */
/*   ned martin  avalanche software           */
/*   visual c++ 6.0                           */
/*   visibility regions for scene rendering   */
/*                                            */
/**********************************************/

/* a world may have 0 or more visibility regions defined. a visibility region is a bounding shape in world coordinates,
a pointer to the scene tree branch that represents the contents of the region, and a list of other regions visible from
this one. when the camera position that renders the scene is contained in a visibility region, that region and all the
regions in its visible list are enabled for rendering. visibility regions can overlap, so the camera can be in more than
one region at the same time. there is a also the concept of the "main world," which is all the world that is outside of
the possible visibility regions */

/******************* includes ****************************/

#include "Game/GamePCH.h"



/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

/******************* externs *****************************/

/******************* global variables ********************/

const s32 CVisRegion::PRELOADED_PARTITION = 0xFFFFFFFF;

/******************* local variables *********************/

/******************* local prototypes ********************/

/******************* functions ***************************/

/* fix-up after loading off disk-- resolve stuff */

bool CVisRegion::FixDiskLoad(CSceneNode *pTreeRoot, ts_RecordList *pVisRegionList)
{
	/* resolve pointer to branch in tree. it hangs directly off the root, and we just count siblings to get to it. */
	/* the scene can not have been modified at this point (that is, no instances added to it). since this is a */
	/* chunk loader, and the vis region chunk comes right after the scene tree chunk, should be no problem */

	CSceneNode *pChild = pTreeRoot->GetFirstChild();
	ASSERT(pChild);	// can't find branch
	u32 i;
	for (i = 0; i < m_u32SiblingIndex; i++)
	{
		pChild = pChild->GetNextSibling();
		ASSERT(pChild);	// can't find branch
	}
	m_pBranch = pChild;
	ASSERT(m_pBranch->GetType() == SCENE_NODE_ALWAYS_PASS);	// double check that we're ok-- first node of branch should always be this type

	/* resolve pointers to the regions visible from this region */

	for (i = 0; i < m_u32VisibleRegionCount; i++)
		m_visRegionLinks[i].m_pVisibleRegion = (CVisRegion *) GetRecordAddress(pVisRegionList, m_visRegionLinks[i].m_u32VisibleRegionIndex);

	/* build the obb that describes this region */
	
	ASSERT(!m_pOBB); // this should be NULL at load time
	m_pOBB = (OBB *) new OBBReference(&m_OBBOrientation, &m_OBBCenter, (AABB *) &m_OBBBoundingBox);

	/* done */

	return(true);
}

/* check if camera is in volume, enable stuff if so */

bool CVisRegion::CheckCamera(Vector3 &vPos)
{
	return(true);
}

/* draw the visibility region */

void CVisRegion::Draw(DrawUtility::COLOR Color)
{
	if (m_pOBB->Radius() < 10000.0f)	// don't draw rest-of-world obb
		m_pOBB->Draw(Color);
}
