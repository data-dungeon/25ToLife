//

#include "stdafx.h"
#include "ConvertModelCommand.h"
#include "../resource.h"
#include "HSelectionList.h"
#include "HMainInterface.h"
#include "ConvertModelDlg.h"

CommandInstance *ConvertModelCommandPlugin::CreateCommandInstance() { return SNEW ConvertModelCommand(this); }

const char *ConvertModelCommandPlugin::GetCLIName() const { return "ConvertModel"; }; // Name used in scripts and commandline
const char *ConvertModelCommandPlugin::GetTitleName() const { return "Convert Model"; }

UINT ConvertModelCommandPlugin::GetBitmapID() const { return IDB_CONVERT; }

String ConvertModelCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Model conversion.";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

ConvertModelCommand::~ConvertModelCommand()
{
   delete m_pConvertInfo;
}

BOOL ConvertModelCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   HMainInterface *pMainInterface = GetHMainInterface();

   if (stricmp( szToken, "-totype" ) == 0)
   {
      char *szNextValue = tokenlist.GetNext();
//      if (stricmp(szNextValue,"BezierSplineModel")==0)
//      {
//         m_nConvertTo = SOT_BezierSplineModel;
//         return TRUE;
//      }
//      else 
      if (stricmp(szNextValue,"PatchModel")==0)
      {
         m_nConvertTo = SOT_PatchModel;
         return TRUE;
      }
      else if (stricmp(szNextValue,"PolyModel")==0)
      {
         m_nConvertTo = SOT_PolyModel;
         return TRUE;
      }
   }
   else if (stricmp( szToken, "-from" ) == 0)
   {
      char *szNextValue = tokenlist.GetNext();
      if (szNextValue)
      {
         String error;
         HSnipeObject *pSnipeObject = pMainInterface->DescriptionToObject(szNextValue,&error);
         m_pFrom = (HHierObject*)pSnipeObject;
      }
      return TRUE;
   }

   return FALSE;
}

UndoType ConvertModelCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL ConvertModelCommand::OnValidate( String &strOutError )
{
   if (!CommandInstance::OnValidate( strOutError ))
      return FALSE;

   if (!m_pFrom)
      return FALSE;

   if (m_nConvertTo == SOT_Unknown)
      return FALSE;
 
   return TRUE;
}

BOOL ConvertModelCommand::OnDo(StringList &strResults)
{
   m_pConvertInfo = ((HHierObject*)m_pFrom)->ConvertTo(m_nConvertTo);
   return TRUE;
}

void ConvertModelCommand::OnRedo()
{
   m_pConvertInfo->ReConvert();
}

void ConvertModelCommand::OnUndo()
{
   m_pConvertInfo->UnConvert();
}

