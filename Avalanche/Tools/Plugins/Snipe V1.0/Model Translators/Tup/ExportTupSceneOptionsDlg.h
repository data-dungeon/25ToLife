#pragma once

#include "resource.h"
#include "afxwin.h"

// ExportTupSceneOptionsDlg dialog

class ExportTupSceneOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(ExportTupSceneOptionsDlg)

public:
   BOOL m_bBinary;

            ExportTupSceneOptionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ExportTupSceneOptionsDlg();

// Dialog Data
	enum { IDD = IDD_EXPORTTUPSCENEOPTIONS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
};
