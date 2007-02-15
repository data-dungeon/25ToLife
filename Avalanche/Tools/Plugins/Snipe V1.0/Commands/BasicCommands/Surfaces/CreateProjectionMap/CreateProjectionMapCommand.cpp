//

#include "stdafx.h"
#include "CreateProjectionMapCommand.h"
#include "HPolyModel.h"
#include "HImageContainer.h"
#include "HImagePointerProperty.h"
#include "HProjectionMapSurfaceDriver.h"
#include "HSelectionList.h"
#include "HSnipeObjectList.h"
#include "HMainInterface.h"
#include "HProperty.h"
#include "HSurface.h"
#include "../../Resource.h"

CommandInstance *CreateProjectionMapCommandPlugin::CreateCommandInstance() { return SNEW CreateProjectionMapCommand(this); }

const char *CreateProjectionMapCommandPlugin::GetCLIName() const { return "CreateProjectionMap"; }; // Name used in scripts and commandline
const char *CreateProjectionMapCommandPlugin::GetTitleName() const { return "Create ProjectionMap"; }

UINT CreateProjectionMapCommandPlugin::GetBitmapID() const { return IDB_DEFAULT; }

String CreateProjectionMapCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Creates a new Projection Map, and apply it to the selected Surface property.";
   strHelp += (String) "  " + GetCLIName() + " -p SURFACEPROPERTYNAME -i IMAGENAME\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName() + " -p MySurface.Diffuse";
   return strHelp;
}

void CreateProjectionMapCommandPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pSelectCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Select");
   if (m_pSelectCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Select ).");
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CreateProjectionMapCommand::CreateProjectionMapCommand(HCommandPluginHelper *pPlugin) :
 CommandInstance(pPlugin)
{
   m_pSurfaceProperty = NULL;
   m_pImage = NULL;
   m_pProjectionMap = NULL;
   m_pProjectionMapDeleteInfo = NULL;
   m_pSelectInstance = NULL;
}

CreateProjectionMapCommand::~CreateProjectionMapCommand()
{
   if (m_pProjectionMapDeleteInfo)
      delete m_pProjectionMapDeleteInfo;

   if (m_pSelectInstance)
      delete m_pSelectInstance;
}

BOOL CreateProjectionMapCommand::ParseToken( const char *szToken, TokenList &tokenlist )
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
         THROW_FATAL_ERROR("Unable to execute CreateProjectionMap command, Surface property not found. " + strError );
      m_pSurfaceProperty = (HProperty *)pSnipeObject;
      return TRUE;
   }
   else if (stricmp( szToken, "-i" ) == 0) // image name
   {
      String strImageName = tokenlist.GetNext();
      String strError;
      HSnipeObject *pSnipeObject = GetHMainInterface()->DescriptionToObject( strImageName, &strError );
      if (!pSnipeObject || !pSnipeObject->IsKindOf(SOT_Image))
         THROW_FATAL_ERROR("Unable to execute CreateProjectionMap command, Image not found. " + strError );
      m_pImage = (HImage *)pSnipeObject;
      return TRUE;
   }

   return FALSE;
}

BOOL CreateProjectionMapCommand::OnValidate( String &strOutError )
{
   HMainInterface *pMI = GetHMainInterface();

   if (!CommandInstance::OnValidate( strOutError ))
      return FALSE;

   if (!m_pSurfaceProperty)
   {
      strOutError = "Unable to execute CreateProjectionMap command, no Surface property supplied. ";
      return FALSE;
   }

   HSurface *pSurface = (HSurface *)m_pSurfaceProperty->GetObject()->IsKindOf( SOT_Surface );
   if (!pSurface)
   {
      strOutError = "Error in CreateProjectionMap command: property supplied is not on a Surface.";
      return FALSE;
   }

   HSnipeObject *pCommandObject = GetHMainInterface()->GetCurrentCommandObject();
   if (!pCommandObject->IsKindOf(SOT_PolyModel))
   {
      strOutError = "Error in CreateProjectionMap command: Must be used on a Model.";
      return FALSE;
   }

   if (!m_pImage)
   {
      HImageContainer *pImageContainer = GetHMainInterface()->GetCurrentProject()->GetImageContainer();
      if (pImageContainer && pImageContainer->GetNumImages())
         m_pImage = pImageContainer->GetImageAt( 0 );
   }

   if (m_strName.IsEmpty())
      m_strName = "Projection Map";

   return TRUE;
}

UndoType CreateProjectionMapCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL CreateProjectionMapCommand::OnDo(StringList &strResults)
{
   HMainInterface *pMI = GetHMainInterface();

   m_pProjectionMap = (HProjectionMapSurfaceDriver *)pMI->CreateObject( SOT_ProjectionMapSurfaceDriver, &m_pProjectionMapDeleteInfo );
   ASSERT(m_pProjectionMap);

   m_pProjectionMap->SetName( m_strName );

   if (m_pImage)
      m_pProjectionMap->GetImage()->StoreValue( m_pImage, FALSE );

   HSurface *pSurface = (HSurface *)m_pSurfaceProperty->GetObject()->IsKindOf( SOT_Surface );
   pSurface->AddSurfaceDriver( m_pProjectionMap, m_pSurfaceProperty );

   String strDescription;
   pMI->ObjectToDescription(m_pProjectionMap, strDescription);
   ASSERT(!strDescription.IsEmpty());
   
   HCommandPluginHelper *pSelectCommand = (HCommandPluginHelper *)((CreateProjectionMapCommandPlugin *)GetCommandPluginHelper())->m_pSelectCommandPlugin->GetPluginData();
   m_pSelectInstance = pSelectCommand->ExecuteInternalCommand( strDescription, strResults );
   ASSERT(m_pSelectInstance);

   return TRUE;
}

void CreateProjectionMapCommand::OnUndo()
{
   m_pProjectionMapDeleteInfo->Delete();
   m_pSelectInstance->OnUndo();
}

void CreateProjectionMapCommand::OnRedo()
{
   m_pProjectionMapDeleteInfo->UnDelete();
   m_pSelectInstance->OnRedo();
}
