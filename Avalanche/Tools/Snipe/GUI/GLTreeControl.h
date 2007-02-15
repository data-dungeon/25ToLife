#pragma once

#include "GLScrollBarControl.h"

class CGLTreeItem;

#define GLTI_FIRST  ((CGLTreeItem *)NULL)
#define GLTI_LAST   ((CGLTreeItem *)(ULONG_PTR)-0x0FFFE)

class CGLTreeControl : public CGLScrollableControl
{
public:
   static GLImageList m_glImageListExpandIcons;

   CList<CGLTreeItem *>  m_selecteditemlist;
   CGLTreeItem          *m_pCaretItem;
   UINT                  m_bNeedsRecalcLayout : 1;

            CGLTreeControl();
   virtual ~CGLTreeControl();

// Operations
   CGLTreeItem *InsertItem( LPARAM lParam, CGLTreeItem *pParent=NULL, CGLTreeItem *pInsertAfter=GLTI_LAST );
   void RemoveItem( CGLTreeItem *pItem );
   void DeleteAllItems();
   void EnsureVisible( CGLTreeItem *pItem );

   enum { GLTC_CARET=0x01, GLTC_DROPHILITE=0x02, GLTC_APPENDTOSELECTION=0x04, GLTC_APPENDFROMCARET=0x08 };

// Overridables
   virt_base virtual CGLTreeItem *AllocItem( CGLTreeControl *pTreeControl, LPARAM lParam );
   virt_base virtual BOOL Select( CGLTreeItem *pItem, UINT nCode=GLTC_CARET );
   virt_base virtual BOOL Deselect( CGLTreeItem *pItem );
   virt_base virtual void DeselectAll();

// CGLControl overrides
   virtual void PostInit();
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
   virtual BOOL OnLButtonDown( UINT nFlags, CPoint point );

// CCmdTarget overrides
   virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
};

class CGLTreeItem : public CGLControl
{
private:
// To conserve space, lParam is stored in m_nID
   CGLTreeControl *m_pTreeControl;
   union {
      ULONG    m_nTIFlags;
      struct {
         ULONG m_bSelected : 1,
               m_bExpanded : 1,
               m_bDragging : 1,
               m_bHidden : 1;
      };
   };

public:
   enum { INDENT=10, HEIGHT=16 };

            CGLTreeItem( CGLTreeControl *pTreeControl );
   virtual ~CGLTreeItem();

// Operations
   CGLTreeControl *GetTreeControl() { return m_pTreeControl; }
   void Expand( BOOL bExpand=TRUE );
   BOOL IsExpanded() { return m_bExpanded; }
   BOOL IsHidden() { return m_bHidden; }
   void SetHidden( BOOL bHidden ) { m_bHidden = bHidden; SetVisible(!bHidden);  }

// Overridables
   virt_base virtual BOOL IsSelected() { return m_bSelected; }
   virt_base virtual void SetSelected( BOOL bSelected );
   virt_base virtual int GetItemHeight() { return HEIGHT; }
   virt_base virtual int GetExpandedMargin() { return 0; }
   virt_base virtual BOOL IsExpandable() { return m_childcontrolarray.GetSize(); }
   virt_base virtual void BeginDrag( CPoint point );
   virt_base virtual void DoDrag( CPoint point );
   virt_base virtual void EndDrag( CPoint point );


// CGLControl overrides
   virtual BOOL SetVisible( BOOL bVisible );
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
   virtual BOOL OnLButtonDown( UINT nFlags, CPoint point );
   virtual BOOL OnMButtonDown( UINT nFlags, CPoint point );
   virtual BOOL OnRButtonDown( UINT nFlags, CPoint point );
   virtual BOOL OnLButtonUp( UINT nFlags, CPoint point );
   virtual BOOL OnMButtonUp( UINT nFlags, CPoint point );
   virtual BOOL OnMouseMove( UINT nFlags, CPoint point );
   virtual BOOL IsTreeItem() { return TRUE; }
};
