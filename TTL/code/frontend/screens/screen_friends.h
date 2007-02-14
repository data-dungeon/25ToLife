#ifndef SCREEN_FRIENDS_H
#define SCREEN_FRIENDS_H

#include "frontend/screens/screen_BaseClass.h"
#include "OSI/OSIPlayerList.hpp"

//=============================================================================
// ScreenFriends
//=============================================================================

class ScreenFriends : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenFriends );

	public:
        enum ScreenStatus
        {
            RUNNING,  /// < Running, nothing selected
			VIEW_CONFIRMED_FRIENDS,
			VIEW_INCOMING_FRIEND_REQUESTS,
			VIEW_OUTGOING_FRIEND_REQUESTS,
            RECENT_PLAYERS_LIST, /// < Selected recent players list
            SEND_FRIEND_REQUEST_BY_NAME,     /// < Selected add friend by name
            GOT_NAME_FOR_FRIEND_REQUEST, /// < Got a name from the virtual keyboard
            BACK      /// < Screen cancelled
        };

        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
        virtual void OnButtonCancel() { _status = BACK; }
        virtual void OnCommand( int Command );
		virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnCursorUpdate( void );
		  virtual void OnKeyboardInput( const char* pString );
#ifdef DIRECTX_PC
		virtual void OnScreenUpdate();
#endif
        //\}

        /** \name Accessors */
        //\{
        ScreenStatus GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
        const char *GetEnteredFriendName();
        //\}
    private:
        /** \name Utility methods */
        //\{
        //\}

		ScreenStatus _status;
};

#endif // SCREEN_FRIENDS_H
