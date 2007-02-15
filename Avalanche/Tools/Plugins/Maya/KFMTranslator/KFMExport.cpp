///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KFM EXPORT
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "KFMTranslator.h"

#include <maya/MItSelectionList.h>
#include <maya/MGlobal.h>
#include <maya/MAnimControl.h>
#include <maya/MTime.h>
#include <maya/MVector.h>
#include <maya/MQuaternion.h>
#include <maya/MPlugArray.h>
#include <maya/MFnTransform.h>
#include <maya/MFnIkJoint.h>
#include <maya/MMatrix.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MEulerRotation.h>

#include "../../libraries/MTools/MTools.h"

MStatus KFMTranslator::writer ( const MFileObject& file,
                               const MString& options,
                               FileAccessMode mode )
{
   if (options.length() > 0)
   {
      int i, length;
      // Start parsing.
      MStringArray optionList;
      MStringArray theOption;
      options.split(';', optionList); // break out all the options.

      length = optionList.length();
      for( i = 0; i < length; ++i )
      {
         theOption.clear();
         optionList[i].split( '=', theOption );

         //			if( theOption[0] == MString("verbose") && theOption.length() > 1 )
         //				if( theOption[1].asInt() > 0 )
         //					m_verbose = true;
         //				else
         //					m_verbose = false;
      }
   }

   m_channellist.clear();

   if( ( mode == MPxFileTranslator::kExportAccessMode ) ||
      ( mode == MPxFileTranslator::kSaveAccessMode ) )
   {
      ExportAll();
   }
   else if( mode == MPxFileTranslator::kExportActiveAccessMode )
   {
      ExportSelected();
   }

   WriteKFM(file.fullName().asChar());

   return MS::kSuccess;
}

MStatus KFMTranslator::ExportSelected()
{
   MStatus status = MS::kSuccess;

   BuildKFM();

   return status;
}

MStatus KFMTranslator::ExportAll()
{
   MStatus status = MS::kSuccess;
   return status;
}

int KFMTranslator::BuildKFM()	   
{
   MStatus status;
   m_channellist.clear();

   MSelectionList selection;
   status = MGlobal::getActiveSelectionList(selection);

   if (selection.length()==0)
   {
      MGlobal::displayError("No bone selected.");
      return MS::kFailure;
   }

   MAnimControl animcontrol;

   MTime endtime = animcontrol.maxTime();

   m_length = Time(endtime.as(MTime::kSeconds));

   MDagPath dagpath;
   MDagPath rootdagpath;

   selection.getDagPath(0,dagpath);

   MTools::GetRootBone(dagpath,rootdagpath);

   BuildChannel(rootdagpath);

   return 1;
}

int KFMTranslator::BuildChannel(MDagPath bonedagpath)
{
   MStatus status;
   Channel channel;

   if (bonedagpath.apiType() != MFn::kJoint)
      return 0;

   MFnDependencyNode depnode(bonedagpath.node());

   const char *debug = depnode.name().asChar();

   MString bonename = MayaBoneNameToHashBoneName(depnode.name().asChar());

   strcpy(channel.m_name,bonename.asChar());

   MDagPath endbonepath;

   //	search for child bone that matches orientation axis
   if (MTools::GetChildBoneThatMatchesOrientationVector(bonedagpath,endbonepath))
   {
      MDagPath parentdagpath(bonedagpath);
      parentdagpath.pop();

      MQuaternion quat;
      MTools::GetInclusiveOrientation(parentdagpath,quat);

      MVector Zidentity(0,0,1);
      MVector Yidentity(0,1,0);

      MVector parentorientationvector = Zidentity.rotateBy(quat);
      parentorientationvector.normalize();

      MVector parentrollvector = Yidentity.rotateBy(quat);
      parentrollvector.normalize();

      channel.m_parentbonedirection = Vector(parentorientationvector.x,parentorientationvector.y,parentorientationvector.z);
      channel.m_parentrolldirection = Vector(parentrollvector.x,parentrollvector.y,parentrollvector.z);

      Zidentity = MVector(0,0,1);
      Yidentity = MVector(0,1,0);

      MQuaternion quat2;
      MTools::GetInclusiveOrientation(bonedagpath,quat2);

      MVector orientationvector = Zidentity.rotateBy(quat2);
      orientationvector.normalize();

      MVector rollvector = Yidentity.rotateBy(quat2);
      rollvector.normalize();

      channel.m_bonedirection = Vector(orientationvector.x,orientationvector.y,orientationvector.z);
      channel.m_rolldirection = Vector(rollvector.x,rollvector.y,rollvector.z);

      if (GetChannelCurves(bonedagpath,channel))
         m_channellist.push_back(channel);
   }

   unsigned numChildren = bonedagpath.childCount();
   for (unsigned i = 0; i < numChildren; i++)
   {
      MDagPath temppath(bonedagpath);
      temppath.push(bonedagpath.child(i));
      BuildChannel(temppath);
   }
   return 1;
}

void FillOutKeyframesRot(MDagPath dagpath,
                         MEulerRotation rotation,
                         MObject animcurvex,
                         std::vector<Keyframe> *keyx,
                         MObject animcurvey,
                         std::vector<Keyframe> *keyy,
                         MObject animcurvez,
                         std::vector<Keyframe> *keyz,
                         std::vector<Keyframe> *keyw)
{
   MStatus status;

   MFnAnimCurve curvex(animcurvex);
   MFnAnimCurve curvey(animcurvey);
   MFnAnimCurve curvez(animcurvez);

   int i;

   unsigned numkeys = 0;
   MTime endtime;

   if (curvex.numKeys()>numkeys)
   {
      numkeys = curvex.numKeys();
      endtime = curvex.time(numkeys-1);
   }
   if (curvey.numKeys()>numkeys)
   {
      numkeys = curvey.numKeys();
      endtime = curvey.time(numkeys-1);
   }
   if (curvez.numKeys()>numkeys)
   {
      numkeys = curvez.numKeys();
      endtime = curvez.time(numkeys-1);
   }

   double totalframes = endtime.as(MTime::kNTSCFrame);

   for (i=0; i < totalframes; i++)
   {
      Keyframe kx,ky,kz,kw;
      MTime timex,timey,timez;
      double valuex,valuey,valuez;
      MEulerRotation	erot;

      kx.m_interpmethod = Keyframe::SPLINE;
      ky.m_interpmethod = Keyframe::SPLINE;
      kz.m_interpmethod = Keyframe::SPLINE;
      kw.m_interpmethod = Keyframe::SPLINE;

      MTime currenttime(i,MTime::kNTSCFrame);

      if (animcurvex == MObject::kNullObj)
         valuex = rotation.x;
      if (animcurvey == MObject::kNullObj)
         valuey = rotation.y;
      if (animcurvez == MObject::kNullObj)
         valuez = rotation.z;

      if (animcurvex != MObject::kNullObj)
         status = curvex.evaluate(currenttime,valuex);
      if (animcurvey != MObject::kNullObj)
         status = curvey.evaluate(currenttime,valuey);
      if (animcurvez != MObject::kNullObj)
         status = curvez.evaluate(currenttime,valuez);

      kx.m_time = currenttime.as(MTime::kSeconds);
      ky.m_time = currenttime.as(MTime::kSeconds);
      kz.m_time = currenttime.as(MTime::kSeconds);
      kw.m_time = currenttime.as(MTime::kSeconds);

      // get a quaternion from the x,y,z euler in Maya 
      erot.x = valuex;
      erot.y = valuey;
      erot.z = valuez;

      MQuaternion quat;
      quat = erot.asQuaternion();
      quat.normalizeIt();

      // here is our quaternion.. apply it to our channels
      kx.m_value = (float)quat.x;
      ky.m_value = (float)quat.y;
      kz.m_value = (float)quat.z;
      kw.m_value = (float)quat.w;

      kx.m_defaulttangents = 1;
      ky.m_defaulttangents = 1;
      kz.m_defaulttangents = 1;
      kw.m_defaulttangents = 1;

      // add keyframe
      keyx->push_back(kx); 
      keyy->push_back(ky); 
      keyz->push_back(kz); 
      keyw->push_back(kw); 
   }
}

void FillOutKeyframesTransScale(int TransEnable,
                                MDagPath dagpath,
                                MVector translate,
                                MObject animcurvex,
                                std::vector<Keyframe> *keyx,
                                MObject animcurvey, 
                                std::vector<Keyframe> *keyy,
                                MObject animcurvez,
                                std::vector<Keyframe> *keyz)
{
   MFnDependencyNode debugdepnode(dagpath.node());
   const char *debug = debugdepnode.name().asChar();

   MQuaternion orientquat,orientquat2;
   MStatus	status;

   // get the quaternion for this joint...
   MFnIkJoint fnjoint(dagpath, &status);
   fnjoint.getOrientation(orientquat2);

   if (!MTools::GetInclusiveOrientation(dagpath,orientquat))
      MGlobal::displayError("Couldn't get inclusive orientation.");

   MFnAnimCurve curvex(animcurvex);
   MFnAnimCurve curvey(animcurvey);
   MFnAnimCurve curvez(animcurvez);

   int i;

   unsigned numkeys = 0;
   MTime endtime;

   if (curvex.numKeys()>numkeys)
   {
      numkeys = curvex.numKeys();
      endtime = curvex.time(numkeys-1);
   }
   if (curvey.numKeys()>numkeys)
   {
      numkeys = curvey.numKeys();
      endtime = curvey.time(numkeys-1);
   }
   if (curvez.numKeys()>numkeys)
   {
      numkeys = curvez.numKeys();
      endtime = curvez.time(numkeys-1);
   }

   double totalframes = endtime.as(MTime::kNTSCFrame);

   for (i=0; i < totalframes; i++)
   {
      Keyframe kx,ky,kz,kw;
      MTime timex,timey,timez;

      kx.m_interpmethod = Keyframe::SPLINE;
      ky.m_interpmethod = Keyframe::SPLINE;
      kz.m_interpmethod = Keyframe::SPLINE;

      MVector translation(0.0,0.0,0.0);

      if (animcurvex == MObject::kNullObj)
         translation.x = translate.x;
      if (animcurvey == MObject::kNullObj)
         translation.y = translate.y;
      if (animcurvez == MObject::kNullObj)
         translation.z = translate.z;

      MAnimControl animcontrol;

      double test;
      test = animcontrol.playbackSpeed();

      MTime currenttime(i,MTime::kNTSCFrame);

      if (animcurvex != MObject::kNullObj)
         status = curvex.evaluate(currenttime,translation.x);
      if (animcurvey != MObject::kNullObj)
         status = curvey.evaluate(currenttime,translation.y);
      if (animcurvez != MObject::kNullObj)
         status = curvez.evaluate(currenttime,translation.z);

      if (TransEnable)
      {
         // adjust for different coord space
         MMatrix inclusive,exclusive,result;
         if (!MTools::GetInclusiveBindPoseMatrix(dagpath,inclusive))
            MessageBox(NULL,"Failed to get bind pose matrix.","Warning.",MB_OK);
         MTools::GetExclusiveBindPoseMatrix(dagpath,exclusive);
         result = inclusive * exclusive.inverse();
         MTransformationMatrix bindmatrix(result);
         MVector bindmatrixtranslation = bindmatrix.translation(MSpace::kWorld);
         translation = translation - bindmatrixtranslation;

         translation = translation * orientquat2.inverse();//THIS IS EVIL!
      }

      kx.m_time = currenttime.as(MTime::kSeconds);
      ky.m_time = currenttime.as(MTime::kSeconds);
      kz.m_time = currenttime.as(MTime::kSeconds);

      kx.m_value = (float)translation.x;
      ky.m_value = (float)translation.y;
      kz.m_value = (float)translation.z;

      kx.m_defaulttangents = 1;
      ky.m_defaulttangents = 1;
      kz.m_defaulttangents = 1;

      // add keyframe
      keyx->push_back(kx); 
      keyy->push_back(ky); 
      keyz->push_back(kz); 
   }
}

int KFMTranslator::GetChannelCurves(MDagPath bonedagpath,Channel &channel)
{
   MStatus status;

   MFnDependencyNode depnode(bonedagpath.node());
   const char *debug = depnode.name().asChar();

   ////////////////////////////////////////////////////
   // Get our plugs
   ////////////////////////////////////////////////////
   MPlug rotateX = depnode.findPlug("rotateX",&status);
   MPlug rotateY = depnode.findPlug("rotateY",&status);
   MPlug rotateZ = depnode.findPlug("rotateZ",&status);

   MPlug translateX = depnode.findPlug("translateX",&status);
   MPlug translateY = depnode.findPlug("translateY",&status);
   MPlug translateZ = depnode.findPlug("translateZ",&status);

   MPlug scaleX = depnode.findPlug("scaleX",&status);
   MPlug scaleY = depnode.findPlug("scaleY",&status);
   MPlug scaleZ = depnode.findPlug("scaleZ",&status);

   ////////////////////////////////////////////////////
   // handle the rotations
   ////////////////////////////////////////////////////
   MPlugArray connected;

   connected.clear();
   rotateX.connectedTo(connected,true,true,&status);
   if (!status)
      return 0;

   MObject rotateXanimcurveObject = MObject::kNullObj;
   if (connected.length()>0)
      rotateXanimcurveObject= connected[0].node();

   connected.clear();
   rotateY.connectedTo(connected,true,true,&status);
   if (!status)
      return 0;

   MObject rotateYanimcurveObject = MObject::kNullObj;
   if (connected.length()>0)
      rotateYanimcurveObject = connected[0].node();

   connected.clear();
   rotateZ.connectedTo(connected,true,true,&status);
   if (!status)
      return 0;

   MObject rotateZanimcurveObject = MObject::kNullObj;
   if (connected.length()>0)
      rotateZanimcurveObject = connected[0].node();

   ////////////////////////////////////////////////////
   // handle the Translations
   ////////////////////////////////////////////////////
   connected.clear();
   translateX.connectedTo(connected,true,true,&status);
   if (!status)
      return 0;

   MObject translateXanimcurveObject = MObject::kNullObj;
   if (connected.length()>0)
      translateXanimcurveObject= connected[0].node();

   connected.clear();
   translateY.connectedTo(connected,true,true,&status);
   if (!status)
      return 0;

   MObject translateYanimcurveObject = MObject::kNullObj;
   if (connected.length()>0)
      translateYanimcurveObject = connected[0].node();

   connected.clear();
   translateZ.connectedTo(connected,true,true,&status);
   if (!status)
      return 0;

   MObject translateZanimcurveObject = MObject::kNullObj;
   if (connected.length()>0)
      translateZanimcurveObject = connected[0].node();

   ////////////////////////////////////////////////////
   // handle the Scales
   ////////////////////////////////////////////////////
   connected.clear();
   scaleX.connectedTo(connected,true,true,&status);
   if (!status)
      return 0;

   MObject scaleXanimcurveObject = MObject::kNullObj;
   if (connected.length()>0)
      scaleXanimcurveObject= connected[0].node();

   connected.clear();
   scaleY.connectedTo(connected,true,true,&status);
   if (!status)
      return 0;

   MObject scaleYanimcurveObject = MObject::kNullObj;
   if (connected.length()>0)
      scaleYanimcurveObject = connected[0].node();

   connected.clear();
   scaleZ.connectedTo(connected,true,true,&status);
   if (!status)
      return 0;

   MObject scaleZanimcurveObject = MObject::kNullObj;
   if (connected.length()>0)
      scaleZanimcurveObject = connected[0].node();

   ////////////////////////////////////////////////////
   // Setup the keys
   ////////////////////////////////////////////////////
   std::vector<Keyframe> &keyframesTransX = channel.m_channelitem[0].m_keyframes;
   std::vector<Keyframe> &keyframesTransY = channel.m_channelitem[1].m_keyframes;
   std::vector<Keyframe> &keyframesTransZ = channel.m_channelitem[2].m_keyframes;

   std::vector<Keyframe> &keyframesScaleX = channel.m_channelitem[3].m_keyframes;
   std::vector<Keyframe> &keyframesScaleY = channel.m_channelitem[4].m_keyframes;
   std::vector<Keyframe> &keyframesScaleZ = channel.m_channelitem[5].m_keyframes;

   std::vector<Keyframe> &keyframesRotX = channel.m_channelitem[6].m_keyframes;
   std::vector<Keyframe> &keyframesRotY = channel.m_channelitem[7].m_keyframes;
   std::vector<Keyframe> &keyframesRotZ = channel.m_channelitem[8].m_keyframes;
   std::vector<Keyframe> &keyframesRotW = channel.m_channelitem[9].m_keyframes;

   MVector translate,scale;

   MFnTransform fnTransform(bonedagpath.node());
   translate = fnTransform.translation(MSpace::kTransform,&status);
   if (status != MS::kSuccess)
      return MS::kFailure;

   MEulerRotation rotation;

   status = fnTransform.getRotation(rotation);// this is in radians
   if (status != MS::kSuccess)
      return MS::kFailure;


   MQuaternion quatrotation = rotation.asQuaternion();

   double dScale[3];

   status = fnTransform.getScale(dScale);
   if (status != MS::kSuccess)
      return MS::kFailure;

   scale.x = dScale[0];
   scale.y = dScale[1];
   scale.z = dScale[2];

   // Rotation Keys
   FillOutKeyframesRot(bonedagpath,
      rotation,
      rotateXanimcurveObject, &keyframesRotX,
      rotateYanimcurveObject, &keyframesRotY,
      rotateZanimcurveObject, &keyframesRotZ,
      &keyframesRotW);

   // Translation Keys
   FillOutKeyframesTransScale(1,bonedagpath,
      translate,
      translateXanimcurveObject, &keyframesTransX,
      translateYanimcurveObject, &keyframesTransY,
      translateZanimcurveObject, &keyframesTransZ);


   // Scale Keys
   FillOutKeyframesTransScale(0,bonedagpath,
      scale,
      scaleXanimcurveObject, &keyframesScaleX,
      scaleYanimcurveObject, &keyframesScaleY,
      scaleZanimcurveObject, &keyframesScaleZ);

   //	if ((translateXanimcurveObject==MObject::kNullObj)&&
   //		(translateYanimcurveObject==MObject::kNullObj)&&
   //		(translateZanimcurveObject==MObject::kNullObj)&&
   //
   //		(rotateXanimcurveObject==MObject::kNullObj)&&
   //		(rotateYanimcurveObject==MObject::kNullObj)&&
   //		(rotateZanimcurveObject==MObject::kNullObj)&&

   //		(scaleXanimcurveObject==MObject::kNullObj)&&
   //		(scaleYanimcurveObject==MObject::kNullObj)&&
   //		(scaleZanimcurveObject==MObject::kNullObj))
   //	{

   //		return 0;
   //	}

   // fix translation
   MQuaternion orientquat;
   MFnIkJoint fnjoint(bonedagpath);
   fnjoint.getOrientation(orientquat);
   MTransformationMatrix xformMatrix = fnjoint.transformation(&status);
   xformMatrix.asMatrixInverse();
   MVector translationvector = xformMatrix.translation(MSpace::kWorld);
   MVector translation = translate - translationvector;
   translation = translation * orientquat.inverse();			// fix the orientation per hash

   // All this needs to be added to prevent AMViewer from crashing
   if ((translateXanimcurveObject==MObject::kNullObj)||(keyframesTransX.size()==0))
      FillInEmptyChannelItem(keyframesTransX,translation.x);
   if ((translateYanimcurveObject==MObject::kNullObj)||(keyframesTransY.size()==0))
      FillInEmptyChannelItem(keyframesTransY,translation.y);
   if ((translateZanimcurveObject==MObject::kNullObj)||(keyframesTransZ.size()==0))
      FillInEmptyChannelItem(keyframesTransZ,translation.z);

   if (((rotateXanimcurveObject==MObject::kNullObj)||(rotateYanimcurveObject==MObject::kNullObj)||(rotateZanimcurveObject==MObject::kNullObj))||
      (keyframesRotX.size()==0)||(keyframesRotY.size()==0)||(keyframesRotZ.size()==0))
   {// due to the euler to quaternion conversion can't solve if missing a channel, just zero out
      FillInEmptyChannelItem(keyframesRotX,quatrotation.x);
      FillInEmptyChannelItem(keyframesRotY,quatrotation.y);
      FillInEmptyChannelItem(keyframesRotZ,quatrotation.z);
      FillInEmptyChannelItem(keyframesRotW,quatrotation.w);
   }

   if ((scaleXanimcurveObject==MObject::kNullObj)||(keyframesScaleX.size()==0))
      FillInEmptyChannelItem(keyframesScaleX,scale.x,true);
   if ((scaleYanimcurveObject==MObject::kNullObj)||(keyframesScaleY.size()==0))
      FillInEmptyChannelItem(keyframesScaleY,scale.y,true);
   if ((scaleZanimcurveObject==MObject::kNullObj)||(keyframesScaleZ.size()==0))
      FillInEmptyChannelItem(keyframesScaleZ,scale.z,true);

   return 1;
}

void KFMTranslator::FillInEmptyChannelItem(std::vector<Keyframe> &keyframes,double value,bool isScale)
{
   keyframes.clear();

   Keyframe begin;
   begin.m_time = 0.0f;
   begin.m_value = (float)value;
   if (isScale)
      begin.m_value = 1.0f;
   begin.m_defaulttangents = 1;
   // add beginning keyframe
   keyframes.push_back(begin); 

   Keyframe end;
   end.m_time = m_length;
   end.m_value = (float)value;
   if (isScale)
      end.m_value = 1.0f;
   end.m_defaulttangents = 1;
   // add end keyframe
   keyframes.push_back(end); 
}

int KFMTranslator::WriteKFM(const char *filename)
{
   int version;
   WORD channelcount;
   float seconds;
   FILE *fp;

   fp = fopen(filename,"wb");

   if (fp == NULL)
      return 0;

   version = KFMVERSION;
   fwrite(&version,sizeof(int),1,fp);

   seconds = m_length.GetSeconds();
   fwrite(&seconds,sizeof(float),1,fp);

   channelcount = m_channellist.size();
   fwrite(&channelcount,sizeof(WORD),1,fp);

   for (int i = 0; i < channelcount; i++)
   {
      Channel &channel = m_channellist[i];

      WriteString(fp,channel.m_name);

      WriteVector(fp,channel.m_bonedirection);
      WriteVector(fp,channel.m_rolldirection);
      WriteVector(fp,channel.m_parentbonedirection);
      WriteVector(fp,channel.m_parentrolldirection);

      for (int j = 0; j < Channel::NUMCHANNELITEMS; j++)
      {
         ChannelItem &chanitem = channel.m_channelitem[j];

         WORD numkf = chanitem.m_keyframes.size();
         fwrite(&numkf,sizeof(WORD),1,fp);// Number of keyframes

         for (int k = 0; k < numkf; k++)
         {
            Keyframe &kf = chanitem.m_keyframes[k];

            seconds = kf.m_time.GetSeconds();
            fwrite(&seconds,sizeof(float),1,fp);

            fwrite(&kf.m_value,sizeof(float),1,fp);
            fwrite(&kf.m_flags,sizeof(unsigned char),1,fp);

            if (!kf.m_defaulttangents)
            {
               WriteVector2(fp,kf.m_it);
               WriteVector2(fp,kf.m_ot);
            }
         }
      }
   }

   fclose(fp);

   return 1;
}

void KFMTranslator::WriteVector(FILE *fp,Vector vect)
{
   float x = (float)vect.x,y = (float)vect.y,z = (float)vect.z;
   fwrite(&x,sizeof(float),1,fp);
   fwrite(&y,sizeof(float),1,fp);
   fwrite(&z,sizeof(float),1,fp);
}

void KFMTranslator::WriteVector2(FILE *fp,Vector2 vect2)
{
   float x = (float)vect2.x,y = (float)vect2.y;
   fwrite(&x,sizeof(float),1,fp);
   fwrite(&y,sizeof(float),1,fp);
}

void KFMTranslator::WriteString(FILE *fp,char *str)
{
   int len = strlen(str);
   for (int i = 0; i < len; i++)
      fputc(str[i],fp);
   fputc('\0',fp);
}
