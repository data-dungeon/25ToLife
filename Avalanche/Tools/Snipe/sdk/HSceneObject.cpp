#include "stdafx.h"
#include "HSceneObject.h"
#include "SceneObject.h"
#include "Motion.h"

void HSceneObject::AddMotionInstance( HMotionInstance *pMotionInstance )
{
   ((SceneObject *)this)->AddMotionInstance( (MotionInstance *)pMotionInstance );
}
