// 
// (c) Copyright 1998-2004, Quazal Technologies Inc.
// ALL RIGHTS RESERVED
// 
// Net-Z, Eterna, Rendez-Vous, Duplicated Object and Duplication Space 
// are trademarks of Quazal.
// 
// Use of this software indicates acceptance of the End User licence
// Agreement for this product. A copy of the EULA is available from
// www.quazal.com.
//   

#include "voicechatmanager.hpp"

#if defined WIN32 || defined _XBOX
#define puts OutputDebugString
#endif

VoiceChatManager::VoiceChatManager()
{
    puts("VoiceChatManager::VoiceChatManager()\n");
	m_pPCMData = NULL;
	m_pMemberSink = NULL;
	m_pChannel = NULL;
	m_pMicrophone = NULL;
	m_uiPacketSize = 0;
	m_idCodec = INVALID_CODEC_ID;
    m_pEventHandler = NULL;
}


VoiceChatManager::~VoiceChatManager()
{
    puts("VoiceChatManager::~VoiceChatManager()\n");
}


qBool VoiceChatManager::InitializeChannel(qChar *szChannelName, qUnsignedInt uiNbStreams, 
										VoiceChannel::EventHandler *pEventHandler)
{
    puts("VoiceChatManager::InitializeChannel()\n");

	// Make sure that if there is no codec, the user has at least set the packet size.
	// If no codec is specified, then we assume that the data passed to the channel is
	// already encoded, so we need to know at least the packet size.  Otherwise, the default
	// packet size, as given by the codec's AudioStreamFormat, is used.
	if ((m_idCodec == INVALID_CODEC_ID) && (m_uiPacketSize == 0)) {
		return false;
	}

	m_pEventHandler = pEventHandler;
	if (Session::GetRole()==SESSION_ROLE_MASTER) {
		// Creates a VoiceChannel with the specified number of streams.
		m_pChannel = VoiceChannel::Create(szChannelName, (qByte)uiNbStreams);
		// This is a good time to set the EventHandler since once it's published, 
		// we will start to receive events.  If we don't want to miss any events, 
		// we need to register the EventHandler before publishing it.
		m_pChannel->SetEventHandler(pEventHandler);
		m_pChannel->SetCodecID(m_idCodec);
		if (m_uiPacketSize > 0) {
			m_pChannel->SetPacketSize(m_uiPacketSize);
		}
		// A VoiceChannel is a DuplicatedObject and therefore, needs to be published just like any other DO.
		m_pChannel->Publish();
	} else {
		// Try and get a voice channel.  If we fail here, process() will keep trying.
		if (m_pChannel == NULL) {
			VoiceChannel::SelectionIterator	itChannels;
			itChannels.GotoStart();
			if (!itChannels.EndReached()) {
				m_pChannel = *itChannels;

                // Here, we also want to set the EventHandler to get Stream Open/Write/Close events.
		        // It is a good idea to do this before the Join so that we do not miss any events.
		        m_pChannel->SetEventHandler(pEventHandler);
			}
		}
	}
	return m_pChannel != NULL;
}



qBool VoiceChatManager::InitializeTalker(DOHandle dohMember, SourceDevice *pMicrophone)
{
    puts("VoiceChatManager::InitializeTalker()\n");
	// Initialize the microphone.  The microphone will be used to poll the SourceDevice
	// for voice data.  When the participant talks, we need to periodically read the 
	// SourceDevice and write that voice data to the ChannelMemberSink.
	m_pMicrophone = pMicrophone;

	// Allocate the buffer needed to hold the voice data that we will read from the SourceDevice and 
	// write to the ChanneMemberSink.  Since both the source and the sink were initialized with the 
	// AudioStreamFormat retrieved from the VoiceChannel, the size required for our voice data buffer
	// is the PacketSize().
	m_pPCMData = new qByte[m_pChannel->GetPacketSize()];

	// This sample uses a ChannelMemberSink to deal with the VoiceChannel Join, Open, Write, Close 
	// and Leave.  ChannelMemberSink is simply a wrapper that simplifies VoiceChannel usage.
	m_pMemberSink = qNew ChannelMemberSink(m_pChannel,dohMember);
	
	return m_pMemberSink->JoinCompleted();
}

bool VoiceChatManager::IsJoined()
{
    if (!m_pMemberSink)
    {
        return false;
    }

    if (!m_pMemberSink->JoinCompleted())
    {
        return false;
    }

    return true;
}

void VoiceChatManager::StartTalking()
{
    puts("VoiceChatManager::StartTalking()\n");
	// Participant wants to start talking so open the MemberSink.  This Open
	// call initiates an open and will return immediately.  The ChannelMemberSink
	// will be opened when the call to IsOpen() succeeds.
	m_pMemberSink->Open();
}


void VoiceChatManager::StopTalking()
{
    puts("VoiceChatManager::StopTalking()\n");
	// Participant wants to start talking so close the MemberSink.
	m_pMemberSink->Close();
	if (m_pMicrophone != NULL) {
		m_pMicrophone->Close();
	}
}


qBool VoiceChatManager::IsTalking()
{
    //puts("VoiceChatManager::IsTalking()\n");
	// Check if Participant is talking by checking if its MemberSink is open
	return m_pMemberSink->IsOpen();
}

// Return false on critical error (failed to open microphone), true otherwise
bool VoiceChatManager::Process()
{
    static int DisplayProcess = 0;

    ++DisplayProcess;
    if (DisplayProcess <= 10) puts("VoiceChatManager::Process()\n");
	// This method must be called frequently.  Not only is it responsible for feeding
	// an open MemberSink with voice data by reading from the microphone SourceDevice, but 
	// it also calls Dispatch on the VoiceChannel.

    // Check and see if the voice channel has been initialized yet
    if (m_pChannel == NULL && m_pEventHandler)
    {
        // Tyr and find an active voice channel
        VoiceChannel::SelectionIterator	itChannels;
		itChannels.GotoStart();
		if (!itChannels.EndReached())
        {
			m_pChannel = *itChannels;

            // Here, we also want to set the EventHandler to get Stream Open/Write/Close events.
		    // It is a good idea to do this before the Join so that we do not miss any events.
		    m_pChannel->SetEventHandler(m_pEventHandler);
            puts("Channel created.\n");
		}
        else
        {
            // No voice channel yet, try again next frame
            return true;
        }
    }

    // No voice channel, no event handler: bail
    else if (m_pChannel == NULL)
    {
        return true;
    }

    // Make sure we've joined the voice channel before doing anything else
    if (!IsJoined())
    {
        return true;
    }

	// First, if the MemberSink is open it means that our Participant wants to talk or is 
	// currently talking.  Remember that the Open call was asynchronous.
	if (m_pMicrophone != NULL) {
		if (m_pMemberSink->IsOpen()) {
			// Open the microphone SourceDevice if necessary.
			if (!m_pMicrophone->IsOpen()) {
				if (!m_pMicrophone->Open()) {
					QTRACE(TRACE_ALWAYS,(_T("Cannot open Microphone")));
                    return false;
				}
			}
			// Read from the microphone SourceDevice and write to the ChannelMemberSink SinkDevice.
			// Make sure you read everything available.  Write will effectively send the voice data 
			// to all the VoiceChannelMembers that are not currently muted.
			while(m_pMicrophone->Read(m_pPCMData,0) == StreamingDevice::Success) {
				m_pMemberSink->Write(m_pPCMData,0);
			}
		}
	}

	Station::FlushAllBundles();

	m_pChannel->Dispatch();

    return true;
}


void VoiceChatManager::Teardown()
{
    puts("VoiceChatManager::Teardown()\n");
	// Unregister our EventHandler since we are leaving
	if (m_pChannel != NULL) {
		m_pChannel->SetEventHandler(NULL);
	}
	// Delete our ChannelMemberSink.  The class is reponsible for closing 
	// opened streams and calling Leave on the Channel.
	if (m_pMemberSink != NULL) {
		qDelete m_pMemberSink;
	}
	// Free memory we allocated for the voice data buffer.
	delete []m_pPCMData;

	// IMPORTANT:  Notice that we do not call DeleteMainRef on the VoiceChannel even though we might 
	// have created it.  This is important since we want to let our VoiceChannel live and migrate on another
	// station until the last player leaves the Session.  It will also migrate upon detection of a fault.
}
