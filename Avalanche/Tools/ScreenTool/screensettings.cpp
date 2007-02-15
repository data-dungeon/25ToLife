#include "pch.h"
#include "screensettings.h"

ScreenSettings::ScreenSettings( void ) : CPropertyPage( IDD_SCREENSETTINGS )
{
	m_ScreenWidth = 640;
	m_ScreenHeight = 480;
	m_SafeZoneX = 5;
	m_SafeZoneY = 5;
	m_DrawBorder = TRUE;
	m_DrawSafeZone = TRUE;
}

BOOL ScreenSettings::OnInitDialog( void )
{
	SetDlgItemInt( IDC_SCREENWIDTH, m_ScreenWidth );
	SetDlgItemInt( IDC_SCREENHEIGHT, m_ScreenHeight );
	SetDlgItemInt( IDC_SAFEZONEX, m_SafeZoneX );
	SetDlgItemInt( IDC_SAFEZONEY, m_SafeZoneY );
	CheckDlgItem( this, IDC_SCREENBORDER, m_DrawBorder );
	CheckDlgItem( this, IDC_SAFEZONEBORDER, m_DrawSafeZone );

	return CPropertyPage::OnInitDialog();
}

void ScreenSettings::OnOK( void )
{
	m_ScreenWidth = GetDlgItemInt( IDC_SCREENWIDTH );
	m_ScreenHeight = GetDlgItemInt( IDC_SCREENHEIGHT );

	if( m_ScreenWidth < 1 )
		m_ScreenWidth = 1;

	if( m_ScreenHeight < 1 )
		m_ScreenHeight = 1;

	m_SafeZoneX = GetDlgItemInt( IDC_SAFEZONEX );
	m_SafeZoneY = GetDlgItemInt( IDC_SAFEZONEY );

	if( m_SafeZoneX < 0 )
		m_SafeZoneX = 0;

	if( m_SafeZoneX > 25 )
		m_SafeZoneX = 25;

	if( m_SafeZoneY < 0 )
		m_SafeZoneY = 0;

	if( m_SafeZoneY > 25 )
		m_SafeZoneY = 25;

	m_DrawBorder = IsDlgButtonChecked( IDC_SCREENBORDER );
	m_DrawSafeZone = IsDlgButtonChecked( IDC_SAFEZONEBORDER );

	return;
}

void ScreenSettings::OnCancel( void )
{
	CPropertyPage::OnCancel();
}