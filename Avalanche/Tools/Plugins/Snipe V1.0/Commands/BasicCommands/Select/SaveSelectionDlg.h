#pragma once

#include "../resource.h"
#include "afxwin.h"

// CSaveSelectionDlg dialog

class CSaveSelectionDlg : public CDialog
{
	DECLARE_DYNAMIC(CSaveSelectionDlg)

public:
   CComboBox m_wndSelectionCombo;
   int       m_nSelection;
   CString   m_strName;
   int       m_nOperation;

            CSaveSelectionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSaveSelectionDlg();

// Dialog Data
	enum { IDD = IDD_SAVESELECTION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
   afx_msg void OnCbnSelchangeSelectionCombo();
};
