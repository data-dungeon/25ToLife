//////////////////////////////////////////////////////
// Attract Mode - attractmode.cpp
//
// Paul Blagay  3/30/04
// Avalanche Software 2004
//////////////////////////////////////////////////////

#include "TTLPCH.h"
#include "attractmode.h"

#include "camera/SecurityCamera/SecurityCam.h"

//vars

static	SecurityCam*				AttractMode_SecurityCams = NULL;
static	bool							AttractMode_Active  = false;
static	CameraScene*				AttractMode_camScene = NULL;
static	float							AttractMode_Timeout;
static	float							AttractMode_InteruptTime;
static	float							AttractMode_Timer;
static	DefaultControlMapper*	AttractMode_defaultMapper;

#define DISABLE_ATTRACT_MODE			// turn off if u so please

//////////////////////////////////////////////////////

void AttractMode::Init( t_Handle worldHandle, CameraScene* camScene)
{
	#if defined (DISABLE_ATTRACT_MODE)
	return;
	#endif

	// create our instance
	AttractMode_SecurityCams = new SecurityCam;
	ASSERT(AttractMode_SecurityCams);

	// save of scene
	AttractMode_camScene = camScene;

	// start up the securitycams
	AttractMode_SecurityCams->Init( worldHandle, camScene );

	// add new control mapper
	AttractMode_defaultMapper = new DefaultControlMapper("Attract");
	ASSERT(AttractMode_defaultMapper);
	g_controlFocus.Add(AttractMode_defaultMapper, 0);
}

//////////////////////////////////////////////////////

void AttractMode::DeInit()
{
	#if defined (DISABLE_ATTRACT_MODE)
	return;
	#endif

	// close it down
	AttractMode_SecurityCams->DeInit();

	// kill the instance
	if (AttractMode_SecurityCams)
	{
		delete AttractMode_SecurityCams;
		AttractMode_SecurityCams = NULL;
	}

	if (AttractMode_defaultMapper)
	{
		g_controlFocus.Remove("Attract");
		AttractMode_defaultMapper = NULL;
	}
}

//////////////////////////////////////////////////////

void AttractMode::Start( bool Fade, float InteruptTime, float Timeout )
{
	#if defined (DISABLE_ATTRACT_MODE)
	return;
	#endif

	ASSERT (AttractMode_camScene);
	if (AttractMode_camScene)
	{
		if (AttractMode_SecurityCams->GetNumCams() != 0)
		{

			// timeout
			AttractMode_Timeout = Timeout;
			AttractMode_InteruptTime = InteruptTime;
			AttractMode_SecurityCams->SetFade( Fade );
			
			// setup cam and controller
			AttractMode_SecurityCams->PushGroup( AttractMode_camScene );
			g_controlFocus.Push(AttractMode_defaultMapper);
			g_cameraDirector.SelectCamera("SecurityCam", AttractMode_SecurityCams->GetCameraNameByIndex(0));
			AttractMode_Active = true;
		}
	}
}

//////////////////////////////////////////////////////

void AttractMode::Stop()
{
	#if defined (DISABLE_ATTRACT_MODE)
	return;
	#endif

	ASSERT (AttractMode_camScene);
	if (AttractMode_camScene)
	{
		if (AttractMode_SecurityCams->GetNumCams() != 0)
		{
			// fade out
			RenderMgr__FadeOut(0);

			// switch camera back to original cam
			AttractMode_SecurityCams->PopGroup(AttractMode_camScene);
			AttractMode_Active = false;

			RenderMgr__FadeIn(0);
		}
	}
}

//////////////////////////////////////////////////////

bool AttractMode::IsActive()
{
	return (AttractMode_Active);
}

//////////////////////////////////////////////////////

void AttractMode::Update (float i_deltasec )
{
	bool result;

	#if defined (DISABLE_ATTRACT_MODE)
	return;
	#endif

	if (AttractMode_Active)
	{
		result = AttractMode_SecurityCams->Update( i_deltasec );

		if (result)
			AttractMode_SecurityCams->NextCamera();

		// update timer
		AttractMode_Timer += i_deltasec;

		// timeout?
		if (AttractMode_Timer > AttractMode_Timeout)
			Stop();

		// only allow skip after 'x' seconds have passed
		if (AttractMode_Timer > AttractMode_InteruptTime)
		{
			// button press to exit
			for ( int i = 0; i < MAX_CONTROLLERS; i++)
			{
				// update his rotation bassd on the 2nd analog stick
				ControllerInputDriver *pad = g_input.GetController(i);
	
				if (pad && pad->AnyButtonPressed())
					Stop();
			}
		}
	}
}

