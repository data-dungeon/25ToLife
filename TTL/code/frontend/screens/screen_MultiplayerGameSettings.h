#ifndef _SCREEN_MP_GAMESETTINGS_H_
#define _SCREEN_MP_GAMESETTINGS_H_

#include "frontend/screens/screen_BaseClass.h"
#include "frontend/UseCase/ParamList.hpp"

class ScreenMultiplayerGameSettings : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenMultiplayerGameSettings );

	public:
        ScreenMultiplayerGameSettings();
        ~ScreenMultiplayerGameSettings();
        void AddConfigObject(ParamType *pValue) { _values.push_back(pValue); }
        void ClearConfig() { _values.clear(); }
		void SetToAny();

	protected:
        GuiListBox* GetListBox( void );
		  void ShowCursorIcon( int n, bool Show );
		  

		virtual void OnInitialize( void );
		virtual void OnPrevScreen( Screen *pPrevScreen );
		virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnCursorLeft( void );
		virtual void OnCursorRight( void );
		virtual void OnButtonOK( void );
		virtual void OnButtonCancel( void );
		virtual void OnButtonCycle( int N );
		virtual void OnButtonPressed( int Button );
		virtual void OnScreenUpdate( void );
		virtual void OnScreenIntro( void );

		virtual void RebuildOptionList( void );

//#ifdef DIRECTX_PC
//		virtual void OnCommand(int Command);
//#endif
		
		bool m_InitMenu;

        ParamList _values;
};

#endif // _SCREEN_MP_GAMESETTINGS_H_
