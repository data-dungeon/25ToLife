#pragma once

#include "resource.h"
#include "afxwin.h"

// ExportMSetOptionsDlg dialog

class ExportMSetOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(ExportMSetOptionsDlg)

   BOOL m_bBinary;
public:
            ExportMSetOptionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ExportMSetOptionsDlg();

// Dialog Data
	enum { IDD = IDD_EXPORTMSETOPTIONS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
};
