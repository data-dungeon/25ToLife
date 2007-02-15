#include "stdafx.h"
#include "GLMotionReductionViewControl.h"
#include "MotionSet.h"
#include "GLViewControl.h"
#include "Bone.h"

CGLMotionChannelObjectBiasViewControl::CGLMotionChannelObjectBiasViewControl( CGLBaseView *pBaseView, MotionChannelBiasObject *pMotionChannelBiasObject ) :
   CGLViewControl( pBaseView ),
   m_TranslateSlider( pBaseView, pMotionChannelBiasObject->GetTranslationBiasProperty() ),
   m_RotateSlider( pBaseView, pMotionChannelBiasObject->GetRotationBiasProperty() ),
   m_ScaleSlider( pBaseView, pMotionChannelBiasObject->GetScaleBiasProperty() )
{
   m_pMotionChannelBiasObject = pMotionChannelBiasObject;

   if (pMotionChannelBiasObject->m_pOriginalTransformableObject->IsBone())
   {
      Bone *pBone = (Bone*)pMotionChannelBiasObject->m_pOriginalTransformableObject;
      Vector vStart = pBone->GetStart()->GetValue();
      m_TranslateSlider.SetOrigin(Vector(vStart.x,vStart.y-2.0f,vStart.z));
      m_RotateSlider.SetOrigin(vStart);
      m_ScaleSlider.SetOrigin(Vector(vStart.x,vStart.y+2.0f,vStart.z));
   }
}
