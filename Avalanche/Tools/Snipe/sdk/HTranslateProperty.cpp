//

#include "stdafx.h"
#include "HTranslateProperty.h"
#include "Properties/Property.h"

// TranslateProperty
HTranslateProperty *HTranslateProperty::New( HVectorProperty::HInfo *pInfo, HProperty *pParent )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   TranslateProperty *pProperty = TranslateProperty::Create();
   pProperty->Init( (VectorProperty::Info *)pInfo, (Property *)pParent );
   return (HTranslateProperty *) pProperty;
}