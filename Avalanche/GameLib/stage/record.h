/*
**
**  File:   record.cpp
**  Date:   June 26, 2000
**  By:     Bryant Collard
**  Desc:   Run recorded protagonists.
**
**  Copyright (c) 2000, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#ifndef RECORD_H
#define RECORD_H

#include "platform/BaseType.h"

class CCast;

class CRecord
{
  public:
    CRecord(CCast &i_masterCast);
    void Initialize(void);
    void Run(void);

  private:
    CCast &d_masterCast;
    bool d_info;
    bool d_record;
    bool d_recording;
    bool d_play;
    bool d_playing;
    bool d_dir;
    bool d_singleStep;
    float d_speed;
    bool d_fwd;
    bool d_loop;
    float d_currentTime;
    float d_recordStartTime;
    float d_recordEndTime;
    float d_playStartTime;
    float d_playEndTime;
    bool d_setPlayStartTime;
    bool d_setPlayEndTime;
};

#endif // RECORD_H
