/*
**
**  File:   NetZConnection.h
**  Date:   October 9, 2003
**  By:     Bryant Collard
**  Desc:   The network connection.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/NetZConnection.h $
**  $Revision: #18 $
**  $DateTime: 2005/08/26 11:32:46 $
**    $Author: Dwain_Skinner $
**
*/

#pragma once

#ifndef NETZ_CONNECTION_H
#define NETZ_CONNECTION_H

// NetZ includes
#include <netz.h>

// Base class
#include "Network/Connection.h"

#define TRACE_STATS
#define TRACE_LOG
//#define EVENT_LOG
#ifndef CDROM
//#define SIMULATE_CONDITIONS
#endif

#ifdef SIMULATE_CONDITIONS
struct SNetworkSimParam
{
	float packetDropProbability;
	int routerBufferSize;
	int inputBandwidth;
	int inputJitter;
	int inputLatency;
	int outputBandwidth;
	int outputJitter;
	int outputLatency;
};
#endif

class CNetZConnection : public CConnection
{
  public:
	CNetZConnection(void);
	virtual ~CNetZConnection(void);

	bool Enable(bool i_lanSession);
	void Disable(void);
	bool IsConfigured(void) {return(s_configured == CONFIGURATION_SUCCEEDED);}
	virtual bool IsEnabled(void) {return(d_net != NULL);}
	virtual void Service(void);

	void InitializeTestSuite(void);
	void TerminateTestSuite(void);
	void SetConnectionTimeout( bool i_useLongTimeout );

	// Memory usage
	virtual u32 GetMemoryUsage ( void );

	// --- Network traffic data tracking functions
	virtual u32 GetBPSReceivedAvg( void );
	virtual u32 GetBPSReceivedMax( void );
	virtual u32 GetBPSReceivedCur( void );
	virtual u32 GetBPSSentAvg( void );
	virtual u32 GetBPSSentMax( void );
	virtual u32 GetBPSSentCur( void );
	virtual void ResetBPSCounters( void );
	virtual u32 GetPacketsReceived( void );
	virtual u32 GetPacketsSent( void );
	virtual u32 GetPacketsLost( void );
	virtual u32 GetPacketsLostTotal( void );
	virtual u32 GetReliableTransmittedTotal( void );
	virtual u32 GetReliableRetransmittedTotal( void );
	virtual u32 GetReliableTransmittedCur( void );
	virtual u32 GetReliableReTransmittedCur( void );
	virtual void DumpCurrentStats( void );
	virtual void DumpAverageStats( void );

	virtual void FlushBundles(void);
	virtual void ToggleBundling(void);
	virtual void MarkTrace(void);

	// Try to configure.
	static bool Configure(void);

	// DLL support
	static void loadNetworkDLLs( );
	static void unLoadNetworkDLLs( );

	// Network configuration support
	static void SetNetworkConfiguration( void *list, int index );
	static void GetNetworkConfiguration( void **list, int *index );

	// --- Platform specific
	static bool InitiateConnection( bool wait );
	static bool UpdateConnectionStatus( void );
	static bool TearDown( void );
	static bool GetLinkStatus( void );
	static bool ConnectionUp( void );
	static bool GetLinkError( ) { return s_linkError; }
	static inline bool IsConfigValid( ) { return( d_profileList != NULL && d_profileIndex != -1 ); }
	static void InitPaths();
	static const char *GetConnectionErrorMessage( void );

	// Logging
	static void InitializeLog(void);
	static void TerminateLog(void);

	// Network configuration settings.
	struct SNetworkConfigSettings
	{
		// Bundling
		bool d_bundleEnabled;
		bool d_bundleMixed;
		uint32 d_bundleMaxSize;
		int32 d_bundleMaxFlushDelay;

		// Stream Bundling
		bool d_streamBundleEnabled;
		int32 d_streamBundleMaxFlushDelay;

		// Transport
		uint32 d_prudpKeepAliveTimeout;
		uint32 d_prudpMaxRetransmission;
		uint32 d_prudpLongMaxSilenceTime;
		uint32 d_prudpShortMaxSilenceTime;
		uint32 d_prudpPS2TimeSlice;
		uint32 d_prudpOtherTimeSlice;
		uint32 d_prudpWindowSize;
		float d_prudpRetransmitTimeoutMultiplier;
		float d_prudpExtraRetransmitTimeoutMultiplier;
		uint32 d_prudpExtraRetransmitTimeoutTrigger;
		uint32 d_prudpRttRetransmit;
		uint32 d_prudpInitialRtt;

		// Connection degradation recognition
		bool d_connectionStabilityEnabled;
		float d_connectionStabilityMinThreshold;
		float d_connectionStabilityMaxThreshold;
		float d_connectionStabilityResentThreshold;
		float d_connectionStabilityLostThreshold;
		uint32 d_connectionStabilityMinPlayers;

		// Trace
		uint32 d_traceFlags;
	};
	static SNetworkConfigSettings s_netSettings;
	static void LoadNetworkConfigSettings(void);

  private:
	// Configuration status.
	static enum EConfigurationStatus
	{
		NOT_CONFIGURED,
		CONFIGURATION_FAILED,
		CONFIGURATION_SUCCEEDED
	} s_configured;

	// NetZ object.
	NetZ* d_net;
	bool d_servicing;

	// Compression/Encryption
	CompressionAlgorithm* d_compressionAlgorithm;
	EncryptionAlgorithm* d_encryptionAlgorithm;

	// performance counters
	TransportPerfCounters* GetPerformanceCounters(void);
	void ResetPerformanceCounters(void);
	uint32 GetPerformanceCount(int i_counterID, int i_counterValue);
#ifdef TRACE_STATS
	uint32 d_nextStatTime;
#endif

	// trace log support
#ifdef TRACE_LOG
	static LogDevice* s_traceLogDevice;
#endif
#ifdef EVENT_LOG
	static LogDevice* s_eventLogDevice;
#endif

	// param manager stuff for simulated network conditions
	void InitializeNetworkSimulation(void);
	void ResetNetworkSimulation(void);
	void TerminateNetworkSimulation(void);
#ifdef SIMULATE_CONDITIONS
	SNetworkSimParam *d_simParam;
	REF_BASIC_MESSAGE_HANDLER(CSimInputHandler, MESSAGE_VALUE, CNetZConnection)
			d_simInputHandler;
	REF_BASIC_MESSAGE_HANDLER(CSimOutputHandler, MESSAGE_VALUE, CNetZConnection)
			d_simOutputHandler;
	CMessageOwner d_messageOwner;
#endif


	// --- Network configuration support
	static void *d_profileList;
	static int d_profileIndex;
	static bool s_linkError;
};

#endif // NETZ_CONNECTION_H
