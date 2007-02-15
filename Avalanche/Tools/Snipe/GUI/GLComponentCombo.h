#pragma once

#include "GLComboControl.h"

class SnipeObjectTypeArray;

class CGLComponentCombo : public CGLComboControl
{
public:
   SnipeObjectTypeArray *m_pSOTArray;

   CGLComponentCombo();

// Operations
   void SetAvailableComponentTypes( SnipeObjectTypeArray *pSOTArray );

// CGLComboControl overrides
   virtual CGLListControl *AllocListControl();
   virtual void SetCurSel( int nIndex, BOOL bNotify=TRUE );

// CGLControl overrides
   virtual void PostInit();
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );

// CCmdTarget overrides
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
};

class CComponentComboCmdUI : public CGLControlCmdUI
{
public:
   CComponentComboCmdUI ( CGLControl *pControl ) :
      CGLControlCmdUI ( pControl ) {}

   void SetAvailableComponentTypes( SnipeObjectTypeArray *pSOTArray );
};

class CGLComponentComboListControl : public CGLComboListControl
{
public:
// CGLListControl overrides
   virtual CGLListItem *AllocItem( CGLListControl *pListControl, LPARAM lParam );
};

class CGLComponentListItem : public CGLListItem
{
public:
   SnipeObjectInfo *m_pSnipeObjectInfo;

   CGLComponentListItem( SnipeObjectInfo *pSnipeObjectInfo ) { m_pSnipeObjectInfo = pSnipeObjectInfo; }

// CGLControl overrides
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
};

