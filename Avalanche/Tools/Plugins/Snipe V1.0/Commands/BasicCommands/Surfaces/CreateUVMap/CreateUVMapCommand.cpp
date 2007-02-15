//

#include "stdafx.h"
#include "CreateUVMapCommand.h"
#include "HPolyModel.h"
#include "HImageContainer.h"
#include "HImagePointerProperty.h"
#include "HUVMapSurfaceDriver.h"
#include "HSelectionList.h"
#include "HSnipeObjectList.h"
#include "HMainInterface.h"
#include "HProperty.h"
#include "HSurface.h"
#include "../../Resource.h"

CommandInstance *CreateUVMapCommandPlugin::CreateCommandInstance() { return SNEW CreateUVMapCommand(this); }

const char *CreateUVMapCommandPlugin::GetCLIName() const { return "CreateUVMap"; }; // Name used in scripts and commandline
const char *CreateUVMapCommandPlugin::GetTitleName() const { return "Create UVMap"; }

UINT CreateUVMapCommandPlugin::GetBitmapID() const { return IDB_DEFAULT; }

String CreateUVMapCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Creates a new UV Map, and apply it to the selected Surface property.";
   strHelp += (String) "  " + GetCLIName() + " -p SURFACEPROPERTYNAME -i IMAGENAME\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName() + " -p MySurface.Diffuse";
   return strHelp;
}

void CreateUVMapCommandPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pSelectCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Select");
   if (m_pSelectCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Select ).");
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CreateUVMapCommand::CreateUVMapCommand(HCommandPluginHelper *pPlugin) :
   CommandInstance(pPlugin)
{
   m_pSurfaceProperty = NULL;
   m_pImage = NULL;
   m_pUVMap = NULL;
   m_pUVMapDeleteInfo = NULL;
   m_pSelectInstance = NULL;
}

CreateUVMapCommand::~CreateUVMapCommand()
{
   if (m_pUVMapDeleteInfo)
      delete m_pUVMapDeleteInfo;
   if (m_pSelectInstance)
      delete m_pSelectInstance;
}

BOOL CreateUVMapCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   if (stricmp( szToken, "-name" ) == 0)
   {
      m_strName = tokenlist.GetNext();
      m_strName.RemoveChar('\"');
      return TRUE;
   }
   else if (stricmp( szToken, "-p" ) == 0) // property name
   {
      String strPropertyName = tokenlist.GetNext();
      String strError;
      HSnipeObject *pSnipeObject = GetHMainInterface()->DescriptionToObject( strPropertyName, &strError );
      if (!pSnipeObject || !pSnipeObject->IsKindOf(SOT_Property))
         THROW_FATAL_ERROR("Unable to execute CreateUVMap command, Surface property not found. " + strError );
      m_pSurfaceProperty = (HProperty *)pSnipeObject;
      return TRUE;
   }
   else if (stricmp( szToken, "-i" ) == 0) // image name
   {
      String strImageName = tokenlist.GetNext();
      String strError;
      HSnipeObject *pSnipeObject = GetHMainInterface()->DescriptionToObject( strImageName, &strError );
      if (!pSnipeObject || !pSnipeObject->IsKindOf(SOT_Image))
         THROW_FATAL_ERROR("Unable to execute CreateUVMap command, Image not found. " + strError );
      m_pImage = (HImage *)pSnipeObject;
      return TRUE;
   }

   return FALSE;
}

BOOL CreateUVMapCommand::OnValidate( String &strOutError )
{
   HMainInterface *pMI = GetHMainInterface();

   if (!m_pSurfaceProperty)
   {
      strOutError = "Unable to execute CreateUVMap command, no Surface property supplied.";
      return FALSE;
   }

   HSurface *pSurface = (HSurface *)m_pSurfaceProperty->GetObject()->IsKindOf( SOT_Surface );
   if (!pSurface)
   {
      strOutError = "Error in CreateUVMap command: property supplied is not on a Surface.";
      return FALSE;
   }

   HSnipeObject *pCommandObject = GetHMainInterface()->GetCurrentCommandObject();
   if (!pCommandObject->IsKindOf(SOT_PolyModel))
   {
      strOutError = "Error in CreateUVMap command: Must be used on a Model.";
      return FALSE;
   }

   if (m_strName.IsEmpty())
      m_strName = "UV Map";

   if (!m_pImage)
   {
      HImageContainer *pImageContainer = GetHMainInterface()->GetCurrentProject()->GetImageContainer();
      if (pImageContainer && pImageContainer->GetNumImages())
         m_pImage = pImageContainer->GetImageAt( 0 );
   }

   return TRUE;
}

UndoType CreateUVMapCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL CreateUVMapCommand::OnDo(StringList &strResults)
{
   HMainInterface *pMI = GetHMainInterface();

   m_pUVMap = (HUVMapSurfaceDriver *)pMI->CreateObject(SOT_UVMapSurfaceDriver);
   ASSERT(m_pUVMap);

   m_pUVMap->SetName( m_strName );

   if (m_pImage)
      m_pUVMap->GetImage()->StoreValue( m_pImage, FALSE );

   HSurface *pSurface = (HSurface *)m_pSurfaceProperty->GetObject()->IsKindOf( SOT_Surface );
   pSurface->AddSurfaceDriver( m_pUVMap, m_pSurfaceProperty );

   String strDescription;
   pMI->ObjectToDescription(m_pUVMap, strDescription);
   ASSERT(!strDescription.IsEmpty());

   HCommandPluginHelper *pSelectCommand = (HCommandPluginHelper *)((CreateUVMapCommandPlugin *)GetCommandPluginHelper())->m_pSelectCommandPlugin->GetPluginData();
   m_pSelectInstance = pSelectCommand->ExecuteInternalCommand( strDescription, strResults );
   ASSERT(m_pSelectInstance);

   return TRUE;
}

void CreateUVMapCommand::OnUndo()
{
   m_pUVMapDeleteInfo->Delete();
   m_pSelectInstance->OnUndo();
}

void CreateUVMapCommand::OnRedo()
{
   m_pUVMapDeleteInfo->UnDelete();
   m_pSelectInstance->OnRedo();
}
