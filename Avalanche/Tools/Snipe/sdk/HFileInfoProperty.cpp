//

#include "stdafx.h"
#include "HFileInfoProperty.h"
#include "Properties/Property.h"

HFileNameProperty *HFileInfoProperty::GetFileNameProperty()
{
   return (HFileNameProperty*)((FileInfoProperty*)this)->GetFileNameProperty();
}