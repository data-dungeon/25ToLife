#include "stdafx.h"
#include "KFM.h"

#include "HMotionCurveDriver.h"
#include "HMotion.h"
#include "HMotionChannel.h"
#include "HMotionCurveDrivers.h"
#include "HMotionKey.h"
#include "HMotionCurve.h"
#include "..\Utility\Matrix44.h"
#include "ImportKFMOptionsDlg.h"


#define EOF_ERROR()				                \
if (feof(fp))				                      \
{										                \
   fclose(fp);						                \
   THROW_FATAL_ERROR("Error reading KFM.\n")	 \
}

#define THROW_ERROR()                         \
{                                             \
   fclose(fp);                                \
   THROW_FATAL_ERROR("Error reading KFM.\n")  \
}                                             \

static BOOL ReadVector(FILE *fp,Vector &v)
{
   fread(&v.x,sizeof(float),1,fp);
   if (feof(fp))
      return FALSE;
   fread(&v.y,sizeof(float),1,fp);
   if (feof(fp))
      return FALSE;
   fread(&v.z,sizeof(float),1,fp);
   if (feof(fp))
      return FALSE;
   return TRUE;
}

static BOOL ReadVector2(FILE *fp,Vector2 &v)
{
   fread(&v.x,sizeof(float),1,fp);
   if (feof(fp))
      return FALSE;
   fread(&v.y,sizeof(float),1,fp);
   if (feof(fp))
      return FALSE;
   return TRUE;
}

static BOOL ReadString(FILE *fp,char *str,int len)
{
   char c;
   int cnt = 0;
   do 
   {
      c = fgetc(fp);
      str[cnt++] = c;
      if (cnt == len)
         break;
      if (feof(fp))
         return FALSE;
   } while (c!='\0');
   return TRUE;
}

void KFMTranslator::OnImport( HMotion *pMotion, const char *szFileName)
{
//   ImportKFMOptionsDlg dlg;
//   if (dlg.DoModal() != IDOK)
//      return;

   HMainInterface *pMI = GetHMainInterface();

   union 
   {
      unsigned char flags;
      struct 
      {
         unsigned char  defaulttangents : 1;
         unsigned char  interpmethod    : 2;
      };
   } ;


   int version;
   char line[256];
   FILE *fp;

   fp = fopen(szFileName,"rb");

   if (fp == NULL)
      THROW_FATAL_ERROR("Couldn't open KFM file.\n");

   fread(&version,sizeof(int),1,fp);
   EOF_ERROR();

   if (version != KFMVERSION)
       THROW_FATAL_ERROR("Incorrect KFM version.\n");

   float seconds;
   fread(&seconds,sizeof(float),1,fp);// duration
   EOF_ERROR();

   unsigned short nChannelCount;
   fread(&nChannelCount,sizeof(unsigned short),1,fp);
   EOF_ERROR();

   for (int nChannelIndex = 0; nChannelIndex < nChannelCount; nChannelIndex++)
   {
      if (!ReadString(fp,line,256))
         THROW_ERROR()

      HMotionChannel *pMotionChannel = pMotion->AddBone(line);

      Vector bonedirection,rolldirection,parentbonedirection,parentrolldirection;

      if (!ReadVector(fp,bonedirection))
         THROW_ERROR();

      if (!ReadVector(fp,rolldirection))
         THROW_ERROR();

      if (!ReadVector(fp,parentbonedirection))
         THROW_ERROR();

      if (!ReadVector(fp,parentrolldirection))
         THROW_ERROR();

      pMotionChannel->ComputeParentToBone(bonedirection,rolldirection,parentbonedirection,parentrolldirection);

      HVectorMotionCurveDriver *pTranslateMotionCurveDriver = (HVectorMotionCurveDriver *)pMI->CreateObject( SOT_VectorMotionCurveDriver );
      ASSERT( pTranslateMotionCurveDriver );
      pTranslateMotionCurveDriver->SetName("Transform.Translate");

      HVectorMotionCurveDriver *pScaleMotionCurveDriver = (HVectorMotionCurveDriver *)pMI->CreateObject( SOT_VectorMotionCurveDriver );
      ASSERT( pScaleMotionCurveDriver );
      pScaleMotionCurveDriver->SetName("Transform.Scale");

      HQuaternionMotionCurveDriver *pRotateMotionCurveDriver = (HQuaternionMotionCurveDriver *)pMI->CreateObject( SOT_QuaternionMotionCurveDriver );
      ASSERT(pRotateMotionCurveDriver);
      pRotateMotionCurveDriver->SetName("Transform.Rotate");

      pMotionChannel->AddMotionCurveDriver(pTranslateMotionCurveDriver);
      pMotionChannel->AddMotionCurveDriver(pScaleMotionCurveDriver);
      pMotionChannel->AddMotionCurveDriver(pRotateMotionCurveDriver);
      CArray<HFloatMotionCurve*> motioncurves;
      motioncurves.SetSize(10);
      motioncurves[0] = (HFloatMotionCurve*)pTranslateMotionCurveDriver->GetMotionCurveAt(0);
      motioncurves[1] = (HFloatMotionCurve*)pTranslateMotionCurveDriver->GetMotionCurveAt(1);
      motioncurves[2] = (HFloatMotionCurve*)pTranslateMotionCurveDriver->GetMotionCurveAt(2);
      motioncurves[3] = (HFloatMotionCurve*)pScaleMotionCurveDriver->GetMotionCurveAt(0);
      motioncurves[4] = (HFloatMotionCurve*)pScaleMotionCurveDriver->GetMotionCurveAt(1);
      motioncurves[5] = (HFloatMotionCurve*)pScaleMotionCurveDriver->GetMotionCurveAt(2);
      motioncurves[6] = (HFloatMotionCurve*)pRotateMotionCurveDriver->GetMotionCurveAt(0);
      motioncurves[7] = (HFloatMotionCurve*)pRotateMotionCurveDriver->GetMotionCurveAt(1);
      motioncurves[8] = (HFloatMotionCurve*)pRotateMotionCurveDriver->GetMotionCurveAt(2);
      motioncurves[9] = (HFloatMotionCurve*)pRotateMotionCurveDriver->GetMotionCurveAt(3);

      for (int nChannelItemIndex = 0; nChannelItemIndex < 10; nChannelItemIndex++)
      {
         unsigned short nNumKeyFrames;
         fread(&nNumKeyFrames,sizeof(unsigned short),1,fp);
         EOF_ERROR();

         float value;

         Vector2 it,ot;

         for (int nKeyframeIndex = 0; nKeyframeIndex < nNumKeyFrames; nKeyframeIndex++)
         {
            fread(&seconds,sizeof(float),1,fp);
            EOF_ERROR();

            fread(&value,sizeof(float),1,fp);
            EOF_ERROR();

            HFloatMotionCurve *pFloatMotionCurve = motioncurves[nChannelItemIndex];
 
            HFloatMotionKey *pMotionKey = pFloatMotionCurve->StoreValue(value,seconds);

            fread(&flags,sizeof(unsigned char),1,fp);
            EOF_ERROR();


            switch (interpmethod)
            {
            case KFMTranslator::SPLINE :
               {
                  pMotionKey->SetInterpolationMethod(HFloatMotionKey::SPLINE);
               }
               break;
            case KFMTranslator::LINEAR :
               {
                  pMotionKey->SetInterpolationMethod(HFloatMotionKey::LINEAR);
               }
               break;
            case KFMTranslator::HOLD :
               {
                  pMotionKey->SetInterpolationMethod(HFloatMotionKey::HOLD);
               }
               break;
            }
  
            if (!defaulttangents)
            {
               if (!ReadVector2(fp,it))
                  THROW_ERROR();

               if (!ReadVector2(fp,ot))
                  THROW_ERROR();

               pMotionKey->SetInTangent(it);
               pMotionKey->SetOutTangent(ot);
            }
         }
      }
   }

   fclose(fp);
}
