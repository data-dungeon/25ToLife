/*************************************/
/*                                   */
/*   GameEngine.c                    */
/*   new engine  04/11/00            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   game engine state and init      */
/*                                   */
/*************************************/

/* initialize the game engine, colletcing all the initialization into one spot. define a record that contains all
default values and provide macros to update them */

/******************* includes ****************************/

#include "Game/GamePCH.h"
#ifdef DIRECTX_PC
#include "platform/PlatformPCH.h"
#endif



#include "Layers/Timing.h"
#include "EngineHelper/timer.h"
#include "Layers/exception/exception.h"
#include "Game/Managers/ShadowMgr.h"
#include "hud/animator.h"
#include "Script/ScriptSystem.h"
#include "cmdcon/cmdcon.h"

/******************* defines *****************************/

/* hack-- exclude the attach manager from pigpen builds to avoid including collision system */

//#define PIGPEN

/* these values serve two purposes. first, nInitStatus is set to NO_INIT in the record declaration. this can be checked
to make sure that the record declaration doesn't get out of synch with the structure definition (i.e. a new field is
added to the structure but not to the record). second, the macros to set the default values can only be called before
the engine initialization-- they have no effect afterwards */

#define GAME_ENGINE_NO_INIT					0x12345678
#define GAME_ENGINE_INIT						0x87654321

/* default values */
/*** NOTE! for apps based on GameLib, some of these values get overwritten  ***/
/*** by GameApp::Initialize, so modifying them here will have no effect. it ***/
/*** is this way because the GameApp is the very first thing created when   ***/
/*** the game runs-- the game-specific code (e.g. SimLoop::Initialize) has  ***/
/*** not had a chance to run. games override default settings in GameApp by ***/
/*** providing the virtual function SetupGameInit. overwritten values are   ***/
/*** marked with o                                                          ***/

		#define DEFAULT_MAX_SCENES							2
/*o*/	#define DEFAULT_MAX_SCENE_NODES					100		// for adding instances to scene tree
		#define DEFAULT_MAX_SHADOWS						20			// this does not guarantee that enough realtime textures are set up-- use GameApp::SetRealtimeTextureInitInfo
		#define DEFAULT_MAX_REFLECTIONS					10			// this does not guarantee that enough realtime textures are set up-- use GameApp::SetRealtimeTextureInitInfo
		#define DEFAULT_MAX_LIGHTS							16
/*o*/	#define DEFAULT_MAX_SPRITES						400		// Doubled the number of available sprites (text driver eats them like candy) - Dan
/*o*/	#define DEFAULT_MAX_PARTICLES						500
		#define DEFAULT_MAX_PARTICLE_EMITTERS			50
		#define DEFAULT_MAX_PARTICLE_DATA_RECORDS		100
		#define DEFAULT_MAX_PARTICLE_CREATE_RECORDS	128		// expected max number of effects to create per frame
		#define DEFAULT_MAX_AURAS							0
/*o*/	#define DEFAULT_MAX_DISTORTIONS					0
/*o*/	#define DEFAULT_MAX_ATTACHERS						0
/*o*/	#define DEFAULT_MAX_SWOOSHES						100
/*o*/	#define DEFAULT_MAX_SWOOSH_EMITTERS				10
/*o*/	#define DEFAULT_MAX_TEXTURE_SETS					16
/*o*/	#define DEFAULT_MAX_POOL_MATRICES				100
/*o*/	#define DEFAULT_MAX_POOL_FRUSTUMS				32
/*o*/	#define DEFAULT_MAX_POOL_INSTANCES				0

/* for file manager */

/*o*/	#define DEFAULT_MAX_CUSTOM_CHUNK_LOADERS		10

/* for data manager */

/*o*/	#define DEFAULT_MAX_DATA_OWNERS					80
/*o*/	#define DEFAULT_MAX_DATA_RECORDS					100

/******************* compile control *********************/

#ifdef WIN32
#ifdef _DEBUG
//#define FPU_EXCEPTIONS_ON	// uncomment to catch FPU exceptions (normally masked)
#endif //_DEBUG
#endif //WIN32

/******************* macros ******************************/

/******************* structures **************************/

/* record that tracks game engine attributes. many values are filled in with default values for game initialization.
macros are provided above for editing them before initialization call. other values are for tracking things that get
set during initialization and maybe game play */

struct ts_GameEngine
{
	/* scene manager */

	int	nMaxScenes;
	int	nMaxSceneNodes;

	/* shadow manager */

	int	nMaxShadows;

	/* reflection manager */

	int	nMaxReflections;

	/* lights, sprites, particles, auras, distortions */

	int	nMaxLights;
	int	nMaxSprites;
	int	nMaxParticles;
	int	nMaxParticleEmitters;
	int	nMaxParticleCreateRecords;
	int	nMaxAuras;
	int	nMaxDistortions;
	int	nMaxAttachers;
	int	nMaxSwooshes;
	int	nMaxSwooshEmitters;
	int	nMaxPoolMatrices;
	int	nMaxPoolFrustums;
	int	nMaxPoolInstances;

	/* file manager */

	int	nMaxCustomChunkLoaders;

	/* data manager */

	int	nMaxDataOwners;
	int	nMaxDataRecords;

	/* texture manager */

	int	nMaxTextureSets;

	/* init flag, also used to make sure record declaration doesn't get out of synch with structure definition */

	int	nInitStatus;
};

/******************* externs *****************************/

/******************* global variables ********************/

/******************* local variables *********************/

/* currently no other file needs access to this. if this changes, move ts_GameEngine to g_State.h and declare
GameEngine extern there, too. a lot like DisplayState in DisplayState.c and d_State,h */
/*** NOTE! for apps based on GameLib, some of these values get overwritten  ***/
/*** by GameApp::Initialize, so modifying them here will have no effect. it ***/
/*** is this way because the GameApp is the very first thing created when   ***/
/*** the game runs-- the game-specific code (e.g. SimLoop::Initialize) has  ***/
/*** not had a chance to run. games override default settings in GameApp by ***/
/*** providing the virtual function SetupGameInit                           ***/

static ts_GameEngine GameEngine = {

	/* scene manager */

	DEFAULT_MAX_SCENES,						// int	nMaxScenes;
	DEFAULT_MAX_SCENE_NODES,				// int	nMaxSceneNodes;						*** overwritten by GameApp::Initialize ***

	/* shadow manager */

	DEFAULT_MAX_SHADOWS,						//	int	nMaxShadows;

	/* reflection manager */

	DEFAULT_MAX_REFLECTIONS,				//	int	nMaxReflections;

	/* lights, sprites, particles, auras, distortions, matrices */

	DEFAULT_MAX_LIGHTS,						//	int	nMaxLights;
	DEFAULT_MAX_SPRITES,						// int	nMaxSprites;							*** overwritten by GameApp::Initialize ***
	DEFAULT_MAX_PARTICLES,					// int	nMaxParticles;							*** overwritten by GameApp::Initialize ***
	DEFAULT_MAX_PARTICLE_EMITTERS,		// int	nMaxParticleEmitters;
	DEFAULT_MAX_PARTICLE_CREATE_RECORDS,// int	nMaxParticleCreateRecords;
	DEFAULT_MAX_AURAS,						// int	nMaxAuras;
	DEFAULT_MAX_DISTORTIONS,				// int	nMaxDistortions;						*** overwritten by GameApp::Initialize ***
	DEFAULT_MAX_ATTACHERS,					// int	nMaxAttachers;							*** overwritten by GameApp::Initialize ***
	DEFAULT_MAX_SWOOSHES,					// int	nMaxSwooshes;							*** overwritten by GameApp::Initialize ***
	DEFAULT_MAX_SWOOSH_EMITTERS,			// int	nMaxSwooshEmitters;					*** overwritten by GameApp::Initialize ***
	DEFAULT_MAX_POOL_MATRICES,				// int	nMaxPoolMatrices;						*** overwritten by GameApp::Initialize ***
	DEFAULT_MAX_POOL_FRUSTUMS,				// int	nMaxPoolFrustums;						*** overwritten by GameApp::Initialize ***
	DEFAULT_MAX_POOL_INSTANCES,			// int	nMaxPoolInstances;					*** overwritten by GameApp::Initialize ***

	/* file manager */

	DEFAULT_MAX_CUSTOM_CHUNK_LOADERS,	// int	nMaxCustomChunkLoaders;				*** overwritten by GameApp::Initialize ***

	/* data manager */

	DEFAULT_MAX_DATA_OWNERS,				// int	nMaxDataOwners;						*** overwritten by GameApp::Initialize ***
	DEFAULT_MAX_DATA_RECORDS,				// int	nMaxDataRecords;						*** overwritten by GameApp::Initialize ***

	/* texture manager */

	DEFAULT_MAX_TEXTURE_SETS,				// int	nMaxTextureSets;						*** overwritten by GameApp::Initialize ***

	/* init status, and to make sure we don't get out of synch with structure definition */

	GAME_ENGINE_NO_INIT						// int	nInitStatus;
};

/******************* local prototypes ********************/

/******************* functions ***************************/

/* initialize the game engine */

bool GameEngine__Init(void)
{
#if defined (WIN32) && !defined (_XBOX)
#ifndef CONSUMER_BUILD
	/* this assert is intended to make sure that this module is compiled with the same MMX_OK setting as the math */
	/* module. the mmx instructions require some alignment of matrices and vectors (__declspec(align(SIMD_ALIGN))). */
	/* if one module assumes alignment and another doesn't, all hell breaks loose. all modules that use math must */
	/* be compiled with same MMX_OK setting. the MMX_OK setting is done through project->settings->c/c++: the */
	/* preprocessor definitions should include MMX_OK=$(MMX_OK), and you should have the environment variable MMX_OK */
	/* set to 1 if your machine can handle SIMD, and 0 if not (older athlons) */

	ASSERT(Math::MMXOK() == MMX_OK);
#endif //CONSUMER_BUILD
#endif //WIN32

#ifdef FPU_EXCEPTIONS_ON
	/* enable fpu exceptions on pc */

	_control87( _EM_INEXACT | _EM_UNDERFLOW | _EM_DENORMAL, MCW_EM );	// turn on exceptions for all fpu errors except inexact, underflow, denormalized
#endif //FPU_EXCEPTIONS_ON

	/* make sure that our declaration of GameEngine values matches struct definition */

	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_NO_INIT);	// if this hits, declaration doesn't match definition (not enough values filled in)
	GameEngine.nInitStatus = GAME_ENGINE_INIT;

	/* one-time check of instance and data types defines */

	ASSERT(MAX_DATA_TYPES <= 256);		// data type field is an unsigned char
	ASSERT(MAX_INSTANCE_TYPES <= 256);	// sub type field is an unsigned char

	/* initialize fast vars */

	g4Identity.Identity();
	g3YRotate90.SetYRotation(Math::Deg2Rad(90.0f));
	vZero.Set(0.0f, 0.0f, 0.0f);
	vUp.Set(0.0f, 1.0f, 0.0f);

#ifdef PS2
	/* check alignment of fast vars */

	ASSERT(IS_VU_ALIGNED(&g4Temp));
	ASSERT(IS_VU_ALIGNED(&g4WorldToCamera));
	ASSERT(IS_VU_ALIGNED(&g4CameraToWorld));
	ASSERT(IS_VU_ALIGNED(&g4LocalToCamera));
	ASSERT(IS_VU_ALIGNED(&g4BoneTransform));
	ASSERT(IS_VU_ALIGNED(WorkVector));
	ASSERT(IS_VU_ALIGNED(&vZero));
	ASSERT(IS_VU_ALIGNED(&vUp));
	ASSERT(IS_VU_ALIGNED(&GeneralBoundingBox));
	ASSERT(IS_VU_ALIGNED(&GeneralBoundingSphere));
	ASSERT(IS_DMA_ALIGNED(u32ViewportPacket));
	ASSERT(IS_DMA_ALIGNED(u32LightPacket));
	ASSERT(IS_DMA_ALIGNED(u32DirectionalLightMatrixPacket));
	ASSERT(IS_DMA_ALIGNED(u32AmbientLightMatrixPacket));
	ASSERT(IS_DMA_ALIGNED(u32FullAmbientLightMatrixPacket));
	ASSERT(IS_DMA_ALIGNED(u32NoLightMatrixPacket));
	ASSERT(IS_DMA_ALIGNED(u32NoLightPlusEnvMapMatrixPacket));
	ASSERT(IS_DMA_ALIGNED(u32VertexWeightingPacket));
	ASSERT(IS_DMA_ALIGNED(u32OverrideColorPacket));
	ASSERT(IS_DMA_ALIGNED(u32VIFInterruptPacket));
	ASSERT(IS_DMA_ALIGNED(u32GSState1Packet));
	ASSERT(IS_DMA_ALIGNED(u32GSState2Packet));
	ASSERT(IS_DMA_ALIGNED(u8TexturePath2Packet));
	ASSERT(IS_DMA_ALIGNED(u8ParticleInitPacket));
	ASSERT(IS_DMA_ALIGNED(u8ParticleDataPacket));
	ASSERT(IS_DMA_ALIGNED(u8SpriteInitPacket));
	ASSERT(IS_DMA_ALIGNED(u8SpriteDataPacket));
	ASSERT(IS_DMA_ALIGNED(u8FlushePacket));
#endif //PS2

	/* platform-specific initialization */

#ifdef PS2
	sceGsResetPath();		// reset all paths into the GS
	sceDmaReset(1);		// initialize and enable DMA controller
#endif //PS2

	/* initialize other code sections */

	if (Layers__Init() == FALSE)
		return(FALSE);
	if (Display__Init() == FALSE)
		return(FALSE);

	/* managers */

	/*** note-- if you add a new manager, also add calls to GameEngine__Reset and GameEngine__Shutdown ***/

	ExceptionHandler::InitExceptions();
	RenderMgr__Init();
	TextureMgr::Init();
	SceneMgr::Init(GameEngine.nMaxSceneNodes);
	ShadowMgr__Init(GameEngine.nMaxShadows);
	ReflectionMgr__Init(GameEngine.nMaxReflections);
	SpriteMgr__Init(GameEngine.nMaxSprites);
	ParticleMgr__Init(GameEngine.nMaxParticles, GameEngine.nMaxParticleEmitters, GameEngine.nMaxParticleCreateRecords);
	AuraMgr__Init(GameEngine.nMaxAuras);
	DistortionMgr__Init(GameEngine.nMaxDistortions);
	dblInit(GameEngine.nMaxCustomChunkLoaders);
	DataMgr::Init(GameEngine.nMaxDataOwners, GameEngine.nMaxDataRecords);
#ifndef PIGPEN
	AttachMgr__Init(GameEngine.nMaxAttachers);
#endif //PIGPEN
	SwooshMgr__Init(GameEngine.nMaxSwooshes, GameEngine.nMaxSwooshEmitters);

	/* set up some systems */

	MatrixPool__Init(GameEngine.nMaxPoolMatrices);
	FrustumPool__Init(GameEngine.nMaxPoolFrustums);
	InstancePool__Init(GameEngine.nMaxPoolInstances);

	/* success */

	return(TRUE);
}

/* status */

bool GameEngine__IsRunning(void)
{
	return(GameEngine.nInitStatus == GAME_ENGINE_INIT);
}

/* reset */

void GameEngine__Reset(void)
{
	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_INIT);	// if this hits, it means GameEngine__Init was never called

	/* managers. dblReset will unload all .dbl files. if your manager would prefer to reset before the data is gone, */
	/* put its call before dblReset */

#ifndef PIGPEN
	AttachMgr__Reset();					// before ParticleMgr since attachers can alloc emitters
#endif //PIGPEN
	SwooshMgr__Reset();					// before ParticleMgr since textures come from particle texture set
	ParticleMgr__Reset();
	dblReset();
	ShadowMgr__Reset();					// ATB - needs to be before TextureMgr__UnMount
	TextureMgr::Cache().Reset();
	TextureMgr::Pause();				// TJC - needs to be remounted at the end of this function before it can be used.
	RenderMgr__Reset();
	ReflectionMgr__Reset();				// before SceneMgr since it kills scenes tied to reflections
	SceneMgr::Reset();
	SpriteMgr__Reset();
//yuch-- need to write commented out reset routines
//	AuraMgr__Reset();
	DistortionMgr__Reset();
	Layers__Reset();

	/* pools-- must be after managers since managers may allocate stuff from pools */

	MatrixPool__Reset();
	FrustumPool__Reset();
	InstancePool__Reset();

	/* data manager-- last so that everyone has a chance to clean up after themselves-- the data manager should have */
	/* to do anything to reset */
	
	DataMgr::Reset();

#ifdef PS2
	// reset the masterlist system.  so that it wont have any dma chunks alloced
	MasterList::Reset();	//RESET after ALL OBJECTS ARE REMOVED SO THEIR DISPLAY LISTS CAN BE REMOVED
#endif

#ifdef ENABLE_WATCHCLIENT
	/* reset the WatchClient */

	WatchClient::Instance()->Reset();
#endif //ENABLE_WATCHCLIENT

#ifdef GCN
	//let all of the double buffered memory get cleared out before starting next levels data
//	VISetBlack( true);
//	VIFlush( );
//	VIWaitForRetrace( );
	if( SoundDriverGCN::GetSoundDriverGCN( ) != NULL)
	{
		SoundDriverGCN::GetSoundDriverGCN( )->SetEnvironmentEffect( SoundDriver::NONE, 0.0f);
	}
	gfxEndRender( );
	gfxEndRender( );
//	VISetBlack( false);
#endif
	
	g_ScriptSystem.Reset();

	/* can be useful for debugging memory leaks. this is the right place to check-- after all systems have been reset, but */
	/* before a new game loop has been started. it can be useful to have a keypress call Layers__ShowMemoryState, for */
	/* example at a menu screen, but there have always been a few non-permanent .DBL files loaded to present the screen. */
	/* this is the place where everything has to be clean */

	/* for pc leak-tracking, also comment out MEM_USE_WIN32_MANAGER in
	 * Layers\Memory.h so that we go through ours */
	/* TJC - this MUST happen on CDROM builds too, so I moved it out of
	 *	the assert */
#ifndef CONSUMER_BUILD
	bool weAreClean = Layers__CheckPermanentMemory();
#ifdef SOMEDAYTHISWILLWORKAGAIN
	ASSERT(weAreClean);
#endif
#endif // CONSUMER_BUILD
	memClearUseBit(MEM_ALL_USE_BITS);	// good spot to clear all mem use bits

// Check the status of the handles
	SoundHandleTracker::Report();
//	Layers__ShowMemoryState( );

	// TJC - remount the Texture Manager (so it can reload the PS2 white texture before its used.
	TextureMgr::Resume();
}

/* shutdown */

void GameEngine__Shutdown(void)
{
	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_INIT);	// if this hits, it means GameEngine__Init was never called
	GameEngine.nInitStatus = GAME_ENGINE_NO_INIT;

	/* managers-- must be done before layers in shut down. dblShutdown will unload all .dbl files. if your manager */
	/* would prefer to shutdown before the data is gone, put its call before dblShutdown */

#ifndef PIGPEN
	AttachMgr__Shutdown();				// before ParticleMgr because attachers can allocate emitters
#endif //PIGPEN
	ParticleMgr__Shutdown();
	SwooshMgr__Shutdown();				// after ParticleMgr since unloading particle data kills swooshes
	dblShutdown();
	TextureMgr::Shutdown();
	RenderMgr__Shutdown();
	ReflectionMgr__Shutdown();			// before SceneMgr since it kills scenes tied to reflections
	SceneMgr::Shutdown();
	ShadowMgr__Shutdown();
	SpriteMgr__Shutdown();
//yuch-- need to write commented out shutdown routines
//	AuraMgr__Shutdown();
	DistortionMgr__Shutdown();

	/* pools-- must be after managers since managers may allocate stuff from pools */

	MatrixPool__Shutdown();
	FrustumPool__Shutdown();
	InstancePool__Shutdown();

	/* data manager-- last so that everyone has a chance to clean up after themselves-- the data manager should have */
	/* to do anything to shut down */
	
	DataMgr::Shutdown();

	/* shutdown other code sections */

	Display__Shutdown();
	Layers__Shutdown();	// must be last since it frees all memory

	g_ScriptSystem.Reset();
}	

/* advance */
#ifdef DIRECTX_PC
void GameEngine__Advance(bool updateInput)
#else
void GameEngine__Advance( void )
#endif
{
	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_INIT);	// if this hits, it means GameEngine__Init was never called

	/* advance the sub-systems */

	EngineTimer__Advance();
#ifndef PIGPEN
	AttachMgr__Advance();	// must be before ParticleMgr, at least, since emitters can be attached and must have their position updated
#endif //PIGPEN
	DistortionMgr__Advance();
	SwooshMgr__Advance();
	ParticleMgr__Advance();
	AuraMgr__Advance();
	float time = g_timer.GetFrameSec();
	if (!Math::Equal(time, 0.0f))
	{
		TextureMgr::Advance(time);
	}
	// Update hud.
	Animator::UpdateAll(g_timer.GetOSFrameSec());
//HACK HACK HACK HACK HACK HACK HACK HACK HACK 
#ifdef DIRECTX_PC 
	if(updateInput)
	{
		MSG  msg;
		WindowsApp *winApp = WindowsApp::IAm();

		// Has something gone seriously wrong?
		if (winApp == NULL)
			return;
		if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
				return;
		else
		{
			// If get message returns FALSE we are quitting
			if (!GetMessage(&msg, NULL, 0, 0))
				return;

			// Go, go, go!
			if (!winApp->HandleMessage(msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
#endif
// HACK HACK HACK HACK HACK HACK HACK HACK HACK
}

/* routines to update default initialization values before init is called */

void GameEngine__SetMaxScenes(int nMaxScenes)
{
	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_NO_INIT);	// if this hits, then GameEngine__Init has already been called, and the update has no effect
	GameEngine.nMaxScenes = nMaxScenes;
}
void GameEngine__SetMaxSceneNodes(int nMaxSceneNodes)
{
	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_NO_INIT);	// if this hits, then GameEngine__Init has already been called, and the update has no effect
	GameEngine.nMaxSceneNodes = nMaxSceneNodes;
}
void GameEngine__SetMaxShadows(int nMaxShadows)
{
	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_NO_INIT);	// if this hits, then GameEngine__Init has already been called, and the update has no effect
	GameEngine.nMaxShadows = nMaxShadows;
}
void GameEngine__SetMaxReflections(int nMaxReflections)
{
	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_NO_INIT);	// if this hits, then GameEngine__Init has already been called, and the update has no effect
	GameEngine.nMaxReflections = nMaxReflections;
}
void GameEngine__SetMaxLights(int nMaxLights)
{
	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_NO_INIT);	// if this hits, then GameEngine__Init has already been called, and the update has no effect
	GameEngine.nMaxLights = nMaxLights;
}
void GameEngine__SetMaxSprites(int nMaxSprites)
{
	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_NO_INIT);	// if this hits, then GameEngine__Init has already been called, and the update has no effect
	GameEngine.nMaxSprites = nMaxSprites;
}
void GameEngine__SetMaxParticles(int nMaxParticles)
{
	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_NO_INIT);	// if this hits, then GameEngine__Init has already been called, and the update has no effect
	GameEngine.nMaxParticles = nMaxParticles;
}
void GameEngine__SetMaxParticleEmitters(int nMaxParticleEmitters)
{
	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_NO_INIT);	// if this hits, then GameEngine__Init has already been called, and the update has no effect
	GameEngine.nMaxParticleEmitters = nMaxParticleEmitters;
}
void GameEngine__SetMaxParticleCreateRecords(int nMaxParticleCreateRecords)
{
	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_NO_INIT);	// if this hits, then GameEngine__Init has already been called, and the update has no effect
	GameEngine.nMaxParticleCreateRecords = nMaxParticleCreateRecords;
}
void GameEngine__SetMaxDistortions(int nMaxDistortions)
{
	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_NO_INIT);	// if this hits, then GameEngine__Init has already been called, and the update has no effect
	GameEngine.nMaxDistortions = nMaxDistortions;
}
void GameEngine__SetMaxAttachers(int nMaxAttachers)
{
	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_NO_INIT);	// if this hits, then GameEngine__Init has already been called, and the update has no effect
	GameEngine.nMaxAttachers = nMaxAttachers;
}
void GameEngine__SetMaxSwooshes(int nMaxSwooshes)
{
	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_NO_INIT);	// if this hits, then GameEngine__Init has already been called, and the update has no effect
	GameEngine.nMaxSwooshes = nMaxSwooshes;
}
void GameEngine__SetMaxSwooshEmitters(int nMaxSwooshEmitters)
{
	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_NO_INIT);	// if this hits, then GameEngine__Init has already been called, and the update has no effect
	GameEngine.nMaxSwooshEmitters = nMaxSwooshEmitters;
}
void GameEngine__SetMaxDataOwners(int nMaxDataOwners)
{
	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_NO_INIT);	// if this hits, then GameEngine__Init has already been called, and the update has no effect
	GameEngine.nMaxDataOwners = nMaxDataOwners;
}
void GameEngine__SetMaxDataRecords(int nMaxDataRecords)
{
	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_NO_INIT);	// if this hits, then GameEngine__Init has already been called, and the update has no effect
	GameEngine.nMaxDataRecords = nMaxDataRecords;
}
void GameEngine__SetMaxTextureSets(int nMaxTextureSets)
{
	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_NO_INIT);	// if this hits, then GameEngine__Init has already been called, and the update has no effect
	GameEngine.nMaxTextureSets = nMaxTextureSets;
}
void GameEngine__SetMaxPoolMatrices(int nMaxPoolMatrices)
{
	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_NO_INIT);	// if this hits, then GameEngine__Init has already been called, and the update has no effect
	GameEngine.nMaxPoolMatrices = nMaxPoolMatrices;
}
void GameEngine__SetMaxPoolFrustums(int nMaxPoolFrustums)
{
	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_NO_INIT);	// if this hits, then GameEngine__Init has already been called, and the update has no effect
	GameEngine.nMaxPoolFrustums = nMaxPoolFrustums;
}
void GameEngine__SetMaxPoolInstances(int nMaxPoolInstances)
{
	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_NO_INIT);	// if this hits, then GameEngine__Init has already been called, and the update has no effect
	GameEngine.nMaxPoolInstances = nMaxPoolInstances;
}
void GameEngine__SetMaxCustomChunkLoaders(int nMaxCustomChunkLoaders)
{
	ASSERT(GameEngine.nInitStatus == GAME_ENGINE_NO_INIT);	// if this hits, then GameEngine__Init has already been called, and the update has no effect
	GameEngine.nMaxCustomChunkLoaders = nMaxCustomChunkLoaders;
}

#ifndef CDROM
void GameEngine__StatsToConsole()
{
	g_console.Notify("Game engine stats:\n");
	g_console.Notify("   Active particles: %4d  Max: %4d\n", ParticleMgr__GetActiveParticleCount(), ParticleMgr__GetMaxParticleCount());
	g_console.Notify("   Active emitters:  %4d  Max: %4d\n", ParticleMgr__GetActiveEmitterCount(), ParticleMgr__GetMaxEmitterCount());
}
#endif //CDROM
