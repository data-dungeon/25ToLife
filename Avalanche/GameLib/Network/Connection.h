/*
**
**  File:   Connection.h
**  Date:   October 9, 2003
**  By:     Bryant Collard
**  Desc:   The network connection.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/Connection.h $
**  $Revision: #4 $
**  $DateTime: 2005/07/28 12:26:02 $
**    $Author: Dwain_Skinner $
**
*/

#pragma once

#ifndef CONNECTION_H
#define CONNECTION_H

#include "EngineHelper/Singleton.h"

class CConnection
{
  public:
	virtual bool IsEnabled(void) {return(false);}
	virtual void Service(void) {}
	virtual void SetConnectionTimeout( bool i_useLongTimeout ){};

	// Memory usage
	virtual u32 GetMemoryUsage ( void )		{ return 0; }

	// --- Network traffic data tracking functions
	virtual u32 GetBPSReceivedAvg( void )	{ return 0; }
	virtual u32 GetBPSReceivedMax( void )	{ return 0; }
	virtual u32 GetBPSReceivedCur( void )	{ return 0; }
	virtual u32 GetBPSSentAvg( void )		{ return 0; }
	virtual u32 GetBPSSentMax( void )		{ return 0; }
	virtual u32 GetBPSSentCur( void )		{ return 0; }
	virtual void ResetBPSCounters( void )	{ }
	virtual u32 GetPacketsReceived( void )	{ return 0; }
	virtual u32 GetPacketsSent( void )		{ return 0; }
	virtual u32 GetPacketsLost( void )		{ return 0; }
	virtual u32 GetPacketsLostTotal( void ){ return 0; }
	virtual u32 GetReliableTransmittedTotal( void ){ return 0; }
	virtual u32 GetReliableRetransmittedTotal( void ){ return 0; }
	virtual u32 GetReliableTransmittedCur( void ){ return 0; }
	virtual u32 GetReliableReTransmittedCur( void ){ return 0; }
	virtual void DumpCurrentStats( void )	{ }
	virtual void DumpAverageStats( void )	{ }

	virtual void FlushBundles(void) {}
	virtual void ToggleBundling(void) {}
	virtual void MarkTrace(void) {}
};

DefineSingleton(CConnection)
#define g_connection GetSingleton(CConnection)

#endif // CONNECTION_H
