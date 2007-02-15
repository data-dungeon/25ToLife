//

#include "stdafx.h"
#include "CreateProjectionMapUI.h"
#include "HCommandPluginManager.h"
#include "HMainInterface.h"
#include "HProjectionMapSurfaceDriver.h"
#include "CreateProjectionMapCommand.h"
#include "../../Resource.h"

const char *CreateProjectionMapCommandUIPlugin::GetCategory() const { return "Surface"; }
const char *CreateProjectionMapCommandUIPlugin::GetTitleName() const { return "Create Projection Map"; }
const char *CreateProjectionMapCommandUIPlugin::GetDescription() const { return "Creates a new Projection Map, and apply it to the selected Surface property."; }

UINT CreateProjectionMapCommandUIPlugin::GetBitmapID() const { return IDB_DEFAULT; }

void CreateProjectionMapCommandUIPlugin::Init( HCommandPluginManager *pCommandManager )
{
   // Collect dependencies here
   m_pCreateProjectionMapCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "CreateProjectionMap");
   if (m_pCreateProjectionMapCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : CreateProjectionMap ).");
}

// Object Message Maps
SnipeObjectType CreateProjectionMapCommandUIPlugin::GetCommandObjectType() { return SOT_Surface; }

void CreateProjectionMapCommandUIPlugin::OnUpdateCommandUIObject( const HSnipeObject *pObject, CCmdUI *pCmdUI )
{
   pCmdUI->Enable( TRUE );
}

void CreateProjectionMapCommandUIPlugin::OnCommandObject( const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters )
{
   pOutPlugin = m_pCreateProjectionMapCommandPlugin;

   String strSurfaceName;
   GetHMainInterface()->ObjectToDescription( pObject, strSurfaceName );

   strOutParameters = "-p " + strSurfaceName + ".Diffuse";
}
