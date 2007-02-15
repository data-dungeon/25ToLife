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
**   $Archive: /Avalanche/GameLibNew/stage/CastProfile.h $
**  $Revision: 4 $
**      $Date: 9/30/02 5:18p $
**    $Author: Nmartin $
**
*/

#ifndef CAST_PROFILE_H
#define CAST_PROFILE_H

#include "profile/codetimer.h"

//#define CAST_PROFILE
//#define CAST_PROFILE_USE_BAR_PROFILER
#ifdef CAST_PROFILE_USE_BAR_PROFILER
#include "Layers/Timing.h"
#endif //CAST_PROFILE_USE_BAR_PROFILER



class CCastProfile
{
  public:
    enum ESection
    {
        UPDATE,
        FIND_TICK,
        CONTROL,
        PREP,
        INFLUENCE,
        ATTEMPT,
        RESOLVE,
        DETECT,
        BODY,
        TERRAIN,
        REDETECT,
        FAILURE,
        ACCEPT,
        RESPOND,
        END_FRAME
    };

    CCastProfile();
    void Initialize(void);
    void Terminate(void);
    void Start(ESection i_section) {g_codeTimer.Start(d_handle, i_section);};
    void End(ESection i_section) {g_codeTimer.End(d_handle, i_section);};
  private:
    TIMERPAGEHANDLE d_handle;
};

extern CCastProfile g_castProfile;

#ifdef CAST_PROFILE
#define CAST_PROFILE_INITIALIZE() g_castProfile.Initialize()
#define CAST_PROFILE_TERMINATE() g_castProfile.Terminate()
#define CAST_PROFILE_START(section) g_castProfile.Start(section)
#define CAST_PROFILE_END(section) g_castProfile.End(section)
#elif defined(CAST_PROFILE_USE_BAR_PROFILER)

/* to hook into profile bar system */

#define CAST_PROFILE_INITIALIZE()	{																																								\
  													bool bResult;																																			\
													bResult = Profiler__ReserveSlot(CCastProfile::UPDATE + PROFILE_UNRESERVED_START, "Cast profile: UPDATE", PROFILE_NORMAL);				\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CCastProfile::FIND_TICK + PROFILE_UNRESERVED_START, "Cast profile: FIND_TICK", PROFILE_NORMAL);		\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CCastProfile::CONTROL + PROFILE_UNRESERVED_START, "Cast profile: CONTROL", PROFILE_NORMAL);			\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CCastProfile::PREP + PROFILE_UNRESERVED_START, "Cast profile: PREP", PROFILE_NORMAL);					\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CCastProfile::INFLUENCE + PROFILE_UNRESERVED_START, "Cast profile: INFLUENCE", PROFILE_NORMAL);		\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CCastProfile::ATTEMPT + PROFILE_UNRESERVED_START, "Cast profile: ATTEMPT", PROFILE_NORMAL);			\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CCastProfile::RESOLVE + PROFILE_UNRESERVED_START, "Cast profile: RESOLVE", PROFILE_NORMAL);			\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CCastProfile::DETECT + PROFILE_UNRESERVED_START, "Cast profile: DETECT", PROFILE_NORMAL);				\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CCastProfile::BODY + PROFILE_UNRESERVED_START, "Cast profile: BODY", PROFILE_NORMAL);					\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CCastProfile::TERRAIN + PROFILE_UNRESERVED_START, "Cast profile: TERRAIN", PROFILE_NORMAL);			\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CCastProfile::REDETECT + PROFILE_UNRESERVED_START, "Cast profile: REDETECT", PROFILE_NORMAL);		\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CCastProfile::FAILURE + PROFILE_UNRESERVED_START, "Cast profile: FAILURE", PROFILE_NORMAL);			\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CCastProfile::ACCEPT + PROFILE_UNRESERVED_START, "Cast profile: ACCEPT", PROFILE_NORMAL);				\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CCastProfile::RESPOND + PROFILE_UNRESERVED_START, "Cast profile: RESPOND", PROFILE_NORMAL);			\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
													bResult = Profiler__ReserveSlot(CCastProfile::END_FRAME + PROFILE_UNRESERVED_START, "Cast profile: END_FRAME", PROFILE_NORMAL);		\
													ASSERT(bResult);	/* if this hits, slot is already in use. some slots are reserved-- see l_Timing.h and Timing.c */	\
												}
#define CAST_PROFILE_TERMINATE()
#define CAST_PROFILE_START(section)	Profiler__Start(section + PROFILE_UNRESERVED_START)
#define CAST_PROFILE_END(section)	Profiler__Stop(section + PROFILE_UNRESERVED_START)
#else
#define CAST_PROFILE_INITIALIZE()
#define CAST_PROFILE_TERMINATE()
#define CAST_PROFILE_START(section)
#define CAST_PROFILE_END(section)
#endif

#endif // CAST_PROFILE_H
