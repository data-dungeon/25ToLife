// ---------------------------------------------------------------------------
// File			-> CVoiceOverIPRoot.cpp
// Author		-> Dwain Skinner
// Created		-> January 22, 2004
// Description	-> Platform independent VoIP class
//
//  Copyright (c) 2004, Avalanche Software Inc.
//  All Rights Reserved.
// ---------------------------------------------------------------------------

// --- Precompiled header
#include "Voice/VoicePCH.h"

#include "Voice/netz/cvoicechat.h"

// --- local defines
#define ENCODED_PACKET_SIZE 49
#define VOICE_FRAME_SIZE 320
#define BUFFERED_TIME (30)

// ----------------------------------------------------------------------------
CVoiceOverIPRoot::CVoiceOverIPRoot( ) : 
	m_pCVoiceDevice( NULL ), 
	m_pCVoiceCodec( NULL ), 
	m_pCVoiceOverIP( NULL ),
	m_pRecordBuffer( NULL ), 
	m_pPlayBuffer( NULL ), 
	m_pEncodeBuffer( NULL ), 
	m_pDecodeBuffer( NULL ),
	m_record( false ),
	m_play( false ),
	m_encode( false ),
	m_decode( false ),
	m_send( false ),
	m_receive( false ),
	m_lastPacketSent( 1 ),
	m_lastPacketReceived( 0 ),
	m_currentCodec( NO_CODEC ),
	m_newCodec( INVALID_CODEC ),
	m_pLocalHost( NULL ),
	m_numPeers( 0 ),
	m_bufferedDataSize( 0 ),
	m_bufferReadOffset( 0 ),
	m_bufferWriteOffset( 0 ),
	m_bufferFilled( false ),
	m_terminationPending( false ),
	m_chatPending (false),
	m_stateUpdateFrequency( STATE_UPDATE_FREQUENCY ),
	m_chatMessageCount( 0 ),
	m_messageOwner( "VoiceOverIPRoot" )
{
	memset( m_hostName, 0, sizeof( m_hostName ) );
	create( );
}


// ----------------------------------------------------------------------------
CVoiceOverIPRoot::~CVoiceOverIPRoot( )	
{
}


// ----------------------------------------------------------------------------
bool CVoiceOverIPRoot::create( void )
{
	m_pCVoiceOverIP = new CVoiceOverIP;
	ASSERTS( m_pCVoiceOverIP, "could not create voice over IP object" );

	m_networkUp = m_pCVoiceOverIP->initNetwork( );
	if( !isNetworkUp( ) )
	{
		delete m_pCVoiceOverIP;
		return( false );
	}

#if 0
	// --- temporary debug to allow voice to be sent and received on a particular host
	m_peerHosts.Add( ( const CHost * ) 0x8000 );
	m_peerHostIP.Add( 0xC0A80003 );
	m_numPeers++;
#endif


	if( ( m_pCVoiceDevice = new CPlatformVoiceDevice ) )
	{
		// --- create the record/play buffers
		m_pRecordBuffer	= m_pCVoiceDevice->createBuffer( RECORD_BUFFER_SIZE );
		m_pPlayBuffer		= m_pCVoiceDevice->createBuffer( PLAY_BUFFER_SIZE );

		if( ( m_pCVoiceCodec = new CVoiceCodec ) )
		{
			m_pCVoiceCodec->create( );
			m_pCVoiceCodec->setCodecIndex( 3 );
			m_currentCodec = m_pCVoiceCodec->getCodecIndex( );

			// --- create the encode/decode buffers
			m_pEncodeBuffer	= m_pCVoiceDevice->createBuffer( RECORD_BUFFER_SIZE );
			m_pDecodeBuffer	= m_pCVoiceDevice->createBuffer( PLAY_BUFFER_SIZE );

			// --- enable states
			enableControl( CONTROL_RECORD, false );
			enableControl( CONTROL_PLAY, false );
			enableControl( CONTROL_ENCODE, true );
			enableControl( CONTROL_TRANSMISSION, true );
			m_send = true;
			m_receive = true;
			return( true );
		}
		ASSERTS( m_pCVoiceCodec, "unable to create voice codec" );
		return( false );
	}
	else
	{
		ASSERTS( m_pCVoiceDevice, "unable to create voice device" );
		return( false );
	}


	return( true );
}


// ----------------------------------------------------------------------------
bool CVoiceOverIPRoot::destroy( )
{
	if( m_pCVoiceDevice )
	{
		delete m_pCVoiceDevice;
		m_pCVoiceDevice = NULL;
	}

	if( m_pCVoiceCodec )
	{
		delete m_pCVoiceCodec;
		m_pCVoiceCodec = NULL;
	}
	return( true );

	return( true );
}

void CVoiceOverIPRoot::initialize( )
{
	// --- register with message handler
	m_hostAddedHandler.Initialize( "RemoteHostAdded", MESSAGE_HANDLE_IF_MASTER, m_messageOwner, this );
	m_hostRemovedHandler.Initialize( "RemoteHostRemoved", MESSAGE_HANDLE_IF_MASTER, m_messageOwner, this );
}

// ----------------------------------------------------------------------------
bool CVoiceOverIPRoot::update( )
{

	// --- init these each frame
	m_recordSize 	= 0;
	m_encodeSize 	= 0;
	m_decodeSize 	= 0;
	m_sendSize		= 0;
	m_receiveSize	= 0;

#ifdef ENABLE_DEBUG_STATS
//	dumpStats( );
#endif


	// --- info about us
	if( !m_pLocalHost )
	{
		if( ValidSingleton( CSession ) && g_session.IsNetworked( ) )
		{
			const char *p = ( g_session.GetLocalHost( ) )->GetName( );
			if( p )
			{
				strcpy( m_hostName, p );
			}
		}
	}

	if( m_pCVoiceDevice )
	{
		// --- update state between peers
		updateState( );

		// --- update press to talk mechanism
		updatePressToTalk( );

		// --- must call device's update routine
		m_pCVoiceDevice->update( );

		// --- should we record data?
		while( m_record && ( m_pCVoiceDevice->getRecordedBytes( ) >= VOICE_FRAME_SIZE ) )
		{
			m_recordSize = m_pCVoiceDevice->record( m_pRecordBuffer, VOICE_FRAME_SIZE );

			// --- should we encode this data?
			if( m_encode && m_recordSize )
			{
				if( m_newCodec != INVALID_CODEC )
				{
					m_currentCodec = m_newCodec;
					m_pCVoiceCodec->setCodecIndex( m_newCodec );
					m_newCodec = INVALID_CODEC;
				}
				// --- attach a VOIP header in the data and encode the recorded data
				voipHeader *pHeader = ( voipHeader * )m_pEncodeBuffer;
				pHeader->packetNumber	= m_lastPacketSent;
				pHeader->codec				= m_currentCodec;
				pHeader->decodedDataSize= ( u16 )m_recordSize;
				m_encodeSize				= m_pCVoiceCodec->encode( m_pRecordBuffer, m_pEncodeBuffer + VOIP_HEADER_SIZE, m_recordSize );
				pHeader->dataSize			= ( u16 )m_encodeSize + VOIP_HEADER_SIZE;
				pHeader->checkSum			= calcCheckSum( m_pEncodeBuffer + VOIP_HEADER_SIZE, m_encodeSize );
			}

			// --- send our data
			if( m_encodeSize && m_send )
			{
				m_sendSize		= sendPacket( m_pEncodeBuffer, m_encodeSize + VOIP_HEADER_SIZE );
			}
		}

		// --- receive our data
		if( m_receive )
		{
			u32 size;

			if( m_pCVoiceDevice->getPlayBackBytes( ) == 0 ) 
			{
				m_bufferFilled = false;
				m_pCVoiceDevice->stopPlaying( );
			}

			// --- receive packets as long as they're available
			while( ( m_receiveSize	= receivePacket( m_pDecodeBuffer + m_bufferWriteOffset ) ) )
			{
				voipHeader *pVoipHeader = ( voipHeader * )( m_pDecodeBuffer + m_bufferWriteOffset );
				if( pVoipHeader->dataSize != m_receiveSize )
				{
					dbgPrint( "Received data size does not match header data size %d != %d...", m_receiveSize, pVoipHeader->dataSize );
				}

				size = m_receiveSize;
				size += 0x10;
				size &= 0xfffffff0;

				m_bufferedDataSize += size;
				m_bufferWriteOffset += size;
				ASSERTS( m_bufferedDataSize < RECORD_BUFFER_SIZE, "buffer overflow" );
				ASSERTS( m_bufferWriteOffset < RECORD_BUFFER_SIZE, "buffer overflow" );
				if( ( m_bufferWriteOffset + ( ENCODED_PACKET_SIZE * 2 ) ) >= ( RECORD_BUFFER_SIZE ) )
				{
					m_bufferWriteOffset = 0;
				}
				//dbgPrint( "Received data, size = %d...\n", m_receiveSize );
				//dbgPrint( "m_bufferWriteOffset = %d...\n", m_bufferWriteOffset );
				//dbgPrint( "m_bufferedDataSize  = %d...\n", m_bufferedDataSize );
			}

			// --- make sure we've buffered up some data
			if( m_bufferedDataSize >= ( ENCODED_PACKET_SIZE * BUFFERED_TIME ) || ( m_bufferFilled && m_bufferedDataSize ) )
			{
				//dbgPrint( "enable Play" );
				enableControl( CONTROL_PLAY, true );
			}

			// --- should we play data?
			while( m_play && m_bufferedDataSize )
			{
				u8 *pCurData = ( u8 * )( m_pDecodeBuffer + m_bufferReadOffset );

				// --- get at our packet header
				voipHeader *pVoipHeader = ( voipHeader * )pCurData;
				if( m_lastPacketReceived >= 0 )
				{
					if( m_lastPacketReceived + 1 != pVoipHeader->packetNumber )
					{
						dbgPrint( "UDP packet dropped... last = %d, current = %d...\n", m_lastPacketReceived, pVoipHeader->packetNumber );
					}
				}
				// --- checksum our received data against the checksum in the packet header
				if( pVoipHeader->checkSum != calcCheckSum( pCurData + VOIP_HEADER_SIZE, pVoipHeader->dataSize - VOIP_HEADER_SIZE ) )
				{
					dbgPrint( "checksum does not match on packet #%d...\n", pVoipHeader->packetNumber );
				}
				m_lastPacketReceived = pVoipHeader->packetNumber;

				// --- look to change our codec if it does not match
				if( ( pVoipHeader->codec & CODEC_BITS ) != m_currentCodec )
				{
					m_currentCodec = ( pVoipHeader->codec && CODEC_BITS );
					m_pCVoiceCodec->setCodecIndex( m_currentCodec );
				}

				// --- decode and play
				m_decodeSize = m_pCVoiceCodec->decode( m_pPlayBuffer, pCurData + VOIP_HEADER_SIZE, pVoipHeader->dataSize - VOIP_HEADER_SIZE, PLAY_BUFFER_SIZE );
				if( m_decodeSize != pVoipHeader->decodedDataSize )
				{
					dbgPrint( "Original recorded size does not match decoded size, %d != %d\n", m_decodeSize, pVoipHeader->decodedDataSize );
				}
				m_pCVoiceDevice->play( m_pPlayBuffer, m_decodeSize );

				size = pVoipHeader->dataSize;
				size += 0x10;
				size &= 0xfffffff0;
				m_bufferedDataSize -= size;
				ASSERTS( m_bufferedDataSize >= 0, "should never go negative" );
				m_bufferReadOffset += size;
				if( ( m_bufferReadOffset + ( ENCODED_PACKET_SIZE * 2 ) ) >= ( RECORD_BUFFER_SIZE ) )
				{
					m_bufferReadOffset = 0;
				}
				if( m_bufferedDataSize <= 0 )
				{
					// --- if we have not started playing start
					m_pCVoiceDevice->startPlaying( );
					m_bufferFilled = false;
					enableControl( CONTROL_PLAY, false );
					if( !m_terminationPending )
						m_bufferFilled = true;
					m_terminationPending = false;
				}
				ASSERTS( m_bufferedDataSize < RECORD_BUFFER_SIZE, "buffer overflow" );
				ASSERTS( m_bufferReadOffset < RECORD_BUFFER_SIZE, "buffer overflow" );

				//dbgPrint( "m_bufferFilled      = %d...\n", m_bufferFilled );
				//dbgPrint( "m_bufferReadOffset  = %d...\n", m_bufferReadOffset );
				//dbgPrint( "m_bufferedDataSize  = %d...\n", m_bufferedDataSize );
			}
		}
	}
	return( true );
}

// ----------------------------------------------------------------------------
void CVoiceOverIPRoot::enableControl( eControlStates state, bool onOff )
{
	switch( state )
	{
		case CONTROL_RECORD:
			m_record = onOff;
			break;

		case CONTROL_PLAY:
			m_play = onOff;
			break;

		case CONTROL_ENCODE:
			m_encode = onOff;
			break;

		case CONTROL_DECODE:
			m_decode = onOff;
			break;

		case CONTROL_TRANSMISSION:
			m_transmission = onOff;
			break;

		default:
			ASSERTS( state < CONTROL_NUM_STATES, "Invalid control state specified" );
	}
}

// ---------------------------------------------------------------------------
u32 CVoiceOverIPRoot::calcCheckSum( u8 *data, u32 size )
{
	u32 checkSum = 0;

	for( u32 i = 0; i < size; i++ )
	{
		checkSum += *data;
	}

	checkSum ^= 0xffffffff;
	return( checkSum );
}

// ----------------------------------------------------------------------------
// ---------------------------- private functions -----------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CVoiceOverIPRoot::CHostAddedHandler::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
			{
	for( int i = 0; i < d_ref->m_numPeers; i++ )
				{
		if( d_ref->m_peerHosts[ i ] == ( const CHost * )i_data )
			return;
				}
	d_ref->m_peerHosts.Add( ( const CHost * )i_data );
/*
#ifdef PS2
			sceInsockInAddr_t ip;
#else
			in_addr ip;
#endif
	ip.s_addr = d_ref->m_peerHosts[ d_ref->m_numPeers ]->GetIPAddress( );
	d_ref->m_peerHostIP.Add( ip.s_addr );
			ip.s_addr = ntohl( ip.s_addr );
#ifndef _XBOX
			dbgPrint( "just added a new peer, IP = %s...\n", inet_ntoa( ip ) );
#endif
			*/
	d_ref->m_numPeers++;
		}

// ----------------------------------------------------------------------------
void CVoiceOverIPRoot::CHostRemovedHandler::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted)
		{
	for( int i = 0; i < d_ref->m_numPeers; i++ )
			{
		if( d_ref->m_peerHosts[ i ] == ( const CHost * )i_data )
				{
			dbgPrint( "just deleted peer, IP = %x...\n", d_ref->m_peerHosts[ i ]->GetIPAddress( ) );
			d_ref->m_peerHosts.Delete( i );
			d_ref->m_peerHostIP.Delete( i );
			d_ref->m_numPeers--;
			return;
			}
		}
}

// ----------------------------------------------------------------------------
bool CVoiceOverIPRoot::sendTeamChat( const char *text )
{
	u8 data[ 256 ];
	u32 hostLen = strlen( m_hostName );
	u32 len = strlen( text ) + 1;
	ASSERTS( ( len + VOIP_HEADER_SIZE ) < 256, "chat buffer overflow" );
	voipHeader *pPacket = ( voipHeader * )data;

	pPacket->checkSum			= 0;
	pPacket->dataSize			= (unsigned short)(hostLen + len);
	pPacket->packetNumber	= m_lastPacketSent;
	pPacket->codec				= VOIP_COMMAND_TEAM_CHAT;
	memcpy( data + VOIP_HEADER_SIZE, m_hostName, hostLen );
	memcpy( data + VOIP_HEADER_SIZE + hostLen, text, len );
	sendPacket( data, hostLen + len + VOIP_HEADER_SIZE );
	showChat( data + VOIP_HEADER_SIZE, hostLen + len );
	return( true );
}

// ----------------------------------------------------------------------------
bool CVoiceOverIPRoot::sendAllChat( const char *text )
{
	u8 data[ 256 ];
	u32 hostLen = strlen( m_hostName );
	u32 len = strlen( text ) + 1;
	ASSERTS( ( len + VOIP_HEADER_SIZE ) < 256, "chat buffer overflow" );
	voipHeader *pPacket = ( voipHeader * )data;

	pPacket->checkSum			= 0;
	pPacket->dataSize			= (unsigned short)(hostLen + len);
	pPacket->packetNumber	= m_lastPacketSent;
	pPacket->codec				= VOIP_COMMAND_ALL_CHAT;
	memcpy( data + VOIP_HEADER_SIZE, m_hostName, hostLen );
	memcpy( data + VOIP_HEADER_SIZE + hostLen, text, len );
	sendPacket( data, hostLen + len + VOIP_HEADER_SIZE );
	showChat( data + VOIP_HEADER_SIZE, hostLen + len );
	return( true );
}

// ----------------------------------------------------------------------------
// ---------------------------- private functions -----------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CVoiceOverIPRoot::updatePressToTalk( )
{
	ControllerInputDriver *c = g_input.GetController( 0 );

	if( c->ButtonDown( CVoiceChat::GetPushToTalk() ) )
	{	
		if( !isRecording( ) )
		{
			enableControl( CONTROL_RECORD, true );
			m_pCVoiceDevice->startRecording( );
		}
	}
	else
	{
		if( isRecording( ) )
		{
			sendTerminationPacket( );
			m_pCVoiceDevice->stopRecording( );
			enableControl( CONTROL_RECORD, false );
		}
	}
}

// ----------------------------------------------------------------------------
void CVoiceOverIPRoot::sendTerminationPacket( )
{
	voipHeader packet;
	packet.checkSum		= 0;
	packet.dataSize		= 0;
	packet.packetNumber	= m_lastPacketSent;
	packet.codec			= VOIP_COMMAND_TERMINATE;
	sendPacket( ( u8 * )&packet, VOIP_HEADER_SIZE );
}

// ---------------------------------------------------------------------------
u32 CVoiceOverIPRoot::sendPacket( u8 *pSrc, u32 size  )
{
	u32 ip;
	int result = 0;
	if( size <= 0 || m_numPeers <= 0 ) return( 0 );

	// --- loop through all the peers
	for( int i = 0; i < m_numPeers; i++ )
	{
		if( m_peerHosts[ i ] )
		{
			ip = m_peerHostIP[ i ];
			result = m_pCVoiceOverIP->sendPacket( pSrc, size, ip );
			if( result < 0 )
			{
				//printf( "error sending data...->%d\n", sceInsockErrno );
				break;
			}
		}
	}
	m_lastPacketSent++;
	return( result );
}

// ---------------------------------------------------------------------------
u32 CVoiceOverIPRoot::receivePacket( u8 *pDst )
{
	u32 ip;
	s32 size = 0;

	if( m_numPeers <= 0 ) return( 0 );

	// --- loop through all the peers
	for( int i = 0; i < m_numPeers; i++ )
	{
		if( m_peerHosts[ i ] )
		{
			ip = m_peerHostIP[ i ];
			size = m_pCVoiceOverIP->receivePacket( pDst, ip );

			// --- did we get a packet?
			if( size )
			{
				// --- decode the data
				size = decodePacket( pDst, size );
				if( size )
					return( size >= 0 ? size : 0 );
			}
		}
		return( size >= 0 ? size : 0 );
	}
	return( size >= 0 ? size : 0 );
}

// ----------------------------------------------------------------------------
s32 CVoiceOverIPRoot::decodePacket( u8 *pSrc, s32 size )
				{
	voipHeader *pVoipHeader = ( voipHeader * )pSrc;

	u16 command = pVoipHeader->codec & COMMAND_BITS;
	switch( command )
					{
		// --- 
		case VOIP_COMMAND_VOICE_DATA:
			break;

		case VOIP_COMMAND_TERMINATE:
						// --- force playback of any previous data
						m_bufferFilled = true;
						size = 0;
						dbgPrint( "Received termination packet...\n" );
						m_lastPacketReceived = pVoipHeader->packetNumber;
						m_terminationPending = false;
						if( m_bufferedDataSize )
						{
							m_terminationPending = true;
							dbgPrint( "set m_terminationPending...\n" );
						}
			break;

		case VOIP_COMMAND_TEAM_CHAT:
			showChat( pSrc + VOIP_HEADER_SIZE, pVoipHeader->dataSize );
			m_lastPacketReceived = pVoipHeader->packetNumber;
			size = 0;
			break;

		case VOIP_COMMAND_ALL_CHAT:
			showChat( pSrc + VOIP_HEADER_SIZE, pVoipHeader->dataSize );
			m_lastPacketReceived = pVoipHeader->packetNumber;
			size = 0;
			break;

		default:
						ASSERTS( false, "unknown command packet" );
					}
	return( size );
				}

// ----------------------------------------------------------------------------
bool CVoiceOverIPRoot::showChat( u8 *pSrc, s32 length )
{
	m_chatPending = true;
	memcpy( m_chatMessage, pSrc, length );
	return( true );
}

// ----------------------------------------------------------------------------
char *CVoiceOverIPRoot::ipToAscii( u32 ip )
{
	memset( m_ipStringBuffer, 0, sizeof( m_ipStringBuffer ) );
	return( m_ipStringBuffer );
}

// ----------------------------------------------------------------------------
static char *chatStrings[ 4 ] = { ":Owned!!!", ":Oh my u suck...", ":nice shot bizsnatch", ":lol..." };
void CVoiceOverIPRoot::updateState( )
{

#if defined( WIN32 )
	if( g_input.ButtonPressed( BC_F1 ) )
	{
		sendAllChat( chatStrings[ 0 ] );
	}
	if( g_input.ButtonPressed( BC_F2 ) )
	{
		sendAllChat( chatStrings[ 1 ] );
	}
	if( g_input.ButtonPressed( BC_F3 ) )
	{
		sendAllChat( chatStrings[ 2 ] );
	}
	if( g_input.ButtonPressed( BC_F4 ) )
	{
		sendAllChat( chatStrings[ 3 ] );
	}
#else
	ControllerInputDriver *c = g_input.GetController( 0 );
	if( c->ButtonDown( CVoiceChat::GetPushToTalk() ) )
	{
		sendAllChat( chatStrings[ rand( ) % 4 ] );
	}
#endif
	if( m_stateUpdateFrequency == 0 )
	{
		m_stateUpdateFrequency = STATE_UPDATE_FREQUENCY;
	}
	else
	{
		m_stateUpdateFrequency--;
	}
}
