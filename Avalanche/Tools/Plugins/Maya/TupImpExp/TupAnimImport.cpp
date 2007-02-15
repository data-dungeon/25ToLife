////////////////////////////////////////////////////////////////////////////
//
// TupAnimImport
//
// Tupperware animation importer
//
////////////////////////////////////////////////////////////////////////////

#include "TupImpExpPch.h"
#include "TupAnimImport.h"

#include "IntHash.h"

TupAnimImport::TupAnimImport()
{
}

MDagPath WalkToTop(MDagPath dagPath)
{
   MDagPath parent = dagPath;
   parent.pop();
   if (parent.apiType()==MFn::kWorld)
      return dagPath;
   return WalkToTop(dagPath);
}

MStatus TupAnimImport::ImportAnimation(const TCHAR *inputFilename,bool bImportAsClip)
{
   MStatus status;

   int result = TupperwareLoadSaveManager::Read(inputFilename,&m_pSceneAgg);
   if (!result)
      return MS::kFailure;
   if (!m_pSceneAgg->GetFirstChildAtom())
      return MS::kFailure;
   m_pTupScene = new TupScene(m_pSceneAgg);

   TupperwareAggregate *pAnimGlobalsAgg = m_pTupScene->GetAnimGlobals();
   TupAnimGlobals tupAnimGlobals(pAnimGlobalsAgg);
   int nAnimGlobalFlags;
   tupAnimGlobals.GetFlags(nAnimGlobalFlags);
   if (!(nAnimGlobalFlags & TupAnimGlobals::FLAGS_USE_MAYA_BONE_TRANSLATION_SPACE))
   {
      MGlobal::displayError("Importer for Maya currently only supports Tup Animations exported from Maya.\n");
      return MS::kFailure;
   }

   TupperwareAggregate *pAnimSetPoolAgg = m_pTupScene->GetAnimSetPool();
   assert(pAnimSetPoolAgg);
   TupAnimSetPool tupAnimSetPool(pAnimSetPoolAgg);

   // get active selection
   MSelectionList activeselection;
   status = MGlobal::getActiveSelectionList(activeselection);

   MDagPath dagPath;
   MObject object;
   activeselection.getDagPath(0,dagPath);
   activeselection.getDependNode(0,object);

   // find root bone
   MDagPath rootJointDagPath;
   if (object.apiType() == MFn::kClip)
      MTools::GetRootBoneFromClip(object,rootJointDagPath);
   else if (object.apiType() == MFn::kCharacter)
      MTools::GetRootBoneFromCharacter(object,rootJointDagPath);
   else if (object.apiType() == MFn::kJoint)
      MTools::GetRootBone(dagPath,rootJointDagPath);
   else
      rootJointDagPath = WalkToTop(dagPath);

   TupArray<JointCurves> jointCurveArray;
   // if not creating a clip hook up animation curves to joints, otherwise hand back data
   // for hooking up to a clip.
   BuildAnimationData(rootJointDagPath,pAnimSetPoolAgg,jointCurveArray,!bImportAsClip);

   if (bImportAsClip)
   {
      MObject characterobject,sourceclipobject;

      if (!HookUpCharacter(object,characterobject,inputFilename,sourceclipobject,jointCurveArray))
         return MS::kFailure;

      HookUpAnimCurvesToSourceClip(characterobject,sourceclipobject,jointCurveArray);
   }
   return status;
}

MStatus TupAnimImport::HookUpCharacter(MObject object,
                                        MObject &characterobject,
                                        MString sourceclipname,
                                        MObject &sourceclipobject,
                                        TupArray<JointCurves> &jointCurveArray)
{
   MStatus status;

   // figure out start and end time of animation
   MTime starttime = jointCurveArray[0].GetStartTime();
   for (int i = 1; i < jointCurveArray.GetSize(); i++)
   {
      MTime temp = jointCurveArray[i].GetStartTime();
      if (temp< starttime)
         starttime = temp;
   }
   MTime stoptime = jointCurveArray[0].GetStopTime();
   for (i = 1; i < jointCurveArray.GetSize(); i++)
   {
      MTime temp = jointCurveArray[i].GetStopTime();
      if (temp > stoptime)
         stoptime = temp;
   }

   // create selectionlist for adding members to character
   MSelectionList bones;
   for (int nJointIndex = 0; nJointIndex < jointCurveArray.GetSize(); nJointIndex++)
   {
      JointCurves *pJointCurves = &jointCurveArray.GetAt(nJointIndex);
      bones.add(pJointCurves->m_object);
   }

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
   MDGModifier dgModifier;
   MFnCharacter fnCharacter;
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
      fnCharacter.setObject(characterobject);
      status = fnCharacter.addMembers(bones);
      if (!status)
         return MStatus::kFailure;
   }
//   else if ((!MTools::FindCharacters(rootjointdagpath,characterobjects))||(m_forcenewcharacter))// create character from selected skeleton
   else if (!MTools::FindCharacters(rootjointdagpath,characterobjects))// create character from selected skeleton
   {
      // create character
      characterobject = fnCharacter.create(bones,MFnSet::kNone,&status);
      if (!status)
         return MStatus::kFailure;
   }
   else
   {
      characterobject = characterobjects[0];
      fnCharacter.setObject(characterobject);
      status = fnCharacter.addMembers(bones);
      if (!status)
         return MStatus::kFailure;
   }

   fnCharacter.setObject(characterobject);

   // creates clip in Visor under unused clips
   if (!replacesourceclip)
   {
      // create clip
      MFnClip fnclip;

      MTime duration = stoptime - starttime;
      sourceclipobject = fnclip.createSourceClip(starttime,duration,dgModifier,&status);
      if (!status)
         return MStatus::kFailure;

      dgModifier.renameNode(sourceclipobject,sourceclipname);
      dgModifier.doIt();

      // attach source to character
      status = fnCharacter.attachSourceToCharacter(sourceclipobject,dgModifier);
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

      status = fnCharacter.addMembers(bones);
      if (!status)
         return MStatus::kFailure;

      dgModifier.renameNode(sourceclipobject,sourceclipname);
      dgModifier.doIt();	
   }

   return MS::kSuccess;
}


void TupAnimImport::HookUpAnimCurvesToSourceClip(MObject characterobject,MObject sourceclipobject,TupArray<JointCurves> &jointCurveArray)
{
   MStatus status;
   MDGModifier dgModifier;

   MFnCharacter fnCharacter(characterobject);

   MPlugArray memberplugs;
   fnCharacter.getMemberPlugs(memberplugs);
   if (!status)
      return;

   int nJointIndex,nJointCurveIndex;

   // build hash table to speed things up
   IntHash jointmap;
   for (nJointIndex = 0; nJointIndex < jointCurveArray.GetSize(); nJointIndex++)
   {
      JointCurves *pJointCurves = &jointCurveArray.GetAt(nJointIndex);
      MFnDependencyNode depNode(pJointCurves->m_object);
      jointmap.AddKey((int)MTools::GetMPtrBase(depNode),nJointIndex);
   }

   // add animation curves to clip
   for (int nMemberIndex = 0; nMemberIndex < (int)memberplugs.length(); nMemberIndex++)
   {
      MFnDependencyNode depNode(memberplugs[nMemberIndex].node());
      if (jointmap.FindKey((int)MTools::GetMPtrBase(depNode),nJointIndex))
      {
         JointCurves *pJointCurves = &jointCurveArray.GetAt(nJointIndex);

         for (nJointCurveIndex = 0; nJointCurveIndex < pJointCurves->m_curves.GetSize(); nJointCurveIndex++)
         {
            JointCurve *pJointCurve = &pJointCurves->m_curves[nJointCurveIndex];
            if (pJointCurve->m_attribute == memberplugs[nMemberIndex].attribute())
            {
               status = fnCharacter.addCurveToClip(pJointCurve->m_curveobject,sourceclipobject,memberplugs[nMemberIndex],dgModifier);
               if (!status)
                  MGlobal::displayWarning("Couldn't connect up source clip to anim curve.\n");   
            }
         }
      }
   }
}