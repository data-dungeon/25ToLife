#ifndef _PROJECTDLG_H_
#define _PROJECTDLG_H_

#include <afxwin.h>
#include <afxdlgs.h>
#include "projectpaths.h"
#include "preferences.h"
#include "screensettings.h"

class ProjectSettings : public CPropertySheet
{
	public:
		ProjectSettings( CWnd* pParentWnd = 0 );

	public:
		ProjectPaths m_ProjectPaths;
		Preferences m_Preferences;
		ScreenSettings m_ScreenSettings;
};

#endif // _PROJECTDLG_H_