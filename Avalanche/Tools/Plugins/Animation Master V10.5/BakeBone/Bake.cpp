// Dan  6/24/2004  \DanLP110\Plugin\HXT\Avalanche\AnimationMaster\MergeDown\Merge.cpp
#include "stdafx.h"
#include "Hxt.h"
#include "Reduce.h"
#include "F_Protos.h"
#include "SDK/HPropDri.h"
#include "SDK/HAction.h"
#include "SDK/HChor.h"
#include "SDK/HProject.h"
#include "SDK/HObject.h"
#include "SDK/HProgres.h"
#include "StrClass.h"

inline BOOL CHxtApp::AbortBake()
{
	if (m_abort)
		return TRUE;
	else if (EscapeKeyDown()) {
		m_abort = TRUE;
		return TRUE;
	}
	else
		return FALSE;
}

void CHxtApp::BakeFrame(HObject *srcobj, HObject *dstobj, Time time )
{
   HTransformProperty *dsttransform, *srctransform;
   dsttransform = dstobj->GetTransform();
   srctransform = srcobj->GetTransform();

   dsttransform->GetRotate()->StoreValue( time, srctransform->GetRotate()->GetValue(), TRUE, FALSE );
   dsttransform->GetTranslate()->StoreValue( time, srctransform->GetTranslate()->GetValue(), FALSE );
   dsttransform->GetScale()->StoreValue( time, srctransform->GetScale()->GetValue(), FALSE );
}

BOOL CHxtApp::BakeBone(HObject *srcobj)
{
   CReduceDialog dlg;

   if (dlg.DoModal() != IDOK)
      return TRUE;

   m_abort = FALSE;

   HObject *dstobj = srcobj->MakeCopy();

   HProgressBar *hprogressbar = GetProgressBar();

   if (hprogressbar) {
      hprogressbar->Initialize();
      CString text;
      text.LoadString(IDS_BAKING);
      hprogressbar->SetMessage1((LPCTSTR)text);
      String name = srcobj->GetName();
      name = name.GetNameOnly();
      hprogressbar->SetMessage2((LPCTSTR)name.Get());
      hprogressbar->SetProgress(0);
   }

   HChor *chor = srcobj->GetChor();
   HProject *project = GetHProject();

   Time steptime;
   steptime.SetFrame( 1.0f, project->GetFPS() );
   Time time(0), totaltime=chor->GetTotalTime()->GetValue();
   Time prevtime = chor->GetTime();

   int totalcount=0;
   int framecount;
   for (framecount=0; time<=totaltime; time=steptime*++framecount )
      totalcount++;

   int currcount = 0;
   time = 0;
   
   for (framecount=0; time<=totaltime; time=steptime*++framecount ) {
      if (AbortBake())
		  break;

      chor->CreateFrame(time);

      BakeFrame( srcobj, dstobj, time );

      if (hprogressbar)
         hprogressbar->SetProgress(fast_ftol_unsigned(((float)++currcount)/totalcount * UNIT));
   }

   chor->SetTime(prevtime);

   //Reduce
   if (dlg.m_errortolerance && dstobj->GetCurrentChorAction() && dstobj->GetCurrentChorAction()->GetChild()) {
      if (hprogressbar)
         hprogressbar->SetMessage1(GetResourceString(IDS_REDUCING).Get());
      ReduceBranch( (HAnimObject *)dstobj->GetCurrentChorAction()->GetChild(), dlg.m_errortolerance );
   }

   ReplaceDrivers(srcobj, dstobj);

   delete dstobj;

   if (hprogressbar)
      hprogressbar->Uninitialize();

   srcobj->SetIsActionDeleted(TRUE);

   project->UpdateAllViews(TRUE);

   return TRUE;
}

BOOL CHxtApp::ReplaceDrivers(HObject *toobj, HObject *fromobj)
{
   HTransformProperty *fromtransform, *totransform;
   fromtransform = fromobj->GetTransform();
   totransform = toobj->GetTransform();

   HAction *fromchoraction = fromobj->GetCurrentChorAction();
   HAction *tochoraction = toobj->GetCurrentChorAction();

   if (fromchoraction==NULL || tochoraction==NULL)
      return FALSE;

   //Lets remove all old drivers
   HPropertyDriver *column, *nextcolumn;
   for (column = toobj->GetTransform()->GetRotate()->GetDriverProperty()->GetHeadDriver(); column; column = nextcolumn) {
      nextcolumn = column->GetNextActionDriver();
      HAction *action = column->GetAction();
      if (action && action->GetObjectType() == HOT_CHORACTION) {
         HPropertyDriver *row, *nextrow;
         for (row = column; row; row = nextrow) {
            nextrow = row->GetNextDriverForAction();
            HPropertyDriver *driver = row->GetDriver();
            if (driver != row)
               delete driver;
            else
               delete row;//shortcut
         }
      }
   }
   for (column = toobj->GetTransform()->GetTranslate()->GetDriverProperty()->GetHeadDriver(); column; column = nextcolumn) {
      nextcolumn = column->GetNextActionDriver();
      HAction *action = column->GetAction();
      if (action && action->GetObjectType() == HOT_CHORACTION) {
         HPropertyDriver *row, *nextrow;
         for (row = column; row; row = nextrow) {
            nextrow = row->GetNextDriverForAction();
            HPropertyDriver *driver = row->GetDriver();
            if (driver != row)
               delete driver;
            else
               delete row;//shortcut
         }
      }
   }
   for (column = toobj->GetTransform()->GetScale()->GetDriverProperty()->GetHeadDriver(); column; column = nextcolumn) {
      nextcolumn = column->GetNextActionDriver();
      HAction *action = column->GetAction();
      if (action && action->GetObjectType() == HOT_CHORACTION) {
         HPropertyDriver *row, *nextrow;
         for (row = column; row; row = nextrow) {
            nextrow = row->GetNextDriverForAction();
            HPropertyDriver *driver = row->GetDriver();
            if (driver != row)
               delete driver;
            else
               delete row;//shortcut
         }
      }
   }

   //Now lets move the drivers from the temp obj to the original obj
   HHashObject *from, *next, *to;
   HAnimObjectShortcut *frommodelbone = (HAnimObjectShortcut *)fromchoraction->FindChildByName("Model Bone");
   HAnimObjectShortcut *tomodelbone;
   if (frommodelbone) {
      tomodelbone = (HAnimObjectShortcut *)tochoraction->FindChildByName("Model Bone");
      if (tomodelbone==NULL) {// dest doesn't have a modelmodelfolder so lets just move the whole folder over
         frommodelbone->MoveWithChildren(tochoraction, NULL, FALSE, TRUE);
         return TRUE;
      }
      else {
         from = frommodelbone;
         to = tomodelbone;
      }
   }
   else {
      from = fromchoraction;
      to = tochoraction;
   }

   for (from = from->GetChild(); from; from=next) {
      UINT type = from->GetObjectType();
      next = (HHashObject *)from->GetTreeSibling(HTreeObject::SKIPNONE);
      if(!from->IsPropertyDriver())
         continue;

      ((HPropertyDriver *)from)->MoveWithChildren(to, NULL, FALSE, TRUE);
   }
   return TRUE;
}
