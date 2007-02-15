#pragma once
#include "GLControls.h"

class CGLScrollBarControl : public CGLControl
{
private:
   static GLImageList m_glImageListBackground;
   static GLImageList m_glImageListKnob;

   CGLControl *m_pOwnerControl;
   int         m_nPos;
   int         m_nLine;
   int         m_nPage;
   int         m_nMin;
   int         m_nMax;
   UINT        m_bUpPressed   : 1,
               m_bDownPressed : 1,
               m_bKnobPressed : 1,
               m_bAutoHide    : 1;

public:
            CGLScrollBarControl();
   virtual ~CGLScrollBarControl();

// Operations
   CGLControl *GetOwnerControl() { return m_pOwnerControl; }
   int GetPos() const { return m_nPos; }
   int GetLine() const { return m_nLine; }
   int GetPage() const { return m_nPage; }
   int GetMin() const { return m_nMin; }
   int GetMax() const { return m_nMax; }

   void SetOwnerControl( CGLControl *pOwnerControl ) { m_pOwnerControl = pOwnerControl; m_pGLRenderWnd = pOwnerControl->m_pGLRenderWnd; }
   void SetLine( int nLine ) { m_nLine = nLine; }
   void SetPage( int nPage );
   void SetMin( int nMin );
   void SetMax( int nMax );

   CRect GetUpRect() const;
   CRect GetDownRect() const;
   CRect GetWellRect() const;
   CRect GetKnobRect() const;

// Overridables
   virt_base virtual void SetPos( int nPos );

// CGLControl overrides
   virtual void PostInit();
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
   virtual BOOL OnLButtonDown( UINT nFlags, CPoint point );
   virtual BOOL OnLButtonUp( UINT nFlags, CPoint point );
   virtual BOOL OnMouseMove( UINT nFlags, CPoint point );
};

class CGLScrollableControl : public CGLControl
{
protected:
   CGLScrollBarControl m_glHScroll;
   CGLScrollBarControl m_glVScroll;
   UINT                m_bPanning : 1;

public:
            CGLScrollableControl();
   virtual ~CGLScrollableControl();

// CGLControl overrides
   virtual void PostInit();
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
   virtual CSize GetControlSize();
   virtual BOOL OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual BOOL OnSetCursor( UINT message, CPoint point );
   virtual BOOL OnMButtonDown( UINT nFlags, CPoint point );
   virtual BOOL OnMButtonUp( UINT nFlags, CPoint point );
   virtual BOOL OnMouseMove( UINT nFlags, CPoint point );
   virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
