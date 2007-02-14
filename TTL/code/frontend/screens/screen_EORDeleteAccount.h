#ifndef SCREEN_EORDELETEACCOUNT_H
#define SCREEN_EORDELETEACCOUNT_H

#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenEORDeleteAccount
//=============================================================================

class ScreenEORDeleteAccount : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenEORDeleteAccount );

	public:
        enum ScreenStatus
        {
            RUNNING,
            BACK,
            DONE
        };

        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
        virtual void OnButtonCancel() { _status = BACK; }
        virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnCursorUpdate( void );
#ifndef DIRECTX_PC
        virtual void OnButtonOK( void ) { _status = DONE; }
#else
		virtual void OnButtonOK( void );
#endif
        //\}

        /** \name Accessors */
        //\{
        ScreenStatus GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
        int GetOffset();
        //\}

        /** \name Mutators */
        //\{
        void ClearProfiles();
        void AddProfile(const char *pName, int Offset);
        //\}

    private:
        /** \name Utility methods */
        //\{
        GuiListBox* GetProfileListBox( void );
#ifdef DIRECTX_PC
		GuiListBox* GetListBox() { return GetProfileListBox(); }
#endif // DIRECTX_PC
        //\}

        ScreenStatus _status;
};

#endif // SCREEN_EORDELETEACCOUNT_H
