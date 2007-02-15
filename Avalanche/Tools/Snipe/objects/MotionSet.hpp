////////////////////////////////////////////////
// MotionCurveDriverBiasObject

extern FloatProperty::Info g_fReductionBiasInfo;

template<class TYPE>
inline MotionCurveDriverBiasObject<TYPE>::MotionCurveDriverBiasObject()
{
   m_pMotionCurveDriver = NULL;
   m_bCurvesNeedRebuilt = TRUE;
   m_fReductionBias.Init( &g_fReductionBiasInfo, this );
   m_fReductionBias.SetSet( FALSE );
}


template<class TYPE>
inline void MotionCurveDriverBiasObject<TYPE>::SetReductionBias( float fBias )
{
   if (m_fReductionBias == fBias)
      return;

   m_fReductionBias.SetValue( fBias );
   m_fReductionBias.OnValueChanged();
}

template<class TYPE>
void MotionCurveDriverBiasObject<TYPE>::SetReductionToleranceParentCache( float fTolerance )
{
   if (fTolerance < 0.0f)
      fTolerance = 0.0f;
   m_fReductionToleranceParentCache = fTolerance;
   OnObjectNotify(NOTIFYOBJ_VALUECHANGED,0,(LPARAM)&m_fReductionBias);
}

template<class TYPE>
inline void MotionCurveDriverBiasObject<TYPE>::SetMotionCurveDriver( MotionCurveDriver *pMotionCurveDriver )
{
   m_pMotionCurveDriver = (TYPE *)pMotionCurveDriver;
   m_bCurvesNeedRebuilt = TRUE;
}

template<class TYPE>
inline void MotionCurveDriverBiasObject<TYPE>::Validate()
{
   if (m_bCurvesNeedRebuilt)
      RebuildCurves();
}

template<class TYPE>
inline void MotionCurveDriverBiasObject<TYPE>::Draw( CGLBaseView *pView )
{
   if (m_pMotionCurveDriver && !pView->m_bDoPick)
   {
      FloatMotionCurve::m_bDrawInBackground = TRUE;
      m_pMotionCurveDriver->Draw( pView );
      FloatMotionCurve::m_bDrawInBackground = FALSE;
   }

   TYPE::Draw( pView );
}

template<class TYPE>
inline BOOL MotionCurveDriverBiasObject<TYPE>::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
   case NOTIFYOBJ_VALUECHANGED:
      {
         Property *pProperty = (Property *)lParam;
         if (pProperty == &m_fReductionBias)
         {
            NotifyNodes( NOTIFYOBJ_ICONCHANGED, 0, (LPARAM)this );
            m_bCurvesNeedRebuilt = TRUE;
            if (m_pProperty)
            {
               TransformableObject *pTransformableObject = (TransformableObject *)m_pProperty.GetValue()->GetObject()->IsKindOf( SOT_TransformableObject );
               if (pTransformableObject)
               {
                  pTransformableObject->SetAnimatedObjectMatricesDirty();
                  pTransformableObject->m_transform.m_bMatrixDirty = TRUE;
               }
            }
            m_fReductionTolerance = m_fReductionToleranceParentCache + m_fReductionBias.GetValue();
         }
         break;
      }
   }

   return TYPE::OnObjectNotify( msg, wParam, lParam );
}

template<class TYPE>
inline void MotionCurveDriverBiasObject<TYPE>::RebuildCurves()
{
   ASSERT( m_bCurvesNeedRebuilt );
   m_bCurvesNeedRebuilt = FALSE;

   UINT nCurveCount = GetNumMotionCurves();
   for (UINT i=0; i<nCurveCount; i++)
   {
      FloatMotionCurve *pDestMotionCurve = (FloatMotionCurve *)GetMotionCurveAt( i );

      if (m_pMotionCurveDriver)
      {
         FloatMotionCurve *pSourceMotionCurve = (FloatMotionCurve *)m_pMotionCurveDriver->GetMotionCurveAt( i );
         pSourceMotionCurve->RebuildAsLinear( m_fReductionTolerance, pDestMotionCurve );
      }
      else
         pDestMotionCurve->RemoveAll();
   }
}

