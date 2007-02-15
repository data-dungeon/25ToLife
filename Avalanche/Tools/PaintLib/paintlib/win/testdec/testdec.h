/*
/--------------------------------------------------------------------
|
|      $Id: testdec.h,v 1.3 2000/01/10 23:53:03 Ulrich von Zadow Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
--------------------------------------------------------------------
*/

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"    // main symbols

// CDibtestApp:
// See dibtest.cpp for the implementation of this class
//

class CDibtestApp : public CWinApp
{
public:
  CDibtestApp();

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CDibtestApp)
public:
  virtual BOOL InitInstance();
  //}}AFX_VIRTUAL

  // Implementation

  //{{AFX_MSG(CDibtestApp)
  // NOTE - the ClassWizard will add and remove member functions here.
  //    DO NOT EDIT what you see in these blocks of generated code !
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


/*
/--------------------------------------------------------------------
|
|      $Log: /Avalanche/tools/PaintLib/paintlib/win/testdec/testdec.h $
 * 
 * 1     4/11/02 10:37a Adam Clayton
|      Revision 1.3  2000/01/10 23:53:03  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|
--------------------------------------------------------------------
*/
