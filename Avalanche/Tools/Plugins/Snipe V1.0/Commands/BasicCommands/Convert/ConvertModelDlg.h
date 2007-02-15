#pragma once

#include "../resource.h"
#include "afxwin.h"
#include "HSnipeObject.h"

// ConvertModelDlg dialog

class ConvertModelDlg : public CDialog
{
	DECLARE_DYNAMIC(ConvertModelDlg)

public:
   CComboBox               m_wndToModelTypeCombo;
   int                     m_nSelection;
   SnipeObjectType         m_nFromModelType;
   SnipeObjectType         m_nToModelType;
   CArray<SnipeObjectType> m_snipeObjectArray;


            ConvertModelDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ConvertModelDlg();

// Dialog Data
	enum { IDD = IDD_CONVERTMODEL_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
   afx_msg void OnCbnSelchangeToModelType();
};
