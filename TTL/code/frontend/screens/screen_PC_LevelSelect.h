#ifndef SCREEN_PCLEVELSELECT_H
#define SCREEN_PCLEVELSELECT_H

#include "frontend/screens/screen_BaseClass.h"


//=============================================================================
// ScreenPCLevelSelect
//=============================================================================

class ScreenPCLevelSelect : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenPCLevelSelect );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenExit( void ) { ScreenBaseClass::OnScreenExit(); }
		void OnCursorUpdate( void );
		virtual void OnButtonOK( void );
		virtual void OnCursorDown( void );
		virtual void OnCursorUp( void );
		virtual void OnCursorLeft( void );
		virtual void OnCursorRight( void );
		virtual void OnButtonPressed( int Button );
		virtual GuiListBox* GetListBox( void ) { return m_pListBox; }
		void RepopulateCheckpointListBox( int level );
		virtual void OnScreenUpdate();

		void StartCurrentMap();

		GuiCycleButton* GetCycleButton();
		GuiListBox* m_pListBox;
};



#endif // SCREEN_PCLEVELSELECT_H
