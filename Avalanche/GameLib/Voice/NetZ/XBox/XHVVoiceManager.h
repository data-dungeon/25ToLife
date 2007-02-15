//-----------------------------------------------------------------------------
// File: XHVVoiceManager.h
//
// Desc: Wraps the XHV voice engine and provides a simple interface to the 
//          game
//
// Hist: 05.06.03 - New for the June 2003 XDK
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _XHVVOICEMANAGER_H_
#define _XHVVOICEMANAGER_H_

#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"
#include <xonline.h>
#include <xhv.h>
#include "voice/NetZ/XBox/MutelistManager.h"

//-----------------------------------------------------------------------------
// Name: struct PlayerVoiceInfo
// Desc: Used to keep track of voice-related details about each player
//-----------------------------------------------------------------------------
struct PlayerVoiceInfo
{
	XUID                    xuid;
	XHV_PLAYBACK_PRIORITY   priority[4];
	BOOL                    bMuted[4];
	BOOL                    bRemoteMuted[4];
};

class CXHVVoiceManager
{
public:
	CXHVVoiceManager();
	~CXHVVoiceManager();

	HRESULT Initialize( LPDIRECTSOUND8 pDSound, 
		XHV_RUNTIME_PARAMS* pXHVParams, 
		ITitleXHV* pTitleXHV );
	HRESULT Shutdown();

	// These functions are simple passthrough functions to m_pXHVEngine:
	HRESULT DoWork();

	HRESULT SetCallbackInterface( ITitleXHV* pTitleXHV )
	{ return m_pXHVEngine->SetCallbackInterface( pTitleXHV ); }
	HRESULT SetVoiceMask( DWORD dwPort, const XHV_VOICE_MASK* pVoiceMask )
	{ return m_pXHVEngine->SetVoiceMask( dwPort, pVoiceMask ); }
	HRESULT GetLocalTalkerStatus( DWORD dwPort, XHV_LOCAL_TALKER_STATUS* pLocalTalkerStatus )
	{ return m_pXHVEngine->GetLocalTalkerStatus( dwPort, pLocalTalkerStatus ); }
	HRESULT SetMaxPlaybackStreamsCount( DWORD dwStreamsCount )
	{ return m_pXHVEngine->SetMaxPlaybackStreamsCount( dwStreamsCount ); }
	HRESULT SubmitIncomingVoicePacket( XUID xuidRemoteTalker, VOID* pvData, DWORD dwSize )
	{ return m_pXHVEngine->SubmitIncomingVoicePacket( xuidRemoteTalker, pvData, dwSize ); }
	HRESULT SetProcessingMode( DWORD dwPort, XHV_PROCESSING_MODE processingMode )
	{ return m_pXHVEngine->SetProcessingMode( dwPort, processingMode ); }
	HRESULT EnableProcessingMode( XHV_PROCESSING_MODE processingMode )
	{ return m_pXHVEngine->EnableProcessingMode( processingMode ); }


	// These functions wrap handling local and remote talkers, so that we can 
	// manage the list of PlayerVoiceInfo structs
	HRESULT RegisterRemoteTalker( XUID xuidRemoteTalker );
	HRESULT UnregisterRemoteTalker( XUID xuidRemoteTalker );
	HRESULT ClearRemoteTalkers();
	HRESULT SetRemoteTalkerPriority( XUID xuidRemoteTalker, DWORD dwPort, XHV_PLAYBACK_PRIORITY priority );
	HRESULT RegisterLocalTalker( DWORD dwPort );
	HRESULT UnregisterLocalTalker( DWORD dwPort );

	// These functions handle higher-level logical operations such as
	// muting and voice-through-speaker control
	HRESULT SetVoiceThroughSpeakers( DWORD dwPort, BOOL bVoiceThroughSpeakers );
	HRESULT SetVoiceSpeakerVolume( FLOAT fVolume, DWORD dwFXIndex );
	HRESULT SetMute( XUID xuidRemoteTalker, DWORD dwPort, BOOL bMuted );
	HRESULT SetRemoteMute( XUID xuidRemoteTalker, DWORD dwPort, BOOL bRemoteMuted );
	BOOL    IsTalking( XUID xuidRemoteTalker, DWORD dwPort );
	HRESULT GetMuteList( DWORD dwPort );
	HRESULT UpdateMuteList( );
	PXHVENGINE GetXHVEnginePtr() const { return m_pXHVEngine; }
	BOOL		inMyMuteList( XUID xuid );
	BOOL		inMyMasterMuteList( XUID xuid );
	void		addToMyMuteList( XUID xuid );
	void		removeFromMyMuteList( XUID xuid );
	void 		MutelistSuspend() { m_muteListManager.Suspend(); }
	void		MutelistResume() { m_muteListManager.Resume(); }


protected:
	HRESULT FindPlayerVoiceInfo( XUID xuidRemoteTalker, DWORD* pdwEntry );
	HRESULT RecalculatePriorities( PlayerVoiceInfo* pInfo );

	LPDIRECTSOUND8      m_pDSound;
	PXHVENGINE          m_pXHVEngine;
	DWORD               m_dwMaxRemoteTalkers;
	DWORD               m_dwNumRemoteTalkers;
	PlayerVoiceInfo*    m_pPlayerVoiceInfo;

	CMutelistManager		m_muteListManager;
	BOOL                m_bEnabledLocalTalkers[ XGetPortCount() ];
	BOOL                m_bVoiceThroughSpeakers[ XGetPortCount() ];
	BOOL					  m_LiveLoggedOn;
};

#endif // _XHVVOICEMANAGER_H_
