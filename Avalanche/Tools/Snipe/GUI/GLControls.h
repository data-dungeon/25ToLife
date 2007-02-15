#pragma once

#include "SnipeObject.h"
#include "GLImageList.h"

// COLOR CONSTANTS
extern GLfloat colorWhite[3];
extern GLfloat colorBlack[3];
extern GLfloat colorGray[3];
extern GLfloat colorDkGray[3];
extern GLfloat colorLtGray[3];
extern GLfloat colorRed[3];
extern GLfloat colorGreen[3];
extern GLfloat colorBlue[3];
extern GLfloat colorLtYellow[3];
extern GLfloat colorYellow[3];
extern GLfloat colorOrange[3];
extern GLfloat colorLtRed[3];
extern GLfloat colorLtGreen[3];
extern GLfloat colorLtBlue[3];
extern GLfloat colorDkRed[3];
extern GLfloat colorDkGreen[3];
extern GLfloat colorDkBlue[3];

class CGLBaseView;
class CGLRenderWnd;
class String;
class GLDrawPrepper;
//class RGBFloat;

#define NUMSPLINESTEPS 15.0

#define WM_GLCOMBO_CHANGED   (WM_USER+0)
#define WM_GLBUTTON_CLICKED  (WM_USER+1)

#define GLC_FIRST  ((CGLControl *)NULL)
#define GLC_LAST   ((CGLControl *)(ULONG_PTR)-0x0FFFE)

/*!
CGLControl is the base class for all OpenGL based controls.
It derives off of SnipeObject only so it can get notification messages from
other SnipeObjects (like Properties changing) if need be.
*/
class CGLControl : public CCmdTarget
{
	DECLARE_DYNAMIC(CGLControl)
public:
   enum { FOCUSRINGWIDTH=4 };
   enum Justify { JUST_NOCHANGE, JUST_LEFT, JUST_RIGHT, JUST_CENTER };
   enum ActivationMsg { ACTIVATION_INACTIVE, ACTIVATION_ACTIVE };
   enum { DRAW_THISONLY=0, DRAW_CHILDREN=1<<1, DRAW_PARENT=1<<2 };

   static CGLControl *m_pCaptureControl;
   static CGLControl *m_pContextMenuControl;

   CGLRenderWnd  *m_pGLRenderWnd;
   CGLControl    *m_pParentControl;
   CArray<CGLControl *> m_childcontrolarray;
   CPoint         m_ptControl; // relative to parent control using justification
   CSize          m_szControl;
   Justify        m_justification;
   CPoint         m_ptMin; // a right/bottom justified control can't get past here
   UINT           m_nID;
   GLDrawPrepper *m_pPrepper;
	GLdouble       m_GLProjectionMatrix[16];

   ULONG          m_bVisible    : 1,
                  m_bEnabled    : 1,
                  m_bInvalid    : 1,
                  m_bAutoDelete : 1,
                  m_bOutsideScissorArea : 1;

            CGLControl();
   virtual ~CGLControl();

   void Init( CGLControl *pParent, CGLRenderWnd *pWnd=NULL, const CPoint &ptControl=CPoint(0,0), const CSize &szControl=CSize(0,0), UINT nID=-1, Justify justification=JUST_LEFT, CGLControl *pInsertAfter=GLC_LAST );

   CGLControl *GetParent() const { return m_pParentControl; }
   static CGLControl *GetCapture() { return m_pCaptureControl; }
   CGLControl *SetCapture();
   CRect GetViewportRect() const;
   void GetGLViewportInfo(GLint glViewportInfo[4]) const;
   void MakeRenderContextCurrent();
   void SetWindowsProjectionMatrix();
   BOOL MakeActive( BOOL bUpdateCmdUIs=FALSE );
   BOOL HitTest( const CPoint &pt );
   CGLControl *HitTestAll( const CPoint &pt );
   CSize GetTextExtent( const String &text );
   String GetAbbreviated(const String &strSrc) const;
   void ControlToClient( CPoint &pt ) const;
   void ControlToClient( CRect &rc ) const;
   void ClientToControl( CPoint &pt ) const;
   void ClientToControl( CRect &rc ) const;
   CGLControl *FindChild( UINT nID );
   int FindChild( CGLControl *pChild );
   BOOL IsDescendentOf( CGLControl *pParent ) const;
   void SetClip( CRect rcClip );

   BOOL IsVisible() const;
   BOOL HasVisibleChildControl();

   void SetEnabled( BOOL bEnabled );
   BOOL IsEnabled();
   void Invalidate();
   BOOL IsInvalid();
   void ValidateChildren();
   void DestroyChildren();
   CSize CreateControlsFromDialogTemplate( UINT nIDTemplate );

   void DrawGradient( RGBAFloat c[4], CRect &rect=CRect(0,0,0,0));
   void DrawBorder();
   void DrawFocusRing();
   void DrawShadow();
   float DrawText( float x, float y, const String &text, float scalex=1.0f );
   void DrawText( float x, float y, float z, const String &text, float scalex=1.0f );
   void DrawSpline( Vector2 p[4], int nNumSteps=NUMSPLINESTEPS );
   void Update();

// Overridables
   virt_base virtual BOOL IsListItem() { return FALSE; }
   virt_base virtual BOOL IsTreeItem() { return FALSE; }
   virt_base virtual void PostInit();
   virt_base virtual BOOL IsThisVisible() const { return m_bVisible; } // Strictly this control.  If a parent is hidden, it won't be reflected here.  IsVisible() does that.
   virt_base virtual BOOL SetVisible( BOOL bVisible );
   virt_base virtual void OnActivate( ActivationMsg msg, CGLControl *pOther );
   virt_base virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
   virt_base virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
   virt_base virtual UINT GetContextMenuID( const CPoint &ptControl, UINT &nDefaultItemID );
   virt_base virtual BOOL GetContextMenu( const CPoint &ptControl, CMenu &menu );
   virt_base virtual BOOL OnLButtonDown( UINT nFlags, CPoint point );
   virt_base virtual BOOL OnLButtonDblClk( UINT nFlags, CPoint point ) { return OnLButtonDown( nFlags, point ); }
   virt_base virtual BOOL OnLButtonUp( UINT nFlags, CPoint point ) { return FALSE; }
   virt_base virtual BOOL OnMButtonDown( UINT nFlags, CPoint point ) { return FALSE; }
   virt_base virtual BOOL OnMButtonDblClk( UINT nFlags, CPoint point ) { return OnMButtonDown( nFlags, point ); }
   virt_base virtual BOOL OnMButtonUp( UINT nFlags, CPoint point ) { return FALSE; }
   virt_base virtual BOOL OnRButtonDown( UINT nFlags, CPoint point ) { return FALSE; }
   virt_base virtual BOOL OnRButtonDblClk( UINT nFlags, CPoint point ) { return OnRButtonDown( nFlags, point ); }
   virt_base virtual BOOL OnRButtonUp( UINT nFlags, CPoint point ) { return FALSE; }
   virt_base virtual BOOL OnMouseMove( UINT nFlags, CPoint point ) { return FALSE; }
   virt_base virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
   virt_base virtual void OnMouseEnter();
   virt_base virtual void OnMouseLeave();
   virt_base virtual BOOL OnSetCursor( UINT message, CPoint point );
   virt_base virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) { return FALSE; }
   virt_base virtual BOOL OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) { return FALSE; }
   virt_base virtual BOOL OnContextMenu( CPoint ptScreen );
   virt_base virtual BOOL OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virt_base virtual CSize GetControlSize() { return m_szControl; } // may include scrollbars
   virt_base virtual UINT GetToolTipText( CPoint point, String &strText ) const;
   virt_base virtual void SetText( const String &strText ) {}
   virt_base virtual void SetProjectionMatrix();
   virt_base virtual void OnDragEnter( CPoint point, CGLControl *pDragControl );
   virt_base virtual void OnDragMove( CPoint point, CGLControl *pDragControl ) {}
   virt_base virtual void OnDragLeave( CPoint point, CGLControl *pDragControl ) {}
   virt_base virtual void OnDragDrop( CPoint point, CGLControl *pDragControl ) {}
   virt_base virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virt_base virtual catch_msg BOOL Load( BaseStream &bs );
   virt_base virtual catch_msg BOOL ParseData( BaseStream &bs, const DataHeader &dataheader ) { return FALSE; }
   virt_base virtual catch_msg SnipeObject *GetObject( const String &strClass ) { return NULL; }
   virt_base virtual void OnSplit( CGLControl *pFirstCell ) {} // gives new control a chance to copy some attributes from the first cell

// CObject overrides
#ifdef _DEBUG
   virtual void Dump( CDumpContext &dc ) const;
#endif
};

class GLDrawPrepper 
{
public:
	GLdouble  m_GLProjectionMatrix[16];
	GLdouble  m_GLModelViewMatrix[16];
   GLfloat   m_Scissor[4];
   GLint     m_Viewport[4];
   GLboolean m_bDepthTest;
   GLboolean m_bLighting;

    GLDrawPrepper( CGLControl *pControl, BOOL bResetProjection=TRUE );
   ~GLDrawPrepper();
};

class NotifySwapChild 
{
public:
   CGLControl *m_pOldChild;
   CGLControl *m_pNewChild;

   NotifySwapChild( CGLControl *pOldChild, CGLControl *pNewChild )
   {
      m_pOldChild = pOldChild;
      m_pNewChild = pNewChild;
   }
};

class CGLTransparentControl : public CGLControl
{
public:
// CGLControl overrides
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
};

class CGLStaticTextControl : public CGLControl
{
public:
   String m_strText;

// CGLControl overrides
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
   virtual void SetText( const String &strText ) { m_strText = strText; }
};

class CGLBorderedControl : public CGLControl
{
public:
// CGLControl overrides
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
};

class CGLHeaderControl : public CGLControl
{
	DECLARE_DYNAMIC(CGLHeaderControl)
public:
   class Column 
   {
   public:
      String m_strName;
      int    m_nWidth;

      Column() {}
      Column( const String &strName, int nWidth )
      {
         m_strName = strName;
         m_nWidth = nWidth;
      }
   };

   CArray<Column> m_columnarray;

            CGLHeaderControl();
   virtual ~CGLHeaderControl();

// Operations
   int AddColumn( const String &strName, int nWidth );

// CGLControl overrides
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
};

class CGLSliderControl : public CGLControl
{
private:
   static GLImageList m_glImageListSliderBar;
   static GLImageList m_glImageListSliderKnob;

   int  m_nPos;
   int  m_nMin;
   int  m_nMax;

   union {
      ULONG    m_nCGLSliderControlFlags;
      struct {
         ULONG m_bScrubbing : 1,
               m_bScrubbed  : 1;
      };
   };

public:
            CGLSliderControl( int nPos=0, int nMin=0, int nMax=100 );
   virtual ~CGLSliderControl();

// Operations
   int GetPos() { return m_nPos; }
   int GetMin() { return m_nMin; }
   int GetMax() { return m_nMax; }

// Overridables
   virt_base virtual int SetPos( int nPos );
   virt_base virtual int SetMin( int nMin );
   virt_base virtual int SetMax( int nMax );
   virt_base virtual void OnEndScrub() {};

// CGLControl overrides
   virtual void PostInit();
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
   virtual BOOL OnLButtonDown( UINT nFlags, CPoint point );
   virtual BOOL OnMouseMove( UINT nFlags, CPoint point );
   virtual BOOL OnLButtonUp( UINT nFlags, CPoint point );
};

class CGLColorControl : public CGLControl
{
private:
   static GLImageList m_glImageListColorChip;
   enum { MARGIN=2 };

   RGBFloat m_rgb;
   UINT     m_bPressed : 1;

public:
            CGLColorControl( RGBFloat rgb );
   virtual ~CGLColorControl();

// Operations
   RGBFloat GetColor() const { return m_rgb; }

// Overridables
   virt_base virtual RGBFloat SetColor( const RGBFloat &rgb );
   virt_base virtual void FillColorChip( const CRect &rcFill );

// CGLControl overrides
   virtual void PostInit();
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
   virtual BOOL OnLButtonDown( UINT nFlags, CPoint point );
   virtual BOOL OnMouseMove( UINT nFlags, CPoint point );
   virtual BOOL OnLButtonUp( UINT nFlags, CPoint point );
};

class CGLImageControl : public CGLControl
{
protected:
   GLImageList m_glImageListBackground;
   BOOL        m_bTiled;

public:
            CGLImageControl();
   virtual ~CGLImageControl();

// Operations
   void LoadBitmap( UINT nBitmapID );

// CGLControl overrides
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
};

class CGLControlCmdUI : public CCmdUI
{
public:
   CGLControlCmdUI()
   {
   }
   CGLControlCmdUI( CGLControl *pControl )
   {
	   m_pOther = (CWnd *)pControl;
      m_nID = pControl->m_nID;
   }

// CCmdUI overrides
   virtual void Enable( BOOL bOn )
   {
      CGLControl *pControl = (CGLControl *)m_pOther;
      pControl->SetEnabled( bOn );
	   m_bEnableChanged = TRUE;
   }
   virtual void SetCheck( int nCheck ) {}
   virtual void SetText( LPCTSTR lpszText )
   {
      CGLControl *pControl = (CGLControl *)m_pOther;
      pControl->SetText( lpszText );
      m_bEnableChanged = TRUE;
   }
};

class CGLControlHeader : public HeaderBase
{
public:
   CGLControl *m_pControl;

    CGLControlHeader() { m_pControl = NULL; }
    CGLControlHeader( CGLControl *pControl, BOOL bWriteHeader, BaseStream &bs );
   ~CGLControlHeader();

   static BOOL IsHeader( MemoryStream &ms );
};

BaseStream &operator >> (BaseStream &bs, CGLControlHeader &value);
