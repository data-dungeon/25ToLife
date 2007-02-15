//

#include "stdafx.h"
#include "StoreValueCommand.h"
#include "HSnipeObjectList.h"
#include "HSelectionList.h"
#include "../resource.h"
#include "HMainInterface.h"
#include "HProperty.h"

CommandInstance *StoreValueCommandPlugin::CreateCommandInstance() { return SNEW StoreValueCommand(this); }


const char *StoreValueCommandPlugin::GetCLIName() const { return "StoreValue"; }; // Name used in scripts and commandline
const char *StoreValueCommandPlugin::GetTitleName() const { return "Store Value"; }

UINT StoreValueCommandPlugin::GetBitmapID() const { return IDB_STOREVALUE; }

String StoreValueCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Sets the value of a property on an object.\n";
   strHelp += (String) "  " + GetCLIName() + " [-t 00:00:00] [-f 0] PROPERTYNAME=VALUE\n";
   strHelp += (String) "    -t  The time at which to set the properties value. This is only used on time base properties.  Time is smpte.\n";
   strHelp += (String) "    -f  The frame at which to set the properties value. This is only used on time base properties.\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName() + " FocalLength=100\n";
   strHelp += (String) "    " + GetCLIName() + " -t 00:00:24 DiffuseColor=1 1 1 0\n";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
StoreValueCommand::~StoreValueCommand()
{
}

UndoType StoreValueCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL StoreValueCommand::OnDo(StringList &strResults)
{
   String strTempValue;
   m_pProperty->GetValueString( strTempValue, m_time );
   m_pProperty->StoreValueFromString( m_strValue, m_time, FALSE );
   m_strValue = strTempValue;

   return TRUE;
}

void StoreValueCommand::OnUndo()
{
   StringList strResult;
   OnDo(strResult);
}

BOOL StoreValueCommand::ParseToken(const char *szToken, TokenList &tokenlist)
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   if (stricmp( szToken, "-t" ) == 0)
   {
      String strTime = tokenlist.GetNext();
      // TODO, set m_time
      return TRUE;
   }
   else if ( szToken[0] == '-')
      return FALSE;

   // Must be Property
   char *szEquals = strrchr( szToken, '=');
   if (szEquals==NULL)
   {
      String strMsg("%1 missing assignment, use a = operator after the property name.");
      strMsg.Replace("%1", (String)szToken);
      THROW_FATAL_ERROR( strMsg );
   }

   String strProperty(szToken, szEquals-szToken);
   szEquals++;
   m_strValue = szEquals;

   char *szNextValue;
   while (szNextValue=tokenlist.GetNext())
      m_strValue+= (String)" " + szNextValue;

   String strError;
   m_pProperty = (HProperty *)GetHMainInterface()->DescriptionToObject( strProperty, &strError )->IsKindOf(SOT_Property);

   if (m_pProperty==NULL)
   {
      String strMsg( "Error in %1 command: %2" );
      strMsg.Replace( "%1", GetCommandPluginHelper()->GetCLIName());
      if (strError.IsEmpty())
         strError = "Property not found.";
      strMsg.Replace( "%2", strError );
      THROW_FATAL_ERROR( strMsg );
   }
   return TRUE;
}

BOOL StoreValueCommand::OnValidate( String &strOutError )
{
   if (!CommandInstance::OnValidate(strOutError))
      return FALSE;

   if (!m_pProperty)
   {
      strOutError = "No property specified.";
      return FALSE;
   }

   return TRUE;
}

