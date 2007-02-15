//
#include "stdafx.h"
#include "Snipe.h"
#include "../Utility/BoundingBox.h"
#include "GLModelView.h"
#include "GLTexCoordView.h"
#include "GLHeaderFrame.h"
#include "ModelDoc.h"
#include "GLModelFrame.h"
#include "Resource.h"
#include "PolyModel.h"
#include "Project.h"
#include "..\Utility\Map.h"
#include "PatchModel.h"
#include "Interface/SnipeNavigator.h"

IMPLEMENT_DYNCREATE(CGLModelView, CGL3DView)

BEGIN_MESSAGE_MAP(CGLModelView, CGL3DView)
   ON_UPDATE_COMMAND_UI(ID_DISPLAY_SHADED, OnUpdateDisplayShaded)
   ON_COMMAND(ID_DISPLAY_SHADED, OnDisplayShaded)
   ON_UPDATE_COMMAND_UI(ID_DISPLAY_SHADEDWIREFRAME, OnUpdateDisplayShadedWireframe)
   ON_COMMAND(ID_DISPLAY_SHADEDWIREFRAME, OnDisplayShadedWireframe)
   ON_UPDATE_COMMAND_UI(ID_DISPLAY_WIREFRAME, OnUpdateDisplayWireframe)
   ON_COMMAND(ID_DISPLAY_WIREFRAME, OnDisplayWireframe)
   ON_UPDATE_COMMAND_UI(ID_DISPLAY_TEXTURES, OnUpdateDisplayTextures)
   ON_COMMAND(ID_DISPLAY_TEXTURES, OnDisplayTextures)
   ON_COMMAND(ID_DISPLAYAS_POLYMODEL, OnDrawAsPolyModel)
   ON_COMMAND(ID_DISPLAYAS_PATCHMODEL, OnDrawAsPatchModel)
END_MESSAGE_MAP()

static TypeProperty::Info::Node f_shadingmodeinfos[] = {
   TypeProperty::Info::Node( "Wireframe", "Wireframe" ),
   TypeProperty::Info::Node( "Shaded", "Shaded" ),
   TypeProperty::Info::Node( "Shaded + Wireframe", "ShadedWireframe" )
};

static TypeProperty::Info::Node f_drawasinfos[] = {
   TypeProperty::Info::Node( "Polygons", "Polygons" ),
   TypeProperty::Info::Node( "Patches", "Patches" ),
};

TypeProperty::Info CGLModelView::m_nShadingModeInfo( "Shading Mode", "ShadingMode", f_shadingmodeinfos, sizeof(f_shadingmodeinfos)/sizeof(TypeProperty::Info::Node), CGLModelView::SM_SHADEDWIREFRAME );
TypeProperty::Info CGLModelView::m_nDrawAsInfo( "Draw As", "DrawAs", f_drawasinfos, sizeof(f_drawasinfos)/sizeof(TypeProperty::Info::Node), CGLModelView::DRAWAS_PATCHMODEL );
//CategoryDisplay
   BoolProperty::Info CGLModelView::m_bDisplayControlHandlesInfo( "Control Handles", "ControlHandles", false );
   BoolProperty::Info CGLModelView::m_bDisplayTexturesInfo( "Textures", "DisplayTextures", TRUE );
   BoolProperty::Info CGLModelView::m_bDisplayStatsInfo( "Statistics", "Statistics", FALSE );
   Property::Info CGLModelView::m_DiagnosticInfo( "Diagnostic", "Diagnostic" );
      BoolProperty::Info CGLModelView::m_bDisplayInRetainInfo( "Retained Mode", "RetainedMode", TRUE );
      BoolProperty::Info CGLModelView::m_bDisplayVertexIndicesInfo( "Vertex Indices", "VertexIndices", FALSE );
      BoolProperty::Info CGLModelView::m_bDisplayVertexNormalIndicesInfo( "Vert Norm Indices", "VertexNormalIndices", FALSE );
      BoolProperty::Info CGLModelView::m_bDisplayVertexNormalsInfo( "Vertex Normals", "VertexNormals", FALSE );
      BoolProperty::Info CGLModelView::m_bDisplayEdgeIndicesInfo( "Edge Indices", "EdgeIndices", FALSE );
         BoolProperty::Info CGLModelView::m_bDisplayContinuityIDsInfo( "Continuity IDs", "ContinuityIDs", false );
      BoolProperty::Info CGLModelView::m_bDisplayContinuityInfo( "Continuity", "Continuity", false );
      BoolProperty::Info CGLModelView::m_bDisplayFaceIndicesInfo( "Face Indices", "FaceIndices", FALSE );
      BoolProperty::Info CGLModelView::m_bDisplayFaceNormalsInfo( "Face Normals", "FaceNormals", FALSE );
      BoolProperty::Info CGLModelView::m_bDisplayFirstVertexInfo( "First Vertex", "FirstVertex", FALSE );
      BoolProperty::Info CGLModelView::m_bDisplayUVFaceIndicesInfo( "UVFace Indices", "UVFaceIndices", FALSE ); // Only on First UVPoly of Face
      BoolProperty::Info CGLModelView::m_bDisplayUVIndicesInfo( "UV Indices", "UVIndices", FALSE ); // Only on First UVPoly of Face
      BoolProperty::Info CGLModelView::m_bDisplayUVEdgeIndicesInfo( "UVEdge Indices", "UVEdgeIndices", FALSE ); // Only on First UVPoly of Face
      BoolProperty::Info CGLModelView::m_bDisplayBoundingBoxInfo( "Bounding Box", "BoundingBox", FALSE );
      BoolProperty::Info CGLModelView::m_bDisplayUVBoundaryInfo( "UV Boundary", "UVBoundary", FALSE );
      BoolProperty::Info CGLModelView::m_bDisplayControlVerticesInfo( "Control Vertices", "ControlVertices", false );
      BoolProperty::Info CGLModelView::m_bDisplayHiddenEdgesInfo( "Hidden Edges", "Hidden Edges", false );

CGLModelView::CGLModelView()
{
   m_nShadingMode.Init( &m_nShadingModeInfo, this );
   m_nDrawAs.Init( &m_nDrawAsInfo, this );
   //CategoryDisplay
      m_bDisplayTextures.Init( &m_bDisplayTexturesInfo, &m_categoryDisplay );
      m_bDisplayControlHandles.Init( &m_bDisplayControlHandlesInfo, &m_categoryDisplay );
      m_bDisplayStats.Init( &m_bDisplayStatsInfo, &m_categoryDisplay );
      m_Diagnostic.Init( &m_DiagnosticInfo, &m_categoryDisplay );
         m_bDisplayInRetain.Init( &m_bDisplayInRetainInfo, &m_Diagnostic );
         m_bDisplayVertexIndices.Init( &m_bDisplayVertexIndicesInfo, &m_Diagnostic );
         m_bDisplayVertexNormalIndices.Init( &m_bDisplayVertexNormalIndicesInfo, &m_Diagnostic );
         m_bDisplayVertexNormals.Init( &m_bDisplayVertexNormalsInfo, &m_Diagnostic );
         m_bDisplayEdgeIndices.Init( &m_bDisplayEdgeIndicesInfo, &m_Diagnostic );
            m_bDisplayContinuityIDs.Init( &m_bDisplayContinuityIDsInfo, &m_bDisplayEdgeIndices );
         m_bDisplayContinuity.Init( &m_bDisplayContinuityInfo, &m_Diagnostic );
         m_bDisplayFaceIndices.Init( &m_bDisplayFaceIndicesInfo, &m_Diagnostic );
         m_bDisplayFaceNormals.Init( &m_bDisplayFaceNormalsInfo, &m_Diagnostic );
         m_bDisplayFirstVertex.Init( &m_bDisplayFirstVertexInfo, &m_Diagnostic );
         m_bDisplayUVFaceIndices.Init( &m_bDisplayUVFaceIndicesInfo, &m_Diagnostic );
         m_bDisplayUVIndices.Init( &m_bDisplayUVIndicesInfo, &m_Diagnostic );
         m_bDisplayUVEdgeIndices.Init( &m_bDisplayUVEdgeIndicesInfo, &m_Diagnostic );
         //m_bDisplayBoundingBox.Init( &m_bDisplayBoundingBoxInfo, &m_Diagnostic );
         m_bDisplayUVBoundary.Init( &m_bDisplayUVBoundaryInfo, &m_Diagnostic );
         m_bDisplayControlVertices.Init( &m_bDisplayControlVerticesInfo, &m_Diagnostic );
         m_bDisplayHiddenEdges.Init( &m_bDisplayHiddenEdgesInfo, &m_Diagnostic );

   m_nID = IDR_MODELVIEW;
   m_pDoPickLimitingComponent = NULL;
   m_nDoPickLimitingComponentBaseType = SOT_Unknown;
}

CGLModelView::~CGLModelView()
{
}

void CGLModelView::PostInit()
{
   CGL3DView::PostInit();

   GetHeaderFrame()->AddHeaderControl( &m_glWireFrameButton, CPoint(5,-1), CSize(BUTTONWIDTH,HEADERHEIGHT+1), ID_DISPLAY_WIREFRAME );
   GetHeaderFrame()->AddHeaderControl( &m_glShadedButton, CPoint(-1,-1), CSize(BUTTONWIDTH,HEADERHEIGHT+1), ID_DISPLAY_SHADED );
   GetHeaderFrame()->AddHeaderControl( &m_glShadedWireframeButton, CPoint(-1,-1), CSize(BUTTONWIDTH,HEADERHEIGHT+1), ID_DISPLAY_SHADEDWIREFRAME );
   GetHeaderFrame()->AddHeaderControl( &m_glDisplayAsPolyModelButton, CPoint(6,-1), CSize(BUTTONWIDTH,HEADERHEIGHT+1), ID_DISPLAYAS_POLYMODEL );
   GetHeaderFrame()->AddHeaderControl( &m_glDisplayAsPatchModelButton, CPoint(-1,-1), CSize(BUTTONWIDTH,HEADERHEIGHT+1), ID_DISPLAYAS_PATCHMODEL );

   PolyModel *pModel = GetDocument()->GetCurrentModel();

   if (pModel)
   {
      if (pModel->GetSOT() == SOT_PatchModel)
         SetMaxDrawAs( DRAWAS_PATCHMODEL );
      else
         SetMaxDrawAs( DRAWAS_POLYMODEL );
   }

   SetDrawAs(GetMaxDrawAs());

   if (!pModel || !pModel->IsKindOf( SOT_PatchModel ))
   {
      m_bDisplayControlHandles.SetVisible( FALSE );
      m_bDisplayControlVertices.SetVisible( FALSE );
      m_bDisplayHiddenEdges.SetVisible( FALSE );
      m_bDisplayContinuity.SetVisible( FALSE );
      m_bDisplayContinuityIDs.SetVisible( FALSE );
   }

   FillViewAngleCombo();
   UpdateHeader();
}

#define ZOOMPADDING 1.03f

void CGLModelView::DefaultZoomFit(SnipeObjectList *pList)
{
   ASSERT(pList);

   BoundingBox bbox;
   pList->ComputeBoundingBox( bbox );

   Vector newtarget;
   Vector vRotate;
   float fDistance;

   if (bbox.IsEmpty())
   {
      newtarget.Set(0);
      vRotate.Set(0);
      fDistance = 175.0f;
   }
   else
   {
      float fHalfFOVy = m_pCurCamera->m_fFOV * 0.5f;
      float fHalfHeight = bbox.GetHeight() * ZOOMPADDING * 0.5f;
      float fDistanceY = fHalfHeight / tan( fHalfFOVy * DEGTORAD );
      fDistanceY += bbox.GetDepth() * 0.5f;

      float fAspect = (GLdouble)m_szControl.cx/(double)m_szControl.cy;
      float fHalfFOVx = fHalfFOVy * fAspect;
      float fHalfWidth = bbox.GetWidth() * ZOOMPADDING * 0.5f;
      float fDistanceX = fHalfWidth / tan( fHalfFOVx * DEGTORAD );
      fDistanceX += bbox.GetDepth() * 0.5f;

      fDistance = max( fDistanceX, fDistanceY );
      if (!fDistance)
         fDistance = GetCamera()->m_fFocusDistance;

      newtarget = bbox.CalculateCenterPoint();
      vRotate = m_pCurCamera->m_transform.m_vRotate;
   }

   m_pCurCamera->CalculateTransform( newtarget, fDistance, vRotate.y, vRotate.x );

   Invalidate();
}

void CGLModelView::UpdateHeader()
{
   m_glWireFrameButton.SetChecked( m_nShadingMode == CGLModelView::SM_WIREFRAME );
   m_glShadedButton.SetChecked( m_nShadingMode == CGLModelView::SM_SHADED );
   m_glShadedWireframeButton.SetChecked( m_nShadingMode == CGLModelView::SM_SHADEDWIREFRAME );
   m_glDisplayAsPolyModelButton.SetChecked( IsDrawAsPolyModel() );
   m_glDisplayAsPatchModelButton.SetChecked( IsDrawAsPatchModel() );

   m_glWireFrameButton.SetVisible( m_nViewAngle < NUM3DVIEWANGLEPRESETS );
   m_glShadedButton.SetVisible( m_nViewAngle < NUM3DVIEWANGLEPRESETS );
   m_glShadedWireframeButton.SetVisible( m_nViewAngle < NUM3DVIEWANGLEPRESETS );
   m_glDisplayAsPolyModelButton.SetVisible( m_nViewAngle < NUM3DVIEWANGLEPRESETS );
   m_glDisplayAsPatchModelButton.SetVisible( m_nViewAngle < NUM3DVIEWANGLEPRESETS );

   m_glDisplayAsPolyModelButton.SetEnabled( GetMaxDrawAs() >= CGLModelView::DRAWAS_POLYMODEL );
   m_glDisplayAsPatchModelButton.SetEnabled( GetMaxDrawAs() >= CGLModelView::DRAWAS_PATCHMODEL );

   CGL3DView::UpdateHeader();
}

void CGLModelView::DrawContents()
{
   SetProjectionMatrix();

   if (m_bDoPick)
   {
      CGLBaseView::DrawContents();
      return;
   }

   if (GetManipulatorInstance())
      GetManipulatorInstance()->PreDraw(this);

   if (GetNavigatorInstance())
      GetNavigatorInstance()->PreDraw(this);

   // set up lights
   glPushAttrib( GL_LIGHTING_BIT );
      CModelDoc *pDoc = GetDocument();
      if (!pDoc->m_lightarray.IsEmpty())
      {
         for (UINT nLightIndex=0; nLightIndex<(UINT)pDoc->m_lightarray.GetCount(); nLightIndex++)
            pDoc->m_lightarray[nLightIndex]->Bind( nLightIndex );
      }
      else if (m_pCurLight)
         m_pCurLight->Bind( 0 );

      CGLBaseView::DrawContents();
   glPopAttrib(); // put lighting back the way it was

   if (m_bDisplayInRetain)
      DrawSelectedObjects();
   DrawGrid();  
   DrawAxis();

   DrawStatistics();

   if (GetNavigatorInstance())
      GetNavigatorInstance()->PostDraw(this);

   if (GetManipulatorInstance())
      GetManipulatorInstance()->PostDraw(this);

   DrawViewControls();
}

void CGLModelView::OnUpdateDisplayWireframe(CCmdUI *pCmdUI)
{
   pCmdUI->SetCheck( m_nShadingMode == SM_WIREFRAME );
}

void CGLModelView::OnDisplayWireframe()
{
   m_nShadingMode.StoreValue( SM_WIREFRAME );

   GetApp().DrawInvalidControls();
}

void CGLModelView::OnUpdateDisplayShaded(CCmdUI *pCmdUI)
{
   pCmdUI->SetCheck( m_nShadingMode == SM_SHADED );
}

void CGLModelView::OnDisplayShaded()
{
   m_nShadingMode.StoreValue( SM_SHADED );

   GetApp().DrawInvalidControls();
}

void CGLModelView::OnUpdateDisplayShadedWireframe(CCmdUI *pCmdUI)
{
   pCmdUI->SetCheck( m_nShadingMode == SM_SHADEDWIREFRAME );
}

void CGLModelView::OnDisplayShadedWireframe()
{
   m_nShadingMode.StoreValue( SM_SHADEDWIREFRAME );

   GetApp().DrawInvalidControls();
}

void CGLModelView::OnUpdateDisplayTextures(CCmdUI *pCmdUI)
{
   pCmdUI->SetCheck( m_bDisplayTextures );
}

void CGLModelView::OnDisplayTextures()
{
   m_bDisplayTextures.StoreValue( !m_bDisplayTextures );

   GetApp().DrawInvalidControls();
}

void CGLModelView::OnDrawAsPolyModel()
{
   SetDrawAs( DRAWAS_POLYMODEL );

   GetApp().DrawInvalidControls();
}

void CGLModelView::OnDrawAsPatchModel()
{
   SetDrawAs( DRAWAS_PATCHMODEL );

   GetApp().DrawInvalidControls();
}

void CGLModelView::DrawStatistics()
{
   CModelDoc *pDoc = GetDocument();
   if (!m_bDisplayStats)
      return;

   Property *pStatistics = pDoc->GetObjectHead()->GetStatisticsProperty();
   if (!pStatistics)
      return;

   PropertyArray *pPropertyArray = pStatistics->GetPropertyArray();
   if (!pPropertyArray)
      return;

   float x = 20, y = 20;
   String strValue;
   glColor3f( 1.0f, 1.0f, 1.0f );
   for (int i=0; i<pPropertyArray->GetCount(); i++)
   {
      Property *pProperty = pPropertyArray->GetAt( i );
      pProperty->GetValueString( strValue );
      DrawText( x, y, pProperty->GetTitle() + ": " + strValue );
      y += 18;
   }
}

void CGLModelView::ConvertSelectionToCurrentSelectionType( SnipeObjectType nSOT )
{
   SelectionList *pSelectionList = GetDocument()->GetSelectionList();

   if (pSelectionList->IsEmpty())
      return;

   SnipeObjectList *pNewList = pSelectionList->CreateSnipeObjectList();
   pNewList->ConvertCompatibleObjectsToSOT(nSOT);

   DoSelectCommand( GetDocument()->GetCommandObject(), pNewList );
   delete pNewList;
}

BOOL CGLModelView::OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYCTRL_BN_CLICKED:
      {
         UINT nID = LOWORD(wParam);
         switch (nID)
         {
            case ID_DISPLAY_SHADED:
               OnDisplayShaded();
               return TRUE;
            case ID_DISPLAY_WIREFRAME:
               OnDisplayWireframe();
               return TRUE;
            case ID_DISPLAY_SHADEDWIREFRAME:
               OnDisplayShadedWireframe();
               return TRUE;
            case ID_DISPLAY_TEXTURES:
               OnDisplayTextures();
               return TRUE;
            case ID_DISPLAYAS_POLYMODEL:
               OnDrawAsPolyModel();
               return TRUE;
            case ID_DISPLAYAS_PATCHMODEL:
               OnDrawAsPatchModel();
               return TRUE;
         }
         break;
      }
   }

   return CGL3DView::OnControlNotify( msg, wParam, lParam );
}

BOOL CGLModelView::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_VALUECHANGED:
      {
         Property *pProperty = (Property *)lParam;
         if (pProperty == &m_nDrawAs && GetDocument() && GetDocument()->m_pCurModel)
            GetDocument()->m_pCurModel->FreeRTVertices();
         UpdateHeader();
         break;
      }
   }

   return CGL3DView::OnObjectNotify( msg, wParam, lParam );
}

BOOL CGLModelView::OnSnipeIdle()
{
   BOOL bMoreIdles = CGL3DView::OnSnipeIdle();

   if (GetCurrentSelectionSOT()==SOT_Bone && SelectionList::GetFocusObject() && SelectionList::GetFocusObject()->IsKindOf(SOT_Bone))
   {
      Invalidate();
      bMoreIdles = TRUE;
   }

   return bMoreIdles;
}
