//

#include "stdafx.h"
#include "ImportMotionCommand.h"
#include "HSelectionList.h"
#include "HMainInterface.h"
#include "HMotionCurveDriver.h"
#include "HMotionCurve.h"
#include "Vector2.h"
#include "HMotionChannel.h"
#include "HModifiedMotionCurve.h"
#include "HMotion.h"
#include "HSnipeObjectList.h"
#include "../../Resource.h"

CommandInstance *ImportMotionCommandPlugin::CreateCommandInstance() { return SNEW ImportMotionCommand(this); }

const char *ImportMotionCommandPlugin::GetCLIName() const { return "ImportMotion"; }; // Name used in scripts and commandline
const char *ImportMotionCommandPlugin::GetTitleName() const { return "Import Motion"; }

UINT ImportMotionCommandPlugin::GetBitmapID() const { return IDB_DEFAULT; }

String ImportMotionCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Imports a motion file into the project.";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

ImportMotionCommand::~ImportMotionCommand()
{
   if (m_pMotions)
      delete m_pMotions;

   for (POSITION pos=m_MotionDeleteInfos.GetHeadPosition(); pos; )
   {
      HMotion::HDeleteInfo *pMotionDeleteInfo = m_MotionDeleteInfos.GetNext( pos );
      delete pMotionDeleteInfo;
   }
}

BOOL ImportMotionCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   HMainInterface *pMainInterface = GetHMainInterface();

   if (stricmp( szToken, "-f" ) == 0) // filename
   {
      String strFileName = tokenlist.GetNext();
      strFileName.RemoveChar( '\"' );
      m_strFileNames.AddTail( strFileName );
      return TRUE;
   }
   else if (stricmp( szToken, "-e" ) == 0) // edit documents
   {
      m_bEditDocuments = TRUE;
      return TRUE;
   }

   return FALSE;
}

UndoType ImportMotionCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL ImportMotionCommand::OnDo( StringList &strResults )
{
   HProject *pProject = GetHMainInterface()->GetCurrentProject();
   m_pMotions = pProject->ImportMotions( m_strFileNames );

   if (!m_pMotions)
      return FALSE;

   for (POSITION pos=m_pMotions->GetHeadPosition(); pos; )
   {
      HMotion *pMotion = (HMotion *)m_pMotions->GetNext( pos );

      if (m_bEditDocuments)
         pMotion->EditDocument();

      String strDescription;
      GetHMainInterface()->ObjectToDescription( pMotion, strDescription );
      strResults.AddTail( strDescription );

      HMotion::HDeleteInfo *pMotionDeleteInfo = NULL;
      m_MotionDeleteInfos.AddTail( pMotionDeleteInfo );
   }

   return TRUE;
}

void ImportMotionCommand::OnUndo()
{
   for (POSITION posMotion=m_pMotions->GetHeadPosition(), posDeleteInfo=m_MotionDeleteInfos.GetHeadPosition(); posMotion && posDeleteInfo; )
   {
      HMotion *pMotion = (HMotion *)m_pMotions->GetNext( posMotion );
      POSITION oldpos = posDeleteInfo;
      HMotion::HDeleteInfo *pMotionDeleteInfo = m_MotionDeleteInfos.GetNext( posDeleteInfo );
      String strError;
      pMotion->Delete( pMotionDeleteInfo, strError );
      ASSERT( pMotionDeleteInfo );
      m_MotionDeleteInfos.SetAt( oldpos, pMotionDeleteInfo );
   }
}

void ImportMotionCommand::OnRedo()
{
   for (POSITION pos=m_MotionDeleteInfos.GetHeadPosition(); pos; )
   {
      HMotion::HDeleteInfo *pMotionDeleteInfo = m_MotionDeleteInfos.GetNext( pos );
      pMotionDeleteInfo->UnDelete();
      if (m_bEditDocuments)
         ((HMotion *)pMotionDeleteInfo->GetSnipeObject())->EditDocument();
   }
}
