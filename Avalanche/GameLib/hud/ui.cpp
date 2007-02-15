/******************* includes ****************************/

#include "hud/HudPCH.h"
#include "hud/ui.h"

UserInterface::UserInterface( void )
{
	m_TextureSet = INVALID_HANDLE;
}

UserInterface::~UserInterface()
{
	FreeTextureSet();
}

void UserInterface::Initialize( void )
{
}

bool UserInterface::LoadTextureSet( const char *pFileName )
{
	FreeTextureSet();

	m_TextureSet = dblLoad( pFileName );

	if( m_TextureSet == INVALID_HANDLE )
		return false;

	return true;
}

void UserInterface::FreeTextureSet( void )
{
	m_TextureSet = INVALID_HANDLE;
}

t_Handle UserInterface::TextureSet( void )
{
	return m_TextureSet;
}

void UserInterface::Update( void )
{
	if( Active() )
		ProcessControllerInput();

	return;
}

void UserInterface::ProcessControllerInput( void )
{
}

bool UserInterface::Active( void )
{
	return m_Active;
}

bool UserInterface::Inactive( void )
{
	return !m_Active;
}

void UserInterface::Activate( void )
{
	m_Active = true;

	OnActivate();
}

void UserInterface::Deactivate( void )
{
	m_Active = false;

	OnDeactivate();
}

void UserInterface::OnPadUp( void )
{
	OnCursorUp();
}

void UserInterface::OnPadDown( void )
{
	OnCursorDown();
}

void UserInterface::OnPadLeft( void )
{
	OnCursorLeft();
}

void UserInterface::OnPadRight( void )
{
	OnCursorRight();
}

void UserInterface::OnPadTriangle( void )
{
	OnButtonCancel();
}

void UserInterface::OnPadCircle( void )
{
	///OnButtonOK();
}

void UserInterface::OnPadSquare( void )
{
#ifdef GCN
	OnButtonCancel();
#endif
}

void UserInterface::OnPadX( void )
{
	OnButtonOK();
}

void UserInterface::OnPadStart( void )
{
	OnStartButton();
}

void UserInterface::OnPadSelect( void )
{
	OnSelectButton();
}

void UserInterface::OnCursorUp( void )
{
}

void UserInterface::OnCursorDown( void )
{
}

void UserInterface::OnCursorLeft( void )
{
}

void UserInterface::OnCursorRight( void )
{
}

void UserInterface::OnButtonOK( void )
{
}

void UserInterface::OnButtonCancel( void )
{
}

void UserInterface::OnStartButton( void )
{
}

void UserInterface::OnSelectButton( void )
{
}

void UserInterface::OnActivate( void )
{
}

void UserInterface::OnDeactivate( void )
{
}
