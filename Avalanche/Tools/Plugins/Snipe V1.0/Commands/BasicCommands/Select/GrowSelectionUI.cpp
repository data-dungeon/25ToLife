//

#include "stdafx.h"
#include "CommandUIAccel.h"
#include "HMainInterface.h"
#include "HSelectionList.h"
#include "HCommandPluginManager.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "GrowSelectionUI.h"
#include "../Resource.h"
#include "HMainInterface.h"
#include "HEdge.h"
#include "HCBaseDoc.h"

const char *GrowSelectionCommandUIPlugin::GetCategory() const { return "Select"; }
const char *GrowSelectionCommandUIPlugin::GetTitleName() const { return "Grow Selection"; }
const char *GrowSelectionCommandUIPlugin::GetDescription() const { return "Grow the current selection."; }

UINT GrowSelectionCommandUIPlugin::GetBitmapID() const { return IDB_GROWSELECTION; }

static CommandUIAccel f_pGrowAccel [] = 
{
   CommandUIAccel(FVIRTKEY|FCONTROL, VK_ADD)
};

void GrowSelectionCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pGrowAccel)/sizeof(CommandUIAccel);
   pAccel = f_pGrowAccel;
}

void GrowSelectionCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pGrowSelectionCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "GrowSelection");
   if (m_pGrowSelectionCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Grow Selection ).");
}

void GrowSelectionCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pToolBar = pMainFrame->AddToolBar(GetCategory());
   pToolBar->AddButton(GetPlugin());
}

// Message Maps
void GrowSelectionCommandUIPlugin::OnUpdateCommandUIDoc(const HCBaseDoc *pDoc, CCmdUI *pCmdUI)
{
   HSelectionList *pList = pDoc->GetSelectionList();
   HSnipeObject *pObject = NULL;

   if (pList && !pList->IsEmpty())
      pObject = pList->GetHead();

   BOOL bEnable = ( pObject && pObject->IsKindOf(SOT_Component));

   if (!bEnable && pCmdUI->m_pMenu)
      pCmdUI->m_pMenu->DeleteMenu( pCmdUI->m_nIndex, MF_BYPOSITION );

   pCmdUI->Enable( bEnable );
}

void GrowSelectionCommandUIPlugin::OnCommandDoc( HCBaseDoc *pDoc, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   pOutPlugin = m_pGrowSelectionCommandPlugin;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char *ShrinkSelectionCommandUIPlugin::GetCategory() const { return "Select"; }
const char *ShrinkSelectionCommandUIPlugin::GetTitleName() const { return "Shrink Selection"; }
const char *ShrinkSelectionCommandUIPlugin::GetDescription() const { return "Shrink the current selection."; }

UINT ShrinkSelectionCommandUIPlugin::GetBitmapID() const { return IDB_SHRINKSELECTION; }

static CommandUIAccel f_pShrinkAccel [] = 
{
   CommandUIAccel(FVIRTKEY|FCONTROL, VK_SUBTRACT)
};

void ShrinkSelectionCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pShrinkAccel)/sizeof(CommandUIAccel);
   pAccel = f_pShrinkAccel;
}

void ShrinkSelectionCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pShrinkSelectionCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "ShrinkSelection");
   if (m_pShrinkSelectionCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Shrink Selection ).");
}

void ShrinkSelectionCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pToolBar = pMainFrame->AddToolBar(GetCategory());
   pToolBar->AddButton(GetPlugin());
   pToolBar->AddButton(NULL);
}

// Message Maps
void ShrinkSelectionCommandUIPlugin::OnUpdateCommandUIDoc(const HCBaseDoc *pDoc, CCmdUI *pCmdUI)
{
   HSelectionList *pList = pDoc->GetSelectionList();
   HSnipeObject *pObject = NULL;

   if (pList && !pList->IsEmpty())
      pObject = pList->GetHead();

   BOOL bVisible = pObject && pObject->IsKindOf(SOT_Component);
   BOOL bEnable = ( bVisible && pList->GetCount() > 1 );

   if (!bVisible && pCmdUI->m_pMenu)
      pCmdUI->m_pMenu->DeleteMenu( pCmdUI->m_nIndex, MF_BYPOSITION );

   pCmdUI->Enable( bEnable );
}

void ShrinkSelectionCommandUIPlugin::OnCommandDoc( HCBaseDoc *pDoc, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   pOutPlugin = m_pShrinkSelectionCommandPlugin;
}
