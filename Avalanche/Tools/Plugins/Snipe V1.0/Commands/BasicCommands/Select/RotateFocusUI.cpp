//

#include "stdafx.h"
#include "RotateFocusUI.h"
#include "CommandUIAccel.h"
#include "HMainInterface.h"
#include "HSelectionList.h"
#include "HCommandPluginManager.h"
#include "HToolBarControl.h"
#include "HCMainFrame.h"
#include "../Resource.h"

const char *RotateFocusPrevCommandUIPlugin::GetCategory() const { return "Select"; }
const char *RotateFocusPrevCommandUIPlugin::GetTitleName() const { return "Rotate to previous near focus object"; }
const char *RotateFocusPrevCommandUIPlugin::GetDescription() const { return "Rotate selection's near focus object to the previous one counter clockwise."; }

UINT RotateFocusPrevCommandUIPlugin::GetBitmapID() const { return IDB_ROTATEFOCUSPREV; }

static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FNOINVERT, '[')
};

void RotateFocusPrevCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}

void RotateFocusPrevCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pRotateFocusCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "RotateFocus");
   if (m_pRotateFocusCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Rotate Focus ).");
}

void RotateFocusPrevCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pToolBar = pMainFrame->AddToolBar(GetCategory());
   pToolBar->AddButton(GetPlugin());
}

// Message Maps
void RotateFocusPrevCommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
   pCmdUI->Enable( pObject->IsKindOf(SOT_Vertex) || pObject->IsKindOf(SOT_Edge) || pObject->IsKindOf(SOT_Poly) );
}

void RotateFocusPrevCommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   pOutPlugin = m_pRotateFocusCommandPlugin;
   strOutParameters = "-p";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char *RotateFocusNextCommandUIPlugin::GetCategory() const { return "Select"; }
const char *RotateFocusNextCommandUIPlugin::GetTitleName() const { return "Rotate to next near focus object"; }
const char *RotateFocusNextCommandUIPlugin::GetDescription() const { return "Rotate selection's near focus object to the next one counter clockwise."; }

UINT RotateFocusNextCommandUIPlugin::GetBitmapID() const { return IDB_ROTATEFOCUSNEXT; }

static CommandUIAccel f_pNextAccel [] = 
{
   CommandUIAccel(FNOINVERT, ']')
};

void RotateFocusNextCommandUIPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pNextAccel)/sizeof(CommandUIAccel);
   pAccel = f_pNextAccel;
}

void RotateFocusNextCommandUIPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pRotateFocusCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "RotateFocus");
   if (m_pRotateFocusCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Rotate Focus ).");
}

void RotateFocusNextCommandUIPlugin::Place(HCMainFrame *pMainFrame) 
{
   HCGLToolBarControl *pToolBar = pMainFrame->AddToolBar(GetCategory());
   pToolBar->AddButton(GetPlugin());
   pToolBar->AddButton(NULL);//SEPARATOR
}

// Message Maps
void RotateFocusNextCommandUIPlugin::OnUpdateCommandUIObject(const HSnipeObject *pObject, CCmdUI *pCmdUI)
{
//   pCmdUI->Enable( pObject->IsKindOf(SOT_Vertex) || pObject->IsKindOf(SOT_Edge) || pObject->IsKindOf(SOT_Poly) );
   BOOL bEnable = HSelectionList::GetNearestFocusObject()!=NULL;
   pCmdUI->Enable( bEnable );
}

void RotateFocusNextCommandUIPlugin::OnCommandObject(const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters)
{
   pOutPlugin = m_pRotateFocusCommandPlugin;
}

