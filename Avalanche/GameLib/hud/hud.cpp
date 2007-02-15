/******************* includes ****************************/

#include "hud/HudPCH.h"
#include "hud/hud.h"

//=============================================================================
// Hud::Hud()
//=============================================================================

Hud::Hud()
{
	Reset();
}

//=============================================================================
// Hud::~Hud()
//=============================================================================

Hud::~Hud()
{
	// TODO: Add destruction code here...
}

//=============================================================================
// Hud::Initialize()
//=============================================================================

void Hud::Initialize( void )
{
	// TODO: Add initialization code here...
}

//=============================================================================
// DebugHud::DebugHud()
//=============================================================================

DebugHud::DebugHud()
{
	// TODO: Add construction code here...
}

//=============================================================================
// DebugHud::~DebugHud()
//=============================================================================

DebugHud::~DebugHud()
{
	// TODO: Add destruction code here...
}

//=============================================================================
// DebugHud::Initialize()
//=============================================================================

void DebugHud::Initialize( void )
{
	m_DebugMenu.LoadTextureSet( "menutex.dbl" );
	m_DebugMenu.InitMenu();

	m_FPS.SetPosition( 624.0f, 8.0f );
	m_FPS.SetOrigin( 1.0f, 0.0f );

	AddChild( &m_FPS );

	return;
}

//=============================================================================
// DebugHud::Update()
//=============================================================================

void DebugHud::Update()
{
	Hud::Update();

	m_DebugMenu.DrawMenu();

	int Performance = Sprite::GetSpritePerformanceStatistics();

	if( Performance > 0 )
	{
		m_Performance.SetText( Printf( "Sprite CPU Time: %d", Performance ) );
		m_Performance.SetTextHeight( 33.0f );
		m_Performance.SetOrigin( 0.0f, 1.0f );
		m_Performance.ScreenPosition( 0.05f, 0.95f );
		m_Performance.Update();
	}

	return;
}