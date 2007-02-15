//

#include "stdafx.h"
#include "LoadSelectionUI.h"
#include "HCommandPluginManager.h"
#include "HMainInterface.h"
#include "HSelection.h"
#include "../resource.h"

const char *LoadSelectionBaseCommandUIPlugin::GetCategory() const { return "Select"; }

void LoadSelectionBaseCommandUIPlugin::Init( HCommandPluginManager *pCommandManager )
{
   // Collect dependencies here
   m_pLoadSelectionCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "LoadSelection");
   if (m_pLoadSelectionCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : LoadSelection ).");
}

// Object Message Maps
SnipeObjectType LoadSelectionBaseCommandUIPlugin::GetCommandObjectType() { return SOT_SnipeObject; }

void LoadSelectionBaseCommandUIPlugin::OnUpdateCommandUIObject( const HSnipeObject *pObject, CCmdUI *pCmdUI )
{
   SnipeObjectType nSOT = pObject->GetSOT();
   BOOL bEnable = nSOT==SOT_Selection || nSOT==SOT_Surface || nSOT==SOT_TexCoords || nSOT==SOT_Bone;

   if (!bEnable && pCmdUI->m_pMenu)
      pCmdUI->m_pMenu->DeleteMenu( pCmdUI->m_nIndex, MF_BYPOSITION );
   else
      pCmdUI->Enable( bEnable );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UINT LoadSelectionReplaceCommandUIPlugin::GetBitmapID() const { return IDB_SELECT; }
const char *LoadSelectionReplaceCommandUIPlugin::GetTitleName() const { return "Load Selection (Replace)"; }
const char *LoadSelectionReplaceCommandUIPlugin::GetDescription() const { return "Replace selects the objects that this objects stores."; }

void LoadSelectionReplaceCommandUIPlugin::OnCommandObject( const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters )
{
   pOutPlugin = m_pLoadSelectionCommandPlugin;
   GetHMainInterface()->ObjectToDescription( pObject, strOutParameters );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UINT LoadSelectionAddCommandUIPlugin::GetBitmapID() const { return IDB_SELECT; }
const char *LoadSelectionAddCommandUIPlugin::GetTitleName() const { return "Load Selection (Add)"; }
const char *LoadSelectionAddCommandUIPlugin::GetDescription() const { return "Add selects the objects that this objects stores."; }

void LoadSelectionAddCommandUIPlugin::OnCommandObject( const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters )
{
   pOutPlugin = m_pLoadSelectionCommandPlugin;
   GetHMainInterface()->ObjectToDescription( pObject, strOutParameters );
   strOutParameters = (String)"-o ADD " + strOutParameters;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UINT LoadSelectionSubtractCommandUIPlugin::GetBitmapID() const { return IDB_SELECT; }
const char *LoadSelectionSubtractCommandUIPlugin::GetTitleName() const { return "Load Selection (Subtract)"; }
const char *LoadSelectionSubtractCommandUIPlugin::GetDescription() const { return "Subtract selects the objects that this objects stores."; }

void LoadSelectionSubtractCommandUIPlugin::OnCommandObject( const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters )
{
   pOutPlugin = m_pLoadSelectionCommandPlugin;
   GetHMainInterface()->ObjectToDescription( pObject, strOutParameters );
   strOutParameters = (String)"-o SUBTRACT " + strOutParameters;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UINT LoadSelectionToggleCommandUIPlugin::GetBitmapID() const { return IDB_SELECT; }
const char *LoadSelectionToggleCommandUIPlugin::GetTitleName() const { return "Load Selection (Toggle)"; }
const char *LoadSelectionToggleCommandUIPlugin::GetDescription() const { return "Toggle selects the objects that this objects stores."; }

void LoadSelectionToggleCommandUIPlugin::OnCommandObject( const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters )
{
   pOutPlugin = m_pLoadSelectionCommandPlugin;
   GetHMainInterface()->ObjectToDescription( pObject, strOutParameters );
   strOutParameters = (String)"-o TOGGLE " + strOutParameters;
}
