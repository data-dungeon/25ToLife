//

#include "stdafx.h"
#include "PeakUI.h"
#include "HComponent.h"
#include "CommandUIAccel.h"
#include "../resource.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "HCommandPluginManager.h"

const char *SmoothEdgeCommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *SmoothEdgeCommandUIPlugin::GetTitleName() const { return "Smooth"; }
const char *SmoothEdgeCommandUIPlugin::GetDescription() const { return "Make all selected edges, vertices, or faces smooth."; }

UINT SmoothEdgeCommandUIPlugin::GetBitmapID() const { return IDB_SMOOTH; }

static CommandUIAccel f_pSoftAccel [] = 
{
   CommandUIAccel(FVIRTKEY, 'O')
};

void SmoothEdgeCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pSoftAccel)/sizeof(CommandUIAccel);
   pAccel = f_pSoftAccel;
}

void SmoothEdgeCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pPeakEdgeCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Peak");
   if (m_pPeakEdgeCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Peak ).");
}

void SmoothEdgeCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pModelToolBar = pMainFrame->AddToolBar(GetCategory());
   pModelToolBar->AddButton(NULL);//Separator
   pModelToolBar->AddButton(GetPlugin());
}

// Message Maps
void SmoothEdgeCommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   BOOL bEnable = pObject->IsKindOf(SOT_Edge) || pObject->IsKindOf(SOT_Poly) || pObject->IsKindOf(SOT_Vertex) ;

   if (!bEnable && pCmdUI->m_pMenu)
      pCmdUI->m_pMenu->DeleteMenu( pCmdUI->m_nIndex, MF_BYPOSITION );
   else
      pCmdUI->Enable( bEnable );
}

void SmoothEdgeCommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   strOutParameters = "-o SMOOTH";
   pOutPlugin = m_pPeakEdgeCommandPlugin;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char *PeakEdgeCommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *PeakEdgeCommandUIPlugin::GetTitleName() const { return "Peak"; }
const char *PeakEdgeCommandUIPlugin::GetDescription() const { return "Make all selected vertices, edges, and faces peaked."; }

UINT PeakEdgeCommandUIPlugin::GetBitmapID() const { return IDB_PEAK; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FVIRTKEY, 'P')
};

void PeakEdgeCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

void PeakEdgeCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pPeakEdgeCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Peak");
   if (m_pPeakEdgeCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Peak ).");
}

void PeakEdgeCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pModelToolBar = pMainFrame->AddToolBar(GetCategory());
   pModelToolBar->AddButton(GetPlugin());
}

// Message Maps
void PeakEdgeCommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   BOOL bEnable = pObject->IsKindOf(SOT_Edge) || pObject->IsKindOf(SOT_Poly) || pObject->IsKindOf(SOT_Vertex) ;

   if (!bEnable && pCmdUI->m_pMenu)
      pCmdUI->m_pMenu->DeleteMenu( pCmdUI->m_nIndex, MF_BYPOSITION );
   else
      pCmdUI->Enable( bEnable );
}

void PeakEdgeCommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   strOutParameters = "-o PEAK";
   pOutPlugin = m_pPeakEdgeCommandPlugin;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char *TogglePeakEdgeCommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *TogglePeakEdgeCommandUIPlugin::GetTitleName() const { return "Toggle Peak/Smooth"; }
const char *TogglePeakEdgeCommandUIPlugin::GetDescription() const { return "Toggle all selected vertices, edges, and faces from peak/smooth to smooth/peak."; }

UINT TogglePeakEdgeCommandUIPlugin::GetBitmapID() const { return IDB_TOOGLE_PEAK_SMOOTH; }

void TogglePeakEdgeCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pPeakEdgeCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Peak");
   if (m_pPeakEdgeCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Peak ).");
}

void TogglePeakEdgeCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pModelToolBar = pMainFrame->AddToolBar(GetCategory());
   pModelToolBar->AddButton(GetPlugin());
}

// Message Maps
void TogglePeakEdgeCommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   BOOL bEnable = pObject->IsKindOf(SOT_Edge) || pObject->IsKindOf(SOT_Poly) || pObject->IsKindOf(SOT_Vertex) ;

   if (!bEnable && pCmdUI->m_pMenu)
      pCmdUI->m_pMenu->DeleteMenu( pCmdUI->m_nIndex, MF_BYPOSITION );
   else
      pCmdUI->Enable( bEnable );
}

void TogglePeakEdgeCommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   strOutParameters = "-o TOGGLE";
   pOutPlugin = m_pPeakEdgeCommandPlugin;
}