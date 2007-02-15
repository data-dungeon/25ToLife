#pragma once
#include "GLDialogWnd.h"

class CGLStringDlg : public CGLDialogWnd
{
public:
   String   m_strTitle;
   String   m_strRequest;
   String   m_strResult;

            CGLStringDlg( const String &strTitle, const String &strRequest, const String &strInitial );
   virtual ~CGLStringDlg();

// Operations
   DECLARE_MESSAGE_MAP()
   afx_msg void OnOK();
   afx_msg void OnCancel();

// CGLDialogWnd overrides
   virtual BOOL Create(); // called from DoModal()
};

BOOL StringRequest( const String &strTitle, const String &strRequest, String &strResult );
