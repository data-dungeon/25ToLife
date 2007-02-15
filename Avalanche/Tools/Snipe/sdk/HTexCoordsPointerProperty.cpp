//

#include "stdafx.h"
#include "HTexCoordsPointerProperty.h"
#include "Properties/TexCoordsPointerProperty.h"

// Info
HTexCoordsPointerProperty::HInfo *HTexCoordsPointerProperty::HInfo::New( const String &strName, const String &strMatchName )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HTexCoordsPointerProperty::HInfo*)SNEW TexCoordsPointerProperty::Info( strName, strMatchName );
}

// HTexCoordsPointerProperty
HTexCoordsPointerProperty *HTexCoordsPointerProperty::New( HTexCoordsPointerProperty::HInfo *pInfo, HProperty *pParent )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   TexCoordsPointerProperty *pProperty = SNEW TexCoordsPointerProperty;
   pProperty->Init( (TexCoordsPointerProperty::Info *)pInfo, (Property *)pParent );
   return (HTexCoordsPointerProperty *) pProperty;
}

HTexCoordsPointerProperty::HInfo *HTexCoordsPointerProperty::GetPropertyInfo() const
{
   return (HTexCoordsPointerProperty::HInfo *) ((const TexCoordsPointerProperty *)this)->GetPropertyInfo();
}

void HTexCoordsPointerProperty::StoreValue( HTexCoords *pTexCoords, const Time &time, BOOL bStoreUndo /*= TRUE*/)
{
   ((TexCoordsPointerProperty *)this)->StoreValue( (TexCoords *)pTexCoords, time, bStoreUndo );
}

void HTexCoordsPointerProperty::StoreValue( HTexCoords *pTexCoords, BOOL bStoreUndo /*= TRUE*/ )
{
   ((TexCoordsPointerProperty *)this)->StoreValue( (TexCoords *)pTexCoords, GetTime(), bStoreUndo );
}

HTexCoords *HTexCoordsPointerProperty::GetValue() const
{
   return (HTexCoords *)((TexCoordsPointerProperty *)this)->GetValue();
}

HTexCoords *HTexCoordsPointerProperty::GetValue(const Time &time) const
{
   return (HTexCoords *)((TexCoordsPointerProperty *)this)->GetValue(time);
}