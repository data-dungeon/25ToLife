/***********************************/
/*                                 */
/* Preview.cpp                     */
/* Pigpen particle tool  12/13/00  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* particle effect previewer       */
/*                                 */
/***********************************/

/* use the game engine to preview particle effects */

/******************* includes ****************************/

/*** pre-compiled headers through .h file (visual studio), can't be inside #if statement. ***/
/*** see .h file for more info.for codewarrior, pre-compiled header is handled through    ***/
/*** xcl compile/link interpreter, and doesn't need any explicit include                  ***/

#include <PigpenPCH.h>

/*** includes that don't belong in pre-compiled header (typically due to low frequency) ***/

/* game engine includes */

#include <Layers/Timing.h>
#include <Layers/Media.h>
#include <Layers/Debug.h>
#include <Layers/Memory.h>
#include <Layers/LayersState.h>
#include <Math/Matrix.h>
#include <Display/Common/DisplayState.h>
#include <Display/Common/DisplayPlatform.h>
#include <Display/Common/GfxFunc.h>
#include <Display/Common/RenderState.h>
#include <Display/Common/TextureMgr.h>
#include <Game/Misc/FastVars.h>
#include <Game/Managers/RenderMgr.h>
#include <Game/Managers/ParticleMgr.h>
#include <Game/Managers/SwooshMgr.h>
#include <Game/Managers/SceneMgr.h>
#include <Game/Managers/DistortionMgr.h>
#include <Game/Managers/LightMgr.h>
#include <Game/Database/ChunkLoaders.h>
#include <Game/Database/Instance.h>
#include <Game/Database/DataHelpers.h>
#include <EngineHelper/Timer.h>
#include <Path/SplinePath.h>

/******************* defines *****************************/

/* for camera control */

#define INC_SPEED 200
#define MAX_SPEED 200

/* standard gravity */

#define STANDARD_GRAVITY	-9.8f

/******************* structures **************************/

/******************* local variables *********************/

static bool bWinInfoSet = FALSE;
static bool bActivateApp = FALSE;
static Vector3 vEmitterPos = Vector3(0.0f, 0.0f, 0.0f);
static Vector3 vEmitterVel = Vector3(0.0f, 0.0f, 0.0f);
static Vector3 vEmitterUp = Vector3(0.0f, 1.0f, 0.0f);
static Vector3 vEmitterSide = Vector3(1.0f, 0.0f, 0.0f);
static char PreviewEmitterName[100];
static char PreviewSwooshEmitterName[100];
static Vector3 InitCameraPos = Vector3(DEFAULT_CAMERA_POS_X, DEFAULT_CAMERA_POS_Y, DEFAULT_CAMERA_POS_Z);
static Vector3 InitCameraRotate = Vector3(0.0f, 0.0f, 0.0f);
static unsigned long ulPreviewFlags = 0;		// preview control flags, set/cleared by preview dialog
static float fPreviewDelay;						// emission delay
static float fPreviewEmitterVel = 0.0f;		// simulated speed of emitter
static float fPreviewYPos = 0.0f;				// emitter's y-position
static float fPreviewWindMPH = 0.0f;			// wind speed for preview
static COLORREF BackgroundColor = 0x808080;	// medium gray
static CString StageFile;
static CString EmitterModelFile;
static float fEmitterModelXPos;
static float fEmitterModelYPos;
static float fEmitterModelZPos;
static float fEmitterModelYRot;
static int nCollisionIndex;
static bool bEmitterVelIsMPH = true;

/* previewing doc, accompanying preview dialog */

static CPigpenDoc *pPreviewDoc = NULL;
static CPreviewDialog *pPreviewDialog = NULL;

/* record original pointers passed to Preview__Create */

static CEmitter *pPreviewEmitter = NULL;
static CEffect *pPreviewEffect = NULL;
static CParticle *pPreviewParticle = NULL;
static CSwoosh *pPreviewSwoosh = NULL;
static CPigpenSwooshEmitter *pPreviewSwooshEmitter = NULL;

/* database, scene manage control */

static t_Handle WorldDBHandle = INVALID_HANDLE;				// handle of world database for use with dblGetChunkData
static t_Handle WorldSceneHandle = INVALID_HANDLE;			// handle of world scene
static t_Handle EmitterModelDBHandle = INVALID_HANDLE;	// handle of emitter model database
static t_Handle ParticleDataHandle = INVALID_HANDLE;		// handle of particle data file (preview .dbl)

/* for keyboard/mouse input for camera control */

#define L_MBUTTON (1<<0)
#define R_MBUTTON (1<<1)
#define M_MBUTTON (1<<2)
static struct
{
	unsigned int buttons;
	int tx, rx;					//trans, rot x
	int ty, ry;
	int tz, rz;
	int mx, my;					//mouse coordinate start point
	int mz, mzlast;			//mouse wheel
} input;

/* camera */

static Vector3 vCameraPosition;
static Vector3 vCameraTarget;
static bool bCameraFirstInput = true;
static float fCameraRotateX = 0.0f;
static float fCameraRotateY = 0.0f;
static Graphics4x4 g4PigpenWorldToCamera;

/* emitter */

static t_Handle EmitterHandle = INVALID_HANDLE;
static CEmitter *pLastEmitter = NULL;

/* swoosh emitter */

static CSwooshEmitter *pGameSwooshEmitter = NULL;

/* emitter model-- drawn at emitter position, e.g. a bike */

static ts_Instance Instance;
static ts_InstancePart2 InstancePart2;
static ts_Instance *pEmitterModelInstance = NULL;

/* test spline path to use */

static SplinePath *pSplinePath = NULL;

/******************* global variables ********************/

/******************* externs *****************************/

/******************* local prototypes ********************/

static void Preview__ExportDBL(float *pfPreviewDelay);
static void Preview__ReexportDBL(bool bSetPreviewDelay);
static bool PreviewCollisionTest(Vector3 *pvPos, Vector3 *pvLastPos, Vector3 *pvCollisionPos, Vector3 *pvCollisionNormal, u32 *pu32CollisionMaterial, bool testTerrain, bool testDynamic);
static void SetPreviewData(CObArray *pArrays);
static t_Handle Preview__CreateEmitter();
static CSwooshEmitter *Preview__CreateSwooshEmitter();
static void UnloadEmitterModel(void);
static void UpdateBackgroundColor(COLORREF Color);
static void MoveCamera(void);
static void LoadStage(void);
static void LoadEmitterModel(void);
static bool ShowGroup( ts_Group *pGroup);
static void SetupDummyGroup(t_Handle WorldDBHandle, ts_Group **ppGroupArray, int *pnGroupCount);
static s32 FAR PASCAL WindowProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
static void BuildTestPath();

/******************* functions ***************************/

/* preview an emitter, an effect, or a particle. there are 3 dialogs that call this: the emitter dialog,
the effect dialog, and the particle dialog. may have to create dummy records to fill the hierarchy to the top */

void Preview__Create(CPigpenDoc *pDoc, CEmitter *pEmitter, CEffect *pEffect, CParticle *pParticle, CSwoosh *pSwoosh, CPigpenSwooshEmitter *pSwooshEmitter)
{
	bool bOpenPreviewDialog = FALSE;
	u32 u32Ticks;

	/* need to record original passed source particle/effect/emitter in case i need to rebuild .dbl file. */
	/* only one of these will be specified */

	pPreviewDoc = pDoc;
//	pPreviewEmitter = pEmitter;
//	pPreviewEffect = pEffect;
//	pPreviewParticle = pParticle;
//	pPreviewSwoosh = pSwoosh;
//	pPreviewSwooshEmitter = pSwooshEmitter;

	/* i'm updating this so that the user can preview a particle and a swoosh at the same time. this is done by */
	/* previewing a particle, then previewing a swoosh while the preview window is still open. closing the preview */
	/* window resets it. only one of pEmitter/pEffect/pParticle/pSwoosh/pSwooshEmitter should be set on calling this */
	/* routine. figure out if subsequent calls can co-exist or whether this should replace an existing effect */

	if (GameEngine__IsRunning())
	{
		int nCount = 0;
		if (pEmitter)
			nCount++;
		if (pEffect)
			nCount++;
		if (pParticle)
			nCount++;
		if (pSwoosh)
			nCount++;
		if (pSwooshEmitter)
			nCount++;
		ASSERT(nCount == 1);
		if ((pEmitter) || (pEffect) || (pParticle))
		{
			/* particle-based, overwrite particle-based variables */

			pPreviewEmitter = pEmitter;
			pPreviewEffect = pEffect;
			pPreviewParticle = pParticle;
		}
		if ((pSwoosh) || (pSwooshEmitter))
		{
			/* swoosh-based, overwrite swoosh-based variables */

			pPreviewSwoosh = pSwoosh;
			pPreviewSwooshEmitter = pSwooshEmitter;
		}
	}
	else
	{
		/* preview not active, overwrite ALL values */

		pPreviewEmitter = pEmitter;
		pPreviewEffect = pEffect;
		pPreviewParticle = pParticle;
		pPreviewSwoosh = pSwoosh;
		pPreviewSwooshEmitter = pSwooshEmitter;
	}

	/* if preview is already running, just re-export */

	if (GameEngine__IsRunning())
	{
		Preview__ReexportDBL(true);
		return;
	}

	/* create .dbl */

	Preview__ExportDBL(&fPreviewDelay);	// first time, get preview delay from emitter

	/* turn off data directory name mangling */

	MediaHAL::EnableNameMangling(false);
	
	/* have we set windows info? */

	if (bWinInfoSet == FALSE)
	{
		Display__SetWinInfo(PigpenApp.m_pMainWnd->m_hWnd, AfxGetInstanceHandle(), WindowProc, 1, NULL);
		bWinInfoSet = TRUE;
	}

	/* get doc's preview prefs into local copy. we need a local copy of data because doc can close */
	/* while preview window is still running (not that that makes much sense, because they won't be */
	/* able to edit anything, but...) */

	BackgroundColor = pPreviewDoc->GetBackgroundColor();
	pPreviewDoc->GetCameraPos(&InitCameraPos, &InitCameraRotate);
	StageFile = pPreviewDoc->GetStageFile();
	EmitterModelFile = pPreviewDoc->GetEmitterModelFile();
	fEmitterModelXPos = pPreviewDoc->GetEmitterModelXPos();
	fEmitterModelYPos = pPreviewDoc->GetEmitterModelYPos();
	fEmitterModelZPos = pPreviewDoc->GetEmitterModelZPos();
	fEmitterModelYRot = pPreviewDoc->GetEmitterModelYRot();

	/* do we need to start up the game engine? */

	if (GameEngine__IsRunning() == FALSE)
	{
		bActivateApp = FALSE;
		static poolCfg poolConfig[] =
		{
			// terminator
			{   0,    0, false, NULL, NULL, NULL },
		};
		Layers__SetMemoryPoolConfig(poolConfig);	// empty pool definition-- i haven't done any analysis to see what would be good sizes
		Display__ResetScreenDimensions();			// make sure screen size doesn't change due to multiple previews
		GameEngine__SetMaxDistortions(10);			// may want some wind
		GameEngine__SetMaxParticles(5000);
		GameEngine__SetMaxSwooshes(500);
		GameEngine__Init();

		/* background color */

		UpdateBackgroundColor(BackgroundColor);

		/* set clip planes */

		Display__SetNearClip(1.0f);
		Display__SetFarClip(1000.0f);

		/* set up camera */

		Preview__ResetCamera();

		/* register cheesy collision routine */

		ParticleMgr__RegisterCollisionTest(PreviewCollisionTest);

		/* update window text */

		SetWindowText(Display__GetWnd(), "Pigpen Preview");

		/* bring up accompanying dialog */

		bOpenPreviewDialog = TRUE;

		/* set up default lighting */

		ts_fRGB Ambient = {0.2f, 0.2f, 0.2f};
		LightMgr__SetAmbient(&Ambient);
		ts_Light Light;
		Light.u32Flags = LIGHT_ON | LIGHT_DIRECTIONAL;
		Light.Pos.Set(1.0f, 1.0f, 1.0f);
		Light.Color.R = 0.8f;
		Light.Color.G = 0.6f;
		Light.Color.B = 0.5f;
		Light.fFalloffStart = 0.0f;
		Light.fFalloff = 0.0f;
		LightMgr__RegisterLight(&Light);
	}
	else
	{
		GameEngine__Reset();
		WorldDBHandle = INVALID_HANDLE;
		WorldSceneHandle = INVALID_HANDLE;
		EmitterModelDBHandle = INVALID_HANDLE;
		pEmitterModelInstance = NULL;
	}

	/* load particle .DBL file */

	ParticleDataHandle = ParticleMgr__LoadData(PREVIEW_FILE_NAME);

	/* record memory locations of source particle data so it can be updated in real-time */

	SetPreviewData(pDoc->GetArrays());

	/* load stage and emitter model */

	LoadStage();
	LoadEmitterModel();

	/* set up the emitter */

	EmitterHandle = Preview__CreateEmitter();
	pGameSwooshEmitter = Preview__CreateSwooshEmitter();

	/* bring up accompanying dialog-- do it after emitter is created since some of the dialog values */
	/* are tied to the emitter */

	if (bOpenPreviewDialog)
	{
		_ASSERT(pPreviewDialog == NULL);	// only one at a time
		pPreviewDialog = new CPreviewDialog(PigpenApp.m_pMainWnd);
		pPreviewDialog->Create(pPreviewDoc);
		pPreviewDialog->InitialPosition();
		pPreviewDialog->ShowData();
	}

	/* make sure current wind value is in effect */

	Preview__UpdateWindMPH();

	/* main loop */

	u32Ticks = EngineTimer__Time();
	while (GameEngine__IsRunning())	// game engine is shut down when window is closed
	{
		/* update master timer */

		g_timer.NewFrame();

		/* set world-to-camera transform matrix */

		MoveCamera();

		/* handle windows messages */

		MSG msg;
		if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (GetMessage(&msg, NULL, 0, 0) == 0)	// returns 0 if it receives WM_QUIT message
			{
				/* we never get here in Pigpen, since the WindowProc doesn't call PostQuitMessage. see comment below */

				ASSERT(FALSE);
				Display__DestroyWindow();	// clean after window is closed
				return;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			/* for most messages we abort render (i'm not really sure why, but it works better). however, ignore mouse */
			/* movement since we get so many of these messages */

			if ((msg.message != WM_MOUSEMOVE) && (msg.message != WM_NCMOUSEMOVE))
				continue;
		}
		if (!bActivateApp)
		{
			WaitMessage();
			continue;
		}

		/* active emitter? */

		if (EmitterHandle != INVALID_HANDLE)
		{
			/* simulate motion of emitter */

			Vector3 vLastPos = vEmitterPos;
			vEmitterPos += vEmitterVel * g_timer.GetFrameSec();

			/* gravity? */

			if (ulPreviewFlags & PREVIEW_GRAVITY)
				vEmitterVel.y(vEmitterVel.y() + (STANDARD_GRAVITY * g_timer.GetFrameSec()));
			else
			{
				vEmitterPos.y(fPreviewYPos);
				vEmitterVel.y(0.0f);
			}

			/* bounce? */

			Vector3 vCollisionPos;
			Vector3 vCollisionNormal;
			u32 u32Material;
			if (PreviewCollisionTest(&vEmitterPos, &vLastPos, &vCollisionPos, &vCollisionNormal, &u32Material, true, true))
			{
				/* gravity on or off? */

				if (ulPreviewFlags & PREVIEW_GRAVITY)
				{
					/* reflect */

					vEmitterPos.y(0.0f);
					vEmitterVel.y(-0.6f * vEmitterVel.y());

					/* if we are at a small velocity, bounce it hard to start again */

					if (vEmitterVel.y() < 1.5f)
					{
						vEmitterPos.Set(0.0f, fPreviewYPos, 0.0f);
						vEmitterVel.y(10.0f);
					}
				}
				else
				{
					/* gravity off, start running along ground again */

					vEmitterPos.y(0.0f);
					vEmitterVel.y(0.0f);
				}
			}

			/* wrap? */

			if (vEmitterPos.z() < -100.0f)
			{
				vEmitterPos.z(100.0f);
				ParticleEmitter__ResetLastPos(EmitterHandle);	// suspend min distance enforcement as we wrap
			}

			/* update emitter model transform? it's not that complicated, so do it every time so i don't */
			/* have to do it at reset-all, etc-- other places that change vEmitterPos */

			Preview__BuildEmitterModelTransform(FALSE);

			/* camera follows? (but it doesn't bounce, so don't adjust y) */

			if (ulPreviewFlags & PREVIEW_CAMERA_FOLLOWS)
			{
				vCameraPosition.x(vCameraPosition.x() + (vEmitterPos.x() - vLastPos.x()));
				vCameraPosition.z(vCameraPosition.z() + (vEmitterPos.z() - vLastPos.z()));
				vCameraTarget.x(vCameraTarget.x() + (vEmitterPos.x() - vLastPos.x()));
				vCameraTarget.z(vCameraTarget.z() + (vEmitterPos.z() - vLastPos.z()));
			}

			/* this is a bit cheesy, there should be a callback or something i think, oh well */

			if ((ulPreviewFlags & PREVIEW_PARTICLE_PATH) && (ParticleEmitter__HasParticlePath(EmitterHandle) == false))
			{
				if (pSplinePath == NULL)
					BuildTestPath();
				ParticleEmitter__SetParticlePath(EmitterHandle, pSplinePath);	// marked to have a path but doesn't, give it one
			}
			if (((ulPreviewFlags & PREVIEW_PARTICLE_PATH) == 0) && (ParticleEmitter__HasParticlePath(EmitterHandle)))
				ParticleEmitter__SetParticlePath(EmitterHandle, NULL);			// marked to have no path but has one, clear it
		}

		/* advance the game engine */

		GameEngine__Advance();
		RenderMgr__Render();

		/* limit to 30 hz */

		while (EngineTimer__Time() - u32Ticks < (1000 / 30))
			;
		u32Ticks = EngineTimer__Time();
	}

	/* clean up after engine shutdown */

	pEmitterModelInstance = NULL;
	WorldDBHandle = INVALID_HANDLE;
	WorldSceneHandle = INVALID_HANDLE;
	EmitterModelDBHandle = INVALID_HANDLE;

	/* clean up preview files */

	remove(PREVIEW_FILE_NAME);
	remove(PREVIEW_LOG_NAME);

	/* i haven't been able to do this cleanly. what i want to do is unregister the window class (for the preview */
	/* window) when it closes. typically, the windowproc calls PostQuitMessage when it receives the WM_DESTROY */
	/* message, which causes a WM_QUIT message to be sent. when that is caught in the GetMessage check above, i */
	/* can safely unregister the window class. but if i have the windowproc call PostQuitMessage, all of Pigpen */
	/* shuts down. i think that this is because they are the same thread (though you can successfully switch */
	/* back and forth between the preview window and pigpen, so don't they have to be separate threads?). so */
	/* unregister until you succeed */

	while (Display__DestroyWindow() == FALSE)
		;
}

/* build the .dbl file for previewing */

static void Preview__ExportDBL(float *pfPreviewDelay)
{
	/* do i need to create a dummy effect/emitter? */

	bool bDeleteDummyEffect = FALSE;
	bool bDeleteDummyEmitter = FALSE;
	bool bDeleteDummySwooshEmitter = FALSE;
	if (pPreviewParticle)
	{
		ASSERT(pPreviewEffect == NULL);
		ASSERT(pPreviewEmitter == NULL);
		pPreviewEffect = pPreviewDoc->AddDummyEffect();	// override NULL passed pEffect
		pPreviewEffect->SetupDummy(pPreviewParticle);
		bDeleteDummyEffect = TRUE;
		pPreviewEmitter = pPreviewDoc->AddDummyEmitter();	// override NULL passed pEmitter
		pPreviewEmitter->SetupDummy(pPreviewEffect);
		bDeleteDummyEmitter = TRUE;
	}
	else if (pPreviewEffect)
	{
		ASSERT(pPreviewParticle == NULL);
		ASSERT(pPreviewEmitter == NULL);
		pPreviewEmitter = pPreviewDoc->AddDummyEmitter();	// override NULL passed pEmitter
		pPreviewEmitter->SetupDummy(pPreviewEffect);
		bDeleteDummyEmitter = TRUE;
	}
	else if (pPreviewEmitter)
	{
		ASSERT(pPreviewParticle == NULL);
		ASSERT(pPreviewEffect == NULL);
	}

	/* same kind of check for swoosh */

	if (pPreviewSwoosh)
	{
		ASSERT(pPreviewSwooshEmitter == NULL);
		pPreviewSwooshEmitter = pPreviewDoc->AddDummySwooshEmitter();	// override NULL passed pEmitter
		pPreviewSwooshEmitter->SetupDummy(pPreviewSwoosh);
		bDeleteDummySwooshEmitter = TRUE;
	}
	else if (pPreviewSwooshEmitter)
	{
		ASSERT(pPreviewSwoosh == NULL);
	}

	/* set preview delay from emitter */

	if ((pPreviewEmitter) && (pfPreviewDelay))
		*pfPreviewDelay = pPreviewEmitter->GetDelay();

	/* export the particle data to a temp .dbl file, record doc that created preview */

	nExportMode = EXPORT_PS2;
	nHackExportMode = EXPORT_PC;	//yuch-- fix this
	if (ExportDBL(pPreviewDoc, pPreviewDoc->GetArrays(), NULL, pPreviewEmitter, pPreviewSwooshEmitter) == FALSE)
		return;	// export failed

	/* record name of emitter for creation */

	if (pPreviewEmitter)
		strcpy(PreviewEmitterName, pPreviewEmitter->GetName().GetBuffer(10));
	else
		strcpy(PreviewEmitterName, "");

	/* record name of swoosh emitter for creation */

	if (pPreviewSwooshEmitter)
		strcpy(PreviewSwooshEmitterName, pPreviewSwooshEmitter->GetName().GetBuffer(10));
	else
		strcpy(PreviewSwooshEmitterName, "");

	/* maybe delete dummies */

	if (bDeleteDummyEffect)
	{
		pPreviewDoc->DeleteDummyEffect();
		pPreviewEffect = NULL;
	}
	if (bDeleteDummyEmitter)
	{
		pPreviewDoc->DeleteDummyEmitter();
		pPreviewEmitter = NULL;
	}
	if (bDeleteDummySwooshEmitter)
	{
		pPreviewDoc->DeleteDummySwooshEmitter();
		pPreviewSwooshEmitter = NULL;
	}
	pLastEmitter = pPreviewEmitter;	// record for Preview__ResetEmitter
}

/* re-build preview dbl because something changed that we can't just update in place (e.g. changing a particle's
graphic-- new graphic may not have been exported before */

static void Preview__ReexportDBL(bool bSetPreviewDelay)
{
	/* unload old data */

	ParticleMgr__UnloadData(ParticleDataHandle);

	/* export new data file */

	if (bSetPreviewDelay)
		Preview__ExportDBL(&fPreviewDelay);
	else
		Preview__ExportDBL(NULL);

	/* load it up */

	ParticleDataHandle = ParticleMgr__LoadData(PREVIEW_FILE_NAME);

	/* record memory locations of source particle data so it can be updated in real-time */

	SetPreviewData(pPreviewDoc->GetArrays());

	/* set up the particle or swoosh emitter */

	EmitterHandle = Preview__CreateEmitter();
	pGameSwooshEmitter = Preview__CreateSwooshEmitter();

	/* maybe update dialog contents */

	if (pPreviewDialog)
		pPreviewDialog->ShowData();
}

/* very simple collision test */

static bool PreviewCollisionTest(Vector3 *pvPos, Vector3 *pvLastPos, Vector3 *pvCollisionPos, Vector3 *pvCollisionNormal, u32 *pu32CollisionMaterial, bool testTerrain, bool testDynamic)
{
	if ((pvPos->y() < 0.0f) && (pvLastPos->y() >= 0.0f))
	{
		*pvCollisionPos = *pvPos;
		pvCollisionPos->y(0.0f);
		pvCollisionNormal->Set(0.0f, 1.0f, 0.0f);
		*pu32CollisionMaterial = nCollisionIndex;
		return(true);
	}
	return(false);
}
void Preview__SetCollisionIndex(int nIndex)
{
	nCollisionIndex = nIndex;
}

static void SetPreviewData(CObArray *pArrays)
{
	int i;
	int nIndex;

	/* go through and record engine data location of exported data. emitter first (will only be one). */
	/* marked records are exported in order, so to find correspondence between pigpen records and */
	/* game engine records it is sufficient to just start the engine record index at 0 and bump it */
	/* one every time an exported record is found */

	nIndex = 0;
	for (i = 0; i < pArrays[EMITTERS].GetSize(); i++)
	{
		CEmitter *pEmitter = (CEmitter *) pArrays[EMITTERS].GetAt(i);
		if (pEmitter->GetExportStatus() == EXPORT_STATUS_VALID)
			pEmitter->SetEngineData(ParticleMgr__GetEmitterSource(0, nIndex++));
		else
			pEmitter->SetEngineData(NULL);
	}

	/* effects */

	nIndex = 0;
	for (i = 0; i < pArrays[EFFECTS].GetSize(); i++)
	{
		CEffect *pEffect = (CEffect *) pArrays[EFFECTS].GetAt(i);
		if (pEffect->GetExportStatus() == EXPORT_STATUS_VALID)
			pEffect->SetEngineData(ParticleMgr__GetEffectSource(0, nIndex++));
		else
			pEffect->SetEngineData(NULL);
	}
	
	/* particles */

	nIndex = 0;
	for (i = 0; i < pArrays[PARTICLES].GetSize(); i++)
	{
		CParticle *pParticle = (CParticle *) pArrays[PARTICLES].GetAt(i);
		if (pParticle->GetExportStatus() == EXPORT_STATUS_VALID)
			pParticle->SetEngineData(ParticleMgr__GetParticleSource(0, nIndex++));
		else
			pParticle->SetEngineData(NULL);
	}

	/* swooshes */

	nIndex = 0;
	for (i = 0; i < pArrays[SWOOSHES].GetSize(); i++)
	{
		CSwoosh *pSwoosh = (CSwoosh *) pArrays[SWOOSHES].GetAt(i);
		if (pSwoosh->GetExportStatus() == EXPORT_STATUS_VALID)
			pSwoosh->SetEngineData(SwooshMgr__GetSwooshSource(0, nIndex++));
		else
			pSwoosh->SetEngineData(NULL);
	}

	/* swoosh emitters */

	nIndex = 0;
	for (i = 0; i < pArrays[SWOOSH_EMITTERS].GetSize(); i++)
	{
		CPigpenSwooshEmitter *pSwooshEmitter = (CPigpenSwooshEmitter *) pArrays[SWOOSH_EMITTERS].GetAt(i);
		if (pSwooshEmitter->GetExportStatus() == EXPORT_STATUS_VALID)
			pSwooshEmitter->SetEngineData(SwooshMgr__GetSwooshEmitterSource(0, nIndex++));
		else
			pSwooshEmitter->SetEngineData(NULL);
	}
}

/* update export buffer and copy into game engine particle source data to update continuous particle preview */

void Preview__UpdateParticle(CParticle *pParticle)
{
	/* maybe nothing to do */

	if (GameEngine__IsRunning() == false)
		return;

	/* try to update record in place */

	if (pParticle->GetEngineData())
	{
		/* we are going to write directly over the data the game engine is running, so we can't */
		/* write any larger than the data that was originally exported. it is up to the exporting */
		/* routines to export a full data record when exporting for preview */

		if (pParticle->FillExportBuffer((char *) pParticle->GetEngineData(), pParticle->GetExportDataSize(), true) != 0)
		{
			/* fix up and done */

			FixParticleSource((ts_ParticleSource *) pParticle->GetEngineData());	// data fix-up usually done by dbl loader

			/* successfully updated record */

			return;
		}
	}

	/* we need to rebuild .dbl */

	Preview__ReexportDBL(false);
}

void Preview__UpdateEffect(CEffect *pEffect)
{
	/* maybe nothing to do */

	if (GameEngine__IsRunning() == false)
		return;

	/* try to update record in place */

	if (pEffect->GetEngineData())
	{
		/* we are going to write directly over the data the game engine is running, so we can't */
		/* write any larger than the data that was originally exported. it is up to the exporting */
		/* routines to export a full data record when exporting for preview */

		if (pEffect->FillExportBuffer((char *) pEffect->GetEngineData(), pEffect->GetExportDataSize(), true) != 0)
		{
			/* fix up and done. get particle data (previously loaded chunk in this database) for */
			/* patching effect references */

			ts_RecordList *pParticleSourceList;
		   pParticleSourceList = (ts_RecordList *) dblGetChunkData(DB_PARTICLE_SOURCE, DEFAULT_CHUNK_KEY_STRING, ParticleMgr__GetDBHandle(0));
			ASSERT(pParticleSourceList);
			FixParticleEffect((ts_Effect *) pEffect->GetEngineData(), pParticleSourceList);	// data fix-up usually done by dbl loader
			
			/* successfully updated record */

			return;
		}
	}

	/* we need to rebuild .dbl */

	Preview__ReexportDBL(false);
}

void Preview__UpdateEmitter(CEmitter *pEmitter)
{
	/* maybe nothing to do */

	if (GameEngine__IsRunning() == false)
		return;

	/* try to update record in place */

	if (pEmitter->GetEngineData())
	{
		/* we are going to write directly over the data the game engine is running, so we can't */
		/* write any larger than the data that was originally exported. it is up to the exporting */
		/* routines to export a full data record when exporting for preview */

		if (pEmitter->FillExportBuffer((char *) pEmitter->GetEngineData(), pEmitter->GetExportDataSize(), true) != 0)
		{
			/* restart emitter */

			ParticleEmitter__Kill(EmitterHandle);
			ASSERT(pPreviewEmitter);
			pPreviewEmitter = pEmitter;
			EmitterHandle = Preview__CreateEmitter();

			/* preview dialog delay stays in sync with emitter delay */

			fPreviewDelay = pEmitter->GetDelay();
			if (pPreviewDialog)
				pPreviewDialog->ShowData();

			/* successfully updated record */

			return;
		}
	}

	/* we need to rebuild .dbl */

	Preview__ReexportDBL(false);
}

/* update export buffer and copy into game engine particle source data to update continuous swoosh preview */

void Preview__UpdateSwoosh(CSwoosh *pSwoosh)
{
	/* maybe nothing to do */

	if (GameEngine__IsRunning() == false)
		return;

	/* try to update record in place */

	if (pSwoosh->GetEngineData())
	{
		/* we are going to write directly over the data the game engine is running, so we can't */
		/* write any larger than the data that was originally exported. it is up to the exporting */
		/* routines to export a full data record when exporting for preview */

		if (pSwoosh->FillExportBuffer((char *) pSwoosh->GetEngineData(), pSwoosh->GetExportDataSize(), true) != 0)
		{
			/* fix up and done */

			FixSwooshSource((ts_SwooshSource *) pSwoosh->GetEngineData());	// data fix-up usually done by dbl loader

			/* restart emitter-- have to because swoosh emitters are built once up front, so changes to swoosh */
			/* won't take effect until swoosh emitter is rebuilt */

			SwooshMgr__KillEmitter(pGameSwooshEmitter);
			pGameSwooshEmitter = Preview__CreateSwooshEmitter();

			/* successfully updated record */

			return;
		}
	}

	/* we need to rebuild .dbl */

	Preview__ReexportDBL(false);
}

void Preview__UpdateSwooshEmitter(CPigpenSwooshEmitter *pSwooshEmitter)
{
	/* maybe nothing to do */

	if (GameEngine__IsRunning() == false)
		return;

	/* try to update record in place */

	if (pSwooshEmitter->GetEngineData())
	{
		/* we are going to write directly over the data the game engine is running, so we can't */
		/* write any larger than the data that was originally exported. it is up to the exporting */
		/* routines to export a full data record when exporting for preview */

		if (pSwooshEmitter->FillExportBuffer((char *) pSwooshEmitter->GetEngineData(), pSwooshEmitter->GetExportDataSize(), true) != 0)
		{
			/* restart emitter */

			SwooshMgr__KillEmitter(pGameSwooshEmitter);
			ASSERT(pPreviewSwooshEmitter);
			pPreviewSwooshEmitter = pSwooshEmitter;
			pGameSwooshEmitter = Preview__CreateSwooshEmitter();

			/* successfully updated record */

			return;
		}
	}

	/* we need to rebuild .dbl */

	Preview__ReexportDBL(false);
}

/* one or the other */

void Preview__UpdateParticleOrSwoosh(CParticle *pParticle, CSwoosh *pSwoosh)
{
	ASSERT((pParticle) || (pSwoosh));
	if (pParticle)
		Preview__UpdateParticle(pParticle);
	else
		Preview__UpdateSwoosh(pSwoosh);
}

/* background color control */

void Preview__SetBackgroundColor(COLORREF Color)
{
	BackgroundColor = Color;

	/* send to doc if still open for saving in prefs */

	if (pPreviewDoc)
		pPreviewDoc->SetBackgroundColor(Color);

	/* update in engine */

	UpdateBackgroundColor(Color);
}
COLORREF Preview__GetBackgroundColor(void)
{
	return(BackgroundColor);
}

/* set camera position to use for initialization */

void Preview__SetCameraInitPos(void)
{
	_ASSERT(GameEngine__IsRunning());	// preview dialog and preview window are tied together
	InitCameraPos = vCameraPosition;
	InitCameraRotate.x(fCameraRotateX);
	InitCameraRotate.y(fCameraRotateY);

	/* send to doc if still open for saving in prefs */
	
	if (pPreviewDoc)
		pPreviewDoc->SetCameraPos(InitCameraPos, InitCameraRotate);
}
Vector3 *Preview__GetCameraPos(void)
{
	/* current camera position */

	_ASSERT(GameEngine__IsRunning());	// preview dialog and preview window are tied together
	return(&vCameraPosition);
}
CString &Preview__GetStageFileAndStatus(void)
{
	static CString StageFileAndStatus;

	StageFileAndStatus = StageFile;
	if (WorldDBHandle == INVALID_HANDLE)
		StageFileAndStatus += " (not found)";
	return(StageFileAndStatus);
}
CString &Preview__GetEmitterModelFileAndStatus(void)
{
	static CString EmitterModelFileAndStatus;

	EmitterModelFileAndStatus = EmitterModelFile;
	if (EmitterModelDBHandle == INVALID_HANDLE)
		EmitterModelFileAndStatus += " (not found)";
	return(EmitterModelFileAndStatus);
}

/* access to local variables */

void *Preview__GetDataPointer(int nDataID)
{
	switch(nDataID)
	{
	case PREVIEW_FLAGS:
		return(&ulPreviewFlags);
	case PREVIEW_DELAY:
		return(&fPreviewDelay);
	case PREVIEW_YPOS:
		return(&fPreviewYPos);
	case PREVIEW_EMITTER_VEL:
		return(&fPreviewEmitterVel);
	case PREVIEW_MODEL_XPOS:
		return(&fEmitterModelXPos);
	case PREVIEW_MODEL_YPOS:
		return(&fEmitterModelYPos);
	case PREVIEW_MODEL_ZPOS:
		return(&fEmitterModelZPos);
	case PREVIEW_MODEL_YROT:
		return(&fEmitterModelYRot);
	case PREVIEW_WIND_MPH:
		return(&fPreviewWindMPH);
	}
	_ASSERT(FALSE);	// all requests must be handled
	return(NULL);
}

/* update emission delay */

void Preview__UpdateDelay(void)
{
	/* maybe nothing to do */

	if (EmitterHandle == INVALID_HANDLE)
		return;

	/* update game engine emitter */

	if (GameEngine__IsRunning())
		ParticleEmitter__UpdateDelay(EmitterHandle, fPreviewDelay);

	/* active emitter in doc gets updated too to stay in sync */

	if ((pPreviewEmitter) && (fPreviewDelay != pPreviewEmitter->GetDelay()))
	{
		pPreviewEmitter->SetDelay(fPreviewDelay);
		pPreviewDoc->SetModifiedFlag(MODIFIED_FLAG);
		pPreviewDoc->UpdateAllViews(NULL, NO_REORDER_EMITTERS_VIEW | NO_CHANGE_EFFECTS_VIEW | NO_CHANGE_PARTICLES_VIEW | NO_CHANGE_GRAPHICS_VIEW | NO_CHANGE_SWOOSHES_VIEW | NO_CHANGE_SWOOSH_EMITTERS_VIEW);
	}
}

/* update emitter velocity */

#define HOURS_PER_SECOND	(1.0f / (60.0f * 60.0f))
#define INCHES_PER_MILE		(5280.0f * 12.0f)
#define METERS_PER_INCH		(1.0f / 39.0f)
void Preview__UpdateEmitterVel(void)
{
	float metersPerSecond = fPreviewEmitterVel;
	if (Preview__EmitterVelIsMPH())
		metersPerSecond *= HOURS_PER_SECOND * INCHES_PER_MILE * METERS_PER_INCH;	// from mph to meters/sec
	vEmitterVel.z(-metersPerSecond);
}

/* update emitter start y-position */

void Preview__UpdateEmitterYPos(void)
{
	vEmitterPos.y(fPreviewYPos);
}

/* stop the emitter */

void Preview__ClearEmitterVel(void)
{
	fPreviewEmitterVel = 0.0;
	Preview__UpdateEmitterVel();
}

/* change emitter velocity units */

void Preview__ToggleEmitterVelUnits(void)
{
	bEmitterVelIsMPH ^= true;
	if (Preview__EmitterVelIsMPH())
		fPreviewEmitterVel /= HOURS_PER_SECOND * INCHES_PER_MILE * METERS_PER_INCH;	// from meters/sec to mph
	else
		fPreviewEmitterVel *= HOURS_PER_SECOND * INCHES_PER_MILE * METERS_PER_INCH;	// from mph to meters/sec
}

/* update wind velocity */

void Preview__UpdateWindMPH(void)
{
	/* remove possibly existing wind */

	DistortionMgr__KillDistortionsByType(DISTORTION_GLOBAL_WIND);

	/* add new wind */

	Vector3 vWind = Vector3(0.0f, 0.0f, 0.0f);
	vWind.x(fPreviewWindMPH * HOURS_PER_SECOND * INCHES_PER_MILE * METERS_PER_INCH);	// from mph to meters/sec
	DistortionMgr__AddGlobalWind(
		&vWind,			//Vector3 *vStrength  
		0.0f,				//float Randomness     
		0.0f,				//float SinAngleStart  
		0.0f,				//float SinAngleInc    
		0.0f,				//float SinMin         
		0.0f,				//float SinMax         
		0.0f,				//float Damping        
		0.0f,				//float Lifetime       
		0,					//int nDistortiou8Flags
		0);				//int u8Flags          
}

/* reset stuff */

void Preview__ResetEmitter(void)
{
	Vector3 vLastPos = vEmitterPos;
	vEmitterPos.Set(0.0f, fPreviewYPos, 0.0f);
	vEmitterVel.y(0.0f);	// but don't clear ground movement

	/* restart emitter if it has a non-zero lifetime. if pLastEmitter is NULL, then this is a preview of either a */
	/* particle or an effect, and the emitter is guranteed to have an infinite lifetime */

	if ((EmitterHandle != INVALID_HANDLE) && (pLastEmitter))
	{
		float *pfLifetime = (float *) pLastEmitter->GetDataPointer(CEmitter::DATA_LIFE);
		if (*pfLifetime != 0.0f)
		{
			ParticleEmitter__Kill(EmitterHandle);
			EmitterHandle = Preview__CreateEmitter();
			ParticleEmitter__UpdateDelay(EmitterHandle, fPreviewDelay);	// make sure current delay is effective-- this routine does not rebuild the export buffer
		}
	}

	/* reset camera too if following  (but it doesn't bounce, so don't adjust y) */

	if (ulPreviewFlags & PREVIEW_CAMERA_FOLLOWS)
	{
		vCameraPosition.x(vCameraPosition.x() + (vEmitterPos.x() - vLastPos.x()));
		vCameraPosition.z(vCameraPosition.z() + (vEmitterPos.z() - vLastPos.z()));
		vCameraTarget.x(vCameraTarget.x() + (vEmitterPos.x() - vLastPos.x()));
		vCameraTarget.z(vCameraTarget.z() + (vEmitterPos.z() - vLastPos.z()));
	}
}

/* initialize camera position */

void Preview__ResetCamera(void)
{
	vCameraPosition = InitCameraPos;
	bCameraFirstInput = true;
	fCameraRotateX = InitCameraRotate.x();
	fCameraRotateY = InitCameraRotate.y();
}

/* a doc was closed-- was it the preview doc? */

void Preview__DocClosed(CPigpenDoc *pDoc)
{
	if (pDoc == pPreviewDoc)
		pPreviewDoc = NULL;
}

/* preview dialog was closed */

void Preview__DialogClosed(void)
{
	pPreviewDialog = NULL;
}

/* load a new stage */

void Preview__LoadNewStage(void)
{
	/* let user pick-- get last file name from registry */

	char InitialPath[_MAX_PATH];
	char Filename[_MAX_PATH];
	LoadPersistentString("Pigpen Stage", InitialPath, sizeof(InitialPath));
	DissectPath(InitialPath, InitialPath, Filename);

	/* file dialog */

	CFileDialog FileDialog(
		TRUE,						// BOOL bOpenFileDialog
		"dbl",					// LPCTSTR lpszDefExt = NULL
		Filename,				// LPCTSTR lpszFileName = NULL
		OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST,	// DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT-- HIDEREADONLY hides a dialog check box, not read only files
		".DBL Files (*.dbl)|*.dbl||",		// LPCTSTR lpszFilter = NULL, apparently can't have spaces around | symbols
		NULL);					// CWnd* pParentWnd = NULL
	FileDialog.m_ofn.lpstrInitialDir = InitialPath;	// set initial directory
	if (FileDialog.DoModal() != IDOK)
		return;

	/* get file name, save in registry */

	StageFile = FileDialog.GetPathName();
	MakePersistentString("Pigpen Stage", StageFile.GetBuffer(10));

	/* clear current stage */

	ASSERT(WorldSceneHandle != INVALID_HANDLE);
	SceneMgr__KillScene(WorldSceneHandle);
	if (WorldDBHandle != INVALID_HANDLE)
		dblRemove(WorldDBHandle);

	/* load new stage */

	LoadStage();

	/* send to doc if still open for saving in prefs */
	
	if (pPreviewDoc)
		pPreviewDoc->SetStageFile(StageFile);
}

/* load a new emitter model */

void Preview__LoadNewEmitterModel(void)
{
	/* let user pick-- get last file name from registry */

	char InitialPath[_MAX_PATH];
	char Filename[_MAX_PATH];
	LoadPersistentString("Pigpen Emitter Model", InitialPath, sizeof(InitialPath));
	DissectPath(InitialPath, InitialPath, Filename);

	/* file dialog */

	CFileDialog FileDialog(
		TRUE,						// BOOL bOpenFileDialog
		"dbl",					// LPCTSTR lpszDefExt = NULL
		Filename,				// LPCTSTR lpszFileName = NULL
		OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST,	// DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT-- HIDEREADONLY hides a dialog check box, not read only files
		".DBL Files (*.dbl)|*.dbl||",		// LPCTSTR lpszFilter = NULL, apparently can't have spaces around | symbols
		NULL);					// CWnd* pParentWnd = NULL
	FileDialog.m_ofn.lpstrInitialDir = InitialPath;	// set initial directory
	if (FileDialog.DoModal() != IDOK)
		return;

	/* get file name, save in registry */

	EmitterModelFile = FileDialog.GetPathName();
	MakePersistentString("Pigpen Emitter Model", EmitterModelFile.GetBuffer(10));

	/* clear current emitter model */

	UnloadEmitterModel();

	/* load new emitter model */

	LoadEmitterModel();

	/* send to doc if still open for saving in prefs */
	
	if (pPreviewDoc)
		pPreviewDoc->SetEmitterModelFile(EmitterModelFile);
}

/* clear emitter model */

void Preview__ClearEmitterModel(void)
{
	/* clear current emitter model */

	UnloadEmitterModel();

	/* send to doc if still open for saving in prefs */
	
	EmitterModelFile = "";
	if (pPreviewDoc)
		pPreviewDoc->SetEmitterModelFile(EmitterModelFile);
}

/* build the transform matrix for the emitter model */

void Preview__BuildEmitterModelTransform(bool bUpdateDoc)
{
	if (pEmitterModelInstance == NULL)
		return;
	if (GameEngine__IsRunning() == FALSE)
		return;

	/* ok, this is easy */

	pEmitterModelInstance->mLocalToWorld.SetYRotation(Math::Deg2Rad(fEmitterModelYRot));
	pEmitterModelInstance->mLocalToWorld.tx = fEmitterModelXPos + vEmitterPos.x();
	pEmitterModelInstance->mLocalToWorld.ty = fEmitterModelYPos + vEmitterPos.y();
	pEmitterModelInstance->mLocalToWorld.tz = fEmitterModelZPos + vEmitterPos.z();

	/* update bounding box/sphere */

	BoundingBox__Transform(&pEmitterModelInstance->pSOM->BoundingBox,
								  &pEmitterModelInstance->BoundingBox,
								  &pEmitterModelInstance->mLocalToWorld);
	Instance.BoundingSphere = pEmitterModelInstance->pSOM->BoundingSphere;
	Instance.BoundingSphere.Center = pEmitterModelInstance->pSOM->BoundingSphere.Center * pEmitterModelInstance->mLocalToWorld;

	/* need to update doc's values? */

	if ((bUpdateDoc) && (pPreviewDoc))
	{
		pPreviewDoc->SetEmitterModelXPos(fEmitterModelXPos);
		pPreviewDoc->SetEmitterModelYPos(fEmitterModelYPos);
		pPreviewDoc->SetEmitterModelZPos(fEmitterModelZPos);
		pPreviewDoc->SetEmitterModelYRot(fEmitterModelYRot);
	}
}

/* mph or meters/sec in preview dialog */

bool Preview__EmitterVelIsMPH()
{
	return(bEmitterVelIsMPH);
}

/* create the emitter-- always id 0, since only one emitter exported for preview */

static t_Handle Preview__CreateEmitter()
{
	/* maybe nothing to do */

	if (strlen(PreviewEmitterName) == 0)
		return(INVALID_HANDLE);

	Vector3 *pvUp = NULL;
	if (pPreviewEmitter)	// if NULL, it was a dummy emitter, dummy emitters are always standard
	{
		if (pPreviewEmitter->GetType() == EMITTER_ORBITER)
			pvUp = &vEmitterUp;
		else if (pPreviewEmitter->GetType() == EMITTER_TIRE_CIRCLE)
			pvUp = &vEmitterSide;
	}
	Vector3 *pvEmitterVel = &vEmitterVel;
	if ((pPreviewEmitter) && (pPreviewEmitter->GetType() == EMITTER_FLUX))
		pvEmitterVel = NULL;
	t_Handle Handle = ParticleEmitter__CreateFromRecord(
		PreviewEmitterName,						// char *pEmitterName,
		&vEmitterPos,								// Vector3 *pvEmitterPos,
		pvEmitterVel,								// Vector3 *pvEmitterVel,
		NULL,											// Vector3 *pvEjectVel,-- use particle data
		FALSE,										// bool bEjectVelChanges,
		pvUp,											// Vector3 *pvUp,
		FALSE);										// bool bStationary);
	return(Handle);
}

/* create the swoosh emitter */

static CSwooshEmitter *Preview__CreateSwooshEmitter()
{
	/* maybe nothing to do */

	if (strlen(PreviewSwooshEmitterName) == 0)
		return(NULL);

	CSwooshEmitter *pSwooshEmitter = SwooshMgr__CreateEmitterFromRecord(
		PreviewSwooshEmitterName,		// char *pSwooshEmitterName,
		&vEmitterPos,						// Vector3 *pvPos,
		false);								// bool bStationary);
	return(pSwooshEmitter);
}

/* unload current emitter model, if any */

static void UnloadEmitterModel(void)
{
	/* clear current emitter model */

	if (pEmitterModelInstance)
	{
		RemoveBoneData(pEmitterModelInstance);
		SceneMgr__RemoveInstance(pEmitterModelInstance);
		pEmitterModelInstance = NULL;
		_ASSERT(EmitterModelDBHandle != INVALID_HANDLE);
		dblRemove(EmitterModelDBHandle);
	}
}

/* update background color in engine */

static void UpdateBackgroundColor(COLORREF Color)
{
	ts_bRGBA ClearColor;

	_ASSERT(GameEngine__IsRunning());	// preview dialog and preview window are tied together
	ClearColor.R = (u8) (Color & 0xff);
	ClearColor.G = (u8) ((Color >> 8) & 0xff);
	ClearColor.B = (u8) ((Color >> 16) & 0xff);
	ClearColor.A = 0xff;

	/* set default color */

	Display__SetClearColor(ClearColor);

	/* set color in scene view if there's a scene */

	ts_Scene *pScene = SceneMgr__GetFirstScene();
	if (pScene)
		pScene->SceneView.SetClearColor(ClearColor);
}

/* move the camera in reaction to keyboard, mouse input */

static void MoveCamera()
{
	Graphics4x4 mtxZRotation;
	static Vector3 vctCam;
	static float fMouseTranScale = 0.10f;
	static float fMouseRotScale = 0.5f;

	//try camera relative motion

	if( bCameraFirstInput || input.rx || input.ry)
	{
		Vector3 vctUnit = Vector3(0.0, 0.0, -1.0);

		fCameraRotateX += -input.rx * fMouseRotScale;
		fCameraRotateY += input.ry * fMouseRotScale;

		mtxZRotation.Identity();
		mtxZRotation.ReverseCatYRotation(Math::Deg2Rad(fCameraRotateX));
		mtxZRotation.ReverseCatXRotation(Math::Deg2Rad(-fCameraRotateY));
		vctCam = vctUnit * mtxZRotation;

		vCameraTarget = vctCam + vCameraPosition;
		input.rx=0;
		input.ry=0;
	}

	if( bCameraFirstInput || input.tx || input.ty)
	//translate: up/down keys = forward, backward
	{
		Vector3 vAdjust = fMouseTranScale * input.ty * vctCam * g_timer.GetOSFrameSec();
  		vCameraPosition += vAdjust;
  		vCameraTarget += vAdjust;

		//translate: left/right keys = left, right
		vAdjust = fMouseTranScale * -input.tx * vctCam * g_timer.GetOSFrameSec();
  		vCameraPosition.x(vCameraPosition.x() + vAdjust.z());
  		vCameraPosition.z(vCameraPosition.z() - vAdjust.x());
  		vCameraTarget.x(vCameraTarget.x() + vAdjust.z());
  		vCameraTarget.z(vCameraTarget.z() - vAdjust.x());
	}
	bCameraFirstInput = false;

	/* create world-to-camera matrix. ripped out of Camera/Camera.cpp (combines LookAt, BuildMatrixWTL, TransformWTL) */
	
	Vector3 axisZ = vCameraPosition - vCameraTarget;
	axisZ.Normalize();

	Vector3 axisY = vUp;
	axisY -= (axisZ * Vector3::Dot(axisY, axisZ));
	axisY.SafeNormalize();

	Vector3 axisX = Vector3::Cross(axisY, axisZ);

	Vector3 point = vCameraPosition * -1.0f;
	Vector3 pointOut;
	pointOut.x((axisX.x() * point.x()) + (axisX.y() * point.y()) + (axisX.z() * point.z()));
	pointOut.y((axisY.x() * point.x()) + (axisY.y() * point.y()) + (axisY.z() * point.z()));
	pointOut.z((axisZ.x() * point.x()) + (axisZ.y() * point.y()) + (axisZ.z() * point.z()));

	g4PigpenWorldToCamera.Set(
		axisX.x(),	axisY.x(), axisZ.x(),
		axisX.y(),	axisY.y(), axisZ.y(),
		axisX.z(),	axisY.z(), axisZ.z(),
		pointOut.x(),	pointOut.y(), pointOut.z());
}

/* load stage dbl file for background */

static void LoadStage(void)
{
	/* register scene for rendering */

	WorldSceneHandle = SceneMgr__RegisterScene(&g4PigpenWorldToCamera);
	Scene__EnableForRendering(WorldSceneHandle, true);

	/* make a weak attempt to find file. the intention here is that you could give your stage */
	/* file to someone else and as long as they put it in their pigpen executable directory we'll */
	/* be ok */

	char FileName[_MAX_PATH];
	strncpy(FileName, StageFile.GetBuffer(10), _MAX_PATH);
	FILE *pfTemp = NULL;
	if (strlen(FileName) != 0)
		pfTemp = fopen(FileName, "rt");
	if (pfTemp)
		fclose(pfTemp);
	else
	{
		/* not found. look in executable path */

		char acDrive[_MAX_DRIVE];
		char acDir[_MAX_DIR];
		char acRoot[_MAX_FNAME];
		char acExt[_MAX_EXT];
		_splitpath(PigpenApp.GetDefaultStageFile().GetBuffer(10), acDrive, acDir, NULL, NULL);
		_splitpath(StageFile.GetBuffer(10), NULL, NULL, acRoot, acExt);
		_makepath(FileName, acDrive, acDir, acRoot, acExt);
	}

	/* load stage */

	ASSERT(WorldDBHandle == INVALID_HANDLE);
	WorldDBHandle = dblLoad(FileName);
	if (WorldDBHandle != INVALID_HANDLE)
	{
		RenderMgr__SetWorldDBHandle(WorldDBHandle);	// let render manager know about world database for texture context management
		CSceneNode *pSceneTree = (CSceneNode *) dblGetChunkData(DB_SCENE_TREE, DEFAULT_CHUNK_KEY_STRING, WorldDBHandle);
		if (pSceneTree == NULL)
			return;
		ts_RecordList *pVisRegionList = (ts_RecordList *) dblGetChunkData(DB_VIS_REGIONS, DEFAULT_CHUNK_KEY_STRING, WorldDBHandle);	// ok if NULL
		SceneMgr__SetSceneTree(pSceneTree, pVisRegionList);
	}
	else
	{
		/* use a dummy scene tree so that particles render even though there's no stage-- particles get rendered as part of */
		/* scene, and scene won't render without a valid scene tree */

		SceneMgr__SetSceneTree(SceneMgr__EmptySceneTree(), NULL);
	}

	/* make particle texture set the active texture set again for preview updates (which call _FixParticleSource, */
	/* which fixes up bind numbers, which requires that the particle texture set be the active texture set) */

	if (ParticleMgr__GetTextureSet(0))
		TextureMgr__MakeTextureSetActive(ParticleMgr__GetTextureSet(0));
}

/* load emitter model dbl file for effect development-- for example, can load a bike to work on tire spray */

static void LoadEmitterModel(void)
{
	/* previous emitter model, if any, should already ave been cleaned up */

	_ASSERT(pEmitterModelInstance == NULL);

	/* make a weak attempt to find file. the intention here is that you could give your */
	/* file to someone else and as long as they put it in their pigpen executable directory we'll */
	/* be ok */

	char FileName[_MAX_PATH];
	strncpy(FileName, EmitterModelFile.GetBuffer(10), _MAX_PATH);
	if (strlen(FileName) == 0)
		return;
	FILE *pfTemp = fopen(FileName, "rt");
	if (pfTemp)
		fclose(pfTemp);
	else
	{
		/* not found. look in executable path */

		char acDrive[_MAX_DRIVE];
		char acDir[_MAX_DIR];
		char acRoot[_MAX_FNAME];
		char acExt[_MAX_EXT];
		_splitpath(PigpenApp.GetDefaultStageFile().GetBuffer(10), acDrive, acDir, NULL, NULL);
		_splitpath(EmitterModelFile.GetBuffer(10), NULL, NULL, acRoot, acExt);
		_makepath(FileName, acDrive, acDir, acRoot, acExt);
	}

	/* load emitter model-- may not be there */

	EmitterModelDBHandle = dblLoad(FileName);
	if (EmitterModelDBHandle == INVALID_HANDLE)
		return;

	/* find a som to use */

	ts_RecordList *pRecordList;
	ts_PatchSOM *pSOM;
	pRecordList = (ts_RecordList *) dblGetChunkData(DB_SOMARRAY, DEFAULT_CHUNK_KEY_STRING, EmitterModelDBHandle);
	if (!pRecordList)
		pRecordList = (ts_RecordList *) dblGetChunkData(DB_SOMARRAYBONE, DEFAULT_CHUNK_KEY_STRING, EmitterModelDBHandle);
	if (pRecordList)
	{
		//an array found, use first element
		ASSERT(pRecordList);
		pSOM = (ts_PatchSOM *) GetRecordAddress(pRecordList, 0);
	}
	else
	{
		//no array found: get the single SOM
		pSOM = (ts_PatchSOM *) dblGetChunkData(DB_SNGLSOM, DEFAULT_CHUNK_KEY_STRING, EmitterModelDBHandle);
		if (!pSOM)
			pSOM = (ts_PatchSOM *) dblGetChunkData(DB_SNGLSOMBONE, DEFAULT_CHUNK_KEY_STRING, EmitterModelDBHandle);
	}
	if (!pSOM)
		return;

	/* set up emitter model instance and add to scene */

	pEmitterModelInstance = &Instance;
	memset(&Instance, 0, sizeof(ts_Instance));
	memset(&InstancePart2, 0, sizeof(ts_InstancePart2));

	/* Setup the link to the second half of the instance record */

	Instance.pInstancePart2 = &InstancePart2;

	/* clear instance flags */

	Instance.dh.u8DataType = DATA_INSTANCE;
	Instance.dh.u16Flags = 0;

	/* set geometry pointer-- either an LOD record or a SOM pointer */

	Instance.pSOM = (ts_PatchSOM *) pSOM;
	Instance.fConstantAlpha = 1.0f;

	/* instance is not active in scene yet */

	Instance.pInstancePart2->u32UseCount = 0;
	for (int i = 0; i < MAX_VIEWS; i++)
		Instance.pInstancePart2->SceneInstHandle[i] = INVALID_HANDLE;

	/* transform matrix */

	Preview__BuildEmitterModelTransform(FALSE);

	/* name */

	strcpy(Instance.pInstancePart2->Name, "Emitter model");

	/* maybe boned */

	Instance__MakeBones(&Instance);

	/* add to scene */

	_ASSERT(WorldSceneHandle != INVALID_HANDLE);
	SceneMgr__AddInstance(pEmitterModelInstance);

	/* make particle texture set the active texture set again for preview updates (which call _FixParticleSource, */
	/* which fixes up bind numbers, which requires that the particle texture set be the active texture set) */

	if (ParticleMgr__GetTextureSet(0))
		TextureMgr__MakeTextureSetActive(ParticleMgr__GetTextureSet(0));
}

/* routine to call to test if group is in view */

static bool ShowGroup( ts_Group *pGroup)
{
	return(Frustum__IntersectsWorldBounds(SceneMgr__GetCurrentSceneView()->GetViewFrustum(), &pGroup->BoundingSphere, &pGroup->BoundingBox, NULL, NULL));
}	

/* set up a single group that consists of a single instance to handle non-som-array dbl files */

static void SetupDummyGroup(t_Handle WorldDBHandle, ts_Group **ppGroupArray, int *pnGroupCount)
{
	int i;
	ts_PatchSOM *pPatchSOM;
	ts_RecordList *pRecordList;
	static ts_Group Group;
	static ts_Instance Instance;
	static ts_InstancePart2 InstancePart2;
	static ts_Instance *DispStatArray[1];

	/* clear dummy records */

	memset(&Group, 0, sizeof(ts_Group));
	memset(&Instance, 0, sizeof(ts_Instance));
	memset(&InstancePart2, 0, sizeof(ts_InstancePart2));

	/* link instance records, name the instance */

	Instance.pInstancePart2 = &InstancePart2;
	strcpy(InstancePart2.Name, "Dummy");

	/* does dbl have an LOD chunk, or just a SOM chunk? */

	pRecordList = (ts_RecordList *) dblGetChunkData(DB_LOD, DEFAULT_CHUNK_KEY_STRING, WorldDBHandle);
	if (pRecordList)
	{
		ASSERT(pRecordList->usRecordCount == 1);
		Instance.pLOD = (ts_LOD *) GetRecordAddress(pRecordList, 0);
		Instance.dh.u16Flags |= INSTANCE_HAS_LOD;
		pPatchSOM = Instance.pLOD->pSOM[0];	// highest LOD model
	}
	else
	{
		pPatchSOM = (ts_PatchSOM *) dblGetChunkData(DB_SNGLSOMBONE, DEFAULT_CHUNK_KEY_STRING, WorldDBHandle);
		if (pPatchSOM == NULL)
			pPatchSOM = (ts_PatchSOM *) dblGetChunkData(DB_SNGLSOM, DEFAULT_CHUNK_KEY_STRING, WorldDBHandle);
		ASSERT(pPatchSOM);
		Instance.pSOM = pPatchSOM;
	}

	/* set instance transform matrix */

	Instance.mLocalToWorld.Identity();

	/* instance's scene inst handles should be cleared */

	for (i = 0; i < MAX_VIEWS; i++)
		InstancePart2.SceneInstHandle[i] = INVALID_HANDLE;

	/* set bounding boxes */

	Instance.BoundingBox = pPatchSOM->BoundingBox;
	Instance.BoundingSphere = pPatchSOM->BoundingSphere;
	Group.BoundingBox = pPatchSOM->BoundingBox;
	Group.BoundingSphere = pPatchSOM->BoundingSphere;

	/* set up list of displayable instances */

	Group.usDispStatInst = 1;
	Group.ppDispStatArray = DispStatArray;
	DispStatArray[0] = &Instance;

	/* and fill in pointers to dummy group */

	*ppGroupArray = &Group;
	*pnGroupCount = 1;
}

/* window proc for controlling messages passed to game engine window */

static s32 FAR PASCAL WindowProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static short lastX, lastY, bFirst=TRUE;

	switch( message)
	{
	case WM_CREATE:
		if (gfxOnCreate(hwnd) == FALSE)
			return -1;
		if (gfxOnCreated(hwnd) == FALSE)
			return -1;
		bActivateApp = TRUE;
		break;
	
	case WM_SIZE:
		gfxOnResize( LOWORD( lParam), HIWORD( lParam));
		break;

	case WM_DESTROY:
		/* user closed the preview window */

		GameEngine__Shutdown();
		gfxOnDestroy( );

		/* destroy preview dialog */

		if (pPreviewDialog)
		{
			pPreviewDialog->DestroyWindow();
			pPreviewDialog = NULL;
		}

		/* no more spline path */

		if (pSplinePath)
		{
			delete pSplinePath;
			pSplinePath = NULL;
		}

		/* no more "parent" doc */

		pPreviewDoc = NULL;
		break;

	case WM_PAINT:
		#ifdef DIRECTX
		{
		    // N8 - !!!
		    ValidateRect( hwnd, NULL );
		}
		#endif
		break;

	case WM_KEYDOWN:
		if (~lParam & (1 << 24))	//extended key?
		{
			switch (wParam)	// not extended key
			{
			case 'W':
				RenderMgr__ToggleWireframe();
				break;
			case 'I':
				RenderMgr__ToggleLighting();
				break;
			case 'R':
				/* reset camera position in case they get lost */

				Preview__ResetCamera();
				break;
			case ' ':
				ParticleEmitter__ClearTimer(EmitterHandle);
				break;
			}
		}
		else
		{
			/* navigation */

			switch (wParam)	// extended key
			{
			case VK_LEFT:
				input.tx -= INC_SPEED;
				input.tx = max( input.tx, -MAX_SPEED);
				break;
			case VK_RIGHT:
				input.tx += INC_SPEED;
				input.tx = min( input.tx, MAX_SPEED);
				break;
			case VK_DOWN:
				input.ty -= INC_SPEED;
				input.ty = max( input.ty, -MAX_SPEED);
				break;
			case VK_UP:
				input.ty += INC_SPEED;
				input.ty = min( input.ty, MAX_SPEED);
				break;
			}
		}
		break;

	case WM_KEYUP:
		if (~lParam & (1 << 24))	//extended key?
		{
		}
		else
		{
			/* navigation */

			switch (wParam)	// extended key
			{
			case VK_LEFT:
				input.tx = 0;
				break;
			case VK_RIGHT:
				input.tx = 0;
				break;
			case VK_DOWN:
				input.ty = 0;
				break;
			case VK_UP:
				input.ty = 0;
				break;
			}
		}
		break;

	case WM_MOUSEWHEEL:
		input.buttons = LOWORD(wParam);    // key flags
		input.rz = input.mzlast - (short) HIWORD(wParam);    // wheel rotation
		input.mx = (short) LOWORD(lParam);    // horizontal position of pointer
		input.my = (short) HIWORD(lParam);    // vertical position of pointer
		input.mzlast = (short) HIWORD(wParam);

		break;

  	case WM_CAPTURECHANGED:
		break;

	case WM_LBUTTONDOWN:
		input.buttons |= L_MBUTTON;
		SetCapture( hwnd);
		bFirst = TRUE;
		break;

	case WM_LBUTTONUP:
		input.buttons &= ~L_MBUTTON;
		ReleaseCapture( );
		input.tz = 0;
		break;

	case WM_RBUTTONDOWN:
		input.buttons |= R_MBUTTON;
		SetCapture( hwnd);
		break;

	case WM_RBUTTONUP:
		input.buttons &= ~R_MBUTTON;
		ReleaseCapture( );
		break;

	case WM_MBUTTONDOWN:
		input.buttons |= M_MBUTTON;
		input.mx = (short)LOWORD(lParam); 
		input.my = (short)HIWORD(lParam);
		SetCapture( hwnd);
		break;

	case WM_MBUTTONUP:
		input.buttons &= ~M_MBUTTON;
		ReleaseCapture( );
		input.tz = 0;
		input.rz = 0;
		break;
		
	case WM_MOUSEMOVE:
		if( input.buttons & L_MBUTTON)
		{
			if( bFirst)
			{
				//first time thru
				lastX = (short)LOWORD(lParam);
				lastY = (short)HIWORD(lParam);
				bFirst = FALSE;
			}
			else
			{
				input.rx = (short)LOWORD(lParam) - lastX;
				input.ry = (short)HIWORD(lParam) - lastY;
				lastX = (short)LOWORD(lParam);
				lastY = (short)HIWORD(lParam);
			}
		}
		break;
	default:
		break;

	}

	return DefWindowProc( hwnd, message, wParam, lParam);
}

/* build a test spline path to see the results of attaching particles to a path */

static void BuildTestPath()
{
	/* malloc room for points that define the new random path, including control points */

	int nSourcePoints = 2;
	int nNewPoints = (nSourcePoints * 3) - 2;	// last point doesn't need control points beyond it
	Vector3 *pvPoints = new Vector3[nNewPoints];

	/* hard-wired points */

	pvPoints[0].Set(0.0f, 3.0f, 0.0f);
	pvPoints[1].Set(1.0f, 3.0f, 0.0f);
	pvPoints[2].Set(4.0f, 1.0f, 0.0f);
	pvPoints[3].Set(5.0f, 1.0f, 0.0f);
	_ASSERT(nNewPoints == 4);

	/* create the path */

	pSplinePath = new SplinePath("test", pvPoints, nNewPoints, true);
}