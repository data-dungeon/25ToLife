/****************************************************************************
 *
 * MassiveClientDriver.h
 *
 * Copyright (C) 2004 Massive Incorporated
 *
 * Author: Massive Incorporated
 *                                                                         
 * Purpose: ntegration of Massive Client into '25 to Life' for Ritual  
 *			          
 *
 ****************************************************************************/

#include "MassiveMediaTracking.h"

#ifndef MASSIVECLIENT_H
#define MASSIVECLIENT_H


#define MASSIVE_GAME_NAME "25_to_life_pc_na"




#ifdef    __cplusplus
extern "C"
{
#endif  /* __cplusplus */

extern bool MassiveClientInitDriver(char *sz_gamename);

extern void MassiveClientReleaseDriver(void);

extern bool MassiveClientLoadZone(const char *sz_zonename);

//extern void RpMassiveClientFrameTick(RwCamera *currentCamera);

extern void MassiveClientZoneFrameTick(void);

extern void MassiveClientFlushImpressions(void);


#ifdef    __cplusplus
}
#endif  /* __cplusplus */

#endif /* PHYSICS_H */
