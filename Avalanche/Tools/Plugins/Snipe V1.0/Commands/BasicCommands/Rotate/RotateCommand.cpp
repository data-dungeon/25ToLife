//

#include "stdafx.h"
#include "RotateCommand.h"
#include "HCBaseDoc.h"
#include "HSelectionList.h"
#include "HPoly.h"
#include "HMainInterface.h"
#include "HSnipeObjectList.h"
#include "RotateCommand.h"
#include "../Resource.h"

CommandInstance *RotateCommandPlugin::CreateCommandInstance() { return SNEW RotateCommand(this); }

const char *RotateCommandPlugin::GetCLIName() const { return "Rotate"; }; // Name used in scripts and commandline
const char *RotateCommandPlugin::GetTitleName() const { return "Rotate"; }

UINT RotateCommandPlugin::GetBitmapID() const { return IDB_ROTATE; }

String RotateCommandPlugin::GetHelp() const
{
   static String strHelp( "Rotates objects and components." );
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

RotateCommand::RotateCommand(HCommandPluginHelper *pPlugin) :
   CommandInstance(pPlugin)
{
   m_vAbout.Set( 0.0f );
   m_vAxis.Set( 0.0f ); // not set
   m_fAngle = 0.0f;
   m_bEuler = FALSE;
}

RotateCommand::~RotateCommand()
{
}

BOOL RotateCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   if (stricmp( szToken, "-about" ) == 0)
   {
      m_vAbout.x = (float)atof( tokenlist.GetNext() );
      m_vAbout.y = (float)atof( tokenlist.GetNext() );
      m_vAbout.z = (float)atof( tokenlist.GetNext() );
   }
   else if (stricmp( szToken, "-euler" ) == 0)
   {
      m_euler.x = (float)atof( tokenlist.GetNext() );
      m_euler.y = (float)atof( tokenlist.GetNext() );
      m_euler.z = (float)atof( tokenlist.GetNext() );
      m_bEuler = TRUE;
   }
   else if (stricmp( szToken, "-axis" ) == 0)
   {
      m_vAxis.x = (float)atof( tokenlist.GetNext() );
      m_vAxis.y = (float)atof( tokenlist.GetNext() );
      m_vAxis.z = (float)atof( tokenlist.GetNext() );
   }
   else if (stricmp( szToken, "-a" ) == 0 || stricmp( szToken, "-angle" ) == 0)
      m_fAngle = (float)atof( tokenlist.GetNext() );
   else if (stricmp( szToken, "-axisx" ) == 0)
      m_vAxis.x = (float)atof( tokenlist.GetNext() );
   else if (stricmp( szToken, "-axisy" ) == 0)
      m_vAxis.y = (float)atof( tokenlist.GetNext() );
   else if (stricmp( szToken, "-axisz" ) == 0)
      m_vAxis.z = (float)atof( tokenlist.GetNext() );
   else if (stricmp( szToken, "-aboutx" ) == 0)
      m_vAxis.x = (float)atof( tokenlist.GetNext() );
   else if (stricmp( szToken, "-abouty" ) == 0)
      m_vAxis.y = (float)atof( tokenlist.GetNext() );
   else if (stricmp( szToken, "-aboutz" ) == 0)
      m_vAxis.z = (float)atof( tokenlist.GetNext() );
   else // you can give an angle without an option (i.e. Rotate -12.5)
   {
      if ( szToken[0] == '-' )
         return FALSE;

      float fAngle = (float)atof( szToken );
      if (fAngle)
         m_fAngle = fAngle;
   }
   return TRUE;
}

BOOL RotateCommand::OnValidate( String &strOutError )
{
   m_matrix = TranslateV( m_vAbout );
   if (m_bEuler)
      m_matrix *= Matrix44( m_euler );
   else
      m_matrix *= RotateV( m_fAngle, m_vAxis );
   m_matrix *= TranslateV( -m_vAbout );

   return TRUE;
}

UndoType RotateCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL RotateCommand::OnDo(StringList &strResults)
{
   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();

   if (m_matrix.IsIdentity() || pSelectionList->IsEmpty())
      return TRUE;

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

void RotateCommand::OnUndo()
{
   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();

   if (m_matrix.IsIdentity() || pSelectionList->IsEmpty())
      return;

   for (POSITION pos=pSelectionList->GetHeadPosition(); pos; )
   {
      HSnipeObject *pObject = pSelectionList->GetNext( pos );
      pObject->PreTransform();
   }

   Matrix44 matrix = m_matrix.Inverse();

   for (POSITION pos=pSelectionList->GetHeadPosition(); pos; )
   {
      HSnipeObject *pObject = pSelectionList->GetNext( pos );
      pObject->Transform( matrix );
   }
}
