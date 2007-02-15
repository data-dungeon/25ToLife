//

#include "stdafx.h"
#include "CreateRotoscopeCommand.h"
#include "HPolyModel.h"
#include "HImageContainer.h"
#include "HImagePointerProperty.h"
#include "HRotoscope.h"
#include "HSelectionList.h"
#include "HSnipeObjectList.h"
#include "HMainInterface.h"
#include "HProperty.h"
#include "HSurface.h"
#include "../Resource.h"

CommandInstance *CreateRotoscopeCommandPlugin::CreateCommandInstance() { return SNEW CreateRotoscopeCommand(this); }

const char *CreateRotoscopeCommandPlugin::GetCLIName() const { return "CreateRotoscope"; }; // Name used in scripts and command line
const char *CreateRotoscopeCommandPlugin::GetTitleName() const { return "Create Rotoscope"; }

UINT CreateRotoscopeCommandPlugin::GetBitmapID() const { return IDB_DEFAULT; }

String CreateRotoscopeCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Creates a new Rotoscope.";
   strHelp += (String) "  " + GetCLIName() + " -o OBJECTNAME -i IMAGENAME\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName() + " -o MyModel";
   return strHelp;
}

void CreateRotoscopeCommandPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pSelectCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Select");
   if (m_pSelectCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Select ).");
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CreateRotoscopeCommand::CreateRotoscopeCommand(HCommandPluginHelper *pPlugin) :
   CommandInstance(pPlugin)
{
   m_pObject = NULL;
   m_pImage = NULL;
   m_pRotoscope = NULL;
   m_pRotoscopeDeleteInfo = NULL;
   m_pSelectInstance = NULL;
}

CreateRotoscopeCommand::~CreateRotoscopeCommand()
{
   if (m_pRotoscopeDeleteInfo)
      delete m_pRotoscopeDeleteInfo;
   if (m_pSelectInstance)
      delete m_pSelectInstance;
}

BOOL CreateRotoscopeCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   if (stricmp( szToken, "-name" ) == 0)
   {
      m_strName = tokenlist.GetNext();
      m_strName.RemoveChar('\"');
      return TRUE;
   }
   else if (stricmp( szToken, "-o" ) == 0) // property name
   {
      String strObjectName = tokenlist.GetNext();
      String strError;
      HSnipeObject *pSnipeObject = GetHMainInterface()->DescriptionToObject( strObjectName, &strError );
      if (!pSnipeObject)
         THROW_FATAL_ERROR("Unable to execute CreateRotoscope command, Object property not found. " + strError );
      SnipeObjectType nSOT = pSnipeObject->GetSOT();
      if (nSOT!=SOT_PolyModel && nSOT!=SOT_PatchModel /*&& nSOT!=SOT_BezierSplineModel*/ && nSOT!=SOT_Motion && nSOT!=SOT_Scene)
         THROW_FATAL_ERROR("Unable to execute CreateRotoscope command, Object type not supported." );
      m_pObject = (HHierObject *)pSnipeObject;
      return TRUE;
   }
   else if (stricmp( szToken, "-i" ) == 0) // image name
   {
      String strImageName = tokenlist.GetNext();
      String strError;
      HSnipeObject *pSnipeObject = GetHMainInterface()->DescriptionToObject( strImageName, &strError );
      if (!pSnipeObject || !pSnipeObject->IsKindOf(SOT_Image))
         THROW_FATAL_ERROR("Unable to execute CreateRotoscope command, Image not found. " + strError );
      m_pImage = (HImage *)pSnipeObject;
      return TRUE;
   }

   return FALSE;
}

BOOL CreateRotoscopeCommand::OnValidate( String &strOutError )
{
   if (!m_pObject)
   {
      strOutError = "Unable to execute CreateRotoscope command, no Object supplied.";
      return FALSE;
   }

   HSnipeObject *pCommandObject = GetHMainInterface()->GetCurrentCommandObject();
   if (pCommandObject != m_pObject )
   {
      strOutError = "Error in CreateRotoscope command: Current history list doesn't match the object specified.";
      return FALSE;
   }

   if (!pCommandObject->IsKindOf(SOT_PolyModel))
   {
      strOutError = "Error in CreateRotoscope command: Invalid object type for a rotoscope.";
      return FALSE;
   }

   if (m_strName.IsEmpty())
      m_strName = "Rotoscope";

   if (!m_pImage)
   {
      HImageContainer *pImageContainer = GetHMainInterface()->GetCurrentProject()->GetImageContainer();
      if (pImageContainer && pImageContainer->GetNumImages())
         m_pImage = pImageContainer->GetImageAt( 0 );
   }

   return TRUE;
}

UndoType CreateRotoscopeCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL CreateRotoscopeCommand::OnDo(StringList &strResults)
{
   HMainInterface *pMI = GetHMainInterface();

   m_pRotoscope = (HRotoscope *) GetHMainInterface()->CreateObject( SOT_Rotoscope, &m_pRotoscopeDeleteInfo );
   ASSERT(m_pRotoscope);

   m_pRotoscope->SetName( m_strName );

   if (m_pImage)
      m_pRotoscope->GetImage()->StoreValue( m_pImage, FALSE );

   m_pObject->AddRotoscope( m_pRotoscope );

   String strDescription;
   pMI->ObjectToDescription(m_pRotoscope, strDescription);
   ASSERT(!strDescription.IsEmpty());

   HCommandPluginHelper *pSelectCommand = (HCommandPluginHelper *)((CreateRotoscopeCommandPlugin *)GetCommandPluginHelper())->m_pSelectCommandPlugin->GetPluginData();
   m_pSelectInstance = pSelectCommand->ExecuteInternalCommand( strDescription, strResults );
   ASSERT(m_pSelectInstance);

   return TRUE;
}

void CreateRotoscopeCommand::OnUndo()
{
   m_pRotoscopeDeleteInfo->Delete();
   m_pSelectInstance->OnUndo();
}

void CreateRotoscopeCommand::OnRedo()
{
   m_pRotoscopeDeleteInfo->UnDelete();
   m_pSelectInstance->OnRedo();
}
