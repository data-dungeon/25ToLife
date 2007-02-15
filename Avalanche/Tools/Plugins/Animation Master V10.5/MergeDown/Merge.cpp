// Dan  8/13/2004  \DanLP110\Plugin\HXT\Avalanche\AnimationMaster\MergeDown\Merge.cpp
#include "stdafx.h"
#include "Hxt.h"
#include "MessageR.h"
#include "SDK/HPropert.h"
#include "SDK/HChor.h"
#include "SDK/HAction.h"
#include "SDK/HPropDri.h"
#include "StrClass.h"

BOOL CHxtApp::MergeDown(HAction *action)
{
   HChorAction *choraction = (HChorAction *)action->GetTreeSibling();

   HActionCache *actcache;
   if (action->IsCache())//Chor Actions are both Caches and Instances
      actcache = (HActionCache *)action;
   else
      actcache = (HActionCache *)action->GetCache();

   CopyAction(actcache, choraction, action);

   HChor *chor = choraction->GetChor();

   delete action;
   chor->Dirty();
   chor->CreateFrame(Time(0));

   return TRUE;
}

void CHxtApp::CopyAction(HHashObject *srcparent, HHashObject *dstparent, HAction *action)
{
   HHashObject *src, *dst;
   for (src = (HHashObject *)srcparent->GetTreeProgeny(HTreeObject::SKIPNONE); src; src=(HHashObject *)src->GetTreeSibling(HTreeObject::SKIPNONE)) {
#ifdef _DEBUG
      String temp = src->GetMatchName();
      int hottype = src->GetObjectType();
#endif
      dst = (HHashObject *)dstparent->FindChildByTreeName(src->GetName());

      if (src->IsAnimObjectShortcut()) {
         HAnimObjectShortcut *srcaos = (HAnimObjectShortcut *)src;
         HAnimObjectShortcut *dstaos = (HAnimObjectShortcut *)dst;
         if (dstaos == NULL) {
            dstaos = srcaos->MakeCopy();
            dstparent->InsertChildAtTail(dstaos);
         }
         CopyAction(srcaos, dstaos, action); 
      }
      else if (src->IsPropertyDriver()) {
         HPropertyDriver *srcdriver = (HPropertyDriver *)src;
         HPropertyDriver *dstdriver = (HPropertyDriver *)dst;
         if (dstdriver==NULL) {
            dstdriver = srcdriver->MakeCopy();
            dstparent->InsertChildAtTail(dstdriver);
            dstdriver->Copy(srcdriver); // OK, this is kind of lame because MakeCopy should have done it
            dstdriver->AddSelfAppliedDrivers(); // but many derivatives of PropertyDriver do not do it, like all constraints, and emptydriver derivatives.
         }

         if (dstdriver->IsStorageDriver())
            dstdriver->CopyKeys(srcdriver, action);
         else
            CopyAction(srcdriver, dstdriver, action);
      }
      else {
         String temp = src->GetMatchName();
         int hottype = src->GetObjectType();
         String msg ("%1, hot = %2 not copied\n");
         msg.Replace("%1", temp);
         msg.Replace("%2", (String)hottype);
         OutputDebugString(msg.Get());
      }
   }
}

