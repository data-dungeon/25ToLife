#include "stdafx.h"
#include "Tup.h"

#include "HMotionCurveDriver.h"
#include "HMotion.h"
#include "HMotionChannel.h"
#include "..\Utility\Matrix44.h"
#include "HMotionCurveDrivers.h"
#include "HMotionKey.h"
#include "HMotionCurve.h"
#include "HTimeProperty.h"

void TupMotionTranslator::OnImport( HMotion *pMotion, const char *szFileName)
{
   // read tup file
   int result = TupperwareLoadSaveManager::Read(szFileName,&m_pSceneAgg);
   if (!result)
      THROW_FATAL_ERROR("TupperwareLoadSaveManager::Read failed");

   // exit if scene empty
   if (!m_pSceneAgg)
      THROW_FATAL_ERROR("SceneAgg empty");

   m_pTupScene = new TupScene(m_pSceneAgg);

   TupperwareAggregate *pAnimGlobals = m_pTupScene->GetAnimGlobals();
   TupAnimGlobals tupAnimGlobals(pAnimGlobals);
   int nAnimGlobalFlags;
   tupAnimGlobals.GetFlags(nAnimGlobalFlags);
   m_bUseMayaBoneTranslationSpace = FALSE;
   if (nAnimGlobalFlags & TupAnimGlobals::FLAGS_USE_MAYA_BONE_TRANSLATION_SPACE)
      m_bUseMayaBoneTranslationSpace = TRUE;

   float seconds;
   tupAnimGlobals.GetStartTime(seconds);
   pMotion->GetPlaybackStart()->StoreValue(Time(seconds));
   tupAnimGlobals.GetEndTime(seconds);
   pMotion->GetPlaybackStop()->StoreValue(Time(seconds));

   TupperwareAggregate *pAnimSetPoolAgg = m_pTupScene->GetAnimSetPool();
   assert(pAnimSetPoolAgg);
   TupAnimSetPool tupAnimSetPool(pAnimSetPoolAgg);

   ImportAnimation(pMotion,pAnimSetPoolAgg);

   // delete pools
   delete m_pSceneAgg;
   delete m_pTupScene;

}

void TupMotionTranslator::ImportAnimation(HMotion *pMotion,TupperwareAggregate *pAnimSetPoolAgg)
{
   HMainInterface *pMI = GetHMainInterface();

   TupAnimSetPool tupAnimSetPool(pAnimSetPoolAgg);

   int nNumAnimSets = tupAnimSetPool.GetNumAnimSets();

   for (int nAnimSetIndex = 0; nAnimSetIndex < nNumAnimSets; nAnimSetIndex++)
   {
      TupperwareAggregate *pAnimSetAgg = tupAnimSetPool.GetAnimSet(nAnimSetIndex);
      TupAnimSet tupAnimSet(pAnimSetAgg);  

      char *name;
      tupAnimSet.GetName(&name);

      String bonename = name;
      StringArray parts;
      bonename.Split('|',parts);

      const char *debug = parts[parts.GetSize()-1].Get();

      HMotionChannel *pMotionChannel = pMotion->AddBone(parts[parts.GetSize()-1].Get());

      float *pBoneDirection,*pRollDirection,*pParentBoneDirection,*pParentRollDirection;

      tupAnimSet.GetBoneDirection(&pBoneDirection);
      tupAnimSet.GetRollDirection(&pRollDirection);
      tupAnimSet.GetParentBoneDirection(&pParentBoneDirection);
      tupAnimSet.GetParentRollDirection(&pParentRollDirection);
      
      pMotionChannel->ComputeParentToBone(Vector(pBoneDirection[0],pBoneDirection[1],pBoneDirection[2]),
                                          Vector(pRollDirection[0],pRollDirection[1],pRollDirection[3]),
                                          Vector(pParentBoneDirection[0],pParentBoneDirection[1],pParentBoneDirection[2]),
                                          Vector(pParentBoneDirection[0],pParentBoneDirection[1],pParentBoneDirection[2]));


      TupperwareAggregate *pAnimChannelPoolAgg = tupAnimSet.GetChannels();
      TupAnimChannelPool tupAnimChannelPool(pAnimChannelPoolAgg);
      BOOL bHasAnimation;

      // TRANSLATE
      if (m_bUseMayaBoneTranslationSpace)
         AddTranslationCurvesConvertFromMayaBoneSpace(pMotionChannel,pAnimChannelPoolAgg,pAnimSetAgg);
      else
      {
         bHasAnimation = FALSE;
         if (IsChannelAnimated("translate.translateX",pAnimChannelPoolAgg))
            bHasAnimation = TRUE;
         if (IsChannelAnimated("translate.translateY",pAnimChannelPoolAgg))
            bHasAnimation = TRUE;
         if (IsChannelAnimated("translate.translateZ",pAnimChannelPoolAgg))
            bHasAnimation = TRUE;
         if (bHasAnimation)
         {
            HVectorMotionCurveDriver *pVectorMotionCurveDriver = (HVectorMotionCurveDriver *)pMI->CreateObject(SOT_VectorMotionCurveDriver);
            ASSERT(pVectorMotionCurveDriver);

            pVectorMotionCurveDriver->SetName("Transform.Translate");
            pMotionChannel->AddMotionCurveDriver(pVectorMotionCurveDriver);
            AddAnimCurve("translate.translateX",(HFloatMotionCurve*)pVectorMotionCurveDriver->GetMotionCurveAt(0),pAnimChannelPoolAgg,100.0f);
            AddAnimCurve("translate.translateY",(HFloatMotionCurve*)pVectorMotionCurveDriver->GetMotionCurveAt(1),pAnimChannelPoolAgg,100.0f);
            AddAnimCurve("translate.translateZ",(HFloatMotionCurve*)pVectorMotionCurveDriver->GetMotionCurveAt(2),pAnimChannelPoolAgg,100.0f);
         }
       }

      // SCALE
      bHasAnimation = FALSE;
      if (IsChannelAnimated("scale.scaleX",pAnimChannelPoolAgg))
         bHasAnimation = TRUE;
      if (IsChannelAnimated("scale.scaleY",pAnimChannelPoolAgg))
         bHasAnimation = TRUE;
      if (IsChannelAnimated("scale.scaleZ",pAnimChannelPoolAgg))
         bHasAnimation = TRUE;
      if (bHasAnimation)
      {
         HVectorMotionCurveDriver *pVectorMotionCurveDriver = (HVectorMotionCurveDriver *)pMI->CreateObject(SOT_VectorMotionCurveDriver);
         ASSERT(pVectorMotionCurveDriver);

         pVectorMotionCurveDriver->SetName("Transform.Scale");
         pMotionChannel->AddMotionCurveDriver(pVectorMotionCurveDriver);
         AddAnimCurve("scale.scaleX",(HFloatMotionCurve*)pVectorMotionCurveDriver->GetMotionCurveAt(0),pAnimChannelPoolAgg);
         AddAnimCurve("scale.scaleY",(HFloatMotionCurve*)pVectorMotionCurveDriver->GetMotionCurveAt(1),pAnimChannelPoolAgg);
         AddAnimCurve("scale.scaleZ",(HFloatMotionCurve*)pVectorMotionCurveDriver->GetMotionCurveAt(2),pAnimChannelPoolAgg);
      }

      // ROTATE
      bHasAnimation = FALSE;
      if (IsChannelAnimated("rotate.rotateX",pAnimChannelPoolAgg))
         bHasAnimation = TRUE;
      if (IsChannelAnimated("rotate.rotateY",pAnimChannelPoolAgg))
         bHasAnimation = TRUE;
      if (IsChannelAnimated("rotate.rotateZ",pAnimChannelPoolAgg))
         bHasAnimation = TRUE;
      if (bHasAnimation)
      {
         int nAnimSetFlags;
         tupAnimSet.GetFlags(nAnimSetFlags);
         if (nAnimSetFlags & TupAnimSet::FLAGS_QUATERNION_ROTATION)
         {
            HQuaternionMotionCurveDriver *pQuaternionMotionCurveDriver = (HQuaternionMotionCurveDriver *)pMI->CreateObject(SOT_QuaternionMotionCurveDriver);
            ASSERT(pQuaternionMotionCurveDriver);

            pQuaternionMotionCurveDriver->SetName("Transform.Rotate");
            pMotionChannel->AddMotionCurveDriver(pQuaternionMotionCurveDriver);
            AddAnimCurve("rotate.rotateX",(HFloatMotionCurve*)pQuaternionMotionCurveDriver->GetMotionCurveAt(0),pAnimChannelPoolAgg);
            AddAnimCurve("rotate.rotateY",(HFloatMotionCurve*)pQuaternionMotionCurveDriver->GetMotionCurveAt(1),pAnimChannelPoolAgg);
            AddAnimCurve("rotate.rotateZ",(HFloatMotionCurve*)pQuaternionMotionCurveDriver->GetMotionCurveAt(2),pAnimChannelPoolAgg);
            AddAnimCurve("rotate.rotateW",(HFloatMotionCurve*)pQuaternionMotionCurveDriver->GetMotionCurveAt(3),pAnimChannelPoolAgg);
         }
         else
         {
            if (m_bUseMayaBoneTranslationSpace)
            {
               AddRotationCurvesConvertFromXYZ(pMotionChannel,pAnimChannelPoolAgg,pAnimSetAgg);
            }
            else
            {
               HEulerMotionCurveDriver *pEulerMotionCurveDriver = (HEulerMotionCurveDriver *)pMI->CreateObject(SOT_EulerMotionCurveDriver);
               ASSERT( pEulerMotionCurveDriver );

               pEulerMotionCurveDriver->SetName("Transform.Rotate");
               pMotionChannel->AddMotionCurveDriver(pEulerMotionCurveDriver);
               AddAnimCurve("rotate.rotateX",(HFloatMotionCurve*)pEulerMotionCurveDriver->GetMotionCurveAt(0),pAnimChannelPoolAgg,(float)RADTODEG);
               AddAnimCurve("rotate.rotateY",(HFloatMotionCurve*)pEulerMotionCurveDriver->GetMotionCurveAt(1),pAnimChannelPoolAgg,(float)RADTODEG);
               AddAnimCurve("rotate.rotateZ",(HFloatMotionCurve*)pEulerMotionCurveDriver->GetMotionCurveAt(2),pAnimChannelPoolAgg,(float)RADTODEG);
            }
         }
      }
   }
}

void TupMotionTranslator::AddTranslationCurvesConvertFromMayaBoneSpace(HMotionChannel *pMotionChannel,TupperwareAggregate *pAnimChannelPoolAgg,TupperwareAggregate *pAnimSetAgg)
{
   HMainInterface *pMI = GetHMainInterface();

   TupAnimSet tupAnimSet(pAnimSetAgg);

   TupperwareAggregate *pLocalToParentBindMatrix = tupAnimSet.GetLocalToParentBindMatrix();
   TupTransform tupTransform(pLocalToParentBindMatrix);
   assert(tupTransform.GetTransformType() == TupTransform::STATIC_MATRIX);
   TupStaticMatrix tupStaticMatrix(pLocalToParentBindMatrix);
   float *pMatrix;
   tupStaticMatrix.GetMatrix(&pMatrix);
   Matrix44 localToParentBindMatrix(pMatrix);
   Vector offset;
   localToParentBindMatrix.GetTranslation(offset);

   float *pQuaternion;
   tupAnimSet.GetJointOrientationQuaternion(&pQuaternion);
   Quaternion orientquat(pQuaternion[0],pQuaternion[1],pQuaternion[2],pQuaternion[3]);
   Quaternion orientquatinverse = orientquat.Inverse();

   TupperwareAggregate *pCurveXAgg = FindCurveForChannelAttribute("translate.translateX",pAnimChannelPoolAgg);
   TupperwareAggregate *pCurveYAgg = FindCurveForChannelAttribute("translate.translateY",pAnimChannelPoolAgg);
   TupperwareAggregate *pCurveZAgg = FindCurveForChannelAttribute("translate.translateZ",pAnimChannelPoolAgg);

   if (!((pCurveXAgg)&&(pCurveYAgg)&&(pCurveZAgg)))
      return;

   HVectorMotionCurveDriver *pVectorCurveDriver = (HVectorMotionCurveDriver *)pMI->CreateObject( SOT_VectorMotionCurveDriver );
   ASSERT(pVectorCurveDriver);

   HFloatMotionCurve *pXCurve = (HFloatMotionCurve*)pVectorCurveDriver->GetMotionCurveAt(0);
   HFloatMotionCurve *pYCurve = (HFloatMotionCurve*)pVectorCurveDriver->GetMotionCurveAt(1);
   HFloatMotionCurve *pZCurve = (HFloatMotionCurve*)pVectorCurveDriver->GetMotionCurveAt(2);

   if (pCurveXAgg)
      FillOutFloatMotionCurveDriver(pXCurve,pCurveXAgg);

   if (pCurveYAgg)
      FillOutFloatMotionCurveDriver(pYCurve,pCurveYAgg);

   if (pCurveZAgg)
      FillOutFloatMotionCurveDriver(pZCurve,pCurveZAgg);

   // bake keys to resolve interpolation
   Time startTime = pXCurve->GetStartTime();
   Time startTime2 = pYCurve->GetStartTime();
   Time startTime3 = pZCurve->GetStartTime();

   Time smallestTime = startTime;
   if (startTime2 < smallestTime)
      smallestTime = startTime2;
   if (startTime3 < smallestTime)
      smallestTime = startTime3;

   Time endTime = pXCurve->GetStopTime();
   Time endTime2 = pYCurve->GetStopTime();
   Time endTime3 = pZCurve->GetStopTime();

   Time largestTime = endTime;
   if (endTime2 > largestTime)
      largestTime = endTime2;
   if (endTime3 > largestTime)
      largestTime = endTime3;

   int nStartKey = (int)smallestTime.GetFrameInt(30.0f);// sample at 30 fps
   int nStopKey = (int)largestTime.GetFrameInt(30.0f);// sample at 30 fps

   HVectorMotionCurveDriver *pVectorMotionCurveDriver = (HVectorMotionCurveDriver *)pMI->CreateObject( SOT_VectorMotionCurveDriver );
   ASSERT( pVectorMotionCurveDriver );

   pVectorMotionCurveDriver->SetName("Transform.Translate");
   pMotionChannel->AddMotionCurveDriver(pVectorMotionCurveDriver);

   int nNumKeys = nStopKey - nStartKey;
   nNumKeys++;

   for (int nKeyIndex = nStartKey; nKeyIndex <= nStopKey; nKeyIndex++)
   {
      Time currentTime( nKeyIndex, 30.0f );// sample at 30 fps
      Vector translate = pVectorCurveDriver->GetValue( currentTime );
      translate = translate - offset;
      translate *= orientquatinverse;
      pVectorMotionCurveDriver->StoreValue( translate, currentTime );
   }

   delete pVectorCurveDriver;
}

void ColumnMajorMatrixMultiply(Matrix44 a,Matrix44 b,Matrix44 &c)
{
   c.SetAt(0,0,(a.GetAt(0,0) * b.GetAt(0,0)) + (a.GetAt(1,0) * b.GetAt(0,1)) + (a.GetAt(2,0) * b.GetAt(0,2)) + (a.GetAt(3,0) * b.GetAt(0,3)));
   c.SetAt(1,0,(a.GetAt(0,0) * b.GetAt(1,0)) + (a.GetAt(1,0) * b.GetAt(1,1)) + (a.GetAt(2,0) * b.GetAt(1,2)) + (a.GetAt(3,0) * b.GetAt(1,3)));
   c.SetAt(2,0,(a.GetAt(0,0) * b.GetAt(2,0)) + (a.GetAt(1,0) * b.GetAt(2,1)) + (a.GetAt(2,0) * b.GetAt(2,2)) + (a.GetAt(3,0) * b.GetAt(2,3)));
   c.SetAt(3,0,(a.GetAt(0,0) * b.GetAt(3,0)) + (a.GetAt(1,0) * b.GetAt(3,1)) + (a.GetAt(2,0) * b.GetAt(3,2)) + (a.GetAt(3,0) * b.GetAt(3,3)));

   c.SetAt(0,1,(a.GetAt(0,1) * b.GetAt(0,0)) + (a.GetAt(1,1) * b.GetAt(0,1)) + (a.GetAt(2,1) * b.GetAt(0,2)) + (a.GetAt(3,1) * b.GetAt(0,3)));
   c.SetAt(1,1,(a.GetAt(0,1) * b.GetAt(1,0)) + (a.GetAt(1,1) * b.GetAt(1,1)) + (a.GetAt(2,1) * b.GetAt(1,2)) + (a.GetAt(3,1) * b.GetAt(1,3)));
   c.SetAt(2,1,(a.GetAt(0,1) * b.GetAt(2,0)) + (a.GetAt(1,1) * b.GetAt(2,1)) + (a.GetAt(2,1) * b.GetAt(2,2)) + (a.GetAt(3,1) * b.GetAt(2,3)));
   c.SetAt(3,1,(a.GetAt(0,1) * b.GetAt(3,0)) + (a.GetAt(1,1) * b.GetAt(3,1)) + (a.GetAt(2,1) * b.GetAt(3,2)) + (a.GetAt(3,1) * b.GetAt(3,3)));

   c.SetAt(0,2,(a.GetAt(0,2) * b.GetAt(0,0)) + (a.GetAt(1,2) * b.GetAt(0,1)) + (a.GetAt(2,2) * b.GetAt(0,2)) + (a.GetAt(3,2) * b.GetAt(0,3)));
   c.SetAt(1,2,(a.GetAt(0,2) * b.GetAt(1,0)) + (a.GetAt(1,2) * b.GetAt(1,1)) + (a.GetAt(2,2) * b.GetAt(1,2)) + (a.GetAt(3,2) * b.GetAt(1,3)));
   c.SetAt(2,2,(a.GetAt(0,2) * b.GetAt(2,0)) + (a.GetAt(1,2) * b.GetAt(2,1)) + (a.GetAt(2,2) * b.GetAt(2,2)) + (a.GetAt(3,2) * b.GetAt(2,3)));
   c.SetAt(3,2,(a.GetAt(0,2) * b.GetAt(3,0)) + (a.GetAt(1,2) * b.GetAt(3,1)) + (a.GetAt(2,2) * b.GetAt(3,2)) + (a.GetAt(3,2) * b.GetAt(3,3)));

   c.SetAt(0,3,(a.GetAt(0,3) * b.GetAt(0,0)) + (a.GetAt(1,3) * b.GetAt(0,1)) + (a.GetAt(2,3) * b.GetAt(0,2)) + (a.GetAt(3,3) * b.GetAt(0,3)));
   c.SetAt(1,3,(a.GetAt(0,3) * b.GetAt(1,0)) + (a.GetAt(1,3) * b.GetAt(1,1)) + (a.GetAt(2,3) * b.GetAt(1,2)) + (a.GetAt(3,3) * b.GetAt(1,3)));
   c.SetAt(2,3,(a.GetAt(0,3) * b.GetAt(2,0)) + (a.GetAt(1,3) * b.GetAt(2,1)) + (a.GetAt(2,3) * b.GetAt(2,2)) + (a.GetAt(3,3) * b.GetAt(2,3)));
   c.SetAt(3,3,(a.GetAt(0,3) * b.GetAt(3,0)) + (a.GetAt(1,3) * b.GetAt(3,1)) + (a.GetAt(2,3) * b.GetAt(3,2)) + (a.GetAt(3,3) * b.GetAt(3,3)));
}

Quaternion BuildQuaternion(Matrix44 matrix)
{
   float tr,s;
   int i;
   Vector v;
   float w;

   tr = matrix.GetAt(0,0) + matrix.GetAt(1,1) + matrix.GetAt(2,2);
   if (tr > 0.0f)
   {
      s = sqrt(tr + 1.0f);
      w = s * 0.5f;
      s = 0.5f / s;

      v.Set((matrix.GetAt(1,2) - matrix.GetAt(2,1)) * s,
         (matrix.GetAt(2,0) - matrix.GetAt(0,2)) * s,
         (matrix.GetAt(0,1) - matrix.GetAt(1,0)) * s);
   }
   else
   {
      // find largest value on diagonal

      i = 0;
      if (matrix.GetAt(1,1) > matrix.GetAt(0,0))
         i = 1;
      if ((matrix.GetAt(2,2) > matrix.GetAt(0,0)) && (matrix.GetAt(2,2) > matrix.GetAt(1,1)))
         i = 2;

      switch(i)
      {
      case 0:
         s = sqrt(matrix.GetAt(0,0) - matrix.GetAt(1,1) - matrix.GetAt(2,2) + 1.0f);
         v.x = ( s * 0.5f);
         assert(s != 0.0f);
         s = 0.5f / s;
         v.y = ( (matrix.GetAt(0,1) + matrix.GetAt(1,0)) * s);
         v.z = ( (matrix.GetAt(0,2) + matrix.GetAt(2,0)) * s);
         w   = (matrix.GetAt(1,2) - matrix.GetAt(2,1)) * s;
         break;
      case 1:
         s = sqrt(matrix.GetAt(1,1) - matrix.GetAt(2,2) - matrix.GetAt(0,0) + 1.0f);
         v.y = ( s * 0.5f);
         assert(s != 0.0f);
         s = 0.5f / s;
         v.x = ( (matrix.GetAt(1,0) + matrix.GetAt(0,1)) * s);
         v.z = ( (matrix.GetAt(1,2) + matrix.GetAt(2,1)) * s);
         w   = (matrix.GetAt(2,0) - matrix.GetAt(0,2)) * s;
         break;
      case 2:
         s = sqrt(matrix.GetAt(2,2) - matrix.GetAt(0,0) - matrix.GetAt(1,1) + 1.0f);
         v.z = ( s * 0.5f);
         assert(s != 0.0f);
         s = 0.5f / s;
         v.x = ( (matrix.GetAt(2,0) + matrix.GetAt(0,2)) * s);
         v.y = ( (matrix.GetAt(2,1) + matrix.GetAt(1,2)) * s);
         w   = (matrix.GetAt(0,1) - matrix.GetAt(1,0)) * s;
         break;
      default:
         assert(FALSE);
      }
 
   }
   return Quaternion(v,w);
}

/*
class MotionKeys
{
public:
   MotionKeys()
   {
      m_pXFloatMotionKey = NULL;
      m_pYFloatMotionKey = NULL;
      m_pZFloatMotionKey = NULL;
   }
   MotionKeys(HFloatMotionKey *pXFloatMotionKey,HFloatMotionKey *pYFloatMotionKey,HFloatMotionKey *pZFloatMotionKey)
   {
      m_pXFloatMotionKey = pXFloatMotionKey;
      m_pYFloatMotionKey = pYFloatMotionKey;
      m_pZFloatMotionKey = pZFloatMotionKey;
   }
   HFloatMotionKey *m_pXFloatMotionKey;
   HFloatMotionKey *m_pYFloatMotionKey;
   HFloatMotionKey *m_pZFloatMotionKey;
};


void TupMotionTranslator::AddRotationCurvesConvertFromXYZ(HMotionChannel *pMotionChannel,TupperwareAggregate *pAnimChannelPoolAgg,TupperwareAggregate *pAnimSetAgg)
{
   TupAnimSet tupAnimSet(pAnimSetAgg);

   TupperwareAggregate *pCurveXAgg = FindCurveForChannelAttribute("rotate.rotateX",pAnimChannelPoolAgg);
   TupperwareAggregate *pCurveYAgg = FindCurveForChannelAttribute("rotate.rotateY",pAnimChannelPoolAgg);
   TupperwareAggregate *pCurveZAgg = FindCurveForChannelAttribute("rotate.rotateZ",pAnimChannelPoolAgg);

   if (!((pCurveXAgg)&&(pCurveYAgg)&&(pCurveZAgg)))
      return;

   HEulerMotionCurveDriver *pEulerMotionCurveDriver = HEulerMotionCurveDriver::New();
   HFloatMotionCurve *pXCurve = (HFloatMotionCurve*)pEulerMotionCurveDriver->GetMotionCurveAt(0);
   HFloatMotionCurve *pYCurve = (HFloatMotionCurve*)pEulerMotionCurveDriver->GetMotionCurveAt(1);
   HFloatMotionCurve *pZCurve = (HFloatMotionCurve*)pEulerMotionCurveDriver->GetMotionCurveAt(2);

   if (pCurveXAgg)
      FillOutFloatMotionCurveDriver(pXCurve,pCurveXAgg);

   if (pCurveYAgg)
      FillOutFloatMotionCurveDriver(pYCurve,pCurveYAgg);

   if (pCurveZAgg)
      FillOutFloatMotionCurveDriver(pZCurve,pCurveZAgg);

   HQuaternionMotionCurveDriver *pQuatMotionCurveDriver = HQuaternionMotionCurveDriver::New();
   pQuatMotionCurveDriver->SetName("Transform.Rotate");
   pMotionChannel->AddMotionCurveDriver(pQuatMotionCurveDriver);

   // resolve independant Euler
   CArray<MotionKeys> motionkeys;

   CMap<Time,Time,UINT,UINT> keymap;
   Time currentTime;
   UINT nIndex;
   for (HFloatMotionKey *pXMotionKey = (HFloatMotionKey*)pXCurve->GetHeadKey(); pXMotionKey; pXMotionKey = (HFloatMotionKey*)pXMotionKey->GetNextKey())
   {
      currentTime = pXMotionKey->GetTime();
      float fValue = pXMotionKey->GetValue();
      keymap.SetAt(currentTime,motionkeys.Add(MotionKeys(pXMotionKey,NULL,NULL)));
   }
   for (HFloatMotionKey *pYMotionKey = (HFloatMotionKey*)pYCurve->GetHeadKey(); pYMotionKey; pYMotionKey = (HFloatMotionKey*)pYMotionKey->GetNextKey())
   {
      currentTime = pYMotionKey->GetTime();
      float fValue = pYMotionKey->GetValue();
      if (keymap.Lookup(currentTime,nIndex))
      {
         MotionKeys *pMotionKeys = &motionkeys.GetAt(nIndex);
         pMotionKeys->m_pYFloatMotionKey = pYMotionKey;
      }
      else
      {
         keymap.SetAt(currentTime,motionkeys.Add(MotionKeys(NULL,pYMotionKey,NULL)));
      }
   }
   for (HFloatMotionKey *pZMotionKey = (HFloatMotionKey*)pZCurve->GetHeadKey(); pZMotionKey; pZMotionKey = (HFloatMotionKey*)pZMotionKey->GetNextKey())
   {
      currentTime = pZMotionKey->GetTime();
      float fValue = pZMotionKey->GetValue();
      if (keymap.Lookup(currentTime,nIndex))
      {
         MotionKeys *pMotionKeys = &motionkeys.GetAt(nIndex);
         pMotionKeys->m_pZFloatMotionKey = pZMotionKey;
      }
      else
      {
         keymap.SetAt(currentTime,motionkeys.Add(MotionKeys(NULL,NULL,pZMotionKey)));
      }
   }

   for (POSITION pos = keymap.GetStartPosition(); pos;)
   {
      keymap.GetNextAssoc(pos,currentTime,nIndex);
      MotionKeys *pMotionKeys = &motionkeys.GetAt(nIndex);

      Vector rotate;

      if (pMotionKeys->m_pXFloatMotionKey)
         rotate.x = pMotionKeys->m_pXFloatMotionKey->GetValue();
      else
         rotate.x = pXCurve->GetValue(currentTime);

      if (pMotionKeys->m_pYFloatMotionKey)
         rotate.y = pMotionKeys->m_pYFloatMotionKey->GetValue();
      else
         rotate.y = pYCurve->GetValue(currentTime);

      if (pMotionKeys->m_pZFloatMotionKey)
         rotate.z = pMotionKeys->m_pZFloatMotionKey->GetValue();
      else
         rotate.z = pZCurve->GetValue(currentTime);

      double sine = sin(rotate.x),cosine = cos(rotate.x);
      double xrotmatcol[16] = { 1, 0, 0, 0, 0, cosine, sine, 0, 0, -sine, cosine ,0 , 0, 0, 0, 1 };//column major
      Matrix44 xmatrix(xrotmatcol);

      sine = sin(rotate.y); cosine = cos(rotate.y);
      double yrotmatcol[16] = { cosine, 0, -sine, 0, 0, 1, 0, 0, sine ,0, cosine, 0 , 0, 0, 0, 1 };//column major
      Matrix44 ymatrix(yrotmatcol);

      sine = sin(rotate.z); cosine = cos(rotate.z);
      double zrotmatcol[16] = { cosine, sine, 0, 0, -sine, cosine, 0, 0, 0, 0, 1, 0 , 0, 0, 0, 1 };//column major
      Matrix44 zmatrix(zrotmatcol);

      Matrix44 matrix = xmatrix;
      Matrix44 c;
      ColumnMajorMatrixMultiply(ymatrix,matrix,c);
      ColumnMajorMatrixMultiply(zmatrix,c,matrix);

      Quaternion quat = BuildQuaternion(matrix);
      pQuatMotionCurveDriver->StoreValue(quat,currentTime);
    }
 
   delete pEulerMotionCurveDriver;
}
*/

void TupMotionTranslator::AddRotationCurvesConvertFromXYZ(HMotionChannel *pMotionChannel,TupperwareAggregate *pAnimChannelPoolAgg,TupperwareAggregate *pAnimSetAgg)
{
   TupAnimSet tupAnimSet(pAnimSetAgg);

   TupperwareAggregate *pCurveXAgg = FindCurveForChannelAttribute("rotate.rotateX",pAnimChannelPoolAgg);
   TupperwareAggregate *pCurveYAgg = FindCurveForChannelAttribute("rotate.rotateY",pAnimChannelPoolAgg);
   TupperwareAggregate *pCurveZAgg = FindCurveForChannelAttribute("rotate.rotateZ",pAnimChannelPoolAgg);

   if (!((pCurveXAgg)&&(pCurveYAgg)&&(pCurveZAgg)))
      return;

   HEulerMotionCurveDriver *pEulerMotionCurveDriver = (HEulerMotionCurveDriver *)GetHMainInterface()->CreateObject( SOT_EulerMotionCurveDriver );
   ASSERT(pEulerMotionCurveDriver);

   HFloatMotionCurve *pXCurve = (HFloatMotionCurve*)pEulerMotionCurveDriver->GetMotionCurveAt(0);
   HFloatMotionCurve *pYCurve = (HFloatMotionCurve*)pEulerMotionCurveDriver->GetMotionCurveAt(1);
   HFloatMotionCurve *pZCurve = (HFloatMotionCurve*)pEulerMotionCurveDriver->GetMotionCurveAt(2);

   if (pCurveXAgg)
      FillOutFloatMotionCurveDriver(pXCurve,pCurveXAgg);

   if (pCurveYAgg)
      FillOutFloatMotionCurveDriver(pYCurve,pCurveYAgg);

   if (pCurveZAgg)
      FillOutFloatMotionCurveDriver(pZCurve,pCurveZAgg);

   // bake keys to resolve interpolation
   Time startTime = pXCurve->GetStartTime();
   Time startTime2 = pYCurve->GetStartTime();
   Time startTime3 = pZCurve->GetStartTime();

   Time smallestTime = startTime;
   if (startTime2 < smallestTime)
      smallestTime = startTime2;
   if (startTime3 < smallestTime)
      smallestTime = startTime3;

   Time endTime = pXCurve->GetStopTime();
   Time endTime2 = pYCurve->GetStopTime();
   Time endTime3 = pZCurve->GetStopTime();

   Time largestTime = endTime;
   if (endTime2 > largestTime)
      largestTime = endTime2;
   if (endTime3 > largestTime)
      largestTime = endTime3;

   int nStartKey = (int)smallestTime.GetFrameInt(30.0f);// sample at 30 fps
   int nStopKey = (int)largestTime.GetFrameInt(30.0f);// sample at 30 fps

   HQuaternionMotionCurveDriver *pQuatMotionCurveDriver = (HQuaternionMotionCurveDriver *)GetHMainInterface()->CreateObject(SOT_QuaternionMotionCurveDriver);
   ASSERT( pQuatMotionCurveDriver );

   pQuatMotionCurveDriver->SetName("Transform.Rotate");
   pMotionChannel->AddMotionCurveDriver(pQuatMotionCurveDriver);

   int nNumKeys = nStopKey - nStartKey;
   nNumKeys++;

   for (int nKeyIndex = nStartKey; nKeyIndex <= nStopKey; nKeyIndex++)
   {
      Time currentTime( nKeyIndex, 30.0f );// sample at 30 fps
      Vector rotate = pEulerMotionCurveDriver->GetValue( currentTime );

      double sine = sin(rotate.x),cosine = cos(rotate.x);
      double xrotmatcol[16] = { 1, 0, 0, 0, 0, cosine, sine, 0, 0, -sine, cosine ,0 , 0, 0, 0, 1 };//column major
      Matrix44 xmatrix(xrotmatcol);

      sine = sin(rotate.y); cosine = cos(rotate.y);
      double yrotmatcol[16] = { cosine, 0, -sine, 0, 0, 1, 0, 0, sine ,0, cosine, 0 , 0, 0, 0, 1 };//column major
      Matrix44 ymatrix(yrotmatcol);

      sine = sin(rotate.z); cosine = cos(rotate.z);
      double zrotmatcol[16] = { cosine, sine, 0, 0, -sine, cosine, 0, 0, 0, 0, 1, 0 , 0, 0, 0, 1 };//column major
      Matrix44 zmatrix(zrotmatcol);

      Matrix44 matrix = xmatrix;
      Matrix44 c;
      ColumnMajorMatrixMultiply(ymatrix,matrix,c);
      ColumnMajorMatrixMultiply(zmatrix,c,matrix);

      Quaternion quat = BuildQuaternion(matrix);
      pQuatMotionCurveDriver->StoreValue(quat,currentTime);
   }

   delete pEulerMotionCurveDriver;
}

BOOL TupMotionTranslator::IsChannelAnimated(const char *attributename,TupperwareAggregate *pAnimChannelPoolAgg)
{
   TupperwareAggregate *pAnimCurveAgg = FindCurveForChannelAttribute(attributename,pAnimChannelPoolAgg);
   if (pAnimCurveAgg)
   {
      TupAnimCurve tupAnimCurve(pAnimCurveAgg);
      float *pInputValues;
      int nNumInputValues;
      tupAnimCurve.GetInputValues(&pInputValues,nNumInputValues);// time
      if (nNumInputValues>0)
         return TRUE;
   }
   return FALSE;
}

void TupMotionTranslator::AddAnimCurve(const char *attributename,HFloatMotionCurve *pFloatMotionCurve,TupperwareAggregate *pAnimChannelPoolAgg,float fMultiply/*=1.0f*/)
{
   TupperwareAggregate *pAnimCurveAgg = FindCurveForChannelAttribute(attributename,pAnimChannelPoolAgg);
   if (pAnimCurveAgg)
      FillOutFloatMotionCurveDriver(pFloatMotionCurve,pAnimCurveAgg,fMultiply);
}

TupperwareAggregate *TupMotionTranslator::FindCurveForChannelAttribute(const char *channelattributename,TupperwareAggregate *pAnimChannelPoolAgg)
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

void TupMotionTranslator::FillOutFloatMotionCurveDriver(HFloatMotionCurve *pFloatMotionCurve,TupperwareAggregate *pAnimCurveAgg,float fMultiply/*=1.0f*/)
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

   for (int i = 0; i < nNumOutputValues; i++)
   {
      HFloatMotionKey *pFloatMotionKey = pFloatMotionCurve->StoreValue(pOutputValues[i] * fMultiply,Time(pInputValues[i]));

      Vector2 it,ot;

      it.x = pInXTangents[i];
      it.y = pInYTangents[i];

      ot.x = pOutXTangents[i];
      ot.y = pOutYTangents[i];

      if ((pKeyFlags[i] & TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_IN_TANGENT_TYPE_FIXED)
         pFloatMotionKey->SetInTangent(it);
      if ((pKeyFlags[i] & TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_FIXED)
         pFloatMotionKey->SetOutTangent(ot);

      HFloatMotionKey::InterpolationMethod nInterpolationMethod;

      if ((pKeyFlags[i] & TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_STEP)
         nInterpolationMethod = HFloatMotionKey::HOLD;
      else if ((pKeyFlags[i] & TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_LINEAR)
         nInterpolationMethod = HFloatMotionKey::LINEAR;
      else
          nInterpolationMethod = HFloatMotionKey::SPLINE;

      pFloatMotionKey->SetInterpolationMethod(nInterpolationMethod);
   }
}

