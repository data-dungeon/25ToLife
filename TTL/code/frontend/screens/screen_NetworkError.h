
#ifndef SCREEN_NETWORKERROR_H
#define SCREEN_NETWORKERROR_H

#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenNetworkConfig
//=============================================================================

class ScreenNetworkError : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenNetworkError );

	enum eErrorCondition
	{
		ERROR_NONE = 0,
		ERROR_CONNECTION_FAILED,
	};

	public:
		virtual void OnScreenUpdate( void );
		static void SetNetworkError( eErrorCondition error ) { m_error = error; }

	protected:
		void SetErrorMessage( const char* pErrorMessage );
		virtual void OnInitialize( void );
		virtual void OnButtonOK( void );
		virtual void OnButtonCancel( void );

		static eErrorCondition m_error;
};

#endif