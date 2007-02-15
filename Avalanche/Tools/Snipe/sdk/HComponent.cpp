#include "stdafx.h"
#include "HComponent.h"
#include "Component.h"

UINT HComponent::GetID()
{
   return ((Component*)this)->GetID();
}

const char *HComponent::GetCommandLineName() const
{
   return ((Component *)this)->GetCommandLineName();
}
