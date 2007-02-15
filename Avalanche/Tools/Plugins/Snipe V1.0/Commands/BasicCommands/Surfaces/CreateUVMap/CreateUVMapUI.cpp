//

#include "stdafx.h"
#include "CreateUVMapUI.h"
#include "HCommandPluginManager.h"
#include "HMainInterface.h"
#include "HUVMapSurfaceDriver.h"
#include "CreateUVMapCommand.h"
#include "../../Resource.h"

const char *CreateUVMapCommandUIPlugin::GetCategory() const { return "Surface"; }
const char *CreateUVMapCommandUIPlugin::GetTitleName() const { return "Create UV Map"; }
const char *CreateUVMapCommandUIPlugin::GetDescription() const { return "Creates a new UV Map, and apply it to the selected Surface property."; }

UINT CreateUVMapCommandUIPlugin::GetBitmapID() const { return IDB_DEFAULT; }

void CreateUVMapCommandUIPlugin::Init( HCommandPluginManager *pCommandManager )
{
   // Collect dependencies here
   m_pCreateUVMapCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "CreateUVMap");
   if (m_pCreateUVMapCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : CreateUVMap ).");
}

// Object Message Maps
SnipeObjectType CreateUVMapCommandUIPlugin::GetCommandObjectType() { return SOT_Surface; }

void CreateUVMapCommandUIPlugin::OnUpdateCommandUIObject( const HSnipeObject *pObject, CCmdUI *pCmdUI )
{
   pCmdUI->Enable( TRUE );
}

void CreateUVMapCommandUIPlugin::OnCommandObject( const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters )
{
   pOutPlugin = m_pCreateUVMapCommandPlugin;

   String strSurfaceName;
   GetHMainInterface()->ObjectToDescription( pObject, strSurfaceName );

   strOutParameters = "-p " + strSurfaceName + ".Diffuse";
}
