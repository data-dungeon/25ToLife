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
#include "Voice/VoicePCH.h"

#include <netz.h>
#include <utilities/consoleio/consoleio.h>
#include <utilities/commandline/stdcmdline.h>
#include <utilities/sessionenumeration/consolesessionenumerator.h>
#include <extensions/voicechat/voicechatext.h>
#include "VoiceChatManager.h"
#include <xhv.h>
#include <dsstdfx.h>
#include "XHVVoiceManager.h"

#include "Voice/NetZ/CVoiceChat.h"
#include "cmdcon/cmdcon.h"
#include "EngineHelper/timer.h"

#include "Audio/Xbox/sdriver.h"
qBool bJoined = false;

// --- create our singleton
CVoiceChat g_CVoiceChat;

// --- Local vars -------------------------------------------------------------
#define MAX_SIMULTANEOUS_TALKERS_ALL 4
#define MAX_SIMULTANEOUS_TALKERS_TEAM1 1
#define MAX_SIMULTANEOUS_TALKERS_TEAM2 1

DOHandle m_talkerDO = INVALID_DOHANDLE;
DOHandle m_talkersDO[ MAX_SIMULTANEOUS_TALKERS_ALL ];
CHost* speakerHost = NULL;
static ControllerInputDriver::HeadSetStatus m_headSetStatus;
static DWORD s_comPort = 0;
#define NB_STREAMS	2
ControllerButton CVoiceChat::s_pushToTalkButton = CB_L2; 
extern XONLINE_USER* m_pUsers;
// ----------------------------------------------------------------------------
qBool XHVInit(CXHVVoiceManager *pVoiceManager, ITitleXHV *pCallbackHandler)
{
	HRESULT hr = S_OK;
	LPDIRECTSOUND8	pDSound;

	//--- Create DirectSound in order to download our DSP image, which 
	//--- contains the voice mixer SRC effects
	DirectSoundCreate( NULL, &pDSound, NULL );
	if( !pDSound )
	{
		ASSERTS( false, "Unable to create Direct Sound Object" );
		return( false );
	}

	//--- Tell DirectSound where the 3D Reverb and XTalk effects are
	DSEFFECTIMAGELOC dsImageLoc;
	dsImageLoc.dwI3DL2ReverbIndex = GraphI3DL2_I3DL2Reverb;
	dsImageLoc.dwCrosstalkIndex = GraphXTalk_XTalk;

	//--- Download the DSP image
	LPDSEFFECTIMAGEDESC pdsImageDesc;
	hr = XAudioDownloadEffectsImage( "d:\\data_xbox\\sound\\dsstdfx.bin", &dsImageLoc, XAUDIO_DOWNLOADFX_EXTERNFILE, &pdsImageDesc );
	if( hr != S_OK )
	{
		ASSERTS( false, "unable to download dsp image" );
		return false;
	}

	//--- Set up parameters for the Voice Chat engine
	XHV_RUNTIME_PARAMS xhvParams = {0};
	xhvParams.dwMaxLocalTalkers         = XGetPortCount();
	xhvParams.dwMaxRemoteTalkers        = MAX_SIMULTANEOUS_TALKERS_ALL;
	xhvParams.dwMaxCompressedBuffers    = 4;                    // 4 buffers per local talker
	xhvParams.dwFlags                   = 0;
	xhvParams.pEffectImageDesc          = pdsImageDesc;
	xhvParams.dwEffectsStartIndex       = GraphVoice_Voice_0;

	//--- The out-of-sync threshold allows the title to control how aggresive
	//--- XHV is about determining that we've lost synchronization with a 
	//--- remote talker.  A voice packet that is received significantly before or
	//--- after its expected time is considered to be "out-of-sync."  If this
	//--- many consecutive packets from a given remote talker are determined to
	//--- be out-of-sync, that remote talker will be reset, causing a brief pause
	//--- in their voice playback.
	//--- This number should be roughly twice as many packets as the title holds
	//--- in its network send buffer.
	xhvParams.dwOutOfSyncThreshold      = 10;   

	hr = pVoiceManager->Initialize( pDSound, &xhvParams, pCallbackHandler );
	if( hr != S_OK )
	{
		ASSERTS( false, "Error while createing XHV Voice Manager" );
		return false;
	}

	return true;
}


VoiceChatManager	*g_pManager = NULL;
CXHVVoiceManager	*g_pVoiceManager = NULL;


// ----------------------------------------------------------------------------
//
// This class forwards the VoiceChannel generated events
// to the Dialog in order to update the window.
class MyEventHandler: public VoiceChannel::EventHandler {
public:
	MyEventHandler( VoiceChannel *pChannel ) { m_pVoiceChannel = pChannel; }
	~MyEventHandler() {}

	// ---------------------------------------------------------------------------
	virtual qBool StreamOpened( qByte idStream, DOHandle dohMember) 
	{ 
		if( dohMember != INVALID_DOHANDLE )
		{
#ifdef _DEBUG
		ConsoleIO::Print(_T("Stream %d opened by %x\n"),idStream,dohMember.GetValue());
#endif
			qBool result = EventHandler::StreamOpened(idStream, dohMember);
			m_talkerDO = dohMember;

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
					m_talkersDO[ idStream ] = dohMember;

					// --- set our start talk time
					if( g_CVoiceChat.getLocalTalker( ) == *refTalker )
						g_CVoiceChat.setTalkTimer( g_timer.GetOSTime( ) );

					XUID uidUser;
					uidUser.qwUserID = (*refTalker)->d_props.d_id;
					uidUser.dwUserFlags = (*refTalker)->d_props.d_userFlags;
					HRESULT hr = g_pVoiceManager->RegisterRemoteTalker(uidUser);
					if (FAILED(hr)) {
						QTRACE(TRACE_ALWAYS,(_T("RegisterRemoteTalker failed with error %x"),hr));
					}
					g_pVoiceManager->SetRemoteTalkerPriority(uidUser, s_comPort, XHV_PLAYBACK_PRIORITY_MAX);
				}
				else
				{
					g_CVoiceChat.setWhosTalking( INVALID_DOHANDLE );
				}

#ifdef _DEBUG
				ConsoleIO::Print(_T("I am a member of this voice channel\n" ) );
#endif
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
		return( false );
	}


	// -------------------------------------------------------------------------
	virtual qBool StreamClosed(qByte idStream, DOHandle dohMember) 
	{ 
		if( dohMember != INVALID_DOHANDLE )
		{
#ifdef _DEBUG
			ConsoleIO::Print(_T("Stream %d closed by %x\n"),idStream,dohMember.GetValue());
#endif
			ASSERT(dohMember.IsAKindOf(DOCLASSID(DOTalker)));
			DOTalker::Ref refTalker(dohMember);
			if (refTalker.IsValid())
			{
				XUID uidUser;
				uidUser.qwUserID = (*refTalker)->d_props.d_id;
				uidUser.dwUserFlags = (*refTalker)->d_props.d_userFlags;

				HRESULT hr = g_pVoiceManager->UnregisterRemoteTalker(uidUser);
				if (FAILED(hr)) {
					QTRACE(TRACE_ALWAYS,(_T("UnregisterRemoteTalker failed with error %x"),hr));
				}
			}
			qBool result = EventHandler::StreamClosed(idStream, dohMember);
			speakerHost = NULL;
			m_talkersDO[ idStream ] = INVALID_DOHANDLE;
			g_CVoiceChat.setListen( false );
			g_CVoiceChat.setWhosTalking( INVALID_DOHANDLE );
			return( result );

		}
		return( false );
	}

	// -------------------------------------------------------------------------
	virtual qBool StreamPacketReceived(qByte idStream, DOHandle dohMember, 
											qByte* pbyStreamPacket, qUnsignedInt uiPacketSize) 
	{
		ASSERT(dohMember.IsAKindOf(DOCLASSID(DOTalker)));
		if( dohMember != INVALID_DOHANDLE )
		{
			DOTalker::Ref refTalker(dohMember);
			if (g_CVoiceChat.isVoiceEnabled() && refTalker.IsValid())
			{
				XUID uidUser;
				uidUser.qwUserID = (*refTalker)->d_props.d_id;
				uidUser.dwUserFlags = (*refTalker)->d_props.d_userFlags;
				HRESULT hr = g_pVoiceManager->SubmitIncomingVoicePacket(uidUser,pbyStreamPacket,uiPacketSize);
				if (FAILED(hr)) {
					QTRACE(TRACE_ALWAYS,(_T("SubmitIncomingVoicePacket failed with error %x"),hr));
				}
				// --- set this talker's DO Handle into our tracking array
				ASSERTS( idStream < MAX_SIMULTANEOUS_TALKERS_ALL, "stream index out of range" );
				m_talkersDO[ idStream ] = dohMember;
				return( (qBool) hr );
			}
		}
		return( false );
	}

	// --- someone just joined or left one of our voice channels
	virtual void MembershipChanged() 
	{ 
#ifdef _DEBUG
		ConsoleIO::Print(_T("Membership Changed\n"));
#endif
		if (m_pVoiceChannel != NULL) 
		{
			m_pVoiceChannel->TraceMembers(TRACE_ALWAYS);
			g_CVoiceChat.setMemberShipChanged( true );
			//g_CVoiceChat.updateMuting( );
		}
	}

	void SetChannel(VoiceChannel *pChannel) { m_pVoiceChannel = pChannel; }
	VoiceChannel *GetChannel( ) { return m_pVoiceChannel; }

public:
	VoiceChannel	*m_pVoiceChannel;
};


// ----------------------------------------------------------------------------
class XVHCallbackHandler: public ITitleXHV {
public:
	STDMETHODIMP CommunicatorStatusUpdate( DWORD dwPort, XHV_VOICE_COMMUNICATOR_STATUS status )
	{
		if( status == XHV_VOICE_COMMUNICATOR_STATUS_INSERTED )
		{
			HRESULT hr;
			if( s_comPort != dwPort )
			{
				if( s_comPort != -1 )
				{
					hr = g_pVoiceManager->UnregisterLocalTalker( s_comPort );
					ASSERT( hr == S_OK );
				}
				hr = g_pVoiceManager->RegisterLocalTalker( dwPort );
				ASSERT( hr == S_OK );
				g_CVoiceChat.setOutputVolume( g_CVoiceChat.getOutputVolume( ) );
				s_comPort = dwPort;
				g_pVoiceManager->SetProcessingMode( s_comPort, XHV_VOICECHAT_MODE );
				g_console.Echo("Registering Local talker on port %d\n", s_comPort );
			}
			g_pVoiceManager->SetVoiceThroughSpeakers( s_comPort, false );
		}
		else
		{
			g_pVoiceManager->SetVoiceThroughSpeakers( dwPort, true );
		}

		g_console.Echo( "Communicator status changed inside Voice Chat on port %d\n", dwPort );
		return S_OK;
	}

    STDMETHODIMP LocalChatDataReady( DWORD dwPort, DWORD dwSize, PVOID pData )
	{
		if( g_pManager && g_CVoiceChat.isVoiceEnabled() )
		{
			if( g_pManager->GetMemberSink() )
			{
				if (g_pManager->GetMemberSink()->IsOpen()) 
				{
					g_pManager->GetMemberSink()->Write((qByte*)pData,dwSize);
				}
			}
		}
		return S_OK;
	}
};


// ----------------------------------------------------------------------------
// statics
CXHVVoiceManager	oVoiceManager;	
XVHCallbackHandler	oHandler;


// ----------------------------------------------------------------------------
CVoiceChat::CVoiceChat( ) :
	m_deviceExists( false ),
	m_enabled( false ),
	m_created( false ),
	m_listen( false ),
	m_memberShipChanged( false ),
	m_teamID( -1 ),
	m_voiceMaskEnable( false ),
	m_talker( NULL ),
	m_chatState( STATE_CHAT_UNINITIALIZED ),
	m_multiPlayMode( LAN_PLAY ),
	m_talkingTalker( INVALID_DOHANDLE),
	m_voiceRedirected( false ),
	m_headsetConnected( false ),
	m_volume( 1.0f ),
	m_voiceBanned( false ),
	m_userId( 0 ),
	m_userFlags( 0 )
{
	m_numStreams[ CHAT_CHANNEL_ALL ]		= MAX_SIMULTANEOUS_TALKERS_ALL;
	m_numStreams[ CHAT_CHANNEL_TEAM1 ]	= MAX_SIMULTANEOUS_TALKERS_TEAM1;
	m_numStreams[ CHAT_CHANNEL_TEAM2 ]	= MAX_SIMULTANEOUS_TALKERS_TEAM2;
	memset( m_chatChannels, 0, sizeof( m_chatChannels ) );
	memset( m_pCodecs, 0, sizeof( m_pCodecs ) );
	m_headSetStatus = ControllerInputDriver::UNINITIALIZED;
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
			delete ( MyEventHandler * )m_chatChannels[ i ].pEventHandler;
			m_chatChannels[ i ].pEventHandler = NULL;
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
	return( true );
}

// ----------------------------------------------------------------------------
void *CVoiceChat::getPlatformVoiceManager( void )
{
	return( ( void * )&oVoiceManager );
}

// ----------------------------------------------------------------------------
bool CVoiceChat::createPlatformSpecific( void )
{
	m_headSetStatus = ControllerInputDriver::UNINITIALIZED;

	// --- check for voice banning
	m_pUsers = XOnlineGetLogonUsers();
	if( m_pUsers )
	{
		for( DWORD i = 0; i < XGetPortCount(); i++ )
		{
			if( m_pUsers[i].xuid.qwUserID != 0 )
			{
				m_voiceBanned = !( XOnlineIsUserVoiceAllowed( m_pUsers[ i ].xuid.dwUserFlags ) );
				m_userId = m_pUsers[i].xuid.qwUserID;
				m_userFlags = m_pUsers[i].xuid.dwUserFlags;
			}
		}
	}

	// --- if we are voice banned do not create the xhv engine
	if( isVoiceChatBanned( ) ) return false;

	if( !m_deviceExists )
	{
		g_pVoiceManager	= &oVoiceManager;
		m_deviceExists = XHVInit( &oVoiceManager,&oHandler );
		if( m_deviceExists )
		{
			ControllerInputDriver *c = NULL;
			if ( g_input.ControllerLocked(0) )
				c = g_input.GetController(0);

			// --- update the communicator	
			if( c )
			{
				ControllerInputDriver::HeadSetStatus headSetStatus = c->GetHeadSetStatus();
				s_comPort = c->GetHeadSetPort();
				g_console.Echo("Registering Local talker on port %d\n", s_comPort );
				oVoiceManager.RegisterLocalTalker( s_comPort );
				HRESULT hr = g_pVoiceManager->SetVoiceSpeakerVolume( m_volume, UserVoiceRateConverter_VoiceRateConverter );
				ASSERT( hr == S_OK );
				oVoiceManager.SetVoiceThroughSpeakers( s_comPort, true );
				oVoiceManager.SetProcessingMode( s_comPort, XHV_VOICECHAT_MODE );
			}
		}
	}
	return m_deviceExists;
}

// ----------------------------------------------------------------------------
bool CVoiceChat::releasePlatformSpecific( void )
{
	m_headSetStatus = ControllerInputDriver::UNINITIALIZED;
	if( m_deviceExists )
	{
		HRESULT hr = 0;
		if( s_comPort != -1 )
			hr = oVoiceManager.UnregisterLocalTalker(s_comPort);
		s_comPort = -1;
		m_created = false;
		hr |= oVoiceManager.Shutdown();
		m_deviceExists = false;
		if( hr == S_OK ) return( true );
	}
	return( false );
}

// ----------------------------------------------------------------------------
const bool CVoiceChat::create( DOTalker* talker, CSession &sesion )
{
	bool			result;
	
	g_pManager = NULL;
	m_headSetStatus = ControllerInputDriver::UNINITIALIZED;


	// --- must have a talker
	ASSERT( talker );
	if( talker == NULL )
	{
		return( false );
	}
	m_talker = talker;
	m_teamID = -1;
	m_chatState = STATE_CHAT_UNINITIALIZED;

	// --- fill in the live XUID into the DOTalker
	m_userId = m_talker->d_props.d_id;
	m_userFlags = m_talker->d_props.d_userFlags;
	m_pUsers = XOnlineGetLogonUsers();
	if( m_pUsers )
	{
		for( DWORD i = 0; i < XGetPortCount(); i++ )
		{
			if( m_pUsers[i].xuid.qwUserID != 0 )
			{
				m_voiceBanned = !( XOnlineIsUserVoiceAllowed( m_pUsers[ i ].xuid.dwUserFlags ) );
				m_userId = m_pUsers[i].xuid.qwUserID;
				m_userFlags = m_pUsers[i].xuid.dwUserFlags;
				if( m_talker )
				{
					m_talker->d_props.d_id = m_pUsers[i].xuid.qwUserID;
					m_talker->d_props.d_userFlags = m_pUsers[i].xuid.dwUserFlags;
					m_talker->d_props.RequestUpdate();
					break;
				}
			}
		}
	}

	if( m_created ) return( true );
	m_created = true;

	// --- XHV Engine
	if( m_multiPlayMode == LAN_PLAY )
	{
		bool success = createPlatformSpecific( );
	}

	// --- create all the voice chat channels we need
	for( int i = 0; i < MAX_CHAT_CHANNELS; i++ )
	{
		// --- create the manager
		m_chatChannels[ i ].pChatManager = ( void * ) new VoiceChatManager;
		ASSERTS( m_chatChannels[ i ].pChatManager, "unable to create voice chat manager" );

		// --- create the event handler
		m_chatChannels[ i ].pEventHandler	= ( void * ) new MyEventHandler( NULL );
		ASSERTS( m_chatChannels[ i ].pEventHandler, "unable to create voice chat event handler" );

		// --- initialize the codec
		//( ( VoiceChatManager * )m_chatChannels[ i ].pChatManager )->SetCodecID( CODEC );

		// --- set the packet size, really just a dummy
		( ( VoiceChatManager * )m_chatChannels[ i ].pChatManager )->SetPacketSize( 10 );

		// --- initialize the channel
		( ( VoiceChatManager * )m_chatChannels[ i ].pChatManager )->InitializeChannel( _T("TTL Voice Channel" ), m_numStreams[ i ], ( MyEventHandler * )m_chatChannels[ i ].pEventHandler );

		m_chatChannels[ i ].queuedDisable = false;
		m_chatChannels[ i ].queuedEnable = false;
		m_chatChannels[ i ].openPending = false;
	}

	result = true;

	if( isVoiceChatBanned( ) ) return true;

	// --- install message handlers for muting support
	m_RequestMutingHandler.Initialize( REQUEST_MUTING,	MESSAGE_HANDLE_IF_MASTER | MESSAGE_ROUTE_TO_MASTER ,m_messageOwner, this );
	m_MuteHandler.Initialize( MUTE,	MESSAGE_HANDLE_IF_MASTER |	MESSAGE_ROUTE_TO_MASTER, m_messageOwner, this );
	m_UnMuteHandler.Initialize( UNMUTE, MESSAGE_HANDLE_IF_MASTER |MESSAGE_ROUTE_TO_MASTER, m_messageOwner, this );
	m_RemoteMuteHandler.Initialize( REMOTE_MUTE,	MESSAGE_HANDLE_IF_MASTER |	MESSAGE_ROUTE_TO_MASTER, m_messageOwner, this );
	m_RemoteUnMuteHandler.Initialize( REMOTE_UNMUTE, MESSAGE_HANDLE_IF_MASTER |MESSAGE_ROUTE_TO_MASTER, m_messageOwner, this );

	m_voiceLastUsedTime = g_timer.GetOSTime( );
	return result;
}


const bool CVoiceChat::update(  )
{
	// --- make sure we've been created
	if( !m_created || isVoiceChatBanned( ) ) return( false );

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
				ASSERTS( m_chatChannels[ i ].pEventHandler, "event handler has not been created yet" );
				MyEventHandler *pEvt	= ( MyEventHandler * )m_chatChannels[ i ].pEventHandler;
				VoiceChatManager *pMgr		= ( VoiceChatManager * )m_chatChannels[ i ].pChatManager;
				if( pEvt == NULL || pMgr == NULL ) continue;

				VoiceChannel *pChannel = pEvt->GetChannel( );
				bool alreadyMember = ( pChannel ? pChannel->IsAMember( m_talker->GetHandle() ) : false );

				if( alreadyMember )
					g_console.Echo( "Still a member of voice channel #%d...\n", i );

				// --- we cannot rejoin this channel as long as we are already a member of it
				if( !alreadyMember )
				{
					m_chatChannels[ i ].discovered = ( ( VoiceChatManager *)m_chatChannels[ i ].pChatManager )->discoverChannel( i );

					// --- see if we discovered a channel
					if( m_chatChannels[ i ].discovered )
					{
						g_console.Echo( "Discovered channel #%d\n", i );

						// --- set the packet size, really just a dummy
						pMgr->SetPacketSize( 10 );

						// --- save away the voice channel in the event handler
						pEvt->SetChannel( pMgr->GetChannel( ) );

						// --- Initialize the Talker.  This will join the VoiceChannel
						g_console.Echo( "Joining voice chat channel #%d, channel id=%p\n", i, pMgr->GetChannel() );
						result = pMgr->InitializeTalker( m_talker->GetHandle(), NULL );
						ASSERTS( result, "unable to initialize all talk voice channel" );
					}
				}
			}
		}
	}

	if( exitEarly ) return false;

	// --- let the XHVEngine do its work
	oVoiceManager.DoWork();

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

	// --- check for press to talk and try to acquire the voice channel
	ControllerInputDriver *c = NULL;
	if ( g_input.ControllerLocked(0) )
		c = g_input.GetController(0);

	// --- update the communicator
	ControllerInputDriver::HeadSetStatus headSetStatus = c->GetHeadSetStatus();
	m_headsetConnected = ( headSetStatus == ControllerInputDriver::INSERTED );
	if( m_talker->d_props.d_headset != m_headsetConnected )
	{
		m_talker->d_props.d_headset = m_headsetConnected;
		m_talker->d_props.RequestUpdate();
	}


	if( headSetStatus != m_headSetStatus )
	{
		m_headSetStatus = headSetStatus;
		if( m_headSetStatus == ControllerInputDriver::INSERTED )
		{
			s_comPort = c->GetHeadSetPort();
			oVoiceManager.SetVoiceThroughSpeakers( s_comPort, false );
		}
		else
			oVoiceManager.SetVoiceThroughSpeakers( s_comPort, true );
	}

	// --- override the headset if user requests redirection thru speakers
	if( m_voiceRedirected )
		oVoiceManager.SetVoiceThroughSpeakers( s_comPort, true );

	// --- now lets process the chat channels
	bool chatting = false;
	g_pManager = NULL;
	for( int i = 0; i < MAX_CHAT_CHANNELS; i++ )
	{
		if( isChatChannelEnabled( i ) )
		{
			VoiceChatManager *pMgr = ( VoiceChatManager * )m_chatChannels[ i ].pChatManager;

			if( c && ( m_talker->GetHandle() != INVALID_DOHANDLE ) )
			{
				g_pManager = pMgr;
				DOTalker::Ref ref( m_talker );
				if( ref.IsValid( ) && ( ref->IsAlive( ) == m_talker->IsAlive() ) &&
					c->ButtonDown( s_pushToTalkButton ) && !m_memberShipChanged && m_headSetStatus == ControllerInputDriver::INSERTED &&
					( ( ( g_timer.GetOSTime() - m_voiceLastUsedTime ) >= MIN_CHAT_WAIT ) || pMgr->IsTalking() ) )
				{
					m_chatChannels[ i ].tryingToChat = true;
					if( !pMgr->IsTalking( ) )
					{
						pMgr->StartTalking( );
						m_chatChannels[ i ].openPending = true;
					}
					else
					{
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
						m_chatChannels[ i ].openPending = false;
					}
					m_memberShipChanged = false;
				}
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

	return( true );
}


const void CVoiceChat::tearDown()
{
	// --- make sure we've been created
	if( !m_created ) return;

	m_created = false;

	m_messageOwner.UnregisterAll();

	if( m_multiPlayMode == LAN_PLAY )
	{
		releasePlatformSpecific();
		m_deviceExists = false;
	}

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
			delete ( MyEventHandler * )m_chatChannels[ i ].pEventHandler;
			m_chatChannels[ i ].pEventHandler = NULL;
		}
	}

	for( int i = 0; i < MAX_SIMULTANEOUS_TALKERS_ALL; i++ )
		m_talkersDO[ i ] = INVALID_DOHANDLE;
	setWhosTalking( INVALID_DOHANDLE );

	m_chatState = STATE_CHAT_UNINITIALIZED;
	m_talker = NULL;
	g_pManager = NULL;
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
		VoiceChatManager *pMgr = ( VoiceChatManager * )m_chatChannels[ channel ].pChatManager;
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
				if( pMgr == g_pManager ) g_pManager = NULL;
			}
		}
	}

	m_chatChannels[ channel ].enabled = trueOrFalse;
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
		s_pushToTalkButton = CB_L2;
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
	if( g_pVoiceManager )
	{
		HRESULT hr = g_pVoiceManager->SetVoiceSpeakerVolume( m_volume, UserVoiceRateConverter_VoiceRateConverter );
		ASSERT( hr == S_OK );
	}
}

// ----------------------------------------------------------------------------
void CVoiceChat::refreshPlatformData( )
{
	oVoiceManager.UpdateMuteList( );
}

// ----------------------------------------------------------------------------
bool CVoiceChat::isPlayerMuted( u64 id, u32 flags )
{
	XUID xuid;
	xuid.qwUserID = id;
	xuid.dwUserFlags = flags;
	if( oVoiceManager.inMyMuteList( xuid ) )
	{
		return( true );
	}
	return( false );
}

// ----------------------------------------------------------------------------
void CVoiceChat::channelVoiceThruSpeakers( bool trueOrFalse )
{ 
	m_voiceRedirected = trueOrFalse;

	// --- we have changed the voice redirection, so lets get and set our headset status
	m_headSetStatus = ControllerInputDriver::UNINITIALIZED;
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

void CVoiceChat::postNetZTearDown( void )
{
}


// -------------------------------- Message Handlers --------------------------
void CVoiceChat::updateMuting( void )
{
	if( isVoiceChatBanned( ) ) return;

	if( m_talker )
	{
		SMuteData dataOut;

		dataOut.id		= m_talker->d_props.d_id;
		dataOut.flags	= m_talker->d_props.d_userFlags;
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

	g_CVoiceChat.handleMuteUnMute( dataIn, false );

}

// ---------------------------------------------------------------------------
// --- someone is responding back to our mute information request and is
// --- telling us that they have us muted, so we must also mute them
void CVoiceChat::CMute::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted )
{
	SMuteData *dataIn = ( SMuteData * )i_data;
	XUID xuid;
	xuid.qwUserID = dataIn->id;
	xuid.dwUserFlags = dataIn->flags;
	oVoiceManager.addToMyMuteList( xuid );
	g_console.Echo("%x id is requesting to be muted\n", dataIn->id );
	g_console.Echo( "adding %x player to my temp mute list\n", dataIn->id );
	g_CVoiceChat.handleMuteUnMute( dataIn, true );
}

// ---------------------------------------------------------------------------
// --- someone is responding back to our mute information request and is
// --- telling us that we are not in their mute list, so unmute them
void CVoiceChat::CUnMute::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted )
{
	SMuteData *dataIn = ( SMuteData * )i_data;
	XUID xuid;
	xuid.qwUserID = dataIn->id;
	xuid.dwUserFlags = dataIn->flags;
	g_console.Echo("%x id is requesting to be Unmuted\n", dataIn->id );
	oVoiceManager.removeFromMyMuteList( xuid );
	g_console.Echo( "removing %x player from my temp mute list\n", dataIn->id );
	g_CVoiceChat.handleMuteUnMute( dataIn, true );
}

// ---------------------------------------------------------------------------
// --- someone is responding back to our mute information request and is
// --- telling us that they have us muted, so we must also mute them
void CVoiceChat::CRemoteMute::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted )
{
	SMuteData *dataIn = ( SMuteData * )i_data;
	XUID xuid;
	xuid.qwUserID = dataIn->id;
	xuid.dwUserFlags = dataIn->flags;
	oVoiceManager.addToMyMuteList( xuid );
	g_console.Echo("%x id is requesting to be muted\n", dataIn->id );
	g_console.Echo( "adding %x player to my temp mute list\n", dataIn->id );
}

// ---------------------------------------------------------------------------
// --- someone is responding back to our mute information request and is
// --- telling us that we are not in their mute list, so unmute them
void CVoiceChat::CRemoteUnMute::HandleMessage(void* i_data, ActorHandle i_sender, bool i_posted )
{
	SMuteData *dataIn = ( SMuteData * )i_data;
	XUID xuid;
	xuid.qwUserID = dataIn->id;
	xuid.dwUserFlags = dataIn->flags;
	g_console.Echo("%x id is requesting to be Unmuted\n", dataIn->id );
	oVoiceManager.removeFromMyMuteList( xuid );
	g_console.Echo( "removing %x player from my temp mute list\n", dataIn->id );
}

// ---------------------------------------------------------------------------
void CVoiceChat::handleMuteUnMute( SMuteData *dataIn, bool remote )
{
	g_console.Echo("%x id is requesting mute data, my id is %x\n", dataIn->id, m_talker->d_props.d_id );

	// --- someone is asking us if he is in our mute list
	XUID xuid;
	xuid.qwUserID = dataIn->id;
	xuid.dwUserFlags = dataIn->flags;
	if( oVoiceManager.inMyMasterMuteList( xuid ) )
	{
		g_console.Echo( "This player is in my Master mute list...Requesting him to mute me\n" );
		SMuteData dataOut;
		dataOut.id		= m_talker->d_props.d_id;
		dataOut.flags	= m_talker->d_props.d_userFlags;
		dataOut.host	= g_session.GetLocalHost();
		ASSERT( dataOut.host );
		if( dataOut.host )
		{
			// --- ok everything is valid so send a message back to the sender with our id letting him know he needs to mute us
			if( remote )
				g_messageDispatcher.SendMessageToHost( REMOTE_MUTE, &dataOut, INVALID_OBJECT_HANDLE, dataIn->host );
			else
				g_messageDispatcher.SendMessageToHost( MUTE, &dataOut, INVALID_OBJECT_HANDLE, dataIn->host );
		}
	}
	else
	{
		g_console.Echo( "This player is not in my master mute list...Requesting him to unmute me\n" );
		SMuteData dataOut;
		dataOut.id		= m_talker->d_props.d_id;
		dataOut.flags	= m_talker->d_props.d_userFlags;
		dataOut.host	= g_session.GetLocalHost();
		ASSERT( dataOut.host );
		if( dataOut.host )
		{
			if( remote )
				g_messageDispatcher.SendMessageToHost( REMOTE_UNMUTE, &dataOut, INVALID_OBJECT_HANDLE, dataIn->host );
			else
				g_messageDispatcher.SendMessageToHost( UNMUTE, &dataOut, INVALID_OBJECT_HANDLE, dataIn->host );
		}
	}
}