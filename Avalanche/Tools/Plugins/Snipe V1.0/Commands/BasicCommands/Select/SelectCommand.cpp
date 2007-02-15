//

#include "stdafx.h"
#include "SelectCommand.h"
#include "../resource.h"
#include "HMainInterface.h"
#include "HSnipeObjectList.h"
#include "HCBaseDoc.h"
#include "HSelectionList.h"
#include "HEdge.h"
#include "HPoly.h"

CommandInstance *SelectCommandPlugin::CreateCommandInstance() { return SNEW SelectCommand(this); }

const char *SelectCommandPlugin::GetTitleName() const { return "Select"; }
const char *SelectCommandPlugin::GetCLIName() const { return "Select"; }; // Name used in scripts and commandline

UINT SelectCommandPlugin::GetBitmapID() const { return IDB_SELECT; }

// The options need converted to TypeProperty
String SelectCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Selects a list of Objects and/or Components.\n";
   strHelp += (String) "  " + GetCLIName() + " [-o OPTIONNAME] [-f FOCUSOBJECTNAME] [-n NEAROBJECTNAME] [OBJECTNAME] [OBJECTNAME.COMPONENTTYPE( [COMPONENTID][,][COMPONENTID[:]COMPONENTID] )]\n";
   strHelp += (String) "    -o REPLACE (Replace current selection with objects. This option is default.)\n";
   strHelp += (String) "    -o ADD (Add objects to current selection.)\n";
   strHelp += (String) "    -o SUBTRACT (Subtract objects from current selection.)\n";
   strHelp += (String) "    -o TOGGLE (Toggle objects to/from current selection.)\n";
   strHelp += (String) "    -o CLEAR (Clear current selection.\n";
   strHelp += (String) "    -f Sets the focus object.  If the focus object is not selected it will get selected.\n";
   strHelp += (String) "    -n Sets the near object for the focus object.  Only valid for Vertices, Edges, and Faces.\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName() + " mymodel1 mymodel2\n";
   strHelp += (String) "    " + GetCLIName() + " -o ADD mymodel1.vertex(0,3,8:18)\n";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

SelectCommand::~SelectCommand()
{
   if (m_pickedobjects)
      delete m_pickedobjects;
}

BOOL SelectCommand::ParseToken(const char *szToken, TokenList &tokenlist )
{
   if (SelectBaseCommand::ParseToken( szToken, tokenlist ))
      return TRUE;

   HMainInterface *pMI = GetHMainInterface();
   ASSERT(pMI);

   if (stricmp( szToken, "-o" ) == 0)
   {
      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute Select Command, invalid option provided.");
      if (strcmp(szNextToken, "REPLACE")==0)
         m_nSelectionType = REPLACE;
      else if (strcmp(szNextToken, "ADD")==0)
         m_nSelectionType = ADD;
      else if (strcmp(szNextToken, "SUBTRACT")==0)
         m_nSelectionType = SUBTRACT;
      else if (strcmp(szNextToken, "TOGGLE")==0)
         m_nSelectionType = TOGGLE;
      else if (strcmp(szNextToken, "CLEAR")==0)
         m_nSelectionType = CLEAR;
      else
      {
         String strMsg("Unable to execute Select Command, invalid option %1.");
         strMsg.Replace("%1", szNextToken );
         THROW_FATAL_ERROR(strMsg);
      }
      return TRUE;
   }
   else if (stricmp( szToken, "-f" ) == 0)
   {
      char *szFocusObject = tokenlist.GetNext();
      String strError;
      m_pFocusObject = pMI->DescriptionToObject(szFocusObject, &strError);
      if (m_pFocusObject == NULL)
      {
         String strMsg("Unable to execute Select command, invalid focus object: %1.");
         strMsg.Replace("%1", strError);
         THROW_FATAL_ERROR(strMsg);
      }
      return TRUE;
   }
   else if (stricmp( szToken, "-n" ) == 0)
   {
      char *szNearestFocusObject = tokenlist.GetNext();
      String strError;
      m_pNearestFocusObject = (HEdge *)pMI->DescriptionToObject(szNearestFocusObject, &strError);
      if (m_pNearestFocusObject == NULL)
      {
         String strMsg("Unable to execute Select command, invalid nearest edge: %1.");
         strMsg.Replace("%1", strError);
         THROW_FATAL_ERROR(strMsg);
      }
      return TRUE;
   }

   if (szToken[0]=='-')
      return FALSE;

   // Must be object list
   String strObjects(szToken);
   char *szNextValue;
   while (szNextValue=tokenlist.GetNext())
      strObjects+= (String)" " + szNextValue;

   StringList strErrorList;
   m_pickedobjects = pMI->DescriptionToObjects( strObjects, strErrorList );

   ASSERT( m_pickedobjects );

   if (!strErrorList.IsEmpty())
   {
      for (POSITION pos = strErrorList.GetHeadPosition(); pos; )
      {
         String &strError = strErrorList.GetNext(pos);
         String strMsg("Unable to select object: %1.");
         strMsg.Replace("%1", strError);
         DisplayWarning( strError );
      }
   }

   if (m_pickedobjects->GetSize()==1 && (m_nSelectionType==REPLACE || m_nSelectionType==ADD))
      m_pFocusObject = m_pickedobjects->GetHead();

   return TRUE;
}

BOOL SelectCommand::OnValidate( String &strOutError )
{
   return TRUE;
}

BOOL SelectCommand::OnDo(StringList &strResults)
{
   if (!SelectBaseCommand::OnDo(strResults))
      return FALSE;

   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();

   if (m_nSelectionType == ADD)
      pSelectionList->Add(m_pickedobjects);
   else if (m_nSelectionType == SUBTRACT)
      pSelectionList->Remove(m_pickedobjects);
   else if (m_nSelectionType == TOGGLE)
      pSelectionList->Toggle(m_pickedobjects);
   else if (m_nSelectionType == REPLACE)
      pSelectionList->SetTo(m_pickedobjects);
   else if (m_nSelectionType == CLEAR)
      pSelectionList->RemoveAll();

   if (!pSelectionList->SetFocusObject(m_pFocusObject, m_pNearestFocusObject))
      THROW_FATAL_ERROR("Invalidate focus object or near focus object specified.");

   return TRUE;
}
