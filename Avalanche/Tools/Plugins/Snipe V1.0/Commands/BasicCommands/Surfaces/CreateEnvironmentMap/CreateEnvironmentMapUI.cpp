//

#include "stdafx.h"
#include "CreateEnvironmentMapUI.h"
#include "HCommandPluginManager.h"
#include "HMainInterface.h"
#include "HEnvironmentMapSurfaceDriver.h"
#include "CreateEnvironmentMapCommand.h"
#include "../../Resource.h"

const char *CreateEnvironmentMapCommandUIPlugin::GetCategory() const { return "Surface"; }
const char *CreateEnvironmentMapCommandUIPlugin::GetTitleName() const { return "Create Environment Map"; }
const char *CreateEnvironmentMapCommandUIPlugin::GetDescription() const { return "Creates a new Environment Map, and apply it to the selected Surface property."; }

UINT CreateEnvironmentMapCommandUIPlugin::GetBitmapID() const { return IDB_DEFAULT; }

void CreateEnvironmentMapCommandUIPlugin::Init( HCommandPluginManager *pCommandManager )
{
   // Collect dependencies here
   m_pCreateEnvironmentMapCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "CreateEnvironmentMap");
   if (m_pCreateEnvironmentMapCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : CreateEnvironmentMap ).");
}

// Object Message Maps
SnipeObjectType CreateEnvironmentMapCommandUIPlugin::GetCommandObjectType() { return SOT_Surface; }

void CreateEnvironmentMapCommandUIPlugin::OnUpdateCommandUIObject( const HSnipeObject *pObject, CCmdUI *pCmdUI )
{
   pCmdUI->Enable( TRUE );
}

void CreateEnvironmentMapCommandUIPlugin::OnCommandObject( const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters )
{
   pOutPlugin = m_pCreateEnvironmentMapCommandPlugin;

   String strSurfaceName;
   GetHMainInterface()->ObjectToDescription( pObject, strSurfaceName );

   strOutParameters = "-p " + strSurfaceName + ".Diffuse";
}
