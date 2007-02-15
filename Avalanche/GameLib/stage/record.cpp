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

/******************* includes ****************************/

#include "stage/StagePCH.h"



#include "EngineHelper/timer.h"
#include "cmdcon/cmdcon.h"



#ifdef ENABLE_RECORD

CRecord::CRecord(CCast &i_masterCast) :
        d_masterCast(i_masterCast)
{
    d_info = false;
    d_record = false;
    d_recording = false;
    d_play = false;
    d_playing = false;
    d_dir = false;
    d_singleStep = false;
    d_speed = 1.0f;
    d_fwd = true;
    d_loop = false;
    d_currentTime = 0.0f;
    d_playStartTime = -1.0f;
    d_playEndTime = -1.0f;
    d_setPlayStartTime = false;
    d_setPlayEndTime = false;
}

void CRecord::Initialize(void)
{
    g_console.CreateToggle("record", "Info", d_info);
    g_console.CreateToggle("record", "Record", d_record);
    g_console.CreateToggle("record", "Play", d_play);
    g_console.CreateToggle("record", "Dir", d_dir);
    g_console.CreateToggle("record", "S", d_singleStep);
    g_console.CreateVar("record", "Time", d_currentTime);
    g_console.CreateVar("record", "Start", d_playStartTime);
    g_console.CreateVar("record", "Finish", d_playEndTime);
    g_console.CreateToggle("record", "Mark", d_setPlayStartTime);
    g_console.CreateToggle("record", "Stop", d_setPlayEndTime);
    g_console.CreateVar("record", "Speed", d_speed);
    g_console.CreateToggle("record", "Loop", d_loop);
}

void CRecord::Run(void)
{
    CCastMember* curr;

    if (d_speed < 0.0f)
        d_speed = -d_speed;

    if (d_dir)
    {
        d_fwd = !d_fwd;
        g_console.Echo ("Setting playback direction to %s",
                d_fwd ? "forward" : "reverse");
        d_dir = false;
    }

    if (d_play)
    {
        if (d_playing)
        {
            if (d_recording)
            {
                g_console.Echo("Stopping playback and restarting recording");
                curr = NULL;
                while(curr = d_masterCast.GetNextMember(curr))
                    curr->GetActor()->SetRecord();
            }
            else
            {
                g_console.Echo("Stopping playback");
                curr = NULL;
                while(curr = d_masterCast.GetNextMember(curr))
                    curr->GetActor()->SetIdle();
            }
            d_playing = false;
        }
        else
        {
            d_recordStartTime = -1.0f;
            d_recordEndTime = -1.0f;
            curr = NULL;
            while(curr = d_masterCast.GetNextMember(curr))
            {
                float actorStart = curr->GetActor()->GetRecordingStart();
                if ((actorStart >= 0.0f) && ((d_recordStartTime < 0.0f)
                        || (actorStart < d_recordStartTime)))
                    d_recordStartTime = actorStart;

                float actorEnd = curr->GetActor()->GetRecordingEnd();
                if ((actorEnd >= 0.0f) && ((d_recordEndTime < 0.0f)
                        || (actorEnd > d_recordEndTime)))
                    d_recordEndTime = actorEnd;
            }

            if ((d_recordStartTime >= 0.0f) &&
                    (d_recordEndTime > d_recordStartTime))
            {
                if (d_recording)
                    g_console.Echo("Stopping recording for playback");

                g_console.Echo ("Recorded range is %f to %f", d_recordStartTime,
                        d_recordEndTime);

                if (d_setPlayStartTime)
                    d_playStartTime = d_currentTime;

                if (d_setPlayEndTime)
                    d_playEndTime = d_currentTime;

                if ((d_playStartTime < d_recordStartTime) ||
                        (d_playStartTime >= d_recordEndTime))
                    d_playStartTime = d_recordStartTime;
                if ((d_playEndTime <= d_playStartTime) ||
                        (d_playEndTime > d_recordEndTime))
                    d_playEndTime = d_recordEndTime;

                if ((d_currentTime < d_playStartTime) ||
                        (d_currentTime > d_playEndTime))
                {
                    if (d_fwd)
                        d_currentTime = d_playStartTime;
                    else
                        d_currentTime = d_playEndTime;
                }

                g_console.Echo("Starting playback at %f in range %f to %f "
                        "(%s at speed %f)", d_currentTime, d_playStartTime,
                        d_playEndTime, (d_fwd ? "fwd" : "rev"), d_speed);

                if (!d_singleStep)
                {
                    if (d_fwd)
                        d_currentTime -= g_timer.GetFrameSec() * d_speed;
                    else
                        d_currentTime += g_timer.GetFrameSec() * d_speed;
                }

                d_playing = true;
            }
            else
                g_console.Echo ("No valid recordings to play back");
        }
        d_play = false;
    }
    else if (d_playing)
    {
        if (d_setPlayStartTime)
        {
            d_playStartTime = d_currentTime;
            g_console.Echo("Setting playback start time to %f",
                    d_playStartTime);
        }

        if (d_setPlayEndTime)
        {
            d_playEndTime = d_currentTime;
            g_console.Echo("Setting playback end time to %f",
                    d_playEndTime);
        }

        if ((d_playStartTime < d_recordStartTime) ||
                (d_playStartTime >= d_recordEndTime))
        {
            d_playStartTime = d_recordStartTime;
            g_console.Echo("Clamping playback start time to %f",
                    d_playStartTime);
        }

        if ((d_playEndTime <= d_playStartTime) ||
                (d_playEndTime > d_recordEndTime))
        {
            d_playEndTime = d_recordEndTime;
            g_console.Echo("Clamping playback end time to %f", d_playEndTime);
        }
    }
    d_setPlayStartTime = false;
    d_setPlayEndTime = false;

    if (d_record)
    {
        if (d_recording)
        {
            if (d_playing)
                g_console.Echo("Disabling recording");
            else
            {
                g_console.Echo("Stopping recording");
                curr = NULL;
                while(curr = d_masterCast.GetNextMember(curr))
                    curr->GetActor()->SetIdle();
            }
            d_recording = false;
        }
        else
        {
            if (d_playing)
                g_console.Echo("Enabling recording");
            else
            {
                g_console.Echo("Starting recording");
                curr = NULL;
                while(curr = d_masterCast.GetNextMember(curr))
                    curr->GetActor()->SetRecord();
            }
            d_recording = true;
        }
        d_record = false;
    }

    if (d_playing)
    {
        if (d_singleStep)
        {
            if (d_fwd)
            {
                float minStepSize = d_recordEndTime - d_currentTime + 1.0f;
                curr = NULL;
                while(curr = d_masterCast.GetNextMember(curr))
                {
                    float stepSize = curr->GetActor()->GetForwardStepSize(
                            d_currentTime);
                    if ((stepSize > 0.0f) && (stepSize < minStepSize))
                        minStepSize = stepSize;
                }
                d_currentTime += minStepSize;
                if (d_currentTime > d_playEndTime)
                {
                    if (d_loop)
                        d_currentTime = d_playStartTime;
                    else
                        d_currentTime = d_playEndTime;
                }
            }
            else
            {
                float minStepSize = d_currentTime - d_recordStartTime + 1.0f;
                curr = NULL;
                while(curr = d_masterCast.GetNextMember(curr))
                {
                    float stepSize = curr->GetActor()->GetReverseStepSize(
                            d_currentTime);
                    if ((stepSize > 0.0f) && (stepSize < minStepSize))
                        minStepSize = stepSize;
                }
                d_currentTime -= minStepSize;
                if (d_currentTime < d_playStartTime)
                {
                    if (d_loop)
                        d_currentTime = d_playEndTime;
                    else
                        d_currentTime = d_playStartTime;
                }
            }
            g_console.Echo ("Step %s to %f", (d_fwd ? "forward" : "backward"),
                    d_currentTime);
        }
        else
        {
            if (d_fwd)
                d_currentTime += g_timer.GetFrameSec() * d_speed;
            else
                d_currentTime -= g_timer.GetFrameSec() * d_speed;

            if (d_currentTime > d_playEndTime)
            {
                if (d_loop)
                    d_currentTime = d_playStartTime;
                else
                    d_currentTime = d_playEndTime;
            }

            if (d_currentTime < d_playStartTime)
            {
                if (d_loop)
                    d_currentTime = d_playEndTime;
                else
                    d_currentTime = d_playStartTime;
            }
        }
        curr = NULL;
        while(curr = d_masterCast.GetNextMember(curr))
            curr->GetActor()->SetPlayback(d_currentTime);
    }

    if (d_singleStep)
    {
        d_speed = 0.0f;
        d_singleStep = false;
    }

    if (d_info)
    {
        bool validRecording;
        float playStartTime = d_playStartTime;
        float playEndTime = d_playEndTime;
        float currentTime = d_currentTime;
        if (!d_playing)
        {
            d_recordStartTime = -1.0f;
            d_recordEndTime = -1.0f;
            CCastMember* curr = NULL;
            while(curr = d_masterCast.GetNextMember(curr))
            {
                float actorStart = curr->GetActor()->GetRecordingStart();
                if ((actorStart >= 0.0f) && ((d_recordStartTime < 0.0f)
                        || (actorStart < d_recordStartTime)))
                    d_recordStartTime = actorStart;

                float actorEnd = curr->GetActor()->GetRecordingEnd();
                if ((actorEnd >= 0.0f) && ((d_recordEndTime < 0.0f)
                        || (actorEnd > d_recordEndTime)))
                    d_recordEndTime = actorEnd;
            }

            if ((d_recordStartTime >= 0.0f) &&
                    (d_recordEndTime > d_recordStartTime))
            {
                validRecording = true;
                if ((playStartTime < d_recordStartTime) ||
                        (playStartTime >= d_recordEndTime))
                    playStartTime = d_recordStartTime;
                if ((playEndTime <= playStartTime) ||
                        (playEndTime > d_recordEndTime))
                    playEndTime = d_recordEndTime;

                if ((currentTime < playStartTime) ||
                        (currentTime > playEndTime))
                {
                    if (d_fwd)
                        currentTime = playStartTime;
                    else
                        currentTime = playEndTime;
                }
            }
            else
                validRecording = false;

            g_console.Echo("Recording currently %s:",
                    d_recording ? "recording" : "idle");
        }
        else
        {
            validRecording = true;
            g_console.Echo("Recording currently playing (recording %s):",
                    d_recording ? "suspended" : "deactivated");
        }

        if (validRecording)
        {
            g_console.Echo("    Recorded range is %f to %f",
                    d_recordStartTime, d_recordEndTime);
            g_console.Echo("    Playback range is %f to %f",
                    playStartTime, playEndTime);
            g_console.Echo("    Current time is %f", currentTime);
        }
        else
            g_console.Echo("    No valid recordings");

        g_console.Echo("    Playback direction %s, speed %f, looping %s",
                d_fwd ? "forward" : "reverse", d_speed, d_loop ? "on" : "off");

        d_info = false;
    }
}

#endif //ENABLE_RECORD
