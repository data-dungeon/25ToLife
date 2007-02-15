// RC  5/3/2002  \Randy95\Plugin\HXT\KFM\Import.cpp

#include "stdafx.h"
#include "StringLi.h"
#include "Hxt.h"
#include "F_Protos.h"
#include "HashFile.h"
#include "Vector2.h"

#include "SDK\HModel.h"
#include "SDK\HAction.h"
#include "SDK\Misc.h"
#include "SDK\HPropDri.h"

class Keyframe {
public:
   enum INTERPOLATIONMETHOD { HOLD, LINEAR, SPLINE };
   Time  m_time;
   float m_value;
   union {
      BYTE m_flags;
      struct {
         BYTE  m_defaulttangents : 1,
               m_interpmethod    : 2;
      };
   };
   Keyframe()
   {
      m_defaulttangents = TRUE;
   }
};

BOOL CHxtApp::OnImportActionCache(HActionCache *hac)
{
   CString caption, filter;
   caption.LoadString(IDS_IMPORTTITLE);
   filter.LoadString(IDS_FILEFILTERS);
   String defdir;//not used for multiselect dlg
   String ext("KFM");
   StringList *namelist = NULL;

   if (!OpenRequest((LPCTSTR)caption, (LPCTSTR)filter, defdir, ext.Get(), ext.Get(), NULL, NULL, &namelist))
      return TRUE;

   if (namelist==NULL) {
      CString caption;
      caption.LoadString(IDS_IMPORTTITLE);
      CString text;
      text.LoadString(IDS_ERRORIMPORTING);
      MessageBox(GetMainApplicationWnd(), text, caption, MB_OK); 
      return FALSE;
   }

   CWaitCursor wait;
   for (POSITION namepos = namelist->GetHeadPosition();namepos;) {
      String filename = namelist->GetNext(namepos);
      if (!ImportKFM(filename, hac)) {
         ReportImportError(filename);
         continue;
      }
   }
   DeleteStringList(namelist);

   return TRUE;
}

BOOL CHxtApp::OnImportActionCacheContainer(HActionCacheContainer *hacc)
{
   CString caption, filter;
   caption.LoadString(IDS_IMPORTTITLE);
   filter.LoadString(IDS_FILEFILTERS);
   String defdir;//not used for multiselect dlg
   String ext("KFM");
   StringList *namelist = NULL;

   if (!OpenRequest((LPCTSTR)caption, (LPCTSTR)filter, defdir, ext.Get(), ext.Get(), NULL, NULL, &namelist))
      return TRUE;

   if (namelist==NULL) {
      CString caption;
      caption.LoadString(IDS_IMPORTTITLE);
      CString text;
      text.LoadString(IDS_ERRORIMPORTING);
      MessageBox(GetMainApplicationWnd(), text, caption, MB_OK); 
      return FALSE;
   }

   CWaitCursor wait;
   for (POSITION namepos = namelist->GetHeadPosition();namepos;) {
      String filename = namelist->GetNext(namepos);
      String nameonly = filename.GetNameOnly();
      HActionCache *hac = HActionCache::New(nameonly.Get());
      if (hac==NULL) {
         ReportImportError(filename);
         continue;
      }

      if (!ImportKFM(filename, hac)) {
         ReportImportError(filename);
         continue;
      }
   }
   DeleteStringList(namelist);

   return TRUE;
}

void CHxtApp::ReportImportError(const String &filename)
{
   CString caption;
   caption.LoadString(IDS_IMPORTTITLE);
   CString text;
   text.LoadString(IDS_ERRORIMPORTINGACTION);
   String temp = (LPCTSTR)text;
   temp.Replace("%1", filename);
   text = temp.Get();
   MessageBox(GetMainApplicationWnd(), text, caption, MB_OK); 
}

HDriverShortcut *FindDriverShortcut( HBone *hbone, HProperty *hproperty );

BOOL CHxtApp::ImportKFM(const String &pfilename, HActionCache *hac)
{
   FileStream fs;
   String filename = pfilename;
   float fTemp;
   int iTemp;

   if (!fs.Open( filename, HashStream::HS_BINARY | HashStream::HS_LOAD))
      return FALSE;

   fs >> iTemp;
   if (iTemp != KFMVERSION) {
      CString msg, title;
      msg.LoadString(IDS_INVALIDVERSION);
      title.LoadString(IDS_IMPORTTITLE);
      MessageBox(GetMainApplicationWnd(), msg, title, MB_ICONEXCLAMATION);
      return FALSE;
   }

   fs >> fTemp;
   Time totaltime;
   totaltime.SetSeconds(fTemp);

   WORD bonecount;
   fs >> bonecount;

/*
The easiest way for the Driver Hierachy for the imported bones to be build is to build 
a temporary model and add one bone.  I will then change the name of this bone to each of the names
of the bones being loaded from the kfm file, and simply use the Property under the bone to set the keyframes,
another solution would be to force the user to pick a model to import the kfm action on to.  The last solution would be to just
allocate all of the nessessary classes to build the hierarchy and drivers and stitch them all together yourself.  This
method would be more tedious and bug prone, plus a lot more confusing, */

// Lets build temporary modelcache with one bone
   HModelCache *hmc = HModelCache::New();
   HBoneCacheContainer *hbcc = HBoneCacheContainer::New();

   hmc->InsertChildAtTail(hbcc);

   HSegmentCache *hsc = HSegmentCache::New("Testing");
   hbcc->InsertChildAtTail(hsc);

// ModelCache is build now lets attach it to our actioncache and build a HModel and HAction
   HModel *hm = hac->EditWithModelCache(hmc);

// We are ready to start storing keyframes, lets grab our HBone from our HModel instance and then retrieve the Transform variables
   HBoneContainer *hbcontainer = hm->GetChildBoneContainer();
   HBone *hbone = hbcontainer->GetChildBone();

   HTranslateProperty *translate = hbone->GetTransform()->GetTranslate();
   HScaleProperty *scale = hbone->GetTransform()->GetScale();
   HRotateProperty *rotate = hbone->GetTransform()->GetRotate();

   String bonename;
   Vector bonedirection, rolldirection, parentbonedirection, parentrolldirection;

   for (int boneindex=0; boneindex < bonecount; boneindex++) {
      bonename = fs.ReadString('\0');
//Because we simply want to keep renaming the bones and calling store value for each of the bones in the KFM file
// We must keep removing the drivers so to trick the software into rebuilding new drivers under the new bone name
      hbone->RemoveDrivers();
      hsc->SetName(bonename.Get());//this will rename the Hbone also, because it is an instance
bonename = hsc->GetName();
hbone->SetName(bonename.Get());

// Need this info for storing Rotate later
      fs >> bonedirection >> rolldirection;
      fs >> parentbonedirection >> parentrolldirection;

      WORD numkf;

      enum { XTRANSLATE, YTRANSLATE, ZTRANSLATE, XSCALE, YSCALE, ZSCALE, TOTAL };
      // We'll handle the quaternion rotate separately because we can't set it a component at a time (x, y, z, w)
      // We also don't really know if the driver attached to the variable will be a quaternion anyways, could be Vector, or Euler
      HFloatProperty *var;
      for (int i=0; i < TOTAL; i++) {
         switch (i) {
         case XTRANSLATE:
            var = translate->GetX();
            break;
         case YTRANSLATE:
            var = translate->GetY();
            break;
         case ZTRANSLATE:
            var = translate->GetZ();
            break;
         case XSCALE:
            var = scale->GetX();
            break;
         case YSCALE:
            var = scale->GetY();
            break;
         case ZSCALE:
            var = scale->GetZ();
            break;
         }

         HChannelCP *prevcp = NULL;
         Vector2 previt, prevot;

         fs >> numkf;
         for (int keyindex = 0; keyindex < numkf; keyindex++) {
            Keyframe kf;
            float seconds;
            fs >> seconds;
            kf.m_time.SetSeconds(seconds);
            fs >> kf.m_value;
            kf.m_value;
            fs >> kf.m_flags;
            var->StoreValue(kf.m_time, kf.m_value);// This will automatically multiply the scales 100 factor

            if (prevcp)
               prevcp->SetBias(previt, prevot);

            if (!kf.m_defaulttangents) {
               HDriverShortcut *drivershortcut = FindDriverShortcut( hbone, var );
               HChannelDriver *driver = (HChannelDriver *)drivershortcut->GetDriver();
               prevcp = driver->GetCPFromTime(kf.m_time);
               fs >> previt;
               fs >> prevot;
            }
            else
               prevcp = NULL;
         }

         if (prevcp)
            prevcp->SetBias(previt, prevot);
      }
// Lets Handle the rotate case now
// We'll create the storagedriver which will build a QuaternionRotateDriver
// Once the driver is created we can store the keyframes.
// After all keyframes are built I will set the bonetoparent rotation in the driver.
// This quaternion rotation is built by the bone directions of the bone and its parent bone.
// These directions were read in above from the KFM file, and were originally created
// when the user first created the action.
// This rotation tells the driver how the bones were positioned when the original keyframes
// were built.  We can use this information to allow this action to be put on models that have the
// same bone hierachy, but with bones not neccessarily positioned the same.direction as the model the action was
// originally built on.
// The bonetoparent quat will be used by the software when the action gets hooked up to a model.  
// It will build a parent compensation quaternion in the drivershortcut under the rotate variable.
// This parent compensation will be created using the model's bone position and the actions rotate driver's
// bonetoparent rotation.


      HDriverShortcut *drivershortcut = (HDriverShortcut *)hac->CreateStorageDriver(rotate);
      HQuaternionRotateDriver *quatdriver = (HQuaternionRotateDriver *)drivershortcut->GetDriver();
      enum {QUATX, QUATY, QUATZ, QUATW, QUATTOTAL };
      HChannelDriver *chandriver;
      for (i=0; i < QUATTOTAL; i++) {
         switch (i) {
            case QUATX:
               chandriver = (HChannelDriver *)quatdriver->GetX();
               break;
            case QUATY:
               chandriver = (HChannelDriver *)quatdriver->GetY();
               break;
            case QUATZ:
               chandriver = (HChannelDriver *)quatdriver->GetZ();
               break;
            case QUATW:
               chandriver = (HChannelDriver *)quatdriver->GetW();
               break;
         }

         HChannelCP *prevcp = NULL;
         Vector2 previt, prevot;

         fs >> numkf;
         for (int keyindex = 0; keyindex < numkf; keyindex++) {
            Keyframe kf;
            float seconds;
            fs >> seconds;
            kf.m_time.SetSeconds(seconds);
            fs >> kf.m_value;
            kf.m_value;
            fs >> kf.m_flags;

            chandriver->StoreValue(kf.m_time, kf.m_value);

            if (prevcp)
               prevcp->SetBias(previt, prevot);

            if (!kf.m_defaulttangents) {
               prevcp = chandriver->GetCPFromTime(kf.m_time);
               fs >> previt;
               fs >> prevot;
            }
            else
               prevcp = NULL;
         }
         if (prevcp)
            prevcp->SetBias(previt, prevot);
      }
      quatdriver->ComputeBoneToParent(bonedirection, rolldirection, parentbonedirection, parentrolldirection);
   }

   hac->EndEditWithModelCache(hm);
   HTreeObject::DeleteHandle(hmc);

   return TRUE;
}