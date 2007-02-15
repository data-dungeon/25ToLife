#pragma once
#include "GLControls.h"

class CGLButtonControl : public CGLControl
{
	DECLARE_DYNAMIC(CGLButtonControl)
public:
   static GLImageList m_glImageListButtonDefault;

   GLImageList *m_pglImageListButton;
   GLImageList  m_glImageListIcon;
   String       m_strText;
   UINT         m_bCheckBox      : 1,
                m_bChecked       : 1,
                m_bPressed       : 1,
                m_bHasBackground : 1,
                m_bDeleteButtonImageList : 1;

            CGLButtonControl( BOOL bCheckBox=FALSE, BOOL bChecked=FALSE );
   virtual ~CGLButtonControl();

   BOOL GetChecked() { return m_bChecked; }
   void SetPressed( BOOL bPressed );
   BOOL GetPressed() { return m_bPressed; }
   void SetButtonImageList( GLImageList *pglImageList ) { m_pglImageListButton = pglImageList; }
   BOOL LoadButtonImageList( UINT nBitmapID, const CRect &rcDice, int cx=0, HINSTANCE hInstance=NULL );

// Overridables
   virt_base virtual BOOL SetChecked( BOOL bChecked );

// CGLControl overrides
   virtual void PostInit();
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
   virtual BOOL OnLButtonDown( UINT nFlags, CPoint point );
   virtual void SetText( const String &strText ) { m_strText = strText; }
};

class CGLButtonCmdUI : public CGLControlCmdUI
{
public: // re-implementations only
   virtual void SetCheck(int nCheck)
   {
      ASSERT(nCheck >= 0 && nCheck <= 2); // 0=>off, 1=>on, 2=>indeterminate
      CGLButtonControl *pButton = (CGLButtonControl*)m_pOther;
      ASSERT(pButton != NULL);
      ASSERT_KINDOF(CGLButtonControl, pButton);

      pButton->SetChecked( nCheck );
   }
};
