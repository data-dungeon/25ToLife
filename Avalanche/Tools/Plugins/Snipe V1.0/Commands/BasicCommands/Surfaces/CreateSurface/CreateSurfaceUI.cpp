//

#include "stdafx.h"
#include "CreateSurfaceUI.h"
#include "HCommandPluginManager.h"
#include "HMainInterface.h"
#include "HSurface.h"
#include "CreateSurfaceDlg.h"
#include "CreateSurfaceCommand.h"

const char *CreateSurfaceCommandUIPlugin::GetCategory() const { return "Surface"; }
const char *CreateSurfaceCommandUIPlugin::GetTitleName() const { return "Create Surface"; }
const char *CreateSurfaceCommandUIPlugin::GetDescription() const { return "Creates a new surface out of the selected faces."; }

UINT CreateSurfaceCommandUIPlugin::GetBitmapID() const { return IDB_DEFAULT; }

void CreateSurfaceCommandUIPlugin::Init( HCommandPluginManager *pCommandManager )
{
   // Collect dependencies here
   m_pCreateSurfaceCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "CreateSurface");
   if (m_pCreateSurfaceCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : CreateSurface ).");
}

// Object Message Maps
SnipeObjectType CreateSurfaceCommandUIPlugin::GetCommandObjectType() { return SOT_Component; }

void CreateSurfaceCommandUIPlugin::OnUpdateCommandUIObject( const HSnipeObject *pObject, CCmdUI *pCmdUI )
{
   pCmdUI->Enable( TRUE );
}

void CreateSurfaceCommandUIPlugin::OnCommandObject( const HSnipeObject *pObject, HCommandPlugin *&pOutPlugin, String &strOutParameters )
{
   pOutPlugin = m_pCreateSurfaceCommandPlugin;

   CCreateSurfaceDlg dlg;
   if (dlg.DoModal() != IDOK)
      return;

   if (dlg.m_nSurface != 0)
   {
      switch (dlg.m_nOperation + 1)
      {
         case CreateSurfaceCommand::OP_REPLACE:
            strOutParameters = "-o REPLACE ";
            break;
         case CreateSurfaceCommand::OP_ADD:
            strOutParameters = "-o ADD ";
            break;
         case CreateSurfaceCommand::OP_SUBTRACT:
            strOutParameters = "-o SUBTRACT ";
            break;
         case CreateSurfaceCommand::OP_TOGGLE:
            strOutParameters = "-o TOGGLE ";
            break;
      }
   }

   if (!dlg.m_strName.IsEmpty())
   {
      strOutParameters += (String)"-name ";
      if (dlg.m_strName.Find(' ') == -1)
         strOutParameters += (LPCTSTR)dlg.m_strName;
      else
         strOutParameters += (String)"\"" + (LPCTSTR)dlg.m_strName + "\"";
   }
}
