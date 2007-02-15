//

#include "stdafx.h"
#include "ScaleCommand.h"
#include "HCBaseDoc.h"
#include "HSelectionList.h"
#include "HPoly.h"
#include "HMainInterface.h"
#include "HSnipeObjectList.h"
#include "ScaleCommand.h"
#include "../Resource.h"

CommandInstance *ScaleCommandPlugin::CreateCommandInstance() { return SNEW ScaleCommand(this); }

const char *ScaleCommandPlugin::GetCLIName() const { return "Scale"; }; // Name used in scripts and commandline
const char *ScaleCommandPlugin::GetTitleName() const { return "Scale"; }

UINT ScaleCommandPlugin::GetBitmapID() const { return IDB_SCALE; }

String ScaleCommandPlugin::GetHelp() const
{
   static String strHelp( "Scales objects and components." );
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

ScaleCommand::ScaleCommand(HCommandPluginHelper *pPlugin) :
   CommandInstance(pPlugin)
{
   m_vAbout.Set( 0.0f );
   m_vScale.Set( 1.0f );
}

ScaleCommand::~ScaleCommand()
{
}

BOOL ScaleCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   if (stricmp( szToken, "-about" ) == 0)
   {
      m_vAbout.x = (float)atof( tokenlist.GetNext() );
      m_vAbout.y = (float)atof( tokenlist.GetNext() );
      m_vAbout.z = (float)atof( tokenlist.GetNext() );
      return TRUE;
   }
   else if (stricmp( szToken, "-scale" ) == 0)
   {
      m_vScale.x = (float)atof( tokenlist.GetNext() );
      m_vScale.y = (float)atof( tokenlist.GetNext() );
      m_vScale.z = (float)atof( tokenlist.GetNext() );
      return TRUE;
   }
   else if (stricmp( szToken, "-x" ) == 0)
   {
      m_vScale.x = (float)atof( tokenlist.GetNext() );
      return TRUE;
   }
   else if (stricmp( szToken, "-y" ) == 0)
   {
      m_vScale.y = (float)atof( tokenlist.GetNext() );
      return TRUE;
   }
   else if (stricmp( szToken, "-z" ) == 0)
   {
      m_vScale.z = (float)atof( tokenlist.GetNext() );
      return TRUE;
   }
   return FALSE;
}

BOOL ScaleCommand::OnValidate( String &strOutError )
{
   m_matrix = TranslateV( m_vAbout );
   m_matrix *= ScaleV( m_vScale );
   m_matrix *= TranslateV( -m_vAbout );

   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();
   if (pSelectionList->IsEmpty())
      THROW_FATAL_ERROR("Nothing selected to scale.");

   return TRUE;
}

UndoType ScaleCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL ScaleCommand::OnDo(StringList &strResults)
{
   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();
   ASSERT(!pSelectionList->IsEmpty());

   for (POSITION pos=pSelectionList->GetHeadPosition(); pos; )
   {
      HSnipeObject *pObject = pSelectionList->GetNext( pos );
      pObject->PreTransform();
   }

   for (POSITION pos=pSelectionList->GetHeadPosition(); pos; )
   {
      HSnipeObject *pObject = pSelectionList->GetNext( pos );
      pObject->Transform( m_matrix );
   }

   return TRUE;
}

void ScaleCommand::OnUndo()
{
   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();
   ASSERT(!pSelectionList->IsEmpty());

   for (POSITION pos=pSelectionList->GetHeadPosition(); pos; )
   {
      HSnipeObject *pObject = pSelectionList->GetNext( pos );
      pObject->PreTransform();
   }

   Matrix44 matrix( m_matrix.Inverse());

   for (POSITION pos=pSelectionList->GetHeadPosition(); pos; )
   {
      HSnipeObject *pObject = pSelectionList->GetNext( pos );
      pObject->Transform( matrix );
   }
}
