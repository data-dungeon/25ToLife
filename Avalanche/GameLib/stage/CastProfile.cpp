/*
**
**  File:   CastProfile.h
**  Date:   December 5, 2001
**  By:     Bryant Collard
**  Desc:   Support for profiling cast code.
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/stage/CastProfile.cpp $
**  $Revision: 4 $
**      $Date: 11/21/02 12:25p $
**    $Author: Dross $
**
*/

/******************* includes ****************************/

#include "stage/StagePCH.h"





CCastProfile g_castProfile;

CCastProfile::CCastProfile()
{
    d_handle = NULL;
}

void CCastProfile::Initialize(void)
{
    CODESECTION castCS[] =
    {
        {
            "Update       ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            true
        },
        {
            "Find Tick  ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            true
        },
        {
            "Control    ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            true
        },
        {
            "Prep       ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            true
        },
        {
            "Influence  ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            true
        },
        {
            "Attempt    ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            true
        },
        {
            "Resolve    ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            true
        },
        {
            "Detect   ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            true
        },
        {
            "Body   ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            true
        },
        {
            "Terrain",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            true
        },
        {
            "Retry  ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            true
        },
        {
            "Failure  ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            true
        },
        {
            "Accept   ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            true
        },
        {
            "Respond  ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            true
        },
        {
            "End Frame  ",
            TIMEFORMAT_MICROSEC,
            TIME_SHOWAVG | TIME_SHOWBEST | TIME_SHOWWORST,
            true
        }
    };

    d_handle = g_codeTimer.RegisterSections("Cast Timings",
            castCS, sizeof(castCS) / sizeof(CODESECTION));
    g_codeTimer.EnableWidthMemory(d_handle);
}

void CCastProfile::Terminate(void)
{
    g_codeTimer.DestroySections(d_handle);
    d_handle = NULL;
}
