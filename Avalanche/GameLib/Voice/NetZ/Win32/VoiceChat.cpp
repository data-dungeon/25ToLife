// VoiceChat.cpp : Defines the class behaviours for the application.
//
#include "Voice/VoicePCH.h"

#include <netz.h>
#include <platform/core/logdevicedebugoutput.h>
#include <utilities/commandline/stdcmdline.h>
#include <utilities/sessionenumeration/windowssessionenumerator.h>
#include "Voice/NetZ/Win32/DSoundSource.h"
#include "Voice/NetZ/Win32/DSoundSink.h"
#include <extensions/voicechat/voicechatext.h>
#include <extensions/avstreams/codecs/lpc10/lpc10codec.h>
#include <extensions/avstreams/codecs/openlpc/openlpccodec.h>
#include <extensions/avstreams/codecs/gsm/gsmcodec.h>

#include "Voice/NetZ/Win32/VoiceChatManager.h"
#include "Voice/NetZ/CVoiceChat.h"
#include "cmdcon/cmdcon.h"
#include "EngineHelper/timer.h"

#ifdef DIRECTX_PC
#include "platform/Win32/wininput.h"
#endif

// --- create our singleton
CVoiceChat g_CVoiceChat;
CHost* speakerHost = NULL;
ControllerButton CVoiceChat::s_pushToTalkButton = CB_STICK1; 

// ----------------------------------------------------------------------------
//
// This class forwards the VoiceChannel generated events
// to the Dialog in order to update the window.
class MyDemuxEventHandler: public DemuxEventHandler {
public:
	MyDemuxEventHandler(VoiceChannel *pChannel) { m_pVoiceChannel = pChannel; }
	~MyDemuxEventHandler() {}

	virtual qBool StreamOpened(qByte idStream, DOHandle dohMember) 
	{ 
		qBool result = DemuxEventHandler::StreamOpened(idStream, dohMember);

		// map the speaker to a host object
		speakerHost = NULL;
		VoiceChannelMember* member = m_pVoiceChannel->FindMember( dohMember );
		if ( member )
		{
			ASSERT(dohMember.IsAKindOf(DOCLASSID(DOTalker)));
			DOTalker::Ref refTalker(dohMember);
			if (refTalker.IsValid())
			{
				g_CVoiceChat.setWhosTalking( dohMember );

				// --- set our start talk time
				if( g_CVoiceChat.getLocalTalker( ) == *refTalker )
					g_CVoiceChat.setTalkTimer( g_timer.GetOSTime( ) );
			}
			else
			{
				g_CVoiceChat.setWhosTalking( INVALID_DOHANDLE );
			}

			DOHandle dohStation = member->GetMemberStation();
			if ( dohStation == ((DOHost*)g_session.GetLocalHost())->GetMasterStation() )
			{
				speakerHost = g_session.GetLocalHost();
			}
			else
			{
				CHost* host;
				for ( host = (DOHost*) g_session.GetNextRemoteHost( NULL ) ;
					host ;
					host = (DOHost*) g_session.GetNextRemoteHost( host ) )
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

	virtual qBool StreamClosed(qByte idStream, DOHandle dohMember) 
	{ 
		qBool result = DemuxEventHandler::StreamClosed(idStream, dohMember);
		speakerHost = NULL;
		g_CVoiceChat.setListen( false );
		g_CVoiceChat.setWhosTalking( INVALID_DOHANDLE );
		return( result );
	}

	virtual void MembershipChanged() 
	{ 
		if (m_pVoiceChannel != NULL) 
		{
			g_CVoiceChat.setMemberShipChanged( true );
		}
	}

	void SetChannel(VoiceChannel *pChannel) { m_pVoiceChannel = pChannel; }
public:
	VoiceChannel	*m_pVoiceChannel;
};
// ----------------------------------------------------------------------------

#define GSM		CodecID(0)
#define CODEC GSM
GSMCodec m_oCodec(100);
DSoundSource *pMicrophone=NULL;
DSoundSink *pStreamSink=NULL;
static bool m_joined = false;
HWND m_hWnd = NULL;


// ----------------------------------------------------------------------------
CVoiceChat::CVoiceChat( void ) :
	m_deviceExists( false ),
	m_enabled( false ),
	m_created( false ),
	m_listen( false ),
	m_memberShipChanged( false ),
	m_teamID( -1 ),
	m_voiceMaskEnable( false ),
	m_talker( NULL ),
	m_chatState( STATE_CHAT_UNINITIALIZED ),
	m_talkingTalker( INVALID_DOHANDLE ),
	m_voiceRedirected( false ),
	m_headsetConnected( false ),
	m_voiceBanned( false ),
	m_userId( 0 )
{
	memset( m_chatChannels, 0, sizeof( m_chatChannels ) );
	memset( m_pCodecs, 0, sizeof( m_pCodecs ) );
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
		delete ( Codec * )m_pCodecs[ i ];
		m_pCodecs[ i ] = NULL;
	}
	m_chatState = STATE_CHAT_UNINITIALIZED;
}

// ----------------------------------------------------------------------------
const bool CVoiceChat::createCodecs( void )
{
#if 0
	if( !m_pCodecs[ CHAT_CODEC_GSM ] )
	{
		m_pCodecs[ CHAT_CODEC_GSM ] = ( void * )new GSMCodec( 100 );
		ASSERTS( m_pCodecs[ CHAT_CODEC_GSM ], "Failed to create gsm codec" );
	}

	if( !m_pCodecs[ CHAT_CODEC_NELLYMOSER ] )
	{
		m_pCodecs[ CHAT_CODEC_NELLYMOSER ] = ( void * )new NellyMoserCodec( 100 );
		ASSERTS( m_pCodecs[ CHAT_CODEC_NELLYMOSER ], "Failed to create gsm codec" );
	}

	if( !m_pCodecs[ CHAT_CODEC_OPENLPC ] )
	{
		m_pCodecs[ CHAT_CODEC_OPENLPC ] = ( void * )new OpenLPCCodec( 1800 );
		ASSERTS( m_pCodecs[ CHAT_CODEC_GSM ], "Failed to create gsm codec" );
	}
	m_pActiveCodec = m_pCodecs[ CHAT_CODEC_NELLYMOSER ];
#endif
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
	if( m_created ) return( true );

	// --- must have a talker
	if( talker == NULL )
	{
		return( false );
	}
	m_talker = talker;
	m_chatState = STATE_CHAT_UNINITIALIZED;
	m_userId = m_talker->d_props.d_id;

	WindowsApp *winApp = WindowsApp::IAm();
	m_hWnd = winApp->GethWnd();
	ASSERTS( m_hWnd, "invalid window handle" );

	// --- create the headset device if it does not exist already
	if( !m_deviceExists )
	{
		// --- Initialize the headset
		CodecManager::GetInstance()->RegisterCodec( GSM, &m_oCodec );
		m_deviceExists = true;
	}
	else
	{
		return( false );
	}

	m_created = true;
	m_teamID = -1;

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
		( ( VoiceChatManager * )m_chatChannels[ i ].pChatManager )->InitializeChannel( _T("TTL Voice Channel" ), 1, ( MyDemuxEventHandler * )m_chatChannels[ i ].pEventHandler );
	}

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
	return( true );
}

// ----------------------------------------------------------------------------
const bool CVoiceChat::update( )
{
	// --- make sure we've been created
	if( !m_created ) return( false );

	// --- make sure we're enabled
	if( !isChatEnabled( ) )
	{
		for( int i = 0; i < MAX_CHAT_CHANNELS; i++ )
		{
			VoiceChatManager *pMgr = ( VoiceChatManager * )m_chatChannels[ i ].pChatManager;
			if( pMgr->IsTalking( ) )
			{
				pMgr->StopTalking( );
			}
		}
		return( false );
	}

	// --- if we're not the session master we need to try and discover voice channels to join
	if( !allJoined( ) ) 
	{
		bool result = false;

		for( int i = 0; i < MAX_CHAT_CHANNELS; i++ )
		{
			if( !m_chatChannels[ i ].discovered && m_chatChannels[ i ].enabled )
			{
				g_console.Echo( "Trying to discover channel #%d\n", i );

				ASSERTS( m_chatChannels[ i ].pChatManager, "chat manager has not been created yet" );
				m_chatChannels[ i ].discovered = ( ( VoiceChatManager *)m_chatChannels[ i ].pChatManager )->discoverChannel( i );

				// --- see if we discovered a channel
				if( m_chatChannels[ i ].discovered )
				{
					g_console.Echo( "Discovered channel #%d\n", i );

					VoiceChatManager *pMgr		= ( VoiceChatManager * )m_chatChannels[ i ].pChatManager;
					MyDemuxEventHandler *pEvt	= ( MyDemuxEventHandler * )m_chatChannels[ i ].pEventHandler;

					// --- create the microphone and sink if we need to
					if( pMicrophone == NULL )
					{
						// Each DSoundSink has to be associated with a window handle (HWND) and the format of the Channel.
						pStreamSink = new DSoundSink( m_hWnd, ( ( VoiceChatManager * )m_chatChannels[ i ].pChatManager )->GetChannel()->GetPacketSize()/2);
						pMicrophone = new DSoundSource( m_oCodec.GetFormat( ) );
					}

					// --- save away the voice channel in the event handler
					pEvt->SetChannel( pMgr->GetChannel( ) );

					ASSERTS( pStreamSink, "unable to create stream sink" );
					( ( DemuxEventHandler * )pMgr->GetEventHandler( ) )->SetStreamSink( 0, pStreamSink );

					// --- Initialize the Talker.  This will join the VoiceChannel
					ASSERTS( pMicrophone, "unable to create microphone" );
					g_console.Echo( "Joining voice channel #%d\n", i );
					result = pMgr->InitializeTalker( m_talker->GetHandle(), pMicrophone );
					ASSERTS( result, "unable to initialize all talk voice channel" );
				}
			}
		}
	}

	// --- check for press to talk and try to acquire the voice channel
#ifdef DIRECTX_PC
	WindowsSystemInputDriver* winInputPtr = (WindowsSystemInputDriver*)g_inputPtr;
	ControllerInputDriver *c = winInputPtr->GetController( 0 );
#else
	ControllerInputDriver *c = g_input.GetController( 0 );
#endif
	// --- now lets process the chat channels
	bool chatting = false;
	for( int i = 0; i < MAX_CHAT_CHANNELS; i++ )
	{
		if( isChatChannelEnabled( i ) )
		{
			VoiceChatManager *pMgr = ( VoiceChatManager * )m_chatChannels[ i ].pChatManager;

			if( c->ButtonDown( s_pushToTalkButton ) && !m_memberShipChanged &&
				( ( ( g_timer.GetOSTime() - m_voiceLastUsedTime ) >= MIN_CHAT_WAIT ) || pMgr->IsTalking() ) )
			{
				m_chatChannels[ i ].tryingToChat = true;
				if( !pMgr->IsTalking( ) )
					pMgr->StartTalking( );
			}
			else
			{
				m_chatChannels[ i ].tryingToChat = false;
				if( pMgr->IsTalking( ) )
				{
					m_voiceLastUsedTime = g_timer.GetOSTime( );
					pMgr->StopTalking( );
				}
				m_memberShipChanged = false;
			}

			// --- process the channel
			pMgr->Process( );

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
					pMgr->StopTalking();
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
	return( true );
}


// ----------------------------------------------------------------------------
const void CVoiceChat::tearDown( void )
{
	// --- make sure we've been created
	if( !m_created ) return;

	m_created = false;
	m_messageOwner.UnregisterAll();

	// --- make sure we stop talking just in case
	for( int i = 0; i < MAX_CHAT_CHANNELS; i++ )
	{
		VoiceChatManager *pMgr = ( VoiceChatManager * )m_chatChannels[ i ].pChatManager;

		if( pMgr->IsTalking( ) ) pMgr->StopTalking( );

		// --- tear down the managers
		pMgr->Teardown( );

		// --- update the status of the channel
		m_chatChannels[ i ].enabled				= false;
		m_chatChannels[ i ].discovered			= false;
		m_chatChannels[ i ].membershipChanged	= true;
		m_chatChannels[ i ].chatting				= false;
		m_chatChannels[ i ].listening				= false;
		m_chatChannels[ i ].tryingToChat			= false;

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

	if( m_deviceExists )
	{
		CodecManager::GetInstance()->UnregisterCodec( GSM );
		m_deviceExists = false;
	}

	m_voiceLastUsedTime = g_timer.GetOSTime( );
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

	// --- we requesting a state change on the channel, so make sure we stop talking
	if( m_chatChannels[ channel ].enabled != trueOrFalse )
	{
		VoiceChatManager *pMgr = ( VoiceChatManager * )m_chatChannels[ channel ].pChatManager;
		if( pMgr )
		{
			if( pMgr->IsTalking( ) )
			{
				pMgr->StopTalking( );
			}
		}
	}
	m_chatChannels[ channel ].enabled = trueOrFalse;
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
				if( pMgr->IsTalking( ) ) pMgr->StopTalking( );
			}
		}
	}
	m_enabled = trueOrFalse;
}

// ----------------------------------------------------------------------------
const void CVoiceChat::enableVoiceMasking( bool trueOrFalse, int maskID /* = 0  */ )
{
	if( trueOrFalse )
	{
		m_voiceMaskID = maskID;
	}
	m_voiceMaskEnable = trueOrFalse;
}

// ----------------------------------------------------------------------------
const void CVoiceChat::updateVoiceMasking( int streamIndex, float a, float b, float c, float d, float e, float f, float g, int env )
{
}

// ----------------------------------------------------------------------------
const bool CVoiceChat::isPlayerTalking( DOTalker *talker )
{
// --- DAS 7/5/2005 add support for this to PC
#if 0
	for( int i = 0; i < MAX_SIMULTANEOUS_TALKERS_ALL; i++ )
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
#endif
	return( false );
}

// ----------------------------------------------------------------------------
void CVoiceChat::setOutputVolume( float volume )
{
	m_volume = volume;
}

// ----------------------------------------------------------------------------
void CVoiceChat::refreshPlatformData( )
{
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
	return true;
}

// ----------------------------------------------------------------------------
const bool CVoiceChat::updateHeadSet( void )
{
	return true;
}

// ----------------------------------------------------------------------------
const bool CVoiceChat::closeHeadSet( void )
{
	return true;
}

// ----------------------------------------------------------------------------
void *CVoiceChat::getPlatformVoiceManager( void )
{
	return( NULL );
}

void CVoiceChat::postNetZTearDown( void )
{
}

// ----------------------------------------------------------------------------
bool CVoiceChat::isPlayerMuted( u64 id, u32 flags )
{
	return( false );
}

// ----------------------------------------------------------------------------
void CVoiceChat::channelVoiceThruSpeakers( bool trueOrFalse )
{ 
	m_voiceRedirected = trueOrFalse;

	// --- we have changed the voice redirection, so lets get and set our headset status
	//m_headSetStatus = ControllerInputDriver::UNINITIALIZED;
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
