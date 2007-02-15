// ---------------------------------------------------------------------------
// File			-> CVoiceOverIPRoot.h
// Author		-> Dwain Skinner
// Created		-> January 22, 2004
// Description	-> Platform independent VoIP class
//
//  Copyright (c) 2004, Avalanche Software Inc.
//  All Rights Reserved.
// ---------------------------------------------------------------------------

#ifndef _CVOICEOVERIPROOT_
#define _CVOICEOVERIPROOT_

#include "container/Darray.h"
#include "Network/Host.h"
#include "GameHelper/Message.h"

//#include "Math/Matrix.h"
//#include "hud/hudlib.h"

class CVoiceOverIP;

class CVoiceOverIPRoot
{

public:
	enum eControlStates
	{
		CONTROL_RECORD = 0,
		CONTROL_PLAY,
		CONTROL_ENCODE,
		CONTROL_DECODE,
		CONTROL_TRANSMISSION,
		CONTROL_NUM_STATES
	};

	enum eGenericCodecs
	{
		CODEC_PCM = 0,
		CODEC_ULAW,
		CODEC_G723_1,
		CODEC_GSM,
		CODEC_LPC10,
		CODEC_CELP,
		CODEC_ADPCM
	};

	CVoiceOverIPRoot											( );
	~CVoiceOverIPRoot											( );

	void							enableControl				( eControlStates state, bool onOff );

	// --- each platform must derive these functions
	bool							create						( void );
	bool							destroy						( void );
	bool							update						( void );
	bool							sendTeamChat				( const char *text );
	bool							sendAllChat					( const char *text );
	//
	// --- status functions
	inline bool					isRecording					( void ) const;
	inline bool					isPlaying					( void ) const;
	inline bool					isNetworkUp					( void ) const;
	inline bool					chatPending					( void ) { return( m_chatPending ); }
	inline char 				*getChatMessage			( void ) { m_chatPending = false; return( m_chatMessage ); }

	void initialize				( );

protected:
	struct voipHeader
	{
		unsigned int		packetNumber;
		unsigned int		checkSum;
		unsigned short		codec;
		unsigned short		dataSize;
		unsigned short		decodedDataSize;
	};

	#define CODEC_BITS 0x00ff
	#define COMMAND_BITS 0xff00
	#define RECORD_BUFFER_SIZE ( 1024 * 32 )
	#define PLAY_BUFFER_SIZE ( 1024 * 32 )
	#define VOIP_HEADER_SIZE ( sizeof( voipHeader ) )
	#define NO_CODEC -1
	#define INVALID_CODEC -2
	#define STATE_UPDATE_FREQUENCY 240
	#define MAX_HOST_NAME 32

	enum eVoIPCommands
	{
		VOIP_COMMAND_VOICE_DATA = 0x0000,
		VOIP_COMMAND_TERMINATE	= 0x0100,
		VOIP_COMMAND_TEAM_CHAT	= 0x0200,
		VOIP_COMMAND_ALL_CHAT	= 0x0400,
	};


	u32							calcCheckSum				( u8 *data, u32 size );

	CVoiceDevice				*m_pCVoiceDevice;			// device that we will be using to play and record
	CVoiceCodec					*m_pCVoiceCodec;			// codec interface that we will use to encode/decode
	CVoiceOverIP				*m_pCVoiceOverIP;			// voice over IP interface
	u8								*m_pRecordBuffer;			// points to our record buffer
	u8								*m_pPlayBuffer;			// points to our play buffer
	u8								*m_pEncodeBuffer;			// points to our encode buffer
	u8								*m_pDecodeBuffer;			// points to our decode buffer
	bool							m_record;					// should we record
	bool							m_play;						// should we play
	bool							m_encode;					// should we encode this data
	bool							m_decode;					// should we decode this data
	bool							m_networkUp;				// is the network up
	bool							m_transmission;			// should we send/receive over the network
	bool							m_isPlaying;
	bool							m_isRecording;
	bool							m_send;						// should we send
	bool							m_receive;					// should we receive

	u32							m_recordSize;
	u32							m_encodeSize;
	u32							m_decodeSize;
	u32							m_sendSize;
	u32							m_receiveSize;
	int							m_lastPacketReceived;
	int							m_lastPacketSent;
	int							m_currentCodec;
	int							m_newCodec;
	int							m_numPeers;
	DynamicArray< const CHost *> m_peerHosts;
	DynamicArray< u32 >		m_peerHostIP;
	const CHost					*m_pLocalHost;
	char							m_hostName[ MAX_HOST_NAME ];
	u32							m_bufferedDataSize;
	u32							m_bufferReadOffset;
	u32							m_bufferWriteOffset;
	bool							m_bufferFilled;
	bool							m_terminationPending;

private:
	void							updatePressToTalk			( void );
	void							sendTerminationPacket	( );
	u32							sendPacket( u8 *pSrc, u32 size  );
	u32							receivePacket( u8 *pDst );
	s32							decodePacket( u8 *pSrc, s32 size );
	bool							showChat( u8 *pSrc, s32 length );
	char							*ipToAscii( u32 ip );
	void							updateState( );

	u32							m_chatMessageCount;
	u32							m_stateUpdateFrequency;
	bool							m_chatPending;

	char							m_chatMessage[ 512 ];
	char							m_ipStringBuffer[ 32 ];

	CMessageOwner				m_messageOwner;
	REF_SEND_MESSAGE_HANDLER(CHostAddedHandler, CVoiceOverIPRoot) m_hostAddedHandler;
	REF_SEND_MESSAGE_HANDLER(CHostRemovedHandler, CVoiceOverIPRoot) m_hostRemovedHandler;
};

// ---------------------------------- inlines --------------------------------
inline bool CVoiceOverIPRoot::isPlaying( void ) const
{
	return( m_play );
}

inline bool CVoiceOverIPRoot::isRecording( void ) const
{
	return( m_record );
}

inline bool CVoiceOverIPRoot::isNetworkUp( void ) const
{
	return( m_networkUp );
}
#endif // _CVOICEOVERIPROOT_
