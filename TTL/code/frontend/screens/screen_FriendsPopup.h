#ifndef _SCREEN_FRIENDSPOPUP_H_
#define _SCREEN_FRIENDSPOPUP_H_

#include "frontend/screens/screen_BasePopup.h"
#include "OSI/OSIPlayer.hpp"

class ScreenFriendsPopup : public ScreenBasePopup
{
	DECLARE_CLASSOBJECT( ScreenFriendsPopup );

	public:
		enum ScreenState {
			RUNNING,
			ACCEPT_FRIEND_REQUEST,
			DENY_FRIEND_REQUEST,
			BLOCK_FRIEND_REQUEST,
			CANCEL_FRIEND_REQUEST,
			REMOVE_FRIEND,
			JOIN_FRIEND_GAME,
			INVITE_FRIEND_TO_GAME,
			SEND_CLAN_INVITE,
			BACK,
		};

		/// The role the screen can be set to, each displays different options
		enum ScreenRole {
			FRIEND_REQUEST_INCOMING,
			FRIEND_REQUEST_OUTGOING,
			FRIEND_REQUEST_CONFIRMED
		};

		/** \name Accessors */
        //\{
		GuiListBox* GetListBox( void );
        ScreenState GetScreenState() { ScreenState current = _screenState; _screenState = RUNNING; return current; }
		//\}

		/** \name Mutators */
        //\{
		void SetScreenRole( OSIPlayer* selectedPlayer );
		//\}

	protected:
		/** \name Base class override */
        //\{
		virtual void OnInitialize( void );
		virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnButtonOK( void );
		virtual void OnButtonCancel() { _screenState = BACK; }
		//\}

		ScreenState _screenState;
		ScreenRole _screenRole;
};

#endif // _SCREEN_FRIENDSPOPUP_H_