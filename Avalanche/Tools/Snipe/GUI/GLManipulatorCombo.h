#pragma once

#include "GLComboControl.h"

class SnipeManipulator;
class SnipeManipulatorList;

class CGLManipulatorCombo : public CGLComboControl
{
public:
   SnipeManipulatorList *m_pSnipeManipulatorList;

   CGLManipulatorCombo();

// Operations
   void SetAvailableManipulators( SnipeManipulatorList *pSnipeManipulatorList );
   void SetManipulator( SnipeManipulator *pSnipeManipulator );
   SnipeManipulator *GetManipulator();

// CGLComboControl overrides
   virtual CGLListControl *AllocListControl();
   virtual void SetCurSel( int nIndex, BOOL bNotify=TRUE );

// CGLControl overrides
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );

// CCmdTarget overrides
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
};

class CManipulatorComboCmdUI : public CGLControlCmdUI
{
public:
   CManipulatorComboCmdUI( CGLControl *pControl )
      : CGLControlCmdUI( pControl ) {}
   void SetAvailableManipulators( SnipeManipulatorList *pSnipeManipulatorList );
   void SetManipulator( SnipeManipulator *pSnipeManipulator );
};

class CGLManipulatorComboListControl : public CGLComboListControl
{
public:
// CGLListControl overrides
   virtual CGLListItem *AllocItem( CGLListControl *pListControl, LPARAM lParam );
};

class CGLManipulatorListItem : public CGLListItem
{
public:
   SnipeManipulator *m_pSnipeManipulator;

   CGLManipulatorListItem( SnipeManipulator *pSnipeManipulator ) { m_pSnipeManipulator = pSnipeManipulator; }

// CGLListItem overrides
   virtual int GetItemHeight();

// CGLControl overrides
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
};
