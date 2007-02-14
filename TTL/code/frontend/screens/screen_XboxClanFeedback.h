#ifndef SCREEN_FRIENDSLIST_H
#define SCREEN_FRIENDSLIST_H

#include "frontend/screens/screen_BaseClass.h"
#include "OSI/OSIClans.hpp"
#include "OSI/OSIPlayerList.hpp"

//=============================================================================
// ScreenXboxClanFeedback
//=============================================================================

class ScreenXboxClanFeedback : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenXboxClanFeedback );

	public:
        enum ScreenStatus
        {
            RUNNING,
            CLAN_SELECTED,
            BACK
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
		int GetSelectedClan();
        //\}

		/** \name Mutators */
        //\{
        void PopulateFromPlayerList( OSIPlayerList& playerList );
        //\}

    private:
        /** \name Utility methods */
        //\{
        GuiTable* GetClanTable( void );
		void MoveCursor( int numRowsToMove );
		void AddClanRowToTable( GuiTable* pTable, int row, OSIClanInfo* clan, int realIndex );
        //\}

        ScreenStatus _status;
		int _selectedClan;
};

#endif // SCREEN_FRIENDSLIST_H
