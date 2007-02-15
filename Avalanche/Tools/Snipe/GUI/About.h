#pragma once

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

/////////////////////////////////////////////////////////////////////////////
// About.cpp : CAboutDlg dialog used for App About
//

class CAboutDlg : public CDialog
{
public:
   CAboutDlg();

   enum { IDD = IDD_ABOUTBOX };

protected:
   virtual BOOL OnInitDialog();
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
   DECLARE_MESSAGE_MAP()
};
