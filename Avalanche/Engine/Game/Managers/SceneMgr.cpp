//============================================================================
//=
//= SceneMgr.cpp
//=
//= the scene manager is responsible for keeping track of all things
//= (typically instances) that might be visible to the player, and for
//= rendering what the player sees. it typically has a tree of CSceneNode
//= records that it can traverse to determine what is visible to any
//= particular scene.  each scene typically has a camera and a view frustum
//= that are used to determine which instances are visible during a particular
//= frame. these are then rendered to generate the view that the player sees.
//= this scene manager does not keep any frame-to-frame information for a
//= scene, but runs the tree fresh each frame (once per frame per registered
//= scene, actually). as the tree is traversed to render a scene, the visible
//= instances are placed in various buckets (currently  opaque, transparent,
//= draw-first, with a draw-last bucket coming soon). once the tree has been
//= completely run, the contents of the buckets are rendered. some buckets
//= may require sorting
//=
//============================================================================

#include "Game/GamePCH.h"

#include "Layers/Timing.h"
#include "hud/newmenu.h"
#include "Game/Managers/OccluderMgr.h"
#include "Game/Managers/ShadowMgr.h"
#include "GameHelper/DebugOutput.h"

#ifdef MASSIVE_AD
#include "Massive\MassiveClientDriver.h"
#include "Massive\MassiveAdObject.h"
#endif

#define FADE_OUT_SPAN						5.0f

//============================================================================

void gfxDrawBufferedLines();

//============================================================================

// visibility debug states
enum {
	VISIBILITY_NORMAL,
	VISIBILITY_DEBUG,
	VISIBILITY_ALL,
	VISIBILITY_SHOW,
	VISIBILITY_FREEZE,
};

// occlusion debug states
enum {
	OCCLUSION_DEBUG_NORMAL,
	OCCLUSION_DEBUG_SHOW,
	OCCLUSION_DEBUG_FLASH,
	OCCLUSION_DEBUG_OFF,
	OCCLUSION_DEBUG_MAX,
};

//============================================================================

namespace SceneMgr
{
	// internal functions
	void			Clear();
	CSceneNode*	AllocSceneNode();
	bool			IsBucketVisible( int nBucketIndex );
	void			SortBuckets();
	void			EmptyBuckets();
	void			ToggleVisibleBucket();
	void			ToggleVisibilityDebug();
	void			ToggleOcclusionDebug();
	void			VisRegionUpdate();

	// a list of all managed scenes
	DynamicArray<CScene*> s_scenes( 0, 4 );

	// an array of scene nodes to support dynamically adding instances (like npc's) to scene tree-- things
	// that don't come in the world database. this is an array, not a pair of free/active linked lists,
	// because i want to keep the CSceneNode structure lean, and linked lists would require a pNext pointer.
	// i can get away with this because dynamically adding something to the scene tree should be a rare event
	CSceneNode*		s_sceneNodeAlloc;
	int				s_maxSceneNodes;						// size of array

	// status
	bool				s_active;								// will not render registered scene(s) if false

	// for SceneMgr::GetFirstCameraInfo, SceneMgr::GetNextCameraInfo
	CScene*			s_currentCameraInfoScene;

	// current scene being rendered
	CScene*			s_currentScene;

	// buckets for sorting to-be-rendered instances
	DynamicArray<NodeSortElement> s_bucket[ MAX_BUCKETS ] =
	{
		DynamicArray<NodeSortElement>( 0, 64 ),	// DRAW_FIRST_BUCKET
		DynamicArray<NodeSortElement>( 0, 64 ),	// OPAQUE_BUCKET
		DynamicArray<NodeSortElement>( 0, 64 ),	// TRANSPARENT_BUCKET
		DynamicArray<NodeSortElement>( 0, 64 )		// DRAW_LAST_BUCKET
	};

	// for instance-rendered-last-frame check. value is put into instance's u32SceneMgrFrame field if instance
	// gets rendered
	u32				s_frame;

	// for debugging
	u32				s_bucketVisible;				// value controls which buckets (draw-first, opaque, transparent) are visible
	BaseMenu*		s_bucketMenu;					// so we can update menu item text
	u32				s_visibilityDebug;
	BaseMenu*		s_visMenu;						// so we can update menu item text
	BaseMenu*		s_occlusionMenu;
	s32				s_occlusionDebug;
	Vector3			s_frozenCameraPos;			// for freezing visibility set visualization

	// camera y rotation
	float				s_cameraYRotation;

	// all occluders
	COccluderMgr	s_occluderMgr;

	// stats for scene
	CSceneStats		s_stats;

	// flag for knowing if a new instance was added to the scene tree
	bool			   s_newlyAddedInstances = false;
};

//============================================================================

class NodeSortElement
{
public:
	NodeSortElement( CInstance* inst, float dist )
	{
		d_inst = inst;
		d_dist = dist;
	}

	NodeSortElement( CInstance* inst, Vector3CRef camera )
	{
		d_inst = inst;
		d_dist = ( camera - inst->BoundingSphere().Center ).Length() + inst->BoundingSphere().Radius - inst->SortBias();
	}

	inline CInstance* Instance() const
		{ return d_inst; }

	// Must support these
	inline int operator<= ( const NodeSortElement& n ) const { return ( d_dist <= n.d_dist ); }
	inline int operator>= ( const NodeSortElement& n ) const { return ( d_dist >= n.d_dist ); }
	inline int operator<  ( const NodeSortElement& n ) const { return ( d_dist <  n.d_dist ); }
	inline int operator>  ( const NodeSortElement& n ) const { return ( d_dist >  n.d_dist ); }

private:
	CInstance* d_inst;
	float		  d_dist;
};

//============================================================================

void SceneMgr::Clear()
{
	s_sceneNodeAlloc = NULL;
	s_maxSceneNodes = 0;
	s_active = false;
	s_currentCameraInfoScene = NULL;
	s_currentScene = NULL;
	s_frame = 0;
	s_bucketVisible = 0;
	s_bucketMenu = NULL;
	s_visibilityDebug = 0;
	s_visMenu = NULL;
	s_cameraYRotation = 0.0f;
	s_frozenCameraPos.Clear();
	s_stats.Clear();
}

//============================================================================
// allocate records for scene's active and inactive instance lists. this is a global pool
// for all scenes

void SceneMgr::Init( int nMaxSceneNodes )
{
	// clear
	Clear();

	// set up pool of scene nodes for use by AddInstance
	s_maxSceneNodes = nMaxSceneNodes;
	MEM_SET_ALLOC_NAME("SceneMgr CSceneNodes");
	s_sceneNodeAlloc = (CSceneNode *) memAlloc(s_maxSceneNodes * sizeof(CSceneNode));
	for (int i = 0; i < s_maxSceneNodes; i++)
		s_sceneNodeAlloc[i].Init();

	// default active
	s_active = true;
}

//============================================================================
// Remove all runtime instances from scene trees

void SceneMgr::RemoveAllInstances()
{
	for ( int i = 0 ; i < s_maxSceneNodes ; i++ )
	{
		if ( s_sceneNodeAlloc[ i ].IsAllocated() )
		{
			RemoveInstance( (CInstance*) s_sceneNodeAlloc[ i ].GetData() );
		}
	}
}

//============================================================================

void SceneMgr::Reset()
{
	int i;

	// reset pool of scene nodes
	for ( i = 0; i < s_maxSceneNodes ; i++ )
	{
		ASSERTF( !s_sceneNodeAlloc[ i ].IsAllocated(), ( "instance '%s' still in scene tree", ((CInstance*)s_sceneNodeAlloc[ i ].GetData())->Name() ) );
		s_sceneNodeAlloc[ i ].Init();
	}

	// free all scenes
	for ( i = 0 ; i < s_scenes.Count() ; i++ )
		delete s_scenes[ i ];
	s_scenes.Reset();

	// empty buckets, free memory taken by arrays
	for ( i = 0 ; i < MAX_BUCKETS ; i++ )
		s_bucket[ i ].Reset();

	// clear occluders
	s_occluderMgr.Reset();

	// default active
	s_active = true;
	s_newlyAddedInstances = false;
}

//============================================================================

void SceneMgr::Shutdown()
{
	// do standard reset
	Reset();

	// shut down
	if ( s_sceneNodeAlloc )
		memFree( s_sceneNodeAlloc );

	Clear();
}

//============================================================================
// register a scene to be rendered by scene manager. scene is empty until either SetSceneTree is
// called to give the scene a scene tree to render or until instances are added by hand via AddInstance

CScene* SceneMgr::RegisterScene( Graphics4x4 *pg4WorldToCamera, int nType /*= SCENE_DISPLAY*/, Texture* pRealtimeTexture /*= NULL*/ )
{
	// allocate a new scene
	CScene* pScene = new CScene;

	// add to list of managed scenes
	s_scenes.Add( pScene );

	// record scene type and possible texture to render into
	ASSERT((nType >= 0) && (nType < SCENE_TYPES));
	pScene->d_type = nType;
	if ( nType == SCENE_RENDER_INTO_TEXTURE )
	{
		ASSERT_PTR(pRealtimeTexture);
		pScene->d_realtimeTexture = pRealtimeTexture;
	}
	else
		pScene->d_realtimeTexture = NULL;

	// record camera
	ASSERT(pg4WorldToCamera);
	ASSERT(pg4WorldToCamera != &g4WorldToCamera);	// can't use the global WorldToCamera variable, it gets used by other stuff (like reflections)
	pScene->d_worldToCamera = pg4WorldToCamera;

	// fill in scene view with default values
	Display__SetSceneViewToDefault( &pScene->d_sceneView );

	// scene is empty
	pScene->d_sceneTree = NULL;

	// scene is default disabled, and unlocked
	pScene->d_enabledForRendering = false;

	return pScene;
}

//============================================================================
// remove a scene from manager

void SceneMgr::KillScene( CScene* scene )
{
	// find in managed list
	for ( int i = 0 ; i < s_scenes.Count() ; i++ )
	{
		if ( s_scenes[ i ] == scene )
		{
			delete scene;
			s_scenes.Delete( i );
			return;
		}
	}

	ASSERTS( false, "SceneMgr::KillScene can't find scene" );
}

//============================================================================

COccluderMgr& SceneMgr::GetOccluderMgr()
{
	return s_occluderMgr;
}

//============================================================================
// render all active scenes. this should only be called once per frame

void SceneMgr::Render()
{
	int i;

	// scene manager must have been initialized, and be active
	if (s_active == false)
		return;

	// update frame counter for instance-rendered-last-frame check
	s_frame++;

	// quick exit
	if ( s_scenes.Count() == 0 && !RenderMgr.bForceClear )
	{
		gfxClearFrameBuffer( Display__GetClearMode(), Display__GetClearColor() );
		return;
	}

	// move dynamic-vis instances
	VisRegionUpdate();

	// prepare particle manager for rendering
	ParticleMgr__PrepareRender();
	ParticulateMgr::Update();

	// render all render-into-texture scenes
	bool bResetFrameBuffer = false;
	for ( i = 0 ; i < s_scenes.Count() ; i++ )
	{
		CScene* scene = s_scenes[ i ];
		if ( scene->d_type == SCENE_RENDER_INTO_TEXTURE )
		{
			// set up for rendering into real-time texture
			ASSERT_PTR(scene->d_realtimeTexture);
			scene->d_realtimeTexture->LockForRender();

			// render scene
			scene->Render();

			// finish rendering into texture
			scene->d_realtimeTexture->UnlockForRender();

			// have to reset the frame buffer
			bResetFrameBuffer = true;
		}
	}

	// rendering into a texture may have changed the frame buffer location
#if (defined(WIN32)) || (defined(PS2))
	if (bResetFrameBuffer)
		Display__SetFrameBuffer();
#endif

	// process all active display scenes
	for ( i = 0 ; i < s_scenes.Count() ; i++ )
	{
		CScene* scene = s_scenes[ i ];
		if ( scene->d_type == SCENE_DISPLAY )
		{
			scene->Render();
		}
	}

	// allow all first-frame instances to be rendered
	if ( s_newlyAddedInstances )
	{
		for ( i = 0 ; i < s_scenes.Count() ; i++ )
		{
			CScene* scene = s_scenes[ i ];
			scene->d_sceneTree->WalkToClearFlag(SCENE_NODE_IGNORE_FIRSTFRAME);
		}
		s_newlyAddedInstances = false;
	}

	// out of the scene manager
	s_currentScene = NULL;
}

#ifdef DIRECTX_PC
void SceneMgr::RenderWithoutSetup()
{
	int i;
	for ( i = 0 ; i < s_scenes.Count() ; i++ )
	{
		CScene* scene = s_scenes[ i ];
		if ( scene->d_type == SCENE_DISPLAY )
		{
			scene->Render();
		}
	}
}
#endif


//============================================================================
// re-render a scene, with a new frustum to cull out unnecessary instances.
// typically used for shadows

void SceneMgr::RerenderScene(CScene *pScene, ts_Frustum *pFrustum, CInstance *pSelf)
{
	// scene must match, or else bucket contents will be wrong
	ASSERT( pScene == s_currentScene );

	for ( int i = 0 ; i < MAX_BUCKETS ; i++ )
		pScene->RerenderBucket( i, pFrustum, pSelf );
}

//============================================================================
// get the first scene registered with the scene manager (may be null).
// typically users are only interested in the scenes that get rendered to the
// screen (type == SCENE_DISPLAY), not scenes that are used for special
// effects. to get all scenes, pass nType == SCENE_TYPE_ALL

CScene* SceneMgr::GetFirstScene( int nType /*= SCENE_DISPLAY*/ )
{
	for ( int i = 0 ; i < s_scenes.Count() ; i++ )
		if ( s_scenes[ i ]->d_type == nType )
			return s_scenes[ i ];

	return NULL;
}

//============================================================================
// get the next scene registered with the scene manager (may be null).
// typically users are only interested in the scenes that get rendered to the
// screen (type == SCENE_DISPLAY), not scenes that are used for special
// effects. to get all scenes, pass nType == SCENE_TYPE_ALL

CScene* SceneMgr::GetNextScene( CScene *pScene, int nType /*= SCENE_DISPLAY*/ )
{
	// find the last scene
	int i;
	for ( i = 0 ; i < s_scenes.Count() ; i++ )
		if ( s_scenes[ i ] == pScene )
			break;
	ASSERTS( i < s_scenes.Count(), "GetNextScene couldn't find last scene" );

	// find next scene
	for ( ++i ; i < s_scenes.Count() ; i++ )
		if ( s_scenes[ i ]->d_type == nType )
			return s_scenes[ i ];

	return NULL;
}

//============================================================================
// return pointer to scene view governing this scene

CSceneView* SceneMgr::GetCurrentSceneView()
{
	if (s_currentScene == NULL)
		return(NULL);
	return(&s_currentScene->d_sceneView);
}

//============================================================================
// set the scene tree root of all scenes, or for a specified scene. the scene
// tree is the data that the scene will render

void SceneMgr::SetSceneTree(CSceneNode *pSceneTree, ts_RecordList *pVisRegionList, CScene* pScene /* = NULL */ )
{
	// if adding to a specific scene, do that now
	if ( pScene )
	{
		// set root
		pScene->d_sceneTree = pSceneTree;

		// set visibility region list-- may be NULL
		pScene->d_visRegionList = pVisRegionList;

		// if there is a visibility region list, the last entry may be the whole-world branch, and needs to be handled
		// somewhat specially. whole world region has a huge bounding box so that the camera test always passes. if we
		// find the camera to be in any true visibility region, then we will rely on whether the whole-world branch is
		// in that region's visible list, and skip testing the whole world region itself
		if ( pVisRegionList )
		{
			pScene->d_wholeWorldIndex = pVisRegionList->usRecordCount;			// assume no whole-world region
			CVisRegion *pWholeWorldRegion = (CVisRegion *) GetRecordAddress(pVisRegionList, pVisRegionList->usRecordCount - 1);
			if (pWholeWorldRegion->GetBoundingBox()->Max.x() == MAXFLOAT32)
				pScene->d_wholeWorldIndex = pVisRegionList->usRecordCount - 1;	// last region is the whole-world region
		}
	}
	else
	{
		for ( int i = 0 ; i < s_scenes.Count() ; i++ )
			SetSceneTree( pSceneTree, pVisRegionList, s_scenes[ i ] );
	}
}

//============================================================================
// construct an empty scene tree-- if scene doesn't have a non-NULL scene tree
// pointer, it won't render. this allows an empty scene

CSceneNode* SceneMgr::EmptySceneTree()
{
	static CSceneNode DummyTree;
	DummyTree.Init();
	DummyTree.SetType(SCENE_NODE_ALWAYS_PASS);
	return(&DummyTree);
}

//============================================================================
// add an instance to all scenes

bool SceneMgr::AddInstance( CInstance* inst )
{
	// must have an instance
	ASSERT_PTR( inst );

	if ( !s_active )
		return false;

	// scenes may share scene trees, so use the processed flag to make sure we
	// don't hit a tree more than once.
	int i;
	for ( i = 0 ; i < s_scenes.Count() ; i++ )
		if ( s_scenes[ i ]->d_sceneTree )
			s_scenes[ i ]->d_sceneTree->SetProcessed( false );

	bool result = true;
	for ( i = 0 ; i < s_scenes.Count() ; i++ )
	{
		CScene* scene = s_scenes[ i ];
		if ( scene->d_sceneTree && !scene->d_sceneTree->IsProcessed() )
		{
			if ( !scene->AddInstance( inst ) )
				result = false;
			scene->d_sceneTree->SetProcessed( true );
		}
	}

	s_newlyAddedInstances = true;
	
	return result;
}

//============================================================================
// remove an instance to all scenes, or to a specified scene. can be called by
// the scene manager's group processing, or can be called directly by the game
// app (typically for things like removing an actor from the scene)

void SceneMgr::RemoveInstance( CInstance* inst )
{
	if ( inst )
	{
		// scenes may share scene trees, so use the processed flag to make sure we
		// don't hit a tree more than once.
		int i;
		for ( i = 0 ; i < s_scenes.Count() ; i++ )
			if ( s_scenes[ i ]->d_sceneTree )
				s_scenes[ i ]->d_sceneTree->SetProcessed( false );

		for ( i = 0 ; i < s_scenes.Count() ; i++ )
		{
			CScene* scene = s_scenes[ i ];
			if ( scene->d_sceneTree && !scene->d_sceneTree->IsProcessed() )
			{
				scene->RemoveInstance( inst );
				scene->d_sceneTree->SetProcessed( true );
			}
		}
	}
}

//============================================================================

void SceneMgr::SetInstanceVisRegionStatic( CInstance* inst, bool useWorld )
{
	if ( inst )
	{
		int i;
		for ( i = 0 ; i < s_scenes.Count() ; i++ )
			if ( s_scenes[ i ]->d_sceneTree )
				s_scenes[ i ]->d_sceneTree->SetProcessed( false );

		for ( i = 0 ; i < s_scenes.Count() ; i++ )
		{
			CScene* scene = s_scenes[ i ];
			if ( scene->d_sceneTree && !scene->d_sceneTree->IsProcessed() )
			{
				scene->SetInstanceVisRegionStatic( inst, useWorld );
				scene->d_sceneTree->SetProcessed( true );
			}
		}
	}
}

//============================================================================

void SceneMgr::SetInstanceVisRegionDynamic( CInstance* inst )
{
	if ( inst )
	{
		int i;
		for ( i = 0 ; i < s_scenes.Count() ; i++ )
			if ( s_scenes[ i ]->d_sceneTree )
				s_scenes[ i ]->d_sceneTree->SetProcessed( false );

		for ( i = 0 ; i < s_scenes.Count() ; i++ )
		{
			CScene* scene = s_scenes[ i ];
			if ( scene->d_sceneTree && !scene->d_sceneTree->IsProcessed() )
			{
				scene->SetInstanceVisRegionDynamic( inst );
				scene->d_sceneTree->SetProcessed( true );
			}
		}
	}
}

//============================================================================
// return pointer to first scene's view frustum, if any

ts_Frustum* SceneMgr::GetFirstViewFrustum()
{
	s_currentCameraInfoScene = GetFirstScene();
	return(GetNextViewFrustum());	// use GetNextViewFrustum to actually process
}

//============================================================================
// return pointer to next scene's view frustum, if any

ts_Frustum* SceneMgr::GetNextViewFrustum()
{
	if (s_currentCameraInfoScene)
	{
		ts_Frustum *pFrustum = s_currentCameraInfoScene->d_sceneView.GetViewFrustum();
		s_currentCameraInfoScene = GetNextScene( s_currentCameraInfoScene );	// to next scene for next call
		return(pFrustum);
	}
	else
		return(NULL);
}

//============================================================================
// find a free scene node. linear search, shouldn't be called very often

CSceneNode* SceneMgr::AllocSceneNode()
{
	// find a free node
	for (int i = 0; i < s_maxSceneNodes; i++)
	{
		if (s_sceneNodeAlloc[i].IsAllocated() == false)
		{
			s_sceneNodeAlloc[i].Init();	// start out fresh
			s_sceneNodeAlloc[i].SetAllocated(true);
			return(&s_sceneNodeAlloc[i]);
		}
	}

	// no free node found
	ASSERT(false);	// shouldn't run out of nodes. if so, increase nMaxSceneNodes count passed to Init
	return(NULL);
}

//============================================================================

void SceneMgr::ResetDisplayLists()
{
	int i;
	for ( i = 0 ; i < s_scenes.Count() ; i++ )
		if ( s_scenes[ i ]->d_sceneTree )
			s_scenes[ i ]->d_sceneTree->SetProcessed( false );

	for ( i = 0 ; i < s_scenes.Count() ; i++ )
	{
		CScene* scene = s_scenes[ i ];
		if ( scene->d_sceneTree && !scene->d_sceneTree->IsProcessed() )
		{
			scene->d_sceneTree->WalkToResetDisplayLists();
			scene->d_sceneTree->SetProcessed( true );
		}
	}
}

//============================================================================
// for debugging, allow user to toggle through various scene lists
// render this list? can be useful to toggle through lists to see which list a
// given instance belongs to

bool SceneMgr::IsBucketVisible(int nBucketIndex)
{
	switch ( s_bucketVisible )
	{
		default:
		case 0:	return true;	// all buckets visible
		case 1:	return nBucketIndex == OPAQUE_BUCKET;
		case 2:	return nBucketIndex == TRANSPARENT_BUCKET;
		case 3:	return nBucketIndex == DRAW_FIRST_BUCKET;
		case 4:	return nBucketIndex == DRAW_LAST_BUCKET;
	}
}

//============================================================================

void SceneMgr::ToggleVisibleBucket()
{
	if ( ++s_bucketVisible > 4 )
		s_bucketVisible = 0;

	if ( s_bucketMenu )
	{
		switch ( s_bucketVisible )
		{
			default:
			case 0:	s_bucketMenu->SetText( "Render: All" );			break;
			case 1:  s_bucketMenu->SetText( "Render: Opaque" );		break;
			case 2:  s_bucketMenu->SetText( "Render: Transparent" );	break;
			case 3:  s_bucketMenu->SetText( "Render: Draw-first" );	break;
			case 4:  s_bucketMenu->SetText( "Render: Draw-last" );	break;
		}
	}
}

//============================================================================

void SceneMgr::ToggleVisibilityDebug( void )
{
	if ( ++s_visibilityDebug > VISIBILITY_FREEZE )
		s_visibilityDebug = VISIBILITY_NORMAL;

	if ( s_visMenu )
	{
		switch ( s_visibilityDebug )
		{
			default:
			case VISIBILITY_NORMAL:	s_visMenu->SetText("Visibility: Normal");	break;
			case VISIBILITY_DEBUG:	s_visMenu->SetText("Visibility: Flash");	break;
			case VISIBILITY_ALL:		s_visMenu->SetText("Visibility: All");		break;
			case VISIBILITY_SHOW:	s_visMenu->SetText("Visibility: Show");	break;
			case VISIBILITY_FREEZE:
				s_visMenu->SetText("Visibility: Freeze");
				s_frozenCameraPos = SceneMgr::GetFirstScene()->d_cameraWorldPos;
				break;
		}
	}
}

//============================================================================

void SceneMgr::ToggleOcclusionDebug()
{
	if ( ++s_occlusionDebug >= OCCLUSION_DEBUG_MAX )
		s_occlusionDebug = 0;

	if ( s_occlusionMenu )
	{
		switch ( s_occlusionDebug )
		{
			default:
			case OCCLUSION_DEBUG_NORMAL:	s_occlusionMenu->SetText( "Occlude Debug: Normal" ); break;
			case OCCLUSION_DEBUG_SHOW:		s_occlusionMenu->SetText( "Occlude Debug: Show"   ); break;
			case OCCLUSION_DEBUG_FLASH:	s_occlusionMenu->SetText( "Occlude Debug: Flash"  ); break;
			case OCCLUSION_DEBUG_OFF:		s_occlusionMenu->SetText( "Occlude Debug: Off"    ); break;
		}
	}
}

//============================================================================

void SceneMgr::RegisterDebugMenus( BaseMenu* menu )
{
	s_bucketMenu    = menu->AddMenuItem( "Render: All",           ToggleVisibleBucket   );
	s_visMenu       = menu->AddMenuItem( "Visibility: Normal",    ToggleVisibilityDebug );
	s_occlusionMenu = menu->AddMenuItem( "Occlude Debug: Normal", ToggleOcclusionDebug  );
}

//============================================================================
// this routine is called by CSceneNode.WalkToRenderScene() when it finds an
// instance node that may be in view

void SceneMgr::FoundInstanceNode(CSceneNode *pSceneNode)
{
	if ( pSceneNode->GetFlags() & SCENE_NODE_IGNORE_FIRSTFRAME )
		return;

	ASSERT(pSceneNode->GetType() == SCENE_NODE_INSTANCE);
	CInstance *pInst = (CInstance *) pSceneNode->GetData();

	GetStats().IncInstancesTotal( 1 );

	// if instance is hidden, ignore it
	if ( pInst->Hidden() )
	{
		GetStats().IncInstancesHidden( 1 );
		return;
	}

	// get camera for fade calculation below
	float cameraZ = -((g4WorldToCamera.r02 * pInst->Matrix().tx) +
							(g4WorldToCamera.r12 * pInst->Matrix().ty) +
							(g4WorldToCamera.r22 * pInst->Matrix().tz) +
							g4WorldToCamera.tz);

	// instance's overall alpha will be calculated into here
	float alpha = pInst->Alpha();
	if ( !Math::Equal( pInst->FadeOutDistance(), INSTANCE_NO_FADE_DISTANCE ) )
	{
		float t = ( pInst->FadeOutDistance() + FADE_OUT_SPAN - Math::Abs( cameraZ ) ) * ( 1.0f / FADE_OUT_SPAN );
		if      ( t < 0.0f )  t = 0.0f;
		else if ( t > 1.0f )  t = 1.0f;
		alpha *= t;
	}

	if ( alpha <= 0.0f )
	{
		pInst->RenderOffScreen();
		return;
	}

	// check instance against view frustum
	u8 u8Clipped;	// non-zero means instance will be clipped by view frustum at render
	bool bFogged;	// non-zero means instance is in fog zone

#ifndef CDROM
	bool bInView = ( g_replayMgr.IsPlaying( ) && pInst->Body() && pInst->Body()->pBones ) ? true : Frustum__IntersectsWorldBounds(
							&pInst->BoundingSphere(),
							&pInst->BoundingBox(),
							&u8Clipped,
							&bFogged,
							pInst->FarClipDisabled() );
#else
	bool bInView = Frustum__IntersectsWorldBounds(
							&pInst->BoundingSphere(),
							&pInst->BoundingBox(),
							&u8Clipped,
							&bFogged,
							pInst->FarClipDisabled() );
#endif

	if ( !bInView )
	{
		pInst->RenderOffScreen();
		GetStats().IncInstancesFrustumCulled( 1 );
		return;
	}


// Handy debug -- draw bounding sphere for actors
//	if ( pInst->Owner() )
//	{
//		DrawUtility::Sphere( pInst->BoundingSphere().Center, DrawUtility::BLUE, pInst->BoundingSphere().Radius );
//	}

	if ( SceneMgr::s_occlusionDebug != OCCLUSION_DEBUG_OFF )
	{
		if ( SceneMgr::GetOccluderMgr().IsOccluded( pInst ) )
		{
			pInst->RenderOffScreen();
			GetStats().IncInstancesOccluded( 1 );

			if ( s_occlusionDebug == OCCLUSION_DEBUG_SHOW )
				DrawUtility::Sphere( pInst->BoundingSphere().Center, DrawUtility::WHITE, pInst->BoundingSphere().Radius );

			if ( s_occlusionDebug != OCCLUSION_DEBUG_FLASH )
				return;
			else if ( ( ( GetFrameNumber() / 25 ) & 1 ) == 0 )
				return;
		}
	}

	// stats
	GetStats().IncInstancesRendered( 1 );

	// confirm NO_FAR_CLIP state. if instance is marked as NO_FAR_CLIP, scene tree node should be too (as
	// well as parent nodes up to root, though that's not checked here). this assert is intended to catch
	// the program setting the INSTANCE_NO_FAR_CLIP flag during runtime on an instance that is already in
	// scene tree-- probably shouldn't happen. talk to ned if this hits
	ASSERT( ( ( pSceneNode->GetFlags() & SCENE_NODE_NO_FAR_CLIP ) != 0 ) == pInst->FarClipDisabled() );

	u32 renderViewFlags = 0;

	// do we clip the near and/or the far planes?
	if ( u8Clipped )
	{
		renderViewFlags |= INSTANCE_VIEW_CLIPPED;
		if ( FAR_PLANE_CLIPPED( u8Clipped ) )
			renderViewFlags |= INSTANCE_VIEW_CLIPPED_FAR_PLANE;
	}

	// are we fogged?
	#ifdef CHECK_FOG_STATUS
	if (bFogged)
		renderViewFlags |= INSTANCE_VIEW_FOGGED;
	#endif CHECK_FOG_STATUS

	// set view flags
	pInst->SetViewFlags( renderViewFlags );

	// put instance into correct bucket. not all instances need to be sorted
	if ( pInst->RenderBucket() < 0 )
	{
		// render first, use specified bucket number as sort value
		NodeSortElement n( pInst, (float)-pInst->RenderBucket() );
		s_bucket[ DRAW_FIRST_BUCKET ].Add( n );
	}
	else if ( pInst->RenderBucket() > 0 )
	{
		// render last, use specified bucket number as sort value
		NodeSortElement n( pInst, (float)pInst->RenderBucket() );
		s_bucket[ DRAW_LAST_BUCKET ].Add( n );
	}
	else if ( pInst->AlphaSort() || alpha < 1.0f )
	{
		// transparent bucket, sort based on distance from camera
		NodeSortElement n( pInst, s_currentScene->d_cameraWorldPos );
		s_bucket[ TRANSPARENT_BUCKET ].Add( n );
	}
	else
	{
		// opaque bucket, sort based on texture array index
		int textureIndex = ( pInst->Model() ) ? pInst->Model()->pTextureArray->CacheID() : 0x7fff;
		NodeSortElement n( pInst, (float)textureIndex );
		s_bucket[ OPAQUE_BUCKET ].Add( n );
	}
}

//============================================================================

void SceneMgr::FoundInstanceNodeSceneBoxFrustumCulled(CSceneNode* pSceneNode)
{
	GetStats().IncInstancesTotal( 1 );
	GetStats().IncInstancesSceneBoxFrustumCulled( 1 );
	CInstance* inst = (CInstance*) pSceneNode->GetData();
	inst->RenderOffScreen();
}

//============================================================================

void SceneMgr::FoundInstanceNodeVisRegionOff(CSceneNode* pSceneNode)
{
	GetStats().IncInstancesTotal( 1 );
	GetStats().IncInstancesVisRegionOff( 1 );
	CInstance* inst = (CInstance*) pSceneNode->GetData();
	inst->RenderOffScreen();
}

//============================================================================
// Really sort the buckets

void SceneMgr::SortBuckets()
{
	for ( int i = 0 ; i < MAX_BUCKETS ; i++ )
	{
		QuickSort<NodeSortElement> getSorted;
		if ( i == TRANSPARENT_BUCKET )
			getSorted.SortDescending( s_bucket[ i ].GetPtr(), s_bucket[ i ].Count() );
		else
			getSorted.SortAscending( s_bucket[ i ].GetPtr(), s_bucket[ i ].Count() );
	}
}

//============================================================================

void SceneMgr::EmptyBuckets()
{
	for ( int i = 0 ; i < MAX_BUCKETS ; i++ )
	{
		// use Clear(), not Reset(), so it keeps the ram allocated around.
		// This way we only malloc the first few frames of rendering.
		s_bucket[ i ].Clear();
	}
}

//============================================================================

void SceneMgr::VisRegionUpdate()
{
	// move dynamically assigned actors to the best fitting vis region

	// scenes may share scene trees, so use the processed flag to make sure we
	// don't hit a tree more than once.
	int i;
	for ( i = 0 ; i < s_scenes.Count() ; i++ )
		if ( s_scenes[ i ]->d_sceneTree )
			s_scenes[ i ]->d_sceneTree->SetProcessed( false );

	bool result = true;
	for ( i = 0 ; i < s_scenes.Count() ; i++ )
	{
		CScene* scene = s_scenes[ i ];
		if ( scene->d_sceneTree && !scene->d_sceneTree->IsProcessed() )
		{
			scene->VisRegionUpdate();
			scene->d_sceneTree->SetProcessed( true );
		}
	}
}

//============================================================================

void SceneMgr::DumpInstanceList()
{
	uint8 cat = g_debugOutput.RegisterCategory("Instances", "INST");
	g_debugOutput.Print( cat, "--INSTANCE LIST--" );
	for ( int i = 0 ; i < s_maxSceneNodes ; i++ )
	{
		if ( s_sceneNodeAlloc[ i ].IsAllocated() )
		{
			CInstance* inst = (CInstance*) s_sceneNodeAlloc[ i ].GetData();
			if ( inst )
			{
				g_debugOutput.Print( cat, "%3d %08X %08X %s %s",
											i,
											inst,
											inst->UniqueID(),
											inst->Hidden() ? "H" : " ",
											inst->Name() ? inst->Name() : "<null>" );
			}
		}
	}
}

//============================================================================

CInstance* SceneMgr::GetInstanceByName( const char* name )
{
	for ( CScene* scene = GetFirstScene(); scene; scene = GetNextScene( scene ) )
	{
		CSceneNode* root = scene->d_sceneTree;
		if ( root == NULL )
			continue;

		CSceneNode* node = root->FindNodeByInstanceName( name );

		if ( node != NULL)
		{
			ASSERT( node->GetType() == SCENE_NODE_INSTANCE );
			ASSERT( node->GetData() != NULL );
			return (CInstance*) node->GetData();
		}
	}

	return NULL;
}

//============================================================================

//============================================================================
//============================================================================
//============================================================================
//=====
//=====          SCENE ROUTINES
//=====
//============================================================================
//============================================================================
//============================================================================



//============================================================================
// scenes can be enabled/disabled while still registered

void CScene::EnableForRendering( bool enable )
{
	d_enabledForRendering = enable;
}

//============================================================================
// render a particular scene. all camera matrices have been calculated. this
// routine is called both by the scene manager to render a game view (e.g. a
// scene that will be shown to the player), and by other managers like the
// reflection and shadow managers to generate a real time texture.
// pScene.SceneView is assumed to have been set up appropriately

void CScene::Render( bool bReflection /*= false*/ )
{
	// quick exit
	if ( !d_enabledForRendering )
		return;
	if ( !d_sceneTree )
		return;

	// reflection rendering calls this with some stuff already set up
	if ( !bReflection )
	{
		// calculate inverse, and camera world position
		d_worldToCamera->Invert( *(Matrix4x4 *) &d_cameraToWorld );
		d_cameraWorldPos = d_cameraToWorld.GetTranslation();

		// create reflection maps for reflecting surfaces in this scene (if any)
		ReflectionMgr__CreateReflections( this );
	}

	// record the current scene
	SceneMgr::s_currentScene = this;

	// copy some already-calculated scene info into fast global variables
	g4WorldToCamera = *d_worldToCamera;
	g4CameraToWorld = d_cameraToWorld;

	// start-of-scene-render callback-- app may want to change scene view settings
	if ( RenderCallbackList[RENDER_SCENE_CALLBACK].bActive )
		RenderCallbackList[RENDER_SCENE_CALLBACK].RenderCallback( RenderCallbackList[RENDER_SCENE_CALLBACK].pCallbackData );

	// render scene view-- controls field of view, near/far clip, fog, etc
#ifdef DIRECTX_PC
	Image * renderImage = RenderToTexture::renderSurface->GetImage();
	//d_sceneView.SetViewport(0,0,(float)renderImage->GetRealImageWidth(), (float)renderImage->GetRealImageHeight());
#endif
	d_sceneView.Render();
	d_sceneView.ClearFrameBuffer();

	// calculate camera y-rotation. pass (1, 0, 0) through camera matrix (rotation only) and look x/z relation.
	// use (1, 0, 0) because at 0 rotation, i want x = 1 and z = 0 so that atan2(z, x) comes out to y-angle = 0.
	// from top view, positive x-axis is to the right and positive z-axis is down. at 0-rotation, camera looks
	// along negative z-axis, or up. at 90 degrees, camera looks left, along negative x-axis (positive angle is
	// counterclockwise rotation)
	static Vector3 vOneZeroZero( 1.0f, 0.0f, 0.0f );
	Vector3 vYRotation;
	vYRotation.FastBasisTransform( vOneZeroZero, g4WorldToCamera );
	SceneMgr::s_cameraYRotation = Math::ArcTan2( vYRotation.z(), vYRotation.x() );

	// calculate world-space frustum view volume for group/instance culling. currently, the global ViewFrustum
	// contains the frustum settings-- near and far clip, aspect ratio, fov, etc. the scene's copy really just
	// contains the world-bounds info. do this after the callback so app has a chance to change scene's frustum
	Frustum__TransformToWorld( d_sceneView.GetViewFrustum(), &g4CameraToWorld );

	#ifdef PS2
	// calculate guard band frustum for clip status refinement
	Frustum__TransformToWorld( d_sceneView.GetGuardBandFrustum(), &g4CameraToWorld );
	#endif //PS2

	// empty rendering buckets
	SceneMgr::EmptyBuckets();

	// initialize the occluders with this camera's point of view.
	if ( SceneMgr::s_occlusionDebug != OCCLUSION_DEBUG_OFF )
	{
		SceneMgr::GetOccluderMgr().Setup( ( SceneMgr::s_visibilityDebug == VISIBILITY_FREEZE ) ? SceneMgr::s_frozenCameraPos : d_cameraWorldPos );
		if ( SceneMgr::s_occlusionDebug == OCCLUSION_DEBUG_SHOW )
			SceneMgr::GetOccluderMgr().Draw();
	}

	// set the frustum for walking the scene tree
	d_sceneTree->SetViewFrustum( d_sceneView.GetViewFrustum() );
	#ifdef PS2
	d_sceneTree->SetGuardBandFrustum( d_sceneView.GetGuardBandFrustum() );
	Frustum__SetupToCheckFrustum( d_sceneView.GetViewFrustum(), d_sceneView.GetGuardBandFrustum() );
	#else
	Frustum__SetupToCheckFrustum( d_sceneView.GetViewFrustum() );
	#endif //PS2

	// set visibility regions-- what can be seen from this camera position
	SetVisibilityRegions();

	// walk the scene tree to find instances to render-- fills rendering buckets
	Profiler__Start( PROFILE_WALK_SCENE_TREE );
	d_sceneTree->WalkToRenderScene();
	Profiler__Stop( PROFILE_WALK_SCENE_TREE );

	// sort the buckets
	SceneMgr::SortBuckets();

	// lights may need updating with world-to-camera matrix
	RenderState__PrepareHardwareLights();

	// reset render state
	RenderState__ForceAll();
	RenderState__Default();

//#ifndef CONSUMER_BUILD
#ifdef WIN32
	// handle wireframe-- currently globally on or off
	if (RenderMgr__WireframeOn())
		RenderState__Request( RS_POLYGONMODE, RS_POLYGONMODE_LINE );
	else
		RenderState__Request( RS_POLYGONMODE, RS_POLYGONMODE_FILL );
	RenderState__Lock(RS_POLYGONMODE);
#endif //WIN32
//#endif //CONSUMER_BUILD

	// calculate all light influences
	ComputeLightInfluences();

	// render culled and sorted active draw-first instances
	Profiler__Start(PROFILE_RENDER_BUCKETS);
	RenderBucket( DRAW_FIRST_BUCKET );
	RenderBucket( OPAQUE_BUCKET );
	TextureMgr::Cache().Unlock();
	Profiler__Stop(PROFILE_RENDER_BUCKETS);

	// unlock any lingering cached textures

	// render shadows
	Profiler__Start(PROFILE_RENDER_SHADOWS);
#if !defined(DIRECTX) || defined(DIRECTX_PC)	
	ShadowMgr__RenderShadows( this );
#endif
	Profiler__Stop(PROFILE_RENDER_SHADOWS);

	// purge texture ram so we have access to the full texture window
	TextureMgr::Cache().Unlock();
//	TextureMgr::Pause();
//	TextureMgr::Resume();

	Profiler__Start(PROFILE_RENDER_BUCKETS);
	RenderBucket( TRANSPARENT_BUCKET );
	RenderBucket( DRAW_LAST_BUCKET );
	TextureMgr::Cache().Unlock();
	Profiler__Stop(PROFILE_RENDER_BUCKETS);

	// callback before particles are rendered
	if (RenderCallbackList[RENDER_PARTICLE_CALLBACK].bActive)
		RenderCallbackList[RENDER_PARTICLE_CALLBACK].RenderCallback(RenderCallbackList[RENDER_PARTICLE_CALLBACK].pCallbackData);

	// render the particles and swooshes in this scene
	Profiler__Start(PROFILE_PARTICLE_RENDER);
	ParticleMgr__Render( this );
	SwooshMgr__Render();
	ParticulateMgr::Render();
	Profiler__Stop(PROFILE_PARTICLE_RENDER);

	// render lighting effects
	//LightMgr__RenderSpecialEffects( this );

	// unlock
	RenderState__Unlock(RS_POLYGONMODE);

	//	AttachMgr__ShowAttachers();

#ifndef CONSUMER_BUILD
	// draw buffered lines, etc
	RenderState__Default();
	MatrixStack__Mode( MS_MODELVIEW );
	MatrixStack__Load( &g4WorldToCamera );
	gfxDrawBufferedLines();

	// maybe visualize scene tree bounding boxes
	if ( RenderMgr__VisualizeSceneTree() )
	{
		MatrixStack__Mode( MS_MODELVIEW );
		MatrixStack__Load( &g4WorldToCamera );
		d_sceneTree->WalkToVisualizeBounds();
	}
#endif
	// done walking the tree-- clean up
	d_sceneTree->SetViewFrustum( NULL );
	#ifdef PS2
	d_sceneTree->SetGuardBandFrustum( NULL );
	#endif //PS2


#ifdef MASSIVE_AD
	//Updates the Client Lib
	MassiveClientZoneFrameTick();
#endif
}

//============================================================================
// given a world position, return screen space x/y position (0.0, 0.0) is top
// left, (1.0, 1.0) is bottom right

bool CScene::GetScreenCoords( Vector3CRef worldPos, Vector2& screenCoords )
{
	// put world coords through world-to-camera
	Vector3 vCameraPos = worldPos * *d_worldToCamera;

	// no result if point is in front of near clip plane. at this point, z is negative into the screen
	if ( vCameraPos.z() > -d_sceneView.GetNearClip() )
		return false;

	// put camera coords through projection matrix
	Vector4 vProjectedPos = vCameraPos.Transform3( d_sceneView.GetProjection() );

	// we now have a position within projection space (also called canonical
	// clip space). if we divide everything by w, we get
	// -1 < x < 1, -1 < y < 1, 0 < z < 1, which is also useful for clipping
	// checks. the x/y range represents the screen
	float x = vProjectedPos.x() / vProjectedPos.w();
	float y = vProjectedPos.y() / vProjectedPos.w();
	#ifdef PS2
	y = -y;
	#endif

	// ok, we have -1 < x < 1 and -1 < y < 1. we want to return x and y between
	// 0 and 1, with (0, 0) the top left of the screen and (1, 1) the bottom right
	x = ( x *  0.5f ) + 0.5f;
	y = ( y * -0.5f ) + 0.5f;

	// now adjust to viewport position on screen
	x = ( x * d_sceneView.GetViewport()->fWidth  ) + d_sceneView.GetViewport()->fX;
	y = ( y * d_sceneView.GetViewport()->fHeight ) + d_sceneView.GetViewport()->fY;

	// done. note that this position may be off-screen, or beyond the far clip plane
	screenCoords.Set( x, y );
	return true;
}

//============================================================================
// compute the light influences for all instances in the scene

void CScene::ComputeLightInfluences()
{
	if ( g_lightSet.lightSetIsLoaded() )
	{
		for ( int bucket = 0 ; bucket < MAX_BUCKETS ; bucket++ )
		{
			int bucketCount = SceneMgr::s_bucket[ bucket ].Count();
			for ( int i = 0 ; i < bucketCount ; i++ )
			{
				// get instance
				CInstance* pInst = SceneMgr::s_bucket[ bucket ][ i ].Instance();
				bool body = pInst->Body( ) && pInst->Body()->usBones;

				if ( ( pInst->DynamicLightInfluences() ) &&
					( pInst->DynamicLightFlags() & INSTANCE_LIGHTING_ENABLED ) )
				{
					if( !pInst->DynamicLightInfluences()->lightsInitialized( ) )
					{
						pInst->DynamicLightInfluences()->cacheAllLights( pInst->DynamicLightFlags() );
					}
					Vector3 vec;
					vec = ( pInst->BoundingBox().Max + pInst->BoundingBox().Min ) * 0.5f;
					pInst->DynamicLightInfluences()->getLightInfluences( vec, pInst->DynamicLightFlags(), body );
					//pInst->DynamicLightInfluences()->getDynamicInfluences( vec, pInst->DynamicLightFlags() );
					#ifndef CDROM
					pInst->DynamicLightInfluences()->drawInfluencesRays( body, vec, pInst->DynamicLightFlags() );
					#endif
				}
			}
		}
	}
}

//============================================================================

void CScene::RenderBucket( int bucket )
{
	if ( SceneMgr::IsBucketVisible( bucket ) )
	{
		int bucketCount = SceneMgr::s_bucket[ bucket ].Count();
		for ( int i = 0 ; i < bucketCount ; i++ )
		{
			// get instance
			CInstance* pInst = SceneMgr::s_bucket[ bucket ][ i ].Instance();

			// z-camera distance comes out negative, make it positive. this calculation is just the
			// z-result of putting the instance's translation vector through the world-to-camera matrix
			// ( *this is for fogging* )
			float fDistance = -((g4WorldToCamera.r02 * pInst->Matrix().tx) +
									(g4WorldToCamera.r12 * pInst->Matrix().ty) +
									(g4WorldToCamera.r22 * pInst->Matrix().tz) +
									g4WorldToCamera.tz);

			pInst->Render( fDistance );
		}
	}
}

//============================================================================
// re-render the contents of a bucket, with an additional frustum check

void CScene::RerenderBucket( int bucket, ts_Frustum* pFrustum, CInstance* pSelf )
{
	int bucketCount = SceneMgr::s_bucket[ bucket ].Count();
	for ( int i = 0 ; i < bucketCount ; i++ )
	{
		// get instance
		CInstance* pInst = SceneMgr::s_bucket[ bucket ][ i ].Instance();

		// don't render self (this is so shadow casters don't receive their own shadow)
		if ( pInst == pSelf )
			continue;

		// skip if we don't receive shadows
		if ( ShadowMgr__RenderingShadows() && !pInst->ReceiveShadows() )
			continue;

		// render if it intersects passed frustum (may not be one, in which case all instances are in view)
		bool bInView = true;
		if (pFrustum)
		{
			bInView = Frustum__IntersectsWorldBounds(
								pFrustum,
								#ifdef PS2
								NULL,	// don't worry about guard band frustum unless interested in clipping status
								#endif //PS2
								&pInst->BoundingSphere(),
								&pInst->BoundingBox(),
								NULL,
								NULL,
								pInst->FarClipDisabled() );
		}
		if (bInView == false)
			continue;

		// for shadows, update texture matrix with instance's transform matrix
		if (RenderMgr__RenderingProjectedTextures())
		{
			TextureState__RequestUnit(0);	// shadow texture is on unit 0
			MatrixStack__Mode(MS_TEXTURE);

			#ifndef DIRECTX
			MatrixStack__Push();
			MatrixStack__Multiply( &pInst->Matrix() );
			#endif
		}

		// z-camera distance comes out negative, make it positive. this calculation is just the
		// z-result of putting the instance's translation vector through the world-to-camera matrix
		// (this is for fogging)
		float fDistance = -((g4WorldToCamera.r02 * pInst->Matrix().tx) +
									(g4WorldToCamera.r12 * pInst->Matrix().ty) +
									(g4WorldToCamera.r22 * pInst->Matrix().tz) +
									g4WorldToCamera.tz);

		pInst->Render( fDistance );

		// for shadows, restore texture matrix
		if (RenderMgr__RenderingProjectedTextures())
		{
			TextureState__RequestUnit(0);	// shadow texture is on unit 0
			MatrixStack__Mode(MS_TEXTURE);

			#ifndef DIRECTX
			MatrixStack__Pop();
			#endif
		}
	}
}

//============================================================================
// loop through the visibility regions to enable/disable branches of the scene tree

void CScene::SetVisibilityRegions()
{
	if ( d_visRegionList )
	{
		int i;

		bool defaultVisibility = false;
		if ( ( SceneMgr::s_visibilityDebug == VISIBILITY_ALL ) ||
			  ( SceneMgr::s_visibilityDebug == VISIBILITY_DEBUG && ( SceneMgr::s_frame & 4 ) ) )
		{
			defaultVisibility = true;
		}

		// set all regions to default visibility
		for ( i = 0 ; i < d_visRegionList->usRecordCount ; i++ )
		{
			CVisRegion* region = (CVisRegion*) GetRecordAddress( d_visRegionList, i );
			region->GetBranch()->SetEnabled( defaultVisibility );
		}

		// loop through testing for camera within region. camera may be within multiple regions. the last region may
		// be the whole-world region (all instances that are not in any visibility region). if so, we only want to test
		// for camera's inclusion (which will always pass, since whole-world region's bounding box is set to be huge)
		// if the camera is not in any visibility region. once we find the camera included in a visibility region, we
		// will rely on whether the whole-world is in the region's visible regions list
		int limit = d_visRegionList->usRecordCount;
		for ( i = 0 ; i < limit ; i++ )
		{
			CVisRegion* region = (CVisRegion*) GetRecordAddress( d_visRegionList, i );

         // regions can be marked so that camera ignores them
			if ( region->GetFlags() & CVisRegion::CAMERA_SKIP_REGION )
				continue;

			// is camera in visibility region?
			bool cameraInRegion = region->GetOBB()->PointInBox( d_cameraWorldPos );
			if ( SceneMgr::s_visibilityDebug == VISIBILITY_FREEZE )
			{
				// use frozen camera position-- lets user fly around in free cam without affecting visibility calculation
				cameraInRegion = region->GetOBB()->PointInBox( SceneMgr::s_frozenCameraPos );
				DrawUtility::Point( SceneMgr::s_frozenCameraPos );
			}

			if ( cameraInRegion )
			{
				// enable this region
				region->GetBranch()->SetEnabled( true );

				#ifndef CONSUMER_BUILD
				if ( SceneMgr::s_visibilityDebug >= VISIBILITY_SHOW )
					region->Draw( DrawUtility::WHITE );
				#endif

				// let partition loader know about active region
				g_loadSOM.AddVisibilitySet( region );

				// enable other regions that can be seen from this region
				for ( int j = 0 ; j < (int)region->GetVisibleRegionCount() ; j++ )
				{
					region->GetVisibleRegion( j )->GetBranch()->SetEnabled( true );

					#ifndef CONSUMER_BUILD
					if ( SceneMgr::s_visibilityDebug >= VISIBILITY_SHOW )
						region->GetVisibleRegion( j )->Draw( DrawUtility::YELLOW );
					#endif //CONSUMER_BUILD
				}

				// found the camera included in a visibility region-- don't test whole-world region
				limit = d_wholeWorldIndex;
			}
		}

		// temporary hack-- for now, see all regions from rest-of-world region (camera not in any visibility region).
		// rest-of-world region not marked correctly
		if ( i == d_visRegionList->usRecordCount )
		{
			for ( int k = 0 ; k < d_visRegionList->usRecordCount ; k++ )
			{
				CVisRegion* region = (CVisRegion*) GetRecordAddress( d_visRegionList, k );
				region->GetBranch()->SetEnabled( true );
			}
		}
	}
}

//============================================================================

bool CScene::AddInstance( CInstance* inst )
{
	// scene may not have a tree?
	if ( !d_sceneTree )
		return false;

	// don't add if already there-- scenes may share a tree
	if ( FindRuntimeInstanceInSceneTree( inst ) )
		return true;

	// find the best place to insert this instance
	CSceneNode* sceneNodeToAddTo = FindBestFitVisRegion( inst );

	// add to tree at correct node
	CSceneNode* instanceNode = SceneMgr::AllocSceneNode();
	instanceNode->SetType( SCENE_NODE_INSTANCE );
	instanceNode->SetFlags( instanceNode->GetFlags() | SCENE_NODE_DYN_VIS | SCENE_NODE_IGNORE_FIRSTFRAME );
	instanceNode->SetData( inst );
	sceneNodeToAddTo->AddChild( instanceNode );

	// scene tree nodes that have any children instance nodes that are marked INSTANCE_NO_FAR_CLIP need to have
	// the SCENE_NODE_NO_FAR_CLIP flag set. this is handled in the filter chain. i'm assuming that we won't be
	// adding any no-far-clip instances by hand. if we do, though, we'll have to walk back up the parent chain
	// setting the SCENE_NODE_NO_FAR_CLIP flag
	ASSERTS( !inst->FarClipDisabled(), "Adding NO_FAR_CLIP instance to scene tree" );

	return true;
}

//============================================================================
// search the vis regions for the one that fits our box the best
CSceneNode* CScene::FindBestFitVisRegion( CInstance* inst )
{
	// default to the root if we can't find anything
	CSceneNode* bestRegionBranch = d_sceneTree;

	if ( d_visRegionList )
	{
		float bestVolume = 0.0001f;	// minimum of *some* intersection
		float bestRegionVolume = 1e20f;

		for ( int i = 0 ; i < d_wholeWorldIndex ; i++ )
		{
			CVisRegion* region = (CVisRegion*) GetRecordAddress( d_visRegionList, i );

			ts_BoundingBox Intersection;
			if ( BoundingBox__IntersectBox( region->GetBoundingBox(), &inst->BoundingBox(), &Intersection ) )
			{
				Vector3 size = Intersection.Max - Intersection.Min;
				float intersectVolume = size.x() * size.y() * size.z();
				if ( intersectVolume >= bestVolume )
				{
					size = region->GetBoundingBox()->Max - region->GetBoundingBox()->Min;
					float regionVolume = size.x() * size.y() * size.z();

					if ( ( intersectVolume > bestVolume ) || ( regionVolume < bestRegionVolume ) )
					{
						bestVolume       = intersectVolume;
						bestRegionVolume = regionVolume;
						bestRegionBranch = region->GetBranch();
					}
				}
			}
		}
	}

	return bestRegionBranch;
}

//============================================================================
// search the scene tree for a "runtime instance" (one not originally bound
// to the world).  it will be directly off the root or hanging right below
// one of the visibility regions.

CSceneNode* CScene::FindRuntimeInstanceInSceneTree( CInstance* inst )
{
	if ( d_sceneTree )
	{
		// first look right off root
		CSceneNode* node = d_sceneTree->SearchChildren( inst );
		if ( node )
			return node;

		// then look in visibility regions
		if ( d_visRegionList )
		{
			for ( int i = 0 ; i < d_visRegionList->usRecordCount ; i++ )
			{
				CVisRegion* region = (CVisRegion*) GetRecordAddress( d_visRegionList, i );
				node = region->GetBranch()->SearchChildren( inst );
				if ( node )
					return node;
			}
		}
	}

	// not found
	return NULL;
}

//============================================================================

void CScene::RemoveInstance( CInstance* inst )
{
	// look for node that points to instance
	CSceneNode* instanceNode = FindRuntimeInstanceInSceneTree( inst );
	if ( instanceNode )
	{
		// remove from tree
		instanceNode->Remove();

		// node is available
		instanceNode->SetAllocated( false );
	}
}

//============================================================================

void CScene::VisRegionUpdate()
{
	int count = 0;

	// move dynamic instances to the best fit vis region
	if ( d_visRegionList && d_wholeWorldIndex > 0 )
	{
		int i;
		CSceneNode* n;

		// mark dynamic-vis instances as processed, so we deal with them once as we move them
		for ( n = d_sceneTree->GetFirstChild() ; n ; n = n->GetNextSibling() )
			if ( ( n->GetFlags() & SCENE_NODE_DYN_VIS ) && !( (CInstance*)n->GetData() )->Hidden() )
				n->SetProcessed( false ), count++;
		for ( i = 0 ; i < d_wholeWorldIndex ; i++ )
		{
			CVisRegion* region = (CVisRegion*) GetRecordAddress( d_visRegionList, i );
			for ( n = region->GetBranch()->GetFirstChild() ; n ; n = n->GetNextSibling() )
				if ( ( n->GetFlags() & SCENE_NODE_DYN_VIS ) && !( (CInstance*)n->GetData() )->Hidden() )
					n->SetProcessed( false ), count++;
		}

		// try to place all dyn vis in the best vis regions
		VisRegionBranchUpdate( d_sceneTree );
		for ( i = 0 ; i < d_wholeWorldIndex ; i++ )
		{
			CVisRegion* region = (CVisRegion*) GetRecordAddress( d_visRegionList, i );
			VisRegionBranchUpdate( region->GetBranch() );
		}
	}
}

//============================================================================

void CScene::VisRegionBranchUpdate( CSceneNode* branch )
{
	CSceneNode* next;
	for ( CSceneNode* n = branch->GetFirstChild() ; n ; n = next )
	{
		next = n->GetNextSibling();
		if ( ( n->GetFlags() & SCENE_NODE_DYN_VIS ) && !n->IsProcessed() )
		{
			CInstance* inst = (CInstance*) n->GetData();
			if ( !inst->Hidden() )
			{
				n->SetProcessed( true );
//				DrawUtility::Box( inst->BoundingBox().Min, inst->BoundingBox().Max, NULL, NULL, DrawUtility::BLUE );
				CSceneNode* bestFit = FindBestFitVisRegion( inst );
				if ( bestFit != branch )
				{
					n->Remove();
					bestFit->AddChild( n );
				}
			}
		}
	}
}

//============================================================================

void CScene::SetInstanceVisRegionStatic( CInstance* inst, bool useWorld )
{
	CSceneNode* node = FindRuntimeInstanceInSceneTree( inst );
	ASSERT( node );
	if ( node )
	{
		// clear dynamic vis-region bit
		node->SetFlags( node->GetFlags() & ~SCENE_NODE_DYN_VIS );

		// remove from current region and place in best one
		CSceneNode* branch = ( useWorld ) ? d_sceneTree : FindBestFitVisRegion( inst );
		node->Remove();
		branch->AddChild( node );
	}
}

//============================================================================

void CScene::SetInstanceVisRegionDynamic( CInstance* inst )
{
	CSceneNode* node = FindRuntimeInstanceInSceneTree( inst );
	ASSERT( node );
	if ( node )
	{
		// set dynamic vis-region bit
		node->SetFlags( node->GetFlags() | SCENE_NODE_DYN_VIS );
	}
}

//============================================================================
