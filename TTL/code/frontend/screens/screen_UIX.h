#ifndef SCREEN_UIX_H
#define SCREEN_UIX_H

#include "frontend/screens/screen_BaseClass.h"
#include "OSI/XBox/OSIXBox.hpp"

//=============================================================================
// ScreenUIX
//=============================================================================

class ScreenUIX : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenUIX );

	public:
        virtual void OnScreenIntro( void );
        virtual void OnButtonCancel() { }

    private:
};


class ScreenUIXFriends : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenUIXFriends );

	protected:
		virtual void OnScreenIntro( void );
		virtual void OnScreenUpdate( void );
		virtual void OnScreenExit( void );
		virtual void OnButtonCancel( void );
};

class ScreenUIXPlayers : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenUIXPlayers );

	protected:
		virtual void OnScreenIntro( void );
		virtual void OnScreenUpdate( void );
		virtual void OnScreenExit( void );
		virtual void OnButtonCancel( void );
};

class ScreenUIXLogin : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenUIXLogin );

	protected:
		virtual void OnScreenIntro( void );
		virtual void OnScreenUpdate( void );
		virtual void OnScreenExit( void );
		virtual void OnButtonCancel( void );
};

#endif // SCREEN_UIX_H