#ifndef _MAINTOOLBAR_H_
#define _MAINTOOLBAR_H_

#include <afxwin.h>
#include <afxext.h>
#include "maintoolbar.h"

class MainToolBar : public CToolBar
{
public:
	BOOL CreateFontList( CFont *pGuiFont );
	TCHAR* GetCurrentFont( void );
	CComboBox m_FontList;
	TCHAR m_szFontName[512];

protected:
	afx_msg void OnFontChange( void );

protected:
	DECLARE_MESSAGE_MAP();
};

#endif // _MAINTOOLBAR_H_