#pragma once

#define TupAnimFile_H

#include "KFM.h"

class TupperwareAggregate;
class TupScene;
class HashMatrix4x4;

class TupAnim
{
private:
   TupperwareAggregate *m_pSceneAgg;
   TupScene *m_pTupScene;
   float m_fFPS;
   float m_fUnitFactor;

public:
   bool LoadAndConvertFile(const char *filename,KFM *pKFM);

private:
   void AddChannel(KFM::Channel *pChannel,TupperwareAggregate *pAnimSetAgg);

   void AddCurve(KFM::ChannelItem *pChannelItem,TupperwareAggregate *pAnimCurveAgg,float fMultiply=1.0f);
   void AddDefaultKey(KFM::ChannelItem *pChannelItem, float nValue);

   void AddTranslationCurves(KFM::Channel *pChannel,TupperwareAggregate *pAnimChannelPoolAgg,TupperwareAggregate *pAnimSetAgg);
   void AddRotationCurves(KFM::Channel *pChannel,TupperwareAggregate *pAnimChannelPoolAgg);
   void AddScaleCurves(KFM::Channel *pChannel,TupperwareAggregate *pAnimChannelPoolAgg);

   TupperwareAggregate *FindCurveForChannelAttribute(const char *channelattributename,TupperwareAggregate *pAnimChannelPoolAgg);
   int GetNumKeys(TupperwareAggregate *pAnimCurveAgg);
   bool GetKey(UINT nIndex,
               TupperwareAggregate *pAnimCurveAgg,
               float &fTime,float &fValue,int *nFlag = NULL,
               float *fInTanAngle = NULL,float *fOutTanAngle = NULL,
               float *fInTanWeight = NULL,float *fOutTanWeight = NULL);
};