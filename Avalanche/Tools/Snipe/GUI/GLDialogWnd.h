#pragma once
#include "GLRenderWnd.h"
#include "GLTitleBarControl.h"

class CGLDialogWnd : public CGLRenderWnd
{
protected:
   enum { TITLEBARHEIGHT=22 };

   CGLTitleBarControl m_glTitleBar;
   CGLImageControl    m_glContentControl;

public:
            CGLDialogWnd();
   virtual ~CGLDialogWnd();

// Operations
   BOOL Create( CWnd *pParent, LPCTSTR lpszWindowName=NULL, const CRect &rcDlg=CRect(0,0,0,0) );
   CGLControl *GetContentControl() { return &m_glContentControl; }
   void InsertControl( CGLControl *pControl, const CRect &rcControl, UINT nID=-1 );

   DECLARE_MESSAGE_MAP()
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnSize(UINT nType, int cx, int cy);

public:
// Overridables
	virt_base virtual INT_PTR DoModal();
   virt_base virtual BOOL Create(); // called from DoModal()
};
