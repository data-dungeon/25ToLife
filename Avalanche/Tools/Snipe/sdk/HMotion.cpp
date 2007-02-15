#include "stdafx.h"
#include "HMotion.h"
#include "Motion.h"
//#include "HMotionChannel.h"

HMotionChannel *HMotion::AddBone( const char *name /*=NULL*/ )
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HMotionChannel*)((Motion*)this)->AddBone(name);
}


UINT HMotion::GetNumMotionChannels()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((Motion*)this)->GetNumMotionChannels();
}

HMotionChannel *HMotion::GetMotionChannelAt(UINT nIndex)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HMotionChannel*)((Motion*)this)->GetMotionChannelAt(nIndex);
}

HMotionInstance *HMotion::CreateInstance()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HMotionInstance *)((Motion *)this)->CreateInstance();
}

HTimeProperty *HMotion::GetLength()
{
   return (HTimeProperty*)((Motion*)this)->GetLength();
}

HTimeProperty *HMotion::GetPlaybackStart()
{
   return (HTimeProperty*)((Motion*)this)->GetPlaybackStart();
}

HTimeProperty *HMotion::GetPlaybackStop()
{
   return (HTimeProperty*)((Motion*)this)->GetPlaybackStop();
}

Time HMotion::GetStartTime()
{
   return ((Motion*)this)->GetStartTime();
}

Time HMotion::GetStopTime()
{
   return ((Motion*)this)->GetStopTime();
}
