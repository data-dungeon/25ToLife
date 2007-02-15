// VoiceChat.cpp : Defines the class behaviours for the application.
//
#include "Voice/VoicePCH.h"
#include <NetZ.h>
#include <Utilities/CommandLine/StdCmdLine.h>
#include <Utilities/SessionEnumeration/ConsoleSessionEnumerator.h>
#include <Extensions/VoiceChat/VoiceChatExt.h>
#include <xhv.h>
#include <dsstdfx.h>
#include "XHVVoiceManager.h"

#include "Voice/NetZ/XBox/VoiceChatManager.h"
#include "Network/NetZ/Participant.h"
#include "Voice/NetZ/CVoiceChat.h"
#include "Network/NetZ/DOHost.h"

// --- create our singleton
CVoiceChat g_CVoiceChat;
CHost* speakerHost = NULL;


#define NB_STREAMS	2

qBool XHVInit( CXHVVoiceManager *pVoiceManager, ITitleXHV *pCallbackHandler )
{
	HRESULT hr = S_OK;
	LPDIRECTSOUND8	pDSound;

	// Create DirectSound in order to download our DSP image, which 
	// contains the voice mixer SRC effects
	DirectSoundCreate( NULL, &pDSound, NULL );

	// Tell DirectSound where the 3D Reverb and XTalk effects are
	DSEFFECTIMAGELOC dsImageLoc;
	dsImageLoc.dwI3DL2ReverbIndex = GraphI3DL2_I3DL2Reverb;
	dsImageLoc.dwCrosstalkIndex = GraphXTalk_XTalk;

	// Download the DSP image
	LPDSEFFECTIMAGEDESC pdsImageDesc;
//	hr = XAudioDownloadEffectsImage( "dspimage", &dsImageLoc, XAUDIO_DOWNLOADFX_XBESECTION, &pdsImageDesc );
	hr = XAudioDownloadEffectsImage( "d:\\data_xbox\\sound\\dsstdfx.bin", &dsImageLoc, XAUDIO_DOWNLOADFX_EXTERNFILE, &pdsImageDesc );



	if( FAILED( hr ) ) {
		QTRACE(TRACE_ALWAYS,(_T("Error %x while downloading Effects image"),hr));
		return false;
	}

	// Set up parameters for the Voice Chat engine
	XHV_RUNTIME_PARAMS xhvParams = {0};
	xhvParams.dwMaxLocalTalkers         = XGetPortCount();
	xhvParams.dwMaxRemoteTalkers        = NB_STREAMS;
	xhvParams.dwMaxCompressedBuffers    = 4;                    // 4 buffers per local talker
	xhvParams.dwFlags                   = 0;
	xhvParams.pEffectImageDesc          = pdsImageDesc;
	xhvParams.dwEffectsStartIndex       = GraphVoice_Voice_0;

	// The out-of-sync threshold allows the title to control how aggresive
	// XHV is about determining that we've lost synchronization with a 
	// remote talker.  A voice packet that is received significantly before or
	// after its expected time is considered to be "out-of-sync."  If this
	// many consecutive packets from a given remote talker are determined to
	// be out-of-sync, that remote talker will be reset, causing a brief pause
	// in their voice playback.
	// This number should be roughly twice as many packets as the title holds
	// in its network send buffer.
	xhvParams.dwOutOfSyncThreshold      = 10;   

	if( FAILED( pVoiceManager->Initialize(pDSound, &xhvParams, pCallbackHandler) ) ) {
		QTRACE(TRACE_ALWAYS,(_T("Error %x while creating the VoiceManager"),hr));
		return false;
	}
	return true;
}


VoiceChatManager	*g_pManager = NULL;
CXHVVoiceManager	*g_pVoiceManager = NULL;

//
// This class forwards the VoiceChannel generated events
// to the Dialog in order to update the window.
class MyEventHandler: public VoiceChannel::EventHandler {
public:
	MyEventHandler() { m_pVoiceChannel = NULL; }
	~MyEventHandler() {}

	virtual void StreamOpened(StreamID idStream, DOHandle dohMember) { 
		XUID uidUser;
		uidUser.qwUserID = dohMember;
		uidUser.dwUserFlags = 0;
		HRESULT hr = g_pVoiceManager->RegisterRemoteTalker(uidUser);
		if (FAILED(hr)) {
			QTRACE(TRACE_ALWAYS,(_T("RegisterRemoteTalker failed with error %x"),hr));
		}
		g_pVoiceManager->SetRemoteTalkerPriority(uidUser, 0, XHV_PLAYBACK_PRIORITY_MAX);
		EventHandler::StreamOpened(idStream, dohMember);
		// map the speaker to a host object
		speakerHost = NULL;
		VoiceChannelMember* member = m_pVoiceChannel->FindMember( dohMember );
		if ( member )
		{
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

	}

	virtual void StreamClosed(StreamID idStream, DOHandle dohMember) { 
		XUID uidUser;
		uidUser.qwUserID = dohMember;
		uidUser.dwUserFlags = 0;
		HRESULT hr = g_pVoiceManager->UnregisterRemoteTalker(uidUser);
		if (FAILED(hr)) {
			QTRACE(TRACE_ALWAYS,(_T("UnregisterRemoteTalker failed with error %x"),hr));
		}
		EventHandler::StreamClosed(idStream, dohMember);
	}

	virtual void StreamPacketReceived(StreamID idStream, DOHandle dohMember, 
		qByte* pbyStreamPacket, qUnsignedInt uiPacketSize) {
			XUID uidUser;
			uidUser.qwUserID = dohMember;
			uidUser.dwUserFlags = 0;
			HRESULT hr = g_pVoiceManager->SubmitIncomingVoicePacket(uidUser,pbyStreamPacket,uiPacketSize);
			if (FAILED(hr)) {
				QTRACE(TRACE_ALWAYS,(_T("SubmitIncomingVoicePacket failed with error %x"),hr));
			}
		}

		virtual void MembershipChanged() { 
			if (m_pVoiceChannel != NULL) {
				m_pVoiceChannel->TraceMembers(TRACE_ALWAYS);
			}
		}

		void SetChannel(VoiceChannel *pChannel) { m_pVoiceChannel = pChannel; }
public:
	VoiceChannel	*m_pVoiceChannel;
};


class XVHCallbackHandler: public ITitleXHV {
public:
	STDMETHODIMP CommunicatorStatusUpdate( DWORD dwPort, XHV_VOICE_COMMUNICATOR_STATUS status )
	{
		QTRACE(TRACE_ALWAYS,(_T("Communicator status changed on port %d"),dwPort));
		return S_OK;
	}

	STDMETHODIMP LocalChatDataReady( DWORD dwPort, DWORD dwSize, PVOID pData )
	{
		if (g_pManager->GetMemberSink()->IsOpen()) {
			g_pManager->GetMemberSink()->Write((qByte*)pData,dwSize);
		}
		return S_OK;
	}
};

// ----------------------------------------------------------------------------

CXHVVoiceManager	oVoiceManager;	
VoiceChatManager oManager;
MyEventHandler oEventHandler;
Participant *pParticipant = NULL;
static bool m_joined = false;
HWND m_hWnd = NULL;

// ----------------------------------------------------------------------------
CVoiceChat::CVoiceChat( void ) :
m_deviceExists( false ),
m_enabled( false ),
m_created( false ),
m_listen( false ),
m_memberShipChanged( false ),
m_teamID( -1 )
{
	memset( m_chatChannels, 0, sizeof( m_chatChannels ) );
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
			//delete ( MyDemuxEventHandler * )m_chatChannels[ i ].pEventHandler;
			//m_chatChannels[ i ].pEventHandler = NULL;
		}
	}
}

// ----------------------------------------------------------------------------
const bool CVoiceChat::create( int teamID )
{
	bool result = false;
//	return( false );

	g_pVoiceManager	= &oVoiceManager;

	XVHCallbackHandler	oHandler;
	if( !( XHVInit( &oVoiceManager,&oHandler ) ) )
	{
		return false;
	}

	m_created = true;
	String strParticipantName;
	strParticipantName.Format(_T("Player %x"),Station::GetLocalStation().GetValue());
	pParticipant = Participant::Create();
	pParticipant->SetName(strParticipantName);
	pParticipant->Publish();


	// Create a VoiceChatManager.  This class wraps all the calls related to 
	// the VoiceChatExtension.  This VoiceChatManager is platform independant 
	// and has references to Win32/MFC.
	g_pManager =  &oManager;

	// Initialize the Channel.  If this station is the Session host, this method
	// creates a VoiceChannel.  If it is not the Sesison host this method waits 
	// until the VoiceChannel has been discovered.  Only one stream is created  
	// meaning it will be "push to talk" since audio mixing is out of the scope 
	// of this sample.
	oManager.SetPacketSize(10);
	oManager.InitializeChannel(_T("Sample Channel"), 1, &oEventHandler);

	if (Session::GetRole()==SESSION_ROLE_MASTER) 
	{

		oEventHandler.SetChannel(oManager.GetChannel());
		oVoiceManager.RegisterLocalTalker(0);
		//oVoiceManager.SetVoiceThroughSpeakers(0,false);
		oVoiceManager.SetVoiceThroughSpeakers(0,true);
		oVoiceManager.SetProcessingMode(0,XHV_VOICECHAT_MODE);

		// Initalize the Talker.  This will join the VoiceChannel.  We are now
		// ready to pop the dialog and let the participant chat.
		result = oManager.InitializeTalker( pParticipant->GetHandle( ), NULL );
	}
	else
	{
		result = true;
		m_joined = false;
	}
	return result;
}

// ----------------------------------------------------------------------------
const bool CVoiceChat::update( int teamID )
{
	// --- make sure we've been created
	if( !m_created ) return( false );

	// --- make sure we're enabled
	if( !m_enabled )
	{
		if( oManager.IsTalking( ) )
		{
			oManager.StopTalking( );
		}
		return( false );
	}

	if (Session::GetRole()!=SESSION_ROLE_MASTER && !m_joined ) 
	{
		if( oManager.discoverChannel( 0 ) )
		{
			oEventHandler.SetChannel(oManager.GetChannel());
			oVoiceManager.RegisterLocalTalker(0);
			oVoiceManager.SetVoiceThroughSpeakers(0,true);
			oVoiceManager.SetProcessingMode(0,XHV_VOICECHAT_MODE);
			m_joined = oManager.InitializeTalker( pParticipant->GetHandle( ), NULL );
		}
	}

	// --- check for press to talk and try to acquire the voice channel
	ControllerInputDriver *c = g_input.GetController( 0 );
	if( c->ButtonDown( CB_O ) && !m_memberShipChanged )
	{
//		m_tryingToChat = true;
		if( !oManager.IsTalking( ) )
			oManager.StartTalking( );
	}
	else
	{
//		m_tryingToChat = false;
		if( oManager.IsTalking( ) )
			oManager.StopTalking( );
		m_memberShipChanged = false;
	}
#if 0
	ControllerInputDriver *c = g_input.GetController( 0 );
	if( c->ButtonDown( CB_O ) && !m_memberShipChanged )
	{
		m_tryingToChat = true;
		if( !oManager.IsTalking( ) )
			oManager.StartTalking( );
	}
	else
	{
		m_tryingToChat = false;
		if( oManager.IsTalking( ) )
			oManager.StopTalking( );
		m_memberShipChanged = false;
	}
	m_chatting = oManager.IsTalking( );

	// --- update messages
	if( m_chatting )
	{
		g_messageDispatcher.SendMessageToAll("ChatTalk", NULL, NULL );
	}
	else if( m_tryingToChat )
	{
		g_messageDispatcher.SendMessageToAll("ChatDeny", NULL, NULL );
	}
	else
	{
		if( m_listen )
		{
			g_messageDispatcher.SendMessageToAll("ChatListen", NULL, NULL );
		}
		else
		{
			g_messageDispatcher.SendMessageToAll("ChatNone", NULL, NULL );
		}
	}
#endif
	oVoiceManager.DoWork();
	oManager.Process(	);

	return( true );
}

// ----------------------------------------------------------------------------
const void CVoiceChat::tearDown( void )
{
	oVoiceManager.UnregisterLocalTalker(0);

	m_created = false;

	// --- make sure we stop talking just in case
	if( oManager.IsTalking( ) )
	{
		oManager.StopTalking( );
	}

	// --- delete the Main Ref
	if ( pParticipant->IsADuplicationMaster())
		pParticipant->DeleteMainRef();

	// --- clean up any memory we used
	m_joined = false;

	// --- tear down the manager
	oManager.Teardown( );

	// --- finally disable
	m_enabled = false;
}

// ----------------------------------------------------------------------------
CHost* CVoiceChat::GetSpeakerHost()
{
	return speakerHost;
}


