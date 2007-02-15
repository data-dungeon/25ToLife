#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "hud/sprite.h"

class Screen : public Sprite
{
	public:
		Screen( void );
		virtual ~Screen();
		
	public:
		static Screen *m_pScreenList;
		Screen *m_pNextScreen;
};

#endif // __SCREEN_H__