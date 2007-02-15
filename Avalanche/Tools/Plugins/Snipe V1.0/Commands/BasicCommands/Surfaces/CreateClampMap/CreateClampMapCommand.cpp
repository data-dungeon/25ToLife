//

#include "stdafx.h"
#include "CreateClampMapCommand.h"
#include "HPolyModel.h"
#include "HImageContainer.h"
#include "HImagePointerProperty.h"
#include "HClampMapSurfaceDriver.h"
#include "HSelectionList.h"
#include "HSnipeObjectList.h"
#include "HMainInterface.h"
#include "HProperty.h"
#include "HSurface.h"
#include "../../Resource.h"

CommandInstance *CreateClampMapCommandPlugin::CreateCommandInstance() { return SNEW CreateClampMapCommand(this); }

const char *CreateClampMapCommandPlugin::GetCLIName() const { return "CreateClampMap"; }; // Name used in scripts and commandline
const char *CreateClampMapCommandPlugin::GetTitleName() const { return "Create ClampMap"; }

UINT CreateClampMapCommandPlugin::GetBitmapID() const { return IDB_DEFAULT; }

String CreateClampMapCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Creates a new Clamp Map, and apply it to the selected Surface property.";
   strHelp += (String) "  " + GetCLIName() + " -p SURFACEPROPERTYNAME -i IMAGENAME\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName() + " -p MySurface.Diffuse";
   return strHelp;
}

void CreateClampMapCommandPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pSelectCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Select");
   if (m_pSelectCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Select ).");
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CreateClampMapCommand::CreateClampMapCommand(HCommandPluginHelper *pPlugin) :
   CommandInstance(pPlugin)
{
   m_pSurfaceProperty = NULL;
   m_pImage = NULL;
   m_pClampMap = NULL;
   m_pClampMapDeleteInfo = NULL;
   m_pSelectInstance = NULL;
}

CreateClampMapCommand::~CreateClampMapCommand()
{
   if (m_pClampMapDeleteInfo)
      delete m_pClampMapDeleteInfo;
   if (m_pSelectInstance)
      delete m_pSelectInstance;
}

BOOL CreateClampMapCommand::ParseToken( const char *szToken, TokenList &tokenlist )
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
         THROW_FATAL_ERROR("Unable to execute CreateClampMap command, Surface property not found. " + strError );
      m_pSurfaceProperty = (HProperty *)pSnipeObject;
      return TRUE;
   }
   else if (stricmp( szToken, "-i" ) == 0) // image name
   {
      String strImageName = tokenlist.GetNext();
      String strError;
      HSnipeObject *pSnipeObject = GetHMainInterface()->DescriptionToObject( strImageName, &strError );
      if (!pSnipeObject || !pSnipeObject->IsKindOf(SOT_Image))
         THROW_FATAL_ERROR("Unable to execute CreateClampMap command, Image not found. " + strError );
      m_pImage = (HImage *)pSnipeObject;
      return TRUE;
   }

   return FALSE;
}

BOOL CreateClampMapCommand::OnValidate( String &strOutError )
{
   HMainInterface *pMI = GetHMainInterface();

   if (!m_pSurfaceProperty)
   {
      strOutError = "Unable to execute CreateClampMap command, no Surface property supplied.";
      return FALSE;
   }

   HSurface *pSurface = (HSurface *)m_pSurfaceProperty->GetObject()->IsKindOf( SOT_Surface );
   if (!pSurface)
   {
      strOutError = "Error in CreateClampMap command: property supplied is not on a Surface.";
      return FALSE;
   }

   HSnipeObject *pCommandObject = GetHMainInterface()->GetCurrentCommandObject();
   if (!pCommandObject->IsKindOf(SOT_PolyModel))
   {
      strOutError = "Error in CreateClampMap command: Must be used on a Model.";
      return FALSE;
   }

   if (!m_pImage)
   {
      HImageContainer *pImageContainer = GetHMainInterface()->GetCurrentProject()->GetImageContainer();
      if (pImageContainer && pImageContainer->GetNumImages())
         m_pImage = pImageContainer->GetImageAt( 0 );
   }

   if (m_strName.IsEmpty())
      m_strName = "Clamp Map";

   return TRUE;
}

UndoType CreateClampMapCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL CreateClampMapCommand::OnDo(StringList &strResults)
{
   HMainInterface *pMI = GetHMainInterface();

   m_pClampMap = (HClampMapSurfaceDriver *)pMI->CreateObject( SOT_ClampMapSurfaceDriver, &m_pClampMapDeleteInfo );
   ASSERT(m_pClampMap);

   m_pClampMap->SetName( m_strName );

   if (m_pImage)
      m_pClampMap->GetImage()->StoreValue( m_pImage, FALSE );

   HSurface *pSurface = (HSurface *)m_pSurfaceProperty->GetObject()->IsKindOf( SOT_Surface );
   pSurface->AddSurfaceDriver( m_pClampMap, m_pSurfaceProperty );

   String strDescription;
   pMI->ObjectToDescription(m_pClampMap, strDescription);
   ASSERT(!strDescription.IsEmpty());

   HCommandPluginHelper *pSelectCommand = (HCommandPluginHelper *)((CreateClampMapCommandPlugin *)GetCommandPluginHelper())->m_pSelectCommandPlugin->GetPluginData();
   m_pSelectInstance = pSelectCommand->ExecuteInternalCommand( strDescription, strResults );
   ASSERT(m_pSelectInstance);

   return TRUE;
}

void CreateClampMapCommand::OnUndo()
{
   m_pClampMapDeleteInfo->Delete();
   m_pSelectInstance->OnUndo();
}

void CreateClampMapCommand::OnRedo()
{
   m_pClampMapDeleteInfo->UnDelete();
   m_pSelectInstance->OnRedo();
}
