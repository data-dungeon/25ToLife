//-----------------------------------------------------------------------------
// File: XboxLive.h
//
//-----------------------------------------------------------------------------

#ifndef XBOXLIVE_H
#define XBOXLIVE_H

#include <xonline.h>
#include <UIX.h>

// Audio plugin for UIX
struct CXboxLiveSoundNameMap 
{ 
	const char* uix; 
	const char* ttl;
};

enum	{
	CXBOXLIVE_NOINVITE,
	CXBOXLIVE_INVITE,
	CXBOXLIVE_CROSS_INVITE,
};

enum	{
	CXBOXLIVE_NOT_SIGNED_IN,
	CXBOXLIVE_SIGNED_IN,
	CXBOXLIVE_PASSCODE_NEEDED,
	CXBOXLIVE_SIGNIN_FAILED,
	CXBOXLIVE_SIGNED_IN_MESSAGE_PENDING,
	CXBOXLIVE_REQUIRES_MANAGEMENT,
	CXBOXLIVE_ETHERNET_DISCONNECTED,
	CXBOXLIVE_SIGNIN_FAILED_UNKNOWN,
	CXBOXLIVE_AUTO_UPDATE_REQUIRED,
	CXBOXLIVE_NO_NETWORK_CONNECTION,
	CXBOXLIVE_SERVERS_TOO_BUSY,
	CXBOXLIVE_CONNECTION_LOST,
	CXBOXLIVE_UIX_EXIT,
};

enum {
	CXBOXLIVE_NETWORK_CONFIGURATION,
	CXBOXLIVE_NEW_ACCOUNT,
	CXBOXLIVE_ACCOUNT_MANAGEMENT,
	CXBOXLIVE_ONLINE_MENU
};

namespace CXboxLive
{
	// non UIX
	bool		Init(void);									// Init the XOnline engine
	bool		SilentLogin(void);						// attempt a silent login
	bool		LoginUpdate(void);						//	update (called once per frame)
	bool		AuthenticateLogin(void);				// handle login state
	bool		Update(void);								// update (once per frame)	
	void		LogOut(void);								// Log us out of Xbox Live
	void		AutoUpdate(void);							// Call Auto Update

	// UIX
	bool		InitUIX();									// Init UIX
	bool		LoginUIX(void);							// Attempt Login Through UIX
	void		LogOutUIX(void);							// Log us out of Xbox Live using UIX
	void		UIXUpdate(DWORD dwFlags);				// UIX Update (called once per frame)
	bool		KillUIX(void);								// Closedown UIX
	bool		InUix(void);								// Is UIX onscreen?
	bool		IsUIXStarted(void);						// is UIX engine started?
	ILiveEngine*			GetLiveEngine(void);		// Get UIX Engine pointer.
	void		SetSoundMap(CXboxLiveSoundNameMap* Map, int NumEntries);
	bool		EndUIXFeature(void);						// End currently active feature

	// invites
	bool		GetInviteFound(void);					// Invite ?
	void		SetInviteFound(bool Flag);				// Set an invite found?
	int		GetInviteType(void);						// Get type of invite (Cross title, or same title)
	char*		GetInvitersName(void);					// Get Inviters Name
	char*		GetBootInvitersName(void);				// Get Inviters Name on Boot
	XNKID*	GetInviterSessionID(void);				// Get Session ID of inviter (needed to join an existing game)

	// gamertags
	char*		GetUserName(void);						// Get UserName of signed in Gamer

	// invites
	bool		GetGameInvite(void);						// Get GameInvite flag from HD
	XONLINE_ACCEPTED_GAMEINVITE* GetBootInvite(); // get the whole invitation so it can be passed to UIX
	bool		CheckInvites(int Controller, XONLINE_NOTIFICATION_TYPE Flags);		// Check for invites

	// friends / players list
	XONLINE_FRIEND*		GetFriendData(void);		// Get Friend request Struct
	bool		StartFriendsList(void);					// Start Friends List using UIX
	bool		StartPlayerList(void);					// Start Players List using UIX

	// login states
	HRESULT	GetAuthenticate(void);					// Get Authenticate val
	void		SaveLoginState(void);					// Save last Login State for future logins
	XONLINE_LOGON_STATE*		GetLoginState(void);	// Get Login State (Xbox type)
	void		ResetLoginState(void);					// Reset Last Login State
	bool		LogInFailed(void);						// Did Login Fail?
	bool		AreWeLoggedIn(void);						// Are we logged in to Xbox Live?
	bool		AreWeLoggingIn(void);					// Are we in process of logging in to Xbox Live?
	void						ClearLastExitCode(void);// Clear the last UIX exit code
	UIX_EXIT_CODE_TYPE	GetLastExitCode(void);	// Get last UIX Exit code
	bool     CheckExitCondition( UIX_EXIT_CODE_TYPE ExitCode );// Returns true and clears the last exit code if the exit condition has occured - Dan S.

	// online status
	void		ToggleOnlineStatus(void);				// Appear on or offline
	void		SetOnlineStatus(bool AppearOffline);// Appear on or offline (bool)
	bool		GetOnlineStatus(void);					// Get current status
	int		GetLoginStatus(void);					// Get Login Status

	// dashboard
	void		GotoDashboard(int Type);				// Go to Dashboard for Xbox Live Management
	bool		GetAutoLogonStatus( void );			// Get Silent Sign in flag

	// silent sign in flags
	bool		SilentLoginOn(void);						// Is Silent Silent In turned on.
	void		SetSilentLogin(bool Flag);				// Set Silent Sign-in flag

	// language
	int		GetFilteredLanguage();					// return language supported by the game
};

// Font rendering for UIX
class CXboxLiveFont : public ITitleFontRenderer
{
public:

	CXboxLiveFont();
	~CXboxLiveFont();
	STDMETHODIMP_(ULONG)	Release();
	STDMETHODIMP			DrawText(LPCWSTR pText, DWORD X, DWORD Y, DWORD MaxWidth);
	STDMETHODIMP			GetTextSize(LPCWSTR pText, DWORD *pWidth, DWORD *pHeight);
	STDMETHODIMP			SetColor(D3DCOLOR Color);
	STDMETHODIMP			SetHeight(DWORD Height);
    void                BeginFrame();
    void                EndFrame();

private:

    DWORD		mColor;
    float		mHeight;
	 float		mScale;
};

// Audio plugin for UIX
class  CXboxLiveAudio : public ITitleAudioPlugin
{
public:
	virtual	ULONG STDMETHODCALLTYPE Release();
	virtual	HRESULT STDMETHODCALLTYPE DoWork();
	virtual	HRESULT STDMETHODCALLTYPE PlaySound( LPCSTR soundName );
};

#endif	//XBOXLIVE_H

