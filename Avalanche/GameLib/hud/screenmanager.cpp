#include "hud/HudPCH.h"
#include "hud/formattedfile.h"
#include "hud/screenmanager.h"
#include "hud/screencontrols.h"
#ifdef DIRECTX_PC
#include "EngineHelper/timer.h"
#endif

#define FILEMARKERCODE					(0xcdfe3c5b)
#define PROJECTFILE_MARKER				(0x5c8e3c1b)
#define PROJECTFILE_VERSION			(0x00000100)

#define PROJECTFILE_TEXTUREMARKER	(0x887c3b1a)
#define PROJECTFILE_TEXTUREVERSION	(0x00000100)

#define PROJECTFILE_SPRITEMARKER		(0x3c5e9b1c)
#define PROJECTFILE_SPRITEVERSION	(0x00000100)
#define PROJECTFILE_SPRITECODE1		(0x12345678)
#define PROJECTFILE_SPRITECODE2		(0x13579bdf)

bool FileMarkerOK( FILEMARKER &FileMarker, u32 Marker, u32 Version )
{
	if( FileMarker.Marker != Marker )
		return false;

	if( FileMarker.Version != Version )
		return false;

	return true;
}

//=============================================================================
// ScreenStack::GetScreenHandler
//=============================================================================

ScreenHandler* ScreenStack::GetScreenHandler( void )
{
	ScreenHandler* pScreenHandler = 0;

	if( m_pScreen )
		pScreenHandler = m_pScreen->GetScreenHandler();

	return pScreenHandler;
}

//=============================================================================
// ScreenManager::ScreenManager
//=============================================================================

ScreenManager::ScreenManager( void )
{
	m_pScreens = 0;
	m_pCurrentScreen = 0;
	m_NextScreenTime = 0.0f;
	m_CurrentTime = 0.0f;
	m_PastTime = 0.0f;
	m_NextScreenQueued = false;

	return;
}

//=============================================================================
// ScreenManager::~ScreenManager
//=============================================================================

ScreenManager::~ScreenManager()
{
	Shutdown();
}

//=============================================================================
// ScreenManager::Init
//
//		The screen manager initializes standard interfaces.  Other interfaces may
//		be registered by the application.
//=============================================================================

bool ScreenManager::Init( void )
{
	// Register the standard sprite types
	HudObject::RegisterClass( RUNTIME_CLASSOBJECT(Sprite) );
	HudObject::RegisterClass( RUNTIME_CLASSOBJECT(Text) );

	// Register the standard sprite interfaces
	SpriteInterface::RegisterClass( RUNTIME_CLASSOBJECT( SpriteInterface ) );
	SpriteInterface::RegisterClass( RUNTIME_CLASSOBJECT( Screen ) );
	SpriteInterface::RegisterClass( RUNTIME_CLASSOBJECT( Button ) );

	// Register standard screen controls
	SpriteInterface::RegisterClass( RUNTIME_CLASSOBJECT( GuiCycleButton ) );
	SpriteInterface::RegisterClass( RUNTIME_CLASSOBJECT( GuiListBox ) );
	SpriteInterface::RegisterClass( RUNTIME_CLASSOBJECT( GuiScrollBar ) );
	SpriteInterface::RegisterClass( RUNTIME_CLASSOBJECT( GuiSlider ) );
	SpriteInterface::RegisterClass( RUNTIME_CLASSOBJECT( GuiProgressBar ) );
	SpriteInterface::RegisterClass( RUNTIME_CLASSOBJECT( GuiTable ) );
	SpriteInterface::RegisterClass( RUNTIME_CLASSOBJECT( GuiMeter ) );
	SpriteInterface::RegisterClass( RUNTIME_CLASSOBJECT( GuiScrollingText ) );

	// Register the default screen handler
	ScreenHandler::RegisterClass( RUNTIME_CLASSOBJECT( ScreenHandler ) );

	return true;
}

//=============================================================================
// ScreenManager::Shutdown
//
//		Deletes the scene tree and all associated screens.
//
//		NOTE: The screens are contained within the scene tree and destroyed
//				automatically.
//=============================================================================

void ScreenManager::Shutdown( void )
{
	//////////////////////////////////////////////////////////////////////////
	// Give the current screen or the current popup a chance to "deactivate"
	// cleanly.

	GotoScreen(0);

	//////////////////////////////////////////////////////////////////////////
	// Clean up memory allocated for screen flow

	ClearScreenHistory();

	//////////////////////////////////////////////////////////////////////////
	// No more screens

	m_pScreens = 0;
	m_pCurrentScreen = 0;

	//////////////////////////////////////////////////////////////////////////
	// Destroy the sprite tree

	while( m_SpriteTree.Child() )
		DestroyScene( m_SpriteTree.Child() );

	//////////////////////////////////////////////////////////////////////////
	// Unregister the interfaces

	HudObject::UnregisterClass( RUNTIME_CLASSOBJECT( Sprite ) );
	HudObject::UnregisterClass( RUNTIME_CLASSOBJECT( Text ) );

	// standard interfaces
	SpriteInterface::UnregisterClass( RUNTIME_CLASSOBJECT( SpriteInterface ) );
	SpriteInterface::UnregisterClass( RUNTIME_CLASSOBJECT( Screen ) );
	SpriteInterface::UnregisterClass( RUNTIME_CLASSOBJECT( Button ) );

	// standard screen controls
	SpriteInterface::UnregisterClass( RUNTIME_CLASSOBJECT( GuiCycleButton ) );
	SpriteInterface::UnregisterClass( RUNTIME_CLASSOBJECT( GuiListBox ) );
	SpriteInterface::UnregisterClass( RUNTIME_CLASSOBJECT( GuiScrollBar ) );
	SpriteInterface::UnregisterClass( RUNTIME_CLASSOBJECT( GuiSlider ) );
	SpriteInterface::UnregisterClass( RUNTIME_CLASSOBJECT( GuiProgressBar ) );
	SpriteInterface::UnregisterClass( RUNTIME_CLASSOBJECT( GuiTable ) );
	SpriteInterface::UnregisterClass( RUNTIME_CLASSOBJECT( GuiMeter ) );
	SpriteInterface::UnregisterClass( RUNTIME_CLASSOBJECT( GuiScrollingText ) );

	// default screen handler
	ScreenHandler::UnregisterClass( RUNTIME_CLASSOBJECT( ScreenHandler ) );
}

//=============================================================================
// ScreenMananger::SetUIHandler
//
//			Sets the current input device.
//=============================================================================

void ScreenManager::SetUIHandler( UIHandler* pUIHandler )
{
	m_pUIHandler = pUIHandler;
}

//=============================================================================
// ScreenManager::GetUIHandler
//
//		Returns a pointer to the current input device.
//=============================================================================

UIHandler* ScreenManager::GetUIHandler( void )
{
	return m_pUIHandler;
}

//=============================================================================
// ScreenManager::RefreshInput
//
//		Refresh the input device.  This is usually called once per frame but may
//		be called as often as is necessary.
//=============================================================================

void ScreenManager::RefreshInput( void )
{
	if( m_pUIHandler )
		m_pUIHandler->RefreshInput();

	return;
}

//=============================================================================
// ScreenManager::PlaySound
//
//		Play a sound by passing the name of the sound (usually a file name) to
//		the sound device.
//=============================================================================

void ScreenManager::PlaySound( const char* pSound )
{
	if( m_pUIHandler )
		m_pUIHandler->PlaySound( pSound );

	return;
}

//=============================================================================
// ScreenManager::LoadScreen
//
//		The screen manager loads one or more screens from the screen file.
//		These screens are stored in a list and the sprites within the screen are
//		added to the sprite tree.
//=============================================================================

bool ScreenManager::LoadScreen( const char* pFileName )
{
	MemoryBank m;
	FormattedFile File;
	bool Result = false;

	if( File.OpenFile( pFileName, FileStream::READ ) )
	{
		if( File.Branch( PROJECTFILE_MARKER, PROJECTFILE_VERSION ) )
		{
			//====================================================================
			// The texture chunk contains a list of textures that need to be
			// loaded.  At the present time, this actually referes to the source
			// texture path (on the PC drive) and is not currently used for any
			// purpose.
			//====================================================================

			if( File.Branch( PROJECTFILE_TEXTUREMARKER, PROJECTFILE_TEXTUREVERSION ) )
			{
				File.Return();
			}

			//====================================================================
			// Process the sprite chunk (scene tree).
			//====================================================================

			if( File.Branch( PROJECTFILE_SPRITEMARKER, PROJECTFILE_SPRITEVERSION ) )
			{
				if( File.Read( m ) )
				{
					u32 SpriteCode1;
					u32 SpriteCode2;
					u32 SpriteCount;

					// make sure its valid

					m.Read( &SpriteCode1, sizeof(SpriteCode1) );
					m.Read( &SpriteCode2, sizeof(SpriteCode2) );
					m.Read( &SpriteCount, sizeof(SpriteCount) );

					if(SpriteCode1 == PROJECTFILE_SPRITECODE1 &&
						SpriteCode2 == PROJECTFILE_SPRITECODE2 )
					{
						Result = true;

						for( u32 i = 0; i < SpriteCount; i++ )
						{
							if( !LoadScreen(m) )
							{
								Result = false;
								break;
							}
						}
					}
				}

				File.Return();
			}

			File.Return();
		}

		File.CloseFile();
	}

	return Result;
}

//=============================================================================
// ScreenManager::LoadScreen 
//
//		Screens are loaded from a memory bank.  The screen manager first loads a
//		screen file into a memory bank and then calls this function to unpack it.
//		Also, this allows a screen to reside in memory rather than on disk to
//		improve performance and reduce load times.
//
//=============================================================================

bool ScreenManager::LoadScreen( MemoryBank &m )
{
	//==========================================================================
	// The screen manager maintains a list of screens that are currently loaded
	// in memory.  If we save a pointer to the very last screen in that list,
	// then load some screens, we can traverse the list and initialize any new
	// screens that were added.
	//==========================================================================
	
	Screen* pScreen = m_pScreens;

	while( pScreen && pScreen->m_pNextScreen )
		pScreen = pScreen->m_pNextScreen;

	if( !LoadSprites( m, 0 ) )
		return false;

	//==========================================================================
	// Now the list has been updated and we can initialize new screens.
	//==========================================================================

	if( !pScreen )
		pScreen = m_pScreens;
	else
		pScreen = pScreen->m_pNextScreen;

	while( pScreen )
	{
		pScreen->InitScreen();
		pScreen = pScreen->m_pNextScreen;
	}

	return true;
}

//=============================================================================
// ScreenManager::LoadSprites
//
//		This function processes the sprite tree within a file.  Every sprite
//		header begins with a 4-byte code called the sanity check.  The sanity
//		check helps to ensure we haven't gotten off track.  The sanity check is
//		special constant minus the number of children expected.
//=============================================================================

bool ScreenManager::LoadSprites( MemoryBank &m, Sprite *pParent )
{
	Sprite *pSprite;
	xstring classname;
	xstring name;

	int SanityCheck;
	int Children;

	if( !m.Read( &SanityCheck, sizeof(SanityCheck) ) )
		return false;

	if( !m.Read( &Children, sizeof(Children) ) )
		return false;

	if( SanityCheck != (0x3fffffff - Children) )
		return false;

	if( !m.ReadString( classname ) )
		return false;

	if( !m.ReadString( name ) )
		return false;

	pSprite = (Sprite *) HudObject::NewObject( classname.get() );

	if( !pSprite )
		return false;

	if( !pSprite->LoadObject( m, 0 ) )
	{
		delete pSprite;
		return false;
	}

	if( !pParent )
		pParent = &m_SpriteTree;

	pParent->AddChild( pSprite );

	Screen* pScreen = (Screen*) pSprite->GetInterface("Screen");

	if( pScreen )
		AddScreen( pScreen, name.get() );

	for( int i = 0; i < Children; i++ )
	{
		if( !LoadSprites( m, pSprite ) )
			return false;
	}

	return true;
}

//=============================================================================
// ScreenManager::UpdateScreens
//
//		This should be called by the application on each frame update.  The scene
//		is updated the active screen receives input.
//=============================================================================

void ScreenManager::UpdateScreens( void )
{
	if( m_CurrentTime >= m_NextScreenTime )
	{
		if( m_NextScreenQueued  )
		{
			m_NextScreenQueued = false;

			// Show it
			ShowScreen( m_pCurrentScreen );
		}

		UpdateScreen( m_pCurrentScreen );
	}

	RefreshInput();

	Screen* pActiveScreen = GetActiveScreen();

	if( m_CurrentTime >= m_AllowInputTime )
	{
		if( pActiveScreen && m_pUIHandler )
			pActiveScreen->HandleInput( m_pUIHandler );
	}
#ifdef DIRECTX_PC
	g_MenuCursor.Update();
#endif

	m_SpriteTree.Update();
}

//=============================================================================
// ScreenManager::UpdateScreen
//
//		This function allows specific screens to be updated without updating
//		the screen manager.  This is particularly important during a load screen.
//=============================================================================

void ScreenManager::UpdateScreen( const char* pScreenName )
{
	Screen* pScreen = GetScreen( pScreenName );

	if( pScreen )
		UpdateScreen( pScreen );

	return;
}

//=============================================================================
// ScreenManager::UpdateScreen
//
//		This function allows specific screens to be updated without updating
//		the screen manager.  This is particularly important during a load screen.
//=============================================================================

void ScreenManager::UpdateScreen( Screen* pScreen )
{
	if( pScreen )
	{
		ScreenHandler* pScreenHandler = pScreen->GetScreenHandler();

		if( pScreenHandler )
			pScreenHandler->OnScreenUpdate();

		UpdateScreen( pScreen->m_pCurrentPopup );
	}

	return;
}

//=============================================================================
// ScreenManager::RenderScreen
//
//		This function allows specific screens to be rendered.
//=============================================================================

void ScreenManager::RenderScreen( const char *pScreenName )
{
	Screen* pScreen = GetScreen( pScreenName );

	if( pScreen )
		RenderScreen( pScreen );

	return;
}

//=============================================================================
// ScreenManager::RenderScreen
//
//		This function allows specific screens to be rendered.
//=============================================================================

void ScreenManager::RenderScreen( Screen* pScreen )
{
	if( pScreen )
	{
		Sprite* pSprite = pScreen->SpriteHost();

		if( pSprite )
			pSprite->Update();
	}

	return;
}

//=============================================================================
// ScreenManager::SetTime
//=============================================================================

void ScreenManager::SetTime( float Time )
{
	m_CurrentTime = Time;
}

//=============================================================================
// ScreenManager::AddScreen
//
//		This function adds the specified screen to the screen list.  The screen
//		manager keeps track of all screens by stuffing them into this list.
//=============================================================================

void ScreenManager::AddScreen( Screen* pScreen, const char* pScreenName )
{
	if( pScreen )
	{
		if( m_pScreens == 0 )
			m_pScreens = pScreen;
		else
		{
			Screen *pLastScreen = m_pScreens;

			while( pLastScreen->m_pNextScreen )
				pLastScreen = pLastScreen->m_pNextScreen;

			pLastScreen->m_pNextScreen = pScreen;
			pScreen->m_pPrevScreen = pLastScreen;
		}

		pScreen->SetScreenName( pScreenName );
		pScreen->SetScreenManager( this );
	}

	return;
}

//=============================================================================
// ScreenManager::DestroyScene
//
//		This function destroys a sprite and all its children.  Any interfaces
//		attached to the sprite are automatically destroyed from within the
//		Sprites destructor function.
//=============================================================================

void ScreenManager::DestroyScene( Sprite *pSprite )
{
	if( pSprite )
	{
		while( pSprite->Child() )
			DestroyScene( pSprite->Child() );

		delete pSprite;
	}

	return;
}

//=============================================================================
// ScreenManager::ShowScreen
//
//		Displays a screen but does not set the focus.  Returns false if the
//		screen does not exist or cannot be found.
//=============================================================================

bool ScreenManager::ShowScreen( const char* pScreenName )
{
	return ShowScreen( GetScreen( pScreenName ) );
}

//=============================================================================
// ScreenManager::ShowScreen
//
//		Displays a screen but does not set the focus.  Returns false if the
//		screen does not exist or cannot be found.
//=============================================================================

bool ScreenManager::ShowScreen( Screen* pScreen )
{
	if( !pScreen )
		return false;

	pScreen->Show();
	
	return true;
}

//=============================================================================
// ScreenManager::HideScreen
//
//		Removes a screen from the display but does not necessarily stop receiving
//		input.  Returns false if the screen does not exist or cannot be found.
//=============================================================================

bool ScreenManager::HideScreen( const char* pScreenName )
{
	return HideScreen( GetScreen( pScreenName ) );
}

//=============================================================================
// ScreenManager::HideScreen
//
//		Removes a screen from the display but does not necessarily stop receiving
//		input.  Returns false if the screen does not exist or cannot be found.
//=============================================================================

bool ScreenManager::HideScreen( Screen* pScreen )
{
	if( !pScreen )
		return false;

	pScreen->Hide();

	return true;
}

//=============================================================================
// ScreenManager::PopScreenHistory
//=============================================================================

Screen* ScreenManager::PopScreenHistory( void )
{
	ScreenStack* pScreenStack = m_ScreenStack.Child();

	if( pScreenStack )
		return PopScreenHistory( pScreenStack->GetScreen() );

	return 0;
}

//=============================================================================
// ScreenManager::PopScreenHistory
//=============================================================================

Screen* ScreenManager::PopScreenHistory( Screen* pScreen )
{
	ScreenStack* pScreenStack = m_ScreenStack.Child();

	while( pScreenStack )
	{
		if( pScreenStack->GetScreen() == pScreen )
		{
			delete pScreenStack;
			return pScreen;
		}

		pScreenStack = pScreenStack->Next();
	}

	// wasn't there
	return 0;
}

//=============================================================================
// ScreenManager::PopScreenHistory
//=============================================================================

Screen* ScreenManager::PopScreenHistory( const char* pScreenName )
{
	return PopScreenHistory( GetScreen( pScreenName ) );
}

//=============================================================================
// ScreenManager::GetScreenStack
//
//		Returns a pointer to the first item in the screen flow, which is also
//		the most recently visited screen.
//=============================================================================

ScreenStack* ScreenManager::GetScreenStack( void )
{
	return m_ScreenStack.Child();
}

//=============================================================================
// ScreenManager::GetScreenHandler
//
//		Returns a pointer to the specified screen handler, or null if that screen
//    does not exist or does not match the specified class name.
//=============================================================================

ScreenHandler* ScreenManager::GetScreenHandler( const char* pScreenName, const char* pClassName )
{
	ScreenHandler* pHandler = GetScreenHandler( pScreenName );

	if( pHandler && 0 == strcmp( pHandler->Classname(), pClassName ) )
		return pHandler;

	return 0;
}

//=============================================================================
// ScreenManager::GetScreenHandler
//
//		Returns a pointer to the specified screen handler, or null if that screen
//    does not exist.
//=============================================================================

ScreenHandler* ScreenManager::GetScreenHandler( const char* pScreenName )
{
	return GetScreenHandler( GetScreen( pScreenName ) );
}

//=============================================================================
// ScreenManager::GetScreenHandler
//
//		Returns a pointer to the specified screen handler.
//=============================================================================

ScreenHandler* ScreenManager::GetScreenHandler( Screen* pScreen )
{
	if( pScreen )
		return pScreen->GetScreenHandler();

	return 0;
}

//=============================================================================
// ScreenManager::SetScreenHandler
//
//		This sets a new screen handler for a specific screen.  Typically, there
//		is one screen handler per screen, though it is entirely possible to
//		manage multiple screens with just a single screen handler.  Keep in mind,
//    however, each screen will always create exactly one instance of that
//		handler.
//=============================================================================

void ScreenManager::SetScreenHandler( const char* pScreenName, const char* pScreenHandler )
{
	Screen* pScreen = GetScreen( pScreenName );

	if( pScreen )
	{
		pScreen->SetScreenHandlerClass( pScreenHandler );
		pScreen->InitScreenHandler();
	}

	return;
}

//=============================================================================
// ScreenManager::GetScreen
//
//		Returns a pointer to the specified screen, or null if that screen does
//		not exist.
//=============================================================================

Screen* ScreenManager::GetScreen( const char* pScreenName )
{
	Screen* pScreen = m_pScreens;

	while( pScreen )
	{
		if( pScreen->m_ScreenName.compare( pScreenName ) )
			break;

		pScreen = pScreen->m_pNextScreen;
	}

	return pScreen;
}

//=============================================================================
// ScreenManager::GetCurrentScreen
//
//		Returns a pointer to the current screen.
//=============================================================================

Screen* ScreenManager::GetCurrentScreen( void )
{
	return m_pCurrentScreen;
}

///=============================================================================
// ScreenManager::GetCurrentScreenHandler
//
//		Returns a pointer to the current screen handler, or null if no screens
//		are active.
//=============================================================================

ScreenHandler* ScreenManager::GetCurrentScreenHandler( void )
{
	Screen* pScreen = GetCurrentScreen();

	if( !pScreen )
		return 0;

	return pScreen->GetScreenHandler();
}

//=============================================================================
// ScreenManager::GetActiveScreen
//
//		Returns a pointer to the screen currently receiving input.
//=============================================================================

Screen* ScreenManager::GetActiveScreen( void )
{
	Screen* pActiveScreen = m_pCurrentScreen;

	if( pActiveScreen )
	{
		Screen* pActivePopup = pActiveScreen->GetActivePopup();

		if( pActivePopup )
			return pActivePopup;
	}

	return pActiveScreen;
}

//=============================================================================
// ScreenManager::GetActiveScreenHandler
//
//		Returns a pointer to the active screen handler, or null if no screens are
//		active.
//=============================================================================

ScreenHandler* ScreenManager::GetActiveScreenHandler( void )
{
	Screen* pScreen = GetActiveScreen();

	if( !pScreen )
		return 0;

	return pScreen->GetScreenHandler();
}

//=============================================================================
// ScreenManager::IsScreenAlreadyActive
//
//		Returns true if the popup is already active, false if it is not.
//=============================================================================

bool ScreenManager::IsScreenAlreadyActive( const char* pScreenName )
{
	return IsScreenAlreadyActive( GetScreen( pScreenName ) );
}

//=============================================================================
// ScreenManager::IsScreenAlreadyActive
//
//		Returns true if the popup is already active, false if it is not.
//=============================================================================

bool ScreenManager::IsScreenAlreadyActive( Screen* pScreen )
{
	Screen* pActiveScreen = GetCurrentScreen();

	while( pActiveScreen )
	{
		if( pActiveScreen == pScreen )
			return true;

		pActiveScreen = pActiveScreen->GetCurrentPopup();
	}

	return false;
}

//=============================================================================
// ScreenManager::SetCurrentScreen
//
//		Sets the active screen.  Only the active screen receives user input.
//=============================================================================

void ScreenManager::SetCurrentScreen( const char* pScreenName )
{
	SetCurrentScreen( GetScreen( pScreenName ) );
}

//=============================================================================
// ScreenManager::SetCurrentScreen
//
//		Sets the active screen.  Only the active screen receives user input.
//=============================================================================

void ScreenManager::SetCurrentScreen( Screen* pScreen )
{
	m_pCurrentScreen = pScreen;
}

//=============================================================================
// ScreenManager::GotoScreen
//
//		Displays the specified screen.  The currently active screen is hidden and
//		pushed onto the screen flow stack.
//=============================================================================

bool ScreenManager::GotoScreen( const char* pScreenName )
{
	Screen* pScreen = GetScreen( pScreenName );

	if( pScreenName && strlen(pScreenName) != 0 )
	{
		//=======================================================================
		// When this assert hits, it means the screen could not be found.
		// Check the name and make sure the appropriate file was loaded.
		//=======================================================================

		ASSERT(pScreen);
	}

	return GotoScreenPtr( pScreen );
}

//=============================================================================
// ScreenManager::GotoScreenPtr
//
//		Displays the specified screen.  The currently active screen is hidden and
//		pushed onto the screen flow stack.
//=============================================================================

bool ScreenManager::GotoScreenPtr( Screen* pScreen )
{
	//==========================================================================
	// Kill any and all active popups...
	//==========================================================================

	RemoveAllPopups();

	//==========================================================================
	// Check if were going to a valid screen or nothing at all.
	//==========================================================================

	if( !pScreen )
	{
		//==========================================================================
		// This happens when the screen cannot be found (or null was used as the screen name).
		// The active screen is hidden and the screen flow stack is erased.
		//==========================================================================

		ClearScreenHistory();

		HideScreen( m_pCurrentScreen );

		m_pCurrentScreen = 0;

		return false;
	}

	//==========================================================================
	// Queue the next screen to appear.
	//==========================================================================

	m_NextScreenTime = m_CurrentTime;
	m_NextScreenQueued = true;
	
	//==========================================================================
	// Push the currently active screen onto the screen flow stack...
	//==========================================================================

	if( m_pCurrentScreen )
	{
		ScreenStack* pScreenStack = new ScreenStack( m_pCurrentScreen );

		if( pScreenStack )
			m_ScreenStack.SetChild( pScreenStack );

		ScreenHandler* pScreenHandler = m_pCurrentScreen->GetScreenHandler();

		if( pScreenHandler )
			m_NextScreenTime += pScreenHandler->GetScreenExitTime();

		//=======================================================================
		// Hide the old screen
		//=======================================================================

		HideScreen( m_pCurrentScreen );
	}

	m_AllowInputTime = m_NextScreenTime;

	ScreenHandler* pScreenHandler = pScreen->GetScreenHandler();

	if( pScreenHandler )
		m_AllowInputTime += pScreenHandler->GetScreenIntroTime();

	m_pCurrentScreen = pScreen;

	return true;
}

//=============================================================================
// ScreenManager::ShowPopup
//
//		This function takes the specified screen and displays it as a popup.
//		The currently active screen loses input but is not removed from the
//		screen.  The previous screen is notified when the popup has ended.
//=============================================================================

bool ScreenManager::ShowPopup( const char* pScreenName )
{
	Screen* pScreen = GetScreen( pScreenName );

	if( pScreenName && strlen(pScreenName) != 0 )
	{
		// Sevenup won't link with this
		// TRACE("::::::ERROR::::: Missing Screen: %s\n", pScreenName);

		ASSERT(pScreen); // Screen not found.  Check the name and make sure the file was loaded.
	}

	return ShowPopup( pScreen );
}

//=============================================================================
// ScreenManager::ShowPopup
//
//		This function takes the specified screen and displays it as a popup.
//		The currently active screen loses input but is not removed from the
//		screen.  The previous screen is notified when the popup has ended.
//=============================================================================

bool ScreenManager::ShowPopup( Screen* pScreen )
{
	//==========================================================================
	// Can't find the screen?
	//==========================================================================

	if( !pScreen )
		return false;

	//==========================================================================
	// Are we already displaying this popup??
	//==========================================================================

	if( IsScreenAlreadyActive( pScreen ) )
		return true;

	//==========================================================================
	// Notify the current screen a popup is about to show, but do not hide it.
	//==========================================================================

	Screen* pActiveScreen = GetActiveScreen();

	if( !pActiveScreen )
		GotoScreenPtr( pScreen );
	else
	{
		pActiveScreen->ShowPopup( pScreen );

		ScreenHandler* pScreenHandler = pActiveScreen->GetScreenHandler();

		if( pScreenHandler )
			pScreenHandler->OnPopupShow();
	}

	return true;
}

//=============================================================================
// ScreenManager::RemoveActivePopup
//=============================================================================

void ScreenManager::RemoveActivePopup( void )
{
	if( m_pCurrentScreen )
		m_pCurrentScreen->RemoveActivePopup();

	return;
}

//=============================================================================
// ScreenManager::RemoveAllPopups
//=============================================================================

void ScreenManager::RemoveAllPopups( void )
{
	if( m_pCurrentScreen )
		m_pCurrentScreen->RemoveAllPopups();

	return;
}

//=============================================================================
// ScreenManager::ClosePopup
//=============================================================================

void ScreenManager::ClosePopup( const char* pScreenName )
{
	ClosePopup( GetScreen( pScreenName ) );
}

//=============================================================================
// ScreenManager::ClosePopup
//=============================================================================

void ScreenManager::ClosePopup( Screen* pPopupScreen )
{
	Screen* pCurrentScreen = GetCurrentScreen();
	Screen* pCurrentPopup;
	
	if( pCurrentScreen )
	{
		if( pCurrentScreen == pPopupScreen )
		{
			m_pCurrentScreen = pCurrentScreen->GetCurrentPopup();
			HideScreen( pCurrentScreen );
			pCurrentScreen->m_pCurrentPopup = 0;
		}
		else
		{
			while( pCurrentScreen )
			{
				pCurrentPopup = pCurrentScreen->GetCurrentPopup();

				if( pCurrentPopup == pPopupScreen )
				{
					pCurrentScreen->RemoveCurrentPopup();
					break;
				}

				pCurrentScreen = pCurrentPopup;
			}
		}
	}

	return;
}

//=============================================================================
// ScreenManager::EndScreen
//
//		This function is called to end a popup or return to the previous screen
//		when the current screen is finished.  The previous screen is popped from
//		the screen flow stack, activated and displayed, and also receives
//		notication that a popup terminated if that is the case.
//=============================================================================

void ScreenManager::EndScreen( int Response )
{
	if( m_pCurrentScreen )
	{
		Screen* pActivePopup = m_pCurrentScreen->GetActivePopup();

		if( pActivePopup )
			m_pCurrentScreen->RemoveActivePopup();
		else
			GotoPrevScreen();
	}
	
	return;
}

//=============================================================================
// ScreenManager::EndPopup
//
//		This function is called to end a popup and return to the previous screen.
//		The previous screen is activated and displayed, and a notifcation is sent
//		to inform the screen when the popup is closed.
//
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// I don't like this function it needs to go away.  Superceaded by
// RemoveActivePopup() but there is too much shit in UseCase using this damn
// broken ass function so I left it in.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//=============================================================================

void ScreenManager::EndPopup( int Result )
{
//	ASSERTS(0, "EndPopup( int Result );\nThis function sucks. Stop using it and call ScreenManager::ClosePopup( "SCREEN_ID" ) instead." );

	ScreenHandler* pScreenHandler;
	Screen* pScreen;
	Screen* pPopup;

	if( m_pCurrentScreen )
	{
		pScreen = m_pCurrentScreen;
		pPopup = m_pCurrentScreen->GetActivePopup();

		while( pPopup )
		{
			pScreenHandler = pScreen->GetScreenHandler();

			if( pScreenHandler )
				pScreenHandler->OnPopupHide();

			pScreen = pPopup;
			pPopup = pPopup->GetActivePopup();
		}

		m_pCurrentScreen->RemoveAllPopups();
	}

	return;
}

//=============================================================================
// ScreenManager::GotoPrevScreen
//
//		Pops the most recently visited screen from the screen flow stack and
//		immediately dislays.  The current screen is deactivated and is not
//		placed onto the screen flow stack (that would create a circular stack!)
//=============================================================================

void ScreenManager::GotoPrevScreen( void )
{
	Screen* pPrevScreen = PopScreenHistory();

	if( pPrevScreen )
	{
		ScreenHandler* pScreenHandler = GetActiveScreenHandler();

		if( pScreenHandler )
			pScreenHandler->OnPrevScreen( pPrevScreen );

		if( GotoScreenPtr( pPrevScreen ) )
			PopScreenHistory();
	}

	return;
}


//=============================================================================
// ScreenManager::ClearScreenHistory
//
//		Call this function to erase screen history buffer.  Gets called by the
//		destructor to do cleanup.
//=============================================================================
	
void ScreenManager::ClearScreenHistory( void )
{
	while( m_ScreenStack.Child() )
		delete m_ScreenStack.Child();

	return;
}

//=============================================================================
// ScreenManager::IsActiveScreenReadyToExit
//
//		Call this function to see if the active screen is ready to exit.
//=============================================================================
	
bool ScreenManager::IsActiveScreenReadyToExit( void )
{
	ScreenHandler* pScreenHandler = GetActiveScreenHandler();

	if( pScreenHandler )
		return pScreenHandler->IsScreenReadyToExit();

	return(true);
}

//=============================================================================
// ScreenManager::IsScreenReadyForInput
//
//		Call this function to see if no screen is queued and the active screen
//		is would allow input.
//=============================================================================
	
bool ScreenManager::IsScreenReadyForInput( void )
{
	return( !m_NextScreenQueued && ( m_CurrentTime >= m_AllowInputTime ) );
}

Screen::Screen( void )
{
	m_pScreenManager = 0;
	m_pScreenHandler = 0;
	m_pScreenControls = 0;
	m_pNextScreen = 0;
	m_pPrevScreen = 0;
	m_pCurrentPopup = 0;
	m_visible = true;

	m_pInitialButton = 0;
	m_pDefaultButton = 0;
	m_pCurrentButton = 0;
	m_pCursor = 0;
	m_ScreenPriority = 0;
	m_ScreenKey = 0;

	memset( m_SoundEvent, 0, sizeof(m_SoundEvent) );

	SetScreenHandlerClass( ScreenHandler::ClassnameStatic() );
}

Screen::~Screen( void )
{
	if( m_pScreenManager )
	{
		if( m_pScreenManager->m_pScreens == this )
			m_pScreenManager->m_pScreens = m_pNextScreen;

		if( m_pScreenManager->m_pCurrentScreen == this )
			m_pScreenManager->m_pCurrentScreen = 0;
	}

	if( m_pPrevScreen )
		m_pPrevScreen->m_pNextScreen = m_pNextScreen;

	if( m_pNextScreen )
		m_pNextScreen->m_pPrevScreen = m_pPrevScreen;

	if( m_pScreenHandler )
	{
		m_pScreenHandler->OnTerminate();
		delete m_pScreenHandler;
	}

	m_pScreenManager = 0;
	m_pScreenHandler = 0;
	m_pPrevScreen = 0;
	m_pNextScreen = 0;
}

void Screen::SetScreenName( const char* pScreenName )
{
	m_ScreenName.set( pScreenName );
}

void Screen::SetScreenManager( ScreenManager* pScreenManager )
{
	m_pScreenManager = pScreenManager;
}

ScreenManager* Screen::GetScreenManager( void )
{
	return m_pScreenManager;
}

ScreenHandler* Screen::GetScreenHandler( void )
{
	return m_pScreenHandler;
}

void Screen::InitScreenHandler( void )
{
	const char* pScreenHandlerClass = GetScreenHandlerClass();

	if( m_pScreenHandler )
		delete m_pScreenHandler;

	m_pScreenHandler = ScreenHandler::NewObject( pScreenHandlerClass );

	if( m_pScreenHandler )
	{
		m_pScreenHandler->m_pScreen = this;
		m_pScreenHandler->OnInitialize();
	}

	return;
}

void Screen::SetScreenHandlerClass( const char* pScreenHandlerClass )
{
	m_ScreenHandlerClass.set( pScreenHandlerClass );
}

const char* Screen::GetScreenHandlerClass( void )
{
	return m_ScreenHandlerClass.get();
}

void Screen::RemoveScreenHandler( void )
{
	m_pScreenHandler = 0;
}

bool Screen::InitScreen( void )
{
	InitScreenHandler();

	Sprite* pSprite = SpriteHost();

	if( pSprite )
		m_ScreenPriority = pSprite->m_state.sprite.priority;

	Hide();

	return true;
}

void Screen::PlaySound( const char* pSound )
{
	if( m_pScreenManager )
		m_pScreenManager->PlaySound( pSound );

	return;
}

bool Screen::Init( void* Param )
{
	return SpriteInterface::Init(Param);
}

Sprite* Screen::GetSprite( int SpriteId )
{
	return GetSprite( SpriteId, SpriteHost() );
}

Sprite* Screen::GetSprite( int SpriteId, Sprite* pParent )
{
	Sprite* pSprite;
	Sprite* pChild;

	if( pParent )
	{
		if( pParent->ObjectId() == SpriteId )
			return pParent;

		pChild = pParent->Child();
	
		while( pChild )
		{
			pSprite = GetSprite( SpriteId, pChild );

			if( pSprite )
				return pSprite;

			pChild = pChild->Next();
		}
	}

	return 0;
}

Text* Screen::GetTextSprite( int SpriteId )
{
	return (Text*) GetSprite( SpriteId );
}

void Screen::SetText( int TextId, const char* pText )
{
	Text* pTextSprite = GetTextSprite( TextId );

	if( pTextSprite )
		pTextSprite->SetText( pText );

	return;
}

void Screen::Printf( int TextId, const char* fmt, ... )
{
	static char szPrintf[512];

	va_list args;
	va_start( args, fmt );
	vsprintf( szPrintf, fmt, args );
	va_end( args );

	SetText( TextId, szPrintf );
}

SpriteInterface* Screen::GetInterface( int SpriteId, const char* pInterfaceName )
{
	Sprite* pSprite = GetSprite( SpriteId );

	if( !pSprite )
		return 0;

	return pSprite->GetInterface( pInterfaceName );
}

SpriteInterface* Screen::AddInterface( int SpriteId, const char* pInterfaceName )
{
	return AddInterface( GetSprite( SpriteId ), pInterfaceName );
}

SpriteInterface* Screen::AddInterface( Sprite* pSprite, const char* pInterfaceName )
{
	if( !pSprite )
		return 0;

	return pSprite->AddInterface( pInterfaceName, 0 );
}

void Screen::ShowSprite( int SpriteId, bool Toggle )
{
	Sprite* pSprite = GetSprite( SpriteId );

	if( pSprite )
		pSprite->Show( Toggle );

	return;
}

void Screen::Show( void )
{
	Sprite* pSprite = SpriteHost();

	if( pSprite )
		pSprite->Show();

	ScreenHandler* pScreenHandler = GetScreenHandler();

	if( pScreenHandler && !pScreenHandler->GetActive() )
	{
		pScreenHandler->SetActive( true );
		pScreenHandler->OnScreenIntro();
	}

	return;
}

void Screen::Hide( void )
{
	Sprite* pSprite = SpriteHost();

	if( pSprite )
		pSprite->Hide();

	ScreenHandler* pScreenHandler = GetScreenHandler();

	if( pScreenHandler && pScreenHandler->GetActive() )
	{
		pScreenHandler->OnScreenExit();
		pScreenHandler->SetActive( false );
	}

	return;
}

bool Screen::IsVisible( void )
{
	Sprite* pSprite = SpriteHost();

	if( !pSprite )
		return false;

	return pSprite->IsVisible();
}

bool Screen::IsHidden( void )
{
	return !IsVisible();
}

void Screen::HandleInput( UIHandler* pUIHandler )
{
	if( pUIHandler && m_pScreenHandler )
		m_pScreenHandler->HandleInput( pUIHandler );

	return;
}

Screen* Screen::GetScreen( const char* pScreenName )
{
	Screen* pScreen = 0;

	if( m_pScreenManager )
		pScreen = m_pScreenManager->GetScreen( pScreenName );

	return pScreen;
}

bool Screen::GotoScreen( const char* pScreenName )
{
	if( m_pScreenManager )
		return m_pScreenManager->GotoScreen( pScreenName );
	
	return false;
}

void Screen::GotoPrevScreen( void )
{
	if( m_pScreenManager )
		m_pScreenManager->GotoPrevScreen();

	return;
}

void Screen::ShowPopup( Screen* pScreen )
{
	RemoveActivePopup();

	m_pCurrentPopup = pScreen;

	if( pScreen )
	{
		Sprite* pScreenSprite = SpriteHost();
		Sprite* pPopupSprite = pScreen->SpriteHost();

		int BasePriority = 0;

		if( pScreenSprite )
			BasePriority = pScreenSprite->GetSortPriority();

		if( pPopupSprite )
		{
			if( pPopupSprite->GetSortPriority() < BasePriority + 0x800 )
				pPopupSprite->SetPriority( BasePriority + 0x800 );
		}

		pScreen->Show();
	}

	return;
}

void Screen::RemoveActivePopup( void )
{
	if( m_pCurrentPopup )
	{
		if( m_pCurrentPopup->m_pCurrentPopup )
			m_pCurrentPopup->RemoveActivePopup();
		else
			RemoveCurrentPopup();
	}

	return;
}

void Screen::RemoveAllPopups( void )
{
	if( m_pCurrentPopup )
		m_pCurrentPopup->RemoveAllPopups();

	RemoveActivePopup();
}

void Screen::RemoveCurrentPopup(	void )
{
	if( m_pCurrentPopup )
	{
		Screen* pNextPopup = m_pCurrentPopup->GetCurrentPopup();

		ScreenHandler* pScreenHandler = GetScreenHandler();

		if( pScreenHandler && !pNextPopup )
			pScreenHandler->OnPopupHide();

		m_pCurrentPopup->Hide();
		m_pCurrentPopup->m_pCurrentPopup = 0;
		m_pCurrentPopup = pNextPopup;
		
	}

	return;
}

Screen* Screen::GetActivePopup( void )
{
	Screen* pActivePopup = m_pCurrentPopup;

	if( pActivePopup )
		pActivePopup = pActivePopup->GetActivePopup();

	if( !pActivePopup )
		pActivePopup = m_pCurrentPopup;

	return pActivePopup;
}

Screen* Screen::GetCurrentPopup( void )
{
	return m_pCurrentPopup;
}

//=============================================================================
// Screen::GetButton
//=============================================================================

Button* Screen::GetButton( int ButtonId )
{
	Button* pButton = 0;
	Sprite* pSprite = GetSprite( ButtonId );

	if( pSprite )
		pButton = (Button*) pSprite->GetInterface( Button::ClassnameStatic() );

	return pButton;
}

//=============================================================================
// Screen::SetInitialButton
//
//		Sets the initial button for this screen.
//=============================================================================

void Screen::SetInitialButton( int ButtonId )
{
	Sprite* pSprite = GetSprite( ButtonId );

	if( pSprite )
	{
		Button* pButton = (Button *) pSprite->GetInterface( Button::ClassnameStatic() );

		if( !pButton )
			pButton = (Button*) pSprite->AddInterface( Button::ClassnameStatic(), 0 );

		SetInitialButton( pButton );
	}

	return;
}

//=============================================================================
// Screen::SetInitialButton
//
//		Sets the initial button for this screen.
//=============================================================================

void Screen::SetInitialButton( Button* pButton )
{
	if( pButton )
	{
		pButton->SetNextButton(0);
		pButton->SetPrevButton(0);
	}

	m_pInitialButton = pButton;

	SelectInitialButton();
}

//=============================================================================
// Screen::GetInitialButton
//
//		Returns a pointer to the initial button.
//=============================================================================

Button* Screen::GetInitialButton( void )
{
	return m_pInitialButton;
}

//=============================================================================
// Screen::GetInitialButtonId
//
//		Returns the Id of the initial button.
//=============================================================================

int Screen::GetInitialButtonId( void )
{
	Button* pButton = GetInitialButton();

	if( !pButton )
		return 0;

	return pButton->GetButtonId();
}

//=============================================================================
// Screen::SetNextButton
//
//		Set the next button in the button cycle order.
//=============================================================================

void Screen::SetNextButton( int ButtonId )
{
	Button* pLastButton = GetLastButton();

	if( !pLastButton )
		SetInitialButton( ButtonId );
	else
	{
		Sprite* pNextSprite = GetSprite( ButtonId );
		Button* pNextButton;

		if( pNextSprite )
		{
			pNextButton = (Button*) pNextSprite->GetInterface( Button::ClassnameStatic() );

			if( !pNextButton )
				pNextButton = (Button*) pNextSprite->AddInterface( Button::ClassnameStatic(), 0 );

			if( pNextButton )
			{
				pLastButton->SetNextButton( pNextButton );
				pNextButton->SetPrevButton( pLastButton );
				pNextButton->SetNextButton( 0 );
			}
		}
	}

	return;
}

//=============================================================================
// Screen::GetLastButton
//=============================================================================

Button* Screen::GetLastButton( void )
{
	Button* pLastButton = GetInitialButton();

	if( pLastButton )
	{
		while( pLastButton->GetNextButton() )
			pLastButton = pLastButton->GetNextButton();
	}

	return pLastButton;
}

//=============================================================================
// Screen::GetCurrentButton
//=============================================================================

Button* Screen::GetCurrentButton( void )
{
	return m_pCurrentButton;
}

//=============================================================================
// Screen::GetCurrentButtonId
//=============================================================================

int Screen::GetCurrentButtonId( void )
{
	int ButtonId = 0;
	Button* pButton = GetCurrentButton();

	if( pButton )
		ButtonId = pButton->GetButtonId();

	return ButtonId;
}

//=============================================================================
// Screen::GetCurrentButtonIndex
//=============================================================================

int Screen::GetCurrentButtonIndex( void )
{
	Button* pButton = GetInitialButton();
	Button* pCurrentButton = GetCurrentButton();

	for( int i = 0; pButton; i++ )
	{
		if( pButton == pCurrentButton )
			return i;

		pButton = pButton->GetNextButton();
	}

	return -1;
}

//=============================================================================
// Screen::GetNthButton
//=============================================================================

Button* Screen::GetNthButton( int n )
{
	Button* pButton = GetInitialButton();
	
	for( int i = 0; i < n; i++ )
	{
		if( pButton )
			pButton = pButton->GetNextButton();
	}

	return pButton;
}

//=============================================================================
// Screen::GetNthButtonId
//=============================================================================

int Screen::GetNthButtonId( int n )
{
	Button* pButton = GetNthButton(n);

	if( pButton )
		return pButton->GetButtonId();

	return 0;
}

//=============================================================================
// Screen::SelectButton
//=============================================================================

void Screen::SelectButton( int ButtonId )
{
	SelectButton( GetButton( ButtonId ) );
}

void Screen::SelectButton( Button* pNewButton )
{
	Button* pOldButton = GetCurrentButton();

	if( pNewButton != pOldButton )
	{
		if( m_pScreenHandler && pOldButton)
		{
			m_pScreenHandler->OnButtonUnselect( pOldButton, pOldButton->SpriteHost() );
		}

		m_pCurrentButton = pNewButton;

		if( pNewButton && m_pScreenHandler )
		{
			m_pScreenHandler->OnButtonSelect( pNewButton, pNewButton->SpriteHost() );
		}
	}

	return;
}

void Screen::SelectInitialButton( void )
{
	Button* pButton = GetInitialButton();

	if( pButton )
	{
		while( pButton && !pButton->Enabled() )
			pButton = pButton->GetNextButton();
	
		SelectButton( pButton );
	}

	return;
}

void Screen::SelectNextButton( void )
{
	Button* pOldButton = GetCurrentButton();
	Button* pNewButton = 0;

	if( !pOldButton )
		pNewButton = GetInitialButton();
	else
	{
		pNewButton = pOldButton->GetNextButton();
		
		if( !pNewButton )
			pNewButton = GetInitialButton();
	}

	if( !pOldButton )
		pOldButton = pNewButton;

	while( pNewButton && !pNewButton->Enabled() )
	{
		pNewButton = pNewButton->GetNextButton();

		if( !pNewButton )
			pNewButton = GetInitialButton();

		if( pOldButton == pNewButton )
			break;
	}

	SelectButton( pNewButton );

	if( pNewButton != pOldButton )
		PlaySoundEvent( SOUNDEVENT_NEXTBUTTON );

	return;
}

void Screen::SelectPrevButton( void )
{
	Button* pOldButton = GetCurrentButton();
	Button* pNewButton = 0;

	if( !pOldButton )
		pNewButton = GetInitialButton();
	else
	{
		pNewButton = pOldButton->GetPrevButton();

		if( !pNewButton )
			pNewButton = GetLastButton();
	}

	if( !pOldButton )
		pOldButton = pNewButton;

	while( pNewButton && !pNewButton->Enabled() )
	{
		pNewButton = pNewButton->GetPrevButton();

		if( !pNewButton )
			pNewButton = GetLastButton();

		if( pOldButton == pNewButton )
			break;
	}

	SelectButton( pNewButton );

	if( pNewButton != pOldButton )
		PlaySoundEvent( SOUNDEVENT_PREVBUTTON );

	return;
}

void Screen::SelectNthButton( int n )
{
	Button* pButton = GetInitialButton();

	for( int i = 0; i < n; i++ )
	{
		if( pButton )
			pButton = pButton->GetNextButton();
	}

	SelectButton( pButton );
}

void Screen::CycleButton( int N )
{
	Button* pButton = GetCurrentButton();

	if( pButton )
	{
		Sprite* pSpriteHost = pButton->SpriteHost();

		if( pSpriteHost )
		{
			GuiCycleButton* pCycleButton = (GuiCycleButton *)
				pSpriteHost->GetInterface( GuiCycleButton::ClassnameStatic() );

			if( pCycleButton )
			{
				int CurrentItem = pCycleButton->CurrentItem();

				if( N < 0 )
					pCycleButton->SelectPrevItem();
				else
				if( N > 0 )
					pCycleButton->SelectNextItem();

				if( CurrentItem != pCycleButton->CurrentItem() )
				{
					if( N < 0 )
						PlaySoundEvent( SOUNDEVENT_PREVBUTTON );
					else
						PlaySoundEvent( SOUNDEVENT_NEXTBUTTON );

					if( m_pScreenHandler )
						m_pScreenHandler->OnButtonCycle( pCycleButton->GetButtonId(), pCycleButton, pSpriteHost );
				}
			}
		}
	}

	return;
}

void Screen::CycleNextButton( void )
{
	CycleButton(1);
}

void Screen::CyclePrevButton( void )
{
	CycleButton(-1);
}

void Screen::UnselectButton( void )
{
	SelectButton(0);
}

Button* Screen::GetDefaultButton( void )
{
	if( m_pDefaultButton )
		return m_pDefaultButton;

	return m_pInitialButton;
}

void Screen::SetDefaultButton( int ButtonId )
{
	SetDefaultButton( GetButton( ButtonId ) );
}

void Screen::SetDefaultButton( Button* pButton )
{
	m_pDefaultButton = pButton;
}

void Screen::SelectDefaultButton( void )
{
	SelectButton( GetDefaultButton() );
}

void Screen::EnableButton( int ButtonId, bool Enabled )
{
	Button* pButton = GetButton( ButtonId );

	if( pButton && pButton->Enabled() != Enabled )
	{
		pButton->Enable( Enabled );

		if( m_pScreenHandler )
		{
			if( Enabled )
				m_pScreenHandler->OnButtonEnabled( pButton, pButton->SpriteHost() );
			else
			{
				if( pButton == GetCurrentButton() )
					UnselectButton();

				m_pScreenHandler->OnButtonDisabled( pButton, pButton->SpriteHost() );
			}
		}
	}

	return;
}

bool Screen::ButtonEnabled( int ButtonId )
{
	Button* pButton = GetButton( ButtonId );

	if( !pButton )
		return false;

	return pButton->Enabled();
}

void Screen::PressButton( void )
{
	Button* pButton = GetCurrentButton();

	if( pButton )
	{
		SOUNDEVENT SoundEvent = SOUNDEVENT_NONE;

		if( !pButton->Enabled() )
			SoundEvent = SOUNDEVENT_DISABLED;
		else
		{
			SoundEvent = SOUNDEVENT_SELECT;

			if( m_pScreenHandler )
				m_pScreenHandler->OnCommand( pButton->GetButtonId() );

			const char* pNextScreen = pButton->GetNextScreen();

			if( pNextScreen && strlen(pNextScreen) > 0 )
			{
				Screen* pScreen = GetScreen( pNextScreen );

				if( pScreen )
				{
					if( m_pScreenHandler )
						m_pScreenHandler->OnNextScreen( pScreen );

					GotoScreen( pNextScreen );
				}
				else if( m_pScreenHandler )
				{
					m_pScreenHandler->OnInvalidAction();
				}
			}
		}

		if( SoundEvent != SOUNDEVENT_NONE )
			PlaySoundEvent( SoundEvent );
	}

	return;
}

void Screen::PressCancel( void )
{
	ScreenManager* pScreenManager = GetScreenManager();
	Screen* pCurrentScreen;
	Screen* pActiveScreen;

	if( pScreenManager )
	{
		pCurrentScreen = pScreenManager->GetCurrentScreen();
		pActiveScreen = pScreenManager->GetActiveScreen();

		if( pActiveScreen == pCurrentScreen )
		{
			if( pScreenManager->GetScreenStack() )
				PlaySoundEvent( SOUNDEVENT_PREVSCREEN );
		}
		else
		if( pActiveScreen )
			PlaySoundEvent( SOUNDEVENT_HIDEPOPUP );

		if( pScreenManager->GetActiveScreen() == this )
			pScreenManager->EndScreen(0);
	}

	return;
}

void Screen::SetNextScreen( int ButtonId, const char* pScreenName )
{
	SetNextScreen( GetButton( ButtonId ), pScreenName );
}

void Screen::SetNextScreen( Button* pButton, const char* pScreenName )
{
	if( pButton )
		pButton->SetNextScreen( pScreenName );

	return;
}

const char* Screen::GetNextScreen( int ButtonId )
{
	return GetNextScreen( GetButton( ButtonId ) );
}

const char* Screen::GetNextScreen( Button* pButton )
{
	const char* pNextScreen = 0;
	
	if( pButton )
		pNextScreen = pButton->GetNextScreen();

	return pNextScreen;
}

void Screen::SetCursor( int SpriteId )
{
	Sprite* pCursor = GetSprite( SpriteId );

	if( pCursor )
		SetCursor( pCursor );

	return;
}

void Screen::SetCursor( Sprite* pCursor )
{
	if( m_pCursor )
		m_pCursor->SetPosition( m_CursorPos.x(), m_CursorPos.y() );

	m_CursorVec.Set( 0.0f, 0.0f );

	m_pCursor = pCursor;

	if( m_pInitialButton )
	{
		Sprite* pButton = m_pInitialButton->SpriteHost();

		if( pCursor && pButton )
		{
			float x, y;
			float x1, y1;
			float x2, y2;
			
			pButton->GetScreenPosition( x1, y1 );
			pCursor->GetScreenPosition( x2, y2 );
			pCursor->GetPosition( x, y );

			m_CursorVec.Set( x2 - x1, y2 - y1 );
			m_CursorPos.Set( x, y );
		}
	}

	return;
}

void Screen::SetCursorPos( float x, float y )
{
	if( m_pCursor )
		m_pCursor->SetPosition( x + m_CursorVec.x(), y + m_CursorVec.y() );

	return;
}

void Screen::SetCursorScreenPos( float x, float y )
{
	if( m_pCursor )
		m_pCursor->SetScreenPosition( x + m_CursorVec.x(), y + m_CursorVec.y() );

	return;
}

Sprite* Screen::GetCursor( void )
{
	return m_pCursor;
}

void Screen::SetCursorOffset( float x, float y )
{
	m_CursorVec.Set( x, y );
}

void Screen::GetCursorOffset( float &x, float &y )
{
	x = m_CursorVec.x();
	y = m_CursorVec.y();
}

void Screen::SetSoundEvent( SOUNDEVENT SoundEvent, const char* pSound )
{
	if( !ValidSoundEvent( SoundEvent ) )
		return;

	const int n = sizeof(m_SoundEvent[0]) /
					  sizeof(m_SoundEvent[0][0]) - 1;

	strncpy( m_SoundEvent[SoundEvent], pSound, n );
}

const char* Screen::GetSoundEvent( SOUNDEVENT SoundEvent )
{
	if( !ValidSoundEvent( SoundEvent ) )
		return 0;

	return m_SoundEvent[SoundEvent];
}

void Screen::PlaySoundEvent( SOUNDEVENT SoundEvent )
{
	if( !ValidSoundEvent( SoundEvent ) )
		return;

	PlaySound( GetSoundEvent( SoundEvent ) );
}

bool Screen::ValidSoundEvent( SOUNDEVENT SoundEvent )
{
	if( SoundEvent < 0 )
		return false;

	if( SoundEvent >= SOUNDEVENT_MAX )
		return false;

	return true;
}

#define SCREEN_LOADSAVE_VERSION1		(0x100)
#define SCREEN_LOADSAVE_VERSION		SCREEN_LOADSAVE_VERSION1

bool Screen::Save( MemoryBank& m )
{
	u32 Version = SCREEN_LOADSAVE_VERSION;

	if( !m.Write( &Version, sizeof(Version) ) )
		return false;

	const char* pHandler = m_ScreenHandlerClass.get();

	if( !pHandler || 0 == strlen(pHandler) )
		pHandler = ScreenHandler::ClassnameStatic();

	if( !m.WriteString( (void*) pHandler ) )
		return false;

	return true;
}

bool Screen::Load( MemoryBank& m )
{
	u32 Version;
	xstring Handler;

	if( !m.Read( &Version, sizeof(Version) ) )
		return false;

	if( Version == SCREEN_LOADSAVE_VERSION1 )
	{
		if( !m.ReadString( Handler ) )
			return false;

		SetScreenHandlerClass( Handler.get() );
	}

	return true;
}

void Screen::OnUpdate( void )
{
	if( m_pScreenHandler && m_pScreenHandler->GetVisible() )
		m_pScreenHandler->OnScreenRefresh();

	return;
}

Button::Button( void )
{
	memset( &m_Param, 0, sizeof(m_Param) );

	m_pNextButton = 0;
	m_pPrevButton = 0;
	m_pParam = 0;
	m_Selected = false;
	m_Enabled = true;
}

int Button::GetButtonId( void )
{
	int OjbectId = 0;

	Sprite* pSprite = SpriteHost();

	if( pSprite )
		OjbectId = pSprite->ObjectId();

	return OjbectId;
}

void Button::Select( void )
{
	m_Selected = true;
}

void Button::Unselect( void )
{
	m_Selected = false;
}

void Button::Enable( bool Enabled )
{
	m_Enabled = Enabled;
}

bool Button::Enabled( void )
{
	return m_Enabled;
}

void Button::SetNextScreen( const char* pScreenName )
{
	if( !pScreenName )
		pScreenName = "";

	int n = sizeof(m_Param.szNextScreen)/sizeof(m_Param.szNextScreen[0])-1;
	
	strncpy( m_Param.szNextScreen, pScreenName, n );

	m_Param.szNextScreen[n] = 0;
}

const char* Button::GetNextScreen( void )
{
	return m_Param.szNextScreen;
}

void Button::SetNextButton( Button* pNextButton )
{
	m_pNextButton = pNextButton;
}

void Button::SetPrevButton( Button* pPrevButton )
{
	m_pPrevButton = pPrevButton;
}

void Button::SetParam( void* pParam )
{
	m_pParam = pParam;
}

void* Button::GetParam( void )
{
	return m_pParam;
}

Button* Button::GetNextButton( void )
{
	return m_pNextButton;
}

Button* Button::GetPrevButton( void )
{
	return m_pPrevButton;
}

bool Button::Init( void* Param )
{
	ButtonParamStruct* pParam = (ButtonParamStruct*) Param;

	if( pParam )
	{
		m_Param = *pParam;
	}

	Sprite* pSprite = SpriteHost();

	if( pSprite )
		pSprite->GetStateStruct( m_InitialState );

	return SpriteInterface::Init( Param );
}

void Button::OnUpdate( void )
{
}

MAKE_CLASSFACTORY( ScreenHandler );

ScreenHandler::ScreenHandler( void )
{
	m_pScreen = 0;
	m_Active = false;
	m_InputAllowed = true;
}

void ScreenHandler::PlaySound( const char* pSound )
{
	if( m_pScreen )
		m_pScreen->PlaySound( pSound );

	return;
}

void ScreenHandler::Show( void )
{
	if( m_pScreen )
		m_pScreen->Show();

	return;
}

void ScreenHandler::Hide( void )
{
	if( m_pScreen )
		m_pScreen->Hide();

	return;
}

void ScreenHandler::HandleInput( UIHandler* pUIHandler )
{
	if( pUIHandler && m_InputAllowed )
	{
		if( pUIHandler->ButtonPressed( UIHandler::CURSOR_UP ) )
			OnCursorUp();
		else
		if( pUIHandler->ButtonPressed( UIHandler::CURSOR_DOWN ) )
			OnCursorDown();

		if( pUIHandler->ButtonPressed( UIHandler::CURSOR_LEFT ) )
			OnCursorLeft();
		else
		if( pUIHandler->ButtonPressed( UIHandler::CURSOR_RIGHT ) )
			OnCursorRight();

		if( pUIHandler->ButtonPressed( UIHandler::BUTTON_OK ) )
			OnButtonOK();
		else
		if( pUIHandler->ButtonPressed( UIHandler::BUTTON_CANCEL ) )
			OnButtonCancel();
		else
		if( pUIHandler->ButtonPressed( UIHandler::BUTTON_START ) )
			OnButtonStart();
		else
		if( pUIHandler->ButtonPressed( UIHandler::BUTTON_SELECT ) )
			OnButtonSelect();
	}

	return;
}

Button* ScreenHandler::GetButton( int ButtonId )
{
	Button* pButton = 0;

	if( m_pScreen )
		pButton = m_pScreen->GetButton( ButtonId );

	return pButton;
}

void ScreenHandler::SetInitialButton( int ButtonId )
{
	if( m_pScreen )
		m_pScreen->SetInitialButton( ButtonId );

	return;
}

void ScreenHandler::SetInitialButton( Button* pButton )
{
	if( m_pScreen )
		m_pScreen->SetInitialButton( pButton );

	return;
}

Button* ScreenHandler::GetInitialButton( void )
{
	Button* pButton = 0;

	if( m_pScreen )
		pButton = m_pScreen->GetInitialButton();

	return pButton;
}

int ScreenHandler::GetInitialButtonId( void )
{
	int ButtonId = 0;
	Button* pButton = GetInitialButton();

	if( pButton )
		ButtonId = pButton->GetButtonId();

	return ButtonId;
}

void ScreenHandler::SetNextButton( int ButtonId )
{
	if( m_pScreen )
		m_pScreen->SetNextButton( ButtonId );

	return;
}

Button* ScreenHandler::GetCurrentButton( void )
{
	Button* pButton = 0;

	if( m_pScreen )
		pButton = m_pScreen->GetCurrentButton();

	return pButton;
}

int ScreenHandler::GetCurrentButtonId( void )
{
	int ButtonId = 0;

	if( m_pScreen )
		ButtonId = m_pScreen->GetCurrentButtonId();

	return ButtonId;
}

int ScreenHandler::GetCurrentButtonIndex( void )
{
	int i = -1;

	if( m_pScreen )
		i = m_pScreen->GetCurrentButtonIndex();

	return i;
}

Button* ScreenHandler::GetNthButton( int n )
{
	Button* pButton = 0;

	if( m_pScreen )
		pButton = m_pScreen->GetNthButton(n);

	return pButton;
}

int ScreenHandler::GetNthButtonId( int n )
{
	int i = 0;

	if( m_pScreen )
		i = m_pScreen->GetNthButtonId(n);

	return i;
}

void ScreenHandler::SelectButton( int ButtonId )
{
	if( m_pScreen )
		m_pScreen->SelectButton( ButtonId );

	return;
}

void ScreenHandler::SelectButton( Button* pButton )
{
	if( m_pScreen )
		m_pScreen->SelectButton( pButton );

	return;
}

void ScreenHandler::SelectInitialButton( void )
{
	if( m_pScreen )
		m_pScreen->SelectInitialButton();

	return;
}

void ScreenHandler::SelectNextButton( void )
{
	if( m_pScreen )
		m_pScreen->SelectNextButton();

	return;
}

void ScreenHandler::SelectPrevButton( void )
{
	if( m_pScreen )
		m_pScreen->SelectPrevButton();

	return;
}

void ScreenHandler::SelectNthButton( int n )
{
	if( m_pScreen )
		m_pScreen->SelectNthButton( n );

	return;
}

void ScreenHandler::CycleButton( int N )
{
	if( m_pScreen )
		m_pScreen->CycleButton( N );

	return;
}

void ScreenHandler::CycleNextButton( void )
{
	if( m_pScreen )
		m_pScreen->CycleNextButton();

	return;
}

void ScreenHandler::CyclePrevButton( void )
{
	if( m_pScreen )
		m_pScreen->CyclePrevButton();

	return;
}

void ScreenHandler::UnselectButton( void )
{
	if( m_pScreen )
		m_pScreen->UnselectButton();

	return;
}

Button* ScreenHandler::GetDefaultButton( void )
{
	Button* pButton = 0;

	if( m_pScreen )
		pButton = m_pScreen->GetDefaultButton();

	return pButton;
}

void ScreenHandler::SetDefaultButton( int ButtonId )
{
	if( m_pScreen )
		m_pScreen->SetDefaultButton( ButtonId );

	return;
}

void ScreenHandler::SetDefaultButton( Button* pButton )
{
	if( m_pScreen )
		m_pScreen->SetDefaultButton( pButton );

	return;
}

void ScreenHandler::SelectDefaultButton( void )
{
	if( m_pScreen )
		m_pScreen->SelectDefaultButton();

	return;
}

void ScreenHandler::EnableButton( int ButtonId, bool Enabled )
{
	if( m_pScreen )
		m_pScreen->EnableButton( ButtonId, Enabled );

	return;
}

bool ScreenHandler::ButtonEnabled( int ButtonId )
{
	if( !m_pScreen )
		return false;

	return m_pScreen->ButtonEnabled( ButtonId );
}

void ScreenHandler::PressButton( void )
{
	if( m_pScreen )
		m_pScreen->PressButton();

	return;
}

void ScreenHandler::PressCancel( void )
{
	if( m_pScreen )
		m_pScreen->PressCancel();

	return;
}

void ScreenHandler::SetButtonParam( int ButtonId, void* pParam )
{
	Button* pButton = GetButton( ButtonId );

	if( pButton )
		pButton->SetParam( pParam );

	return;
}

void* ScreenHandler::GetButtonParam( int ButtonId )
{
	void* pParam = 0;
	Button* pButton = GetButton( ButtonId );

	if( pButton )
		pParam = pButton->GetParam();

	return pParam;
}

void ScreenHandler::SetNextScreen( int ButtonId, const char* pScreenName )
{
	if( m_pScreen )
		m_pScreen->SetNextScreen( ButtonId, pScreenName );

	return;
}

void ScreenHandler::SetNextScreen( Button* pButton, const char* pScreenName )
{
	if( m_pScreen )
		m_pScreen->SetNextScreen( pButton, pScreenName );

	return;
}

const char* ScreenHandler::GetNextScreen( Button* pButton )
{
	const char* pNextScreen = 0;

	if( m_pScreen )
		pNextScreen = m_pScreen->GetNextScreen( pButton );

	return pNextScreen;
}

const char* ScreenHandler::GetNextScreen( int ButtonId )
{
	const char* pNextScreen = 0;

	if( m_pScreen )
		pNextScreen = m_pScreen->GetNextScreen( ButtonId );

	return pNextScreen;
}

void ScreenHandler::SetActive( bool Active )
{
	m_Active = Active;
}

bool ScreenHandler::GetActive( void )
{
	return m_Active;
}

bool ScreenHandler::GetVisible( void )
{
	Sprite* pScreenSprite = GetScreenSprite();

	if( !pScreenSprite )
		return false;

	if( pScreenSprite->m_state.familycolor.a == 0 )
		return false;

	if( !pScreenSprite->m_state.sprite.visible )
		return false;

	return true;
}

void ScreenHandler::AllowInput( bool Allowed )
{
	m_InputAllowed = Allowed;
}

bool ScreenHandler::IsInputAllowed( void )
{
	return m_InputAllowed;
}

void ScreenHandler::SetCursor( int SpriteId )
{
	if( m_pScreen )
		m_pScreen->SetCursor( SpriteId );

	return;
}

void ScreenHandler::SetCursor( Sprite* pSprite )
{
	if( m_pScreen )
		m_pScreen->SetCursor( pSprite );

	return;
}

void ScreenHandler::SetCursorPos( float x, float y )
{
	if( m_pScreen )
		m_pScreen->SetCursorPos( x, y );
}

void ScreenHandler::SetCursorScreenPos( float x, float y )
{
	if( m_pScreen )
		m_pScreen->SetCursorScreenPos( x, y );

	return;
}

Sprite* ScreenHandler::GetCursor( void )
{
	Sprite* pCursor = 0;

	if( m_pScreen )
		pCursor = m_pScreen->GetCursor();

	return pCursor;
}

void ScreenHandler::SetCursorOffset( float x, float y )
{
	if( m_pScreen )
		m_pScreen->SetCursorOffset( x, y );

	return;
}

void ScreenHandler::GetCursorOffset( float &x, float &y )
{
	x = y = 0.0f;

	if( m_pScreen )
		m_pScreen->GetCursorOffset( x, y );

	return;
}

#ifdef DIRECTX_PC
void ScreenHandler::SelectMenuItem() 
{
	g_MenuCursor.ForceButtonUp(0);
    OnButtonOK(); 
}
#endif

void ScreenHandler::SetSoundEvent( Screen::SOUNDEVENT SoundEvent, const char* pSound )
{
	if( m_pScreen )
		m_pScreen->SetSoundEvent( SoundEvent, pSound );

	return;
}

const char* ScreenHandler::GetSoundEvent( Screen::SOUNDEVENT SoundEvent )
{
	if( !m_pScreen )
		return 0;

	return m_pScreen->GetSoundEvent( SoundEvent );
}

void ScreenHandler::PlaySoundEvent( Screen::SOUNDEVENT SoundEvent )
{
	if( m_pScreen )
		m_pScreen->PlaySoundEvent( SoundEvent );

	return;
}

void ScreenHandler::OnInitialize( void )
{
}

void ScreenHandler::OnTerminate( void )
{
}

void ScreenHandler::OnCommand( int Command )
{
}

void ScreenHandler::OnInvalidAction( void )
{
	PlaySoundEvent( Screen::SOUNDEVENT_INVALID );
}

void ScreenHandler::OnCursorUp( void )
{
	SelectPrevButton();
}

void ScreenHandler::OnCursorDown( void )
{
	SelectNextButton();
}

void ScreenHandler::OnCursorLeft( void )
{
	CycleButton(-1);
}

void ScreenHandler::OnCursorRight( void )
{
	CycleButton(1);
}

void ScreenHandler::OnButtonOK( void )
{
	PressButton();
}

void ScreenHandler::OnButtonCancel( void )
{
	PressCancel();
}

void ScreenHandler::OnButtonStart( void )
{
#if defined _XBOX || defined DIRECTX_PC
	PressButton();
#endif
}

void ScreenHandler::OnButtonSelect( void )
{
}

void ScreenHandler::OnNextButton( void )
{
}

void ScreenHandler::OnPrevButton( void )
{
}

void ScreenHandler::OnNextScreen( Screen* pNextScreen )
{
}

void ScreenHandler::OnPrevScreen( Screen* pPrevScreen )
{
}

void ScreenHandler::OnScreenIntro( void )
{
	SelectDefaultButton();
}

void ScreenHandler::OnScreenExit( void )
{
	UnselectButton();
}

void ScreenHandler::OnScreenUpdate( void )
{
#ifdef DIRECTX_PC

	if (g_MenuCursor.IsActive() && !m_pScreen->GetActivePopup() ) {
		float x1, y1, x2, y2;	
		// check for back navigation
		if (g_MenuCursor.CursorButtonPressed(1))
		{
			g_MenuCursor.ForceButtonUp(1);
			OnButtonCancel();
		}
		float x = g_MenuCursor.GetX();
		float y = g_MenuCursor.GetY();
		// functionality for list box based menus
		GuiListBox* listBox = GetListBox();
		if (listBox)
		{
			int items = listBox->GetItemCount();
			for (int c=0; c < items; ++c)
			{
				listBox->GetCursorRect(x1,y1,x2,y2);
				// HACK HACK HACK
				// expand this out a little bit for the delete
				// button for the profile listbox. ugh
				x1 -= 32;
				if ( ( x > x1 && x < x2 ) && ( y > y1 && y < y2 ) )
				{
					if ( g_MenuCursor.CursorButtonPressed(0) )
						SelectMenuItem();
					break;
				}
				//listBox->OnCursorDown();
				OnCursorDown();
			}
		}
		Button* button = GetDefaultButton();
		// functionality for button based menus
		if (!button)
		{
			if (g_MenuCursor.CursorButtonPressed(0))
				OnButtonStart();
		}
		// search through the buttons to see if we're over one
		// if so, check to see if we're clicking the mouse
		while(button) 
		{
			button->SpriteHost()->GetFamilyRect(x1, y1, x2, y2);
			if ( ( x > x1 && x < x2 ) && ( y > y1 && y < y2 ) )
			{
				if (!button->Enabled()) break;
				SelectButton(button);
				if ( g_MenuCursor.CursorButtonPressed(0) )
				{
						button->m_Selected = true;
						g_MenuCursor.ForceButtonUp(0);
						OnButtonOK(); 
				}
				else
				{
						button->m_Selected = false;
				}
				break;
			}
			button = button->GetNextButton();
		}
	}
	// add support for scroll wheel
	int mouseWheelState = g_MenuCursor.GetMouseWheelState();
	if (mouseWheelState == 1) OnCursorUp();
	else if (mouseWheelState == -1) OnCursorDown();

#endif
}

void ScreenHandler::OnScreenRefresh( void )
{
}

void ScreenHandler::OnPopupShow( void )
{
}

void ScreenHandler::OnPopupHide( void )
{
}

void ScreenHandler::OnButtonSelect( Button* pButton, Sprite* pSprite )
{
}

void ScreenHandler::OnButtonUnselect( Button* pButton, Sprite* pSprite )
{
}

void ScreenHandler::OnButtonEnabled( Button* pButton, Sprite* pSprite )
{
	if( pButton && pSprite )
	{
		pSprite->m_state.color.b = pButton->m_InitialState.color.b;
		pSprite->m_state.color.g = pButton->m_InitialState.color.g;
		pSprite->m_state.color.r = pButton->m_InitialState.color.r;
	}

	return;
}

void ScreenHandler::OnButtonDisabled( Button* pButton, Sprite* pSprite )
{
	if( pButton && pSprite )
	{
		pSprite->m_state.color.b = pButton->m_InitialState.color.b >> 2;
		pSprite->m_state.color.g = pButton->m_InitialState.color.g >> 2;
		pSprite->m_state.color.r = pButton->m_InitialState.color.r >> 2;
	}

	return;
}

void ScreenHandler::OnButtonCycle( int ButtonId, GuiCycleButton* pCycleButton, Sprite *pSprite )
{
}

void ScreenHandler::OnScreenMessage( ScreenHandler* pScreenHandler, int message, void* pParam )
{
}

void ScreenHandler::OnGuiMessage( GuiControl* pControl, int message, void* pParam )
{
}

Sprite* ScreenHandler::GetScreenSprite( void )
{
	Sprite* pSprite = 0;

	if( m_pScreen )
		pSprite = m_pScreen->SpriteHost();

	return pSprite;
}

Sprite* ScreenHandler::GetSprite( int SpriteId )
{
	Sprite* pSprite = 0;

	if( m_pScreen )
		pSprite = m_pScreen->GetSprite( SpriteId );

	return pSprite;
}

Sprite* ScreenHandler::GetSprite( int SpriteId, Sprite* pParent )
{
	Sprite* pSprite = 0;

	if( m_pScreen )
		pSprite = m_pScreen->GetSprite( SpriteId, pParent );

	return pSprite;
}

Text* ScreenHandler::GetTextSprite( int SpriteId )
{
#ifdef _DEBUG
	Sprite* pSprite = GetSprite( SpriteId );

	if( pSprite && strcmp( Text::ClassnameStatic(), pSprite->Classname() ) )
	{
		ASSERT(0);
		return 0;
	}

	return (Text*) pSprite;
#else
	return (Text*) GetSprite( SpriteId );
#endif
}

Text* ScreenHandler::GetTextSprite( int SpriteId, Sprite* pParent )
{
#ifdef _DEBUG
	Sprite* pSprite = GetSprite( SpriteId, pParent );

	if( pSprite && strcmp( Text::ClassnameStatic(), pSprite->Classname() ) )
	{
		ASSERT(0);
		return 0;
	}

	return (Text*) pSprite;
#else
	return (Text*) GetSprite( SpriteId, pParent );
#endif
}

SpriteInterface* ScreenHandler::GetInterface( int SpriteId, const char* pInterfaceName )
{
	SpriteInterface* pInterface = 0;

	if( m_pScreen )
		pInterface = m_pScreen->GetInterface( SpriteId, pInterfaceName );

	return pInterface;
}

SpriteInterface* ScreenHandler::AddInterface( int SpriteId, const char* pInterfaceName )
{
	SpriteInterface* pInterface = 0;

	if( m_pScreen )
		pInterface = m_pScreen->AddInterface( SpriteId, pInterfaceName );

	return pInterface;
}

SpriteInterface* ScreenHandler::AddInterface( Sprite* pSprite, const char* pInterfaceName )
{
	SpriteInterface* pInterface = 0;

	if( m_pScreen )
		pInterface = m_pScreen->AddInterface( pSprite, pInterfaceName );

	return pInterface;
}

Screen* ScreenHandler::GetScreen( const char* pScreenName )
{
	Screen* pScreen = 0;

	if( m_pScreen )
		pScreen = m_pScreen->GetScreen( pScreenName );

	return pScreen;
}

ScreenHandler* ScreenHandler::GetScreenHandler( const char* pScreenName, const char* pClassName )
{
	ScreenManager* pScreenManager = GetScreenManager();

	if( pScreenManager )
		return pScreenManager->GetScreenHandler( pScreenName, pClassName );

	return 0;
}

ScreenHandler* ScreenHandler::GetScreenHandler( const char* pScreenName )
{
	return GetScreenHandler( GetScreen( pScreenName ) );
}

ScreenHandler* ScreenHandler::GetScreenHandler( Screen* pScreen )
{
	ScreenManager* pScreenManager = GetScreenManager();

	if( pScreenManager )
		return pScreenManager->GetScreenHandler( pScreen );

	return 0;
}

ScreenManager* ScreenHandler::GetScreenManager( void )
{
	ScreenManager* pScreenManager = 0;

	if( m_pScreen )
		pScreenManager = m_pScreen->GetScreenManager();

	return pScreenManager;
}

UIHandler* ScreenHandler::GetUIHandler( void )
{
	ScreenManager* pScreenManager = GetScreenManager();

	if( !pScreenManager )
		return 0;

	return pScreenManager->GetUIHandler();
}

int ScreenHandler::ShowPopup( const char* pScreenName )
{
	ScreenManager* pScreenManager = GetScreenManager();

	if( !pScreenManager )
		return false;

	return pScreenManager->ShowPopup( pScreenName );
}

bool ScreenHandler::GotoScreen( const char* pScreenName )
{
	ScreenManager* pScreenManager = GetScreenManager();

	if( !pScreenManager )
		return false;
   
	return pScreenManager->GotoScreen( pScreenName );
}

void ScreenHandler::GotoPrevScreen( void )
{
	ScreenManager* pScreenManager = GetScreenManager();

	if( pScreenManager )
		pScreenManager->GotoPrevScreen();

	return;
}

void ScreenHandler::EndScreen( int Result )
{
	ScreenManager* pScreenManager = GetScreenManager();

	if( pScreenManager )
		pScreenManager->EndScreen( Result );

	return;
}

void ScreenHandler::SetTexture( int SpriteId, t_Handle Handle, int Texture )
{
	Sprite* pSprite = GetSprite( SpriteId );

	if( pSprite )
		pSprite->SetTextureSet( Handle, Texture );

	return;
}

void ScreenHandler::SetText( int TextId, const char* pText )
{
	Text* pTextSprite = GetTextSprite( TextId );

	if( pTextSprite )
	{
		char* pClassname = pTextSprite->Classname();

		if( 0 == strcmp( pClassname, "Text" ) )
			pTextSprite->SetText( pText );
	}

	return;
}

void ScreenHandler::SetText( int TextId, int ParentId, const char* pText )
{
	SetText( TextId, GetSprite( ParentId ), pText );
}

void ScreenHandler::SetText( int TextId, Sprite* pParent, const char* pText )
{
	Text* pTextSprite = GetTextSprite( TextId, pParent );

	if( pTextSprite )
		pTextSprite->SetText( pText );

	return;
}


void ScreenHandler::Printf( int TextId, const char* fmt, ... )
{
	static char szPrintf[512];

	va_list args;
	va_start( args, fmt );
	vsprintf( szPrintf, fmt, args );
	va_end( args );

	SetText( TextId, szPrintf );
}

void ScreenHandler::ShowSprite( int SpriteId, bool Show )
{
	Sprite* pSprite = GetSprite( SpriteId );

	if( pSprite )
		pSprite->Show( Show );

	return;
}

#ifdef DIRECTX_PC

//==========================================================================
// MenuCursor::Initialize()
// initializes the pointers necessary for the cursor to function properly
//==========================================================================
bool MenuCursor::Initialize( Sprite* inSpritePtr, SystemInputDriver* inInputPtr )
{
	// for whatever reason, if we've previously intialized our sprite, delete it
	if (pSprite) delete pSprite;
	if (inSpritePtr) pSprite = inSpritePtr;
	else return false;
	pSprite->SetPriority(-1);
	if (inInputPtr) {
		// we should find a way to do this safely
		pInput = (WindowsSystemInputDriver *)inInputPtr;
	}
	else return false;
	return true;
}

//==========================================================================
// MenuCursor::GetRect()
// returns a rectangle with the extents of the sprite
//==========================================================================
bool MenuCursor::GetRect(float &x1, float &x2, float &y1, float&y2)
{
	if (!isInitialized()) return false;
	pSprite->GetFamilyRect(x1,y1,x2,y2);
	return true;
}

//==========================================================================
// MenuCursor::~MenuCursor()
// Menu Cursor deconstructor
//==========================================================================
MenuCursor::~MenuCursor(){
	// delete our sprite, but NOT our input driver
	//if (pSprite) delete pSprite;
}

//==========================================================================
// MenuCursor::~setMaxXandY(float x, float y)
// sets the maximum screen position for the cursor sprite.
//==========================================================================
void MenuCursor::SetMaxXandY(float x, float y)
{
	if(pSprite)
	{
		float spriteHeight, spriteWidth;
		pSprite->GetSize(spriteWidth, spriteHeight);
		maxWindowX = 640 - spriteWidth;
		maxWindowY = 480 - spriteHeight;
	}
}

//==========================================================================
// MenuCursor::IsInitialized()
// private function that should be checked before doing any menu cursor
// operations
//==========================================================================
bool MenuCursor::isInitialized() { return (pSprite && pInput); }

//==========================================================================
// MenuCursor::Update()
// updates the rendering and position information for the cursor
//==========================================================================
bool MenuCursor::Update()
{
	if(!isInitialized()) return false;
	active = false;
	mouseWheelState = pInput->wasMouseWheelMoved();
	if(hidden) {
		pSprite->Update();
		return true;
	}
	// handle our button states
	for ( int c=0; c < NUM_MOUSE_BUTTONS; ++c )
	{
		buttonDownLast[c] = buttonDownNow[c];
		// we still need to be "active" if we pressed a
		// button last frame
		if ( buttonDownLast[c] ) active = true;
		buttonDownNow[c] = false;
	}



	// check for activity
	if ( pInput->MouseX() ||
		 pInput->MouseY() ||
		 pInput->LeftMouseButtonDown() ||
		 pInput->RightMouseButtonDown()   )
	{
		// update our position
		active = true;
		float mousex = (float)pInput->MouseX();
		float mousey = (float)pInput->MouseY();

		xPos += mousex;
		yPos += mousey;

		if( xPos > maxWindowX ) 
			xPos = maxWindowX;
		if( yPos > maxWindowY ) 
			yPos = maxWindowY;
		if( xPos < 0 ) xPos = 0;
		if( yPos < 0 ) yPos = 0;
		if ( pInput->LeftMouseButtonDown() ) buttonDownNow[0] = true;
		else buttonDownNow[0] = false;
		if ( pInput->RightMouseButtonDown() ) buttonDownNow[1] = true;
		else buttonDownNow[1] = false;
		pSprite->SetPosition(xPos, yPos);
		// update the cursor sprite itself
		pSprite->Update();
	}
	return true;
}

MenuCursor g_MenuCursor;
#endif
