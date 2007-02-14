#ifndef SCREEN_CLANMAIN_H
#define SCREEN_CLANMAIN_H

#include "frontend/texteffects.h"
#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenClanMain
//=============================================================================

class ScreenClanMain : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenClanMain );

	public:
        enum ScreenStatus
        {
            RUNNING,
#ifdef _XBOX
            FEEDBACK,
#endif
			MEMBERS,
            CREATE,
            INVITATIONS,
            LEAVE,
			VIEW_NOTICES,
            POST_NOTICE,
			NOTICE_POSTED,
            BACK
        };

        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
        virtual void OnButtonCancel() { _status = BACK; }
        virtual void OnCommand( int Command );
		virtual void OnDialogCommand( int Command );
		virtual void OnKeyboardInput( const char* keyboardStr );
        //\}

        /** \name Accessors */
        //\{
        ScreenStatus GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
		const OSIchar* const getEnteredClanNotice() const { if( _clanNotice[0] ) { return _clanNotice; } else { return NULL; } }
        //\}

		/** \name Mutators */
        //\{
		void SetNotices(OSIClanNoticeList *noticeList);
		void SetAccessLevel(const OSIClanRole &clanRole);
		//\}

		void ClearScreen();

		enum DialogCommands { COMMAND_DIALOG_OK }; // added for "voice banned" messages on xbox

private:
        ScreenStatus _status;

		OSIchar _clanNotice[ OSI_STRING_LENGTH_LONG ];
};

#endif // SCREEN_CLANMAIN_H
