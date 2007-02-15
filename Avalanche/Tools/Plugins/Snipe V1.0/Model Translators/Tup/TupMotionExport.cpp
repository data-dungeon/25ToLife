#include "stdafx.h"
#include "Tup.h"
#include "HMotion.h"
#include "HMotionChannel.h"
#include "HMotionCurve.h"
#include "HMotionKey.h"
#include "HMotionCurveDrivers.h"
#include "..\Utility\Vector2.h"
#include "HTimeProperty.h"

#include "ExportTupMotionOptionsDlg.h"

void AddUnits(TupScene *pTupScene)
{
   int unitFlags = 0;
   unitFlags |= TupScene::TIME_UNIT_SECONDS;// we convert to this
   unitFlags |= TupScene::LINEAR_UNIT_METER; // we convert to this
   unitFlags |= TupScene::ANGULAR_UNIT_RADIANS; // we convert to this
   unitFlags |= TupScene::UP_AXIS_Y; // use native format
   pTupScene->AddUnitFlags(unitFlags);
}

void AddAnimationGlobals(TupScene *pTupScene, HMotion *pMotion)
{
   TupperwareAggregate *pAnimGlobalsAgg = pTupScene->AddAnimGlobals();
   TupAnimGlobals tupAnimGlobals(pAnimGlobalsAgg);

   tupAnimGlobals.AddStartTime((float)pMotion->GetStartTime().GetSeconds());
   tupAnimGlobals.AddEndTime((float)pMotion->GetStopTime().GetSeconds());

   tupAnimGlobals.AddStartPlaybackTime((float)pMotion->GetPlaybackStart()->GetValue().GetSeconds());
   tupAnimGlobals.AddEndPlaybackTime((float)pMotion->GetPlaybackStop()->GetValue().GetSeconds());

/*
   tupAnimGlobals.AddCurrentTime((float)MAnimControl::currentTime().value());

   int nFlags = 0;
   nFlags |= TupAnimGlobals::FLAGS_USE_MAYA_BONE_TRANSLATION_SPACE;// this flag is set, if animation is exporting from Maya
   tupAnimGlobals.AddFlags(nFlags);
*/
}

void TupMotionTranslator::OnExport( HMotion *pMotion, const char *szFileName)
{
   ExportTupMotionOptionsDlg dlg;
   if (dlg.DoModal() != IDOK)
      return;

   // create a new scene
   m_pSceneAgg = new TupperwareAggregate;
   m_pTupScene = new TupScene(m_pSceneAgg);

   // create the animation set pool
   TupperwareAggregate *pAnimSetPoolAgg = m_pTupScene->AddAnimSetPool();

   // add in the date, time and version
   m_pTupScene->AddExporterDate(g_pExporterDate);
   m_pTupScene->AddExporterTime(g_pExporterTime);
   m_pTupScene->AddVersion(TupScene::GetSceneVersion());
   AddUnits(m_pTupScene);

   AddAnimationGlobals(m_pTupScene,pMotion);
   AddMotion(pMotion,pAnimSetPoolAgg);

   // save the tree
   TupperwareLoadSaveManager::FormatType formatType = dlg.m_bBinary ? TupperwareLoadSaveManager::BINARY : TupperwareLoadSaveManager::ASCII;
   int nResult = TupperwareLoadSaveManager::Write(formatType,szFileName,m_pSceneAgg);

   // delete tree
   delete m_pSceneAgg;
   delete m_pTupScene;
}

void TupMotionTranslator::AddMotion(HMotion *pMotion,TupperwareAggregate *pAnimSetPoolAgg)
{
   for (UINT nMotionChannelIndex = 0; nMotionChannelIndex < pMotion->GetNumMotionChannels(); nMotionChannelIndex++)
   {
      AddMotionChannel(pMotion->GetMotionChannelAt(nMotionChannelIndex),pAnimSetPoolAgg);
   }
}

void TupMotionTranslator::AddMotionChannel(HMotionChannel *pMotionChannel,TupperwareAggregate *pAnimSetPoolAgg)
{
   TupAnimSetPool tupAnimSetPool(pAnimSetPoolAgg);
   String channelname = pMotionChannel->GetName();

   int nAnimSetIndex = tupAnimSetPool.AddAnimSet();
   TupperwareAggregate *pAnimSetAgg = tupAnimSetPool.GetAnimSet(nAnimSetIndex);
   TupAnimSet *pTupAnimSet = new TupAnimSet(pAnimSetAgg);
   pTupAnimSet->SetAnimSetType(TupAnimSet::GLOBAL);
   pTupAnimSet->AddName(channelname.Get());

//   AddReusableActionInfo(pAnimSetAgg,nodeDagPath);

//   AddLocalToParentBindMatrix(pAnimSetAgg,nodeDagPath);

//   AddJointOrientationQuaternion(pAnimSetAgg,nodeDagPath);

   TupperwareAggregate *pAnimChannelPoolAgg = pTupAnimSet->AddChannels();
   HVectorMotionCurveDriver *pVectorMotionCurveDriver = (HVectorMotionCurveDriver*)pMotionChannel->GetMotionCurveDriver("Transform.Translate");
   AddMotionCurve("translate.translateX",(HFloatMotionCurve*)pVectorMotionCurveDriver->GetMotionCurveAt(0),TupAnimCurve::FLAGS_OUTPUT_TYPE_LINEAR,pAnimChannelPoolAgg);
   AddMotionCurve("translate.translateY",(HFloatMotionCurve*)pVectorMotionCurveDriver->GetMotionCurveAt(1),TupAnimCurve::FLAGS_OUTPUT_TYPE_LINEAR,pAnimChannelPoolAgg);
   AddMotionCurve("translate.translateZ",(HFloatMotionCurve*)pVectorMotionCurveDriver->GetMotionCurveAt(2),TupAnimCurve::FLAGS_OUTPUT_TYPE_LINEAR,pAnimChannelPoolAgg);

   HQuaternionMotionCurveDriver *pQuaternionMotionCurveDriver = (HQuaternionMotionCurveDriver*)pMotionChannel->GetMotionCurveDriver("Transform.Rotate");
   AddMotionCurve("rotate.rotateX",(HFloatMotionCurve*)pQuaternionMotionCurveDriver->GetMotionCurveAt(0),TupAnimCurve::FLAGS_OUTPUT_TYPE_ANGULAR,pAnimChannelPoolAgg);
   AddMotionCurve("rotate.rotateY",(HFloatMotionCurve*)pQuaternionMotionCurveDriver->GetMotionCurveAt(1),TupAnimCurve::FLAGS_OUTPUT_TYPE_ANGULAR,pAnimChannelPoolAgg);
   AddMotionCurve("rotate.rotateZ",(HFloatMotionCurve*)pQuaternionMotionCurveDriver->GetMotionCurveAt(2),TupAnimCurve::FLAGS_OUTPUT_TYPE_ANGULAR,pAnimChannelPoolAgg);
   AddMotionCurve("rotate.rotateW",(HFloatMotionCurve*)pQuaternionMotionCurveDriver->GetMotionCurveAt(3),TupAnimCurve::FLAGS_OUTPUT_TYPE_ANGULAR,pAnimChannelPoolAgg);

   pVectorMotionCurveDriver = (HVectorMotionCurveDriver*)pMotionChannel->GetMotionCurveDriver("Transform.Scale");
   AddMotionCurve("scale.scaleX",(HFloatMotionCurve*)pVectorMotionCurveDriver->GetMotionCurveAt(0),TupAnimCurve::FLAGS_OUTPUT_TYPE_UNITLESS,pAnimChannelPoolAgg);
   AddMotionCurve("scale.scaleY",(HFloatMotionCurve*)pVectorMotionCurveDriver->GetMotionCurveAt(1),TupAnimCurve::FLAGS_OUTPUT_TYPE_UNITLESS,pAnimChannelPoolAgg);
   AddMotionCurve("scale.scaleZ",(HFloatMotionCurve*)pVectorMotionCurveDriver->GetMotionCurveAt(2),TupAnimCurve::FLAGS_OUTPUT_TYPE_UNITLESS,pAnimChannelPoolAgg);
}

void TupMotionTranslator::AddMotionCurve(const char *attributename,HFloatMotionCurve *pFloatMotionCurve,TupAnimCurve::CurveFlags nCurveFlags,TupperwareAggregate *pAnimChannelPoolAgg)
{
   TupAnimChannelPool tupAnimChannelPool(pAnimChannelPoolAgg);
   int nAnimChannelIndex = tupAnimChannelPool.AddAnimChannel();
   TupperwareAggregate *pAnimChannelAgg = tupAnimChannelPool.GetAnimChannel(nAnimChannelIndex);
   TupAnimChannel tupAnimChannel(pAnimChannelAgg);

   tupAnimChannel.AddAttribute(attributename);

   TupperwareAggregate *pAnimCurvePoolAgg = tupAnimChannel.AddCurves();
   TupAnimCurvePool tupAnimCurvePool(pAnimCurvePoolAgg);
   int nAnimCurveIndex = tupAnimCurvePool.AddAnimCurve();
   TupperwareAggregate *pAnimCurveAgg = tupAnimCurvePool.GetAnimCurve(nAnimCurveIndex);
   TupAnimCurve tupAnimCurve(pAnimCurveAgg);
   
   int nFlags = 0;
   nFlags |= TupAnimCurve::FLAGS_INPUT_TYPE_TIME;
   nFlags |= nCurveFlags;
   tupAnimCurve.AddFlags(nFlags);

   UINT nNumKeys = pFloatMotionCurve->CountNumKeys();

   TupArray<float> inputValues; inputValues.SetSize(nNumKeys);
   TupArray<float> outputValues; outputValues.SetSize(nNumKeys);
   TupArray<int> keyFlags; keyFlags.SetSize(nNumKeys);

   TupArray<float> inTangentX; inTangentX.SetSize(nNumKeys);
   TupArray<float> inTangentY; inTangentY.SetSize(nNumKeys);
   TupArray<float> outTangentX; outTangentX.SetSize(nNumKeys);
   TupArray<float> outTangentY; outTangentY.SetSize(nNumKeys);

   UINT nKeyIndex = 0;
   for (HFloatMotionKey *pMotionKey = (HFloatMotionKey*)pFloatMotionCurve->GetHeadKey(); pMotionKey; pMotionKey = (HFloatMotionKey*)pMotionKey->GetNextKey(),nKeyIndex++)
   {
      inputValues[nKeyIndex] = pMotionKey->GetTime().GetSeconds();// convert to seconds

      if (nCurveFlags & TupAnimCurve::FLAGS_OUTPUT_TYPE_LINEAR)
      {
         outputValues[nKeyIndex] = pMotionKey->GetValue() * 0.01f;
      }
      else if (nCurveFlags & TupAnimCurve::FLAGS_OUTPUT_TYPE_ANGULAR)
      {
         outputValues[nKeyIndex]  = pMotionKey->GetValue() * (float)DEGTORAD;
      }
 
      int nKeyFlag = 0;

      switch (pMotionKey->GetInterpolationMethod())
      {
      case HMotionKey::LINEAR: 
         {
            nKeyFlag |= TupAnimCurve::FLAGS_IN_TANGENT_TYPE_LINEAR;
            nKeyFlag |= TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_LINEAR;
        }
         break;
      case HMotionKey::SPLINE:
         {
            nKeyFlag |= TupAnimCurve::FLAGS_IN_TANGENT_TYPE_SMOOTH;
            nKeyFlag |= TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_SMOOTH;
         }
         break;
      case HMotionKey::HOLD:
         {
            nKeyFlag |= TupAnimCurve::FLAGS_IN_TANGENT_TYPE_STEP;
            nKeyFlag |= TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_STEP;
         }
         break;
      }

      keyFlags[nKeyIndex] = nKeyFlag;

      inTangentX[nKeyIndex] = pMotionKey->GetInTangent().x;
      inTangentX[nKeyIndex] = pMotionKey->GetInTangent().y;
      outTangentX[nKeyIndex] = pMotionKey->GetOutTangent().x;
      outTangentX[nKeyIndex] = pMotionKey->GetOutTangent().y;
   }

   if (nNumKeys)
   {
      float startTime = inputValues[0];
      float endTime = inputValues[nNumKeys-1];
      tupAnimCurve.AddStartTime(startTime);
      tupAnimCurve.AddEndTime(endTime);

      tupAnimCurve.AddInputValues(inputValues.GetData(),nNumKeys);
      tupAnimCurve.AddOutputValues(outputValues.GetData(),nNumKeys);
      tupAnimCurve.AddKeyFlags(keyFlags.GetData(),nNumKeys);

      tupAnimCurve.AddInTangentX(inTangentX.GetData(),nNumKeys);
      tupAnimCurve.AddInTangentY(inTangentY.GetData(),nNumKeys);
      tupAnimCurve.AddOutTangentX(outTangentX.GetData(),nNumKeys);
      tupAnimCurve.AddOutTangentY(outTangentY.GetData(),nNumKeys);
   }
}