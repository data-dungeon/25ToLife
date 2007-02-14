#ifndef SCREEN_NETWORKCONFIG_H
#define SCREEN_NETWORKCONFIG_H

#include "frontend/screens/screen_BaseClass.h"

#ifdef SN_TARGET_PS2
#include <netz.h>
#include <platform/stack/ps2common/ps2networking.h>
#include <platform/stack/ps2common/ps2module.h>
#include <platform/stack/ps2common/ps2profilelist.h>
#endif

//=============================================================================
// ScreenNetworkConfig
//=============================================================================
#define MAX_CONFIGS 10

enum {
	NETWORKCONFIG_IDLE,
	NETWORKCONFIG_IDLE_RESULT,
	NETWORKCONFIG_LOAD,
	NETWORKCONFIG_LOAD_RESULT,
};

class ScreenNetworkConfig : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenNetworkConfig );

	public:
		ScreenNetworkConfig( void );
		virtual ~ScreenNetworkConfig();

		void SetNetworkField( int Id, const char* pString );
		void SetNetworkProduct( const char* pString );
		void SetNetworkVendor( const char* pString );
		void SetNetworkType( const char* pString );

		enum eStates
		{
			STATE_NONE,
			STATE_SHOW_DIALOG,
			STATE_CHOOSE_CONFIG,
			STATE_INITIATE_CONNECTION,
			STATE_BACKUP,
		};

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnButtonOK( void );
		virtual void OnButtonCancel( void );
		virtual void OnCommand( int Command );
		virtual void OnScreenUpdate( void );
		virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnDialogCommand( int Command );
		void MemCardState();

		GuiListBox* GetListBox( void );

	private:
		int m_numConfigs;
		bool m_profileEnabled[ MAX_CONFIGS ];
		int m_state;
		unsigned int m_timer;
		int	m_MemCardState;
		int m_LastResult;

		GuiListBox		*m_ListBox;
};

class ScreenNetworkConnect : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenNetworkConnect );

	public:
		static void MultiPlayerLAN( void );
		static void MultiPlayerOnline( void );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenUpdate( void );
		virtual void OnButtonCancel( void );
		virtual void OnButtonPressed( int Button );
		void ShowMessage( const char* pMessage );

		enum eState
		{
			STATE_NONE = 0,
			STATE_CONNECTING,
			STATE_CONNECTED,
			STATE_FAILED,
			STATE_DONE,
		};
		Animator m_LedAnim;
		int m_state;
		unsigned int m_time;

		bool m_Button1Pressed;
		bool m_Button2Pressed;
};

#endif // SCREEN_NETWORKCONFIG_H