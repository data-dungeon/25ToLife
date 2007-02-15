
#include "stdafx.h"
#include "HPlaceholderObject.h"
#include "PlaceholderObject.h"

const String &HPlaceholderObject::GetFileText() const
{
   return ((PlaceholderObject *)this)->GetFileText();
}
