//

#include "stdafx.h"
#include "SubdivideDownUI.h"
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

const char *SubdividePatchDownCommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *SubdividePatchDownCommandUIPlugin::GetTitleName() const { return "Subdivide Patch Down"; }
const char *SubdividePatchDownCommandUIPlugin::GetDescription() const { return "Turns a patch's subdivisions down."; }

UINT SubdividePatchDownCommandUIPlugin::GetBitmapID() const { return IDB_SUBDIVIDEDOWN; }

void SubdividePatchDownCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pSubdivideCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Subdivide");
   if (m_pSubdivideCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Subdivide ).");
}

void SubdividePatchDownCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pEditToolBar = pMainFrame->AddToolBar(GetCategory());
   pEditToolBar->AddButton(GetPlugin());
}

// Object Message Maps
SnipeObjectType SubdividePatchDownCommandUIPlugin::GetCommandObjectType() { return SOT_Patch; }

void SubdividePatchDownCommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( TRUE );
}

void SubdividePatchDownCommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   if (!pObject->IsKindOf(SOT_Patch))
      return;

   HMainInterface *pMI = GetHMainInterface();
   ASSERT(pMI);

   HSelectionList *pSelectionList = pMI->GetCurrentSelectionList();
   if (pSelectionList->GetCount()==0)
      return;

   strOutParameters = "-dec ";
   String strDescription;

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

const char *SubdivideModelDownCommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *SubdivideModelDownCommandUIPlugin::GetTitleName() const { return "Subdivide Models Down"; }
const char *SubdivideModelDownCommandUIPlugin::GetDescription() const { return "Turns all models' subdivisions in the active view down."; }

UINT SubdivideModelDownCommandUIPlugin::GetBitmapID() const { return IDB_SUBDIVIDEDOWN; }

static CommandUIAccel f_pSubdivideModelDownAccel[] = 
{
   CommandUIAccel(FVIRTKEY, VK_NEXT)
};

void SubdivideModelDownCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pSubdivideModelDownAccel)/sizeof(CommandUIAccel);
   pAccel = f_pSubdivideModelDownAccel;
}

void SubdivideModelDownCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
}

void SubdivideModelDownCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pEditToolBar = pMainFrame->AddToolBar(GetCategory());
   pEditToolBar->AddButton(GetPlugin());
}

void SubdivideModelDownCommandUIPlugin::OnCommandView( HCGLBaseView *pView, HCommandPlugin *&pOutPlugin, String &strOutParameters )
{
   HCGLModelView *pModelView = (HCGLModelView *)pView;
   HCModelDoc *pModelDoc = pModelView->GetDocument();
   HSnipeObject *pObjectHead = pModelDoc->GetObjectHead();

   switch (pObjectHead->GetSOT())
   {
      case SOT_PatchModel:
      {
         HPatchModel *pPatchModel = (HPatchModel *)pObjectHead;
         int nPatchSubdivisions = pPatchModel->GetPatchSubdivisions()->GetValue();
         nPatchSubdivisions--;
         if (nPatchSubdivisions<0)
            nPatchSubdivisions = 0;
         pPatchModel->GetPatchSubdivisions()->StoreValue( nPatchSubdivisions );
      }
   }
}
