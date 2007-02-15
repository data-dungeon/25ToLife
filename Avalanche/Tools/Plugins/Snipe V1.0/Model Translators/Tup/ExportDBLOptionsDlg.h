#pragma once

#include "resource.h"
#include "afxwin.h"

// ExportDBLOptionsDlg dialog

class ExportDBLOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(ExportDBLOptionsDlg)

public:
   CString  m_strFilterChain;
   CString  m_strProcessFilename;

            ExportDBLOptionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ExportDBLOptionsDlg();

// Dialog Data
	enum { IDD = IDD_EXPORTDBLOPTIONS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   afx_msg void OnBrowse();
   afx_msg void OnBrowse2();

	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
};
