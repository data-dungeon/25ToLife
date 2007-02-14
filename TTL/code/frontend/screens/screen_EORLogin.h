#ifndef SCREEN_EORLOGIN_H
#define SCREEN_EORLOGIN_H

#include "frontend/screens/screen_BaseClass.h"
#include "frontend/savegame.h"

//=============================================================================
// ScreenEORLogin
//=============================================================================

class ScreenEORLogin : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenEORLogin );

	public:
        enum ScreenStatus
        {
            RUNNING,
            LOGIN,
            CREATE_ACCOUNT,
            EDIT_ACCOUNT,
            RECOVER_ACCOUNT,
            DELETE_ACT,
			PASSWORD_ENTERED,
            BACK
        };

        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
		virtual void OnScreenUpdate( void );
        virtual void OnCursorLeft( void );
        virtual void OnCursorRight( void );
        virtual void OnButtonCancel() { _status = BACK; }
        virtual void OnCommand( int Command );
		virtual void OnKeyboardInput(const char* keyboardString);
#ifdef DIRECTX_PC
		virtual void OnButtonOK();
#endif
        //\}

        /** \name Accessors */
        //\{
        ScreenStatus GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
        ts_OnlineAccount *GetCurrentProfile();
		int GetCurrentProfileOffset();
        //\}

        /** \name Mutators */
        //\{
        void SelectProfile(int Offset);
        void AddProfile(ts_OnlineAccount *pAccount, int AccountNum);
        void Clear();
        void UpdateAccounts();
		void Reset();
        //\}

    private:
        /** \name Utility methods */
        //\{
        void _UpdateScreen();
		void _VerifyOnlineAccounts();
        //\}

        ScreenStatus _status;
		int _numAccounts;
};


#endif // SCREEN_EORLOGIN_H
