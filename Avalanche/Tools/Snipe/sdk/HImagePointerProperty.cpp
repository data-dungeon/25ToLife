//

#include "stdafx.h"
#include "HImagePointerProperty.h"
#include "Properties/ImagePointerProperty.h"

// Info
HImagePointerProperty::HInfo *HImagePointerProperty::HInfo::New( const String &strName, const String &strMatchName )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HImagePointerProperty::HInfo*)SNEW ImagePointerProperty::Info( strName, strMatchName );
}

// HImagePointerProperty
HImagePointerProperty *HImagePointerProperty::New( HImagePointerProperty::HInfo *pInfo, HProperty *pParent )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ImagePointerProperty *pProperty = SNEW ImagePointerProperty;
   pProperty->Init( (ImagePointerProperty::Info *)pInfo, (Property *)pParent );
   return (HImagePointerProperty *) pProperty;
}

HImagePointerProperty::HInfo *HImagePointerProperty::GetPropertyInfo() const
{
   return (HImagePointerProperty::HInfo *) ((const ImagePointerProperty *)this)->GetPropertyInfo();
}

void HImagePointerProperty::StoreValue( HImage *pImage, const Time &time, BOOL bStoreUndo /*= TRUE*/)
{
   ((ImagePointerProperty *)this)->StoreValue( (Image *)pImage, time, bStoreUndo );
}

void HImagePointerProperty::StoreValue( HImage *pImage, BOOL bStoreUndo /*= TRUE*/ )
{
   ((ImagePointerProperty *)this)->StoreValue( (Image *)pImage, GetTime(), bStoreUndo );
}

HImage *HImagePointerProperty::GetValue() const
{
   return (HImage *)((ImagePointerProperty *)this)->GetValue();
}

HImage *HImagePointerProperty::GetValue(const Time &time) const
{
   return (HImage *)((ImagePointerProperty *)this)->GetValue(time);
}

HTransform2Property *HImagePointerProperty::GetTransform()
{
   return (HTransform2Property *)&((ImagePointerProperty *)this)->m_transform;
}
