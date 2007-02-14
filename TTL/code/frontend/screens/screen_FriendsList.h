#ifndef SCREEN_FRIENDSLIST_H
#define SCREEN_FRIENDSLIST_H

#include "frontend/screens/screen_BaseClass.h"
#include "OSI/OSIPlayerList.hpp"

//=============================================================================
// ScreenFriendsList
//=============================================================================

class ScreenFriendsList : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenFriendsList );

	public:
        enum ScreenStatus
        {
            RUNNING,
            FRIEND_SELECTED,
            BACK
        };

		enum DisplayMode
		{
			DISPLAY_CONFIRMED_FRIENDS,
			DISPLAY_INCOMING_FRIEND_REQUESTS,
			DISPLAY_OUTGOING_FRIEND_REQUESTS,

			NUM_DISPLAY_MODES
		};

        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
        virtual void OnButtonCancel( void );
		virtual void OnButtonPressed( int Button );
        virtual void OnCursorUp();
        virtual void OnCursorDown();
		virtual void OnButtonOK();
        //\}

        /** \name Accessors */
        //\{
        ScreenStatus GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
		int GetSelectedPlayer() { return _selectedFriend; }
        //\}

		/** \name Mutators */
        //\{
        void SetFriendsList( OSIPlayerList &friendsList, DisplayMode displayMode );
        //\}

    private:
        /** \name Utility methods */
        //\{
        GuiTable* GetFriendsTable( void );
		void MoveCursor( int numRowsToMove );
		void AddFriendRowToTable( GuiTable* pTable, int row, OSIPlayer* myFriend, int realIndex );
        //\}

        ScreenStatus _status;
		DisplayMode _displayMode;

		int _selectedFriend;
};

#endif // SCREEN_FRIENDSLIST_H
