#pragma once

#include "GLTreeControl.h"
#include "Interface/SelectionList.h"

class SnipeObject;
class CGLObjectTreeItem;

class CGLObjectTreeControl : public CGLTreeControl, public SnipeObject
{
   DECLARE_MESSAGE_MAP()
public:
   static GLImageList m_glImageListBackground;
   static GLImageList m_glImageListTreeIcons;

   SnipeObject *m_pObjectHead;

            CGLObjectTreeControl();
   virtual ~CGLObjectTreeControl();

// Operations
   void InsertObjectBranch( SnipeObject *pObject, CGLTreeItem *pParentItem=NULL );
   void SetObjectHead( SnipeObject *pObjectHead );
   SnipeObject *GetObjectHead() { return m_pObjectHead; }

   CGLObjectTreeItem *FindTreeItem( CGLObjectTreeItem *pParentItem, SnipeObject *pChildObject );
   CGLObjectTreeItem *FindTreeItem( SnipeObject *pObject );
   SnipeObject *GetCaretObject();

   afx_msg void OnUpdateOutputControl( CCmdUI *pCmnUI );

// CGLTreeControl overrides
   virtual CGLTreeItem *AllocItem( CGLTreeControl *pTreeControl, LPARAM lParam );

// CGLControl overrides
   virtual void PostInit();
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual String GetMatchName() const { return "CGLObjectTreeControl"; }
   virtual const char *GetClassName() const { return "CGLObjectTreeControl"; }
};

class CGLObjectTreeItem : public CGLTreeItem, public SnipeObject
{
public:
//          m_nID is a (HierObject *)
   UINT     m_bOdd : 1;

            CGLObjectTreeItem( CGLTreeControl *pTreeControl );
   virtual ~CGLObjectTreeItem();

   SnipeObject *GetObject() { return (SnipeObject *)m_nID; }
   CGLObjectTreeControl *GetTree() { return (CGLObjectTreeControl *)m_pParentControl; }

// CGLTreeItem overrides
   virtual BOOL IsSelected();
   virtual void SetSelected( BOOL bSelected );
   virtual int GetItemHeight();
   virtual void BeginDrag( CPoint point );
   virtual void DoDrag( CPoint point );
   virtual void EndDrag( CPoint point );

// CGLControl overrides
   virtual void PostInit();
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
   virtual BOOL GetContextMenu( const CPoint &ptControl, CMenu &menu );
   virtual BOOL OnLButtonDblClk( UINT nFlags, CPoint point );

// CCmdTarget overrides
   virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual String GetMatchName() const { return "CGLObjectTreeItem"; }
   virtual const char *GetClassName() const { return "CGLObjectTreeItem"; }
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
