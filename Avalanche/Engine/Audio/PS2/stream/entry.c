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
 *                           entry.c
 *                  IOP code of Multistream's API
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      5.00            Oct,31,2001      Jason Page 
 *      6.00            Nov,28,2002      Jason Page 
 *      6.10            Jan,31,2003      Jason Page 
 *      6.20            Feb,25,2003      Jason Page 
 *      7.3             Nov,18,2004      Jason Page 
 */

#include <kernel.h>
#include <sys/types.h>
#include <stdio.h>
#include <sif.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <msifrpc.h>
#include "stream.h"

ModuleInfo Module = {"multi_streamer", 0x0102};

/* in stream.c */
extern int sce_adpcm_loop (void);
int MS_RPCThreadID;

int start (void)
{
struct ThreadParam param;
int th;

  if (! sceSifCheckInit ())
	sceSifInit ();

	sceSifInitRpc(0);

    param.attr         = TH_C;
    param.entry        = sce_adpcm_loop;
    param.initPriority = BASE_priority - 2;
    param.stackSize    = 0x2000;		/* Was 0x800 */
    param.option       = 0;
    th = CreateThread (&param);
    if (th > 0)
    {
		MS_RPCThreadID=th;
		StartThread (th, 0);


		printf (" Exit MultiStream V7.3 Thread Load \n");
		return 0;
    }
	    else
    {
		return 1;
    }
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* This file ends here, DON'T ADSTUFF AFTER THIS */
