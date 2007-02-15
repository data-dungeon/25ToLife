#pragma once

#include "HScene.h"

class HTimeProperty;

class SNIPEEXPORT HMotionInstance : public HHierObject
{
public:
};

class HMotionChannel;

class SNIPEEXPORT HMotion : public HScene
{
public:
   HMotionChannel *AddBone( const char *name = NULL );
   UINT GetNumMotionChannels();
   HMotionChannel *GetMotionChannelAt(UINT nIndex);
   HMotionInstance *CreateInstance();

   HTimeProperty *GetLength();
   HTimeProperty *GetPlaybackStart();
   HTimeProperty *GetPlaybackStop();

   Time GetStartTime();
   Time GetStopTime();
};
