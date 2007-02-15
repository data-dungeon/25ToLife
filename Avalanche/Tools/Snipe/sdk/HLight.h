#pragma once

#include "HSceneObject.h"

class SNIPEEXPORT HLight : public HSceneObject
{
public:
   void Bind( UINT nIndex );
   void UnBind();
};