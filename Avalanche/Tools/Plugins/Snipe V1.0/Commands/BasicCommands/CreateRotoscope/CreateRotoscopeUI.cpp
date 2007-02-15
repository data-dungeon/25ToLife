//

#include "stdafx.h"
#include "CreateRotoscopeUI.h"
#include "HCommandPluginManager.h"
#include "HMainInterface.h"
#include "HRotoscope.h"
#include "CreateRotoscopeCommand.h"
#include "../Resource.h"

const char *CreateRotoscopeCommandUIPlugin::GetCategory() const { return "Modeling"; }
const char *CreateRotoscopeCommandUIPlugin::GetTitleName() const { return "Create Rotoscope"; }
const char *CreateRotoscopeCommandUIPlugin::GetDescription() const { return "Creates a new Rotoscope."; }

UINT CreateRotoscopeCommandUIPlugin::GetBitmapID() const { return IDB_DEFAULT; }

void CreateRotoscopeCommandUIPlugin::Init( HCommandPluginManager *pCommandManager )
{
   // Collect dependencies here
   m_pCreateRotoscopeCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "CreateRotoscope");
   if (m_pCreateRotoscopeCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : CreateRotoscope ).");
}

// Object Message Maps
SnipeObjectType CreateRotoscopeCommandUIPlugin::GetCommandObjectType() { return SOT_HierObject; }

void CreateRotoscopeCommandUIPlugin::OnUpdateCommandUIObject( const HSnipeObject *pObject, CCmdUI *pCmdUI )
{
   SnipeObjectType nSOT = pObject->GetSOT();
   BOOL bEnable = nSOT==SOT_PolyModel || nSOT==SOT_PatchModel /*|| nSOT==SOT_BezierSplineModel*/ || nSOT==SOT_Motion || nSOT==SOT_Scene;

   if (!bEnable && pCmdUI->m_pMenu)
      pCmdUI->m_pMenu->DeleteMenu( pCmdUI->m_nIndex, MF_BYPOSITION );
   else
      pCmdUI->Enable( bEnable );
}

void CreateRotoscopeCommandUIPlugin::OnCommandObject( const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters )
{
   pOutPlugin = m_pCreateRotoscopeCommandPlugin;

   String strObjectName;
   GetHMainInterface()->ObjectToDescription( pObject, strObjectName );

   strOutParameters = "-o " + strObjectName;
}
