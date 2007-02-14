#ifndef SCREEN_CLANLIST_H
#define SCREEN_CLANLIST_H

#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenClanList
//=============================================================================

class ScreenClanList : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenClanList );

	public:
        enum ScreenStatus
        {
            RUNNING,
            MEMBER_SELECTED,
            BACK
        };

        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
        virtual void OnButtonCancel( void );
        virtual void OnCommand( int Command );
		virtual void OnButtonPressed( int Button );
        virtual void OnCursorUp();
        virtual void OnCursorDown();
		virtual void OnScreenRefresh();
		virtual void OnButtonOK();
        //\}

        /** \name Accessors */
        //\{
        ScreenStatus GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
		const OSIClanMembership* const GetSelectedMember() const { return _clanMember; }
        //\}

    private:
        /** \name Utility methods */
        //\{
        GuiTable* GetClanTable( void );
		void MoveCursor( int numRowsToMove );
        //\}

        ScreenStatus _status;

		const OSIClanMembership* _clanMember;
};

#endif // SCREEN_CLANLIST_H
