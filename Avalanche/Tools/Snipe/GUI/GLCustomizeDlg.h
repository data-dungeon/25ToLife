#pragma once
#include "GLDialogWnd.h"
#include "GLTabControl.h"
#include "GLListControl.h"

class CGLToolBarsPane : public CGLTransparentControl
{
public:
   CGLListControl *m_pglToolBarList;

// GLControl overrides
   virtual void PostInit();
   virtual BOOL OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};

class CGLCommandListControl : public CGLListControl
{
public:
   CommandUI *m_pCurrentCommandUI;

            CGLCommandListControl();
   virtual ~CGLCommandListControl();

   BOOL Select( UINT nID );

// CGLListControl overrides
   virtual CGLListItem *AllocItem( CGLListControl *pListControl, LPARAM lParam );
   virtual BOOL Select( CGLListItem *pItem, UINT nCode=GLTC_CARET );
};

class CGLCommandListItem : public CGLListItem
{
public:
   class Info 
   {
   public:
      CommandUI *m_pCommandUI;

      Info( CommandUI *pCommandUI )
      {
         m_pCommandUI = pCommandUI;
      }
   };
private:
   UINT m_nIndex;
   Info m_info;

public:
   CGLCommandListItem( const Info *pInfo );

// Operations
   CGLCommandListControl *GetListControl() { return (CGLCommandListControl *)m_pParentControl; }
   UINT GetIndex() { return m_nIndex; }
   const Info *GetInfo() const { return &m_info; }

// CGLListItem overrides
   virtual void SetIndex( UINT nIndex );

// CGLControl overrides
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
};

class CGLCommandsPane : public CGLTransparentControl
{
public:
   CGLListControl        *m_pglCategoryList;
   CGLCommandListControl  m_glCommandList;
   CGLListControl        *m_pglShortcutsForCommandList;

            CGLCommandsPane();
   virtual ~CGLCommandsPane();

// Operations
   void FillCategoriesList();
   void FillCommandsList();
   void FillShortcutsList();
   void SelectCommand( UINT nID );

// GLControl overrides
   virtual void PostInit();
   virtual BOOL OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};

class CGLCustomizeDlg : public CGLDialogWnd
{
public:
   enum { TOOLBARS_PANE, COMMANDS_PANE };

   static GLImageList m_glImageListTabBorder;
   static GLImageList m_glImageListTabs;

   CGLTabControl      m_glTabControl;
     CGLToolBarsPane    m_glToolBarsPane;
     CGLCommandsPane    m_glCommandsPane;
   CGLButtonControl   m_glCloseBtn;

            CGLCustomizeDlg();
   virtual ~CGLCustomizeDlg();

// Operations
   BOOL Create();

   DECLARE_MESSAGE_MAP()
   afx_msg void OnClose();
   afx_msg void OnCommand( UINT nID );
};
