#ifndef _SCREEN_PLAYERSLIST_H_
#define _SCREEN_PLAYERSLIST_H_

#include "frontend/screens/screen_BaseClass.h"
#include "frontend/screens/screen_BasePopup.h"
#include "OSI/OSIPlayerList.hpp"

class ScreenPlayersList : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenPlayersList );

	public:
		enum ScreenState {
			RUNNING,
			PLAYER_SELECTED,
			CANCEL,
		};

		/// Accessors and Mutators
		GuiListBox*		GetListBox( void );
		int				GetSelectedPlayerIndex( void )							{ return _selectedPlayerIndex ; }
		ScreenState		GetScreenState( void );

		void			SetRecentPlayersList( OSIPlayerList& recentPlayerList );
		Sprite*     GetFriendIcon( int PlayerIndex );
		void			ShowFriendIcon( int PlayerIndex, bool Show );
		int			CurrentSelection( void );

	protected:
		virtual void	OnInitialize( void );
		virtual void	OnScreenIntro( void );
		virtual void   OnScreenUpdate( void );
		virtual void	OnCursorUp( void );
		virtual void	OnCursorDown( void );
		virtual void   OnCursorUpdate( void );
		virtual void	OnButtonOK( void );
		virtual void	OnButtonCancel( void );
		virtual void   OnPopupShow( void );
		virtual void	OnPopupHide( void );

	private:
		int				_selectedPlayerIndex ;
		ScreenState		_screenState ;
};


class ScreenPlayersPopup : public ScreenBasePopup
{
	DECLARE_CLASSOBJECT( ScreenPlayersPopup );

	public:
		enum ScreenState {
			RUNNING,
			SEND_FRIEND_REQUEST,
			SEND_CLAN_INVITE,
			SEND_MATCH_INVITE,
			CANCEL,
		};

		GuiListBox* GetListBox( void );
        ScreenState GetScreenState() { ScreenState current = _screenState; _screenState = RUNNING; return current; }

		virtual void SetTitle( const char* pText );
		void SetOptionList( OSIClanRole myClanRole, bool isSelectedPlayerInMatch );
	protected:
		virtual void OnInitialize( void );
		virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnButtonOK( void );
		virtual void OnButtonCancel() { _screenState = CANCEL; }

		ScreenState _screenState;
};

#endif // _SCREEN_PLAYERSLIST_H_