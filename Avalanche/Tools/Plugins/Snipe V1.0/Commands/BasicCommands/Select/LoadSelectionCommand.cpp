//

#include "stdafx.h"
#include "LoadSelectionCommand.h"
#include "HSelection.h"
#include "HSelectionList.h"
#include "HSnipeObjectList.h"
#include "HMainInterface.h"
#include "../Resource.h"
#include "HSurface.h"
#include "HTexCoords.h"
#include "HPoly.h"
#include "HUVPoly.h"
#include "HBone.h"

CommandInstance *LoadSelectionCommandPlugin::CreateCommandInstance() { return SNEW LoadSelectionCommand(this); }

const char *LoadSelectionCommandPlugin::GetCLIName() const { return "LoadSelection"; }; // Name used in scripts and commandline
const char *LoadSelectionCommandPlugin::GetTitleName() const { return "Load Selection"; }

UINT LoadSelectionCommandPlugin::GetBitmapID() const { return IDB_SELECT; }

String LoadSelectionCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Selects the objects that this objects stores";
   strHelp += (String) "  " + GetCLIName() + " [-o OPTIONNAME] OBJECTNAMES\n";
   strHelp += (String) "    -o REPLACE (Replace current selection with objects. This option is default.)\n";
   strHelp += (String) "    -o ADD (Add objects to current selection.)\n";
   strHelp += (String) "    -o SUBTRACT (Subtract objects from current selection.)\n";
   strHelp += (String) "    -o TOGGLE (Toggle objects to/from current selection.)\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName() + " -o ADD Selection2";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

LoadSelectionCommand::LoadSelectionCommand(HCommandPluginHelper *pPlugin) :
   SelectBaseCommand(pPlugin)
{
   m_nSelectionType = REPLACE;
   m_pObjectList = NULL;
   m_pSelectionList = NULL;
}

LoadSelectionCommand::~LoadSelectionCommand()
{
   if (m_pObjectList)
      delete m_pObjectList;
   if (m_pSelectionList)
      delete m_pSelectionList;
}

BOOL LoadSelectionCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (SelectBaseCommand::ParseToken( szToken, tokenlist ))
      return TRUE;

   if (stricmp( szToken, "-o" ) == 0)
   {
      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute LoadSelection Command, invalid option provided.");
      if (strcmp(szNextToken, "REPLACE")==0)
         m_nSelectionType = REPLACE;
      else if (strcmp(szNextToken, "ADD")==0)
         m_nSelectionType = ADD;
      else if (strcmp(szNextToken, "SUBTRACT")==0)
         m_nSelectionType = SUBTRACT;
      else if (strcmp(szNextToken, "TOGGLE")==0)
         m_nSelectionType = TOGGLE;
      else
      {
         String strMsg("Unable to execute LoadSelection Command, invalid option %1.");
         strMsg.Replace("%1", szNextToken );
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
   m_pObjectList = GetHMainInterface()->DescriptionToObjects( strObjects, strErrorList );

   ASSERT(m_pObjectList);

   if (!strErrorList.IsEmpty())
   {
      for (POSITION pos = strErrorList.GetHeadPosition(); pos; )
      {
         String &strError = strErrorList.GetNext(pos);
         String strMsg("Unable to execute LoadSelection command: %1.");
         strMsg.Replace("%1", strError);
         DisplayWarning( strMsg );
      }
   }

   for (POSITION pos=m_pObjectList->GetHeadPosition(); pos; )
   {
      HSnipeObject *pSnipeObject = m_pObjectList->GetNext( pos );
      switch( pSnipeObject->GetSOT() )
      {
         case SOT_Selection:
         case SOT_Surface:
         case SOT_TexCoords:
         case SOT_Bone:
            break;
         default:
         {
            String strMsg("Unable to execute LoadSelection command, invalid object: %1.");
            strMsg.Replace("%1", pSnipeObject->GetTitle());
            THROW_FATAL_ERROR(strMsg);
            break;
         }
      }
   }

   if (m_pObjectList->GetSize()==0)
   {
      String strMsg("Unable to execute LoadSelection command, No objects specified.");
      THROW_FATAL_ERROR(strMsg);
   }

   return TRUE;
}

BOOL LoadSelectionCommand::OnDo(StringList &strResults)
{
   SelectBaseCommand::OnDo(strResults);

   if (m_pSelectionList==NULL) // Not a redo
   {
      m_pSelectionList = HSnipeObjectList::New();

      for (POSITION pos = m_pObjectList->GetHeadPosition(); pos; )
      {
         HSnipeObject *pObject = m_pObjectList->GetNext( pos );
         switch(pObject->GetSOT())
         {
            case SOT_Selection:
               LoadSelection( (HSelection *)pObject );
               break;
            case SOT_Surface:
               LoadSelection( (HSurface *)pObject );
               break;
            case SOT_TexCoords:
               LoadSelection( (HTexCoords *)pObject );
               break;
            case SOT_Bone:
               LoadSelection( (HBone *)pObject );
               break;
         }
      }
   }

   m_pSelectionList->ConvertCompatibleObjectsToSOT(GetHMainInterface()->GetCurrentSelectionSOT());

   for (POSITION pos = m_pSelectionList->GetHeadPosition(); pos; )
   {
      POSITION oldpos = pos;
      HSnipeObject *pObject = m_pSelectionList->GetNext(pos);
      if (!pObject->IsVisible())
         m_pSelectionList->RemoveAt(oldpos);
   }

   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();

   if (m_nSelectionType == ADD)
      pSelectionList->Add(m_pSelectionList);
   else if (m_nSelectionType == SUBTRACT)
      pSelectionList->Remove(m_pSelectionList);
   else if (m_nSelectionType == TOGGLE)
      pSelectionList->Toggle(m_pSelectionList);
   else if (m_nSelectionType == REPLACE)
      pSelectionList->SetTo(m_pSelectionList);

   return TRUE;
}

void LoadSelectionCommand::LoadSelection( HSelection *pSelection )
{
   String strDecription =  pSelection->GetSelectionDescription();
   StringList strErrorList;

   HSnipeObjectList *pList = GetHMainInterface()->DescriptionToObjects( strDecription, strErrorList );

   ASSERT(pList);

   if (!strErrorList.IsEmpty())
   {
      for (POSITION pos = strErrorList.GetHeadPosition(); pos; )
      {
         String &strError = strErrorList.GetNext(pos);
         String strMsg("Error loading selection: %1");
         strMsg.Replace("%1", strError);
         DisplayWarning( strMsg );
      }
   }

   for (POSITION pos = pList->GetHeadPosition(); pos; )
   {
      HSnipeObject *pObject = pList->GetNext( pos );
      m_pSelectionList->AddTail( pObject );
   }

   delete pList;
}

void LoadSelectionCommand::LoadSelection( HSurface *pSurface )
{
   SnipeObjectType nSOT = GetHMainInterface()->GetCurrentSelectionSOT();
   for (UINT i=0; i<pSurface->GetNumFaces(); i++)
   {
      HPoly *pPoly = pSurface->GetFaceAt( i );
      m_pSelectionList->AddTail( pPoly );
   }
}

void LoadSelectionCommand::LoadSelection( HPropertySet *pPropertySet )
{
   for (POSITION pos = pPropertySet->GetHeadPropertyPolyPosition(); pos; )
   {
      HPropertyPoly *pPropertyPoly = pPropertySet->GetNextPropertyPoly(pos);
      if (pPropertyPoly->GetPropertySet()->GetSOT() != SOT_TexCoords)
         continue;
      HUVPoly *pUVPoly = (HUVPoly*)pPropertyPoly;
      m_pSelectionList->AddTail( pUVPoly->GetPoly() );
   }
}

void LoadSelectionCommand::LoadSelection( HBone *pBone )
{
   for (POSITION pos = pBone->GetVertexHeadPosition(); pos; )
   {
      HVertex *pVertex = pBone->GetNextVertex(pos);
      m_pSelectionList->AddTail( pVertex );
   }
}
