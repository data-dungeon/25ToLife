//

#include "stdafx.h"
#include "CreateMotionInstanceCommand.h"
#include "HPolyModel.h"
#include "HImageContainer.h"
#include "HImagePointerProperty.h"
#include "HSelectionList.h"
#include "HSnipeObjectList.h"
#include "HMainInterface.h"
#include "HProperty.h"
#include "HSurface.h"
#include "../../Resource.h"

// CreateMotionInstance -o s__ChickenLittle_CoreBonesOnly -m ..|..|Motions|Chick_Dodgeball_superthrow

CommandInstance *CreateMotionInstanceCommandPlugin::CreateCommandInstance() { return SNEW CreateMotionInstanceCommand(this); }

const char *CreateMotionInstanceCommandPlugin::GetCLIName() const { return "CreateMotionInstance"; }; // Name used in scripts and commandline
const char *CreateMotionInstanceCommandPlugin::GetTitleName() const { return "Create Motion Instance"; }

UINT CreateMotionInstanceCommandPlugin::GetBitmapID() const { return IDB_DEFAULT; }

String CreateMotionInstanceCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Creates an instance of the given Motion, and adds it to the given Scene Object.\n";
   strHelp += (String) "  " + GetCLIName() + " -o SCENEOBJECTNAME -m MOTIONNAME [-name INSTANCENAME] [-s STARTTIME] [-e ENDTIME]\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName() + " -o MyModel -m WalkCycle";
   return strHelp;
}

void CreateMotionInstanceCommandPlugin::Init( HCommandPluginManager *pCommandManager )
{
   // Collect dependencies here
   m_pSelectCommandPlugin = pCommandManager->FindCommandByCLIName( "SNIPEBASICCOMMANDS", "Select" );
   if (m_pSelectCommandPlugin == NULL)
      THROW_FATAL_ERROR( "Missing dependent command ( SnipeBasicCommand : Select )." );
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CreateMotionInstanceCommand::CreateMotionInstanceCommand( HCommandPluginHelper *pPlugin ) :
   CommandInstance( pPlugin )
{
   m_pSceneObject = NULL;
   m_pMotion = NULL;
   m_pMotionInstance = NULL;
   m_pMotionInstanceDeleteInfo = NULL;

   CreateMotionInstanceCommandPlugin *pCreateMotionInstanceCommandPlugin = (CreateMotionInstanceCommandPlugin *)pPlugin;
   HCommandPluginHelper *pSelectCommandPluginHelper = (HCommandPluginHelper *)pCreateMotionInstanceCommandPlugin->m_pSelectCommandPlugin->GetPluginData();
   m_pSelectCommandInstance = pSelectCommandPluginHelper->CreateCommandInstance();
}

CreateMotionInstanceCommand::~CreateMotionInstanceCommand()
{
   if (m_pMotionInstanceDeleteInfo)
   {
      delete m_pMotionInstance;
      delete m_pMotionInstanceDeleteInfo;
   }

   if (m_pSelectCommandInstance)
      delete m_pSelectCommandInstance;
}

void CreateMotionInstanceCommand::OnParse( const String &strParameters )
{
   CommandInstance::OnParse( strParameters );
}

BOOL CreateMotionInstanceCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   if (stricmp( szToken, "-name" ) == 0)
   {
      m_strName = tokenlist.GetNext();
      m_strName.RemoveChar('\"');
      return TRUE;
   }
   else if (stricmp( szToken, "-o" ) == 0) // scene object name
   {
      String strSceneObjectName = tokenlist.GetNext();
      String strError;
      HSnipeObject *pSnipeObject = GetHMainInterface()->DescriptionToObject( strSceneObjectName, &strError );
      if (!pSnipeObject || !pSnipeObject->IsKindOf(SOT_SceneObject))
         THROW_FATAL_ERROR("Unable to execute CreateMotionInstance command, scene object not found. " + strError );
      m_pSceneObject = (HSceneObject *)pSnipeObject;
      return TRUE;
   }
   else if (stricmp( szToken, "-m" ) == 0) // motion name
   {
      String strImageName = tokenlist.GetNext();
      String strError;
      HSnipeObject *pSnipeObject = GetHMainInterface()->DescriptionToObject( strImageName, &strError );
      if (!pSnipeObject)
      {
         // TODO: try to find it in the actions folder
         //HProject *pProject = GetHMainInterface()->GetCurrentProject();
         //pSnipeObject = GetHMainInterface()->DescriptionToObject( strImageName, &strError );
      }
      if (!pSnipeObject || !pSnipeObject->IsKindOf(SOT_Motion))
         THROW_FATAL_ERROR("Unable to execute CreateMotionInstance command, Motion not found. " + strError );
      m_pMotion = (HMotion *)pSnipeObject;
      return TRUE;
   }

   return FALSE;
}

BOOL CreateMotionInstanceCommand::OnValidate( String &strOutError )
{
   if (!m_pSceneObject)
   {
      strOutError = "Error in CreateMotionInstance command: No Scene Object provided.";
      return FALSE;
   }
   if (!m_pMotion)
   {
      strOutError = "Error in CreateMotionInstance command: No Motion provided.";
      return FALSE;
   }

   m_pMotionInstance = m_pMotion->CreateInstance();
   if (!m_pMotionInstance)
   {
      strOutError = "Error in CreateMotionInstance command: Couldn't create a new Motion instance.";
      return FALSE;
   }

   if (!m_strName.IsEmpty())
      m_pMotionInstance->SetName( m_strName );

   m_pSceneObject->AddMotionInstance( m_pMotionInstance );

   String strMotionInstanceName;
   GetHMainInterface()->ObjectToDescription( m_pMotionInstance, strMotionInstanceName );
   m_pSelectCommandInstance->OnParse( strMotionInstanceName );

   return TRUE;
}

UndoType CreateMotionInstanceCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL CreateMotionInstanceCommand::OnDo(StringList &strResults)
{
   return m_pSelectCommandInstance->OnDo( strResults );
}

void CreateMotionInstanceCommand::OnUndo()
{
   m_pSelectCommandInstance->OnUndo();

   String strError;
   ASSERT( m_pMotionInstanceDeleteInfo==NULL );
   m_pMotionInstance->Delete( m_pMotionInstanceDeleteInfo, strError );
}

void CreateMotionInstanceCommand::OnRedo()
{
   m_pMotionInstanceDeleteInfo->UnDelete();
   delete m_pMotionInstanceDeleteInfo;
   m_pMotionInstanceDeleteInfo = NULL;

   m_pSelectCommandInstance->OnRedo();
}
