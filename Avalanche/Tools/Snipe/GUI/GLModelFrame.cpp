#include "stdafx.h"
#include "GLModelFrame.h"
#include "GLModelView.h"
#include "GLTexCoordView.h"
#include "GLSchematicView.h"
#include "GLExplorerView.h"
#include "ModelDoc.h"
#include "PolyModel.h"
#include "Resource.h"

IMPLEMENT_SERIAL(CGLModelFrame, CGLHeaderFrame, 1)

CGLModelFrame::CGLModelFrame()
{
}

CGLModelFrame::~CGLModelFrame()
{
}

void CGLModelFrame::PostInit()
{
   CGLHeaderFrame::PostInitGuts();

   AddHeaderControl( &m_glViewModeComboControl, CPoint(0,0), CSize(94,CGLHeaderFrame::HEADERHEIGHT), ID_VIEW_MODE_COMBO );

   CreateView();

   m_glViewModeComboControl.AddString( "Modeler" );
   m_glViewModeComboControl.AddString( "Texture Coordinates" );
   m_glViewModeComboControl.AddString( "Explorer" );
   m_glViewModeComboControl.AddString( "Schematic" );
   m_glViewModeComboControl.SetCurSel( 0, FALSE );
}

void CGLModelFrame::Save( BaseStream &bs, BOOL bWriteHeader/*=FALSE*/ )
{
   CGLControlHeader header( this, bWriteHeader, bs );

   bs << DataHeader( "View", GetView()->GetViewAngle() ) << nl;
   GetView()->m_pCurCamera->Save( bs, TRUE );
}

BOOL CGLModelFrame::ParseData( BaseStream &bs, const DataHeader &dataheader )
{
   if (dataheader.m_strKey == "View")
   {
      ViewAngleIndex nIndex = (ViewAngleIndex)atoi( dataheader.m_strValue.Get() );
      GetView()->SetViewAngle( nIndex, FALSE );
      return TRUE;
   }
   else if (GetView()->GetCamera()->ParseData( bs, dataheader ))
      return TRUE;
   else
      return CGLHeaderFrame::ParseData( bs, dataheader );
}

SnipeObject *CGLModelFrame::GetObject( const String &strClass )
{
   if (strClass == "Camera")
      return GetView()->GetCamera();
   else
      return CGLHeaderFrame::GetObject( strClass );
}

void CGLModelFrame::SetViewMode( int nIndex )
{
   CRuntimeClass *pViewClass = NULL;

   switch (nIndex)
   {
      case VIEWMODE_MODELER:
         pViewClass = RUNTIME_CLASS(CGLModelView);
         break;
      case VIEWMODE_UVEDITOR:
         pViewClass = RUNTIME_CLASS(CGLTexCoordView);
         break;
      case VIEWMODE_EXPLORER:
         pViewClass = RUNTIME_CLASS(CGLExplorerView);
         break;
      case VIEWMODE_SCHEMATIC:
         pViewClass = RUNTIME_CLASS(CGLSchematicView);
         break;
   }

   m_nNextControlPos = m_glViewModeComboControl.m_ptControl.x + m_glViewModeComboControl.m_szControl.cx;

   if (m_pViewClass != pViewClass)
   {
      m_pViewClass = pViewClass;
      CreateView();
   }
}

BOOL CGLModelFrame::OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYCTRL_CB_CHANGED:
      {
         UINT nID = LOWORD(wParam);
         int nIndex = HIWORD(wParam);
         switch (nID)
         {
            case ID_VIEW_MODE_COMBO:
            {
               SetViewMode( nIndex );
               GetApp().DrawInvalidControls();
               return TRUE;
            }
         }
         break;
      }
   }

   return CGLHeaderFrame::OnControlNotify( msg, wParam, lParam );
}
