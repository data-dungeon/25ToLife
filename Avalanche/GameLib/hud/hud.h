#ifndef HUD_H
#define HUD_H

#include "hud/sprite.h"
#include "hud/text.h"
#include "hud/newmenu.h"

//=============================================================================
// class Hud : Please do not stuff game specific content into the Hud.  This is
//				 : intended to be a base-class with minimum features.
//=============================================================================

class Hud : public Sprite
{
	DECLARE_CLASSOBJECT( Hud );

	public:
		Hud();
		virtual ~Hud();
		virtual void Initialize( void );

	public:
};

//=============================================================================
// class DebugHud : Please do not stuff game specific content into the DebugHud.
//					   : This is intended to be a base-class with minimum features.
//						: The DebugHud should be removed for shipping versions.
//=============================================================================

class DebugHud : public Hud
{
	DECLARE_CLASSOBJECT( DebugHud );

	public:
		DebugHud();
		virtual ~DebugHud();
		virtual void Initialize( void );
		virtual void Update( void );

	public:
		Font				m_Font;
		Text				m_FPS;
		DebugMenu		m_DebugMenu;
		Text				m_Performance;
};

#endif // HUD_H
