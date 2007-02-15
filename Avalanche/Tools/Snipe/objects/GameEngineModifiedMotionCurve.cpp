#include "stdafx.h"
#include "GameEngineModifiedMotionCurve.h"
#include "Motion.h"
#include "Properties\MotionCurveDriver.h"

void GameEngineModifiedMotionCurve::Rebuild()
{
   MotionCurveDriver *pMotionCurveDriver = m_pMotionCurve->GetMotionCurveDriver();
   Motion *pMotion = (Motion*)pMotionCurveDriver->GetParent()->GetParent()->GetParent();

   float fMaxVal = 1.0f;
   float fPercent = pMotion->m_fReductionTolerance.GetValue();

#if 0
   if ((fPercent > 0.0f) && ( fPercent < 1.0f))
      fPercent = powf(fPercent,2)/powf(1.0f,2);
#else
   if ((fPercent > 0.0f) && ( fPercent < 1.0f))
      fPercent = powf(fPercent,3)/powf(1.0f,3);
#endif

   float fErrorTolerance = fMaxVal * fPercent;
   fErrorTolerance += fMaxVal/* * pMotionCurveDriver->m_fReductionBias*/;

   if (fErrorTolerance <= 0.0f)
      fErrorTolerance = 0.0f;

   m_pMotionCurve->RebuildAsLinear(fErrorTolerance,this);
}


/////////////////////////////////////////////////////////////////////////////////////////
// GemsModifiedMotionCurve

void GemsModifiedMotionCurve::Rebuild()
{
   MotionCurveDriver *pMotionCurveDriver = m_pMotionCurve->GetMotionCurveDriver();
   Motion *pMotion = (Motion*)pMotionCurveDriver->GetParent()->GetParent()->GetParent();
   m_pMotionCurve->RebuildAsCubic(pMotion->m_fReductionTolerance,this);
}
