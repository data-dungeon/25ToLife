#pragma once

#include "../../resource.h"
#include "afxwin.h"

// CCreateSurfaceDlg dialog

class CCreateSurfaceDlg : public CDialog
{
	DECLARE_DYNAMIC(CCreateSurfaceDlg)

public:
   CComboBox m_wndSurfaceCombo;
   int       m_nSurface;
   CString   m_strName;
   int       m_nOperation;

            CCreateSurfaceDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCreateSurfaceDlg();

// Dialog Data
	enum { IDD = IDD_CREATESURFACE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
   afx_msg void OnCbnSelchangeSurfaceCombo();
};
