//

#include "stdafx.h"
#include "CreateModelCommand.h"
#include "HPolyModel.h"
#include "HSelectionList.h"
#include "HSnipeObjectList.h"
#include "../Resource.h"

CommandInstance *CreateModelCommandPlugin::CreateCommandInstance() { return SNEW CreateModelCommand(this); }

const char *CreateModelCommandPlugin::GetCLIName() const { return "CreateModel"; }; // Name used in scripts and commandline
const char *CreateModelCommandPlugin::GetTitleName() const { return "Create Model"; }

UINT CreateModelCommandPlugin::GetBitmapID() const { return IDB_CREATEMODEL; } //IDB_CREATEMODEL

String CreateModelCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Creates a new Model and adds it to the project.";
   strHelp += (String) "  " + GetCLIName() + " [-type MODELTYPE] [-edit] [-name MODELNAME].\n";
   strHelp += (String) "    -name MODELNAME (Name to assign to the model)\n";
   strHelp += (String) "    -type MODELTYPE (Sets the type of model to create, can be one of the following)\n";
   strHelp += (String) "          POLY   (Creates a poly model)\n";
   strHelp += (String) "          PATCH  (Creates a patch model)\n";
   strHelp += (String) "          SPLINE (Creates a bezier spline model, this is default if no MODELTYPE is specified) \n";
   strHelp += (String) "    -edit (Opens an edit window for the new model)\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName() + "\n";
   strHelp += (String) "    " + GetCLIName() + " -t POLY\n";
   return strHelp;
}

void CreateModelCommandPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pSelectCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Select");
   if (m_pSelectCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Select ).");
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CreateModelCommand::CreateModelCommand(HCommandPluginHelper *pPlugin) :
   CommandInstance(pPlugin)
{
   m_nModelType = MT_SPLINE;
   m_pDeleteInfo = NULL;
   m_pModel = NULL;
   m_pSelectInstance = NULL;
   m_bEdit = FALSE;
}

CreateModelCommand::~CreateModelCommand()
{
   if (m_pDeleteInfo)
      delete m_pDeleteInfo;
   if (m_pSelectInstance)
      delete m_pSelectInstance;
}

BOOL CreateModelCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   if (stricmp( szToken, "-name" ) == 0)
   {
      m_strName = tokenlist.GetNext();
      m_strName.RemoveChar('\"');
      return TRUE;
   }
   else if (stricmp( szToken, "-type" ) == 0)
   {
      String strModelType = tokenlist.GetNext();
      if (strModelType == "POLY")
         m_nModelType = MT_POLY;
      else if (strModelType == "PATCH")
         m_nModelType = MT_PATCH;
      else if (strModelType == "SPLINE")
         m_nModelType = MT_SPLINE;
      else
      {
         String strMsg("Unable to execute CreateModel command, Invalid MODELTYPE %1." );
         strMsg.Replace("%1", strModelType);
         THROW_FATAL_ERROR(strMsg);
      }
      return TRUE;
   }
   else if (stricmp( szToken, "-edit" ) == 0)
   {
      m_bEdit = TRUE;
      return TRUE;
   }

   return FALSE;
}

BOOL CreateModelCommand::OnValidate( String &strOutError )
{
   if (!CommandInstance::OnValidate( strOutError ))
      return FALSE;

   HSnipeObject *pCommandObject = GetHMainInterface()->GetCurrentCommandObject();
   if (!pCommandObject->IsKindOf(SOT_Project))
   {
      strOutError = "Error in CreateModel command: Must be used on a Project.";
      return FALSE;
   }

   if (m_strName.IsEmpty())
      m_strName = "Untitled";

   return TRUE;
}

UndoType CreateModelCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL CreateModelCommand::OnDo(StringList &strResults)
{
   HMainInterface *pMI = GetHMainInterface();

   switch(m_nModelType)
   {
   case MT_POLY:
      m_pModel = (HPolyModel *)GetHMainInterface()->CreateObject( SOT_PolyModel, &m_pDeleteInfo );
      break;
   case MT_PATCH:
      m_pModel = (HPolyModel *)GetHMainInterface()->CreateObject( SOT_PatchModel, &m_pDeleteInfo );
      break;
//   case MT_SPLINE:
//      m_pModel = (HPolyModel *)GetHMainInterface()->CreateObject( SOT_BezierSplineModel, &m_pDeleteInfo );
//      break;
   }

   if (!m_pModel)
   {
      ASSERT(FALSE);
      return FALSE;
   }

   m_pModel->SetName( m_strName );

   HProject *pProject = GetHMainInterface()->GetCurrentProject();
   pProject->AddModel( m_pModel );

   String strDescription;
   pMI->ObjectToDescription(m_pModel, strDescription);
   ASSERT(!strDescription.IsEmpty());

   HCommandPluginHelper *pSelectCommand = (HCommandPluginHelper *)((CreateModelCommandPlugin *)GetCommandPluginHelper())->m_pSelectCommandPlugin->GetPluginData();
   m_pSelectInstance = pSelectCommand->ExecuteInternalCommand( strDescription, strResults );
   ASSERT(m_pSelectInstance);

   if (m_bEdit)
      m_pModel->EditObject();

   return TRUE;
}

void CreateModelCommand::OnUndo()
{
   m_pDeleteInfo->Delete();
   m_pSelectInstance->OnUndo();
}

void CreateModelCommand::OnRedo()
{
   m_pDeleteInfo->UnDelete();
   m_pSelectInstance->OnRedo();
}
