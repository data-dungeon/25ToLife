/* SCE CONFIDENTIAL
 $PSLibId$
 */
/*
 *        Emotion Engine (I/O Processor) Library Sample Program
 *
 *                         - Multistream -
 *
 *                           Shift-JIS
 *
 *      Copyright (C) 2000-2001 Sony Computer Entertainment Europe
 *                        All Rights Reserved.
 *
 *                            ms_switches.h
 *                  IOP code of Multistream's API
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *			7.0			11/June/2003	Jason Page
 */

#ifndef MS7_SWITCHES
#define MS7_SWITCHES

/****** Make SCREAM compatible *****/
#define SCREAM_COMPATIBLE	0	// 0=OFF, 1=ON

/****** Delay KeyOn timers ***** */

#define ONE_T ((1000*1000)/48000)	/* One TICK	(1/48000th of a second) */
#define TWO_T (ONE_T*2)				/* Two TICK's */


/* Revert back to the MultiStream 5.2 way of loading. */
/* Which means that your SPU buffers are the same size as your IOP buffers */
/* The reason? There may be problems with the small SPU buffer code, so removing this is the safe option */
/* Note - No problems detected since 7.0 release */
#define MS_LOAD62 1	// 0 = 5.2 loading code, 1 = 6.2+ loading code

#define SOUND_DTS_ENABLE 0		// 1 = Allow DTS playback

/* DEBUG switch for SOUND_SetParam, SOUND_SetAddr, SOUND_SetCoreAttr,SOUND_SetSwitch. */
/* If you pass an incorrect command value (such as SD_P_PITCH when calling SOUND_SetAddress instead of SetParam), */
/* a debug printf will tell you so.. */
#define SDR_DEBUG 1

/* NOTE: Using WaitVBlank will cause loading to take approx twice as long as interrupt timers */
#define MS_TIMER 1	/* Set to 0 to use WaitVBlank or 1 for using interrupt timers to wake StreamADPCM thread. */

/* Realtime DSP effect switch. */
/* Thanks to Criterion Games for first showing this technique at the Sony DevStation 2003 */
#define MS_DSP_ACTIVE 1		// Allow DSP functions..
#define MS_DSP_DEBUG 0		// Checks for CPU overruns..
/* Sets a debug mode, where you can check how many scan lines certain routines take */
#define MS_SCANLINE_DEBUG 0

#define DSP_FILTER_OPTIMISED 0	// 1 = Filter code will use assembly found in bgm_r2s.s, 0 = code in C (ms_dsp.c)

#ifdef _DEBUG
#define DEBUG_STATE	/* COMMENT OUT THIS LINE TO REMOVE ALL MSERRORS (printf's)...!! */
#endif

#ifdef DEBUG_STATE
#define MSERROR(x) printf x
#else
#define MSERROR(x) {}
#endif

#if (MS_SCANLINE_DEBUG==0)
#define ONE_HSCAN 1000*1000/60	/* Approx 60 frames per second (Timer for how often StreamADPCM is called) */
#else
#define ONE_HSCAN 1000*1000/(60*312)	/* Approx 60 frames per second (Timer for how often StreamADPCM is called) */
#endif

#endif
