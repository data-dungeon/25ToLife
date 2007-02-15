#pragma once

#include "resource.h"
#include "afxwin.h"

// ImportTupSceneOptionsDlg dialog

class ImportTupSceneOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(ImportTupSceneOptionsDlg)

public:
            ImportTupSceneOptionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ImportTupSceneOptionsDlg();

// Dialog Data
	enum { IDD = IDD_IMPORTTUPSCENEOPTIONS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
};
