/*
**
**  File:   StageProfile.h
**  Date:   December 5, 2001
**  By:     Bryant Collard
**  Desc:   Support for profiling stage code.
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/stage/StageProfile.cpp $
**  $Revision: 6 $
**      $Date: 11/21/02 12:25p $
**    $Author: Dross $
**
*/

/******************* includes ****************************/

#include "stage/StagePCH.h"





CStageProfile g_stageProfile;

CStageProfile::CStageProfile()
{
    d_handle = NULL;
}

void CStageProfile::Initialize(void)
{
    CODESECTION stageCS[] =
    {
        {
            "Advance          ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            false
        },
        {
            "Control        ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            false
        },
        {
            "Build Cluster  ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            false
        },
        {
            "Protagonist    ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            false
        },
        {
            "First Scatter",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            false
        },
        {
            "Cluster      ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            false
        },
        {
            "Last Scatter ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            false
        },
        {
            "Non-Protagonist",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            false
        },
        {
            "Animate        ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            false
        },
        {
            "Chain OBB    ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            true
        },
        {
            "Chain Prep   ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            true
        },
        {
            "Anim Process ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            true
        },
        {
            "Chain Sim  ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            true
        },
        {
            "Garbage        ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            false
        },
        {
            "Debug          ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            true
        }
    };

    d_handle = g_codeTimer.RegisterSections("Stage Timings",
            stageCS, sizeof(stageCS) / sizeof(CODESECTION));
    g_codeTimer.EnableWidthMemory(d_handle);
}

void CStageProfile::Terminate(void)
{
    g_codeTimer.DestroySections(d_handle);
    d_handle = NULL;
}
