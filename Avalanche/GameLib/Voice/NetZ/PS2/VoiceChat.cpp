// 
// (c) Copyright 1998-2003, Quazal Technologies Inc.
// ALL RIGHTS RESERVED
// 
// Net-Z, Eterna, Q-Platform, Duplicated Object and Duplication Space 
// are trademarks of Quazal.
// 
// Use of this software indicates acceptance of the End User licence
// Agreement for this product. A copy of the EULA is available from
// www.quazal.com.
//   
#define __VOICECHAT__C
#include "Voice/VoicePCH.h"

#include <netz.h>
#include <platform/stack/ps2common/ps2module.h>
#include <utilities/consoleio/consoleio.h>
#include <utilities/commandline/stdcmdline.h>
#include <utilities/sessionenumeration/consolesessionenumerator.h>
#include "Voice/NetZ/PS2/PS2LogitechDevices.h"
#include "Voice/NetZ/PS2/PS2LogitechSource.h"
#include "Voice/NetZ/PS2/PS2LogitechSink.h"
#include "OpenLPCCodec.h"
#include "GSMCodec.h"
#include <extensions/voicechat/voicechatext.h>
#include <libpad.h>
#include "Voice/NetZ/PS2/VoiceChatManager.h"
#include "Voice/NetZ/CVoiceChat.h"
#include "Voice/NetZ/PS2/NellyMoserCodec.h"
#include "cmdcon/cmdcon.h"
#include "EngineHelper/timer.h"
#include "Network/NetZ/NetZConnection.h"

// --- create our singleton
CVoiceChat g_CVoiceChat;
CHost* speakerHost = NULL;
u32 m_lastMS = 0;
DOHandle m_talkerDO = INVALID_DOHANDLE;
CSession *pSession=NULL;
ControllerButton CVoiceChat::s_pushToTalkButton = CB_STICK1; 

extern const char* g_RemoteIPAddress;

// --- Local vars -------------------------------------------------------------
#define MAX_SIMULTANEOUS_TALKERS_ALL 4
#define MAX_SIMULTANEOUS_TALKERS_TEAM1 1
#define MAX_SIMULTANEOUS_TALKERS_TEAM2 1

PS2LogitechSink *pStreamSink = NULL;
PS2LogitechSource *pMicrophone = NULL;
DOHandle m_talkersDO[ MAX_SIMULTANEOUS_TALKERS_ALL ];
qByte m_talkersStream[ MAX_SIMULTANEOUS_TALKERS_ALL ];
static u32 s_microphoneTestFrequency = 0;
#define CODEC CodecID(0)

// ----------------------------------------------------------------------------
// This class forwards the VoiceChannel generated events
// to the Dialog in order to update the window.
class MyDemuxEventHandler: public DemuxEventHandler 
{
public:
	MyDemuxEventHandler(VoiceChannel *pChannel) { m_pVoiceChannel = pChannel; }
	~MyDemuxEventHandler() {}

	virtual qBool StreamOpened(qByte idStream, DOHandle dohMember) 
	{ 
		if( dohMember != INVALID_DOHANDLE )
		{
			ConsoleIO::Print(_T("Stream %d opened by %x\n"),idStream,dohMember.GetValue());
			qBool result = DemuxEventHandler::StreamOpened(idStream, dohMember);
			m_talkerDO = dohMember;

			// --- map the speaker to a host object
			speakerHost = NULL;
			VoiceChannelMember* member = m_pVoiceChannel->FindMember( dohMember );
			if ( member )
			{
				ASSERT(dohMember.IsAKindOf(DOCLASSID(DOTalker)));
				DOTalker::Ref refTalker(dohMember);
				if (refTalker.IsValid())
				{
					g_CVoiceChat.setWhosTalking( dohMember );
					m_talkersDO[ idStream ] = dohMember;
					g_console.Echo( "Setting m_talkersDO[%d]\n", idStream );

					// --- set our start talk time
					if( g_CVoiceChat.getLocalTalker( ) == *refTalker )
						g_CVoiceChat.setTalkTimer( g_timer.GetOSTime( ) );
				}
				else
				{
					g_CVoiceChat.setWhosTalking( INVALID_DOHANDLE );
				}

				ConsoleIO::Print(_T("I am a member of this voice channel\n" ) );
				DOHandle dohStation = member->GetMemberStation();

				if ( dohStation == ((DOHost*)pSession->GetLocalHost())->GetMasterStation() )
				{
					speakerHost = pSession->GetLocalHost();
				}
				else
				{
					CHost* host;
					for ( host = (DOHost*) pSession->GetNextRemoteHost( NULL ) ;
							host ;
							host = (DOHost*) pSession->GetNextRemoteHost( host ) )
					{
						if ( dohStation == ((DOHost*)host)->GetMasterStation() )
						{
							speakerHost = host;
							break;
						} 
					}
				}
			}
			g_CVoiceChat.setListen( true );
			return( result );
		}
		return( false );
	}

	virtual qBool StreamClosed(qByte idStream, DOHandle dohMember) 
	{ 
		if( dohMember != INVALID_DOHANDLE )
		{
			ConsoleIO::Print(_T("Stream %d closed by %x\n"),idStream,dohMember.GetValue());
			qBool result = DemuxEventHandler::StreamClosed(idStream, dohMember);
			speakerHost = NULL;
			m_talkersDO[ idStream ] = INVALID_DOHANDLE;
			g_console.Echo( "Clearing m_talkersDO[%d]\n", idStream );
			g_CVoiceChat.setListen( false );
			g_CVoiceChat.setWhosTalking( INVALID_DOHANDLE );
			return( result );
		}
		return( false );
	}


	virtual qBool StreamPacketReceived( qByte idStream, DOHandle dohMember, qByte* pbyStreamPacket, qUnsignedInt uiPacketSize )
	{
		if( dohMember != INVALID_DOHANDLE )
		{
			ASSERTS( pStreamSink, "Fatal Voice Error, Must have a stream sink to write to" );
			if( pStreamSink )
			{
				g_console.Echo("Received incoming stream packet from %d on stream %d...\n", ( unsigned int )dohMember, idStream );
				if( g_CVoiceChat.isVoiceEnabled() )
				{
					// --- set this talker's DO Handle into our tracking array
					ASSERTS( idStream < MAX_SIMULTANEOUS_TALKERS_ALL, "stream index out of range" );
					m_talkersDO[ idStream ] = dohMember;

					pStreamSink->SetIncomingPacketStreamIndex( idStream );
					DemuxEventHandler::StreamPacketReceived( idStream, dohMember, pbyStreamPacket, uiPacketSize );
					g_console.Echo("Passing on stream packet from %d on stream %d...\n", ( unsigned int )dohMember, idStream );
				}
			}
		}
	}


	virtual void MembershipChanged() 
	{ 
		ConsoleIO::Print(_T("Membership Changed\n"));
		if (m_pVoiceChannel != NULL) {
			m_pVoiceChannel->TraceMembers(TRACE_ALWAYS);
			g_CVoiceChat.setMemberShipChanged( true );
			//g_CVoiceChat.updateMuting( );
		}
	}

	void SetChannel(VoiceChannel *pChannel) { m_pVoiceChannel = pChannel; }
	void SetChannelIndex( int i ) { m_channelIndex = i; }
	VoiceChannel *GetChannel( ) { return m_pVoiceChannel; }

public:
	VoiceChannel	*m_pVoiceChannel;
	int				m_channelIndex;
};
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
CVoiceChat::CVoiceChat( void ) :
	m_deviceExists( false ),
	m_enabled( false ),
	m_created( false ),
	m_listen( false ),
	m_memberShipChanged( false ),
	m_teamID( -1 ),
	m_voiceMaskEnable( true ),
	m_pitchWarp( 0.25f ),
	m_pitchOffset( 0.2f ),
	m_spectrumWarp( 0.25f ),
	m_spectrumOffset( 0.5005f ),
	m_spectrumLedge( 0.0f ),
	m_robot( 0.0f ),
	m_whisper( 0.0f ),
	m_environment( 0 ),
	m_talker( NULL ),
	m_chatState( STATE_CHAT_UNINITIALIZED ),
	m_talkingTalker( INVALID_DOHANDLE ),
	m_voiceRedirected( false ),
	m_headsetConnected( false ),
	m_volume( 1.0f ),
	m_voiceBanned( false ),
	m_userId( 0 )
{
	m_numStreams[ CHAT_CHANNEL_ALL ]		= MAX_SIMULTANEOUS_TALKERS_ALL;
	m_numStreams[ CHAT_CHANNEL_TEAM1 ]	= MAX_SIMULTANEOUS_TALKERS_TEAM1;
	m_numStreams[ CHAT_CHANNEL_TEAM2 ]	= MAX_SIMULTANEOUS_TALKERS_TEAM2;
	memset( m_chatChannels, 0, sizeof( m_chatChannels ) );
	memset( m_pCodecs, 0, sizeof( m_pCodecs ) );
	for( int i = 0; i < MAX_SIMULTANEOUS_TALKERS_ALL; i++ )
	{
		m_talkersDO[ i ] = INVALID_DOHANDLE;
	}
	m_voiceLastUsedTime = g_timer.GetOSTime( );
}

// ----------------------------------------------------------------------------
CVoiceChat::~CVoiceChat( void )
{
	for( int i = 0; i < MAX_CHAT_CHANNELS; i++ )
	{
		if( m_chatChannels[ i ].pChatManager )
		{
			delete ( VoiceChatManager * )m_chatChannels[ i ].pChatManager;
			m_chatChannels[ i ].pChatManager = NULL;
		}
		if( m_chatChannels[ i ].pEventHandler )
		{
			delete ( MyDemuxEventHandler * )m_chatChannels[ i ].pEventHandler;
			m_chatChannels[ i ].pEventHandler = NULL;
		}
	}

	for( int i = 0; i < CHAT_NUM_CODECS; i++ )
	{
		if( m_pCodecs[ i ] )
		{
			delete ( Codec * )m_pCodecs[ i ];
			m_pCodecs[ i ] = NULL;
		}
	}

	for( int i = 0; i < MAX_SIMULTANEOUS_TALKERS_ALL; i++ )
	{
		m_talkersDO[ i ] = INVALID_DOHANDLE;
	}
	m_chatState = STATE_CHAT_UNINITIALIZED;
}

// ----------------------------------------------------------------------------
const bool CVoiceChat::createCodecs( void )
{
	if( !m_pCodecs[ CHAT_CODEC_GSM ] )
	{
//		m_pCodecs[ CHAT_CODEC_GSM ] = ( void * )new GSMCodec( 100 );
//		ASSERTS( m_pCodecs[ CHAT_CODEC_GSM ], "Failed to create gsm codec" );
	}

	if( !m_pCodecs[ CHAT_CODEC_NELLYMOSER ] )
	{
		m_pCodecs[ CHAT_CODEC_NELLYMOSER ] = ( void * )new NellyMoserCodec( 100 );
		ASSERTS( m_pCodecs[ CHAT_CODEC_NELLYMOSER ], "Failed to create nellyMoser codec" );
	}

	if( !m_pCodecs[ CHAT_CODEC_OPENLPC ] )
	{
//		m_pCodecs[ CHAT_CODEC_OPENLPC ] = ( void * )new OpenLPCCodec( 1800 );
//		ASSERTS( m_pCodecs[ CHAT_CODEC_OPENLPC ], "Failed to create openLPC codec" );
//		state = ( ( Codec * )m_pCodecs[ CHAT_CODEC_OPENLPC ] )->CreateDecoderState( );
//		( ( Codec * )m_pCodecs[ CHAT_CODEC_OPENLPC ] )->ResetState( state );

	}
	m_pActiveCodec = m_pCodecs[ CHAT_CODEC_NELLYMOSER ];
	return( true );
}

// ----------------------------------------------------------------------------
bool CVoiceChat::createPlatformSpecific( void )
{
	return( false );
}

// ----------------------------------------------------------------------------
bool CVoiceChat::releasePlatformSpecific( void )
{
	return( false );
}

// ----------------------------------------------------------------------------
const bool CVoiceChat::create( DOTalker* talker, CSession &session )
{
	s_microphoneTestFrequency = 0;

	if( m_created ) return( true );

	// --- must have a talker
	if( talker == NULL )
	{
		return( false );
	}

	m_talker = talker;
	pSession = &session;

	m_created = true;
	m_teamID = -1;
	m_chatState = STATE_CHAT_UNINITIALIZED;
	m_userId = m_talker->d_props.d_id;

	// --- create the codecs
	createCodecs( );

	if( !m_deviceExists )
	{
		// --- Initialize the headset (ignore fail on atwinmon decks, we're good).
		if( PS2LogitechDevices::Init() || ( g_RemoteIPAddress != NULL ) )
		{
			CodecManager::GetInstance()->RegisterCodec( CODEC, ( Codec * )m_pActiveCodec );
			m_deviceExists = true;
		}
		else
		{
		}
	}

	// --- install message handlers for muting support
	m_RequestMutingHandler.Initialize( REQUEST_MUTING,	MESSAGE_HANDLE_IF_MASTER | MESSAGE_ROUTE_TO_MASTER ,m_messageOwner, this );
	m_MuteHandler.Initialize( MUTE,	MESSAGE_HANDLE_IF_MASTER |	MESSAGE_ROUTE_TO_MASTER, m_messageOwner, this );
	m_UnMuteHandler.Initialize( UNMUTE, MESSAGE_HANDLE_IF_MASTER |MESSAGE_ROUTE_TO_MASTER, m_messageOwner, this );
	m_RemoteMuteHandler.Initialize( REMOTE_MUTE,	MESSAGE_HANDLE_IF_MASTER |	MESSAGE_ROUTE_TO_MASTER, m_messageOwner, this );
	m_RemoteUnMuteHandler.Initialize( REMOTE_UNMUTE, MESSAGE_HANDLE_IF_MASTER |MESSAGE_ROUTE_TO_MASTER, m_messageOwner, this );

	// --- create all the voice chat channels we need
	for( int i = 0; i < MAX_CHAT_CHANNELS; i++ )
	{
		// --- create the manager
		m_chatChannels[ i ].pChatManager = ( void * ) new VoiceChatManager;
		ASSERTS( m_chatChannels[ i ].pChatManager, "unable to create voice chat manager" );

		// --- create the event handler
		m_chatChannels[ i ].pEventHandler	= ( void * ) new MyDemuxEventHandler( NULL );
		ASSERTS( m_chatChannels[ i ].pEventHandler, "unable to create voice chat event handler" );

		// --- initialize the codec
		( ( VoiceChatManager * )m_chatChannels[ i ].pChatManager )->SetCodecID( CODEC );

		// --- initialize the channel
		( ( VoiceChatManager * )m_chatChannels[ i ].pChatManager )->InitializeChannel( _T("TTL Voice Channel" ), m_numStreams[ i ], ( MyDemuxEventHandler * )m_chatChannels[ i ].pEventHandler );

		m_chatChannels[ i ].queuedDisable = false;
		m_chatChannels[ i ].queuedEnable = false;
		m_chatChannels[ i ].openPending = false;
	}

	// --- make sure these don't exist yet
	if( pStreamSink )
	{
		delete pStreamSink;
		pStreamSink = NULL;
	}
	if( pMicrophone )
	{
		delete pMicrophone;
		pMicrophone = NULL;
	}
	m_voiceLastUsedTime = g_timer.GetOSTime();
	return true;
}

// ----------------------------------------------------------------------------
const bool CVoiceChat::update( )
{
	// --- make sure we've been created
	if( !m_created ) return( false );

	bool exitEarly = false;
	for( int i = 0; i < MAX_CHAT_CHANNELS; i++ )
	{
		VoiceChatManager *pMgr = ( VoiceChatManager * )m_chatChannels[ i ].pChatManager;
		if( pMgr )
		{
			if( m_chatChannels[ i ].openPending && m_chatChannels[ i ].queuedDisable )
			{
				exitEarly = true;
				g_console.Echo( "Open was pending....\n" );
				if( pMgr->IsTalking() )
				{
					g_console.Echo( "Open finally occured...\n" );
					pMgr->StopTalking();
					m_chatChannels[ i ].openPending = false;
				}
			}
			else
			{
				// --- when we tried to disable this channel it had a pending open, so we must wait to disable it
				if( m_chatChannels[ i ].queuedDisable )
				{
					enableChatChannel( ( eChatChannels )i, false );
				}

				// --- we queued an open so now enable the channel
				if( m_chatChannels[ i ].queuedEnable )
				{
					enableChatChannel( ( eChatChannels ) i, true );
				}
			}
			pMgr->Process( );
		}
	}


	// --- try and discover voice channels to join
	if( !allJoined( ) ) 
	{
		bool result = false;

		for( int i = 0; i < MAX_CHAT_CHANNELS; i++ )
		{
			if( !m_chatChannels[ i ].discovered && m_chatChannels[ i ].enabled )
			{
				g_console.Echo( "Trying to discover channel #%d\n", i );
				ASSERTS( m_chatChannels[ i ].pChatManager, "chat manager has not been created yet" );
				MyDemuxEventHandler *pEvt	= ( MyDemuxEventHandler * )m_chatChannels[ i ].pEventHandler;
				VoiceChatManager *pMgr		= ( VoiceChatManager * )m_chatChannels[ i ].pChatManager;
				VoiceChannel *pChannel = pEvt->GetChannel( );
				bool alreadyMember = ( pChannel ? pChannel->IsAMember( m_talker->GetHandle() ) : false );

				if( alreadyMember )
					g_console.Echo( "Still a member of voice channel #%d...\n", i );

				// --- we cannot rejoin this channel as long as we are already a member of it
				if( !alreadyMember )
				{
					m_chatChannels[ i ].discovered = pMgr->discoverChannel( i );
			
					// --- see if we discovered a channel
					if( m_chatChannels[ i ].discovered )
					{
						g_console.Echo( "Discovered channel #%d\n", i );

						// --- create the microphone and sink if we need to
						if( pMicrophone == NULL )
						{
							pStreamSink = new PS2LogitechSink( ( ( Codec * )m_pActiveCodec )->GetFormat( ) );
							pStreamSink->createSinkBuffers( MAX_SIMULTANEOUS_TALKERS_ALL, ( ( Codec * )m_pActiveCodec )->GetNbBytesPerEncodedFrame( ) );

							// --- fill an extra buffer with encoded blank space
							if( m_pActiveCodec && m_pActiveCodec == m_pCodecs[ CHAT_CODEC_NELLYMOSER ] && m_pCodecs[ CHAT_CODEC_NELLYMOSER ] )
							{
								( ( NellyMoserCodec * )m_pActiveCodec )->Encode( NULL, pStreamSink->GetPCMBuffer( ), pStreamSink->GetExtraEncodeBuffer( 0 ) );
								( ( NellyMoserCodec * )m_pActiveCodec )->Encode( NULL, pStreamSink->GetPCMBuffer( ), pStreamSink->GetExtraEncodeBuffer( 1 ) );
							}

							if( pStreamSink->GetDeviceHandle( ) != LGAUD_INVALID_DEVICE )
							{
								lgAudSetPlaybackVolume( pStreamSink->GetDeviceHandle( ), LGAUD_CH_BOTH, (unsigned char)( m_volume * 100.0f ) );
							}
							pMicrophone = new PS2LogitechSource( ( ( Codec * )m_pActiveCodec )->GetFormat( ) );
						}
						
						// --- save away the voice channel in the event handler
						pEvt->SetChannel( pMgr->GetChannel( ) );
						pEvt->SetChannelIndex( i );

						ASSERTS( pStreamSink, "unable to create stream sink" );
						for( int j = 0; j < m_numStreams[ i ]; j++ ) 
						{
							( ( DemuxEventHandler * )pMgr->GetEventHandler( ) )->SetStreamSink( j, pStreamSink );
						}

						// --- Initialize the Talker.  This will join the VoiceChannel
						ASSERTS( pMicrophone, "unable to create microphone" );
						g_console.Echo( "Joining voice channel #%d\n", i );
						result = pMgr->InitializeTalker( m_talker->GetHandle(), pMicrophone );
						ASSERTS( result, "unable to initialize all talk voice channel" );
					}
				}
			}
		}
	}

	if( exitEarly ) return false;

	if( m_talker->d_props.d_voiceToSpeakers != m_voiceRedirected )
	{
		m_talker->d_props.d_voiceToSpeakers = m_voiceRedirected;
		m_talker->d_props.RequestUpdate();
	}

	if( !isVoiceEnabled( ) )
	{
		// --- if voice is disable make it look like they have no headset
		m_headsetConnected = false;
		if( m_talker && m_talker->d_props.d_headset != m_headsetConnected )
		{
			m_talker->d_props.d_headset = m_headsetConnected;
			m_talker->d_props.RequestUpdate();
		}
		return( false );
	}

	// --- update headset status
	if( pMicrophone && g_timer.GetOSTime() > s_microphoneTestFrequency && !pMicrophone->IsOpen() )
	{
		pMicrophone->Open();
		if( pMicrophone->IsOpen() )
			pMicrophone->Close();
		s_microphoneTestFrequency = g_timer.GetOSTime() + 3000;
	}
	m_headsetConnected = ( pMicrophone && ( pMicrophone->GetDeviceHandle() != LGAUD_INVALID_DEVICE ) );
	if( m_talker->d_props.d_headset != m_headsetConnected )
	{
		m_talker->d_props.d_headset = m_headsetConnected;
		m_talker->d_props.RequestUpdate();
	}

	// --- check for press to talk and try to acquire the voice channel
	ControllerInputDriver *c = g_input.GetController( 0 );

	// --- now lets process the chat channels
	bool chatting = false;
	for( int i = 0; i < MAX_CHAT_CHANNELS; i++ )
	{
		if( isChatChannelEnabled( i ) )
		{
			VoiceChatManager *pMgr = ( VoiceChatManager * )m_chatChannels[ i ].pChatManager;

			if( pMgr && pMgr->GetMemberSink() && pMgr->GetMemberSink()->JoinCompleted() )
			{
				if( c && ( m_talker->GetHandle() != INVALID_DOHANDLE ) )
				{
					DOTalker::Ref ref( m_talker );

					if( ref.IsValid( ) && ( ref->IsAlive( ) == m_talker->IsAlive() ) &&
						c->ButtonDown( s_pushToTalkButton ) && !m_memberShipChanged &&
						pMicrophone && ( pMicrophone->GetDeviceHandle() != LGAUD_INVALID_DEVICE ) &&
						( ( ( g_timer.GetOSTime() - m_voiceLastUsedTime ) >= MIN_CHAT_WAIT ) || pMgr->IsTalking() ) )
					{
						m_chatChannels[ i ].tryingToChat = true;
						if( !pMgr->IsTalking( ) )
						{
							pMgr->StartTalking( );
							m_chatChannels[ i ].openPending = true;
							//g_console.Echo( "-----Requesting to talk, set openPending to true" );
						}
						else
						{
							//g_console.Echo( "-----Already talking, set openPending to false" );
							m_chatChannels[ i ].openPending = false;
						}
					}
					else
					{
						m_chatChannels[ i ].tryingToChat = false;
						if( pMgr->IsTalking( ) )
						{
							pMgr->StopTalking( );
							m_voiceLastUsedTime = g_timer.GetOSTime( );
							//g_console.Echo( "-----Requesting to stop talking, set openPending to false" );
							m_chatChannels[ i ].openPending = false;
						}
						m_memberShipChanged = false;
					}
				}

				// --- process the channel
				pMgr->Process( );

				if( true )// pStreamSink && ( pStreamSink->GetDeviceHandle() != LGAUD_INVALID_DEVICE ) )
				{
					// --- we now need to decode and forcefully right to the sink, since I'm mixing multiple streams
					if( m_pActiveCodec && m_pActiveCodec == m_pCodecs[ CHAT_CODEC_NELLYMOSER ] )
					{
						NellyMoserCodec* pCodec = ( NellyMoserCodec * )m_pActiveCodec;

						// -- see if we can flush the streams yet
						u32 curMS = g_timer.GetOSTime();

						// --- wait for the correct number of millisecs before we flush all streams that have accumulated
						if( ( curMS - m_lastMS ) >= ( pCodec->GetNumFramesPerPacket( ) * 17 ) || !m_chatChannels[ CHAT_CHANNEL_ALL ].enabled )
						{
							m_lastMS = curMS;
							if( pStreamSink )
							{
								while( pStreamSink->HaveDataToDecodeOnStreams( m_numStreams[ i ] ) )
								{
									int curNumStreams = 0;
									bool haveStream[ MAX_SIMULTANEOUS_TALKERS_ALL ] = { false, false, false, false };
									int lastStream = 0;
/*									updateVoiceMasking( 0, 0.25f, 0.2f, 0.25f, 0.5005f, 0.0f, 0.0f, 0.0f, 0 );
									updateVoiceMasking( 1, 0.25f, 0.2f, 0.25f, 0.5005f, 0.0f, 0.0f, 0.0f, 0 );
									updateVoiceMasking( 2, 0.25f, 0.2f, 0.25f, 0.5005f, 0.0f, 0.0f, 0.0f, 0 );
									updateVoiceMasking( 3, 0.25f, 0.2f, 0.25f, 0.5005f, 0.0f, 0.0f, 0.0f, 0 );*/

									// --- we have voice data from someone, so update their voice mask accordingly
									for( int streamIndex = 0; streamIndex < m_numStreams[ i ]; streamIndex++ )
									{
										m_talkerDO = m_talkersDO[ streamIndex ];
										if( m_talkerDO != INVALID_DOHANDLE )
										{
											DOTalker::Ref ref( m_talkerDO );
											if( ref.IsValid( ) && ( ( ref->IsAlive( ) == m_talker->IsAlive() || i == CHAT_CHANNEL_ALL ) ) && ( *ref != m_talker ) && pStreamSink->HaveDataToDecode( streamIndex ) )
											{
/*												updateVoiceMasking(	streamIndex, 
																			ref->d_props.d_pitchWarp, 
																			ref->d_props.d_pitchOffset, 
																			ref->d_props.d_spectrumWarp, 
																			ref->d_props.d_spectrumOffset, 
																			ref->d_props.d_spectrumLedge, 
																			ref->d_props.d_robot, 
																			ref->d_props.d_whisper, 
																			ref->d_props.d_environ );*/
												haveStream[ streamIndex ] = true;
												lastStream = streamIndex;
												curNumStreams++;
											}
											else
											{
												pStreamSink->flushStream( streamIndex );
											}
										}
										else
										{
											pStreamSink->flushStream( streamIndex );
										}
									}
									// --- did we have streams to decode
									if( curNumStreams )
									{
										if( curNumStreams == 1 )
										{
											pCodec->DecodeSingleStream( pStreamSink->GetSinkBuffer( lastStream ), pStreamSink->GetPCMBuffer( ) );
											g_console.Echo("Decoding a single stream on stream %d\n", lastStream );
										}
										else
										{
											qByte *pSink1 = ( haveStream[ 0 ] ? pStreamSink->GetSinkBuffer( 0 ) : pStreamSink->GetExtraEncodeBuffer( 0 ) );
											qByte *pSink2 = ( haveStream[ 1 ] ? pStreamSink->GetSinkBuffer( 1 ) : pStreamSink->GetExtraEncodeBuffer( 0 ) );
											qByte *pSink3 = ( haveStream[ 2 ] ? pStreamSink->GetSinkBuffer( 2 ) : pStreamSink->GetExtraEncodeBuffer( 0 ) );
											qByte *pSink4 = ( haveStream[ 3 ] ? pStreamSink->GetSinkBuffer( 3 ) : pStreamSink->GetExtraEncodeBuffer( 0 ) );
											pCodec->DecodeMultipleStreams( pSink1, pSink2, pSink3, pSink4, pStreamSink->GetPCMBuffer( ) );
											g_console.Echo( "Decoding multiple streams, 0=%d, 1=%d, 2=%d, 3=%d\n", haveStream[ 0 ], haveStream[ 1 ], haveStream[ 2 ], haveStream[ 3 ] );
										}
										pStreamSink->WriteFinal( );
									}
								}
							}
						}
					}
					else
					{
						if( pStreamSink && pStreamSink->HaveDataToDecode( ) )
						{
							OpenLPCCodec *pCodec = ( OpenLPCCodec * )m_pActiveCodec;
							pCodec->Decode( NULL, pStreamSink->GetSinkBuffer( 0 ), pStreamSink->GetPCMBuffer( ) );
							pStreamSink->WriteFinal( );
						}
					}
				}
				// --- did we aquire the channel?
				m_chatChannels[ i ].chatting = pMgr->IsTalking( );

				// --- update messages
				if( m_chatChannels[ i ].chatting )
				{
					if( m_chatState != STATE_CHAT_TALKING )
					{
						g_messageDispatcher.SendMessageToAll("ChatTalk", NULL, INVALID_OBJECT_HANDLE );
						m_chatState = STATE_CHAT_TALKING;
					}

					// --- see if we need to make this user stop chatting
					if( ( g_timer.GetOSTime() - m_voiceLastUsedTime ) >= MAX_CHAT_TIME )
					{
						m_voiceLastUsedTime = g_timer.GetOSTime( );
						if( pMgr->IsTalking() )
						{
							pMgr->StopTalking();
							m_chatChannels[ i ].openPending = false;
						}
					}
					chatting = true;
				}
				else if( m_chatChannels[ i ].tryingToChat && !chatting )
				{
					if( m_chatState != STATE_CHAT_DENY )
					{
						g_messageDispatcher.SendMessageToAll("ChatDeny", NULL, INVALID_OBJECT_HANDLE );
						m_chatState = STATE_CHAT_DENY;
					}
				}
				else
				{
					if( m_listen && !chatting )
					{
						if( m_chatState != STATE_CHAT_LISTEN )
						{
							g_messageDispatcher.SendMessageToAll("ChatListen", NULL, INVALID_OBJECT_HANDLE );
							m_chatState = STATE_CHAT_LISTEN;
						}
					}
					else
					{
						if( m_chatState != STATE_CHAT_NONE )
						{
							g_messageDispatcher.SendMessageToAll("ChatNone", NULL, INVALID_OBJECT_HANDLE );
							m_chatState = STATE_CHAT_NONE;
						}
					}
				}
			}
		}
	}
}


// ----------------------------------------------------------------------------
const void CVoiceChat::tearDown( void )
{
	//TraceLog::GetInstance()->SetFlag(TRACE_ALWAYS | TRACE_CONNECTION | TRACE_OPERATION);

	g_console.Echo( "CVoiceChat::tearDown called...\n" );
	// --- make sure we've been created
	if( !m_created ) return;

	m_created = false;

	m_messageOwner.UnregisterAll();

	// --- make sure we stop talking just in case
	for( int i = 0; i < MAX_CHAT_CHANNELS; i++ )
	{
		VoiceChatManager *pMgr = ( VoiceChatManager * )m_chatChannels[ i ].pChatManager;

		if( pMgr )
		{
			if( pMgr->IsTalking( ) ) pMgr->StopTalking( );
			pMgr->Process( );

			// --- tear down the managers
			pMgr->Teardown( );
		}

		// --- update the status of the channel
		m_chatChannels[ i ].enabled				= false;
		m_chatChannels[ i ].discovered			= false;
		m_chatChannels[ i ].membershipChanged	= true;
		m_chatChannels[ i ].chatting				= false;
		m_chatChannels[ i ].listening				= false;
		m_chatChannels[ i ].tryingToChat			= false;
		m_chatChannels[ i ].queuedDisable		= false;
		m_chatChannels[ i ].queuedEnable			= false;
		m_chatChannels[ i ].openPending			= false;

		if( m_chatChannels[ i ].pChatManager )
		{
			delete ( VoiceChatManager *)m_chatChannels[ i ].pChatManager;
			m_chatChannels[ i ].pChatManager = NULL;
		}

		if( m_chatChannels[ i ].pEventHandler )
		{
			delete ( MyDemuxEventHandler *)m_chatChannels[ i ].pEventHandler;
			m_chatChannels[ i ].pEventHandler = NULL;
		}
	}

	for( int i = 0; i < MAX_SIMULTANEOUS_TALKERS_ALL; i++ )
		m_talkersDO[ i ] = INVALID_DOHANDLE;
	setWhosTalking( INVALID_DOHANDLE );

	// --- clean up any memory we used
	if( pStreamSink )
	{
		delete pStreamSink;
		pStreamSink = NULL;
	}
	if( pMicrophone )
	{
		delete pMicrophone;
		pMicrophone = NULL;
	}

#if 0
	if( m_deviceExists )
	{
		CodecManager::GetInstance()->UnregisterCodec( CODEC );
		m_deviceExists = false;
	}
	// --- tear down the codecs also
	for( int i = 0; i < CHAT_NUM_CODECS; i++ )
	{
		if( m_pCodecs[ i ] )
		{
			delete ( Codec * )m_pCodecs[ i ];
			m_pCodecs[ i ] = NULL;
		}
	}
#endif
	m_voiceLastUsedTime = g_timer.GetOSTime( );
	m_chatState = STATE_CHAT_UNINITIALIZED;
	m_talker = NULL;

}

void CVoiceChat::postNetZTearDown( void )
{
	if( m_deviceExists )
	{
		CodecManager::GetInstance()->UnregisterCodec( CODEC );
		m_deviceExists = false;

		// --- tear down the codecs also
		for( int i = 0; i < CHAT_NUM_CODECS; i++ )
		{
			if( m_pCodecs[ i ] )
			{
				delete ( Codec * )m_pCodecs[ i ];
				m_pCodecs[ i ] = NULL;
			}
		}
	}
}

// ----------------------------------------------------------------------------
CHost* CVoiceChat::GetSpeakerHost()
{
	return speakerHost;
}

// ----------------------------------------------------------------------------
void CVoiceChat::enableChatChannel( eChatChannels channel, bool trueOrFalse )
{
	// --- make sure we've been created
	if( !m_created ) return;

	if( channel > 0 ) m_teamID = channel;

	if( channel == 0 && trueOrFalse)
	{
		g_console.Echo("Enabling All Chat Channel\n" );
	}
	VoiceChatManager *pMgr = ( VoiceChatManager * )m_chatChannels[ channel ].pChatManager;

	// --- channel opened before we got here and we had an openPending
	if( m_chatChannels[ channel ].openPending && pMgr && pMgr->IsTalking() )
	{
		g_console.Echo( "Channel %d opened, while we had a pendingOpen\n" );
		m_chatChannels[ channel ].openPending = false;
	}

	// --- we cannot disable this channel yet if someone has requested to open the channel, but did not acquire it
	// --- since a pending open will happen latter and the channel will never get closed
	if( m_chatChannels[ channel ].openPending && !trueOrFalse )
	{
		g_console.Echo("Disable is blocked on channel %d, will try next cycle\n", channel );
		m_chatChannels[ channel ].queuedDisable = true;
		return;
	}

	// --- we also cannot enable this channel if we have queued up a disable since the disable will undo our enable later
	if( m_chatChannels[ channel ].queuedDisable && trueOrFalse )
	{
		g_console.Echo("Enable is blocked on channel %d, will try next cycle\n", channel );
		m_chatChannels[ channel ].queuedEnable = true;
		return;
	}

	// --- temp
	g_console.Echo("Performing enableChatChannel, enable=%d, channel=%d\n", trueOrFalse, channel );

	// --- we requesting a state change on the channel, so make sure we stop talking
	if( m_chatChannels[ channel ].enabled != trueOrFalse )
	{
		if( pMgr )
		{
			if( pMgr->IsTalking( ) )
			{
				pMgr->StopTalking( );
				m_chatChannels[ channel ].openPending = false;
			}

			pMgr->Process( );

			// --- if we are disabling the channel unjoin it
			if( !trueOrFalse )
			{
				pMgr->unJoin( );
				m_chatChannels[ channel ].discovered = false;
			}

		}
	}
	m_chatChannels[ channel ].enabled = trueOrFalse;

#ifndef CDROM
	if( m_chatChannels[ channel ].queuedDisable )
		g_console.Echo( "Disable coming from a queued disable on channel %d\n", channel );
#endif

	m_chatChannels[ channel ].listening = false;
	m_chatChannels[ channel ].tryingToChat = false;
	m_chatChannels[ channel ].chatting = false;
	if( !trueOrFalse )
	{
		m_chatChannels[ channel ].queuedDisable = false;
		for( int i = 0; i < MAX_SIMULTANEOUS_TALKERS_ALL; i++ )
			m_talkersDO[ i ] = INVALID_DOHANDLE;
		setWhosTalking( INVALID_DOHANDLE );
		g_console.Echo( "Clearing m_talkersDO array\n"  );
	}
	if( trueOrFalse )
		m_chatChannels[ channel ].queuedEnable = false;

	// --- back to the front end reset our pushToTalk button
	if( channel == CHAT_CHANNEL_ALL && trueOrFalse )
		s_pushToTalkButton = CB_STICK1;
}


// ----------------------------------------------------------------------------
const void CVoiceChat::enable( bool trueOrFalse )
{
	if( !trueOrFalse )
	{
		for( int i = 0; i < MAX_CHAT_CHANNELS; i++ )
		{
			VoiceChatManager *pMgr = ( VoiceChatManager * )m_chatChannels[ i ].pChatManager;
			if( pMgr )
			{
				if( pMgr->IsTalking( ) )
				{
					pMgr->StopTalking( );
					m_chatChannels[ i ].openPending = false;
				}

				pMgr->Process( );
			}
		}
	}
	m_enabled = trueOrFalse;
}

// ----------------------------------------------------------------------------
const bool CVoiceChat::isPlayerTalking( DOTalker *talker )
{
	int maxSlots = 0;
	for( int i = 0; i < MAX_CHAT_CHANNELS; i++ )
	{
		if( m_chatChannels[ i ].enabled )
		{
			maxSlots = m_numStreams[ i ];
		}
	}

	for( int i = 0; i < maxSlots; i++ )
	{
		if( m_talkersDO[ i ] != INVALID_DOHANDLE )
		{
			ASSERT( m_talkersDO[ i ].IsAKindOf( DOCLASSID( DOTalker ) ) );
			DOTalker::Ref refTalker( m_talkersDO[ i ] );
			if( refTalker.IsValid() )
			{
				if( *refTalker == talker ) return( true );
			}
		}
	}
	return( false );
}

// ----------------------------------------------------------------------------
void CVoiceChat::setOutputVolume( float volume )
{
	m_volume = volume;
	if( pStreamSink )
	{
		pStreamSink->SetVolume( m_volume );
	}
}

// ----------------------------------------------------------------------------
// pitch warp			0.5 -> 2.5
// pitch offset		-75 -> 300
// spectrum warp		0.5 -> 2.5
// spectrum offset	-1000 -> 1000
// spectrum ledge		0 -> 1000
// robot					0 -> 1
// whisper				0 -> 1
//
#define PITCH_WARP(a)		( 0.5f + ( a * 2.0f ) )
#define PITCH_OFFSET(a)		( -75.0f + ( a * 375.0f ) )
#define SPECTRUM_WARP(a)	( 0.5f + ( a * 2.0f ) )
#define SPECTRUM_OFFSET(a)	( -1000.0f + ( a * 2000.0f ) )
#define SPECTRUM_LEDGE(a)	( a * 1000.0f )
#define ROBOT(a)				( a )
#define WHISPER(a)			( a )

const void CVoiceChat::enableVoiceMasking( bool trueOrFalse, int maskID /* = 0  */ )
{
	if( !trueOrFalse )
	{
		m_pitchWarp = 0.25f;
		m_pitchOffset = 0.2f;
		m_spectrumWarp = 0.25f;
		m_spectrumOffset = 0.5005f;
		m_spectrumLedge = 0.0f;
		m_robot = 0.0f;
		m_whisper = 0.0f;
		m_environment = 0;
	}

	// --- make sure we can do voice masking
	if( ( m_voiceMaskEnable != trueOrFalse ) && m_pCodecs[ CHAT_CODEC_NELLYMOSER ] )
	{
		NellyMoserCodec *pCodec = ( NellyMoserCodec * )m_pCodecs[ CHAT_CODEC_NELLYMOSER ];
		ASSERT( pCodec );
		pCodec->SetVoiceMaskEffect( 0, PITCH_WARP(0.25f), PITCH_OFFSET(0.2f), SPECTRUM_WARP(0.25f), 
												SPECTRUM_OFFSET(0.5005f), SPECTRUM_LEDGE(0.0f), ROBOT(0.0f), WHISPER(0.0f), 0 );
	}
	m_voiceMaskEnable = trueOrFalse;
}

// ----------------------------------------------------------------------------
const void CVoiceChat::updateVoiceMasking( int streamIndex, float a, float b, float c, float d, float e, float f, float g, int env )
{
	if( m_pCodecs[ CHAT_CODEC_NELLYMOSER ] )
	{
		NellyMoserCodec *pCodec = ( NellyMoserCodec * )m_pCodecs[ CHAT_CODEC_NELLYMOSER ];
		ASSERT( pCodec );
		pCodec->SetVoiceMaskEffect( streamIndex, PITCH_WARP(a), PITCH_OFFSET(b), SPECTRUM_WARP(c), 
			SPECTRUM_OFFSET(d), SPECTRUM_LEDGE(e), ROBOT(f), WHISPER(g), env );
	}
}

// ----------------------------------------------------------------------------
void CVoiceChat::refreshPlatformData( )
{
}

// ----------------------------------------------------------------------------
void CVoiceChat::channelVoiceThruSpeakers( bool trueOrFalse )
{ 
	m_voiceRedirected = trueOrFalse;

	// --- we have changed the voice redirection, so lets get and set our headset status
	//m_headSetStatus = ControllerInputDriver::UNINITIALIZED;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ------------------------- Non Networking Methods ---------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
const bool CVoiceChat::openHeadSet( void )
{
	// --- if we've been created then we must be in the lobby and don't need to do
	// any of this

	// --- make sure voice chat is disabled while we do this
	enable( false );

	// -- we are changing these in the lobby, so don't recreate
	if( m_created )
	{
		for( int i = 0; i < MAX_CHAT_CHANNELS; i++ )
		{
			VoiceChatManager *pMgr = ( VoiceChatManager * )m_chatChannels[ i ].pChatManager;

			if( pMgr )
			{
				if( pMgr->IsTalking( ) ) 
				{
					pMgr->StopTalking( );
					m_chatChannels[ i ].openPending = false;
				}
				pMgr->Process( );
			}
		}
		return true;
	}

	ASSERTS( pMicrophone == NULL, "We should never have a valid microphone yet" );
	ASSERTS( pStreamSink == NULL, "We should never have a valid microphone yet" );
	ASSERTS( m_pActiveCodec == NULL, "We should never have a valid codec yet" );

	// --- create the codec we will be using
	m_pActiveCodec = ( void * ) new NellyMoserCodec( 100 );
	ASSERTS( m_pActiveCodec, "Failed to create codec" );
	m_pCodecs[ CHAT_CODEC_NELLYMOSER ] = m_pActiveCodec;

	// --- initialize the device
	CNetZConnection::InitPaths();
	sceSifInitRpc( 0 );
	if( PS2LogitechDevices::Init() || ( g_RemoteIPAddress != NULL ) )
	{
		CodecManager::GetInstance()->RegisterCodec( CODEC, ( Codec * )m_pActiveCodec );
	}

	// --- create our headset device
	pStreamSink = new PS2LogitechSink( ( ( Codec * )m_pActiveCodec )->GetFormat( ) );
	ASSERTS( pStreamSink, "Failed to create headset sink" );
	pStreamSink->createSinkBuffers( 1, ( ( Codec * )m_pActiveCodec )->GetNbBytesPerEncodedFrame( ) );
	pMicrophone = new PS2LogitechSource( ( ( Codec * )m_pActiveCodec )->GetFormat( ) );
	ASSERTS( pMicrophone, "Failed to create headset source" );
	if( pStreamSink->GetDeviceHandle( ) != LGAUD_INVALID_DEVICE )
	{
		lgAudSetPlaybackVolume( pStreamSink->GetDeviceHandle( ), LGAUD_CH_BOTH, 90 );
	}

	enableVoiceMasking( true );
	return( pMicrophone != NULL && pStreamSink != NULL && m_pActiveCodec != NULL );
}

// ----------------------------------------------------------------------------
const bool CVoiceChat::updateHeadSet( void )
{
	static StreamingDevice::Status status = StreamingDevice::Success;
	NellyMoserCodec *pCodec = ( NellyMoserCodec * )m_pActiveCodec;
	ASSERT( pCodec );

	updateVoiceMasking( 0, m_pitchWarp, m_pitchOffset, m_spectrumWarp, m_spectrumOffset, m_spectrumLedge, m_robot, m_whisper, m_environment );

	// --- make sure we are using Nellymoser
	if( m_pActiveCodec != m_pCodecs[ CHAT_CODEC_NELLYMOSER ] ) return( false );

	// --- make sure the headset is valid
	if( pMicrophone )
	{
		// --- open the source device if its not open
		if( !pMicrophone->IsOpen( ) )
		{
			if( !pMicrophone->Open( ) ) 
			{
				return false;
			}
		}

		// --- open the sink device if its not open
		if( pStreamSink )
		{
			if( !pStreamSink->IsOpen( ) )
			{
				if( !pStreamSink->Open( ) )
				{
					return( false );
				}
			}
		}

		// --- read from the device
		status = pMicrophone->Read( ( qByte * )pStreamSink->GetPCMBuffer( ), 0 );

		// --- handle the result
		switch( status )
		{
			case StreamingDevice::Success:
				pCodec->Encode( NULL, pStreamSink->GetPCMBuffer( ), pStreamSink->GetSinkBuffer( 0 ) );
				pCodec->DecodeSingleStream( pStreamSink->GetSinkBuffer( 0 ), pStreamSink->GetPCMBuffer( ) );
				pStreamSink->WriteFinal( );
				break;

			case StreamingDevice::Error:
				pMicrophone->Close( );
				pStreamSink->Close( );
				ASSERT( false );
				break;

			case StreamingDevice::NotReady:
				break;

			default:
				g_console.Echo("Unknown Streaming error->%d\n", status );
				ASSERT( false );
				break;
		}
	}

	return true;
}

// ----------------------------------------------------------------------------
const bool CVoiceChat::closeHeadSet( void )
{
	ASSERTS( pMicrophone != NULL, "We should have a valid microphone" );
	ASSERTS( pStreamSink != NULL, "We should have a valid microphone" );
	ASSERTS( m_pActiveCodec != NULL, "We should have a valid codec" );
	if( pMicrophone->IsOpen( ) ) pMicrophone->Close( );
	if( pStreamSink->IsOpen( ) ) pStreamSink->Close( );

	// --- if we've been created we must be in the lobby, and should not do any of this
	if( m_created ) 
	{
		if( m_talker )
		{
			DOTalker::Ref ref( m_talker );
			if( ref.IsValid( ) && ( ref->IsAlive( ) == m_talker->IsAlive() ) )
			{
				ref->d_props.d_pitchWarp			= m_pitchWarp;
				ref->d_props.d_pitchOffset			= m_pitchOffset;
				ref->d_props.d_spectrumWarp		= m_spectrumWarp;
				ref->d_props.d_spectrumOffset		= m_spectrumOffset;
				ref->d_props.d_spectrumLedge		= m_spectrumLedge;
				ref->d_props.d_robot					= m_robot;
				ref->d_props.d_whisper				= m_whisper;
				ref->d_props.d_environ				= m_environment;
				ref->d_props.RequestUpdate();
			}
		}
		enable( true );
		return true;
	}

	if( pMicrophone )			delete pMicrophone;
	if( pStreamSink )			delete pStreamSink;
	if( m_pActiveCodec )		delete ( ( NellyMoserCodec * )m_pActiveCodec );
	pMicrophone			= NULL;
	pStreamSink			= NULL;
	m_pActiveCodec		= NULL;
	m_pCodecs[ CHAT_CODEC_NELLYMOSER ] = NULL;
	CodecManager::GetInstance()->UnregisterCodec( CODEC );

	return true;
}

// ----------------------------------------------------------------------------
void *CVoiceChat::getPlatformVoiceManager( void )
{
	return( NULL );
}

// ----------------------------------------------------------------------------
bool CVoiceChat::isPlayerMuted( u64 id, u32 flags )
{
	return( false );
}

// -------------------------------- Message Handlers --------------------------
void CVoiceChat::updateMuting( void )
{
	if( m_talker )
	{
		SMuteData dataOut;

		dataOut.id		= m_talker->d_props.d_id;
		dataOut.host	= g_session.GetLocalHost();
		g_messageDispatcher.SendMessageToSession( REQUEST_MUTING, &dataOut, INVALID_OBJECT_HANDLE );
		g_console.Echo( "Requesting Mute Information, my id is %x\n", dataOut.id );
	}
}

// ---------------------------------------------------------------------------
// --- Someone is requesting information about our mute list
void CVoiceChat::CRequestMuting::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted )
{
	SMuteData *dataIn = ( SMuteData * )i_data;


	// --- bail if we sent the message to all
	if( dataIn->id == d_ref->m_talker->d_props.d_id )
		return;

	g_console.Echo("%x id is requesting mute data, my id is %x\n", dataIn->id, d_ref->m_talker->d_props.d_id );
	SMuteData dataOut;
	dataOut.id		= d_ref->m_talker->d_props.d_id;
	dataOut.host	= g_session.GetLocalHost();
	ASSERT( dataOut.host );

	// --- ok everything is valid so send a message back to the sender with our id letting him know he needs to mute us
	g_messageDispatcher.SendMessageToHost( REMOTE_MUTE, &dataOut, INVALID_OBJECT_HANDLE, dataIn->host );
	g_console.Echo("Sending test message to mute me id=%x to id=%x\n", d_ref->m_talker->d_props.d_id, dataIn->id );

}

// ---------------------------------------------------------------------------
// --- someone is responding back to our mute information request and is
// --- telling us that have us muted, so we must also mute them
void CVoiceChat::CRemoteMute::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted )
{
	SMuteData *dataIn = ( SMuteData * )i_data;

	g_console.Echo("%x id is requesting to be muted\n", dataIn->id );
}

// ---------------------------------------------------------------------------
// --- someone is responding back to our mute information request and is
// --- telling us that we are not in their mute list, so unmute them
void CVoiceChat::CRemoteUnMute::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted )
{
	SMuteData *dataIn = ( SMuteData * )i_data;
	g_console.Echo("%x id is requesting to be unmuted\n", dataIn->id );
}

// ---------------------------------------------------------------------------
// --- someone is responding back to our mute information request and is
// --- telling us that have us muted, so we must also mute them
void CVoiceChat::CMute::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted )
{
	SMuteData *dataIn = ( SMuteData * )i_data;

	g_console.Echo("%x id is requesting to be muted\n", dataIn->id );
}

// ---------------------------------------------------------------------------
// --- someone is responding back to our mute information request and is
// --- telling us that we are not in their mute list, so unmute them
void CVoiceChat::CUnMute::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted )
{
	SMuteData *dataIn = ( SMuteData * )i_data;
	g_console.Echo("%x id is requesting to be unmuted\n", dataIn->id );
}

// ---------------------------------------------------------------------------
// THIS IS CRAP.  The overloaded "new" and "delete" functions aren't overloaded
// in the DLL!  This is to fix this, and it feels dirty.
void* operator new( unsigned int size )   { MEM_SET_RETURN_ADDRESS(); return memAlloc( size, 16 ); }
void* operator new[]( unsigned int size ) { MEM_SET_RETURN_ADDRESS(); return memAlloc( size, 16 ); }
void  operator delete( void* addr )       { memFree( addr ); }
void  operator delete[]( void* addr )     { memFree( addr ); }

// ---------------------------------------------------------------------------
