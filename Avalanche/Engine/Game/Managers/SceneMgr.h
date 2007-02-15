//============================================================================
//=
//= SceneMgr.h
//=
//= The scene manager keeps track of scenes.  A scene is the a scene tree to
//= be considered for rendering, as well as a camera and viewport
//=
//= Multiple scenes may be used to render split screen, for example.  Scenes
//= could be added to render a particular view into a texture that is later
//= applies to something in the game (a TV screen, perhaps, or enviromental
//= maps from a cube map).
//=
//= The scene manager determines each frame which instances are visible by
//= doing view frustum checks.  It does not take advange of frame to frame
//= coherency.
//=
//= The instances for each scen are divided into groups based on render flags.
//= Those groups are RENDER_FIRST, OPAQUE, TRANSPARENT, RENDER_LAST.
//= These groups are rendered in this order, and instances within a group
//= are sorted as needed.
//=
//============================================================================

#ifndef __SCENEMGR_H
#define __SCENEMGR_H

#include "Layers/LayersStruct.h"
#include "Layers/DLinkList.h"
#include "Layers/SLinkList.h"
#include "Display/Common/SceneView.h"

//============================================================================

class CInstance;
struct ts_Group;
struct ts_Frustum;
struct ts_PatchSOM;
struct ts_BoneDisplayListRecord;
struct ts_RecordList;
class CSceneNode;
class BaseMenu;
class Texture;
class NodeSortElement;
class COccluderMgr;

//============================================================================
// number of rendering "buckets." each bucket holds instances of a particular type (e.g. opaque) that
// are collected for rendering by walking the scene tree. some buckets need to be sorted (e.g. transparent).
// this is handled by having the bucket contain multiple linked list heads, and instances are placed into
// the correct list within the bucket by distance. this works a lot like the old ordering tables on the ps1,
// except it's by instance instead of poly

enum
{
	// important-- this is in the order that the buckets should be rendered
	DRAW_FIRST_BUCKET = 0,
	OPAQUE_BUCKET,
	TRANSPARENT_BUCKET,
	DRAW_LAST_BUCKET,

	MAX_BUCKETS		// must be last
};

//============================================================================
// scene types-- not all scenes are for generating a game view

enum
{
	SCENE_DISPLAY = 0,				// scene is rendered to frame buffer and shown to the player
	SCENE_RENDER_INTO_TEXTURE,		// render into a texture to be used when rendering display scene

	// must be last
	SCENE_TYPES,
	SCENE_TYPE_ALL = SCENE_TYPES	// for SceneMgr::GetFirstScene, SceneMgr::GetNextScene
};

//============================================================================

// for adding instances to scene tree. indices 0..<vis region count - 1> refer to the vis region branches
#define SCENE_TREE_BRANCH_UNKNOWN	-2	// an initialization value that means we haven't tried to add to scene tree yet
#define SCENE_TREE_ROOT					-1

//============================================================================
// STATS

#define GAME_STATS		// remove to stop accumulating render stats

#ifdef GAME_STATS
	#define SCENE_STAT( name ) \
		int d_ ## name; \
		inline int  name() const { return d_ ## name;      } \
		inline void Inc ## name( int v ) { d_ ## name += v; }
#else
	#define SCENE_STAT( name ) \
		inline int  name() const { return 0; } \
		inline void Inc ## name( int v ) {}
#endif

class CSceneStats
{
public:
	SCENE_STAT( InstancesRendered )
	SCENE_STAT( InstancesOccluded )
	SCENE_STAT( InstancesHidden )
	SCENE_STAT( InstancesVisRegionOff )
	SCENE_STAT( InstancesSceneBoxFrustumCulled )
	SCENE_STAT( InstancesFrustumCulled )
	SCENE_STAT( InstancesTotal )
	SCENE_STAT( Vertices )
	SCENE_STAT( Polys )
	SCENE_STAT( ClippedPolys )
	SCENE_STAT( UnclippedPolys )
 	SCENE_STAT( TriStrips )
	SCENE_STAT( TriStripPolys )
	SCENE_STAT( SphereCull )
	SCENE_STAT( BoxCull )
	SCENE_STAT( TextureLoadCount )
	SCENE_STAT( TextureLoadBytes )
	SCENE_STAT( RenderuSec )
	SCENE_STAT( ProcessuSec )
	SCENE_STAT( ProcessWaituSec )
	SCENE_STAT( ProcessBlockeduSec )
	SCENE_STAT( ProcessSleepuSec )
	SCENE_STAT( ShadowTriangles )
	SCENE_STAT( ShadowTrianglesInFrustum )
	SCENE_STAT( BonesComputed )
	SCENE_STAT( BonesUpdated )
	SCENE_STAT( BonesRendered )
	SCENE_STAT( ParticlesDrawn )
	SCENE_STAT( ParticlesUpdated )
	SCENE_STAT( ContextChangesPerFrame )
	SCENE_STAT( MaxContextChangesPerModel )
	void Clear() { memset( this, 0, sizeof(*this) ); }
};

//============================================================================
// scene instance handler. there can be multiple scenes rendered per frame

class CScene
{
public:
	void Render( bool bReflection = false );
	void RenderBucket( int bucket ) NAMED_SECTION( "text.rend1" );
	void RerenderBucket( int bucket, ts_Frustum* frustum, CInstance* self );
	void SetVisibilityRegions();
	void ComputeLightInfluences();
	void EnableForRendering( bool enable );
	bool GetScreenCoords( Vector3CRef worldPos, Vector2 &screenCoords );
	bool AddInstance( CInstance* inst );
	void RemoveInstance( CInstance* inst );
	void SetInstanceVisRegionStatic( CInstance* inst, bool useWorld );
	void SetInstanceVisRegionDynamic( CInstance* inst );
	void VisRegionUpdate();
	void VisRegionBranchUpdate( CSceneNode* branch );
	CSceneNode* FindRuntimeInstanceInSceneTree( CInstance* inst );
	CSceneNode* FindBestFitVisRegion( CInstance* inst );

	// world to camera matrix
	Graphics4x4*		d_worldToCamera;

	// scene view to use for scene. this controls stuff like field of view. near and far clip, background color, fog.
	// each scene gets its own record. at scene registration, this record is filled in with the default values from
	// the DisplayState record (which are typically set by processing the light.var file)
	CSceneView			d_sceneView;

	// inverse of WorldToCamera matrix, camera position in world
	Graphics4x4			d_cameraToWorld;
	Vector3				d_cameraWorldPos;

	// pointer to root of scene tree to traverse for this scene
	CSceneNode*			d_sceneTree;

	// pointer to list of visibility regions to use for this scene. typically comes from DB_VIS_REGIONS chunk that
	// comes from same dbl as scene tree
	ts_RecordList*		d_visRegionList;
	int					d_wholeWorldIndex;

	// scene type (game view, render-into-texture, etc)
	int					d_type;
	Texture*				d_realtimeTexture;

	// status
	bool					d_enabledForRendering;
};

//============================================================================
// scene manager is a list of registered scenes, and a pool of instance records to be shared by all scenes

namespace SceneMgr
{
	void			Init( int maxSceneNodes );
	void			Reset();
	void			Shutdown();

	CScene*		RegisterScene( Graphics4x4* pg4WorldToCamera, int nType = SCENE_DISPLAY, Texture* pRealtimeTexture = NULL );
	void			KillScene( CScene* scene );
	void			Render();
#ifdef DIRECTX_PC
	void			RenderWithoutSetup();
#endif
	void			RerenderScene(CScene* pScene, ts_Frustum* pFrustum, CInstance* pSelf);
	CScene*		GetFirstScene( int type = SCENE_DISPLAY );
	CScene*		GetNextScene( CScene* scene, int nType = SCENE_DISPLAY );
	CSceneView*	GetCurrentSceneView();
	void			SetSceneTree(CSceneNode* pSceneTree, ts_RecordList* pVisRegions, CScene* scene = NULL );	// if no scene, set for all scenes
	CSceneNode*	EmptySceneTree();
	bool			AddInstance( CInstance* inst );
	void			RemoveInstance( CInstance* pInst );
	void			RemoveAllInstances();
	ts_Frustum*	GetFirstViewFrustum();
	ts_Frustum*	GetNextViewFrustum();
	void			SetInstanceVisRegionStatic( CInstance* inst, bool useWorld );
	void			SetInstanceVisRegionDynamic( CInstance* inst );

	void			DumpInstanceList();
	CInstance*	GetInstanceByName( const char* name );

	COccluderMgr& GetOccluderMgr();

	void			ResetDisplayLists();

	void			RegisterDebugMenus( BaseMenu* menu );

	void			FoundInstanceNode(CSceneNode* pSceneNode);
	void			FoundInstanceNodeSceneBoxFrustumCulled(CSceneNode* pSceneNode);
	void			FoundInstanceNodeVisRegionOff(CSceneNode* pSceneNode);

	extern u32 s_frame;
	inline u32 GetFrameNumber() { return s_frame; }

	extern CSceneStats  s_stats;
	inline CSceneStats& GetStats() { return s_stats; }

	extern CScene* s_currentScene;
	inline CScene* GetCurrentScene() { return s_currentScene; }

	extern float   s_cameraYRotation;
	inline float   GetCameraYRotation() { return s_cameraYRotation; }
};

//============================================================================

#endif // __SCENEMGR_H
