// GLRenderWnd.h : interface of the CGLRenderWnd class
//

#pragma once

#include "GLBaseView.h"

class CGLRenderWnd : public CWnd
{
// Attributes
private:
   CArray<CGLControl *> m_childcontrolarray;
   CList<CGLControl *>  m_invalidcontrollist;
public:
	static HGLRC         m_glrcShared;
   static HGLRC         m_glrcCurrent;

   HGLRC                m_glrc;
   CGLControl          *m_pActiveControl;
   CGLControl          *m_pMouseOverControl;
   UINT                 m_bDrawingInvalidControls : 1;

	         CGLRenderWnd();
	virtual ~CGLRenderWnd();

// Operations
public:
   BOOL Create( CWnd *pParent );
   int AddControl( CGLControl *pControl );
   BOOL RemoveControl( CGLControl *pControl );
   BOOL RemoveControlAt( int nIndex );
   CGLControl *HitTestControls( const CPoint &point ) const;
   CGLControl *GetActiveControl() { return m_pActiveControl; } //!< @returns The currently active control
   CGLControl *SetActiveControl( CGLControl *pControl ); //!< @returns The previously active control
   void MakeRenderContextCurrent();
   void Update();

   void AddInvalidControl( CGLControl *pControl );
   void RemoveInvalidControl( CGLControl *pControl );
   void DrawInvalidControls();
   BOOL HasInvalidControls() { return !m_invalidcontrollist.IsEmpty(); }

// Generated message map functions
   afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
   afx_msg void OnTimer(UINT nIDEvent);
   afx_msg void OnSetFocus(CWnd* pOldWnd);
   afx_msg void OnKillFocus(CWnd* pNewWnd);
   afx_msg void OnEditPaste();

// CWnd overrides
public:
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
   virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
#ifdef _DEBUG
   virtual void Dump(CDumpContext& dc) const;
#endif
   afx_msg void OnMove(int x, int y);
   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};

class GLContextPusher
{
public:
	HGLRC m_hOldGLRC;
	HDC m_hOldHDC;

    GLContextPusher( CGLRenderWnd *glwnd);
   ~GLContextPusher();
};

void InitGLExtensions();
