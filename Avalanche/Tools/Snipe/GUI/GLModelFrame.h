#pragma once
#include "GLHeaderFrame.h"
#include "PropertyBar.h"

class CGLModelView;

class CGLModelFrame : public CGLHeaderFrame
{
   DECLARE_SERIAL( CGLModelFrame )
public:
// Attributes
   enum { VIEWMODE_MODELER, VIEWMODE_UVEDITOR, VIEWMODE_EXPLORER, VIEWMODE_SCHEMATIC };
   CGLComboControl  m_glViewModeComboControl; // TODO: move this into a derived HeaderFrame class.

            CGLModelFrame();
   virtual ~CGLModelFrame();

// Operations
   CGLModelView *GetView() { return (CGLModelView *)m_pView; }
   int GetViewMode() { return m_glViewModeComboControl.GetCurSel(); }
   void SetViewMode( int nIndex );

// CGLControl overrides
   virtual void PostInit();
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL ParseData( BaseStream &bs, const DataHeader &dataheader );
   virtual catch_msg SnipeObject *GetObject( const String &strClass );
   virtual BOOL OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};
