//

#include "stdafx.h"
#include "CreateEnvironmentMapCommand.h"
#include "HPolyModel.h"
#include "HImageContainer.h"
#include "HImagePointerProperty.h"
#include "HEnvironmentMapSurfaceDriver.h"
#include "HSelectionList.h"
#include "HSnipeObjectList.h"
#include "HMainInterface.h"
#include "HProperty.h"
#include "HSurface.h"
#include "../../Resource.h"

CommandInstance *CreateEnvironmentMapCommandPlugin::CreateCommandInstance() { return SNEW CreateEnvironmentMapCommand(this); }

const char *CreateEnvironmentMapCommandPlugin::GetCLIName() const { return "CreateEnvironmentMap"; }; // Name used in scripts and commandline
const char *CreateEnvironmentMapCommandPlugin::GetTitleName() const { return "Create EnvironmentMap"; }

UINT CreateEnvironmentMapCommandPlugin::GetBitmapID() const { return IDB_DEFAULT; }

String CreateEnvironmentMapCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Creates a new Environment Map, and apply it to the selected Surface property.";
   strHelp += (String) "  " + GetCLIName() + " -p SURFACEPROPERTYNAME -i IMAGENAME\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName() + " -p MySurface.Diffuse";
   return strHelp;
}

void CreateEnvironmentMapCommandPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pSelectCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Select");
   if (m_pSelectCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Select ).");
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CreateEnvironmentMapCommand::CreateEnvironmentMapCommand(HCommandPluginHelper *pPlugin) :
   CommandInstance(pPlugin)
{
   m_pSurfaceProperty = NULL;
   m_pImage = NULL;
   m_pEnvironmentMap = NULL;
   m_pEnvironmentMapDeleteInfo = NULL;
   m_pSelectInstance = NULL;
}

CreateEnvironmentMapCommand::~CreateEnvironmentMapCommand()
{
   if (m_pEnvironmentMapDeleteInfo)
      delete m_pEnvironmentMapDeleteInfo;
   if (m_pSelectInstance)
      delete m_pSelectInstance;
}

BOOL CreateEnvironmentMapCommand::ParseToken( const char *szToken, TokenList &tokenlist )
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
         THROW_FATAL_ERROR("Unable to execute CreateEnvironmentMap command, Surface property not found. " + strError );
      m_pSurfaceProperty = (HProperty *)pSnipeObject;
      return TRUE;
   }
   else if (stricmp( szToken, "-i" ) == 0) // image name
   {
      String strImageName = tokenlist.GetNext();
      String strError;
      HSnipeObject *pSnipeObject = GetHMainInterface()->DescriptionToObject( strImageName, &strError );
      if (!pSnipeObject || !pSnipeObject->IsKindOf(SOT_Image))
         THROW_FATAL_ERROR("Unable to execute CreateEnvironmentMap command, Image not found. " + strError );
      m_pImage = (HImage *)pSnipeObject;
      return TRUE;
   }

   return FALSE;
}

BOOL CreateEnvironmentMapCommand::OnValidate( String &strOutError )
{
   HMainInterface *pMI = GetHMainInterface();

   if (!m_pSurfaceProperty)
   {
      strOutError = "Unable to execute CreateEnvironmentMap command, no Surface property supplied.";
      return FALSE;
   }

   HSurface *pSurface = (HSurface *)m_pSurfaceProperty->GetObject()->IsKindOf( SOT_Surface );
   if (!pSurface)
   {
      strOutError = "Error in CreateEnvironmentMap command: property supplied is not on a Surface.";
      return FALSE;
   }

   HSnipeObject *pCommandObject = GetHMainInterface()->GetCurrentCommandObject();
   if (!pCommandObject->IsKindOf(SOT_PolyModel))
   {
      strOutError = "Error in CreateEnvironmentMap command: Must be used on a Model.";
      return FALSE;
   }

   if (!m_pImage)
   {
      HImageContainer *pImageContainer = GetHMainInterface()->GetCurrentProject()->GetImageContainer();
      if (pImageContainer && pImageContainer->GetNumImages())
         m_pImage = pImageContainer->GetImageAt( 0 );
   }

   if (m_strName.IsEmpty())
      m_strName = "Environment Map";

   return TRUE;
}

UndoType CreateEnvironmentMapCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL CreateEnvironmentMapCommand::OnDo(StringList &strResults)
{
   HMainInterface *pMI = GetHMainInterface();

   m_pEnvironmentMap = (HEnvironmentMapSurfaceDriver *)pMI->CreateObject( SOT_EnvironmentMapSurfaceDriver, &m_pEnvironmentMapDeleteInfo );
   ASSERT(m_pEnvironmentMap);

   m_pEnvironmentMap->SetName( m_strName );

   if (m_pImage)
      m_pEnvironmentMap->GetImage()->StoreValue( m_pImage, FALSE );

   HSurface *pSurface = (HSurface *)m_pSurfaceProperty->GetObject()->IsKindOf( SOT_Surface );
   pSurface->AddSurfaceDriver( m_pEnvironmentMap, m_pSurfaceProperty );

   String strDescription;
   pMI->ObjectToDescription(m_pEnvironmentMap, strDescription);
   ASSERT(!strDescription.IsEmpty());

   HCommandPluginHelper *pSelectCommand = (HCommandPluginHelper *)((CreateEnvironmentMapCommandPlugin *)GetCommandPluginHelper())->m_pSelectCommandPlugin->GetPluginData();
   m_pSelectInstance = pSelectCommand->ExecuteInternalCommand( strDescription, strResults );
   ASSERT(m_pSelectInstance);

   return TRUE;
}

void CreateEnvironmentMapCommand::OnUndo()
{
   m_pEnvironmentMapDeleteInfo->Delete();
   m_pSelectInstance->OnUndo();
}

void CreateEnvironmentMapCommand::OnRedo()
{
   m_pEnvironmentMapDeleteInfo->UnDelete();
   m_pSelectInstance->OnRedo();
}
