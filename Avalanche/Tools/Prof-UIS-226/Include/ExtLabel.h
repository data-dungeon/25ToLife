// This is part of the Professional User Interface Suite library.
// Copyright (C) 2001-2004 FOSS Software, Inc.
// All rights reserved.
//
// http://www.prof-uis.com
// http://www.fossware.com
// mailto:support@fossware.com
//
// This source code can be used, modified and redistributed
// under the terms of the license agreement that is included
// in the Professional User Interface Suite package.
//
// Warranties and Disclaimers:
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND
// INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
// IN NO EVENT WILL FOSS SOFTWARE INC. BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES,
// INCLUDING DAMAGES FOR LOSS OF PROFITS, LOSS OR INACCURACY OF DATA,
// INCURRED BY ANY PERSON FROM SUCH PERSON'S USAGE OF THIS SOFTWARE
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

#if (!defined __EXT_LABEL_H)
#define __EXT_LABEL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

/////////////////////////////////////////////////////////////////////////////
// CExtLabel window

class __PROF_UIS_API CExtLabel : public CStatic
{
public:
	DECLARE_DYNCREATE( CExtLabel );
	CExtLabel();

// Attributes
public:
	
// Operations
public:
	void SetFontBold( bool bSet = true )
	{
		ASSERT_VALID( this );
		m_bFontBold = bSet;
		Invalidate();
	}
	void SetFontItalic( bool bSet = true )
	{
		ASSERT_VALID( this );
		m_bFontItalic = bSet;
		Invalidate();
	}
	void SetFontUnderline( bool bSet = true )
	{
		ASSERT_VALID( this );
		m_bFontUnderline = bSet;
		Invalidate();
	}
	void SetFontStrikeOut( bool bSet = true )
	{
		ASSERT_VALID( this );
		m_bFontStrikeOut = bSet;
		Invalidate();
	}
	
	bool GetFontBold()
	{
		ASSERT_VALID( this );
		return m_bFontBold;
	}
	bool GetFontItalic()
	{
		ASSERT_VALID( this );
		return m_bFontItalic;
	}
	bool GetFontUnderline()
	{
		ASSERT_VALID( this );
		return m_bFontUnderline;
	}
	bool GetFontStrikeOut()
	{
		ASSERT_VALID( this );
		return m_bFontStrikeOut;
	}
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtLabel)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtLabel();
	virtual void OnDrawLabelText(
		CDC & dc,
		const RECT & rcText,
		__EXT_MFC_SAFE_LPCTSTR strText,
		DWORD dwDrawTextFlags,
		bool bEnabled
		);

protected:
	bool m_bFontBold:1;
	bool m_bFontItalic:1;
	bool m_bFontUnderline:1;
	bool m_bFontStrikeOut:1;

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtLabel)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // (!defined __EXT_LABEL_H)
