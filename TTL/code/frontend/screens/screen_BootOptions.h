#ifndef _SCREEN_BOOTOPTIONS_H_
#define _SCREEN_BOOTOPTIONS_H_

#include "frontend/screens/screen_BaseClass.h"

class ScreenBootOptions : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenBootOptions );

	protected:
		virtual void OnInitialize( void );
};

#endif // _SCREEN_BOOTOPTIONS_H_