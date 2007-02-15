//

#include "stdafx.h"
#include "HTransform2Property.h"
#include "HVector2Property.h"
#include "HFloatProperty.h"
#include "Properties/Property.h"

HTransform2Property *HTransform2Property::New( HTransform2Property::HInfo *pInfo, HProperty *pParent )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   Transform2Property *pProperty = Transform2Property::Create();
   pProperty->Init( (Transform2Property::Info *)pInfo, (Property *)pParent );
   return (HTransform2Property *) pProperty;
}

HVector2Property *HTransform2Property::GetTranslate()
{
   return (HVector2Property *) &((Transform2Property *)this)->m_v2Translate;
}

HVector2Property *HTransform2Property::GetScale()
{
   return (HVector2Property *) &((Transform2Property *)this)->m_v2Scale;
}

HFloatProperty *HTransform2Property::GetRotate()
{
   return (HFloatProperty *) &((Transform2Property *)this)->m_fRotate;
}