#include "stdafx.h"
#include "HLight.h"
#include "Light.h"

void HLight::Bind( UINT nIndex )
{
   ((Light *)this)->Bind( nIndex );
}

void HLight::UnBind()
{
   ((Light *)this)->UnBind();
}

