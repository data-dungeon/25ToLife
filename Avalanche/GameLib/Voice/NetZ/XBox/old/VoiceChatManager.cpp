// ----------------------------------------------------------------------------
// File			-> VoiceChatManager.cpp
// Created		-> July 15, 2004
// Author		-> Dwain Skinner
// Description	-> simple class to manage a voice channel, based on Quazal VoiceChat sample
//
//  Copyright (c) 2004, Avalanche Software Inc.
// ----------------------------------------------------------------------------

#define __VOICECHATMANAGER__C
#include "Voice/VoicePCH.h"
#include "VoiceChatManager.h"

bool m_joined = false;

// ----------------------------------------------------------------------------
VoiceChatManager::VoiceChatManager( ) :
	m_pPCMData( NULL ),
	m_pMemberSink( NULL ),
	m_pMicrophone( NULL ),
	m_pChannel( NULL ),
	m_uiPacketSize( 0 ),
	m_idCodec( INVALID_CODEC_ID )
{
}


// ----------------------------------------------------------------------------
VoiceChatManager::~VoiceChatManager()
{
}


// ----------------------------------------------------------------------------
qBool VoiceChatManager::InitializeChannel(qChar *szChannelName, qUnsignedInt uiNbStreams, 
										VoiceChannel::EventHandler *pEventHandler)
{
	// --- Make sure that if there is no codec, the user has at least set the packet size.
	// --- If no codec is specified, then we assume that the data passed to the channel is
	// --- already encoded, so we need to know at least the packet size.  Otherwise, the default
	// --- packet size, as given by the codec's AudioStreamFormat, is used.
	if( ( m_idCodec == INVALID_CODEC_ID ) && ( m_uiPacketSize == 0 ) ) 
	{
		return false;
	}

	// --- if we're the session master create the voice channel
	m_pEventHandler = pEventHandler;
	if( Session::GetRole( ) == SESSION_ROLE_MASTER ) 
	{
		m_pChannel = VoiceChannel::Create( szChannelName, (qByte)uiNbStreams );

		// --- This is a good time to set the EventHandler since once it's published, 
		// --- we will start to receive events.  If we don't want to miss any events, 
		// --- we need to register the EventHandler before publishing it.
		m_pChannel->SetEventHandler( pEventHandler );
		m_pChannel->SetCodecID( m_idCodec );
		m_pChannel->SetJitterTolerance( 300 );
		if (m_uiPacketSize > 0) 
		{
			m_pChannel->SetPacketSize( m_uiPacketSize );
		}

		// --- A VoiceChannel needs to be published just like any other DO.
		m_pChannel->Publish();
	} 
	else 
	{
		m_pChannel = NULL;
		m_joined = false;
		return( false );
	}

	return m_pChannel != NULL;
}


// ----------------------------------------------------------------------------
qBool VoiceChatManager::discoverChannel( int channelNo )
{
	int count=0;

	// --- Try to discover a the voice channel if it has not been found yet
	if( m_pChannel == NULL )
	{
		VoiceChannel::SelectionIterator	itChannels;
		itChannels.GotoStart();
		while( !itChannels.EndReached( ) && count != channelNo )
		{
			itChannels++;
			count++;
		}
		if (!itChannels.EndReached( ) ) 
		{
			m_pChannel = *itChannels;
		}
		if( m_pChannel && count == channelNo )
			m_pChannel->SetEventHandler(m_pEventHandler);
		else
		{
			ASSERTS( false, "unable to find voice channel" );
		}
	}
	return( m_pChannel != NULL );
}


// ----------------------------------------------------------------------------
qBool VoiceChatManager::InitializeTalker( DOHandle dohMember, SourceDevice *pMicrophone )
{
	// --- only initialize the talker once
	if( m_pMicrophone ) return true;

	// --- Initialize the microphone.  The microphone will be used to poll the SourceDevice
	// --- for voice data.  When the participant talks, we need to periodically read the 
	// --- SourceDevice and write that voice data to the ChannelMemberSink.
	m_pMicrophone = pMicrophone;

	// --- Allocate the buffer needed to hold the voice data that we will read from the SourceDevice and 
	// --- write to the ChanneMemberSink.  Since both the source and the sink were initialized with the 
	// --- AudioStreamFormat retrieved from the VoiceChannel, the size required for our voice data buffer
	// --- is the PacketSize().
	m_pPCMData = new qByte[ m_pChannel->GetPacketSize( ) ];

	// --- ChannelMemberSink is simply a wrapper that simplifies VoiceChannel usage.
	m_pMemberSink = qNew ChannelMemberSink( m_pChannel, dohMember );

	// --- see if we were able to join, if not we will keep trying to join in the update function
	m_joined = m_pMemberSink->JoinCompleted( );
	return true;
}


// ----------------------------------------------------------------------------
void VoiceChatManager::StartTalking()
{
	// --- can't start talking if we have no sink
	if( m_pMemberSink == NULL ) return;

	// --- make sure we're a member of the voice channel
	if( m_pMemberSink->JoinCompleted( ) )
	{
		m_joined = true;

		// --- try to open the voice channel
		if( !m_pMemberSink->IsOpen( ) )
			m_pMemberSink->Open();

		// --- open the microphone so we can speak
		if( m_pMicrophone )
		{
			if( !m_pMicrophone->IsOpen( ) )
				m_pMicrophone->Open();
		}
	}
}


// ----------------------------------------------------------------------------
void VoiceChatManager::StopTalking()
{
	// --- can't stop talking if we have no sink
	if( m_pMemberSink == NULL ) return;

	// --- make sure we're a member of this voice channel
	if( m_pMemberSink->JoinCompleted( ) )
	{
		m_joined = true;
		 
		// --- make sure the channel was opened by us
		if( m_pMemberSink->IsOpen( ) )
		{
			// --- close the channel
			m_pMemberSink->Close( );

			// --- close the microphone
			if ( m_pMicrophone != NULL ) 
			{
				if( m_pMicrophone->IsOpen( ) )
					m_pMicrophone->Close( );
			}
		}
	}
}


// ----------------------------------------------------------------------------
qBool VoiceChatManager::IsTalking()
{
	bool result = false;

	if( m_pMemberSink == NULL ) return( false );
	
	// --- Make sure we're a member of this channel and see if the channel is open
	if( m_pMemberSink->JoinCompleted() )
	{
		m_joined = true;
		result = m_pMemberSink->IsOpen();
	}
	return( result );
}


// ----------------------------------------------------------------------------
void VoiceChatManager::Process()
{
	// --- no need to process if we have no channel
	if( m_pMemberSink == NULL ) return;

	// --- no need to process if we are not a member
	if( m_pMemberSink->JoinCompleted() )
	{
		m_joined = true;
	}
	else
	{
		return;
	}

	// -------------------------------------------------------------------------------------------
	// --- This method must be called frequently.  Not only is it responsible for feeding      ---
	// --- an open MemberSink with voice data by reading from the microphone SourceDevice, but ---
	// --- it also calls Dispatch on the VoiceChannel.                                         ---
	// -------------------------------------------------------------------------------------------

	// --- First, if the MemberSink is open it means that our Participant wants to talk or is 
	// --- currently talking.  Remember that the Open call was asynchronous.
	if( m_pMicrophone != NULL ) 
	{
		if( m_pMemberSink->IsOpen( ) ) 
		{
			// --- Open the microphone SourceDevice if necessary.
			if( !m_pMicrophone->IsOpen( ) ) 
			{
				if( !m_pMicrophone->Open( ) ) 
				{
					QTRACE(TRACE_ALWAYS,(_T("Cannot open Microphone")));
				}
			}

			// --- Read from the microphone SourceDevice and write to the ChannelMemberSink SinkDevice.
			// --- Make sure you read everything available.  Write will effectively send the voice data 
			// --- to all the VoiceChannelMembers that are not currently muted.
			while( m_pMicrophone->Read( m_pPCMData, 0 ) == StreamingDevice::Success ) 
			{
				m_pMemberSink->Write( m_pPCMData, 0 );
			}
		}
	}

	Station::FlushAllBundles();
	m_pChannel->Dispatch();
}


// ----------------------------------------------------------------------------
void VoiceChatManager::Teardown()
{
	// --- Unregister our EventHandler since we are leaving
	if( m_pChannel != NULL ) 
	{
		m_pMemberSink->Close( );
		m_pChannel->SetEventHandler( NULL );
		if( m_pChannel->IsADuplicationMaster( ) )
			m_pChannel->DeleteMainRef( );
		m_pChannel = NULL;
	}

	// --- Delete our ChannelMemberSink.  The class is responsible for closing 
	// --- opened streams and calling Leave on the Channel.
	if( m_pMemberSink != NULL ) 
	{
		qDelete m_pMemberSink;
		m_pMemberSink = NULL;
	}

	// --- Free memory we allocated for the voice data buffer.
	if( m_pPCMData )
	{
		delete []m_pPCMData;
		m_pPCMData = NULL;
	}
	m_joined = false;
	m_pMicrophone = NULL;
}
