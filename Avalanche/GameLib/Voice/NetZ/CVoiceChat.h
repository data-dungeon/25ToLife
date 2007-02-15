// ----------------------------------------------------------------------------
// File				-> CVoiceChat.h
// Created			-> April 13, 2004
// Author			-> Dwain Skinner
// Description		-> NetZ Voice Chat integration class
// ----------------------------------------------------------------------------


#ifndef __CVOICECHAT__H
#define __CVOICECHAT__H

#include "Network/NetZ/DOHost.h"
#include "Network/NetZ/DOTalker.h"
#include "platform/ctroller.h"

class CHost;
class DOTalker;

#define REQUEST_MUTING "RequestMuting"
#define MUTE "RequestMute"
#define UNMUTE "RequestUnMute"
#define REMOTE_MUTE "RemoteMute"
#define REMOTE_UNMUTE "RemoteUnMute"

struct SMuteData
{
	u64 id;
	u32 flags;
	CHost *host;
};

class CVoiceChat
{

public:
	CVoiceChat													( );
	~CVoiceChat													( );

	enum eMultiPlayType
	{
		ONLINE_PLAY = 0,
		LAN_PLAY,
	};

	// --- available codecs during the game
	enum eChatCodecs
	{
		CHAT_CODEC_GSM,
		CHAT_CODEC_OPENLPC,
		CHAT_CODEC_NELLYMOSER,
		CHAT_NUM_CODECS
	};


	enum eChatChannels
	{
		CHAT_CHANNEL_ALL = 0,
		CHAT_CHANNEL_TEAM1,
		CHAT_CHANNEL_TEAM2,
		MAX_CHAT_CHANNELS
	};

	enum eChatState
	{
		STATE_CHAT_UNINITIALIZED = 0,
		STATE_CHAT_NONE,
		STATE_CHAT_TALKING,
		STATE_CHAT_DENY,
		STATE_CHAT_LISTEN,
	};

	// --- maximum amount of time someone can stay on the mic
	#define MAX_CHAT_TIME ( 1000 * 10 )

	// --- minimum amount of time someone must wait before they can use the chat again
	#define MIN_CHAT_WAIT ( 1000 * 2 )
	
	const bool createCodecs									( void );
	const bool create											( DOTalker* talker, CSession &session );
	const bool update											( );
	const void tearDown										( void );
	const void enable											( bool trueOrFalse );
	const void setListen										( bool trueOrFalse ) { m_listen = trueOrFalse; }
	const void setMemberShipChanged						( bool trueOrFalse ) { m_memberShipChanged = trueOrFalse; }
	static CHost* GetSpeakerHost();
	inline DOHandle getWhosTalking						( void ) const;
	inline void setWhosTalking								( DOHandle talker );
	inline const bool amITalking							( void ) const;
	const bool isPlayerTalking								( DOTalker *talker );
	DOTalker *getLocalTalker								( void ) { return m_talker; }
	void setTalkTimer											( u32 time ) { m_voiceLastUsedTime = time; }
	u32 getTalkTimer											( void ) { return m_voiceLastUsedTime; }

	void enableChatChannel									( eChatChannels channel, bool trueOrFalse );
	inline const bool isChatEnabled						( void ) const;
	inline const bool allJoined							( void ) const;
	inline const bool isChatChannelEnabled				( int chan ) const;
	inline eChatState getState								( void ) const { return m_chatState; }
	inline const bool isVoiceEnabled						( void ) const { return m_enabled; }
	const void enableVoiceMasking							( bool trueOrFalse, int maskID = 0 );
	inline const bool isVoiceMaskingEnabled			( void ) const { return( m_voiceMaskEnable ); }
	const void updateVoiceMasking							( int streamIndex, float a, float b, float c, float d, float e, float f, float g, int env );
	inline const void setVoiceMaskParms					( float pitchWarp, float pitchOffset, float spectrumWarp, float spectrumOffset, float spectrumLedge, float robot, float whisper, short environment );
	inline const void getVoiceMaskParms					( float *pitchWarp, float *pitchOffset, float *spectrumWarp, float * spectrumOffset, float *spectrumLedge, float *robot, float *whisper, short *environment );

	// --- methods that allow the headset to be used outside of a networked game
	// --- these should never be called when there is a real networked game or lobby running
	const bool openHeadSet									( void );
	const bool updateHeadSet								( void );
	const bool closeHeadSet									( void );

	// --- platform specific create and shutdown
	bool createPlatformSpecific							( void );
	bool releasePlatformSpecific							( void );
	void *getPlatformVoiceManager							( void );
	inline void setMultiPlayMode							( eMultiPlayType type );
	void postNetZTearDown									( void );
	void refreshPlatformData								( void );

	// --- voice output redirection override
	void channelVoiceThruSpeakers							( bool trueOrFalse );
	void setOutputVolume										( float volume );
	float getOutputVolume									( ) { return m_volume; }

	// Set the key to watch
	static void SetPushToTalk								( ControllerButton button ) { s_pushToTalkButton = button; }
	static ControllerButton GetPushToTalk				( void ) { return s_pushToTalkButton; }
	
	// --- muting support
	void updateMuting( void );
   bool isPlayerMuted( u64 id, u32 flags );
	u64 getUserId( void )									{ return m_userId; }
	u32 getUserFlags( void )								{ return m_userFlags; }
	void handleMuteUnMute( SMuteData *dataIn, bool remote );

	// --- banning support
	bool isVoiceChatBanned( void )						{ return m_voiceBanned; }
   
private:
	struct chatChannelDef
	{
		void				*pChatManager;
		void				*pEventHandler;
		bool				discovered;
		bool				enabled;
		bool				tryingToChat;
		bool				chatting;
		bool				listening;
		bool				membershipChanged;
		bool				created;
		bool				queuedDisable;
		bool				queuedEnable;
		bool				openPending;
	};

	CMessageOwner				m_messageOwner;
	REF_SEND_MESSAGE_HANDLER(CRequestMuting, CVoiceChat) m_RequestMutingHandler;
	REF_SEND_MESSAGE_HANDLER(CMute, CVoiceChat) m_MuteHandler;
	REF_SEND_MESSAGE_HANDLER(CUnMute, CVoiceChat) m_UnMuteHandler;
	REF_SEND_MESSAGE_HANDLER(CRemoteMute, CVoiceChat) m_RemoteMuteHandler;
	REF_SEND_MESSAGE_HANDLER(CRemoteUnMute, CVoiceChat) m_RemoteUnMuteHandler;

	int															m_teamID;
	chatChannelDef												m_chatChannels[ MAX_CHAT_CHANNELS ];
	bool															m_deviceExists;
	bool															m_enabled;
	bool															m_created;
	bool															m_listen;
	bool															m_memberShipChanged;
	void															*m_pCodecs[ CHAT_NUM_CODECS ];
	void															*m_pActiveCodec;
	bool															m_voiceMaskEnable;
	int															m_voiceMaskID;
	int															m_numStreams[ MAX_CHAT_CHANNELS ];
	float															m_pitchWarp;
	float															m_pitchOffset;
	float															m_spectrumWarp;
	float															m_spectrumOffset;
	float															m_spectrumLedge;
	float															m_robot;
	float															m_whisper;
	short															m_environment;
	DOTalker*													m_talker;
	eChatState													m_chatState;
	eMultiPlayType												m_multiPlayMode;
	DOHandle														m_talkingTalker;
	bool															m_voiceRedirected;
	bool															m_headsetConnected;
	float															m_volume;
	bool															m_voiceBanned;
	static ControllerButton									s_pushToTalkButton;
	u64															m_userId;
	u32															m_voiceLastUsedTime;
	u32															m_userFlags;
};


// ----------------------------------------------------------------------------
inline const bool CVoiceChat::isChatEnabled( void ) const
{
	for( int i = 0; i < MAX_CHAT_CHANNELS; i++ )
	{
		if( m_chatChannels[ i ].enabled ) return( true );
	}
	return( false );
}

// ----------------------------------------------------------------------------
inline const bool CVoiceChat::allJoined( void ) const
{
	for( int i = 0; i < MAX_CHAT_CHANNELS; i++ )
	{
		if( !m_chatChannels[ i ].discovered && m_chatChannels[ i ].enabled ) return( false );
	}
	return( true );
}

// ----------------------------------------------------------------------------
inline const bool CVoiceChat::isChatChannelEnabled( int chan ) const
{
	ASSERTS( chan < MAX_CHAT_CHANNELS, "illegal chat channel" );
	return( m_chatChannels[ chan ].enabled );
}

// ----------------------------------------------------------------------------
inline const bool CVoiceChat::amITalking( void ) const
{
	for( int i = 0; i < MAX_CHAT_CHANNELS; i++ )
	{
		if( m_chatChannels[ i ].discovered && m_chatChannels[ i ].enabled && m_chatChannels[ i ].chatting ) return( true );
	}
	return( true );
}

inline const void CVoiceChat::setVoiceMaskParms( float pitchWarp, float pitchOffset, float spectrumWarp, float spectrumOffset, float spectrumLedge, float robot, float whisper, short environment )
{
	m_pitchWarp			= pitchWarp;
	m_pitchOffset		= pitchOffset;
	m_spectrumWarp		= spectrumWarp;
	m_spectrumOffset	= spectrumOffset;
	m_spectrumLedge	= spectrumLedge;
	m_robot				= robot;
	m_whisper			= whisper;
	m_environment		= environment;
}

inline const void CVoiceChat::getVoiceMaskParms( float *pitchWarp, float *pitchOffset, float *spectrumWarp, float * spectrumOffset, float *spectrumLedge, float *robot, float *whisper, short *environment )
{
	*pitchWarp			= m_pitchWarp;
	*pitchOffset		= m_pitchOffset;
	*spectrumWarp		= m_spectrumWarp;
	*spectrumOffset	= m_spectrumOffset;
	*spectrumLedge		= m_spectrumLedge;
	*robot				= m_robot;
	*whisper				= m_whisper;
	*environment		= m_environment;
}

inline void CVoiceChat::setMultiPlayMode( eMultiPlayType type )
{
	m_multiPlayMode = type;
}

inline DOHandle CVoiceChat::getWhosTalking( void ) const
{
	return( m_talkingTalker );
}

inline void CVoiceChat::setWhosTalking( DOHandle talker )
{
	m_talkingTalker = talker;
}

extern CVoiceChat g_CVoiceChat;
#endif //__CVOICECHAT__H
