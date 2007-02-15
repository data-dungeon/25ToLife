#include "pch.h"
#include "projectdlg.h"

ProjectSettings::ProjectSettings( CWnd* pParentWnd ) : CPropertySheet( "Project Settings", pParentWnd, 0 )
{
	AddPage( &m_ProjectPaths );
	AddPage( &m_Preferences );
	AddPage( &m_ScreenSettings );
}