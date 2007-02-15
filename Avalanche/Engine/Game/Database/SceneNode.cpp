/**********************************************/
/*                                            */
/* SceneNode.cpp                              */
/* big juju prototype  1/02/02                */
/* ned martin  avalanche software             */
/* visual c++ 6.0                             */
/* nodes for scene graph tree                 */
/*                                            */
/**********************************************/

/* the SceneNode class is basic component of a scene tree, which typically represents a world. the sene tree is traversed
by the scene manager to render the world */

/******************* includes ****************************/

#include "Game/GamePCH.h"

#include "EngineHelper/filestream.h"



#ifdef TOOLS
/* tool includes */

#include "DBLUtil.h"
#include "DBLWrite.h"
#include "DBLSwap.h"
#include "instanceinfo.h"
#include "../common/pluginerror.h"	// error codes
#endif //TOOLS



/******************* defines *****************************/

/* define a value that means null pointer in disk image, since 0 is a valid value to be fixed up */

#define SCENE_NODE_DISK_NULL		(CSceneNode *) 0xffffffff

/* define percent that we can expand a node bounding box to accept an instance */

#ifdef TOOLS	// ImportExport only
#define BOUNDING_NODE_EXPAND		(20.0f / 100.0f)
#endif //TOOLS

/******************* macros ******************************/

/******************* structures **************************/

/******************* externs *****************************/

/******************* global variables ********************/

/******************* local variables *********************/

/* a pointer to a frustum to by used by all nodes when walking a tree. used for handling bounding box nodes */

static ts_Frustum *pSceneNodeViewFrustum;
static ts_Frustum *pSceneNodeGuardBandFrustum;

/* data to use when walking a tree to fix up load from disk */

static void *pSceneNodeChunkStart;
static ts_RecordList *pSceneNodeInstList;	// record list from previously-loaded instance chunk

/* callbacks for WalkWithCallbacks */

t_SceneNodeCallback SceneNodeCallback[SCENE_NODE_TYPES] = { NULL };

/* for use when building scene tree */

#ifdef TOOLS	// ImportExport only
static CSceneNode *pBestBoundingParent;
static int nBestBoundingParentDepth;
static ts_BoundingBox *pBestBoundingInstBox;
static bool bBestBoundingExpand;
static float fDivideTargetSize;
#endif //TOOLS

/******************* local prototypes ********************/

/******************* functions ***************************/

/* clear it out */

void CSceneNode::Init()
{
	memset(this, 0, sizeof(CSceneNode));
}

/* add a child to current node */ 

void CSceneNode::AddChild(CSceneNode *pChildNode)
{
	ASSERT( pChildNode->m_pParent      == NULL );
	ASSERT( pChildNode->m_pNextSibling == NULL );

	// add child to head of list
	pChildNode->m_pNextSibling = m_pFirstChild;
	m_pFirstChild = pChildNode;
	pChildNode->SetParent( this );
}

/* remove node from tree. this use to be done with a call to SLinkList__RemoveLink, but i want save space in the CSceneNode
record (by using a pointer instead of a ts_SLinkList record), so do it by hand */

void CSceneNode::Remove()
{
	ASSERT_PTR( m_pParent );

	// find ptr that points to us
	CSceneNode** link = &m_pParent->m_pFirstChild;
	for ( CSceneNode* n = *link ; n && n != this ; n = *link )
		link = &n->m_pNextSibling;

	// we better be pointing to node to remove
	ASSERT( *link == this );

	// relink
	*link = m_pNextSibling;

	// clean us up
	m_pParent = NULL;
	m_pNextSibling = NULL;
}

/* set the view frustum to use when checking bounding box nodes while walking a tree */

void CSceneNode::SetViewFrustum(ts_Frustum *pFrustum)
{
	pSceneNodeViewFrustum = pFrustum;	// not a member of class, but a static variable used by all nodes
}
ts_Frustum *CSceneNode::GetViewFrustum()
{
	return(pSceneNodeViewFrustum);
}
void CSceneNode::SetGuardBandFrustum(ts_Frustum *pFrustum)
{
	pSceneNodeGuardBandFrustum = pFrustum;	// not a member of class, but a static variable used by all nodes
}
ts_Frustum *CSceneNode::GetGuardBandFrustum()
{
	return(pSceneNodeGuardBandFrustum);
}

/* search the children of the passed node for the passed data pointer (not recursive) */

CSceneNode *CSceneNode::SearchChildren(void *pData)
{
	// search 1st generation children for match
	for ( CSceneNode* n = m_pFirstChild ; n ; n = n->m_pNextSibling )
		if ( n->m_pData == pData )
			return n;

	// not found
	return NULL;
}

/* search the tree from the passed node down for the passed data pointer (recursive) */

CSceneNode *CSceneNode::SearchTree(void *pData)
{
	// search children, and children's children, for match
	for ( CSceneNode* n = m_pFirstChild ; n ; n = n->m_pNextSibling )
	{
		if ( n->m_pData == pData )
			return n;

		// check recursive
		static CSceneNode* found;
		found = n->SearchTree( pData );
		if ( found )
			return found;
	}

	// not found
	return NULL;
}

CSceneNode *CSceneNode::FindNodeByInstanceName(const char* name)
{
	if ( GetType() == SCENE_NODE_INSTANCE )
	{
		CInstance* instance = (CInstance*)GetData();
		if ( instance && strcmp( instance->Name(), name) == 0 )
		{
			return this;
		}
	}

	for ( CSceneNode* n = m_pFirstChild; n; n = n->m_pNextSibling )
	{
		CSceneNode* node = n->FindNodeByInstanceName( name );
		if ( node )
			return node;
	}

	return NULL;
}

/* set data needed for fixing up records loaded from disk */

void CSceneNode::SetDiskLoadData(void *pChunkStart, ts_RecordList *pInstList)
{
	/* variables not members of class, but a static variable used by all nodes */

	pSceneNodeChunkStart = pChunkStart;	// for fixing up node pointers
	pSceneNodeInstList = pInstList;		// for getting instance pointers
}

/* count node's children */

int CSceneNode::ChildCount()
{
	int count = 0;
	for ( CSceneNode* n = m_pFirstChild ; n ; n = n->m_pNextSibling )
		count++;
	return count;
}

/* how deep in tree is this node? */

int CSceneNode::GetDepth()
{
	int depth = 0;
	for ( CSceneNode* n = m_pParent ; n ; n = n->m_pParent )
		depth++;
	return depth;
}

/* clear all callbacks used by WalkWithCallbacks */

void CSceneNode::ClearCallbacks()
{
	for (int i = 0; i < SCENE_NODE_TYPES; i++)
		SceneNodeCallback[i] = NULL;
}

/* register a callback for use with WalkWithCallbacks */
/*** static functions ***/

void CSceneNode::RegisterCallback(int nType, t_SceneNodeCallback Callback)
{
	ASSERT((nType >= 0) && (nType < SCENE_NODE_TYPES));
	SceneNodeCallback[nType] = Callback;
}
void CSceneNode::RegisterCallbackAllTypes(t_SceneNodeCallback Callback)
{
	for (int i = 0; i < SCENE_NODE_TYPES; i++)
		SceneNodeCallback[i] = Callback;
}

/************************************** ImportExport *************************************/

#ifdef TOOLS

/* swap for big endian machines */

void CSceneNode::SwapEndianInPlace()
{
	DBLUtil__SwapEndianInPlace((u32 *) &m_pData);
	DBLUtil__SwapEndianInPlace((u32 *) &m_pFirstChild);
	DBLUtil__SwapEndianInPlace((u32 *) &m_pNextSibling);
	DBLUtil__SwapEndianInPlace((u32 *) &m_pParent);
	DBLUtil__SwapEndianInPlace((u32 *) &m_pNextRender);
	DBLUtil__SwapEndianInPlace(&m_u32Flags);
}

/* set target bounding box size for WalkToDivideXZ or WalkToDivideXYZ */

void CSceneNode::SetDivideTargetSize(float fTargetSize)
{
	fDivideTargetSize = fTargetSize;	// in meters
}

/* find bounding node that best fits passed bounding box */

void CSceneNode::InitBestBoundingParent(CSceneNode *pParent, ts_BoundingBox *pInstBox, bool bExpand)
{
	pBestBoundingParent = pParent;						// not a member of class, but a static variable used by all nodes
	nBestBoundingParentDepth = pParent->GetDepth();	// not a member of class, but a static variable used by all nodes
	pBestBoundingInstBox = pInstBox;						// not a member of class, but a static variable used by all nodes
	bBestBoundingExpand = bExpand;						// not a member of class, but a static variable used by all nodes
}

CSceneNode *CSceneNode::GetBestBoundingParent()
{
	return(pBestBoundingParent);	// not a member of class, but a static variable used by all nodes
}

#endif //TOOLS



/************************************** tree walks ***************************************/

/* this is a generic tree walk with callbacks for nodes. typically, i prefer to code up the specific walks, because
they are easier to read (that is, you can tell what the walk routine is doing by looking at it, instead of having to
look at the callbacks), but some things are so butt-simple that a generic walk is fine */

void CSceneNode::WalkWithCallbacks()
{
	if ( SceneNodeCallback[ GetType() ] )
		SceneNodeCallback[ GetType() ]( this );

	// process all the children
	static CSceneNode*	n;
	CSceneNode*				next;
	for ( n = m_pFirstChild ; n ; n = next )
	{
		next = n->m_pNextSibling;
		n->WalkWithCallbacks();
	}
}

// sometimes it's better to process the children before the current node 
void CSceneNode::WalkWithCallbacksChildrenFirst()
{
	// process all the children
	static CSceneNode*	n;
	CSceneNode*				next;
	for ( n = m_pFirstChild ; n ; n = next )
	{
		next = n->m_pNextSibling;
		n->WalkWithCallbacksChildrenFirst();
	}

	if ( SceneNodeCallback[ GetType() ] )
		SceneNodeCallback[ GetType() ]( this );
}

void CSceneNode::WalkToSetFlag(u32 flag)
{
	SetFlags(GetFlags() | flag);
	for ( CSceneNode* n = m_pFirstChild ; n ; n = n->m_pNextSibling )
	{
		n->WalkToSetFlag(flag);
	}
}

void CSceneNode::WalkToClearFlag(u32 flag)
{
	SetFlags(GetFlags() & ~flag);
	for ( CSceneNode* n = m_pFirstChild ; n ; n = n->m_pNextSibling )
	{
		n->WalkToClearFlag(flag);
	}
}

#ifndef TOOLS

/************************************** engine *******************************************/

/* walk the tree for rendering a scene. this checks bounding box nodes against the registered view frustum (via
CSceneNode.RegisterFrustum), and calls back to the scene manager for instance nodes. this routine is recursive */

void CSceneNode::WalkToRenderScene()
{
	// static to save stack space
	static bool processChildren;
	processChildren = false;

	if ( IsEnabled() )
	{
		switch ( GetType() )
		{
			case SCENE_NODE_INSTANCE:
				SceneMgr::FoundInstanceNode( this );
				break;

			case SCENE_NODE_BOUNDING_BOX:
				processChildren = Frustum__IntersectsWorldBox( (ts_BoundingBox*) m_pData, (m_u32Flags & SCENE_NODE_NO_FAR_CLIP) != 0 );
				break;

			case SCENE_NODE_ALWAYS_PASS:
				processChildren = true;
				break;

			default:
				ASSERT( false );
				break;
		}

		if ( processChildren )
		{
			for ( CSceneNode* n = m_pFirstChild ; n ; n = n->m_pNextSibling )
				n->WalkToRenderScene();
		}
		else
		{
			for ( CSceneNode* n = m_pFirstChild ; n ; n = n->m_pNextSibling )
				n->WalkToRenderSceneBoxFrustumCulled();
		}
	}
	else
	{
		for ( CSceneNode* n = m_pFirstChild ; n ; n = n->m_pNextSibling )
			n->WalkToRenderVisRegionOff();
	}
}

// the whole purpose of this routine is to remove display lists that
// aren't visible because they're culled by the vis region.
void CSceneNode::WalkToRenderSceneBoxFrustumCulled()
{
	if ( GetType() == SCENE_NODE_INSTANCE )
		SceneMgr::FoundInstanceNodeSceneBoxFrustumCulled( this );

	for ( CSceneNode* n = m_pFirstChild ; n ; n = n->m_pNextSibling )
		n->WalkToRenderSceneBoxFrustumCulled();
}

void CSceneNode::WalkToRenderVisRegionOff()
{
	if ( GetType() == SCENE_NODE_INSTANCE )
		SceneMgr::FoundInstanceNodeVisRegionOff( this );

	for ( CSceneNode* n = m_pFirstChild ; n ; n = n->m_pNextSibling )
		n->WalkToRenderVisRegionOff();
	
}

#ifndef CONSUMER_BUILD
/* walk the tree to visualize bounding boxes */

void CSceneNode::WalkToVisualizeBounds()
{
	/* if node is disabled, we don't process it or its children */

	if (IsEnabled())
	{
		/* assume we will process children */

		static bool bProcessChildren;	// static so it won't take up stack space during recursion
		bProcessChildren = TRUE;

		/* what kind of node? */

		switch(GetType())
		{
		case SCENE_NODE_INSTANCE:
			BoundingBox__Draw(&((CInstance *) m_pData)->BoundingBox());
			break;
		case SCENE_NODE_BOUNDING_BOX:
			/* if bounding box is not in view, don't process children */

			bProcessChildren = Frustum__IntersectsWorldBox((ts_BoundingBox *) m_pData, (m_u32Flags & SCENE_NODE_NO_FAR_CLIP) != 0);

			/* maybe visualize bounding boxes */

			if (bProcessChildren)
				BoundingBox__Draw((ts_BoundingBox *) m_pData);
			break;
		case SCENE_NODE_ALWAYS_PASS:
			break;
		default:
			ASSERT(FALSE);	// unknown node type
		}

		/* process children (recursive) */

		if (bProcessChildren)
		{
			CSceneNode *pChild = m_pFirstChild;
			while (pChild)
			{
				pChild->WalkToVisualizeBounds();
				pChild = pChild->m_pNextSibling;
			}
		}
	}
}
#endif //CONSUMER_BUILD

/* walk the tree to clear all instance display lists */

void CSceneNode::WalkToResetDisplayLists()
{
	/* what kind of node? */

	switch(GetType())
	{
	case SCENE_NODE_INSTANCE:
		((CInstance*)m_pData)->RemoveDisplayList();
		break;
	case SCENE_NODE_BOUNDING_BOX:
	case SCENE_NODE_ALWAYS_PASS:
		break;
	default:
		ASSERT(FALSE);	// unknown node type
	}

	/* process children (recursive) */

	CSceneNode *pChild = m_pFirstChild;
	while (pChild)
	{
		pChild->WalkToResetDisplayLists();
		pChild = pChild->m_pNextSibling;
	}
}

/* recursively fix up the scene nodes after loading from disk */

void CSceneNode::WalkToFixDiskLoad()
{
	/* fix up pointers. a valid pointer contains offset from chunk data start. a null pointer contains */
	/* SCENE_NODE_DISK_NULL */

	if (m_pParent == SCENE_NODE_DISK_NULL)
		m_pParent = NULL;
	else
		m_pParent = (CSceneNode *) ((u32) pSceneNodeChunkStart + (u32) m_pParent);
	if (m_pFirstChild == SCENE_NODE_DISK_NULL)
		m_pFirstChild = NULL;
	else
		m_pFirstChild = (CSceneNode *) ((u32) pSceneNodeChunkStart + (u32) m_pFirstChild);
	if (m_pNextSibling == SCENE_NODE_DISK_NULL)
		m_pNextSibling = NULL;
	else
		m_pNextSibling = (CSceneNode *) ((u32) pSceneNodeChunkStart + (u32) m_pNextSibling);

	/* data pointer fix up depends on node type */

	switch(GetType())
	{
	case SCENE_NODE_INSTANCE:
		/* m_pData holds instance index */

		if (pSceneNodeInstList)
		{
			m_pData = GetRecordAddress(pSceneNodeInstList, (u32) m_pData);

			/* temporary, until this is in the filter chain. if this instance is marked as NO_FAR_CLIP, */
			/* set flag back up the parent chain */

			CInstance *pInst = (CInstance *) m_pData;
			if ( pInst->FarClipDisabled() )
			{
				/* mark this node and go back up the tree */

				CSceneNode *pParent = this;
				while (pParent)
				{
					pParent->m_u32Flags |= SCENE_NODE_NO_FAR_CLIP;
					pParent = pParent->m_pParent;
				}
			}
		}
		else
			m_pData = NULL;
		break;
	case SCENE_NODE_ALWAYS_PASS:
		/* m_pData should always be null */

		ASSERT(m_pData == NULL);
		break;
	case SCENE_NODE_BOUNDING_BOX:
		/* bounding box record immediately follows node record */

		m_pData = (void *) ((u32) this + sizeof(CSceneNode));
		break;
	default:
		ASSERT(FALSE);	// unknown node type
	}

	/* process children (recursive) */

	CSceneNode *pChild = m_pFirstChild;
	while (pChild)
	{
		pChild->WalkToFixDiskLoad();
		pChild = pChild->m_pNextSibling;
	}
}

/* find an instance in the tree. by no means is the instance found the closest to the top of the tree, it's just
the one that happens to be found first */

CInstance *CSceneNode::WalkToFindFirstInstance()
{
	/* what kind of node? */

	switch(GetType())
	{
	case SCENE_NODE_INSTANCE:
		return((CInstance *) m_pData);
		break;
	case SCENE_NODE_BOUNDING_BOX:
	case SCENE_NODE_ALWAYS_PASS:
		break;
	default:
		ASSERT(FALSE);	// unknown node type
	}

	/* process children (recursive) */

	CSceneNode *pChild = m_pFirstChild;
	while (pChild)
	{
		static CInstance *pInst;	// ok to be static, because as soon as it's found we will exit all the way out
		pInst = pChild->WalkToFindFirstInstance();
		if (pInst)
			return(pInst);
		pChild = pChild->m_pNextSibling;
	}

	/* no instance found */

	return(NULL);
}

#ifndef CDROM
/* see if passed instance is in scene tree-- slow, for development only */

bool CSceneNode::WalkToFindInstance(CInstance *pInst)
{
	/* what kind of node? */

	switch(GetType())
	{
	case SCENE_NODE_INSTANCE:
		if (pInst == m_pData)
			return(true);
		break;
	case SCENE_NODE_BOUNDING_BOX:
	case SCENE_NODE_ALWAYS_PASS:
		break;
	default:
		ASSERT(FALSE);	// unknown node type
	}

	/* process children (recursive) */

	CSceneNode *pChild = m_pFirstChild;
	while (pChild)
	{
		if (pChild->WalkToFindInstance(pInst))
			return(true);
		pChild = pChild->m_pNextSibling;
	}

	/* not found down this branch */

	return(false);
}

/* see if passed vertex color buffer is used by an instance in the scene tree */

bool CSceneNode::WalkToFindVertexColors(void *pVertexColors)
{
	/* what kind of node? */

	switch(GetType())
	{
	case SCENE_NODE_INSTANCE:
		if ( pVertexColors == ((CInstance*) m_pData)->VertexColorPtr() )
			return(true);
		break;
	case SCENE_NODE_BOUNDING_BOX:
	case SCENE_NODE_ALWAYS_PASS:
		break;
	default:
		ASSERT(FALSE);	// unknown node type
	}

	/* process children (recursive) */

	CSceneNode *pChild = m_pFirstChild;
	while (pChild)
	{
		if (pChild->WalkToFindVertexColors(pVertexColors))
			return(true);
		pChild = pChild->m_pNextSibling;
	}

	/* not found down this branch */

	return(false);
}

/* walk to print all instances out to a file */

int CSceneNode::WalkToReportInstances(FileStream *pLogFile, ts_PatchSOM *pCheckSOM /*= NULL*/, int nCount /*= 0*/)
{
	/* what kind of node? */

	switch(GetType())
	{
	case SCENE_NODE_INSTANCE:
		if (pCheckSOM)
		{
			CInstance* inst = (CInstance*) m_pData;
			if ( inst->Model() == pCheckSOM )
			{
				pLogFile->Print( "        Used by instance %s\n", inst->Name() );
				nCount++;
			}
		}
		else
		{
			pLogFile->Print("   %s\n", ((CInstance *) m_pData)->Name() );
			nCount++;
		}
		break;
	case SCENE_NODE_BOUNDING_BOX:
	case SCENE_NODE_ALWAYS_PASS:
		break;
	default:
		ASSERT(FALSE);	// unknown node type
	}

	/* process children (recursive) */

	CSceneNode *pChild = m_pFirstChild;
	while (pChild)
	{
		nCount = pChild->WalkToReportInstances(pLogFile, pCheckSOM, nCount);
		pChild = pChild->m_pNextSibling;
	}

	/* return number of instance reported */

	return(nCount);
}
#endif //CDROM

#else //TOOLS

/************************************** ImportExport *************************************/

/* divide a bounding box into 2 or 4 child bounding boxes if it is larger than the target size */

void CSceneNode::WalkToDivideXZ()
{
	/* ignore non-bounding-box nodes */

	if (GetType() != SCENE_NODE_BOUNDING_BOX)
	{
		CSceneNode *pChild = m_pFirstChild;
		while (pChild)
		{
			pChild->WalkToDivideXZ();	// go recursive
			pChild = pChild->GetNextSibling();
		}
		return;
	}

	/* get size of prospective child bounding box nodes */

	ts_BoundingBox *pBox = (ts_BoundingBox *) m_pData;
	float fXChildSize = (pBox->Max.x() - pBox->Min.x()) / 2.0f;
	float fZChildSize = (pBox->Max.z() - pBox->Min.z()) / 2.0f;
	if ((fXChildSize < fDivideTargetSize) && (fZChildSize < fDivideTargetSize))
		return;	// done-- no more children

	/* how to divide? may want to divide in both x and y, or in x only, or in z only */

	CSceneNode *pChild;
	ts_BoundingBox *pChildBox;

	/* first child is from min x to center x, min z to center z. if current x is already < target size, */
	/* don't divide in x (that is, only divide in z). same for z. dividing into 4 will use all 4 children. */
	/* dividing only in x will use children 1 and 2. dividing only in z will use children 1 and 3 */

	pChildBox = (ts_BoundingBox *) malloc(sizeof(ts_BoundingBox));
	*pChildBox = *pBox;
	if (fXChildSize >= fDivideTargetSize)	// may not need to divide in x
		pChildBox->Max.x(pChildBox->Min.x() + fXChildSize);	// max x = center x
	if (fZChildSize >= fDivideTargetSize)	// may not need to divide in z
		pChildBox->Max.z(pChildBox->Min.z() + fZChildSize);	// max z = center z
	pChild = new CSceneNode;
	pChild->Init();
	pChild->SetType(SCENE_NODE_BOUNDING_BOX);
	pChild->SetData(pChildBox);
	AddChild(pChild);
	pChild->WalkToDivideXZ();	// recursively divide some more

	/* 2nd child node-- center x to max x, min z to center z. only need if dividing in x */

	if (fXChildSize >= fDivideTargetSize)
	{
		pChildBox = (ts_BoundingBox *) malloc(sizeof(ts_BoundingBox));
		*pChildBox = *pBox;
		pChildBox->Min.x(pChildBox->Min.x() + fXChildSize);							// min x = center x
		if (fZChildSize >= fDivideTargetSize)	// may not need to divide in z
			pChildBox->Max.z(pChildBox->Min.z() + fZChildSize);	// max z = center z
		pChild = new CSceneNode;
		pChild->Init();
		pChild->SetType(SCENE_NODE_BOUNDING_BOX);
		pChild->SetData(pChildBox);
		AddChild(pChild);
		pChild->WalkToDivideXZ();	// recursively divide some more
	}

	/* 3rd child node-- min x to center x, center z to max z. only need if dividing in z */

	if (fZChildSize >= fDivideTargetSize)
	{
		pChildBox = (ts_BoundingBox *) malloc(sizeof(ts_BoundingBox));
		*pChildBox = *pBox;
		if (fXChildSize >= fDivideTargetSize)	// may not need to divide in x
			pChildBox->Max.x(pChildBox->Min.x() + fXChildSize);	// max x = center x
		pChildBox->Min.z(pChildBox->Min.z() + fZChildSize);							// min z = center z
		pChild = new CSceneNode;
		pChild->Init();
		pChild->SetType(SCENE_NODE_BOUNDING_BOX);
		pChild->SetData(pChildBox);
		AddChild(pChild);
		pChild->WalkToDivideXZ();	// recursively divide some more
	}

	/* 4th child node-- center x to max x, center z to max z. only need if dividing into 4 sub-nodes */

	if ((fXChildSize >= fDivideTargetSize) && (fZChildSize >= fDivideTargetSize))
	{
		pChildBox = (ts_BoundingBox *) malloc(sizeof(ts_BoundingBox));
		*pChildBox = *pBox;
		pChildBox->Min.x(pChildBox->Min.x() + fXChildSize);							// min x = center x
		pChildBox->Min.z(pChildBox->Min.z() + fZChildSize);							// min z = center z
		pChild = new CSceneNode;
		pChild->Init();
		pChild->SetType(SCENE_NODE_BOUNDING_BOX);
		pChild->SetData(pChildBox);
		AddChild(pChild);
		pChild->WalkToDivideXZ();	// recursively divide some more
	}
}

/* find the smallest bounding box that contains instance. call InitBestBoundingParent first, call GetBestBoundingParent
to retrieve results */

void CSceneNode::WalkToFindBestBoundingParentXZ(int nDepth)
{
	/* instance nodes are not under consideration */

	if (GetType() == SCENE_NODE_INSTANCE)
	{
		ASSERT(m_pFirstChild == NULL);	// instances shouldn't have any child nodes
		return;
	}

	/* this routine is originally passed in the tree root, not a bounding box */

	if (GetType() == SCENE_NODE_BOUNDING_BOX)
	{
		/* we allow the bounding node boxes to expand a bit to include instances. this is because when large models */
		/* are broken up along grid bounds, we don't cut polys-- they extend out of the grid bounds. but the broken */
		/* up pieces should fit in a grid-sized bounding node */

		static ts_BoundingBox NodeBox;	// static so it won't take up stack space during recursion
		NodeBox = *((ts_BoundingBox *) m_pData);
		if (bBestBoundingExpand)
		{
			static float fXSize;				// static so it won't take up stack space during recursion
			static float fZSize;				// static so it won't take up stack space during recursion
			fXSize = NodeBox.Max.x() - NodeBox.Min.x();
			fZSize = NodeBox.Max.z() - NodeBox.Min.z();
			NodeBox.Min.x(NodeBox.Min.x() - (fXSize * BOUNDING_NODE_EXPAND));
			NodeBox.Min.z(NodeBox.Min.z() - (fZSize * BOUNDING_NODE_EXPAND));
			NodeBox.Max.x(NodeBox.Max.x() + (fXSize * BOUNDING_NODE_EXPAND));
			NodeBox.Max.z(NodeBox.Max.z() + (fZSize * BOUNDING_NODE_EXPAND));
		}

		/* does instance fit? */

		if (BoundingBox__ContainsPointXZ(&NodeBox, pBestBoundingInstBox->Min) == FALSE)
			return;	// doesn't fit, no point in recursing through children
		if (BoundingBox__ContainsPointXZ(&NodeBox, pBestBoundingInstBox->Max) == FALSE)
			return;	// doesn't fit, no point in recursing through children
	}

	/* instance fits. is this parent better than current best? */

	if (nDepth > nBestBoundingParentDepth)
	{
		ASSERT(nDepth == GetDepth());
		pBestBoundingParent = this;
		nBestBoundingParentDepth = nDepth;
	}

	/* go recursive to keep looking */

	CSceneNode *pChild = m_pFirstChild;
	while (pChild)
	{
		pChild->WalkToFindBestBoundingParentXZ(nDepth + 1);
		pChild = pChild->m_pNextSibling;
	}
}

/* remove bounding nodes that have 0 or 1 children. if they have 0 children, obviously they're not needed. if they have
one child, it may be an instance or a bounding box. in the case of the instance, the bounding node's box will just match
the instance's bounding box, so it's redundant. in the case of a single bounding box node, it's clear that the parent
is redundant. for always-pass nodes, remove if they have 0 children-- they are empty */

void CSceneNode::WalkToRemoveUnneededNodes()
{
	/* first process children, so that we have valid status when we look at current node */

	CSceneNode *pChild;
	CSceneNode *pNextChild = m_pFirstChild;
	while (pNextChild)
	{
		pChild = pNextChild;
		pNextChild = pNextChild->m_pNextSibling;	// child may get removed, so get next pointer before processing
		pChild->WalkToRemoveUnneededNodes();
	}

	/* now look at this node */

	if (GetType() == SCENE_NODE_BOUNDING_BOX)
	{
		static int nChildCount;	// static so it won't take up stack space during recursion
		nChildCount = ChildCount();

		/* 1 child? remove this node and move child up */

		if (nChildCount == 1)
		{
			pChild = m_pFirstChild;
			pChild->Remove();
			m_pParent->AddChild(pChild);
		}

		/* 0 or 1 children? delete */

		if (nChildCount <= 1)
		{
			Remove();

			/* free memory */

			free(m_pData);
			delete this;
		}
	}
	else if (GetType() == SCENE_NODE_ALWAYS_PASS) 
	{
		/* don't remove tree root */

		if ((ChildCount() == 0) && (m_pParent != NULL))
		{
			Remove();
			ASSERT(m_pData == NULL);
			delete this;
		}
	}
}

/* resize the bounding nodes so that they contain all their children */

void CSceneNode::WalkToResizeBoundingNodes()
{
	/* first process children, so that we have valid status when we look at current node */

	CSceneNode *pChild = m_pFirstChild;
	while (pChild)
	{
		pChild->WalkToResizeBoundingNodes();
		pChild = pChild->m_pNextSibling;
	}

	/* now look at this node */

	if (GetType() == SCENE_NODE_BOUNDING_BOX)
	{
		/* first clear it to get best results-- contents may be smaller than original quad tree */

		static ts_BoundingBox *pBox;	// static so it won't take up stack space during recursion
		pBox = (ts_BoundingBox *) m_pData;
		BoundingBox__Clear(pBox);

		/* and include children */

		pChild = m_pFirstChild;
		while (pChild)
		{
			switch(pChild->GetType())
			{
			case SCENE_NODE_INSTANCE:
				static CInstanceInfo *pInstanceInfo;	// static so it won't take up stack space during recursion
				pInstanceInfo = (CInstanceInfo *) pChild->m_pData;
				BoundingBox__Include(pBox, pInstanceInfo->m_BoundingBox.Min);
				BoundingBox__Include(pBox, pInstanceInfo->m_BoundingBox.Max);
				break;
			case SCENE_NODE_ALWAYS_PASS:
				break;
			case SCENE_NODE_BOUNDING_BOX:
				static ts_BoundingBox *pChildBox;	// static so it won't take up stack space during recursion
				pChildBox = (ts_BoundingBox *) pChild->m_pData;
				BoundingBox__Include(pBox, pChildBox->Min);
				BoundingBox__Include(pBox, pChildBox->Max);
				break;
			default:
				ASSERT(FALSE);	// unknown node type
			}
			pChild = pChild->m_pNextSibling;
		}

		/* should never end up empty-- means an error in the tree */

		ASSERT(BoundingBox__IsEmpty(pBox) == FALSE);
	}
}

/* walk the tree and write the nodes out to the .dbl file -- ImportExport only. this is kind of a hack, */
/* but this is a two stage process-- walk the tree once to write the records out so that we know where */
/* they end up in the file. then walk again to set the file offset relationships between the nodes. for */
/* example, since a node may have children, it's hard to predict where they will end up in the file. by */
/* writing all the nodes out and recording where each one is in the file, i can go back and fix up offsets */
/* in the second pass (WalkToFixExportData). the only real hack part of this is that i use the m_pNextRender */
/* field to record the file position during the first pass, since i know that is a run-time field only */

int CSceneNode::WalkToExportData()
{
	/* record file offset in m_pNextRender field. m_pNextRender is a run-time only field, it does not */
	/* contain any data in the database. this is used by WalkToFixExportData */

	m_pNextRender = (CSceneNode *) (ftell(pDBLFile) - lChunkDataStart);	// offset from start of chunk

	/* what kind of node? this must match corresponding section in WalkToFixExportData */

	switch(GetType())
	{
	case SCENE_NODE_INSTANCE:
	case SCENE_NODE_ALWAYS_PASS:
		if (DBLUtil__WriteRecord(this, pDBLFile) == FALSE)
			HANDLE_ERROR(RETURN_SCENE_NODE_WRITE_FAILED);
		break;
	case SCENE_NODE_BOUNDING_BOX:
		/* for bounding box node, write out node and then immediately follow with bounding box record */

		if (DBLUtil__WriteRecord(this, pDBLFile) == FALSE)
			HANDLE_ERROR(RETURN_SCENE_NODE_WRITE_FAILED);
		if (DBLUtil__WriteRecord((ts_BoundingBox *) m_pData, pDBLFile) == FALSE)
			HANDLE_ERROR(RETURN_SCENE_NODE_WRITE_FAILED);
		break;
	default:
		ASSERT(FALSE);	// unknown node type
	}

	/* process children (recursive) */

	CSceneNode *pChild = m_pFirstChild;
	while (pChild)
	{
		int iReturnCode;
		if ((iReturnCode = pChild->WalkToExportData()) != RETURN_SUCCESS)
			return(iReturnCode);
		pChild = pChild->m_pNextSibling;
	}

	/* success */

	return(RETURN_SUCCESS);
}

/* this is the second pass of the two-pass data export process. when this is called, all the nodes in the */
/* scene tree have their file position stored in the m_pNextRender field (as a u32), so i can calculate */
/* the file offsets necessary to link them back together at run-time */

int CSceneNode::WalkToFixExportData()
{
	/* copy contents into a node i can edit, then set the pointers to offsets. each node's m_pNextRender */
	/* field contains offset from the start of the chunk data. 0 is a valid value (it is the file offset */
	/* of the first node in the tree), so flag NULL pointers with another value */

	static CSceneNode CopyNode;	// static so it won't take up stack space during recursion
	CopyNode = *this;
	if (m_pParent)
		CopyNode.m_pParent = m_pParent->m_pNextRender;
	else
		CopyNode.m_pParent = SCENE_NODE_DISK_NULL;
	if (m_pFirstChild)
		CopyNode.m_pFirstChild = m_pFirstChild->m_pNextRender;
	else
		CopyNode.m_pFirstChild = SCENE_NODE_DISK_NULL;
	if (m_pNextSibling)
		CopyNode.m_pNextSibling = m_pNextSibling->m_pNextRender;
	else
		CopyNode.m_pNextSibling = SCENE_NODE_DISK_NULL;
	CopyNode.m_pNextRender = NULL;	// just to be clean, doesn't need fix up at run-time load

	/* fix up m_pData pointer contents. should be ok for most things already */

	switch(GetType())
	{
	case SCENE_NODE_INSTANCE:
		/* convert m_pData from CInstanceInfo pointer to export index */

		static CInstanceInfo *pInstanceInfo;	// static so it won't take up stack space during recursion
		pInstanceInfo = (CInstanceInfo *) m_pData;
		ASSERT(pInstanceInfo->IsValidForSceneTree());
		CopyNode.SetData((void *) pInstanceInfo->GetExportIndex());	// will get fixed up at game load time
		break;
	case SCENE_NODE_ALWAYS_PASS:
		/* should always be null */

		ASSERT(CopyNode.m_pData == NULL);
		break;
	case SCENE_NODE_BOUNDING_BOX:
		/* currently points to a valid ts_BoundingBox record, but for disk purposes this will always be fixed */
		/* up at load time to point to bounding record that immediately follows node record, so clear it just */
		/* to be clean */

		CopyNode.m_pData = NULL;
		break;
	default:
		ASSERT(FALSE);	// unknown node type
	}

	/* what kind of node? this must match corresponding section in WalkToExportData, except it writes out */
	/* CopyNode instead of current node */

	switch(GetType())
	{
	case SCENE_NODE_INSTANCE:
	case SCENE_NODE_ALWAYS_PASS:
		if (DBLUtil__WriteRecord(&CopyNode, pDBLFile) == FALSE)
			HANDLE_ERROR(RETURN_SCENE_NODE_WRITE_FAILED);
		break;
	case SCENE_NODE_BOUNDING_BOX:
		/* for bounding box node, write out node and then immediately follow with bounding box record */

		if (DBLUtil__WriteRecord(&CopyNode, pDBLFile) == FALSE)
			HANDLE_ERROR(RETURN_SCENE_NODE_WRITE_FAILED);
		if (DBLUtil__WriteRecord((ts_BoundingBox *) m_pData, pDBLFile) == FALSE)
			HANDLE_ERROR(RETURN_SCENE_NODE_WRITE_FAILED);
		break;
	default:
		ASSERT(FALSE);	// unknown node type
	}

	/* process children (recursive) */

	CSceneNode *pChild = m_pFirstChild;
	while (pChild)
	{
		int iReturnCode;
		if ((iReturnCode = pChild->WalkToFixExportData()) != RETURN_SUCCESS)
			return(iReturnCode);
		pChild = pChild->m_pNextSibling;
	}

	/* success */

	return(RETURN_SUCCESS);
}

/* recursively free a scene tree. assumes all nodes were created via new, and that all bounding boxes were malloc'd */

void CSceneNode::WalkToFree()
{
	/* first process children-- tree frees itself from leaves to the root */

	CSceneNode *pNextChild = m_pFirstChild;
	while (pNextChild)
	{
		CSceneNode *pChild = pNextChild;
		pNextChild = pNextChild->m_pNextSibling;	// child may get removed, so get next pointer before processing
		pChild->WalkToFree();
	}

	/* now look at this node-- data to free? */

	switch(GetType())
	{
	case SCENE_NODE_INSTANCE:
	case SCENE_NODE_ALWAYS_PASS:
		break;
	case SCENE_NODE_BOUNDING_BOX:
		free(m_pData);	// bounding box data is malloc'd
		break;
	default:
		ASSERT(FALSE);	// unknown node type
	}

	/* and then the node itself */

	delete this;
}

#endif //TOOLS
