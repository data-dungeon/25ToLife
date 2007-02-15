//

#include "stdafx.h"
#include "HTransformProperty.h"
#include "HTranslateProperty.h"
#include "HScaleProperty.h"
#include "HRotateProperty.h"
#include "Properties/Property.h"

// TransformProperty
HTransformProperty *HTransformProperty::New( HTransformProperty::HInfo *pInfo, HProperty *pParent )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   TransformProperty *pProperty = TransformProperty::Create();
   pProperty->Init( (TransformProperty::Info *)pInfo, (Property *)pParent );
   return (HTransformProperty *) pProperty;
}

HTranslateProperty *HTransformProperty::GetTranslate()
{
   return (HTranslateProperty *) &((TransformProperty *)this)->m_vTranslate;
}

HScaleProperty *HTransformProperty::GetScale()
{
   return (HScaleProperty *) &((TransformProperty *)this)->m_vScale;
}

HRotateProperty *HTransformProperty::GetRotate()
{
   return (HRotateProperty *) &((TransformProperty *)this)->m_vRotate;
}