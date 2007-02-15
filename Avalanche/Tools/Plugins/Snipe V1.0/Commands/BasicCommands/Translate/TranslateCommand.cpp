//

#include "stdafx.h"
#include "TranslateCommand.h"
#include "HCBaseDoc.h"
#include "HSelectionList.h"
#include "HPoly.h"
#include "HMainInterface.h"
#include "HSnipeObjectList.h"
#include "TranslateCommand.h"
#include "../Resource.h"

CommandInstance *TranslateCommandPlugin::CreateCommandInstance() { return SNEW TranslateCommand(this); }

const char *TranslateCommandPlugin::GetCLIName() const { return "Translate"; }; // Name used in scripts and commandline
const char *TranslateCommandPlugin::GetTitleName() const { return "Translate"; }

UINT TranslateCommandPlugin::GetBitmapID() const { return IDB_TRANSLATE; }

String TranslateCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Translates objects and components.\n";
   strHelp += (String) "  " + GetCLIName() + " [-o OBJECTNAMES] [-x XOFFSET] [-y YOFFSET] [-z ZOFFSET]\n";
   strHelp += (String) "    -o OBJECTNAMES  (optional: if no objects are specified, then the selection list is used.)\n";
   strHelp += (String) "    -x XOFFSET  (optional: floating point value to offset objects on the x axis.)\n";
   strHelp += (String) "    -y YOFFSET  (optional: floating point value to offset objects on the y axis.)\n";
   strHelp += (String) "    -z ZOFFSET  (optional: floating point value to offset objects on the z axis.)\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName() + " -x 1.0\n";
   strHelp += (String) "    " + GetCLIName() + " -x 1.0 -y -1.0 -z 5.0\n";
   strHelp += (String) "    " + GetCLIName() + " -o Vertex(4) -x 1.0 -y 0.0 -z 0.0\n";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

TranslateCommand::TranslateCommand(HCommandPluginHelper *pPlugin) :
   CommandInstance(pPlugin)
{
   m_vDelta.Set( 0.0f );
   m_pSelectionList = NULL;
}

TranslateCommand::~TranslateCommand()
{
}

BOOL TranslateCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   if (stricmp( szToken, "-x" ) == 0)
   {
      m_vDelta.x = (float)atof( tokenlist.GetNext() );
      return TRUE;
   }
   else if (stricmp( szToken, "-y" ) == 0)
   {
      m_vDelta.y = (float)atof( tokenlist.GetNext() );
      return TRUE;
   }
   else if (stricmp( szToken, "-z" ) == 0)
   {
      m_vDelta.z = (float)atof( tokenlist.GetNext() );
      return TRUE;
   }
   else if (stricmp( szToken, "-o" ) == 0)
   {
      String strObjects = tokenlist.GetNext();
      StringList strErrorList;

      HSnipeObjectList *pSnipeObjectList = GetHMainInterface()->DescriptionToObjects( strObjects, strErrorList );

      if (!strErrorList.IsEmpty() || !pSnipeObjectList)
      {
         // Could be more descriptive using the strErrorList
         String strError("Invalid Object(s): %1");
         strError.Replace("%1", strObjects);
         THROW_FATAL_ERROR(strError);
      }

      for (POSITION pos = pSnipeObjectList->GetHeadPosition(); pos;)
      {
         HSnipeObject *pSnipeObject = pSnipeObjectList->GetNext(pos);
         m_objectlist.AddTail(pSnipeObject);
      }

      delete pSnipeObjectList;
      
      return TRUE;
   }
   return FALSE;
}

BOOL TranslateCommand::OnValidate( String &strOutError )
{
   m_matrix.Identity();
   m_matrix.SetTranslation( m_vDelta );

   if (m_objectlist.IsEmpty())
      m_pSelectionList = GetHMainInterface()->GetCurrentSelectionList();

   return TRUE;
}

UndoType TranslateCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL TranslateCommand::IsListEmpty() const
{
   if (m_pSelectionList)
      return m_pSelectionList->IsEmpty();
   else
      return m_objectlist.IsEmpty();
}

POSITION TranslateCommand::GetListHeadPosition()
{
   if (m_pSelectionList)
      return m_pSelectionList->GetHeadPosition();
   else
      return m_objectlist.GetHeadPosition();
}

HSnipeObject *TranslateCommand::GetListNext( POSITION &pos )
{
   if (m_pSelectionList)
      return m_pSelectionList->GetNext(pos);
   else
      return m_objectlist.GetNext(pos);
}

BOOL TranslateCommand::OnDo(StringList &strResults)
{
   if (IsListEmpty())
      return TRUE;

   for (POSITION pos=GetListHeadPosition(); pos; )
   {
      HSnipeObject *pObject = GetListNext( pos );
      pObject->PreTransform();
   }

   for (POSITION pos=GetListHeadPosition(); pos; )
   {
      HSnipeObject *pObject = GetListNext( pos );
      pObject->Transform( m_matrix );
   }

   return TRUE;
}

void TranslateCommand::OnUndo()
{
   if (IsListEmpty())
      return;

   for (POSITION pos=GetListHeadPosition(); pos; )
   {
      HSnipeObject *pObject = GetListNext( pos );
      pObject->PreTransform();
   }

   Matrix44 matrix( m_matrix.Inverse());

   for (POSITION pos=GetListHeadPosition(); pos; )
   {
      HSnipeObject *pObject = GetListNext( pos );
      pObject->Transform( matrix );
   }
}
