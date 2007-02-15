#include "PS2VoiceProvider.hpp"
#include <extensions/avstreams/codecs/lpc10/lpc10codec.h>
#include <extensions/avstreams/codecs/openlpc/openlpccodec.h>
#include <extensions/avstreams/codecs/gsm/gsmcodec.h>
#include <extensions/voicechat/voicechatext.h>
#include "../Participant.h"
#include "net_main.h"

// NOTE: If you change this, make sure to change the definition in net_main.h
#define GSM		CodecID(0)

PS2VoiceProvider::PS2VoiceProvider()
{
    _participantPtr = NULL;
    _chatManagerPtr = NULL;
    _microphonePtr = NULL;
    _channelInitialized = false;
    _talkerInitialized = false;
    _channelJoined = false;
}

PS2VoiceProvider::~PS2VoiceProvider()
{
    shutdown();
}

/*virtual*/ bool PS2VoiceProvider::start()
{
    String strParticipantName("Player");  // TODO: Set this to the player's actual name

    // Already started, bail
    if (_chatManagerPtr) return true;

    // Create the participant object
    _participantPtr = Participant::Create();
	_participantPtr->SetName(strParticipantName);
	_participantPtr->Publish();

    // Create a VoiceChatManager.  This class wraps all the calls related to 
	// the VoiceChatExtension.  This VoiceChatManager is platform independant.
	_chatManagerPtr = new VoiceChatManager();

    // Initialize the Channel.  If this station is the Session host, this method
	// creates a VoiceChannel.  If it is not the Sesison host this method waits 
	// until the VoiceChannel has been discovered.
	_chatManagerPtr->SetCodecID(GSM);
	_channelInitialized = _chatManagerPtr->InitializeChannel(_T("Gunfighter Channel"), 2, this);

    // Create a PS2LogitechSource that will read from the headset.  We initialize
	// it with the Channel AudioStreamFormat we retrieved from the manager.
	_microphonePtr = new PS2LogitechSource(PXNetPlay::get_instance()->get_codec_ptr()->GetFormat());

    // No microphone available: abort voice startup
    if (!_microphonePtr)
    {
        _participantPtr->DeleteMainRef();
        _participantPtr = NULL;

        delete _chatManagerPtr;
        _chatManagerPtr = NULL;

        _channelInitialized = false;
    }

    // Finish initializing the channel if it was created.
    if (_channelInitialized)
    {
        FinishChannelInit();
    }

    return true;
}

/*virtual*/ void PS2VoiceProvider::shutdown()
{
    _channelInitialized = false;
    _talkerInitialized = false;
    _channelJoined = false;

    if (_participantPtr)
    {
        _participantPtr->DeleteMainRef();
        _participantPtr = NULL;
    }

    if (_chatManagerPtr)
    {
        delete _chatManagerPtr;
        _chatManagerPtr = NULL;
    }

    if (_microphonePtr)
    {
        delete _microphonePtr;
        _microphonePtr = NULL;
    }
}

/*virtual*/ void PS2VoiceProvider::update()
{
    // Can't go here until we've initialized
    if (!_chatManagerPtr) return;

    // Give the networking threads some time to run
    Platform::Sleep(1);

    if (_chatManagerPtr->GetChannel() && !_channelInitialized)
    {
        printf("Initializing channel...\n");

        FinishChannelInit();

        _channelInitialized = true;

        return;
    }

    if (!_talkerInitialized && _channelInitialized)
    {
        printf("Initializing talker...\n");
        // Initalize the Talker.  This will join the VoiceChannel.  We are now
	    // ready to pop the dialog and let the participant chat.
	    _channelJoined = _chatManagerPtr->InitializeTalker(_participantPtr->GetHandle(), _microphonePtr);
        _talkerInitialized = true;
    }

    // See if the channel's been joined yet
    if (!_channelJoined)
    {
        _channelJoined = _chatManagerPtr->IsJoined();
        printf("-");
    }

    // If we're all initialized, and 2 people are in the channel and we're not talking,
    // start talking
    if (_channelJoined && _talkerInitialized && _channelInitialized && !_chatManagerPtr->IsTalking() &&
        _chatManagerPtr->GetChannel()->GetMemberList()->size() == 2)
    {
        _chatManagerPtr->StartTalking();
    }

    // If Process() returns false, the microphone didn't open and we should bail
    if (!_chatManagerPtr->Process())
    {
        shutdown();
    }
}

/*virtual*/ bool PS2VoiceProvider::started()
{
    return true;
}

/*virtual*/ void PS2VoiceProvider::StreamOpened(StreamID idStream, DOHandle dohMember)
{
    printf("Stream %d opened by %x\n", idStream, dohMember.GetValue());
    DemuxEventHandler::StreamOpened(idStream, dohMember);

    // Make sure we're talking
    /*if (_chatManagerPtr)
    {
        if (!_chatManagerPtr->IsTalking())
        {
            _chatManagerPtr->StartTalking();
        }
    }*/
}

/*virtual*/ void PS2VoiceProvider::StreamClosed(StreamID idStream, DOHandle dohMember)
{
    printf("Stream %d closed by %x\n", idStream, dohMember.GetValue());
	DemuxEventHandler::StreamClosed(idStream, dohMember);
}

/*virtual*/ void PS2VoiceProvider::MembershipChanged()
{
    if (!_chatManagerPtr) return;

    printf("Membership Changed\n");
	if (_chatManagerPtr->GetChannel() != NULL) {
		_chatManagerPtr->GetChannel()->TraceMembers(TRACE_ALWAYS);

        printf("%d members.\n", _chatManagerPtr->GetChannel()->GetMemberList()->size());

        // Make sure if everyone's on the channel that we're talking on it
        /*if (_chatManagerPtr)
        {
            if (_chatManagerPtr->GetChannel()->GetMemberList()->size() == 2 && !_chatManagerPtr->IsTalking())
            {
                printf("Start with the talking, please.\n");
                _chatManagerPtr->StartTalking();
            }
        }*/
	}
}

void PS2VoiceProvider::FinishChannelInit()
{
    if (!_chatManagerPtr) return;

    // Create as many SinkDevice objects as there are Streams on this VoiceChannel.  This is 
	// necessary since we do not do any voice mixing and multiple streams can be opened at the 
	// same time.  We simply assign a DSinkDevice to each created stream of the VoiceChannel.
	//  
	StreamID i = 0;

	for (i = 0; i < _chatManagerPtr->GetChannel()->GetNbStreams(); i++)
    {
		// Each PS2LogitechSink has to be created with the format of the Channel.
		PS2LogitechSink *pStreamSink = new PS2LogitechSink(PXNetPlay::get_instance()->get_codec_ptr()->GetFormat());

		// We register it to our DemuxEventHandler.  The DemuxEventHandler is an EventHandler that 
		// simply takes the Channel Events StreamOpened/PacketReceived/StreamClosed and calls 
		// Open/Write/Close on the Stream.
		((DemuxEventHandler*)_chatManagerPtr->GetEventHandler())->SetStreamSink(i, pStreamSink);
	}

    printf("Channel initialized with %d streams\n", _chatManagerPtr->GetChannel()->GetNbStreams());
}