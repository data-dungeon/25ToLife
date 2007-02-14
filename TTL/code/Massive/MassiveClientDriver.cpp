/****************************************************************************
 *
 * MassiveClientDriver.cpp
 *
 * Copyright (C) 2005 Massive Incorporated
 *
 * Author: Massive Incorporated
 *                                                                         
 * Purpose: Integration of Massive Client into '25 to Life' for Ritual 
 *			           
 *
 ****************************************************************************/
#include "stdio.h"      // for fprintf();
#include "string.h"
//#define MASSIVE_DEBUG

#include "Display/DisplayPCH.h"
#include "MassiveMediaTracking.h"
#include "MassiveClientDriver.h"
#include "MassivePublicKey.h"
#include "MassiveAdObject.h"

#ifdef MASSIVE_DEBUG
//These are needed for console debug output
//for Massive's QA
#include "Game/GamePCH.h"
#include "cmdcon/cmdcon.h"
#endif


static MassiveMediaHandle	mmhZone = MMT_INVALID_HANDLE;	// Handle of the current zone.


//******************************************************************************
// MassiveClientInitDriver()
// Call me crazy but I bet this Initiates the Client Driver
//******************************************************************************
bool MassiveClientInitDriver(char *gameName)
{
    // Initialize media tracking system
	MMT_MASSIVE_INIT_STRUCT initStruct;
	initStruct.s_publicKey = (unsigned char*)Massive_PublicKey;
	initStruct.s_SKU = gameName;
	initStruct.u_libraryConfigFlags = MMT_ENABLE_LOGGING;
	initStruct.u_impressionFlushTime = 0;
	initStruct.v_GameTag = NULL;
	if( MMT_SUCCESS != MMT_Initialize(&initStruct) )
    {
        // Unable to initialize ad client system; load default ad media
		#ifdef MASSIVE_DEBUG
			g_console.Echo("MMT_Initialize: ERROR. MMT_Initialize failed.\n");
		#endif
        return false;
    }
	//Ensure internal LinkList is clean (it better be at init)
	m_MassiveIE.RemoveAll();
	return true;
}


//MassiveObjectFrameTick(mao);
//******************************************************************************
// RpMassiveClienZonetFrameTick()
// Client has to call every frame to keep Massive Client Driver up to date used during Zone Updates
//******************************************************************************
void MassiveClientZoneFrameTick()
{
	// Make call to MMT_NetUpdate for this frame (should occur once per frame)

	if (MMT_SUCCESS != MMT_NetUpdate()) 
	{
		#ifdef MASSIVE_DEBUG
			g_console.Echo("MMT_NetUpdate: ERROR. MMT_NetUpdate failed.\n");
		#endif
	}
 		// Wait for the Zone to download then proceed ticking the IEs
		MMT_STATUS eStatus = MMT_STATUS_INVALID;
		MMT_ERROR err = MMT_GENERROR;

		err = MMT_GetStatus(mmhZone, &eStatus);
		if (MMT_SUCCESS == err) 
		{
			if(MMT_STATUS_OBJECT_READY == eStatus)
			{
					MassiveObjectFrameTick();
			}
		}
		else 
		{
			#ifdef MASSIVE_DEBUG
				g_console.Echo("MMT_GetStatus: ERROR. Failed while getting status of a zone handle.\n");
				g_console.Echo("g_hZone = %d]\n", mmhZone);
			#endif
		}


}

//******************************************************************************
// RpMassiveClientLoadZone(Zone Name)
// Request download of Zone information from Massive AdServer
//******************************************************************************

bool MassiveClientLoadZone(const char *zoneName)
{
	MMT_ERROR err = MMT_GENERROR;
	MMT_STATUS stat = MMT_STATUS_INVALID;
	err = MMT_GetZoneHeaders(zoneName, &mmhZone);
	if (MMT_SUCCESS != err) 
	{
		#ifdef MASSIVE_DEBUG
			g_console.Echo("MMT_GetZoneHeaders: ERROR. Request for Zone information failed.\n");
			g_console.Echo("[Zone Name = %s]\n", zoneName);
		#endif
		return false;
	}
	else
	{
/*
		do
		{
	       MassiveClientZoneFrameTick();
		   err = MMT_GetStatus(mmhZone,&stat);
		}while(stat != MMT_STATUS_OBJECT_READY);
*/
		m_MassiveIE.RemoveAll(); //Entering a new zone clear out all out IE's
	}
	return true;
}

//******************************************************************************
// MassiveClientReleaseDriver()
// So simple even a cave man can do it...
//******************************************************************************
void MassiveClientReleaseDriver(void)
{
	m_MassiveIE.RemoveAll();
	MMT_DeInitialize();
	#ifdef MASSIVE_DEBUG
	g_console.Echo("DeInitializing AdClient\n");
#endif

}



