#pragma once

#include "resource.h"
#include "afxwin.h"

// ExportTupMotionOptionsDlg dialog

class ExportTupMotionOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(ExportTupMotionOptionsDlg)

   BOOL m_bBinary;
public:
            ExportTupMotionOptionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ExportTupMotionOptionsDlg();

// Dialog Data
	enum { IDD = IDD_EXPORTTUPMOTIONOPTIONS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
};
