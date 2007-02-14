#include "TTLPCH.h"
#include "frontend/screens/screen_BasePopup.h"

//=============================================================================
// ScreenBasePopup::ScreenBasePopup - The default constructor
//=============================================================================

ScreenBasePopup::ScreenBasePopup( void )
{
	m_ResultCancel = PopupCancel;
	m_ResultOK     = PopupOK;
}

//=============================================================================
// ScreenBasePopup::~ScreenBasePopup - The default destructor
//=============================================================================

ScreenBasePopup::~ScreenBasePopup( void )
{
}

//=============================================================================
// ScreenBasePopup::SetPopupTitle
//=============================================================================

void ScreenBasePopup::SetPopupTitle( const char* pTitle )
{
	Text* pTitleText = GetPopupTitle();

	if( pTitleText )
		pTitleText->SetText( pTitle );

	return;
}

//=============================================================================
// ScreenBasePopup::SetPopupMessage
//=============================================================================

void ScreenBasePopup::SetPopupMessage( const char* pMessage )
{
	Text* pMessageText = GetPopupMessage();

	if( pMessageText )
		pMessageText->SetText( pMessage );

	return;
}

//=============================================================================
// ScreenBasePopup::SetHelpText
//=============================================================================

void ScreenBasePopup::SetHelpText( int N, int Icon, const char* pHelp )
{
	Text* pHelpText = GetHelpText( N );

	if( pHelpText )
		pHelpText->SetText( pHelp );

	return;
}

//=============================================================================
// ScreenBasePopup::GetPopupTitle
//=============================================================================

Text* ScreenBasePopup::GetPopupTitle( void )
{
	return 0;
}

//=============================================================================
// ScreenBasePopup::GetPopupMessage
//=============================================================================

Text* ScreenBasePopup::GetPopupMessage( void )
{
	return 0;
}

//=============================================================================
// ScreenBasePopup::GetHelpText
//=============================================================================

Text* ScreenBasePopup::GetHelpText( int N )
{
	return 0;
}

//=============================================================================
// ScreenBasePopup::OnButtonOK
//=============================================================================

void ScreenBasePopup::OnButtonOK( void )
{
	EndScreen( m_ResultOK );
}

//=============================================================================
// ScreenBasePopup::OnButtonCancel
//=============================================================================

void ScreenBasePopup::OnButtonCancel( void )
{
	EndScreen( m_ResultCancel );
}

#ifdef DIRECTX_PC
void ScreenBasePopup::OnScreenIntro()
{
	ScreenBaseClass::OnScreenIntro();
	QuitButton.Show(false);
	BackButton.Show(false);
}
#endif