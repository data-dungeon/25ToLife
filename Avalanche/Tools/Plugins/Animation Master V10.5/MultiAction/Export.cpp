// Dan  8/16/2004  \DanLP110\Plugin\HXT\KFM\Export.cpp

#include "stdafx.h"
#include "Hxt.h"
#include "HashFile.h"
#include "Resource.h"
#include "F_Protos.h"
#include "Vector2.h"
#include "ExportActionDialog.h"
#include "ExportActionsDialog.h"
#include "ExportMultiActionDialog.h"

#include "SDK\HChor.h"
#include "SDK\HModel.h"
#include "SDK\HAction.h"
#include "SDK\HProgres.h"
#include "SDK\HPropDri.h"
#include "SDK\Misc.h"
#include "SDK\HProject.h"

BOOL CHxtApp::OnExportActionCache(HActionCache *hac)
{
   HModelCache *hmc = hac->GetDefaultModelCache();
   if (hmc==NULL) {
      CString msg, title;
      msg.LoadString(IDS_ACTIONHASNOMODELASSIGNED);
      String temp = (LPCTSTR)msg;
      temp.Replace("%1", hac->GetName());
      msg = temp.Get();
      title.LoadString(IDS_EXPORTTITLE);

      MessageBox(GetMainApplicationWnd(), msg, title, MB_OK);
      return FALSE;
   }

   CExportActionDialog dlg(hac);
   if (dlg.DoModal()!=IDOK)
      return TRUE;

   return ExportKFM(dlg.m_filename, dlg.m_errortolerance, hac, hmc);
}

//*********************************************************************************

BOOL CHxtApp::OnExportActionCacheContainer(HActionCacheContainer *hacc)
{
   CExportActionsDialog dlg(hacc);
   if (dlg.DoModal()!=IDOK)
      return TRUE;

   for (POSITION pos = dlg.m_selected.GetHeadPosition(); pos;) {
      HActionCache *hac = dlg.m_selected.GetNext(pos);
      HModelCache *hmc = hac->GetDefaultModelCache();
      if (hmc==NULL) {
         CString msg, title;
         msg.LoadString(IDS_ACTIONHASNOMODELASSIGNED);
         String temp = (LPCTSTR)msg;
         temp.Replace("%1", hac->GetName());
         msg = temp.Get();
         title.LoadString(IDS_EXPORTTITLE);
         MessageBox(GetMainApplicationWnd(), msg, title, MB_OK);
         continue;
      }
      String filename = hac->GetName();
      filename = filename.GetNameOnlyWithoutSuffix();
      filename += ".kfm";
      filename = CombineNameToPath(dlg.m_dir, filename);
      ExportKFM(filename, dlg.m_errortolerance, hac, hmc);
   }
   return TRUE;
}

//*********************************************************************************
BOOL CHxtApp::OnExportMultiAction(HModel *hmodel)
{
   CExportMultiActionDialog dlg(hmodel);
   if (dlg.DoModal()!=IDOK)
      return TRUE;

   return TRUE;
//   return ExportKFM(dlg.m_filename, dlg.m_errortolerance, hac, hmc);
}

//*********************************************************************************

void CHxtApp::ReportExportError(const String &filename)
{
   CString caption;
   caption.LoadString(IDS_EXPORTTITLE);
   CString text;
   text.LoadString(IDS_ERROREXPORTINGACTION);
   String temp = (LPCTSTR)text;
   temp.Replace("%1", filename);
   text = temp.Get();
   MessageBox(GetMainApplicationWnd(), text, caption, MB_OK); 
}

BOOL CHxtApp::ExportKFM(String &filename, float errortolerance, HActionCache *hac, HModelCache *hmc)
{
   FileStream fs; 

   if ( !fs.Open( filename, HashStream::HS_BINARY ))
      return FALSE;

   CWaitCursor wait;
   HProgressBar *hprogress = GetProgressBar();
   if (hprogress) {
      hprogress->Initialize();
      CString text;
      text.LoadString(IDS_EXPORTTITLE);
      hprogress->SetMessage1((LPCTSTR)text);
   }

   // Convert all constraints to Keys
   HModel *hm = hac->EditWithModelCache(hmc);

   HActionCache *hunconstrainedac = hac->CreateUncontrainedActionCache(hm, errortolerance);
   
   float scalefactor = hunconstrainedac->GetScaleFactor();
   hac->EndEditWithModelCache(hm);
   if (hunconstrainedac==NULL) {
      ReportExportError(filename);
      return FALSE;
   }
   hm = hunconstrainedac->EditWithModelCache(hmc);

   fs << KFMVERSION;
   fs << hunconstrainedac->GetTotalTime().GetSeconds();

// Count bone shortcuts, these are bones that have drivers
   HAnimObjectShortcut *bonesfolder = (HAnimObjectShortcut *) hunconstrainedac->FindChildByName("Bones");// Finds "Bones" shortcut under driver folder

   WORD count = 1;
   if (bonesfolder)
      for (HAnimObjectShortcut *boneshortcut = (HAnimObjectShortcut *)bonesfolder->GetChild(); boneshortcut; boneshortcut=(HAnimObjectShortcut *)boneshortcut->GetSibling(), count++);
   fs << count;

   SaveBoneTransformDriver(fs, hm, NULL, errortolerance, scalefactor);

   int i = 0;
   if (bonesfolder) {
      for (HAnimObjectShortcut *boneshortcut = (HAnimObjectShortcut *)bonesfolder->GetChild(); boneshortcut; boneshortcut=(HAnimObjectShortcut *)boneshortcut->GetSibling()) {
         if (hprogress)
            hprogress->SetMessage2((LPCTSTR)boneshortcut->GetName());
         HBone *bone = (HBone *)boneshortcut->GetAnimObject();
         SaveBoneTransformDriver(fs, bone, boneshortcut, errortolerance, scalefactor);
         if (hprogress)
            hprogress->SetProgress((int)((float)++i/count*100));
      }
   }

   hunconstrainedac->EndEditWithModelCache(hm);
   HTreeObject::DeleteHandle(hunconstrainedac);
   if (hprogress)
      hprogress->Uninitialize();
   return TRUE;
}

HDriverShortcut *FindDriverShortcut( HBone *hbone, HProperty *hproperty )
{
   for (HPropertyDriver *driver=hbone->GetTransform()->GetHeadDriver(); driver; driver=driver->GetNextDriver())
      if (driver->GetProperty() == hproperty && driver->IsSimpleDriverShortcut())
         return (HDriverShortcut *)driver;
   return NULL;
}

void CHxtApp::SaveBoneTransformDriver(FileStream &fs, HBone *bone, HAnimObjectShortcut *boneshortcut, float errortolerance, float scalefactor)
{
   String name;
   if (bone) {
      name = bone->GetName();
      fs << bone->GetName() << '\0';
   }
   else if (boneshortcut) {
      name = boneshortcut->GetName();
      fs << boneshortcut->GetName() << '\0';
   }
   else
      return;

// We have several routes we can take to save all the rotate, translate, and scale keyframes.
// The easiest would be to just walk all the boneshortcuts for this action 
// and if their animobject pointer points to a bone, then retrieve the transform variable out of animobject (HBone *)
// and then walk the keyframes of the rotate, translate, scale variables within the transform variable.
// BUUUTT, this would cause any drivers in this action not be saved if it couldn't be matched up to a bone in 
// the model that we are exporting the action on.  (Most likely no one would care, but I would like to make 
// this example plugin handle extreme cases too.
// Also if the artist chose not to use the default quaternion rotate driver, we would need to convert the rotation 
// to quaternions.  But you can not do this at each rotation keyframe because the motion will not interpolate or 
// look the same, Thus we must sample the rotation at every frame and then reduce the keyframes.
// So the method this plugin uses is to first build an unconstrained action, that way we can assume the motion is 
// completely keyframe driven, and that only one driver exists under each variable.  Then instead of walking the transform 
// variables, we will actually walk the drivers of the action that way it doesn't matter if the model we are exporting this 
// action from actually had a matching bone.  However if their was a matching bone I went ahead and got the driver through the 
// transform variable rather then finding it with FindChildByName using the boneshortcut just to demonstrate how to do it.

// The only reason the action even needs to be applied to a model when exporting is so that we can call CreateUnconstrainedAction.
// We MUST have information about the model to perform this task.

   Vector bonedirection, rolldirection, parentbonedirection, parentrolldirection;
   HRotateDriver *rotatedriver = NULL;
   HChannelDriver *translatedriver[3];
   HChannelDriver *scaledriver[3];
   for (int i=0; i < 3; i++) {
      translatedriver[i] = NULL;
      scaledriver[i] = NULL;
   }

   if (bone) { 
      // the default model has a bone that name matched the bone in this action
      // lets get the driver through the transform variable
      // we could have done the same thing as the ELSE does, but I chose to demostrate this method also
      bone->ComputeBoneDirections(bonedirection, rolldirection, parentbonedirection, parentrolldirection);
//quatdriver->ComputeBoneToParent(bonedirection, rolldirection, parentbonedirection, parentrolldirection);

      HDriverShortcut *drivershortcut;

      drivershortcut = FindDriverShortcut( bone, bone->GetTransform()->GetRotate() );
      if (drivershortcut)
         rotatedriver = (HRotateDriver *)drivershortcut->GetDriver();

      drivershortcut = FindDriverShortcut( bone, bone->GetTransform()->GetTranslate()->GetX() );
      if (drivershortcut)
         translatedriver[0] = (HChannelDriver *)drivershortcut->GetDriver(); // could be a bad assumption that these are HChannelDrivers in the future, they can be any derivitive of HFloatDriver
      drivershortcut = FindDriverShortcut( bone, bone->GetTransform()->GetTranslate()->GetY() );
      if (drivershortcut)
         translatedriver[1] = (HChannelDriver *)drivershortcut->GetDriver();
      drivershortcut = FindDriverShortcut( bone, bone->GetTransform()->GetTranslate()->GetZ() );
      if (drivershortcut)
         translatedriver[2] = (HChannelDriver *)drivershortcut->GetDriver();

      drivershortcut = FindDriverShortcut( bone, bone->GetTransform()->GetScale()->GetX());
      if (drivershortcut)
         scaledriver[0] = (HChannelDriver *)drivershortcut->GetDriver();
      drivershortcut = FindDriverShortcut( bone, bone->GetTransform()->GetScale()->GetY());
      if (drivershortcut)
         scaledriver[1] = (HChannelDriver *)drivershortcut->GetDriver();
      drivershortcut = FindDriverShortcut( bone, bone->GetTransform()->GetScale()->GetZ());
      if (drivershortcut)
         scaledriver[2] = (HChannelDriver *)drivershortcut->GetDriver();
   }
   else { //This action has drivers for bones that are not in our default model
      //Lets just find the drivers ourselves

      // could be a bad assumption that these are HChannelDrivers in the future, they can be any derivitive of HFloatDriver
      HPropertyShortcut *transform = (HPropertyShortcut *)boneshortcut->FindChildByName("Transform");// just a place holder
      if (transform) {
         rotatedriver = (HRotateDriver *)transform->FindChildByName("Rotate");

         HPropertyShortcut *translatefolder = (HPropertyShortcut *)transform->FindChildByName("Translate");
         if (translatefolder) {
            translatedriver[0] = (HChannelDriver *)translatefolder->FindChildByName("X");
            translatedriver[1] = (HChannelDriver *)translatefolder->FindChildByName("Y");
            translatedriver[2] = (HChannelDriver *)translatefolder->FindChildByName("Z");
         }
         
         HPropertyShortcut *scalefolder = (HPropertyShortcut *)transform->FindChildByName("Scale");
         if (scalefolder) {
            scaledriver[0] = (HChannelDriver *)scalefolder->FindChildByName("X");
            scaledriver[1] = (HChannelDriver *)scalefolder->FindChildByName("Y");
            scaledriver[2] = (HChannelDriver *)scalefolder->FindChildByName("Z");
         }
      }

      if (rotatedriver)
         rotatedriver->ComputeBoneDirections(bonedirection, rolldirection, parentbonedirection, parentrolldirection);
      else {
         bonedirection.Set(0.0f,0.0f,1.0f);
         rolldirection.Set(0.0f,1.0f,0.0f);
         parentbonedirection.Set(0.0f,0.0f,1.0f);
         parentrolldirection.Set(0.0f,1.0f,0.0f);
      }
   }

   fs << bonedirection << rolldirection << parentbonedirection << parentrolldirection;

   HQuaternionRotateDriver *quatrotatedriver = NULL;

   if (rotatedriver && rotatedriver->GetObjectType()!=HOT_QUATERNIONROTATEDRIVER) {
// Must be EULER or Vector, Lets build our own Quaternion based rotate driver, 
// and jam keyframes in on every frame and then reduce.  This will get us the same motion
      quatrotatedriver = HQuaternionRotateDriver::New(rotatedriver->GetProperty());
      Time totaltime = rotatedriver->GetLastKey();

      Time prevtime, time;
      for (time=0; time <= totaltime; time.StepFrame(GetHProject()->GetFPS())) {
         TimeRange tr = FOREVER;
         Quaternion quat = rotatedriver->GetValue(time, tr);
         quatrotatedriver->StoreValue(time, quat);
      }
      rotatedriver = quatrotatedriver;

      if (errortolerance)
         quatrotatedriver->Reduce(errortolerance);
   }
   SaveKeyFrames(fs, (HQuaternionRotateDriver *)rotatedriver, translatedriver, scaledriver, scalefactor);
   if (quatrotatedriver)
      delete quatrotatedriver;
}

enum { HOLD, LINEAR, SPLINE };

BOOL CHxtApp::SaveKeyFrames(FileStream &fs, HQuaternionRotateDriver *rotatedriver, HChannelDriver *translatedriver[], HChannelDriver *scaledriver[], float scalefactor)
{
   union {
      BYTE flags;
      struct {
         BYTE defaulttangents : 1,
              interpmethod    : 2;
      };
   };

   // save translates
   for (int i=0; i < 3; i++) {
      HChannelDriver *channeldriver = translatedriver[i];
      if (channeldriver && channeldriver->CountKeys())
         SaveChannelDriver(fs, channeldriver, 1.0f/scalefactor);
      else {
         flags = 0;
         interpmethod = SPLINE;
         defaulttangents = TRUE;
         fs << (WORD)1 << 0.0f << 0.0f << flags;
      }
   }

   // save scales
   for (i=0; i < 3; i++) {
      HChannelDriver *channeldriver = scaledriver[i];
      if (channeldriver && channeldriver->CountKeys())
         SaveChannelDriver(fs, channeldriver, 100.0f);
      else {
         flags = 0;
         interpmethod = SPLINE;
         defaulttangents = TRUE;
         fs << (WORD)1 << 0.0f << 1.0f << flags;
      }
   }

   // save quaternion rotate
   if (rotatedriver) {
      flags = 0;
      interpmethod = SPLINE;
      defaulttangents = TRUE;

      // could be a bad assumption that these are HChannelDrivers in the future
      HChannelDriver *channeldriver = (HChannelDriver *)rotatedriver->GetX();
      if (channeldriver && channeldriver->CountKeys())
         SaveChannelDriver(fs, (HChannelDriver *)channeldriver, 100.0f);
      else
         fs << (WORD)1 << 0.0f << 0.0f << flags;//x

      channeldriver = (HChannelDriver *)rotatedriver->GetY();
      if (channeldriver && channeldriver->CountKeys())
         SaveChannelDriver(fs, (HChannelDriver *)channeldriver, 100.0f);
      else
         fs << (WORD)1 << 0.0f << 0.0f << flags;//y

      channeldriver = (HChannelDriver *)rotatedriver->GetZ();
      if (channeldriver && channeldriver->CountKeys())
         SaveChannelDriver(fs, (HChannelDriver *)channeldriver, 100.0f);
      else
         fs << (WORD)1 << 0.0f << 0.0f << flags;//z

      channeldriver = (HChannelDriver *)rotatedriver->GetW();
      if (channeldriver && channeldriver->CountKeys())
         SaveChannelDriver(fs, (HChannelDriver *)channeldriver, 100.0f);
      else
         fs << (WORD)1 << 0.0f << 1.0f << flags;//w
   }
   else {
      flags = 0;
      interpmethod = SPLINE;
      defaulttangents = TRUE;
      fs << (WORD)1 << 0.0f << 0.0f << flags;//x
      fs << (WORD)1 << 0.0f << 0.0f << flags;//y
      fs << (WORD)1 << 0.0f << 0.0f << flags;//z
      fs << (WORD)1 << 0.0f << 1.0f << flags;//w
   }

   return TRUE;
}

void CHxtApp::SaveChannelDriver(FileStream &fs, HChannelDriver *channeldriver, float unit)
{
   union {
      BYTE flags;
      struct {
         BYTE defaulttangents : 1,
              interpmethod    : 2;
      };
   };

   WORD keys = channeldriver->CountKeys();
   fs << keys;

   for (HChannelCP *cp = channeldriver->GetHeadCP(); cp; cp=cp->GetNext()) {
      flags = 0;
      CPInterpolationMethod cpmethod = cp->GetInterpolationMethod();
      if (cpmethod == CPI_DEFAULT)
         cpmethod = channeldriver->GetInterpolationMethod();

      switch (cpmethod) {
      case CPI_HOLD:
         interpmethod = HOLD;
         break;
      case CPI_LINEAR:
         interpmethod = LINEAR;
         break;
      case CPI_SPLINE:
      case CPI_ZEROSLOPE:
         interpmethod = SPLINE; // we don't store zero slope, we just write out the tangents
         break;
      }
      BOOL isdefaulttangents = cp->GetInGamma() == 0.0f;
      isdefaulttangents = isdefaulttangents && cp->GetInMagnitude() == 1.0f;
      isdefaulttangents = isdefaulttangents && cp->GetOutGamma() == 0.0f;
      isdefaulttangents = isdefaulttangents && cp->GetOutMagnitude() == 1.0f;
      isdefaulttangents = isdefaulttangents && cp->IsSmooth();
      isdefaulttangents = isdefaulttangents && cpmethod != CPI_ZEROSLOPE;
/*
      defaulttangents = cp->GetInGamma() == 0 && cp->GetInMagnitude() == 1 &&
                        cp->GetOutGamma() == 0 && cp->GetOutMagnitude() == 1 && cp->IsSmooth() &&
                        cpmethod != CPI_ZEROSLOPE;
*/
      defaulttangents = isdefaulttangents;

      float value = cp->GetValue()/unit;
      fs << cp->GetTime().GetSeconds() << value << flags;

      if (!defaulttangents) {
         Vector2 in = cp->GetInTangent();
         in.y /= unit;
         Vector2 out = cp->GetOutTangent();
         out.y /= unit;
         fs << in << out;
      }
   }
}


