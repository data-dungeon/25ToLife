//

#pragma once
#include "GLListControl.h"

class CGLComboControl : public CGLControl
{
   DECLARE_DYNAMIC(CGLComboControl)
public:
   class ComboEntry
   {
   public:
      String m_strText;
      LPARAM m_lParam;

      ComboEntry() {}
      ComboEntry( const String &strText, LPARAM lParam )
      {
         m_strText = strText;
         m_lParam = lParam;
      }
   };

   static GLImageList m_glImageListCombo;

   CArray<ComboEntry> m_entries;
   int                m_nIndex;
   CGLListControl    *m_pGLListControl;
   UINT               m_bPressed      : 1,
                      m_bWindowsLike  : 1,
                      m_bFlatLook     : 1;

            CGLComboControl();
   virtual ~CGLComboControl();

   void ResetContent( BOOL bNotify=TRUE );
   int AddString( const String &strText, LPARAM lParam=0L );
   int DeleteString( UINT nIndex );
   int GetCurSel() const;
   int GetCount() const { return m_entries.GetCount(); }
   int Find( LPARAM lParam );
   LPARAM GetItemData( UINT nIndex ) const;
   void SetWindowsLike( BOOL bWindowsLike=TRUE ) { m_bWindowsLike = bWindowsLike; }
   void SetListControl( CGLListControl *pListControl ) { m_pGLListControl = pListControl; }

// Overridables
   virt_base virtual CGLListControl *AllocListControl();
   virt_base virtual void GetText( String &strValue ) const;
   virt_base virtual void SetCurSel( int nIndex, BOOL bNotify=TRUE );
   virt_base virtual void OnAddItem( int nIndex ) {}
   virt_base virtual void OnDeleteItem( int nIndex ) {}

// CGLControl overrides
   virtual void PostInit();
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
   virtual BOOL OnLButtonDown( UINT nFlags, CPoint point );
   virtual BOOL OnLButtonUp( UINT nFlags, CPoint point );
   virtual BOOL OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};

class CGLComboListControl : public CGLListControl
{
public:
// CGLListControl overrides
   virtual CGLListItem *AllocItem( CGLListControl *pListControl, LPARAM lParam );
};

class CGLComboListItem : public CGLBasicListItem
{
private:
   UINT m_nIndex;
   const CGLComboControl::ComboEntry *m_pComboEntry;

public:
   CGLComboListItem( const CGLComboControl::ComboEntry *pComboEntry );

// Operations
   CGLComboListControl *GetListControl() { return (CGLComboListControl *)m_pParentControl; }
   UINT GetIndex() { return m_nIndex; }

// CGLListItem overrides
   virtual void SetIndex( UINT nIndex ) { m_nIndex = nIndex; }

// CGLControl overrides
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
};
