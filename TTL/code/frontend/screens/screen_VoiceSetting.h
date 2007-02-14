#ifndef _SCREEN_VOICESETTING_H_
#define _SCREEN_VOICESETTING_H_

#include "frontend/screens/screen_BaseClass.h"

class ScreenVoiceSetting : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenVoiceSetting );

	public:
		ScreenVoiceSetting( void );

		void ChangeVoiceOutput( int n );
		void AdjustOutputVolume( int n );
		bool GetVoiceThruSpeakers( void );
		void UpdateVoiceChatDevice( void );

		void GetProfileVoiceSettings( void );
		void SetProfileVoiceSettings( void );

		ts_VoiceParams m_VoiceParams;

	protected:
		virtual void OnInitialize( void );
		virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnCursorLeft( void );
		virtual void OnCursorRight( void );
		virtual void OnCursorIncrement( int N );
		virtual void OnButtonCancel( void );
		virtual void OnButtonPressed( int Button );
		virtual void OnCommand( int Command );
		virtual void OnCommandDefaultSetting( void );
		virtual void OnCommandDone( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenUpdate( void );
};

#endif // _SCREEN_VOICESETTING_H_