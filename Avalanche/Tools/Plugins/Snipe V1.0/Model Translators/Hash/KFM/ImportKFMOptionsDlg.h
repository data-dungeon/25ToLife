#pragma once

#include "..\resource.h"
#include "afxwin.h"

// ImportTupSceneOptionsDlg dialog

class ImportKFMOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(ImportKFMOptionsDlg)

public:
   BOOL m_bReduceKeys;
   float m_fTolerance;

            ImportKFMOptionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ImportKFMOptionsDlg();

// Dialog Data
	enum { IDD = IDD_IMPORTKFMOPTIONS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
};
