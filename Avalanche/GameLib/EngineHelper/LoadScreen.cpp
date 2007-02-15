///////////////////////////////////////////////////////////////////////////////
// Load screen!
///////////////////////////////////////////////////////////////////////////////
#include "EngineHelper/EngineHelperPCH.h"

#ifdef _XBOX
#include "Display/DirectX/XBResourceManager.h"
#endif
#include "Display/Common/DisplayState.h"
#include "Display/Common/GfxFunc.h"
#include "Game/Database/SceneNode.h"
#include "Game/Managers/RenderMgr.h"
#include "Game/Managers/SpriteMgr.h"
#include "Game/Managers/TextureMgr/TextureMgr.h"
#include "Layers/OSThreading/OSThread.h"
#include "Layers/Timing.h"
#include "hud/text.h"
#include "GameHelper/Translator.h"
#include "platform/systemi.h"

#include "EngineHelper/LoadScreen.h"

#if defined(PS2)
#include "platform/PS2/VU0RegisterPreserver.h"
#endif

// Define this for no load screen
// on TTL_PC, this wasn't working because the D3D device
// wasn't being specified to play nice with multithreaded apps
// after I changed this, the loading screen worked great.
// just a heads up.  
#if defined(WIN32) && !defined(_XBOX) && !defined(DIRECTX_PC)
//#define NOLOADSCREEN
#endif

///////////////////////////////////////////////////////////////////////////////
// The thread class!
///////////////////////////////////////////////////////////////////////////////
class LoadScreenThread : public OSThread
{
public:
	LoadScreenThread(LoadScreenModule *module);
	~LoadScreenThread();

private:
	LoadScreenModule	*m_module;
	Text 					*m_loadingText;

	// The thread members
	void Startup();
	bool Run();
	void Cleanup();
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LoadScreenThread::LoadScreenThread(
LoadScreenModule *module) :
m_module(module)
{
	if (m_module && !module->WantLoadingText())
		m_loadingText = NULL;
	else
	{
		m_loadingText = new Text;
		if (m_loadingText)
		{
			m_loadingText->SetText("Loading");
			m_loadingText->SetPosition(48.0f, 444.0f - m_loadingText->GetTextHeight());
			//m_loadingText->ShadowSettings(true, 255, 2, 2);
			m_loadingText->ShowAll();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LoadScreenThread::~LoadScreenThread()
{
	if (m_loadingText)
	{
		m_loadingText->ClearText();
		m_loadingText->HideAll();
		m_loadingText->ReleaseAll();
		m_loadingText->Update();
		delete m_loadingText;
	}

	// Kill this
	delete m_module;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoadScreenThread::Startup()
{
#if defined(PS2)
	g_VU0RegisterPreserver.Save();
#endif

#ifdef _XBOX
	DirectX_Device->BlockUntilIdle();
#endif

	if (m_module)
		m_module->Startup();

#if defined(PS2)
	g_VU0RegisterPreserver.Restore();
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LoadScreenThread::Run()
{
#if defined(PS2)
	g_VU0RegisterPreserver.Save();
#endif

	// Allow the controller to update
	g_input.BeginInput();
	
  // Update the text & rotation
	if (m_loadingText)
	{
		char text[32];
		strcpy(text, g_translator.Translate("Loading"));
		int numDots = (EngineTimer__Time() / 500) % 5;
		for (int i = 0; i < numDots; i++)
			strcat( text, "." );
		m_loadingText->SetTextNoTranslate(text);
		m_loadingText->Update();
		Sprite::DrawAll();
	}

	// Standard render crap
	gfxStartRender();

	RenderState__Default();
	RenderState__Request( RS_FOG, RS_FALSE );
#ifdef PS2
	GSState__Send();
#endif //PS2

	// setup frame buffer for rendering-- this is what will be displayed next frame buffer swap
	Display__SetFrameBuffer();

	// Render it!
	if (m_module)
		m_module->Render();
	TextureMgr::Cache().Unlock();

	// Do the sprites
	if (m_loadingText)
		SpriteMgr__Render();

	// Done frame
	gfxEndRender();
	g_input.DoneInput();

#if defined(PS2)
	g_VU0RegisterPreserver.Restore();
#endif

	Sleep(33);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoadScreenThread::Cleanup()
{
#if defined(PS2)
	g_VU0RegisterPreserver.Save();
#endif

#ifdef _XBOX
	DirectX_Device->BlockUntilIdle();
#endif

	MatrixStack__Mode(MS_PROJECTION);
	MatrixStack__Pop();
	RenderState__Default();

	if (m_module)
		m_module->Cleanup();

#if defined(PS2)
	g_VU0RegisterPreserver.Restore();
#endif
}

namespace LoadScreen
{
static LoadScreenThread 		*s_thread = NULL;
static LoadScreenModuleCreate s_creator = NULL;

///////////////////////////////////////////////////////////////////////////////
// Wee
///////////////////////////////////////////////////////////////////////////////
bool Setup(LoadScreenModuleCreate creator)
{
#ifndef NOLOADSCREEN
	// Save this shit for later
	s_creator = creator;
#endif
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Kickoff a loadscreen
///////////////////////////////////////////////////////////////////////////////
bool Kickoff()
{
	// Already running?
	if (s_thread)
		return true;

	// Is there a creator?
	if (!s_creator)
		return false;

	LoadScreenModule *module = s_creator();
	if (!module)
		return false;

	// Start the thread
	s_thread = new LoadScreenThread(module);
	s_thread->Start(OSThread::HIGH);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Is the loadscreen running?
///////////////////////////////////////////////////////////////////////////////
bool IsRunning()
{
	return (s_thread != NULL);
}

///////////////////////////////////////////////////////////////////////////////
// Kill the loadscreen
///////////////////////////////////////////////////////////////////////////////
bool Kill()
{
	if (!s_thread)
		return false;

	// Kill the thread
	s_thread->Kill(true);
	delete s_thread;
	s_thread = NULL;

	return true;
}

};

