#ifndef SCREEN_CLANINVITE_H
#define SCREEN_CLANINVITE_H

#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenClanInvite
//=============================================================================

class ScreenClanInvite : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenClanInvite );

	public:
        enum ScreenStatus
        {
            RUNNING,
            INVITE_SELECTED,
			INVITE_ENTER_INVITEE_NAME,
			INVITE_INBOX,
			INVITE_SENT_INVITE_LIST,
            INVITE_SEND,
            BACK
        };

		/// The role the screen can be set to, each displays different options
		enum ScreenRole {
			INBOX_MODE,
			SENT_INVITE_LIST_MODE,
		};

        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
		virtual void OnButtonPressed( int Button );
		virtual void OnButtonOK( void );
        virtual void OnButtonCancel();
        virtual void OnCommand( int Command );
        virtual void OnCursorUp();
        virtual void OnCursorDown();
		virtual void OnKeyboardInput( const char* keyboardString );
		virtual void OnScreenRefresh();
        //\}

        /** \name Accessors */
        //\{
        ScreenStatus GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
		const OSIchar* const GetEnteredName() const { if( _clanInvitee[0] ) { return _clanInvitee; } else { return NULL; } }
		const OSIClanInvitation* const GetSelectedInvitation() const { return &_clanInvite; }
		ScreenRole GetScreenRole() const { return _screenRole; }
        //\}

		/** \name Mutators */
        //\{
        virtual void SetAccessLevel(const OSIClanRole &clanRole);
		void SetScreenRole( const ScreenRole& screenRole );
        //\}

    private:

        /** \name Utility methods */
        //\{
        GuiTable* GetInviteTable( void );
		bool IsFirstButtonSelected();
		bool IsButtonSelected();
		bool IsLastButtonSelected();
        //\}

        ScreenStatus _status;
		ScreenRole _screenRole;
		OSIClanRole _clanRole;

		OSIchar _clanInvitee[OSI_PLAYER_NAME_LENGTH ]; ///< Storage for sending an invitation
		OSIClanInvitation _clanInvite; ///< Storage for the highlighted clan invite
};

#endif // SCREEN_CLANINVITE_H
