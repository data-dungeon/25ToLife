//

#include "stdafx.h"
#include "HScaleProperty.h"
#include "Properties/Property.h"

// ScaleProperty
HScaleProperty *HScaleProperty::New( HVectorProperty::HInfo *pInfo, HProperty *pParent )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ScaleProperty *pProperty = ScaleProperty::Create();
   pProperty->Init( (VectorProperty::Info *)pInfo, (Property *)pParent );
   return (HScaleProperty *) pProperty;
}