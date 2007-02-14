#ifndef SCREEN_CREATEPROFILE_H
#define SCREEN_CREATEPROFILE_H

#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenCreateProfile
//=============================================================================

enum	{
	CREATEPROFILE_IDLE,
	CREATEPROFILE_SAVE,
	CREATEPROFILE_SAVE_RESULT,
};

class ScreenCreateProfile : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenCreateProfile );

	public:
		ScreenCreateProfile( void );
		virtual ~ScreenCreateProfile();

	protected:
		virtual void OnScreenIntro( void );
		virtual void OnKeyboardInput( const char* pString );
		virtual void OnKeyboardCancel( void );
		virtual void OnScreenRefresh();
		virtual void OnSaveGameResult( int Result );

		int	m_MemCardState;
		char	m_Profile[64];
};

#endif // SCREEN_ONLINEPROFILE_H