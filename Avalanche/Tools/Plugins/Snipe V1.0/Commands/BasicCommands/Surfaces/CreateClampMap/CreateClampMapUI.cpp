//

#include "stdafx.h"
#include "CreateClampMapUI.h"
#include "HCommandPluginManager.h"
#include "HMainInterface.h"
#include "HClampMapSurfaceDriver.h"
#include "CreateClampMapCommand.h"
#include "../../Resource.h"

const char *CreateClampMapCommandUIPlugin::GetCategory() const { return "Surface"; }
const char *CreateClampMapCommandUIPlugin::GetTitleName() const { return "Create Clamp Map"; }
const char *CreateClampMapCommandUIPlugin::GetDescription() const { return "Creates a new Clamp Map, and apply it to the selected Surface property."; }

UINT CreateClampMapCommandUIPlugin::GetBitmapID() const { return IDB_DEFAULT; }

void CreateClampMapCommandUIPlugin::Init( HCommandPluginManager *pCommandManager )
{
   // Collect dependencies here
   m_pCreateClampMapCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "CreateClampMap");
   if (m_pCreateClampMapCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : CreateClampMap ).");
}

// Object Message Maps
SnipeObjectType CreateClampMapCommandUIPlugin::GetCommandObjectType() { return SOT_Surface; }

void CreateClampMapCommandUIPlugin::OnUpdateCommandUIObject( const HSnipeObject *pObject, CCmdUI *pCmdUI )
{
   pCmdUI->Enable( TRUE );
}

void CreateClampMapCommandUIPlugin::OnCommandObject( const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters )
{
   pOutPlugin = m_pCreateClampMapCommandPlugin;

   String strSurfaceName;
   GetHMainInterface()->ObjectToDescription( pObject, strSurfaceName );

   strOutParameters = "-p " + strSurfaceName + ".Diffuse";
}
