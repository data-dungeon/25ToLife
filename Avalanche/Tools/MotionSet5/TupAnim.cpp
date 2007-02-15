#include "stdafx.h"
#include "TupAnim.h"
#include "KFM.h"
#include "TokenizeString.h" 
#include "HashQuaternion.h"
#include "HashRotate.h"
#include "HashMatrix4x4.h"

bool TupAnim::LoadAndConvertFile(const char *filename,KFM *pKFM)
{
   int result = TupperwareLoadSaveManager::Read(filename,&m_pSceneAgg);
   if (!result)
      return false;
   if (!m_pSceneAgg->GetFirstChildAtom())
      return false;

   m_pTupScene = new TupScene(m_pSceneAgg);

   int nFlags;
   m_pTupScene->GetUnitFlags(nFlags);

   m_fFPS = 30.0f;
 
   m_fUnitFactor = 1.0f;
   if (nFlags & TupScene::LINEAR_UNIT_METER)
      m_fUnitFactor = 100.0f;
   else
      printf("Unknown unit type.\n");

   TupperwareAggregate *pAnimSetPoolAgg = m_pTupScene->GetAnimSetPool();
   assert(pAnimSetPoolAgg);
   TupAnimSetPool tupAnimSetPool(pAnimSetPoolAgg);

   int nNumAnimSets = tupAnimSetPool.GetNumAnimSets();
   pKFM->m_channels.SetSize(nNumAnimSets);
   for (int nAnimSetIndex = 0; nAnimSetIndex < nNumAnimSets; nAnimSetIndex++)
   {
      TupperwareAggregate *pAnimSetAgg = tupAnimSetPool.GetAnimSet(nAnimSetIndex);
      AddChannel(&pKFM->m_channels[nAnimSetIndex],pAnimSetAgg);
   }
   
   pKFM->m_length = pKFM->FindEndTime()-pKFM->FindStartTime();

   TupperwareAggregate *pAnimGlobals = m_pTupScene->GetAnimGlobals();
   TupAnimGlobals tupAnimGlobals(pAnimGlobals);
   int nAnimGlobalFlags;
   tupAnimGlobals.GetFlags(nAnimGlobalFlags);
   m_bUseMayaBoneTranslationSpace = FALSE;
   if (nAnimGlobalFlags & TupAnimGlobals::FLAGS_USE_MAYA_BONE_TRANSLATION_SPACE)
      m_bUseMayaBoneTranslationSpace = TRUE;

   delete m_pTupScene;

   return true;
}

void TupAnim::AddChannel(KFM::Channel *pChannel,TupperwareAggregate *pAnimSetAgg)
{
   TupAnimSet tupAnimSet(pAnimSetAgg);  
   char *name;
   tupAnimSet.GetName(&name);

   TupArray<TupString> parts;
   TokenizeString::BuildArray(name,"|",parts);

   TupString bonename = parts.GetAt(parts.GetSize()-1);

   pChannel->m_name = (const char*)bonename;

   float *pBoneDirection,*pRollDirection,*pParentBoneDirection,*pParentRollDirection;
   
   tupAnimSet.GetBoneDirection(&pBoneDirection);
   tupAnimSet.GetRollDirection(&pRollDirection);
   tupAnimSet.GetParentBoneDirection(&pParentBoneDirection);
   tupAnimSet.GetParentRollDirection(&pParentRollDirection);

   pChannel->m_boneDirection = HashVector3(pBoneDirection[0],pBoneDirection[1],pBoneDirection[2]);
   pChannel->m_rollDirection = HashVector3(pRollDirection[0],pRollDirection[1],pRollDirection[2]);
   pChannel->m_parentBoneDirection = HashVector3(pParentBoneDirection[0],pParentBoneDirection[1],pParentBoneDirection[2]);
   pChannel->m_parentRollDirection = HashVector3(pParentRollDirection[0],pParentRollDirection[1],pParentRollDirection[2]);

   pChannel->ComputeParentToBone();

   TupperwareAggregate *pAnimChannelPoolAgg = tupAnimSet.GetChannels();
   TupAnimChannelPool tupAnimChannelPool(pAnimChannelPoolAgg);

   pChannel->m_channelItems.SetSize(10);

   if (m_bUseMayaBoneTranslationSpace)
   {
      AddTranslationCurvesConvertFromMayaBoneSpace(pChannel,pAnimChannelPoolAgg,pAnimSetAgg);
   }
   else
   {
      AddAnimCurve("translate.translateX",&pChannel->m_channelItems[0],pAnimChannelPoolAgg);
      AddAnimCurve("translate.translateY",&pChannel->m_channelItems[1],pAnimChannelPoolAgg);
      AddAnimCurve("translate.translateZ",&pChannel->m_channelItems[2],pAnimChannelPoolAgg);
   }

//   AddScaleCurves(pChannel,pAnimChannelPoolAgg);//baked
   AddAnimCurve("scale.scaleX",&pChannel->m_channelItems[3],pAnimChannelPoolAgg);
   AddAnimCurve("scale.scaleY",&pChannel->m_channelItems[4],pAnimChannelPoolAgg);
   AddAnimCurve("scale.scaleZ",&pChannel->m_channelItems[5],pAnimChannelPoolAgg);

   int nAnimSetFlags;
   tupAnimSet.GetFlags(nAnimSetFlags);
   if (nAnimSetFlags & TupAnimSet::FLAGS_QUATERNION_ROTATION)
   {
      AddAnimCurve("rotate.rotateX",&pChannel->m_channelItems[6],pAnimChannelPoolAgg);
      AddAnimCurve("rotate.rotateY",&pChannel->m_channelItems[7],pAnimChannelPoolAgg);
      AddAnimCurve("rotate.rotateZ",&pChannel->m_channelItems[8],pAnimChannelPoolAgg);
      AddAnimCurve("rotate.rotateW",&pChannel->m_channelItems[9],pAnimChannelPoolAgg);
   }
   else
   {
    AddRotationCurvesConvertFromEuler(pChannel,pAnimChannelPoolAgg);
   }
}

void TupAnim::AddTranslationCurvesConvertFromMayaBoneSpace(KFM::Channel *pChannel,TupperwareAggregate *pAnimChannelPoolAgg,TupperwareAggregate *pAnimSetAgg)
{
   TupAnimSet tupAnimSet(pAnimSetAgg);

   TupperwareAggregate *pLocalToParentBindMatrix = tupAnimSet.GetLocalToParentBindMatrix();
   TupTransform tupTransform(pLocalToParentBindMatrix);
   assert(tupTransform.GetTransformType() == TupTransform::STATIC_MATRIX);
   TupStaticMatrix tupStaticMatrix(pLocalToParentBindMatrix);
   float *pMatrix;
   tupStaticMatrix.GetMatrix(&pMatrix);
   HashMatrix4x4 localToParentBindMatrix(pMatrix);
   HashVector3 offset = localToParentBindMatrix.GetTranslation();
  
   float *pQuaternion;
   tupAnimSet.GetJointOrientationQuaternion(&pQuaternion);
   HashQuaternion orientquat(pQuaternion[0],pQuaternion[1],pQuaternion[2],pQuaternion[3]);
   HashQuaternion orientquatinverse = orientquat.Inverse();

   TupperwareAggregate *pCurveXAgg = FindCurveForChannelAttribute("translate.translateX",pAnimChannelPoolAgg);
   TupperwareAggregate *pCurveYAgg = FindCurveForChannelAttribute("translate.translateY",pAnimChannelPoolAgg);
   TupperwareAggregate *pCurveZAgg = FindCurveForChannelAttribute("translate.translateZ",pAnimChannelPoolAgg);

   KFM::ChannelItem *pChannelItemX = new KFM::ChannelItem;
   KFM::ChannelItem *pChannelItemY = new KFM::ChannelItem;
   KFM::ChannelItem *pChannelItemZ = new KFM::ChannelItem;

   if (pCurveXAgg)
      AddCurve(pChannelItemX,pCurveXAgg,m_fUnitFactor);
   else
      AddDefaultKey(pChannelItemX,0.0f);

   if (pCurveYAgg)
      AddCurve(pChannelItemY,pCurveYAgg,m_fUnitFactor);
   else
      AddDefaultKey(pChannelItemY,0.0f);

   if (pCurveZAgg)
      AddCurve(pChannelItemZ,pCurveZAgg,m_fUnitFactor);
   else
      AddDefaultKey(pChannelItemZ,0.0f);

   pChannelItemX->CalcTangents();
   pChannelItemY->CalcTangents();
   pChannelItemZ->CalcTangents();

   HashTime endTime = pChannelItemX->GetEndTime();
   HashTime endTime2 = pChannelItemY->GetEndTime();
   HashTime endTime3 = pChannelItemZ->GetEndTime();

   HashTime largestTime = endTime;
   if (endTime2 > largestTime)
      largestTime = endTime2;
   if (endTime3 > largestTime)
      largestTime = endTime3;

   HashTime startTime = pChannelItemX->GetStartTime();
   HashTime startTime2 = pChannelItemY->GetStartTime();
   HashTime startTime3 = pChannelItemZ->GetStartTime();

   HashTime smallestTime = startTime;
   if (startTime2 < smallestTime)
      smallestTime = startTime2;
   if (startTime3 < smallestTime)
      smallestTime = startTime3;

   int nStartKey = (int)smallestTime.GetFrameInt(m_fFPS);
   int nStopKey = (int)largestTime.GetFrameInt(m_fFPS);

   KFM::ChannelItem *pChannelItemX2 = &pChannel->m_channelItems[0];
   KFM::ChannelItem *pChannelItemY2 = &pChannel->m_channelItems[1];
   KFM::ChannelItem *pChannelItemZ2 = &pChannel->m_channelItems[2];   

   int nNumKeys = nStopKey - nStartKey;
   nNumKeys++;

   pChannelItemX2->m_keyframes.SetSize(nNumKeys);
   pChannelItemY2->m_keyframes.SetSize(nNumKeys);
   pChannelItemZ2->m_keyframes.SetSize(nNumKeys);
 
   for (int nKeyIndex = nStartKey; nKeyIndex <= nStopKey; nKeyIndex++)
   {
      HashTime currentTime(nKeyIndex,m_fFPS);
      double x = pChannelItemX->Interpolate(currentTime);
      double y = pChannelItemY->Interpolate(currentTime);
      double z = pChannelItemZ->Interpolate(currentTime); 

      HashVector3 translate(x,y,z);
      translate = translate - offset * m_fUnitFactor;
      translate *= orientquatinverse;

      pChannelItemX2->m_keyframes[nKeyIndex].m_time = currentTime;
      pChannelItemY2->m_keyframes[nKeyIndex].m_time = currentTime;
      pChannelItemZ2->m_keyframes[nKeyIndex].m_time = currentTime;

      pChannelItemX2->m_keyframes[nKeyIndex].m_defaulttangents = 1;
      pChannelItemY2->m_keyframes[nKeyIndex].m_defaulttangents = 1;
      pChannelItemZ2->m_keyframes[nKeyIndex].m_defaulttangents = 1;

      pChannelItemX2->m_keyframes[nKeyIndex].m_value = translate.x;
      pChannelItemY2->m_keyframes[nKeyIndex].m_value = translate.y;
      pChannelItemZ2->m_keyframes[nKeyIndex].m_value = translate.z;
   }

   delete pChannelItemX;
   delete pChannelItemY;
   delete pChannelItemZ;

   pChannelItemX2->CalcTangents();
   pChannelItemY2->CalcTangents();
   pChannelItemZ2->CalcTangents();
}

void TupAnim::AddRotationCurvesConvertFromEuler(KFM::Channel *pChannel,TupperwareAggregate *pAnimChannelPoolAgg)
{
   TupperwareAggregate *pCurveXAgg = FindCurveForChannelAttribute("rotate.rotateX",pAnimChannelPoolAgg);
   TupperwareAggregate *pCurveYAgg = FindCurveForChannelAttribute("rotate.rotateY",pAnimChannelPoolAgg);
   TupperwareAggregate *pCurveZAgg = FindCurveForChannelAttribute("rotate.rotateZ",pAnimChannelPoolAgg);

   KFM::ChannelItem *pChannelItemX = new KFM::ChannelItem;
   KFM::ChannelItem *pChannelItemY = new KFM::ChannelItem;
   KFM::ChannelItem *pChannelItemZ = new KFM::ChannelItem;

   if (pCurveXAgg)
      AddCurve(pChannelItemX,pCurveXAgg);
   else
      AddDefaultKey(pChannelItemX,0.0f);

   if (pCurveYAgg)
      AddCurve(pChannelItemY,pCurveYAgg);
   else
      AddDefaultKey(pChannelItemY,0.0f);

   if (pCurveZAgg)
      AddCurve(pChannelItemZ,pCurveZAgg);
   else
      AddDefaultKey(pChannelItemZ,0.0f);

   pChannelItemX->CalcTangents();
   pChannelItemY->CalcTangents();
   pChannelItemZ->CalcTangents();

   HashTime endTime = pChannelItemX->GetEndTime();
   HashTime endTime2 = pChannelItemY->GetEndTime();
   HashTime endTime3 = pChannelItemZ->GetEndTime();

   HashTime largestTime = endTime;
   if (endTime2 > largestTime)
      largestTime = endTime2;
   if (endTime3 > largestTime)
      largestTime = endTime3;

   HashTime startTime = pChannelItemX->GetStartTime();
   HashTime startTime2 = pChannelItemY->GetStartTime();
   HashTime startTime3 = pChannelItemZ->GetStartTime();

   HashTime smallestTime = startTime;
   if (startTime2 < smallestTime)
      smallestTime = startTime2;
   if (startTime3 < smallestTime)
      smallestTime = startTime3;

   int nStartKey = (int)smallestTime.GetFrameInt(m_fFPS);
   int nStopKey = (int)largestTime.GetFrameInt(m_fFPS);

   KFM::ChannelItem *pChannelItemX2 = &pChannel->m_channelItems[6];
   KFM::ChannelItem *pChannelItemY2 = &pChannel->m_channelItems[7];
   KFM::ChannelItem *pChannelItemZ2 = &pChannel->m_channelItems[8];
   KFM::ChannelItem *pChannelItemW2 = &pChannel->m_channelItems[9];

   int nNumKeys = nStopKey - nStartKey;
   nNumKeys++;

   pChannelItemX2->m_keyframes.SetSize(nNumKeys);
   pChannelItemY2->m_keyframes.SetSize(nNumKeys);
   pChannelItemZ2->m_keyframes.SetSize(nNumKeys);
   pChannelItemW2->m_keyframes.SetSize(nNumKeys);

   for (int nKeyIndex = nStartKey; nKeyIndex <= nStopKey; nKeyIndex++)
   {
      HashTime currentTime(nKeyIndex,m_fFPS);
      double x = pChannelItemX->Interpolate(currentTime);
      double y = pChannelItemY->Interpolate(currentTime);
      double z = pChannelItemZ->Interpolate(currentTime); 

      HashMatrix4x4 matrix;
      matrix = GetXRotateMatrix(x);
      matrix = matrix * GetYRotateMatrix(y);
      matrix = matrix * GetZRotateMatrix(z);

 //     HashVector3 translate,rotate,scale,shear;
 //     matrix.Decompose(translate,scale,shear,rotate);

      HashQuaternion quat(matrix);

      pChannelItemX2->m_keyframes[nKeyIndex].m_time = currentTime;
      pChannelItemY2->m_keyframes[nKeyIndex].m_time = currentTime;
      pChannelItemZ2->m_keyframes[nKeyIndex].m_time = currentTime;
      pChannelItemW2->m_keyframes[nKeyIndex].m_time = currentTime; 

      pChannelItemX2->m_keyframes[nKeyIndex].m_defaulttangents = 1;
      pChannelItemY2->m_keyframes[nKeyIndex].m_defaulttangents = 1;
      pChannelItemZ2->m_keyframes[nKeyIndex].m_defaulttangents = 1;
      pChannelItemW2->m_keyframes[nKeyIndex].m_defaulttangents = 1;

      pChannelItemX2->m_keyframes[nKeyIndex].m_value = quat.GetV().x;
      pChannelItemY2->m_keyframes[nKeyIndex].m_value = quat.GetV().y;
      pChannelItemZ2->m_keyframes[nKeyIndex].m_value = quat.GetV().z;
      pChannelItemW2->m_keyframes[nKeyIndex].m_value = quat.GetW(); 
   }

   delete pChannelItemX;
   delete pChannelItemY;
   delete pChannelItemZ;

   pChannelItemX2->CalcTangents();
   pChannelItemY2->CalcTangents();
   pChannelItemZ2->CalcTangents();
   pChannelItemW2->CalcTangents();
}

void TupAnim::AddScaleCurves(KFM::Channel *pChannel,TupperwareAggregate *pAnimChannelPoolAgg)
{
   TupperwareAggregate *pCurveXAgg = FindCurveForChannelAttribute("scale.scaleX",pAnimChannelPoolAgg);
   TupperwareAggregate *pCurveYAgg = FindCurveForChannelAttribute("scale.scaleY",pAnimChannelPoolAgg);
   TupperwareAggregate *pCurveZAgg = FindCurveForChannelAttribute("scale.scaleZ",pAnimChannelPoolAgg);

   KFM::ChannelItem *pChannelItemX = new KFM::ChannelItem;
   KFM::ChannelItem *pChannelItemY = new KFM::ChannelItem;
   KFM::ChannelItem *pChannelItemZ = new KFM::ChannelItem;

   if (pCurveXAgg)
      AddCurve(pChannelItemX,pCurveXAgg);
   else
      AddDefaultKey(pChannelItemX,1.0f);

   if (pCurveYAgg)
      AddCurve(pChannelItemY,pCurveYAgg);
   else
      AddDefaultKey(pChannelItemY,1.0f);

   if (pCurveZAgg)
      AddCurve(pChannelItemZ,pCurveZAgg);
   else
      AddDefaultKey(pChannelItemZ,1.0f);

   pChannelItemX->CalcTangents();
   pChannelItemY->CalcTangents();
   pChannelItemZ->CalcTangents();

   HashTime endTime = pChannelItemX->GetEndTime();
   HashTime endTime2 = pChannelItemY->GetEndTime();
   HashTime endTime3 = pChannelItemZ->GetEndTime();

   HashTime largestTime = endTime;
   if (endTime2 > largestTime)
      largestTime = endTime2;
   if (endTime3 > largestTime)
      largestTime = endTime3;

   HashTime startTime = pChannelItemX->GetStartTime();
   HashTime startTime2 = pChannelItemY->GetStartTime();
   HashTime startTime3 = pChannelItemZ->GetStartTime();

   HashTime smallestTime = startTime;
   if (startTime2 < smallestTime)
      smallestTime = startTime2;
   if (startTime3 < smallestTime)
      smallestTime = startTime3;

   int nStartKey = (int)smallestTime.GetFrameInt(m_fFPS);
   int nStopKey = (int)largestTime.GetFrameInt(m_fFPS);

   KFM::ChannelItem *pChannelItemX2 = &pChannel->m_channelItems[3];
   KFM::ChannelItem *pChannelItemY2 = &pChannel->m_channelItems[4];
   KFM::ChannelItem *pChannelItemZ2 = &pChannel->m_channelItems[5];   

   int nNumKeys = nStopKey - nStartKey;
   nNumKeys++;

   pChannelItemX2->m_keyframes.SetSize(nNumKeys);
   pChannelItemY2->m_keyframes.SetSize(nNumKeys);
   pChannelItemZ2->m_keyframes.SetSize(nNumKeys);

   for (int nKeyIndex = nStartKey; nKeyIndex <= nStopKey; nKeyIndex++)
   {
      HashTime currentTime(nKeyIndex,m_fFPS);
      double x = pChannelItemX->Interpolate(currentTime);
      double y = pChannelItemY->Interpolate(currentTime);
      double z = pChannelItemZ->Interpolate(currentTime); 

      pChannelItemX2->m_keyframes[nKeyIndex].m_time = currentTime;
      pChannelItemY2->m_keyframes[nKeyIndex].m_time = currentTime;
      pChannelItemZ2->m_keyframes[nKeyIndex].m_time = currentTime;

      pChannelItemX2->m_keyframes[nKeyIndex].m_defaulttangents = 1;
      pChannelItemY2->m_keyframes[nKeyIndex].m_defaulttangents = 1;
      pChannelItemZ2->m_keyframes[nKeyIndex].m_defaulttangents = 1;

      pChannelItemX2->m_keyframes[nKeyIndex].m_value = x;
      pChannelItemY2->m_keyframes[nKeyIndex].m_value = y;
      pChannelItemZ2->m_keyframes[nKeyIndex].m_value = z;
   }

   delete pChannelItemX;
   delete pChannelItemY;
   delete pChannelItemZ;

   pChannelItemX2->CalcTangents();
   pChannelItemY2->CalcTangents();
   pChannelItemZ2->CalcTangents();
}

TupperwareAggregate *TupAnim::FindCurveForChannelAttribute(const char *channelattributename,TupperwareAggregate *pAnimChannelPoolAgg)
{
   TupAnimChannelPool tupAnimChannelPool(pAnimChannelPoolAgg);
   int nNumChannels = tupAnimChannelPool.GetNumAnimChannels();
   for (int nChannelItemIndex = 0; nChannelItemIndex < nNumChannels; nChannelItemIndex++)
   {
      TupperwareAggregate *pAnimChannelAgg = tupAnimChannelPool.GetAnimChannel(nChannelItemIndex);
      TupAnimChannel tupAnimChannel(pAnimChannelAgg);
      char *channelattributename2;
      tupAnimChannel.GetAttribute(&channelattributename2);
      if (stricmp(channelattributename,channelattributename2)==0)
      {
         TupperwareAggregate *pAnimCurvePoolAgg = tupAnimChannel.GetCurves();
         TupAnimCurvePool tupAnimCurvePool(pAnimCurvePoolAgg);
         if (tupAnimCurvePool.GetNumAnimCurves()>0)
            return tupAnimCurvePool.GetAnimCurve(0);
      }
   }
   return NULL;
}

void TupAnim::AddAnimCurve(const char *attributename,KFM::ChannelItem *pChannelItem,TupperwareAggregate *pAnimChannelPoolAgg)
{
   TupperwareAggregate *pAnimCurveAgg = FindCurveForChannelAttribute(attributename,pAnimChannelPoolAgg);

   if (pAnimCurveAgg)
      AddCurve(pChannelItem,pAnimCurveAgg);
   else
      AddDefaultKey(pChannelItem,1.0f);

  pChannelItem->CalcTangents();
}

int TupAnim::GetNumKeys(TupperwareAggregate *pAnimCurveAgg)
{
   TupAnimCurve tupAnimCurve(pAnimCurveAgg);

   float *pOutputValues;
   int nNumOutputValues;
   tupAnimCurve.GetOutputValues(&pOutputValues,nNumOutputValues);// value
   return nNumOutputValues;
}

bool TupAnim::GetKey(UINT nIndex,
                      TupperwareAggregate *pAnimCurveAgg,
                      float &fTime,float &fValue,
                      int *nFlag,
                      float *fInTanAngle,float *fOutTanAngle,
                      float *fInTanWeight,float *fOutTanWeight)
{
   TupAnimCurve tupAnimCurve(pAnimCurveAgg);

   float *pOutputValues;
   int nNumOutputValues;
   tupAnimCurve.GetOutputValues(&pOutputValues,nNumOutputValues);// value
   fValue = pOutputValues[nIndex];

   if (nIndex >= (UINT)nNumOutputValues)
      return false;

   float *pInputValues;
   int nNumInputValues;
   tupAnimCurve.GetInputValues(&pInputValues,nNumInputValues);// time
   fTime = pInputValues[nIndex];

   if (nFlag)
   {
      int *pKeyFlags;
      int nNumKeyFlags;
      tupAnimCurve.GetKeyFlags(&pKeyFlags,nNumKeyFlags);
      *nFlag = pKeyFlags[nIndex];
   }

   if (fInTanAngle)
   {
      float *pInAngles;
      int nNumInAngles;
      tupAnimCurve.GetInTangentAngles(&pInAngles,nNumInAngles);
      *fInTanAngle = pInAngles[nIndex];
   }

   if (fInTanWeight)
   {
      float *pInWeights;
      int nNumInWeights;
      tupAnimCurve.GetInTangentWeights(&pInWeights,nNumInWeights);
   }

   if (fOutTanAngle)
   {
      float *pOutAngles;
      int nNumOutAngles;
      tupAnimCurve.GetOutTangentAngles(&pOutAngles,nNumOutAngles);
      *fOutTanAngle = pOutAngles[nIndex];
   }

   if (fOutTanWeight)
   {
      float *pOutWeights;
      int nNumOutWeights;
      tupAnimCurve.GetOutTangentWeights(&pOutWeights,nNumOutWeights);
      *fOutTanWeight = pOutWeights[nIndex];
   }
   return true;
}

void TupAnim::AddDefaultKey(KFM::ChannelItem *pChannelItem,float nValue)
{
   pChannelItem->m_keyframes.SetSize(1);
   KFM::Keyframe *pKeyFrame = &pChannelItem->m_keyframes[0];
   pKeyFrame->m_defaulttangents = 0;
   pKeyFrame->m_value = nValue;
   pKeyFrame->m_time.SetSeconds(0.0);
   pKeyFrame->m_interpmethod |= KFM::Keyframe::INTERPOLATIONMETHOD::LINEAR;
}

void TupAnim::AddCurve(KFM::ChannelItem *pChannelItem,TupperwareAggregate *pAnimCurveAgg,float fMultiply/*=1.0f*/)
{
   TupAnimCurve tupAnimCurve(pAnimCurveAgg);

   float *pOutputValues;
   int nNumOutputValues;
   tupAnimCurve.GetOutputValues(&pOutputValues,nNumOutputValues);// value

   float *pInputValues;
   int nNumInputValues;
   tupAnimCurve.GetInputValues(&pInputValues,nNumInputValues);// time

   int *pKeyFlags;
   int nNumKeyFlags;
   tupAnimCurve.GetKeyFlags(&pKeyFlags,nNumKeyFlags);

   float *pInXTangents;
   int nNumInXTangents;
   tupAnimCurve.GetInTangentX(&pInXTangents,nNumInXTangents);

   float *pInYTangents;
   int nNumInYTangents;
   tupAnimCurve.GetInTangentY(&pInYTangents,nNumInYTangents);

   float *pOutXTangents;
   int nNumOutXTangents;
   tupAnimCurve.GetOutTangentX(&pOutXTangents,nNumOutXTangents);

   float *pOutYTangents;
   int nNumOutYTangents;
   tupAnimCurve.GetOutTangentY(&pOutYTangents,nNumOutYTangents);

   pChannelItem->m_keyframes.SetSize(nNumOutputValues);

   for (int i = 0; i < nNumOutputValues; i++)
   {
      KFM::Keyframe *pKeyFrame = &pChannelItem->m_keyframes[i];
      pKeyFrame->m_time.SetSeconds(pInputValues[i]);
      pKeyFrame->m_value = pOutputValues[i] * fMultiply;
      pKeyFrame->m_flags = 0;
      pKeyFrame->m_interpmethod = 0;

      if ((pKeyFlags[i] & TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_SMOOTH)
      {
         pKeyFrame->m_interpmethod |= KFM::Keyframe::INTERPOLATIONMETHOD::SPLINE;
         pKeyFrame->m_defaulttangents = 1;
      }
      else if ((pKeyFlags[i] & TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_STEP)
         pKeyFrame->m_interpmethod |= KFM::Keyframe::INTERPOLATIONMETHOD::HOLD;
      else if ((pKeyFlags[i] & TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_LINEAR)
         pKeyFrame->m_interpmethod |= KFM::Keyframe::INTERPOLATIONMETHOD::LINEAR;

      HashVector2 it,ot;

      it.x = pInXTangents[i];
      it.y = pInYTangents[i];

      ot.x = pOutXTangents[i];
      ot.y = pOutYTangents[i];

      if ((pKeyFlags[i] & TupAnimCurve::FLAGS_IN_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_IN_TANGENT_TYPE_FIXED)
         pKeyFrame->m_it = it;
      if ((pKeyFlags[i] & TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_FIXED)
         pKeyFrame->m_ot = ot;
   }
}
