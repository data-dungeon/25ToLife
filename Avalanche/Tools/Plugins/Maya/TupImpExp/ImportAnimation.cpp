///////////////////////////////////////////////////////////////////////////
//
// ImportAnimation
//
// Tupperware importer/ exporter for Maya
//
////////////////////////////////////////////////////////////////////////////

#include "TupImpExpPch.h"
#include "TupImport.h"
#include "TupAnimImport.h"

// Pass bHookUpCurves as true if you want Animation Curves hooked up to Joint, pass false
// if you want dont want the curves to be hooked up and you intend to hook up curves via a clip.
void TupAnimImport::BuildAnimationData(const MDagPath &rootDagPath,
                                       TupperwareAggregate *pAnimSetPoolAgg,
                                       TupArray<JointCurves> &jointCurveArray,
                                       bool bHookUpCurves)
{
   TupAnimSetPool tupAnimSetPool(pAnimSetPoolAgg);
   MDagPath dagPath;
   int nNumAnimSets = tupAnimSetPool.GetNumAnimSets();
   for (int nAnimSetIndex = 0; nAnimSetIndex < nNumAnimSets; nAnimSetIndex++)
   {
      TupperwareAggregate *pAnimSetAgg = tupAnimSetPool.GetAnimSet(nAnimSetIndex);
      JointCurves *pJointCurves = NULL;
      if (!bHookUpCurves)
      {
         int nIndex = jointCurveArray.Add(JointCurves());
         pJointCurves = &jointCurveArray.GetAt(nIndex);
      }
      BuildJointCurves(rootDagPath,pAnimSetAgg,pJointCurves);
   }
}

// same as MTools. eliminated joint dependency
bool FindBoneByName(const char *name,const MDagPath &dagPath,MDagPath &result)
{
   MFnDependencyNode depNode(dagPath.node());
   if (depNode.name()==name)
   {
      result = dagPath;
      return true;
   }
   // recurse through children for match
   UINT nNumChildren = dagPath.childCount();
   for (UINT i = 0; i < nNumChildren; i++)
   {
      MDagPath childPath(dagPath);
      childPath.push(dagPath.child(i));

      MDagPath founddagpath;
      if (FindBoneByName(name,childPath,result))
         return true;
   }
   return false;
}

void TupAnimImport::BuildJointCurves(const MDagPath &rootDagPath,TupperwareAggregate *pAnimSetAgg,JointCurves *pJointCurves)
{
   MStatus status;
   TupAnimSet tupAnimSet(pAnimSetAgg);

   bool bHookUpCurves = true;

   char *name;
   tupAnimSet.GetName(&name);
   MStringArray parts;
   MString fullPathName = name;
   fullPathName.split('|',parts);

   MDagPath dagPath;
   // try to hook up by name
   if ( !FindBoneByName ( parts[parts.length()-1].asChar(), rootDagPath, dagPath) )
      return;

   if (pJointCurves)
   {
      pJointCurves->m_object = dagPath.node();
      bHookUpCurves = false;
   }

   TupperwareAggregate *pChannelsAgg = tupAnimSet.GetChannels();
   TupAnimChannelPool tupAnimChannelPool(pChannelsAgg);

   int nNumAnimChannels = tupAnimChannelPool.GetNumAnimChannels();
   for (int nAnimChannelIndex = 0; nAnimChannelIndex < nNumAnimChannels; nAnimChannelIndex++)
   {
      TupperwareAggregate *pAnimChannelAgg = tupAnimChannelPool.GetAnimChannel(nAnimChannelIndex);
      TupAnimChannel tupAnimChannel(pAnimChannelAgg);

      char *attribute;
      tupAnimChannel.GetAttribute(&attribute);
      MString temp = attribute;
      MStringArray parts;
      temp.split('.',parts);
      MFnDependencyNode depNode(dagPath.node());
      MPlug attributeplug = depNode.findPlug(parts[parts.length()-1],&status);
      if (!status)
         return;

      JointCurve *pJointCurve = NULL;
      if (pJointCurves)
      {
         int nIndex = pJointCurves->m_curves.Add(JointCurve());
         pJointCurve = &pJointCurves->m_curves.GetAt(nIndex);
         pJointCurve->m_attribute = attributeplug.attribute();
      }
 
      MObject curveobject;

      TupperwareAggregate *pAnimCurvePoolAgg = tupAnimChannel.GetCurves();
      TupAnimCurvePool tupAnimCurvePool(pAnimCurvePoolAgg);
      int nNumAnimCurves = tupAnimCurvePool.GetNumAnimCurves();
      for (int nAnimCurveIndex = 0; nAnimCurveIndex < nNumAnimCurves; nAnimCurveIndex++)
      {
         TupperwareAggregate *pAnimCurveAgg = tupAnimCurvePool.GetAnimCurve(nAnimCurveIndex);
         curveobject = BuildAnimCurve(dagPath,attributeplug.attribute(),pAnimCurveAgg,bHookUpCurves);
         if (pJointCurve)
         {
            TupAnimCurve tupAnimCurve(pAnimCurveAgg);
            float fValue;
            tupAnimCurve.GetStartTime(fValue);
            pJointCurve->m_starttime = MTime(fValue,MTime::kSeconds);
            tupAnimCurve.GetEndTime(fValue);
            pJointCurve->m_stoptime = MTime(fValue,MTime::kSeconds);
            pJointCurve->m_curveobject = curveobject;
         }
      }
   }
}

