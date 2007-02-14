/******************* includes ****************************/

#include "TTLPCH.h"
#include "sim/hud.h"
#include "Layers/MemoryConfig.h"
#include "Network/Connection.h"

#if defined(PS2)
#include "Display/PS2/Renderer/Code/MasterList.h"
#endif

#include <netz.h>

#if defined(_XBOX)
	#define STAT_WINDOW_SPACE	( 24.0f )
	#define STAT_WINDOW_X		( 48.0f )
	#define STAT_WINDOW_Y		( 496.0f - ( NUM_STATS_LINES * STAT_WINDOW_SPACE ) )

int	g_DXFreeWatermark = 64000000;
int	g_HeapFreeWatermark = 64000000;

#else
	#define STAT_WINDOW_SPACE	( 24.0f )
	#define STAT_WINDOW_X		( 8.0f  )
	#define STAT_WINDOW_Y		( 472.0f - ( NUM_STATS_LINES * STAT_WINDOW_SPACE ) )
#endif

#define SETCOLOR()	SetColor( 0.9f, 0.9f, 0.9f, 0.8f )

#if defined(PS2)
extern int stackcheck();
#endif

//============================================================================

enum
{
	DEBUG_PANEL_NONE,
	DEBUG_PANEL_NETWORK,
	DEBUG_PANEL_MEMORY,
	DEBUG_PANEL_RENDER,
	DEBUG_PANEL_INSTANCE,
	DEBUG_PANEL_AI,
	DEBUG_PANEL_COLLISION,
};

//============================================================================

GameHud::GameHud( void )
{
}

//============================================================================

GameHud::~GameHud()
{
	Shutdown();
}

//============================================================================

void GameHud::Shutdown( void )
{
}

//============================================================================

void GameHud::Initialize( void )
{
#ifdef ALLOW_DEBUG_MENU
	m_Debug.Initialize();
#endif // ALLOW_DEBUG_MENU
}

//============================================================================

void GameHud::Update( void )
{
	Hud::Update();

#ifdef ALLOW_DEBUG_MENU
	m_Debug.Update();
#endif // ALLOW_DEBUG_MENU
}

//============================================================================

GameDebug::GameDebug( void )
{
}

//============================================================================

GameDebug::~GameDebug()
{
}

//============================================================================

void GameDebug::Initialize( void )
{
#ifdef ALLOW_DEBUG_MENU
	m_Menu.LoadTextureSet( "shell\\shell.dbl" );
	m_Menu.InitMenu();

	AddChild( &m_FPS );
	for (int c = 0; c < NUM_COORD_LINES; c++)
	{
		AddChild( &m_Coordinates[c] );
		m_Coordinates[c].SetTextHeight( 22.0f );
		m_Coordinates[c].SetOrigin( 1.0f, 0.0f );
		m_Coordinates[c].SetPosition( 600.0f, 50.0f + (float)c * 26.0f);
		m_Coordinates[c].ShadowOn();
		m_Coordinates[c].SETCOLOR();
	}

	m_FPS.SetTextHeight( 20.0f );
	m_FPS.SetOrigin( 1.0f, 0.0f );
	m_FPS.SetPosition( 612.0f, 18.0f );
	m_FPS.ShadowOn();
	m_FPS.SETCOLOR();

	int i;
	for ( i = 0 ; i < NUM_STATS_LINES ; i++ )
	{
		AddChild( &m_StatsText[ i ] );
		m_StatsText[ i ].SetTextHeight( 20.0f );
		m_StatsText[ i ].SetPosition( STAT_WINDOW_X, STAT_WINDOW_Y + (float)i * STAT_WINDOW_SPACE );
		m_StatsText[ i ].SetTextFixedSpacing( 16.0f );
	}

	ShowPanel( DEBUG_PANEL_NONE );
	ShowCoordinates( false );

#ifdef _DEBUG
	ShowFPS( true );
#else
	ShowFPS( false );
#endif
#endif
}

//============================================================================

void GameDebug::ShowPanel( int panel )
{
#ifdef ALLOW_DEBUG_MENU
	m_Panel = panel;

	if ( panel == DEBUG_PANEL_NONE )
	{
		for ( int i = 0 ; i < NUM_STATS_LINES ; i++ )
		{
			m_StatsText[ i ].Hide();
		}
	}
	else
	{
		for ( int i = 0 ; i < NUM_STATS_LINES ; i++ )
		{
			m_StatsText[ i ].Show();
			m_StatsText[ i ].SetText( "" );
		}
	}
#endif
}

//============================================================================

void GameDebug::UpdatePanelMemory()
{
#ifdef ALLOW_DEBUG_MENU
	#if defined (_XBOX)
	MEMORYSTATUS stat;
	GlobalMemoryStatus(&stat);
	#endif

	int freeBytes, freeHandles, largestFree, allocBytes, allocHandles;

	if ( LayersState.pu8LocalMemory )
	{
		Layers__GetMemoryStats( &freeBytes, &freeHandles, &largestFree, &allocBytes, &allocHandles );

		#if defined(PS2)

		u32 iopMemory = sceSifQueryTotalFreeMemSize();
		u32 network = ValidSingleton(CConnection) ? g_connection.GetMemoryUsage() : 0;
		u32 sysFree  = Layers__SystemRAMAvailable(64*1024*1024);
		extern int osiHandles;
		m_StatsText[0].SetText(Printf("Heap:  %8d(%4dk), IOP:%dk STACK:%dk", Layers__GetMemorySize(), Layers__GetMemorySize() / 1024, iopMemory / 1024, stackcheck() / 1024));
		m_StatsText[1].SetText(Printf("Free:  %8d(%4dk), %d hnd, SY:%dk", freeBytes, freeBytes / 1024, freeHandles, sysFree / 1024));
		m_StatsText[2].SetText(Printf("LFree: %8d(%4dk,%.0f%%)", largestFree, largestFree / 1024, (float)largestFree / (float)freeBytes * 100.0f));
		m_StatsText[3].SetText(Printf("Alloc: %8d(%4dk), %d hnd, Net:%dk, OSI:%d hnd", allocBytes, allocBytes / 1024, allocHandles, network / 1024, osiHandles));

		#elif defined(_XBOX)
		m_StatsText[0].SetText(Printf("Heap:  %8d(%4dk)", Layers__GetMemorySize(), Layers__GetMemorySize() / 1024));
		m_StatsText[1].SetText(Printf("Free:  %8d(%4dk), %d hnd", freeBytes, freeBytes / 1024, freeHandles));
		m_StatsText[2].SetText(Printf("DX Heap: %d,  DX Free: %d", g_MemoryConfig.OSReserve, stat.dwAvailPhys));
		m_StatsText[3].SetText(Printf("LFree: %8d(%4dk,%.0f%%)", largestFree, largestFree / 1024, (float)largestFree / (float)freeBytes * 100.0f));
		m_StatsText[4].SetText(Printf("Alloc: %8d(%4dk), %d hnd", allocBytes, allocBytes / 1024, allocHandles));
		#else

		m_StatsText[0].SetText(Printf("Heap:  %8d(%4dk)", Layers__GetMemorySize(), Layers__GetMemorySize() / 1024));
		m_StatsText[1].SetText(Printf("Free:  %8d(%4dk), %d hnd", freeBytes, freeBytes / 1024, freeHandles));
		m_StatsText[2].SetText(Printf("LFree: %8d(%4dk,%.0f%%)", largestFree, largestFree / 1024, (float)largestFree / (float)freeBytes * 100.0f));
		m_StatsText[3].SetText(Printf("Alloc: %8d(%4dk), %d hnd", allocBytes, allocBytes / 1024, allocHandles));

		#endif
	}

	#if defined (_XBOX)
	if (stat.dwAvailPhys < g_DXFreeWatermark)
		g_DXFreeWatermark = stat.dwAvailPhys;

	if (freeBytes < g_HeapFreeWatermark)
		g_HeapFreeWatermark = freeBytes;
	#endif

#endif


}

//============================================================================

void GameDebug::UpdatePanelNetwork()
{
#ifdef ALLOW_DEBUG_MENU
	if ( ValidSingleton(CConnection) )
	{
		m_StatsText[0].SetText(Printf( "BPS received avg  :%06d,  BPS sent avg :%06d", g_connection.GetBPSSentAvg( ), g_connection.GetBPSReceivedAvg( ) ) );
		m_StatsText[1].SetText(Printf( "BPS received max  :%06d,  BPS sent max :%06d", g_connection.GetBPSReceivedMax( ), g_connection.GetBPSSentMax( ) ) );
		m_StatsText[2].SetText(Printf( "BPS received cur  :%06d,  BPS sent cur :%06d", g_connection.GetBPSReceivedCur( ), g_connection.GetBPSSentCur( ) ) );
		m_StatsText[3].SetText(Printf( "Packets received  :%06d,  Packets sent :%06d", g_connection.GetPacketsReceived( ), g_connection.GetPacketsSent( ) ) );
		m_StatsText[4].SetText(Printf( "Packets trans     :%06d,  Packets retr :%06d", g_connection.GetReliableTransmittedTotal( ), g_connection.GetReliableRetransmittedTotal( ) ) );
		m_StatsText[5].SetText(Printf(""));
	}
#endif
}

//============================================================================

void GameDebug::UpdatePanelCollision(void)
{
#ifdef ALLOW_DEBUG_MENU
#ifdef COLLISION_SYSTEM_STATS
	if(ValidSingleton(CollisionSystem))
	{
		m_StatsText[0].SetText(Printf( "Tests/Frame:%06d  OBBvsTri:%06d AABBvsTri:%06d LinevsTri:%06d", g_collisionSystem.GetTotalTestCount(),
												 CTerrainOBBIterator::triangleCount,
												 CTerrainAABBIterator::triangleCount,
												 CTerrainLineIterator::triangleCount));
		// Icky.  I shouldnt be clearing this here... but today Im lazy, and it works...
		CTerrainOBBIterator::triangleCount = 0;
		CTerrainAABBIterator::triangleCount = 0;
		CTerrainLineIterator::triangleCount = 0;
	}
	else
#endif
		m_StatsText[0].SetText(Printf(""));

#ifdef CLUSTER_SYSTEM_STATS
	if(ValidSingleton(Stage))
	{
		m_StatsText[1].SetText(Printf( "Cluster Sort Swaps Per Frame :%06d", 
												 g_stage.GetClusterSystem().GetSwapsThisFrame() ));
		m_StatsText[2].SetText(Printf( "Cluster elements NonInteract:%03d Seed:%03d Depnd:%03d", 
												 g_stage.GetClusterSystem().GetNonInteractiveElements(), 
												 g_stage.GetClusterSystem().GetSeedElements(), 
												 g_stage.GetClusterSystem().GetDependantElements() ));

		CCluster* cluster = g_stage.GetClusterSystem().ClusterHead();
		int clustercount = 0;
		int clusterfiends = 0;
		while(cluster)
		{
			clustercount++;
			clusterfiends += cluster->Members();
			cluster = cluster->next;
		}

		
		m_StatsText[3].SetText(Printf("Clusters:%03d (%03d clstrs)  Scat0:%03d S1:%03d", 
												clusterfiends,
												clustercount,
												g_stage.GetClusterSystem().Scatter(0).Members(),
												g_stage.GetClusterSystem().Scatter(1).Members()));

		m_StatsText[4].SetText(Printf("Time: %5dms", g_stage.GetClusterSystem().GetTimeBuildClusters()));
	}
#endif
#endif
}


//============================================================================

void GameDebug::UpdatePanelRender()
{
#ifdef ALLOW_DEBUG_MENU
	static u32 lastTime = 0;
	static u32 lastTotalPolysPerSecond = 0;
	static u32 curTotalPolysPerSecond = 0;

	CSceneStats& stats = SceneMgr::GetStats();

#if defined(PS2)

	#define MICROSEC_PER_FIELD ((1 / 59.94f) * 1000000)

	float			RenderuS = stats.RenderuSec( );
	float			RenderFields = RenderuS / MICROSEC_PER_FIELD;
	unsigned int	RenderHz = 60 / (unsigned int) Math::Ceil( RenderFields);
	
	float			CPUus = stats.ProcessuSec( ) - stats.ProcessSleepuSec( );
	float			CPUFields = CPUus / MICROSEC_PER_FIELD;
	unsigned int	CPUHz = (unsigned int) Math::Ceil(60 / CPUFields);
	
	unsigned int	vtxs = stats.Vertices( );
	unsigned int	vtxs_frm = (unsigned int) ((float) vtxs / RenderFields);
	unsigned int	vtxs_sec = vtxs_frm * 60;
	
	float Stallus = stats.ProcessBlockeduSec();
	float Waitus = stats.ProcessWaituSec();

	m_StatsText[0].SetText( Printf( "Txtr: %3ld (%7d) TC: %3d/%3d Inst:%3d/%3d Prtcl:%3d/%3d",
			                           stats.TextureLoadCount(), stats.TextureLoadBytes(),
												TextureMgr::Cache().Hits(), TextureMgr::Cache().Misses(),
												stats.InstancesRendered(), stats.InstancesTotal(),
											   stats.ParticlesDrawn(), stats.ParticlesUpdated() ) );
	m_StatsText[1].SetText( Printf( " CPU: %6ld uS  %1.2f frames  %2d Hz  %4ld bones", (unsigned int) CPUus, CPUFields, CPUHz, stats.BonesComputed() ));
	m_StatsText[2].SetText( Printf( "Rend: %6ld uS  %1.2f frames  %2d Hz  %4ld bones", (unsigned int) RenderuS, RenderFields, RenderHz, stats.BonesRendered()) );
	m_StatsText[3].SetText( Printf( "Stal: %6ld uS  %6ld uS    Chnk:%3d/%3d/%3d", (unsigned int)Stallus, (unsigned int)Waitus,
											  MasterList::ChunksUsedLastFrame(), 
											  MasterList::CurrAllocatedChunks() - MasterList::MinChunksFree(),
											  MasterList::CurrAllocatedChunks()));
	m_StatsText[4].SetText( Printf( "Vtxs: %7ld   %7ld/frame   %7ld/sec  ",vtxs, vtxs_frm, vtxs_sec) );
	m_StatsText[5].SetText( Printf( "CtxSwaps  /Frame:%04d Max/Model:%04d",  stats.ContextChangesPerFrame(), stats.MaxContextChangesPerModel() ) );

	TextureMgr::Cache().ClearStats();

#else

	int strips = stats.TriStrips();
	u32 stripPolys = stats.TriStripPolys();
	float pps = (stripPolys == 0) ? 0 : (float)stripPolys / (float)strips;	// prevent divide by 0
	
	//tally polys over each 1 second interval for display
	curTotalPolysPerSecond += stripPolys;
	if( (EngineTimer__Time() - lastTime) > 1000 )
	{
		lastTime = EngineTimer__Time();
		lastTotalPolysPerSecond = curTotalPolysPerSecond;
		curTotalPolysPerSecond = 0;
	}
	
	/* leave lines 0 and 1 blank */
	m_StatsText[2].SetText( Printf("Strips/Plys/APPS/PPS: %5ld/%8ld/%3.1f/%8ld", strips, stats.TriStripPolys(), pps, lastTotalPolysPerSecond ) );
	m_StatsText[3].SetText( Printf("Bones Comp/Updt/Rend: %4ld/%4ld/%4ld", stats.BonesComputed(), stats.BonesUpdated(), stats.BonesRendered() ) );
	m_StatsText[4].SetText( Printf("Instances %3d/%3d", stats.InstancesRendered(), stats.InstancesTotal() ) );

#endif
#endif
}

//============================================================================

void GameDebug::UpdatePanelInstance()
{
#ifdef ALLOW_DEBUG_MENU
	CSceneStats& stats = SceneMgr::GetStats();

	m_StatsText[0].SetText( Printf( "Instances Rendered: %3d/%3d    Hidden:%3d", stats.InstancesRendered(), stats.InstancesTotal(), stats.InstancesHidden() ) );
	m_StatsText[1].SetText( Printf( " Vis Region Culled: %3d", stats.InstancesVisRegionOff() ) );
	m_StatsText[2].SetText( Printf( " Scene Node Culled: %3d", stats.InstancesSceneBoxFrustumCulled() ) );
	m_StatsText[3].SetText( Printf( "    Frustum Culled: %3d", stats.InstancesFrustumCulled() ) );
	m_StatsText[4].SetText( Printf( "          Occluded: %3d", stats.InstancesOccluded() ) );
#endif
}

//============================================================================

void GameDebug::UpdatePanelAI()
{
#ifdef ALLOW_DEBUG_MENU
//	m_StatsText[0].SetText( "" );
//	m_StatsText[1].SetText( "" );
//	m_StatsText[2].SetText( "" );
//	m_StatsText[3].SetText( "" );
	m_StatsText[4].SetText( Printf( "NPCS:%2d", g_referee.GetNumNonPlayers() ) );
#endif
}

//============================================================================

void GameDebug::UpdateFPS()
{
#ifdef ALLOW_DEBUG_MENU
	// Set the frames per seconds
	m_FPS.SetText( Printf( "fps %3.0f", 1.0f / SecondsPerFrame ) );
#endif
}

//============================================================================

void GameDebug::UpdateCoordinates()
{
#ifdef ALLOW_DEBUG_MENU
	if ( ValidSingleton( CPlayers ) )
	{
		// Set Avatar #1's coordinates
		if ( g_players.GetPlayer( 0 ) )
		{
			Vector3 pos = g_players.GetPlayer(0)->GetBodyInWorld();
			m_Coordinates[0].SetText( Printf("Avatar: X=%.2f Y=%.2f Z=%.2f", (float)pos.x(), (float)pos.y(), (float)pos.z()) );
		}
	}

	// --- DAS, crash bug fix, don't know why g_cameraDirectorPtr was NULL, but it was.
	if( g_cameraDirectorPtr )
	{
		Camera &camera = g_cameraDirector.CurrentCamera();
		Vector3 eye = camera.Eye();
		m_Coordinates[1].SetText( Printf("Eye: X=%.2f Y=%.2f Z=%.2f", (float)eye.x(), (float)eye.y(), (float)eye.z()) );
		float fov = camera.FOV();
		float hither = camera.Hither();
		float yon = camera.Yon();
		m_Coordinates[2].SetText( Printf("Params: FOV=%.1f hither=%.1f yon=%.1f", fov, hither, yon) );
	}
#endif
}

//============================================================================

void GameDebug::Update( void )
{	
#ifdef ALLOW_DEBUG_MENU
	if ( m_ShowFPS )
	{
		UpdateFPS();
	}
	
	if ( m_ShowCoordinates )
	{
		UpdateCoordinates();
	}

	switch ( m_Panel )
	{
		case DEBUG_PANEL_NONE:		break;
		case DEBUG_PANEL_RENDER:	UpdatePanelRender();			break;
		case DEBUG_PANEL_NETWORK:	UpdatePanelNetwork();		break;
		case DEBUG_PANEL_MEMORY:	UpdatePanelMemory();			break;
		case DEBUG_PANEL_INSTANCE: UpdatePanelInstance();		break; 
		case DEBUG_PANEL_AI:			UpdatePanelAI();				break;
		case DEBUG_PANEL_COLLISION:UpdatePanelCollision();		break;
	}
	
	Hud::Update();
#endif
}

//============================================================================

void GameDebug::WriteDebugMessage( int line, const char *Message )
{
#ifdef ALLOW_DEBUG_MENU
	m_DebugText[line].SetText( Message );
#endif
}

//============================================================================

void GameDebug::ToggleFPS( void )
{
#ifdef ALLOW_DEBUG_MENU
	ShowFPS( !m_ShowFPS );
#endif
}

//============================================================================

void GameDebug::ShowFPS( bool TrueOrFalse )
{
#ifdef ALLOW_DEBUG_MENU
	m_ShowFPS = TrueOrFalse;

	if( m_ShowFPS )
	{
		m_FPS.Show();
	}
	else
	{
		m_FPS.Hide();
	}
#endif
}

//============================================================================

void GameDebug::ToggleCoordinates( void )
{
#ifdef ALLOW_DEBUG_MENU
	ShowCoordinates( !m_ShowCoordinates );
#endif
}

//============================================================================

void GameDebug::ShowCoordinates( bool TrueOrFalse )
{
#ifdef ALLOW_DEBUG_MENU
	m_ShowCoordinates = TrueOrFalse;

	for (int c = 0; c < NUM_COORD_LINES; c++)
	{
		if( m_ShowCoordinates )
			m_Coordinates[c].Show();
		else
			m_Coordinates[c].Hide();
	}
#endif
}

//============================================================================

void GameDebug::ToggleNetworkStats()
{
#ifdef ALLOW_DEBUG_MENU
	ShowNetworkStats( m_Panel != DEBUG_PANEL_NETWORK );
#endif
}

//============================================================================

void GameDebug::ShowNetworkStats( bool TrueOrFalse )
{
#ifdef ALLOW_DEBUG_MENU
	if ( TrueOrFalse )
	{
		if ( ValidSingleton(CConnection) )
			g_connection.ResetBPSCounters();

		ShowPanel( DEBUG_PANEL_NETWORK );
	}
	else
	{
		ShowPanel( DEBUG_PANEL_NONE );
	}
#endif
}

//============================================================================

void GameDebug::ToggleRenderStats()
{
	ShowRenderStats( m_Panel != DEBUG_PANEL_RENDER );
}

//============================================================================

void GameDebug::ShowRenderStats( bool TrueOrFalse )
{
#ifdef ALLOW_DEBUG_MENU
	if ( TrueOrFalse )
	{
		ShowPanel( DEBUG_PANEL_RENDER );
		ShowFPS( true );
	}
	else
	{
		ShowPanel( DEBUG_PANEL_NONE );
		ShowFPS( false );
	}
#endif
}

//============================================================================

void GameDebug::ToggleMemoryStats()
{
#ifdef ALLOW_DEBUG_MENU
	ShowMemoryStats( m_Panel != DEBUG_PANEL_MEMORY );
#endif
}

//============================================================================

void GameDebug::ShowMemoryStats( bool TrueOrFalse )
{
#ifdef ALLOW_DEBUG_MENU
	if ( TrueOrFalse )
	{
		ShowPanel( DEBUG_PANEL_MEMORY );
	}
	else
	{
		ShowPanel( DEBUG_PANEL_NONE );
	}
#endif
}

//============================================================================

void GameDebug::ToggleInstanceStats()
{
	ShowInstanceStats( m_Panel != DEBUG_PANEL_INSTANCE );
}

//============================================================================

void GameDebug::ShowInstanceStats( bool TrueOrFalse )
{
#ifdef ALLOW_DEBUG_MENU
	if ( TrueOrFalse )
	{
		ShowPanel( DEBUG_PANEL_INSTANCE );
	}
	else
	{
		ShowPanel( DEBUG_PANEL_NONE );
	}
#endif
}

//============================================================================

void GameDebug::ToggleAIStats()
{
#ifdef ALLOW_DEBUG_MENU
	ShowAIStats( m_Panel != DEBUG_PANEL_AI );
#endif
}

//============================================================================

void GameDebug::ShowAIStats( bool TrueOrFalse )
{
#ifdef ALLOW_DEBUG_MENU
	if ( TrueOrFalse )
	{
		ShowPanel( DEBUG_PANEL_AI );
	}
	else
	{
		ShowPanel( DEBUG_PANEL_NONE );
	}
#endif
}

//============================================================================

void GameDebug::ToggleCollisionStats()
{
#ifdef ALLOW_DEBUG_MENU
	ShowCollisionStats( m_Panel != DEBUG_PANEL_COLLISION );
#endif
}

//============================================================================

void GameDebug::ShowCollisionStats( bool TrueOrFalse )
{
#ifdef ALLOW_DEBUG_MENU
	if ( TrueOrFalse )
	{
		ShowPanel( DEBUG_PANEL_COLLISION );
	}
	else
	{
		ShowPanel( DEBUG_PANEL_NONE );
	}
#endif
}

//============================================================================


