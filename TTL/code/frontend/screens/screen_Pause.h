#ifndef _SCREEN_PAUSE_H_
#define _SCREEN_PAUSE_H_

#include "frontend/screens/screen_BaseClass.h"

class ScreenPause : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenPause );

	typedef enum
	{
		OBJECTIVE_INCOMPLETE,
		OBJECTIVE_COMPLETED,
		OBJECTIVE_FAILED,
	}
	OBJECTIVE_STATE;

	public:
		void SetPrimaryObjective( int n, const char* pObjective );
		void SetSecondaryObjective( int n, const char* pObjective, const char* pProgress );
		void SetPrimaryObjectiveState( int n, OBJECTIVE_STATE State );
		void SetSecondaryObjectiveState( int n, OBJECTIVE_STATE State );
		void SetObjectiveState( Text* pObjective, OBJECTIVE_STATE State );
		int GetPrimaryId( int n );
		int GetSecondaryId( int n );
		int GetProgressId( int n );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenExit( void );
		virtual void OnScreenUpdate();
		virtual void OnButtonStart( void );
		virtual void OnCommand( int Command );
#ifdef DIRECTX_PC
		virtual void OnButtonPressed( int button );
#endif
		virtual float GetScreenExitTime( void ) { return 0.2f; }
		void OnCommandContinue( void );
		void OnCommandFriends( void );
		void OnCommandQuit( void );
		void OnDialogCommand( int Command );
};

class ScreenPauseOnline : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenPauseOnline );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenExit( void );
		virtual void OnCommand( int Command );
		virtual void OnButtonStart( void );
		virtual void OnButtonPressed( int Button );
#ifdef DIRECTX_PC
		virtual void OnButtonCancel();
#endif

		void OnCommandContinue( void );
		void OnCommandFriends( void );
		void OnCommandPlayers( void );
		void OnCommandQuit( void );
		void OnDialogCommand( int Command );
};

class ScreenQuitGame : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenQuitGame );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenExit( void );
		virtual float GetScreenIntroTime( void ) { return 0.35f; }
		virtual bool IsScreenReadyToExit( void );

		uint32 m_TimeToQuit;
};

#endif // _SCREEN_PAUSE_H_
