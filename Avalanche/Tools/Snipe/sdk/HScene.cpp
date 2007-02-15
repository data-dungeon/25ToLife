#include "stdafx.h"
#include "HScene.h"
#include "Scene.h"

HFileInfoProperty *HScene::GetFileInfoProperty()
{
   return (HFileInfoProperty*)((Scene*)this)->GetFileInfoProperty();
}

HHierObject *HScene::AddInstance(HHierObject *pHierObject)
{
   return (HHierObject*)((Scene *)this)->AddInstance((HierObject*)pHierObject);
}

