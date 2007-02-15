//

#include "stdafx.h"
#include "SelectContinuousCommand.h"
#include "HSelectionList.h"
#include "HMainInterface.h"
#include "HSnipeObjectList.h"
#include "HEdge.h"
#include "../Resource.h"

CommandInstance *SelectContinuousCommandPlugin::CreateCommandInstance() { return SNEW SelectContinuousCommand(this); }

const char *SelectContinuousCommandPlugin::GetCLIName() const { return "SelectContinuous"; }; // Name used in scripts and commandline
const char *SelectContinuousCommandPlugin::GetTitleName() const { return "Select Continuous"; }

UINT SelectContinuousCommandPlugin::GetBitmapID() const { return IDB_SELECTCONTINUOUS; }

String SelectContinuousCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Selects continuous components of like kind.";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

BOOL SelectContinuousCommand::OnDo(StringList &strResults)
{
   if (!SelectBaseCommand::OnDo(strResults))
      return FALSE;

   HSnipeObjectList *pList = HSnipeObjectList::New();
   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();

   for (POSITION pos = pSelectionList->GetHeadPosition(); pos; )
   {
      HSnipeObject *pSnipeObject = pSelectionList->GetNext( pos );

      if (pSnipeObject->IsKindOf( SOT_Vertex ))
      {
         HSnipeObject *pNearestFocusObject = HSelectionList::GetNearestFocusObject();
         if (pNearestFocusObject->IsKindOf(SOT_Edge))
            GetContinuousVertices( (HEdge *)pNearestFocusObject, pList );
         else
            THROW_FATAL_ERROR("No focus near edge.");
      }
      else if (pSnipeObject->IsKindOf( SOT_Edge ))
         GetContinuousEdges((HEdge *)pSnipeObject, pList);
      else
         ASSERT(FALSE);
   }

   if (!pList->IsEmpty())
      pSelectionList->Add( pList );

   delete pList;

   return TRUE;
}

void GetContinuousVertices( HEdge *pStartEdge, HSnipeObjectList *pList )
{
   HEdge *pCurrEdge = pStartEdge;
   HEdge *pPrevEdge = NULL;

   while (pCurrEdge)
   {
      if (pCurrEdge->GetFirstVertex()->IsVisible())
         pList->AddTail(pCurrEdge->GetFirstVertex());

      if (pCurrEdge->GetSecondVertex()->IsVisible())
         pList->AddTail(pCurrEdge->GetSecondVertex());

      HEdge *pTempEdge = pCurrEdge->GetContinuousEdge( pPrevEdge );
      pPrevEdge = pCurrEdge;
      pCurrEdge = pTempEdge;

      if (pCurrEdge==pStartEdge)
         break;
   }
   if (pCurrEdge==NULL)
      pCurrEdge = pStartEdge;

   pPrevEdge = NULL;

   // Lets walk Previous Now
   while (pCurrEdge)
   {
      if ( pCurrEdge != pStartEdge ) { // Already done above
         if (pCurrEdge->GetFirstVertex()->IsVisible())
            pList->AddTail(pCurrEdge->GetFirstVertex());
         if (pCurrEdge->GetSecondVertex()->IsVisible())
            pList->AddTail(pCurrEdge->GetSecondVertex());
      }

      HEdge *pTempEdge = pCurrEdge->GetContinuousEdge( pPrevEdge, TRUE );
      pPrevEdge = pCurrEdge;
      pCurrEdge = pTempEdge;

      if (pCurrEdge==pStartEdge)
         break;
   }
}

void GetContinuousEdges( HEdge *pStartEdge, HSnipeObjectList *pList )
{
   HEdge *pCurrEdge = pStartEdge;
   HEdge *pPrevEdge = NULL;

   while (pCurrEdge)
   {
      if (pCurrEdge->IsVisible())
         pList->AddTail(pCurrEdge);

      HEdge *pTempEdge = pCurrEdge->GetContinuousEdge( pPrevEdge );
      pPrevEdge = pCurrEdge;
      pCurrEdge = pTempEdge;

      if (pCurrEdge==pStartEdge)
         break;
   }
   if (pCurrEdge==NULL)
      pCurrEdge = pStartEdge;

   pPrevEdge = NULL;

   // Lets walk Previous Now
   while (pCurrEdge)
   {
      if ( pCurrEdge != pStartEdge && pCurrEdge->IsVisible() ) // Already done above
         pList->AddTail(pCurrEdge);

      HEdge *pTempEdge = pCurrEdge->GetContinuousEdge( pPrevEdge, TRUE );
      pPrevEdge = pCurrEdge;
      pCurrEdge = pTempEdge;

      if (pCurrEdge==pStartEdge)
         break;
   }
}
