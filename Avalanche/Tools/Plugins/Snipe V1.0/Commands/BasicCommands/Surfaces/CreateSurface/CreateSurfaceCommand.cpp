//

#include "stdafx.h"
#include "CreateSurfaceCommand.h"
#include "HPolyModel.h"
#include "HSurface.h"
#include "HSelectionList.h"
#include "HSnipeObjectList.h"
#include "HMainInterface.h"
#include "../../Resource.h"

CommandInstance *CreateSurfaceCommandPlugin::CreateCommandInstance() { return SNEW CreateSurfaceCommand(this); }

const char *CreateSurfaceCommandPlugin::GetCLIName() const { return "CreateSurface"; }; // Name used in scripts and commandline
const char *CreateSurfaceCommandPlugin::GetTitleName() const { return "Create Surface"; }

UINT CreateSurfaceCommandPlugin::GetBitmapID() const { return IDB_DEFAULT; }

String CreateSurfaceCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Creates a new surface out of the selected faces.";
   strHelp += (String) "  " + GetCLIName() + " [-o OPTIONNAME] -name SURFACENAME\n";
   strHelp += (String) "    -o NEW (Creates a new surface object and stores currently selected faces in it. This option is default.)\n";
   strHelp += (String) "    -o REPLACE (Replace the given surface with currently selected faces.)\n";
   strHelp += (String) "    -o ADD (Add currently selected faces to the given surface.)\n";
   strHelp += (String) "    -o SUBTRACT (Subtract currently selected faces from the given surface.)\n";
   strHelp += (String) "    -o TOGGLE (Toggle currently selected faces in or out of the given surface.)\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName() + " -o REPLACE -name MySurface";
   return strHelp;
}

void CreateSurfaceCommandPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pSelectCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "Select");
   if (m_pSelectCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : Select ).");
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CreateSurfaceCommand::CreateSurfaceCommand(HCommandPluginHelper *pPlugin) :
   CommandInstance(pPlugin)
{
   m_pSurface = NULL;
   m_operation = OP_NEW;
   m_pSurfaceDeleteInfo = NULL;
   m_pSelectInstance = NULL;
}

CreateSurfaceCommand::~CreateSurfaceCommand()
{
   if (m_pSurfaceDeleteInfo)
      delete m_pSurfaceDeleteInfo;
   if (m_pSelectInstance)
      delete m_pSelectInstance;
}

BOOL CreateSurfaceCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   if (stricmp( szToken, "-name" ) == 0)
   {
      m_strName = tokenlist.GetNext();
      m_strName.RemoveChar('\"');
      return TRUE;
   }
   if (stricmp( szToken, "-o" ) == 0)
   {
      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute CreateSurface Command, invalid option provided.");
      if (strcmp(szNextToken, "NEW")==0)
         m_operation = OP_NEW;
      else if (strcmp(szNextToken, "REPLACE")==0)
         m_operation = OP_REPLACE;
      else if (strcmp(szNextToken, "ADD")==0)
         m_operation = OP_ADD;
      else if (strcmp(szNextToken, "SUBTRACT")==0)
         m_operation = OP_SUBTRACT;
      else if (strcmp(szNextToken, "TOGGLE")==0)
         m_operation = OP_TOGGLE;
      else
      {
         String strMsg("Unable to execute CreateSurface Command, invalid option %1.");
         strMsg.Replace("%1", szNextToken );
         THROW_FATAL_ERROR(strMsg);
      }
      return TRUE;
   }

   return FALSE;
}

BOOL CreateSurfaceCommand::OnValidate( String &strOutError )
{
   if (m_strName.IsEmpty())
      m_strName = "Untitled";

   HSnipeObject *pCommandObject = GetHMainInterface()->GetCurrentCommandObject();
   if (!pCommandObject->IsKindOf(SOT_PolyModel))
   {
      strOutError = "Error in CreateSurface command: Must be used on a Model.";
      return FALSE;
   }

   HPolyModel *pPolyModel = (HPolyModel *)pCommandObject;

   if (m_operation != OP_NEW)
   {
      HSurfaceContainer *pSurfaceContainer = pPolyModel->GetSurfaceContainer();
      if (pSurfaceContainer)
         m_pSurface = pSurfaceContainer->FindSurface( m_strName );

      if (!m_pSurface)
      {
         strOutError = "Error in %1 command: Couldn't find \"%2\" surface.";
         strOutError.Replace( "%1", GetCommandPluginHelper()->GetCLIName() );
         strOutError.Replace( "%2", m_strName );
         return FALSE;
      }
   }

   return TRUE;
}

UndoType CreateSurfaceCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL CreateSurfaceCommand::OnDo(StringList &strResults)
{
   HMainInterface *pMI = GetHMainInterface();
   HSnipeObject *pCommandObject = pMI->GetCurrentCommandObject();
   HPolyModel *pPolyModel = (HPolyModel *)pCommandObject;

   if (!m_pSurface)// Must be a New operation
      m_pSurface = pPolyModel->AddSurface( m_strName.Get(), -1, &m_pSurfaceDeleteInfo );

   ASSERT(m_pSurface);

//   if (m_operation != OP_NEW)
//      m_strOldDescription = m_pSurface->GetSelectionDescription();

   if (m_operation == OP_REPLACE)
      m_pSurface->RemoveAllFaces();

   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();
   HSnipeObjectList *pList = pSelectionList->CreateSnipeObjectList();
   pList->ConvertCompatibleObjectsToSOT( SOT_Poly );

   for (POSITION pos=pList->GetHeadPosition(); pos; )
   {
      HPoly *pPoly = (HPoly *)pList->GetNext( pos );

      switch (m_operation)
      {
         case OP_NEW:
         case OP_REPLACE:
         case OP_ADD:
            m_pSurface->AddFace( pPoly );
            break;
         case OP_SUBTRACT:
            m_pSurface->RemoveFace( pPoly );
            break;
         case OP_TOGGLE:
            if (m_pSurface->FindFace( pPoly ) == -1)
               m_pSurface->AddFace( pPoly );
            else
               m_pSurface->RemoveFace( pPoly );
            break;
      }
   }

   delete pList;

   //m_strNewDescription = m_pSurface->GetSelectionDescription();

   String strDescription;
   pMI->ObjectToDescription(m_pSurface, strDescription);
   ASSERT(!strDescription.IsEmpty());

   HCommandPluginHelper *pSelectCommand = (HCommandPluginHelper *)((CreateSurfaceCommandPlugin *)GetCommandPluginHelper())->m_pSelectCommandPlugin->GetPluginData();
   m_pSelectInstance = pSelectCommand->ExecuteInternalCommand( strDescription, strResults );
   ASSERT(m_pSelectInstance);
   return TRUE;
}

void CreateSurfaceCommand::OnRedo()
{
   if (m_pSurfaceDeleteInfo)
      m_pSurfaceDeleteInfo->UnDelete();
   //else
   //   m_pSurface->SetSelectionDescription( m_strNewDescription );
   m_pSelectInstance->OnUndo();
}

void CreateSurfaceCommand::OnUndo()
{
   if (m_pSurfaceDeleteInfo)
      m_pSurfaceDeleteInfo->Delete();
   //else
   //   m_pSurface->SetSelectionDescription( m_strOldDescription );
   m_pSelectInstance->OnRedo();
}
