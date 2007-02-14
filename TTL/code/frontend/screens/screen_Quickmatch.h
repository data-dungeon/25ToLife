#ifndef SCREEN_QUICKMATCH_H
#define SCREEN_QUICKMATCH_H

#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenQuickmatch
//=============================================================================

class ScreenQuickmatch : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenQuickmatch );

	public:
        enum ScreenStatus
        {
            RUNNING,
            JOIN,
            NEXT,
            BACK
        };

        /** \name Accessors */
        //\{
        ScreenStatus GetStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
        //\}

        /** \name Mutators */
        //\{
        void SetMatchNum(int Match, int NumMatches);
        void SetGameType(const char *pGameType);
        void SetNumPlayers(int NumPlayers, int MaxPlayers);
        void SetHostName(const char *pHostName);
        void SetMapName(const char *pMapName);
        //\}

    private:
        /** \name Base class overrides */
        //\{
		virtual void OnInitialize(void);
        virtual void OnScreenIntro(void);
#ifndef DIRECTX_PC
        virtual void OnButtonOK(void) { _status = JOIN; }
#else
		virtual void OnCommand(int commandId);
		virtual void OnCusorLeft() { SelectPrevButton(); }
		virtual void OnCursorRight() { SelectNextButton(); }
#endif
        virtual void OnButtonCancel(void) { _status = BACK; }
        virtual void OnButtonPressed( int Button );
        //\}

        ScreenStatus _status;
};

#endif // SCREEN_QUICKMATCH_H
