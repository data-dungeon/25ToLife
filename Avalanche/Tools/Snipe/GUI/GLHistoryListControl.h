#pragma once

#include "GLListControl.h"

class CommandList;
class SnipeCommand;
class HierObject;

class CGLHistoryListControl : public CGLListControl
{
   DECLARE_MESSAGE_MAP()
public:
   SnipeObject *m_pCurrentObject;

   CGLHistoryListControl();

   void SetCommandList( SnipeObject *pObject );

   afx_msg void OnUpdateOutputControl( CCmdUI *pCmnUI );

// CGLListControl overrides
   virtual CGLListItem *AllocItem( CGLListControl *pListControl, LPARAM lParam );
   virtual BOOL Select( CGLListItem *pItem, UINT nCode=GLTC_CARET );

// CGLControl overrides
   virtual void PostInit();

// CCmdTarget overrides
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
};

class CGLHistoryListItem : public CGLListItem
{
public:
   class Info 
   {
   public:
      SnipeCommand *m_pCommand;

      Info( SnipeCommand *pCommand )
      {
         m_pCommand = pCommand;
      }
   };
private:
   UINT m_nIndex;
   Info m_info;

public:
   CGLHistoryListItem( const Info *pInfo );

// Operations
   CGLHistoryListControl *GetListControl() { return (CGLHistoryListControl *)m_pParentControl; }
   UINT GetIndex() { return m_nIndex; }
   const Info *GetInfo() const { return &m_info; }

// CGLListItem overrides
   virtual void SetIndex( UINT nIndex );

// CGLControl overrides
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
};

class CHistoryCmdUI : public CGLControlCmdUI
{
public:
   CHistoryCmdUI ( CGLControl *pControl ) :
      CGLControlCmdUI ( pControl ) {}

   void SetCommandObject( SnipeObject *pObject );

// CCmdUI overrides
   virtual void Enable(BOOL bOn) {}
};

