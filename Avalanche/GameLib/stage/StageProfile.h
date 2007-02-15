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
**   $Archive: /Avalanche/gamelibnew/stage/StageProfile.h $
**  $Revision: 7 $
**      $Date: 7/08/03 5:28p $
**    $Author: Dross $
**
*/

#ifndef STAGE_PROFILE_H
#define STAGE_PROFILE_H

#include "profile/codetimer.h"

//#define STAGE_PROFILE
#define STAGE_PROFILE_USE_BAR_PROFILER
#ifdef STAGE_PROFILE_USE_BAR_PROFILER
#include "Layers/Timing.h"
#endif //STAGE_PROFILE_USE_BAR_PROFILER


class CStageProfile
{
  public:
    enum ESection
    {
        ADVANCE,
        BEGIN_FRAME,
        BUILD_CLUSTER,
        PROTAGONIST,
        FIRST_SCATTER,
        CLUSTER,
        LAST_SCATTER,
        NON_PROTAGONIST,
        ANIMATE,
        CHAIN_OBB,
        CHAIN_PREP,
        ANIM_PROCESS,
        CHAIN_SIM,
        GARBAGE,
        DEBUG,
		END_FRAME
    };

    CStageProfile();
    void Initialize(void);
    void Terminate(void);
    void Start(ESection i_section) {g_codeTimer.Start(d_handle, i_section);};
    void End(ESection i_section) {g_codeTimer.End(d_handle, i_section);};
  private:
    TIMERPAGEHANDLE d_handle;
};

extern CStageProfile g_stageProfile;

#ifdef STAGE_PROFILE
#define STAGE_PROFILE_INITIALIZE() g_stageProfile.Initialize()
#define STAGE_PROFILE_TERMINATE() g_stageProfile.Terminate()
#define STAGE_PROFILE_START(section) g_stageProfile.Start(section)
#define STAGE_PROFILE_END(section) g_stageProfile.End(section)
#elif defined(STAGE_PROFILE_USE_BAR_PROFILER)

/* to hook into profile bar system */

#define STAGE_PROFILE_INITIALIZE()	{																																								\
  													bool bResult;																																			\
													bResult = Profiler__ReserveSlot(CStageProfile::ADVANCE + PROFILE_UNRESERVED_START, "Stage profile: ADVANCE", PROFILE_NORMAL);				\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CStageProfile::BEGIN_FRAME + PROFILE_UNRESERVED_START, "Stage profile: BEGIN_FRAME", PROFILE_NORMAL);				\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CStageProfile::BUILD_CLUSTER + PROFILE_UNRESERVED_START, "Stage profile: BUILD_CLUSTER", PROFILE_NORMAL);				\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CStageProfile::PROTAGONIST + PROFILE_UNRESERVED_START, "Stage profile: PROTAGONIST", PROFILE_NORMAL);				\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CStageProfile::FIRST_SCATTER + PROFILE_UNRESERVED_START, "Stage profile: FIRST_SCATTER", PROFILE_NORMAL);				\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CStageProfile::CLUSTER + PROFILE_UNRESERVED_START, "Stage profile: CLUSTER", PROFILE_NORMAL);				\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CStageProfile::LAST_SCATTER + PROFILE_UNRESERVED_START, "Stage profile: LAST_SCATTER", PROFILE_NORMAL);				\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CStageProfile::NON_PROTAGONIST + PROFILE_UNRESERVED_START, "Stage profile: NON_PROTAGONIST", PROFILE_NORMAL);				\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CStageProfile::ANIMATE + PROFILE_UNRESERVED_START, "Stage profile: ANIMATE", PROFILE_NORMAL);				\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CStageProfile::CHAIN_OBB + PROFILE_UNRESERVED_START, "Stage profile: CHAIN_OBB", PROFILE_NORMAL);				\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CStageProfile::CHAIN_PREP + PROFILE_UNRESERVED_START, "Stage profile: CHAIN_PREP", PROFILE_NORMAL);				\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CStageProfile::ANIM_PROCESS + PROFILE_UNRESERVED_START, "Stage profile: ANIM_PROCESS", PROFILE_NORMAL);				\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CStageProfile::CHAIN_SIM + PROFILE_UNRESERVED_START, "Stage profile: CHAIN_SIM", PROFILE_NORMAL);				\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CStageProfile::GARBAGE + PROFILE_UNRESERVED_START, "Stage profile: GARBAGE", PROFILE_NORMAL);				\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CStageProfile::DEBUG + PROFILE_UNRESERVED_START, "Stage profile: DEBUG", PROFILE_NORMAL);				\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CStageProfile::END_FRAME + PROFILE_UNRESERVED_START, "Stage profile: END_FRAME", PROFILE_NORMAL);				\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
												}
#define STAGE_PROFILE_TERMINATE()
#define STAGE_PROFILE_START(section)	Profiler__Start(section + PROFILE_UNRESERVED_START)
#define STAGE_PROFILE_END(section)	Profiler__Stop(section + PROFILE_UNRESERVED_START)
#else
#define STAGE_PROFILE_INITIALIZE()
#define STAGE_PROFILE_TERMINATE()
#define STAGE_PROFILE_START(section)
#define STAGE_PROFILE_END(section)
#endif

#if defined(__SNTUNER_H_INC__) && 0
#undef STAGE_PROFILE_START
#undef STAGE_PROFILE_END
#undef STAGE_PROFILE_INITIALIZE
#undef STAGE_PROFILE_TERMINATE
#define STAGE_PROFILE_INITIALIZE()
#define STAGE_PROFILE_TERMINATE()
#define STAGE_PROFILE_START(section) snStartMarker(section+1, #section)
#define STAGE_PROFILE_END(section) snStopMarker(section+1)
#endif


#endif // STAGE_PROFILE_H
