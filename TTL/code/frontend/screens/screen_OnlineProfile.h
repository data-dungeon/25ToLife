#ifndef SCREEN_ONLINEPROFILE_H
#define SCREEN_ONLINEPROFILE_H

#include "frontend/screens/screen_BaseClass.h"

// for the message-handling stuff
#include "GameHelper/Message.h"


enum	{
	ONLINEPROFILE_IDLE,
	ONLINEPROFILE_DETECT,
	ONLINEPROFILE_DETECT_RESULT,
	ONLINEPROFILE_GET_PROFILES_RESULT,
	ONLINEPROFILE_LOAD,
	ONLINEPROFILE_LOAD_RESULT,
	ONLINEPROFILE_LOAD_RESULT_OK,
	ONLINEPROFILE_LOAD_BADDATA,
	ONLINEPROFILE_DELETE,
	ONLINEPROFILE_DELETE_RESULT,
};
	
//=============================================================================
// ScreenOnlineProfile
//=============================================================================

class ScreenOnlineProfile : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenOnlineProfile );

	public:
		ScreenOnlineProfile( void );
		virtual ~ScreenOnlineProfile();

		void ShowProfilesPrompt( void );
		void ShowProfileDeletePrompt( void );
		void DeleteProfile( void );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenUpdate();
		virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnCursorUpdate( void );
		virtual void OnButtonOK( void );
		virtual void OnButtonCancel( void );
		virtual void OnButtonPressed( int Button );
		virtual void OnDialogCommand( int Command );
		virtual void OnDialogCancel( void );
		virtual void OnPopupShow( void );
		virtual void OnPopupHide( void );
		virtual void HandleInput( UIHandler* pUIHandler );

		int m_ProfilesFound;
		int m_MemCardState;	
		int m_LastMemCardState;
		int m_Selection;
		int m_CardID;
};

#endif // SCREEN_ONLINEPROFILE_H