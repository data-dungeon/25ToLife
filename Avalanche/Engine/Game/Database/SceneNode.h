/**********************************************/
/*                                            */
/* SceneNode.h                                */
/* big juju prototype  2/02/02                */
/* ned martin  avalanche software             */
/* visual c++ 6.0                             */
/* nodes for scene graph tree                 */
/*                                            */
/**********************************************/

/* the SceneNode class is basic component of a scene tree, which typically represents a world. the sene tree is traversed
by the scene manager to render the world */

#ifndef __SCENENODE_H
#define __SCENENODE_H

/* system includes */

/* engine includes */

#include "platform/BaseType.h"

/******************* forward declarations ****************/

class CSceneNode;				// forward declare for t_SceneNodeCallback declaration
struct ts_Frustum;			// in Game/Misc/Frustum.h
struct ts_BoundingBox;		// in Game/Misc/Bounds.h
struct ts_RecordList;		// in Game/Database/DBFile.h
class CInstance;		// in Game/Database/Instance.h
class FileStream;		// in EngineHelper/FileStream.h 
/******************* defines *****************************/

/* node types */

enum
{
	SCENE_NODE_INSTANCE = 0,	// pData points to a CInstance record
	SCENE_NODE_BOUNDING_BOX,	// pData points to a ts_BoundingBox record, in world space
	SCENE_NODE_ALWAYS_PASS,		// pData is NULL, just skip node and keep processing

	/*** NOTE! if you add more types, you may have to update SCENE_NODE_TYPE_MASK,   ***/
	/*** below. also, you will have to update the switch statements in SceneNode.cpp ***/

	/* must be last */

	SCENE_NODE_TYPES			
};

/* m_u32Flags values. first bits are node type. start individual flag bits at top of flags word so we don't
have to worry about bit collision and rebuilding data */

#define SCENE_NODE_TYPE_MASK		0x00000003	// must be big enough to cover all node types above
#define SCENE_NODE_ALLOCATED		0x80000000	// for finding a free node in SceneMgr::AllocSceneNode
#define SCENE_NODE_DISABLED		0x40000000	// if set, tree branch starting with this node is ignored
#define SCENE_NODE_NO_FAR_CLIP	0x20000000	// contains at least one child instance that has INSTANCE_NO_FAR_CLIP flag set
#define SCENE_NODE_PROCESSED		0x10000000	// transient -- used to not duplicate work on nodes or the whole tree
#define SCENE_NODE_DYN_VIS			0x08000000	// dynamically reassign to appropraite vis region
#define SCENE_NODE_IGNORE_FIRSTFRAME	0x04000000  // don't display for the first frame

/******************* macros ******************************/

/******************* structures **************************/

/* callback typedef for WalkWithCallbacks */

typedef void (*t_SceneNodeCallback)(CSceneNode *pNode);

/******************* global variables ********************/

/******************* global prototypes *******************/

/******************* class *******************************/

/*** note-- there may be lots of these in the scene tree (one per instance, and one per bounding node), so keep it lean! ***/

class CSceneNode
{
// Constructor/destructor

public:

	CSceneNode() {}
	~CSceneNode() {}

// Implementation-- data

protected:

	/* node may or may not point to some data. a typical type of data to point to is an instance. another typical one */
	/* is a bounding box/sphere record, used for nodes that divide the world up into smaller and smaller pieces before */
	/* you reach the leaves of the tree, which are typically instances */

	void				*m_pData;

	/* each node has a singly-linked list of child nodes-- just make it a pointer instead of a ts_SLinkList */
	/* to save some space, since there will be a lot of these. don't need much of the SLinkList functionality */

	CSceneNode		*m_pFirstChild;

	/* these pointers implement the child node linked list. each node has to be able to get back to it's parent */
	/* so that it can be removed from the tree (you can't trace backwards along a singly-linked list). using a */
	/* pParent pointer is much cheape, memory-wise, than using a doubly-linked list */

	CSceneNode		*m_pNextSibling;
	CSceneNode		*m_pParent;

	/* typically, nodes cannot be rendered as they are found while traversing the tree. for example, they may need */
	/* to be assigned to different rendering "buckets" (e.g. all opaque instances together), or they may need to be */
	/* sorted against the other instances in their bucket (e.g. transparent instances need to be rendered back to */
	/* front). so we need to be able to link things together while traversing the tree, and then to render them */
	/* in the correct order when we're done. this pointer allows the nodes to be linked together. we accomplish */
	/* sorting by using an array of singly linked lists, instances are added to the linked list at the correct */
	/* distance. this is a lot like ordering tables on the ps1, only it's for nodes instead of polys. */

	CSceneNode		*m_pNextRender;

	/* we need some information about what kind of node this is, and some status flags */

	u32				m_u32Flags;

// Implementation-- code

public:

	/*** must match disk data-- no virtual functions!!! ***/

	/* simple routines */
	inline bool IsAllocated() const					{ return((m_u32Flags & SCENE_NODE_ALLOCATED) != 0); }
	inline bool IsDisabled() const					{ return((m_u32Flags & SCENE_NODE_DISABLED) != 0); }
	inline bool IsEnabled() const						{ return((m_u32Flags & SCENE_NODE_DISABLED) == 0); }
	inline bool HasChildren() const					{ return(m_pFirstChild != NULL); }
	inline void *GetData() const						{ return(m_pData); }
	inline int GetType() const							{ return( (int)( m_u32Flags & SCENE_NODE_TYPE_MASK ) ); }
	inline u32 GetFlags() const						{ return( m_u32Flags & ~SCENE_NODE_TYPE_MASK ); }
	inline CSceneNode *GetNextRender() const		{ return(m_pNextRender); }
	inline CSceneNode *GetFirstChild() const		{ return(m_pFirstChild); }
	inline CSceneNode *GetNextSibling() const		{ return(m_pNextSibling); }	// note that this is NOT GetNextChild-- parent only knows about first child
	inline CSceneNode *GetParent() const			{ return(m_pParent); }

	inline void SetAllocated(bool bState)			{ if (bState) m_u32Flags |= SCENE_NODE_ALLOCATED; else m_u32Flags &= ~SCENE_NODE_ALLOCATED; }
	inline void SetEnabled(bool bState)				{ if (bState) m_u32Flags &= ~SCENE_NODE_DISABLED; else m_u32Flags |= SCENE_NODE_DISABLED; }
	inline void SetData(void *pData)					{ m_pData = pData; }
	inline void SetType(int nType)					{ ASSERT( ( nType & ~SCENE_NODE_TYPE_MASK ) == 0 );   m_u32Flags = ( m_u32Flags & ~SCENE_NODE_TYPE_MASK ) | nType; }
	inline void SetFlags(u32 u32Flags)				{ ASSERT( ( u32Flags & SCENE_NODE_TYPE_MASK ) == 0 ); m_u32Flags = ( m_u32Flags & SCENE_NODE_TYPE_MASK ) | u32Flags; }
	inline void SetNextRender(CSceneNode *pNext)	{ m_pNextRender = pNext; }
	inline void SetFirstChild(CSceneNode *pFirstChild) { m_pFirstChild = pFirstChild; }
	inline void SetNextSibling(CSceneNode *pNextSibling) { m_pNextSibling = pNextSibling; }
	inline void SetParent(CSceneNode *pParent)			{ m_pParent = pParent; }
	inline bool IsProcessed() const					{ return ( m_u32Flags & SCENE_NODE_PROCESSED ) != 0; }
	inline void SetProcessed( bool state )			{ if ( state ) m_u32Flags |= SCENE_NODE_PROCESSED; else m_u32Flags &= ~SCENE_NODE_PROCESSED; }

	/* routines defined in .cpp */

	void Init();
	void AddChild(CSceneNode *pChildNode);
	void Remove();
	CSceneNode *SearchChildren(void *pData);
	CSceneNode *SearchTree(void *pData);
	CSceneNode *FindNodeByInstanceName(const char* name);
	int ChildCount();
	int GetDepth();

	static void SetViewFrustum(ts_Frustum *pFrustum);
	static ts_Frustum *GetViewFrustum();

	static void SetGuardBandFrustum(ts_Frustum *pFrustum);
	static ts_Frustum *GetGuardBandFrustum();

	static void SetDiskLoadData(void *pChunkStart, ts_RecordList *pInstList);

	static void ClearCallbacks();
	static void RegisterCallback(int nType, t_SceneNodeCallback Callback);
	static void RegisterCallbackAllTypes(t_SceneNodeCallback Callback);

#ifdef TOOLS	// ImportExport only
	void SwapEndianInPlace();
	void SetDivideTargetSize(float fTargetSize);
	void InitBestBoundingParent(CSceneNode *pParent, ts_BoundingBox *pInstBox, bool bExpand);
	CSceneNode *GetBestBoundingParent();
#endif //TOOLS

	/* tree walks */

	void WalkWithCallbacks();
	void WalkWithCallbacksChildrenFirst();
	void WalkToSetFlag(u32 flag);
	void WalkToClearFlag(u32 flag);
#ifndef TOOLS
	void WalkToRenderScene();
	void WalkToRenderVisRegionOff();
	void WalkToRenderSceneBoxFrustumCulled();
	void WalkToVisualizeBounds();
	void WalkToResetDisplayLists();
	void WalkToFixDiskLoad();
	CInstance *WalkToFindFirstInstance();
	bool WalkToFindInstance(CInstance *pInst);
	bool WalkToFindVertexColors(void *pVertexColors);
	int WalkToReportInstances(FileStream *pLogFile, ts_PatchSOM *pCheckSOM = NULL, int nCount = 0);
#else //TOOLS
	void WalkToDivideXZ();
	void WalkToFindBestBoundingParentXZ(int nDepth);
	void WalkToRemoveUnneededNodes();
	void WalkToResizeBoundingNodes();
	int WalkToExportData();
	int WalkToFixExportData();
	void WalkToFree();
#endif //TOOLS

protected:

// operations

public:

};

#endif // __SCENENODE_H

