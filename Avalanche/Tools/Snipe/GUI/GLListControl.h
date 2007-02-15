#pragma once

#include "GLScrollBarControl.h"

class CGLListItem;

#define GLLI_FIRST  ((CGLListItem *)NULL)
#define GLLI_LAST   ((CGLListItem *)(ULONG_PTR)-0x0FFFE)

class CGLComboControl;

class CGLListControl : public CGLScrollableControl
{
public:
   enum { GLTC_CARET=0x01, GLTC_DROPHILITE=0x02, GLTC_APPENDTOSELECTION=0x04, GLTC_APPENDFROMCARET=0x08 };

   GLImageList           m_glImageListIcons;
   CList<CGLListItem *>  m_selecteditemlist;
   CGLListItem          *m_pCaretItem;
   CGLListItem          *m_pDropTarget;
   CGLComboControl      *m_pComboControl; // only if this list is for a combo
   UINT                  m_bNeedsRecalcLayout : 1;

            CGLListControl();
   virtual ~CGLListControl();

// Operations
   BOOL SetIconBitmap( UINT nBitmapID, UINT nIconWidth=16 );
   CGLListItem *InsertItem( LPARAM lParam, CGLListItem *pInsertAfter=GLLI_LAST );
   void RemoveItem( CGLListItem *pItem );
   void DeleteAllItems();
   BOOL Deselect( CGLListItem *pItem );
   void DeselectAll();
   void EnsureVisible( CGLListItem *pItem );
   int GetCurSel() const;

// Overridables
   virt_base virtual CGLListItem *AllocItem( CGLListControl *pListControl, LPARAM lParam );
   virt_base virtual BOOL Select( CGLListItem *pItem, UINT nCode=GLTC_CARET );

// CGLControl overrides
   virtual void PostInit();
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
   virtual BOOL OnLButtonDown( UINT nFlags, CPoint point );
   virtual BOOL OnMButtonUp( UINT nFlags, CPoint point );
   virtual BOOL OnRButtonUp( UINT nFlags, CPoint point );
   virtual BOOL OnMouseMove( UINT nFlags, CPoint point );
   virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   virtual BOOL OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam );

// CCmdTarget overrides
   virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
};

class CGLListItem : public CGLControl
{
private:
// To conserve space, lParam is stored in m_nID
   ULONG m_bSelected    : 1,
         m_bDropHilited : 1;

public:
   enum { MARGIN=2, HEIGHT=16 };

            CGLListItem();
   virtual ~CGLListItem();

// Operations
   CGLListControl *GetListControl() { return (CGLListControl *)m_pParentControl; }
   BOOL IsSelected() { return m_bSelected; }
   BOOL IsDropHilited() { return m_bDropHilited; }

// Overridables
   virt_base virtual void SetSelected( BOOL bSelected );
   virt_base virtual void SetDropHilited( BOOL bDropHilited );
   virt_base virtual int GetItemHeight() { return HEIGHT; }
   virt_base virtual void SetIndex( UINT nIndex ) {}

// CGLControl overrides
   virtual BOOL IsListItem() { return TRUE; }
   virtual BOOL OnLButtonDown( UINT nFlags, CPoint point );
   virtual BOOL OnLButtonUp( UINT nFlags, CPoint point );
   virtual BOOL OnMButtonDown( UINT nFlags, CPoint point );
   virtual BOOL OnMButtonUp( UINT nFlags, CPoint point );
   virtual BOOL OnRButtonDown( UINT nFlags, CPoint point );
   virtual BOOL OnMouseMove( UINT nFlags, CPoint point );
};

class CGLBasicListItem : public CGLListItem
{
public:
   class Info 
   {
   public:
      String m_strName;
      UINT   m_nIconIndex;

      Info()
      {
         m_nIconIndex = -1;
      }
      Info( const String &strName, UINT nIconIndex=-1 )
      {
         m_strName = strName;
         m_nIconIndex = nIconIndex; 
      }
   };
private:
   Info m_info;

public:
   CGLBasicListItem() {}
   CGLBasicListItem( CGLBasicListItem::Info *pInfo );

// Operations
   Info *GetInfo() { return &m_info; }

// CGLControl overrides
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
};