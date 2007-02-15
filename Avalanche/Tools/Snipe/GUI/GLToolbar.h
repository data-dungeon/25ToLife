#pragma once

#include "GLTabControl.h"

class CommandUI;

class CGLToolBarControl : public CGLControl
{
private:
   class Button {
   public:
      CommandUI *m_pCommandUI;
      UINT       m_bChecked : 1,
                 m_bPressed : 1,
                 m_bEnabled : 1;

      Button() {}
      Button( CommandUI *pCommandUI )
      {
         m_pCommandUI = pCommandUI;
         m_bChecked = FALSE;
         m_bPressed = FALSE;
         m_bEnabled = FALSE;
      }
   };

   enum { SEPARATORWIDTH=4 };
   static GLImageList m_glImageListButton;

   String         m_strName;
   CArray<Button> m_buttonarray;

public:
            CGLToolBarControl();
            CGLToolBarControl( const String &strName );
   virtual ~CGLToolBarControl();

// Operations
   const String &GetName() const { return m_strName; }
   int GetButtonCount() const { return m_buttonarray.GetCount(); }
   Button *GetButton( int nIndex ) { return &m_buttonarray[ nIndex ]; }
   UINT GetButtonToolTipText( int nIndex, String &strText ) const;
   BOOL IsButtonPressed( int nIndex ) const { return m_buttonarray[ nIndex ].m_bPressed; }
   void CheckButton( int nIndex, BOOL bChecked );
   BOOL PressButton( int nIndex, BOOL bPressed );
   void EnableButton( int nIndex, BOOL bEnabled );
   BOOL CreateFromResource( UINT nIDResource, UINT nIDTitle );
   void SetName( const String &strName ) { m_strName = strName; }
   int AddButton( CommandUI *pCommandUI=NULL, int nPos=-1 );
   int HitTestButtons( CPoint point ) const;
   int GetMinWidth() const;

// CGLControl overrides
   virtual void PostInit();
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
   virtual BOOL OnLButtonDown( UINT nFlags, CPoint point );
   virtual UINT GetToolTipText( CPoint point, String &strText ) const;
   virtual void OnMouseEnter();
   virtual void OnMouseLeave();

// CCmdTarget overrides
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
};

class CGLToolBarShelf : public CGLTabControl
{
public:
            CGLToolBarShelf();
   virtual ~CGLToolBarShelf();

// Operations
   int GetToolBarCount() const { return GetTabCount(); }
   CGLToolBarControl *GetToolBarAt( int nIndex ) const { return (CGLToolBarControl *)GetItemData( nIndex ); }
   CGLToolBarControl *FindToolBar( const String &strName ) const;
   CGLToolBarControl *AddToolBar( const String &strName );

// CCmdTarget overrides
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
};
