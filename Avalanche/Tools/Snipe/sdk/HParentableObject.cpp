
#include "stdafx.h"
#include "HParentableObject.h"
#include "ParentableObject.h"

void HParentableObject::SetParent( HSnipeObject *pParent )
{
   ((ParentableObject *)this)->SetParent( (SnipeObject *)pParent);
}
