/**********************************************/
/*                                            */
/* EnvEffects.cpp                             */
/* big juju  4/16/03                          */
/* ned martin  avalanche software             */
/* visual c++ 6.0                             */
/* environmental effects                      */
/*                                            */
/**********************************************/

/* coordinate actors and particles to create combination effects, typically using simple geometry */

/******************* includes ****************************/

#include "TTLPCH.h"

#include "environ/EnvEffects.h"
#include "environ/ActorCreator.h"
#include "stage/ActorCreator.h"

#include "Display/Common/TextureAnimatorArray.h"

/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

/******************* externs *****************************/

/******************* global variables ********************/

/******************* local variables *********************/

static char *pSplashName[CEnvEffects::SPLASH_COUNT] = {"SplashExplode", "SplashTak", "SplashSmall"};
static char *pSplashEffect[CEnvEffects::SPLASH_COUNT] = {"SplashExpld", "SplashTak", "SplashSm"};
static Vector3 SplashScale[CEnvEffects::SPLASH_COUNT] = {Vector3(2.0f, 3.0f, 2.0f), Vector3(1.7f, 2.2f, 1.7f), Vector3(1.0f, 1.0f, 1.0f)};

/******************* local prototypes ********************/

/******************* functions ***************************/

/* Constructor/destructor */

CEnvEffects::CEnvEffects()
{
#ifndef CDROM
	/* give user control over initial scale through console */

//	g_console.CreateVar("splash", "explodex", SplashScale[SPLASH_EXPLODE].x());
//	g_console.CreateVar("splash", "explodey", SplashScale[SPLASH_EXPLODE].y());
//	g_console.CreateVar("splash", "explodez", SplashScale[SPLASH_EXPLODE].z());
//	g_console.CreateVar("splash", "takx", SplashScale[SPLASH_TAK].x());
//	g_console.CreateVar("splash", "taky", SplashScale[SPLASH_TAK].y());
//	g_console.CreateVar("splash", "takz", SplashScale[SPLASH_TAK].z());
//	g_console.CreateVar("splash", "smallx", SplashScale[SPLASH_SMALL].x());
//	g_console.CreateVar("splash", "smally", SplashScale[SPLASH_SMALL].y());
//	g_console.CreateVar("splash", "smallz", SplashScale[SPLASH_SMALL].z());
#endif //CDROM
}
CEnvEffects::~CEnvEffects()
{
#ifndef INTERACTIVE_CONSOLE
	/* clean up-- may have already been done by call to g_console.Clear() in ~SimLoop */

	g_console.DeleteContext("splash");
#endif //CDROM
}

bool CEnvEffects::CreateSplash(const SPLASH_TYPE Type, Vector3CRef vPos) const
{
	/* for now, since the splash textures are big, only do this if the splash .dbl has been pre-loaded */

	char FileName[100];
	sprintf(FileName, "Replace\\%s.dbl", pSplashName[Type]);
	if (dblFindFile(FileName) == INVALID_HANDLE)
		return(false);

	/* create the splash actor */

	CActor* pSplash = CActorCreator::Create( "Splash" );
	if (pSplash == NULL)
		return(false);
 
	/* place the splash */

	DirCos3x3 mOrientation;
	mOrientation.Identity();
	pSplash->Place(vPos, mOrientation);

	/* initialize */

//	pSplash->SetInitializeSourceData(pSplashName[Type]);	// set name so correct geometry will be loaded
	pSplash->Initialize();
//	pSplash->SetEnvType(GetEnvType("Splash"));				// must be done after initialize
//	pSplash->Category() |= GetActorCategory("Splash");

	/* no collision */

//	pSplash->MoveToNonProtagonistCast();

	/* draw last (after water surface), don't write to z-buffer (may have waterfall particles behind it) */
	pSplash->Instance()->SetRenderBucket( 2 );	// water is in 1
	pSplash->Instance()->SetZWriteDisabled( true );
	
	/* scale */

	Vector3 vScale = SplashScale[Type];
//	pSplash->SetScale(vScale);

	/* texture scrolling */

	// TJC - I scroll texture 0, since there is no way for me to know
	// what the original texture that was being scrolled was.  Hope Im
	// right... :)
	ASSERT_PTR(pSplash->Instance()->GetTextureAnimatorArray());
	pSplash->Instance()->GetTextureAnimatorArray()->GetOrCreateByTextureIndex(0)->SetTextureTransformUVel(0.5f);
 
	/* load motion */

	char MotionFileName[100];
	sprintf(MotionFileName, "Environ\\WaterEffects\\%s.mdb", pSplashName[Type]);
	pSplash->LoadMotion( MotionFileName, 0, 0 );
	pSplash->StartMotion( "NEEDNAMEHERE", 0, MOTNINF_FLAG_ONESHOT_AND_HOLD | MOTNINF_FLAG_NOINTERP);	// no interp so that base pose doesn't show up at start

	/* i want splash to fade in, then fade out again. to do this, i fade in normally, but use the CSimple's */
	/* fade-out-after-death mode to fade out again, so the MarkForDeath time has to exclude the fade-out time */

#define FADE_IN_TIME 0.1f
#define FADE_OUT_TIME 0.1f
#define ANIM_FRAMES 18
	pSplash->Instance()->SetAlpha( 0.0f );
//	pSplash->AlphaToTarget(1.0f, FADE_IN_TIME);
//	pSplash->SetFadeOutAfterDeathTime(FADE_OUT_TIME);
	pSplash->MarkForDeath(((float) ANIM_FRAMES / 30.0f) - FADE_OUT_TIME);	// exclude fade-out time from lifetime-- life will be extended to allow fade-out

	/* start particle effect */

	ParticleMgr__CreateEffect(
		pSplashEffect[Type],	// const char *pEffectName,
		&vPos,					// const Vector3 *pvPos,
		NULL,						// const Vector3 *pvVelUnitVector,
		NULL,						// const float *pfVelMagnitude,
		NULL,						// const Vector3 *pvAdditiveVel,
		0.0f						// const float fFirstAdvanceTime,
		// const float fScale = 1.0f,
		// const int nScaleRandomPct = 0,
		//	const float fConstantAlpha = 1.0f,
		//	const float fColorMult = 1.0f,
		// const u16 u16ForceFlags = 0,
		// const int iSceneIndex = -1);
	);

	/* success */

	return(true);
}

