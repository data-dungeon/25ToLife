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

#if (!defined __EXT_GROUPBOX_H)
#define __EXT_GROUPBOX_H

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CExtGroupBox window

class __PROF_UIS_API CExtGroupBox : public CButton
{
// Construction
public:
	DECLARE_DYNAMIC( CExtGroupBox );
	CExtGroupBox();

// Attributes
public:

// Operations
public:

	enum e_style_t
	{
		STYLE_ETCHED	= 0,
		STYLE_ROUNDED	= 1,
		STYLE_FLAT		= 2,
		STYLE_CAPTION	= 3,
	};
	enum e_text_align_t
	{
		ALIGN_LEFT		= 0,
		ALIGN_RIGHT		= 1,
		ALIGN_CENTER	= 2,
	};

	void SetStyle( e_style_t eStyle )
	{
		ASSERT_VALID( this );
		m_eStyle = eStyle;
		Invalidate();
	}
	e_style_t GetStyle() const
	{
		ASSERT_VALID( this );
		return m_eStyle;
	}

	void SetTextAlign( e_text_align_t eTextAlign )
	{
		ASSERT_VALID( this );
		m_eTextAlign = eTextAlign;
		Invalidate();
	}
	e_text_align_t GetTextAlign() const
	{
		ASSERT_VALID( this );
		return m_eTextAlign;
	}

	void SetCaptionHeaderHeight( int nHeight )
	{
		ASSERT_VALID( this );
		m_nCaptionHeaderHeight = nHeight;
		Invalidate();
	}
	int GetCaptionHeaderHeight() const
	{
		ASSERT_VALID( this );
		return m_nCaptionHeaderHeight;
	}
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtGroupBox)
	protected:
	virtual void PreSubclassWindow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtGroupBox();

protected:
	e_text_align_t m_eTextAlign;
	e_style_t m_eStyle;
	int m_nCaptionHeaderHeight;
	
	virtual void OnDrawLabelText(
		CDC & dc,
		CRect& rcText,
		__EXT_MFC_SAFE_LPCTSTR strText,
		DWORD dwDrawTextFlags,
		bool bEnabled
		);

	virtual void OnDrawFrameRect(
		CDC & dc,
		const CRect & rcClient
		);
		
	// Generated message map functions
protected:
	//{{AFX_MSG(CExtGroupBox)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __EXT_GROUPBOX_H
