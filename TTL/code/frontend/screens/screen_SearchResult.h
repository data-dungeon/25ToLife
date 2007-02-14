#ifndef SCREEN_SEARCHRESULT_H
#define SCREEN_SEARCHRESULT_H

#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenSearchResult
//=============================================================================

class ScreenSearchResult : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenSearchResult );

	public:
		ScreenSearchResult( void );
		virtual ~ScreenSearchResult();

		void SetSessionParam( int ParamId, const char* pParamText );
		void SetTimeLimit( const char* pTimeLimit );
		void SetTimeLimit( int Minutes, int Seconds );
		void SetScoreLimit( const char* pScoreLimit );
		void SetScoreLimit( int ScoreLimit );
		void ClearSearchResults( void );

	protected:
		virtual void OnInitialize( void );
		virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnCursorLeft( void );
		virtual void OnCursorRight( void );
		virtual void OnCursorUpdate( void );
		virtual void OnButtonOK( void );
		virtual void OnButtonCancel( void );
		virtual void OnButtonPressed( int Button );
		virtual void OnScreenUpdate( void );
		virtual void UpdateSessionInformation( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenExit( void );

		enum
		{
			UPDATE,
			END_LAUNCH,
			END_CANCEL
		} updateStatus;

		bool m_reFresh;
};

#endif // SCREEN_SEARCHRESULT_H
