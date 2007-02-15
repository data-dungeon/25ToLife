//

#include "stdafx.h"
#include "HRotateProperty.h"
#include "Properties/Property.h"

// RotateProperty
HRotateProperty *HRotateProperty::New( HVectorProperty::HInfo *pInfo, HProperty *pParent )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   RotateProperty *pProperty = RotateProperty::Create();
   pProperty->Init( (VectorProperty::Info *)pInfo, (Property *)pParent );
   return (HRotateProperty *) pProperty;
}