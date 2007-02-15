//

#include "stdafx.h"
#include "CommandUIAccel.h"
#include "../resource.h"
#include "HToolBarControl.h"
#include "HCGLModelView.h"
#include "SelectionTypeUI.h"
#include "HCMainFrame.h"

const char *VertexSelectionCommandUIPlugin::GetTitleName() const { return "Vertex Selection"; }
const char *VertexSelectionCommandUIPlugin::GetDescription() const { return "Changed selection type to vertices."; }
const char *VertexSelectionCommandUIPlugin::GetCategory() const { return "Select"; }

UINT VertexSelectionCommandUIPlugin::GetBitmapID() const { return IDB_VERTEX; }

static CommandUIAccel f_pVertexAccel [] = 
{
   CommandUIAccel(FVIRTKEY, '1')
};

void VertexSelectionCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pVertexAccel)/sizeof(CommandUIAccel);
   pAccel = f_pVertexAccel;
}

void VertexSelectionCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pManipulatorToolBar = pMainFrame->AddToolBar(GetCategory());
   pManipulatorToolBar->AddButton(GetPlugin());
}

// View Message Maps
void VertexSelectionCommandUIPlugin::OnUpdateCommandUIView(const HCGLBaseView *pView, CCmdUI *pCmdUI)
{
   BOOL bEnable = FALSE;

   for (UINT i=0; i < pView->GetNumSelectionSOTs(); i++)
   {
      if (pView->GetSelectionSOTAt(i) == SOT_Vertex)
      {
         bEnable = TRUE;
         break;
      }
   }
   pCmdUI->Enable( bEnable );
   pCmdUI->SetCheck( bEnable && pView->GetCurrentSelectionSOT() == SOT_Vertex );
}

void VertexSelectionCommandUIPlugin::OnCommandView(HCGLBaseView *pView, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   pView->SetCurrentSelectionSOT( SOT_Vertex );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char *EdgeSelectionCommandUIPlugin::GetTitleName() const { return "Edge Selection"; }
const char *EdgeSelectionCommandUIPlugin::GetDescription() const { return "Changed selection type to Edges."; }
const char *EdgeSelectionCommandUIPlugin::GetCategory() const { return "Select"; }

UINT EdgeSelectionCommandUIPlugin::GetBitmapID() const { return IDB_EDGE; }

static CommandUIAccel f_pEdgeAccel [] = 
{
   CommandUIAccel(FVIRTKEY, '2')
};

void EdgeSelectionCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pEdgeAccel)/sizeof(CommandUIAccel);
   pAccel = f_pEdgeAccel;
}

void EdgeSelectionCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pManipulatorToolBar = pMainFrame->AddToolBar(GetCategory());
   pManipulatorToolBar->AddButton(GetPlugin());
}

// View Message Maps
void EdgeSelectionCommandUIPlugin::OnUpdateCommandUIView(const HCGLBaseView *pView, CCmdUI *pCmdUI)
{
   BOOL bEnable = FALSE;

   for (UINT i=0; i < pView->GetNumSelectionSOTs(); i++)
   {
      if (pView->GetSelectionSOTAt(i) == SOT_Edge)
      {
         bEnable = TRUE;
         break;
      }
   }
   pCmdUI->Enable( bEnable );
   pCmdUI->SetCheck( bEnable && pView->GetCurrentSelectionSOT() == SOT_Edge );
}

void EdgeSelectionCommandUIPlugin::OnCommandView(HCGLBaseView *pView, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   pView->SetCurrentSelectionSOT( SOT_Edge );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char *FaceSelectionCommandUIPlugin::GetTitleName() const { return "Face Selection"; }
const char *FaceSelectionCommandUIPlugin::GetDescription() const { return "Changed selection type to Faces."; }
const char *FaceSelectionCommandUIPlugin::GetCategory() const { return "Select"; }

UINT FaceSelectionCommandUIPlugin::GetBitmapID() const { return IDB_FACE; }

static CommandUIAccel f_pFaceAccel [] = 
{
   CommandUIAccel(FVIRTKEY, '3')
};

void FaceSelectionCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pFaceAccel)/sizeof(CommandUIAccel);
   pAccel = f_pFaceAccel;
}

void FaceSelectionCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pManipulatorToolBar = pMainFrame->AddToolBar(GetCategory());
   pManipulatorToolBar->AddButton(GetPlugin());
   pManipulatorToolBar->AddButton(NULL);
}

// View Message Maps
void FaceSelectionCommandUIPlugin::OnUpdateCommandUIView(const HCGLBaseView *pView, CCmdUI *pCmdUI)
{
   BOOL bEnable = FALSE;

   for (UINT i=0; i < pView->GetNumSelectionSOTs(); i++)
   {
      if (pView->GetSelectionSOTAt(i) == SOT_Poly)
      {
         bEnable = TRUE;
         break;
      }
   }
   pCmdUI->Enable( bEnable );
   pCmdUI->SetCheck( bEnable && pView->GetCurrentSelectionSOT() == SOT_Poly );
}

void FaceSelectionCommandUIPlugin::OnCommandView(HCGLBaseView *pView, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   pView->SetCurrentSelectionSOT( SOT_Poly );
}
