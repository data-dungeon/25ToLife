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

#if (!defined __EXT_MDI_WINDOWS_LIST_DIALOG_H)
#define __EXT_MDI_WINDOWS_LIST_DIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if (!defined __EXT_MFC_NO_MDI_WINDOWS_LIST_DIALOG)

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __EXT_RESIZABLE_DIALOG_H)
	#include <ExtResizableDialog.h>
#endif

#if (!defined __EXT_LABEL_H)
	#include <ExtLabel.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtMdiWindowsListDlg dialog

class __PROF_UIS_API CExtMdiWindowsListDlg : public CExtResizableDialog
{
	void _Init( HWND hMDIFrameWnd );
// Construction
public:
	CExtMdiWindowsListDlg(
		CMDIFrameWnd * pMDIFrameWnd,
		CWnd * pParent = NULL
		);
	CExtMdiWindowsListDlg(
		HWND hMDIFrameWnd,
		CWnd * pParent = NULL
		);

	inline static HWND stat_FindMdiClientHWND(
		HWND hWndMdiFrame
		)
	{
		for(	HWND hWnd = ::GetWindow( hWndMdiFrame, GW_CHILD );
				hWnd != NULL;
				hWnd = ::GetWindow( hWnd, GW_HWNDNEXT )
			)
		{
			ASSERT( ::IsWindow( hWnd ) );
			TCHAR strClassName[ 512 ];
			::memset( strClassName, 0, sizeof(strClassName) );
			::GetClassName( hWnd, strClassName, 512 );
			if( _tcslen( strClassName ) == 0 )
				continue;
			_tcslwr( strClassName );
			//if( _tcscmp( strClassName, _T("mdiclient") ) == 0 )
			if( _tcsstr( strClassName, _T("mdiclient") ) != NULL )
				return hWnd;
		}
		return NULL;
	}

// Dialog Data
	//{{AFX_DATA(CExtMdiWindowsListDlg)
	CExtButton	m_btnOk;
	CExtButton	m_btnTileVert;
	CExtButton	m_btnTileHorz;
	CExtButton	m_btnSave;
	CExtButton	m_btnMinimize;
	CExtButton	m_btnCloseWindows;
	CExtButton	m_btnCascade;
	CExtButton	m_btnActivate;
	CExtLabel	m_labelListWindows;
	//}}AFX_DATA
 	CExtWFF< CListBox >	m_ListWindows;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtMdiWindowsListDlg)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void _SelectMDIActive();
	void _FillMDIWindowList();
	virtual void OnUpdateButtons();
	HICON m_hIcon;
	HWND m_hMDIFrameWnd;	
	HWND m_hWndMDIClient;
	
	// Generated message map functions
	//{{AFX_MSG(CExtMdiWindowsListDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMIS);
	afx_msg int  OnCompareItem(int nIDCtl, LPCOMPAREITEMSTRUCT lpCIS);
	afx_msg void OnSelchangeListboxWindows();
	afx_msg void OnDblclkListboxWindows();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // (!defined __EXT_MFC_NO_MDI_WINDOWS_LIST_DIALOG)

#endif // (!defined __EXT_MDI_WINDOWS_LIST_DIALOG_H)
