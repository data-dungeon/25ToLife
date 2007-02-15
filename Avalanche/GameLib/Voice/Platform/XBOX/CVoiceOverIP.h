// ---------------------------------------------------------------------------
// File			-> CVoiceOverIP.h
// Author		-> Dwain Skinner
// Created		-> January 22, 2004
// Description	-> Platform dedependent VoIP class
//
//  Copyright (c) 2004, Avalanche Software Inc.
//  All Rights Reserved.
// ---------------------------------------------------------------------------

#ifndef _CVOICEOVERIP_
#define _CVOICEOVERIP_

class CVoiceOverIP
{

public:
	CVoiceOverIP												( );
	~CVoiceOverIP												( );

	// --- each platform must derive these functions
	bool							create						( void );
	bool							destroy						( void );
	bool							update						( void );

	// --- bring up the network
	bool							initNetwork					( );
	u32							sendPacket					( u8 *pSrc, u32 size, u32 ip );
	u32							receivePacket				( u8 *pDst, u32 ip );

protected:

private:
	bool							m_networkUp;
	int							m_udpSocket;
	int							m_udpSocketIn;
	int							m_udpPortIn;

#define ENABLE_DEBUG_STATS
#define DEBUG_OUTPUT_FREQUENCY 30*10

#ifdef ENABLE_DEBUG_STATS
	// --- debug
	void							dumpStats( );
	u32							m_largestPacketReceived;
	u32							m_largestPacketSent;
	u32							m_debugOutputFrequency;
	u32							m_gameDelay;
#endif

};

// ---------------------------------- inlines --------------------------------

#endif // _CVOICEOVERIP_