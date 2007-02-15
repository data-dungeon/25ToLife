//

#include "stdafx.h"
#include "SubdivideUpUI.h"
#include "CommandUIAccel.h"
#include "../resource.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "HCGLModelView.h"
#include "HCommandPluginManager.h"
#include "SubdivideCommand.h"
#include "HCModelDoc.h"
#include "HPatchModel.h"
#include "HIntProperty.h"
#include "HSelectionList.h"

const char *SubdividePatchUpCommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *SubdividePatchUpCommandUIPlugin::GetTitleName() const { return "Subdivide Patch Up"; }
const char *SubdividePatchUpCommandUIPlugin::GetDescription() const { return "Turns a patch's subdivisions down."; }

UINT SubdividePatchUpCommandUIPlugin::GetBitmapID() const { return IDB_SUBDIVIDEUP; }

void SubdividePatchUpCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pSubdivideCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Subdivide");
   if (m_pSubdivideCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Subdivide ).");
}

void SubdividePatchUpCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pEditToolBar = pMainFrame->AddToolBar(GetCategory());
   pEditToolBar->AddButton(GetPlugin());
}

// Object Message Maps
SnipeObjectType SubdividePatchUpCommandUIPlugin::GetCommandObjectType() { return SOT_Patch; }

void SubdividePatchUpCommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( pObject->IsDeletable() );
}

void SubdividePatchUpCommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   if (!pObject->IsKindOf(SOT_Patch))
      return;

   HMainInterface *pMI = GetHMainInterface();
   ASSERT(pMI);

   HSelectionList *pSelectionList = pMI->GetCurrentSelectionList();
   if ( pSelectionList->GetCount() == 0 )
      return;

   String strDescription;

   strOutParameters = "-inc ";

   for ( POSITION pos = pSelectionList->GetHeadPosition(); pos; )
   {
      HSnipeObject *pSnipeObject = pSelectionList->GetNext(pos);

      if (!pSnipeObject->IsKindOf(SOT_Patch))
         continue;

      pMI->ObjectToDescription(pSnipeObject,strDescription);
      strOutParameters += " ";
      strOutParameters += strDescription;
   }

   pOutPlugin = m_pSubdivideCommandPlugin;
}

//////////////////////////////////////////////////////////
// SubdivideDownCommandUIPlugin

const char *SubdivideModelUpCommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *SubdivideModelUpCommandUIPlugin::GetTitleName() const { return "Subdivide Models Up"; }
const char *SubdivideModelUpCommandUIPlugin::GetDescription() const { return "Turns all models' subdivisions in the active view up."; }

UINT SubdivideModelUpCommandUIPlugin::GetBitmapID() const { return IDB_SUBDIVIDEUP; }

static CommandUIAccel f_pSubdivideModelUpAccel[] = 
{
   CommandUIAccel(FVIRTKEY, VK_PRIOR)
};

void SubdivideModelUpCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pSubdivideModelUpAccel)/sizeof(CommandUIAccel);
   pAccel = f_pSubdivideModelUpAccel;
}

void SubdivideModelUpCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
}

void SubdivideModelUpCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pEditToolBar = pMainFrame->AddToolBar(GetCategory());
   pEditToolBar->AddButton(GetPlugin());
}

void SubdivideModelUpCommandUIPlugin::OnCommandView( HCGLBaseView *pView, HCommandPlugin *&pOutPlugin, String &strOutParameters )
{
   HCGLModelView *pModelView = (HCGLModelView *)pView;
   HCModelDoc *pModelDoc = pModelView->GetDocument();
   HSnipeObject *pObjectHead = pModelDoc->GetObjectHead();

   switch (pObjectHead->GetSOT())
   {
      case SOT_PatchModel:
//      case SOT_BezierSplineModel:
      {
         HPatchModel *pPatchModel = (HPatchModel *)pObjectHead;
         int nPatchSubdivisions = pPatchModel->GetPatchSubdivisions()->GetValue();
         nPatchSubdivisions++;
         if (nPatchSubdivisions>4)
            nPatchSubdivisions = 4;
         pPatchModel->GetPatchSubdivisions()->StoreValue( nPatchSubdivisions );
      }
   }
}
