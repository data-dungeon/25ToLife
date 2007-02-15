// Snipe.h : main header file for the SNIPE application
//

#if !defined(AFX_SNIPE_H__43A4C0AC_0369_4063_BC19_7EBCB371EFD9__INCLUDED_)
#define AFX_SNIPE_H__43A4C0AC_0369_4063_BC19_7EBCB371EFD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "Properties/Property.h"
//#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSnipeApp:
// See Snipe.cpp for the implementation of this class
//

class Project;
class CSnipeDocTemplate;
class CGLRenderWnd;
class SnipeObject;
class CBaseDoc;
typedef CTypedPtrList<CObList, CBaseDoc *> CBaseDocList;

class CSnipeApp : public CWinApp, public SnipeObject
{
private:
   static TimeProperty::Info m_timeinfo;
   CSnipeDocTemplate    *m_pSceneDocTemplate;
   CSnipeDocTemplate    *m_pModelDocTemplate;
	CSnipeDocTemplate    *m_pImageDocTemplate;
	CSnipeDocTemplate    *m_pMotionDocTemplate;
   CSnipeDocTemplate    *m_pMotionSetDocTemplate;
   CBaseDocList          m_docList;          // open documents
   TimeProperty          m_time;
   union {
      ULONG m_nSnipeAppFlags;
      struct {
         ULONG m_bDisplayCommandInOutput : 1;
      };
   };
 
public:
   CList<CGLRenderWnd *> m_glRenderWndList;

            CSnipeApp();
   virtual ~CSnipeApp();

// Operations
   CSnipeDocTemplate *GetSceneDocTemplate() { return m_pSceneDocTemplate; }
   CSnipeDocTemplate *GetModelDocTemplate() { return m_pModelDocTemplate; }
   CSnipeDocTemplate *GetImageDocTemplate() { return m_pImageDocTemplate; }
   CSnipeDocTemplate *GetMotionDocTemplate() { return m_pMotionDocTemplate; }
   CSnipeDocTemplate *GetMotionSetDocTemplate() { return m_pMotionSetDocTemplate; }
   CBaseDocList &GetDocList() { return m_docList; }
   void InvalidateAllViews( SnipeObject *pObject );
   void DrawInvalidControls();
   Time GetTime() { return m_time.GetCurrentValue(); }
   void SetTime( const Time &time ) { m_time.StoreValue( time, 0, FALSE ); }
   const TimeProperty *GetTimeProperty() const { return &m_time; }

   BOOL IsDisplayCommandInOutput() const { return m_bDisplayCommandInOutput; }
   void SetDisplayCommandInOutput( BOOL bDisplay ) { m_bDisplayCommandInOutput = bDisplay; }

	//{{AFX_MSG(CSnipeApp)
   DECLARE_MESSAGE_MAP()
   afx_msg void OnToolsCustomize();
	afx_msg void OnAppAbout();
	afx_msg void OnWindowCloseAll();
   afx_msg void OnUpdateRecentFileMenu(CCmdUI* pCmdUI);
   //}}AFX_MSG

public:
// CWinApp overrides
   virtual BOOL InitInstance();
   virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);

// CWinThread overrides
   virtual BOOL IsIdleMessage(MSG* pMsg);  // checks for special messages
   virtual BOOL OnIdle(LONG lCount); // return TRUE if more idle processing

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual String GetMatchName() const { return "CSnipeApp"; }
   virtual const char *GetClassName() const { return "CSnipeApp"; }
};

#define COLOR_MASK	RGB(0x00,0x80,0x80)

AFX_INLINE BOOL CreateImageList(CImageList& il, UINT nID)
{
	if (!il.Create(16, 16, ILC_MASK|ILC_COLOR24, 0, 1))
		return FALSE;
	CBitmap bmp;
	VERIFY(bmp.LoadBitmap(nID));

	il.Add(&bmp, COLOR_MASK);

	return TRUE;	
}

CSnipeApp &GetApp();

BOOL ReadWindowPlacement(LPWINDOWPLACEMENT pwp);
void WriteWindowPlacement(LPWINDOWPLACEMENT pwp);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SNIPE_H__43A4C0AC_0369_4063_BC19_7EBCB371EFD9__INCLUDED_)
