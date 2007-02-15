#pragma once

#include "GLButtonControl.h"
#include "../Utility/AString.h"

class CGLTabControl : public CGLControl
{
	DECLARE_DYNAMIC(CGLTabControl)
   DECLARE_MESSAGE_MAP()
public:
   class Tab 
   {
   public:
      String m_name;
      CRect  m_rcTab;
      LPARAM m_lParam;

      Tab() {}
      Tab( const String &name, LPARAM lParam );

      CGLControl *GetControlHead() { return (CGLControl *)m_lParam; } // only valid if m_bAutoControlSwap
      BOOL HitTest( const CPoint &pt );
   };

   GLImageList     *m_pglImageListBorder;
   GLImageList     *m_pglImageListTabs;
   int              m_nTabIndent;
   int              m_nTabSpacing;
   int              m_nTabTextMargin;
   UINT             m_bCenter          : 1;

protected:
   enum { TAB_INDENT=5, TAB_TEXTMARGIN=4, TAB_SPACING=-3 };

   static GLImageList m_glDefaultImageListBorderTop;
   static GLImageList m_glDefaultImageListBorderBottom;
   static GLImageList m_glDefaultImageListTabsTop;
   static GLImageList m_glDefaultImageListTabsBottom;

   CGLButtonControl m_glDocListButton;
   CGLButtonControl m_glCloseButton;
   CArray<Tab>      m_tabarray;
   int              m_nSelectedTab;
   UINT             m_bRemoving        : 1,
                    m_bTop             : 1,
                    m_bAutoControlSwap : 1, // Tab->lParam is control head pointer
                    m_bRemovableTabs   : 1;

public:
            CGLTabControl( BOOL bTop, BOOL bAutoControlSwap=FALSE, BOOL bRemovableTabs=TRUE );
   virtual ~CGLTabControl();

   int AddTab( const String &strName, LPARAM lParam );
   int AddTab( Tab &tab );
   void RemoveTab( int nIndex );
   int GetTabCount() const;
   LPARAM GetItemData( int nIndex ) const;
   void SetItemData( int nIndex, LPARAM lParam );
   Tab *HitTestTabs( const CPoint &pt );
   void DrawTab( int nIndex );
   int FindTab( Tab *pTab );
   int FindTab( LPARAM lParam );
   void SelectTab( int nIndex );
   CRect GetInsideRect();

   afx_msg void OnSelectTab( UINT nID );
   afx_msg void OnTabControlClose();
   afx_msg void OnUpdateTabControlCloseAllButThis(CCmdUI *pCmdUI);
   afx_msg void OnTabControlCloseAllButThis();

// Overridables
   virt_base virtual UINT GetTabMenuID();

// CGLControl overrides
   virtual void PostInit();
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
   virtual BOOL OnLButtonDown( UINT nFlags, CPoint point );
   virtual BOOL OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual UINT GetContextMenuID( const CPoint &point, UINT &nDefaultItemID );
};
