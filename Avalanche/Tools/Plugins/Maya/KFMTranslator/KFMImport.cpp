///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KFM IMPORT
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "KFMTranslator.h"
#include <maya/MItSelectionList.h>
#include <maya/MGlobal.h>
#include <maya/MFnCharacter.h>
#include <maya/MFnClip.h>
#include <maya/MPlugArray.h>
#include <maya/MTime.h>
#include <maya/MMatrix.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MQuaternion.h>
#include <maya/MFnIkJoint.h>
#include <maya/MEulerRotation.h>
#include <maya/MFnDagNode.h>

#include "../../libraries/MTools/MTools.h"

MStatus KFMTranslator::reader ( const MFileObject& file,
                               const MString& options,
                               FileAccessMode mode)
{
   MStatus status;
   MDGModifier dgModifier;
   MSelectionList selection;

   m_forcenewcharacter = false;
   m_applytobones = false;

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

         if( theOption[0] == MString("forcenewcharacter") && theOption.length() > 1 )
            if( theOption[1].asInt() > 0 )
               m_forcenewcharacter = true;
            else
               m_forcenewcharacter = false;

         // forces animation onto bones
         if( theOption[0] == MString("applytobones") && theOption.length() > 1 )
            if( theOption[1].asInt() > 0 )
               m_applytobones = true;
            else
               m_applytobones = false;

      }
   }

   // get active selection
   status = MGlobal::getActiveSelectionList(selection);
   if (selection.length()==0)
   {
      MGlobal::displayError("No bone selected.");
      return MS::kFailure;
   }

   m_animchannels.clear();
   m_bonemap.Clear();
   m_channellist.clear();

   MDagPath dagpath;
   MDagPath dagpathbone;

   MObject object;
   selection.getDagPath(0,dagpath);
   selection.getDependNode(0,object);

   int err = ReadKFM(file.fullName().asChar());
   if (!err)
   {
      MGlobal::displayError("Failed to load KFM. May be corrupt.");
      return MS::kFailure;
   }

   // find root bone
   MDagPath rootjointdagpath;
   if (object.apiType() == MFn::kClip)
      MTools::GetRootBoneFromClip(object,rootjointdagpath);
   else if (object.apiType() == MFn::kCharacter)
      MTools::GetRootBoneFromCharacter(object,rootjointdagpath);
   else
      MTools::GetRootBone(dagpath,rootjointdagpath);

   MSelectionList bones;
   bones.clear();

   // loop through channels creating Maya animation curves
   for (int i = 0; i < (int)m_channellist.size(); i++)
   {
      Channel &channel = m_channellist[i];

      MString channelname;
      channelname = channel.m_name;
      channelname = HashBoneNameToMayaBoneName(channelname.asChar());

      if (channelname.substring(0,7)=="Shortcut")
         channelname = channelname.substring(12,channelname.length()-1);

      const char *debug = channelname.asChar();
    
      if ( !MTools::FindBoneByName ( channelname.asChar(), rootjointdagpath, dagpathbone) )
         continue;

      // apply our animation curves
      // if m_applytobones == TRUE, AnimCurve is created then connected to the bones transform
      if (BuildAnimCurves(channel,dagpathbone))
         bones.add(dagpathbone);
   }

   if (m_applytobones)
   {
      m_animchannels.clear();
      return MS::kSuccess;
   }

   MObject characterobject,sourceclipobject;
   if (!HookUpSourceClip(object,bones,characterobject,file.name(),sourceclipobject))
      return MS::kFailure;

   HookUpAnimCurvesToSourceClip(characterobject,sourceclipobject);

   m_animchannels.clear();

   return MS::kSuccess;
}

int KFMTranslator::BuildAnimCurves(Channel &channel,MDagPath dagpath)
{
   MFnDependencyNode depnode(dagpath.node());
   const char *debug = depnode.name().asChar();

   MStatus status;
   MDGModifier dgModifier;

   int i;

   // get the framecounts for the different channels

   // translation
   int framecount_Tx = channel.m_channelitem[0].m_keyframes.size();
   int framecount_Ty = channel.m_channelitem[1].m_keyframes.size();
   int framecount_Tz = channel.m_channelitem[2].m_keyframes.size();

   // scale
   int framecount_Sx = channel.m_channelitem[3].m_keyframes.size();
   int framecount_Sy = channel.m_channelitem[4].m_keyframes.size();
   int framecount_Sz = channel.m_channelitem[5].m_keyframes.size();

   // rotation
   int framecount_Rx = channel.m_channelitem[6].m_keyframes.size();
   int framecount_Ry = channel.m_channelitem[7].m_keyframes.size();
   int framecount_Rz = channel.m_channelitem[8].m_keyframes.size();
   int framecount_Rw = channel.m_channelitem[9].m_keyframes.size();


   // get our MAYA attribute plugs
   MPlug translateX = depnode.findPlug("translateX",&status);
   MPlug translateY = depnode.findPlug("translateY",&status);
   MPlug translateZ = depnode.findPlug("translateZ",&status);

   MPlug rotateX = depnode.findPlug("rotateX",&status);
   MPlug rotateY = depnode.findPlug("rotateY",&status);
   MPlug rotateZ = depnode.findPlug("rotateZ",&status);

   MPlug scaleX = depnode.findPlug("scaleX",&status);
   MPlug scaleY = depnode.findPlug("scaleY",&status);
   MPlug scaleZ = depnode.findPlug("scaleZ",&status);

   MFnAnimCurve rotcurvex,rotcurvey,rotcurvez;
   MFnAnimCurve trancurvex,trancurvey,trancurvez;
   MFnAnimCurve scalcurvex,scalcurvey,scalcurvez;

   MObject rotcurveobjectx = MObject::kNullObj;
   MObject rotcurveobjecty = MObject::kNullObj;
   MObject rotcurveobjectz = MObject::kNullObj;

   MObject transcurveobjectx = MObject::kNullObj;
   MObject transcurveobjecty = MObject::kNullObj;
   MObject transcurveobjectz = MObject::kNullObj;

   MObject scalcurveobjectx = MObject::kNullObj;
   MObject scalcurveobjecty = MObject::kNullObj;
   MObject scalcurveobjectz = MObject::kNullObj;

   int maxtime,xmaxtime,ymaxtime,zmaxtime;
   Time currenttime;
   double currenttimeinseconds;

   if (framecount_Tx>0)
   {
      xmaxtime = (int)(channel.m_channelitem[0].m_keyframes[framecount_Tx-1].m_time.GetSeconds()*30.0);
      ymaxtime = (int)(channel.m_channelitem[1].m_keyframes[framecount_Ty-1].m_time.GetSeconds()*30.0);
      zmaxtime = (int)(channel.m_channelitem[2].m_keyframes[framecount_Tz-1].m_time.GetSeconds()*30.0);

      MVector translate;

      maxtime = 0;

      if (xmaxtime>maxtime)
         maxtime = xmaxtime;
      if (ymaxtime>maxtime)
         maxtime = ymaxtime;
      if (zmaxtime>maxtime)
         maxtime = zmaxtime;

      if (maxtime>0)
      {
         if (m_applytobones)
         {
            transcurveobjectx = trancurvex.create(translateX,MFnAnimCurve::kAnimCurveTL,NULL,&status);
            transcurveobjecty = trancurvey.create(translateY,MFnAnimCurve::kAnimCurveTL,NULL,&status);
            transcurveobjectz = trancurvez.create(translateZ,MFnAnimCurve::kAnimCurveTL,NULL,&status);
         }
         else
         {
            transcurveobjectx = trancurvex.create(MFnAnimCurve::kAnimCurveTL,NULL,&status);
            transcurveobjecty = trancurvey.create(MFnAnimCurve::kAnimCurveTL,NULL,&status);
            transcurveobjectz = trancurvez.create(MFnAnimCurve::kAnimCurveTL,NULL,&status);
         }
      }

      // gets a translation vector to help in Hash to Maya space conversion
      MMatrix inclusive,exclusive,result;
      MTools::GetInclusiveBindPoseMatrix(dagpath,inclusive);
      MTools::GetExclusiveBindPoseMatrix(dagpath,exclusive);
      result = inclusive * (exclusive.inverse());

      MTransformationMatrix bindmatrix(result);

      for (i = 0; i < (maxtime+1); i++)
      {
         currenttimeinseconds = i/30.0;
         currenttime.SetSeconds(currenttimeinseconds);
         MTime currentmtime(currenttimeinseconds,MTime::kSeconds);
         translate.x = channel.m_channelitem[0].Interpolate(currenttime);	
         translate.y = channel.m_channelitem[1].Interpolate(currenttime);
         translate.z = channel.m_channelitem[2].Interpolate(currenttime);

         // adjust for different coord space


         MQuaternion orientquat;

         MDagPath dagpath(dagpath);
         MFnIkJoint fnjoint(dagpath, &status);
         fnjoint.getOrientation(orientquat);

         translate = translate * orientquat;//THIS IS EVIL!

         translate = translate + bindmatrix.translation(MSpace::kWorld);

         trancurvex.addKey(currentmtime,translate.x,MFnAnimCurve::kTangentSmooth,MFnAnimCurve::kTangentSmooth,NULL,&status);
         trancurvey.addKey(currentmtime,translate.y,MFnAnimCurve::kTangentSmooth,MFnAnimCurve::kTangentSmooth,NULL,&status);
         trancurvez.addKey(currentmtime,translate.z,MFnAnimCurve::kTangentSmooth,MFnAnimCurve::kTangentSmooth,NULL,&status);
      }
   }

   if (framecount_Rx>0)
   {
      xmaxtime = (int)(channel.m_channelitem[6].m_keyframes[framecount_Rx-1].m_time.GetSeconds()*30.0);
      ymaxtime = (int)(channel.m_channelitem[7].m_keyframes[framecount_Ry-1].m_time.GetSeconds()*30.0);
      zmaxtime = (int)(channel.m_channelitem[8].m_keyframes[framecount_Rz-1].m_time.GetSeconds()*30.0);

      int wmaxtime = (int)(channel.m_channelitem[9].m_keyframes[framecount_Rw-1].m_time.GetSeconds()*30.0);

      maxtime = 0;

      if (xmaxtime>maxtime)
         maxtime = xmaxtime;
      if (ymaxtime>maxtime)
         maxtime = ymaxtime;
      if (zmaxtime>maxtime)
         maxtime = zmaxtime;
      if (wmaxtime>maxtime)
         maxtime = wmaxtime;

      if (maxtime>0)
      {
         if (m_applytobones)
         {
            rotcurveobjectx = rotcurvex.create(rotateX,MFnAnimCurve::kAnimCurveTA,NULL,&status);
            rotcurveobjecty = rotcurvey.create(rotateY,MFnAnimCurve::kAnimCurveTA,NULL,&status);
            rotcurveobjectz = rotcurvez.create(rotateZ,MFnAnimCurve::kAnimCurveTA,NULL,&status);
         }
         else
         {
            rotcurveobjectx = rotcurvex.create(MFnAnimCurve::kAnimCurveTA,NULL,&status);
            rotcurveobjecty = rotcurvey.create(MFnAnimCurve::kAnimCurveTA,NULL,&status);
            rotcurveobjectz = rotcurvez.create(MFnAnimCurve::kAnimCurveTA,NULL,&status);
         }
      }

      MEulerRotation	erot;
      MQuaternion quat;

      for (i = 0; i < (maxtime+1); i++)
      {
         currenttimeinseconds = i/30.0;
         currenttime.SetSeconds(currenttimeinseconds);
         MTime currentmtime(currenttimeinseconds,MTime::kSeconds);
         quat.x = channel.m_channelitem[6].Interpolate(currenttime);
         quat.y = channel.m_channelitem[7].Interpolate(currenttime);
         quat.z = channel.m_channelitem[8].Interpolate(currenttime);
         quat.w = channel.m_channelitem[9].Interpolate(currenttime);

         erot = quat.asEulerRotation();
         rotcurvex.addKey(currentmtime,erot.x,MFnAnimCurve::kTangentSmooth,MFnAnimCurve::kTangentSmooth,NULL,&status);
         rotcurvey.addKey(currentmtime,erot.y,MFnAnimCurve::kTangentSmooth,MFnAnimCurve::kTangentSmooth,NULL,&status);
         rotcurvez.addKey(currentmtime,erot.z,MFnAnimCurve::kTangentSmooth,MFnAnimCurve::kTangentSmooth,NULL,&status);
      }
   }

   if (framecount_Sx>0)
   {
      xmaxtime = (int)(channel.m_channelitem[3].m_keyframes[framecount_Sx-1].m_time.GetSeconds()*30.0);
      ymaxtime = (int)(channel.m_channelitem[4].m_keyframes[framecount_Sy-1].m_time.GetSeconds()*30.0);
      zmaxtime = (int)(channel.m_channelitem[5].m_keyframes[framecount_Sz-1].m_time.GetSeconds()*30.0);

      maxtime = 0;

      if (xmaxtime>maxtime)
         maxtime = xmaxtime;
      if (ymaxtime>maxtime)
         maxtime = ymaxtime;
      if (zmaxtime>maxtime)
         maxtime = zmaxtime;

      if (maxtime>0)
      {
         if (m_applytobones)
         {
            scalcurveobjectx = scalcurvex.create(scaleX,MFnAnimCurve::kAnimCurveTU,NULL,&status);
            scalcurveobjecty = scalcurvey.create(scaleY,MFnAnimCurve::kAnimCurveTU,NULL,&status);
            scalcurveobjectz = scalcurvez.create(scaleZ,MFnAnimCurve::kAnimCurveTU,NULL,&status);
         }
         else
         {
            scalcurveobjectx = scalcurvex.create(MFnAnimCurve::kAnimCurveTU,NULL,&status);
            scalcurveobjecty = scalcurvey.create(MFnAnimCurve::kAnimCurveTU,NULL,&status);
            scalcurveobjectz = scalcurvez.create(MFnAnimCurve::kAnimCurveTU,NULL,&status);
         }
      }

      MVector scale;

      for (i = 0; i < (maxtime+1); i++)
      {
         currenttimeinseconds = i/30.0;
         currenttime.SetSeconds(currenttimeinseconds);
         MTime currentmtime(currenttimeinseconds,MTime::kSeconds);
         scale.x = channel.m_channelitem[3].Interpolate(currenttime);	
         scale.y = channel.m_channelitem[4].Interpolate(currenttime);
         scale.z = channel.m_channelitem[5].Interpolate(currenttime);
         scalcurvex.addKey(currentmtime,scale.x,MFnAnimCurve::kTangentSmooth,MFnAnimCurve::kTangentSmooth,NULL,&status);
         scalcurvey.addKey(currentmtime,scale.y,MFnAnimCurve::kTangentSmooth,MFnAnimCurve::kTangentSmooth,NULL,&status);
         scalcurvez.addKey(currentmtime,scale.z,MFnAnimCurve::kTangentSmooth,MFnAnimCurve::kTangentSmooth,NULL,&status);
      }
   }

   AnimChannelMObjects animchannel;

   // translation
   animchannel.transcurveobjectx = transcurveobjectx;
   animchannel.transcurveobjecty = transcurveobjecty;
   animchannel.transcurveobjectz = transcurveobjectz;

   //rotation
   animchannel.rotcurveobjectx = rotcurveobjectx;
   animchannel.rotcurveobjecty = rotcurveobjecty;
   animchannel.rotcurveobjectz = rotcurveobjectz;

   // scale
   animchannel.scalcurveobjectx = scalcurveobjectx;
   animchannel.scalcurveobjecty = scalcurveobjecty;
   animchannel.scalcurveobjectz = scalcurveobjectz;

   m_animchannels.push_back(animchannel);

   m_bonemap.AddKey((int)MTools::GetMPtrBase(depnode),m_animchannels.size()-1);

   return 1;
}

MStatus KFMTranslator::HookUpSourceClip(MObject object,
                                        const MSelectionList &bones,
                                        MObject &characterobject,
                                        MString sourceclipname,
                                        MObject &sourceclipobject)
{
   MStatus status;

   bool replacesourceclip = false;

   MFnDagNode dagnode(object);
   MDagPath dagpath;
   dagnode.getPath(dagpath);

   MDagPath rootjointdagpath;
   MTools::GetRootBone(dagpath,rootjointdagpath);

   // set character to bindpose
   // not sure if this is necessary? better safe than sorry
#if 0
   MObject bindposeobject;
   MTools::GetBindPoseFromJoint(rootjointdagpath,bindposeobject);
   MFnDependencyNode bindposedepnode(bindposeobject);
   const char *bindposedebug = bindposedepnode.name().asChar();
   MString bindPoseName = bindposedepnode.name();
   MGlobal::executeCommand(MString("dagPose -restore ") + bindPoseName);
   dgModifier.doIt();
#endif

   // find character
   MDGModifier mDgModifier;
   MFnCharacter mFnCharacter;
   MObjectArray characterobjects;

   // evaluate selection as to how to get character node
   // may need to create new one based on the above setting or if there is wasn't one created previously
   if (object.apiType() == MFn::kClip)//replace clip with character already existing
   {
      MTools::GetCharacterFromClip(object,characterobject);
      replacesourceclip = true;
   }
   else if (object.apiType() == MFn::kCharacter)// add clip to previously created character
   {
      characterobject = object;
      mFnCharacter.setObject(characterobject);
      status = mFnCharacter.addMembers(bones);
      if (!status)
         return MStatus::kFailure;
   }
   else if ((!MTools::FindCharacters(rootjointdagpath,characterobjects))||(m_forcenewcharacter))// create character from selected skeleton
   {
      // create character
      characterobject = mFnCharacter.create(bones,MFnSet::kNone,&status);
      if (!status)
         return MStatus::kFailure;
   }
   else
   {
      characterobject = characterobjects[0];
      mFnCharacter.setObject(characterobject);
      status = mFnCharacter.addMembers(bones);
      if (!status)
         return MStatus::kFailure;
   }

   mFnCharacter.setObject(characterobject);

   // creates clip in Visor under unused clips
   if (!replacesourceclip)
   {
      // create clip
      MFnClip fnclip;

      MTime start(0.0,MTime::kSeconds),duration(m_length.GetSeconds(),MTime::kSeconds);
      sourceclipobject = fnclip.createSourceClip(start,duration,mDgModifier,&status);
      if (!status)
         return MStatus::kFailure;

      mDgModifier.renameNode(sourceclipobject,sourceclipname);
      mDgModifier.doIt();

      // attach source to character
      status = mFnCharacter.attachSourceToCharacter(sourceclipobject,mDgModifier);
      if (!status)
         return MStatus::kFailure;
   }
   // replaces the source clip currently selected in the Trax editor
   else
   {
      MFnClip fninstanceclip(object);
      sourceclipobject = fninstanceclip.sourceClip(&status);
      if (!status)
         return MStatus::kFailure;

      MFnClip fnsourceclip(sourceclipobject);

      //		MSelectionList members;
      //		status = mFnCharacter.getMembers(members,true);
      //		if (!status)
      //			RETURN_ERROR()

      //		status = mFnCharacter.removeMembers(members);	
      //		if (!status)
      //			RETURN_ERROR()

      status = mFnCharacter.addMembers(bones);
      if (!status)
         return MStatus::kFailure;

      mDgModifier.renameNode(sourceclipobject,sourceclipname);
      mDgModifier.doIt();	
   }

   return MS::kSuccess;
}

void KFMTranslator::HookUpAnimCurvesToSourceClip(MObject characterobject,MObject sourceclipobject)
{
   MStatus status;
   MDGModifier mDgModifier;

   MFnCharacter mFnCharacter;
   mFnCharacter.setObject(characterobject);

   MPlugArray memberplugs;
   mFnCharacter.getMemberPlugs(memberplugs);
   if (!status)
      return;

   for (int i = 0; i < (int)memberplugs.length(); i+=10)
   {
      MString plugname = memberplugs[i].name();
      const char *debug = plugname.asChar();

      MFnDependencyNode mFnDepNode(memberplugs[i].node());

      int nIndex;
      // lookup animchannel from bone
      if (m_bonemap.FindKey((int)MTools::GetMPtrBase(mFnDepNode),nIndex))
      {
         AnimChannelMObjects &animchannel = m_animchannels[nIndex];

         if (animchannel.transcurveobjectx!=MObject::kNullObj)
            status = mFnCharacter.addCurveToClip(animchannel.transcurveobjectx,sourceclipobject,memberplugs[i+1],mDgModifier);
   
         if (animchannel.transcurveobjecty!=MObject::kNullObj)
            status = mFnCharacter.addCurveToClip(animchannel.transcurveobjecty,sourceclipobject,memberplugs[i+2],mDgModifier);

         if (animchannel.transcurveobjectz!=MObject::kNullObj)
            status = mFnCharacter.addCurveToClip(animchannel.transcurveobjectz,sourceclipobject,memberplugs[i+3],mDgModifier);

         if (animchannel.rotcurveobjectx!=MObject::kNullObj)
            status = mFnCharacter.addCurveToClip(animchannel.rotcurveobjectx,sourceclipobject,memberplugs[i+4],mDgModifier);
  
         if (animchannel.rotcurveobjecty!=MObject::kNullObj)
            status = mFnCharacter.addCurveToClip(animchannel.rotcurveobjecty,sourceclipobject,memberplugs[i+5],mDgModifier);

         if (animchannel.rotcurveobjectz!=MObject::kNullObj)
            status = mFnCharacter.addCurveToClip(animchannel.rotcurveobjectz,sourceclipobject,memberplugs[i+6],mDgModifier);

         if (animchannel.scalcurveobjectx!=MObject::kNullObj)
            status = mFnCharacter.addCurveToClip(animchannel.scalcurveobjectx,sourceclipobject,memberplugs[i+7],mDgModifier);
     
         if (animchannel.scalcurveobjecty!=MObject::kNullObj)
            status = mFnCharacter.addCurveToClip(animchannel.scalcurveobjecty,sourceclipobject,memberplugs[i+8],mDgModifier);
   
         if (animchannel.scalcurveobjectz!=MObject::kNullObj)
            status = mFnCharacter.addCurveToClip(animchannel.scalcurveobjectz,sourceclipobject,memberplugs[i+9],mDgModifier);
      }
   }
}


int KFMTranslator::ReadKFM(const char *filename)
{
   int version;
   WORD channelcount;
   float seconds;
   char line[256];
   FILE *fp;
   int err;

   fp = fopen(filename,"rb");

   if (fp == NULL)
      return 0;

   fread(&version,sizeof(int),1,fp);
   EOF_ERROR();

   if (version != KFMVERSION)
      return 0;

   fread(&seconds,sizeof(float),1,fp);
   EOF_ERROR();

   m_length.SetSeconds(seconds);

   fread(&channelcount,sizeof(WORD),1,fp);
   EOF_ERROR();

   m_channellist.clear();

   m_channellist.resize(channelcount);

   for (int i = 0; i < channelcount; i++)
   {
      Channel &channel = m_channellist[i];

      err = ReadString(fp,line,256);
      IF_ERROR(err);

      strcpy(channel.m_name,line);

      Vector bonedirection,rolldirection,parentbonedirection,parentrolldirection;

      err = ReadVector(fp,bonedirection);
      IF_ERROR(err);
      err = ReadVector(fp,rolldirection);
      IF_ERROR(err);
      err = ReadVector(fp,parentbonedirection);
      IF_ERROR(err);
      err = ReadVector(fp,parentrolldirection);
      IF_ERROR(err);

      channel.ComputeParentToBone(bonedirection, rolldirection,parentbonedirection, parentrolldirection);

      for (int j = 0; j < Channel::NUMCHANNELITEMS; j++)
      {
         ChannelItem &chanitem = channel.m_channelitem[j];
         WORD numkf;
         fread(&numkf,sizeof(WORD),1,fp);// Number of keyframes
         EOF_ERROR();

         chanitem.m_keyframes.resize(numkf);

         for (int k = 0; k < numkf; k++)
         {
            Keyframe &kf = chanitem.m_keyframes[k];

            fread(&seconds,sizeof(float),1,fp);
            EOF_ERROR();

            kf.m_time.SetSeconds(seconds);

            fread(&kf.m_value,sizeof(float),1,fp);
            EOF_ERROR();

            fread(&kf.m_flags,sizeof(unsigned char),1,fp);
            EOF_ERROR();

            if (!kf.m_defaulttangents)
            {
               err = ReadVector2(fp,kf.m_it);
               IF_ERROR(err);
               err = ReadVector2(fp,kf.m_ot);
               IF_ERROR(err);
            }
         }

         // build default tangents now
         for (k=0; k < numkf; k++)
         {
            Keyframe &kf = chanitem.m_keyframes[k];
            if (kf.m_defaulttangents)
               chanitem.CalcTangents(k);
         }
      }
   }

   fclose(fp);

   return 1;
}

int KFMTranslator::ReadVector(FILE *fp,Vector &vect)
{
   float x,y,z;
   fread(&x,sizeof(float),1,fp);
   if (feof(fp))
      return 0;
   fread(&y,sizeof(float),1,fp);
   if (feof(fp))
      return 0;
   fread(&z,sizeof(float),1,fp);
   if (feof(fp))
      return 0;
   vect.x = x;
   vect.y = y;
   vect.z = z;
   return 1;
}

int KFMTranslator::ReadVector2(FILE *fp,Vector2 &vect)
{
   float x,y;
   fread(&x,sizeof(float),1,fp);
   if (feof(fp))
      return 0;
   fread(&y,sizeof(float),1,fp);
   if (feof(fp))
      return 0;
   vect.x = x;
   vect.y = y;
   return 1;
}

int KFMTranslator::ReadString(FILE *fp,char *str,int len)
{
   char c;
   int cnt = 0;
   do {
      c = fgetc(fp);
      str[cnt++] = c;
      if (cnt == len)
         break;
      if (feof(fp))
         return 0;
   } while (c!='\0');
   return 1;
}

