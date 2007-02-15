/**
 *  \file PS2VoiceProvider.hpp
 *  \author Chris Stark, Ritual Entertainment
 *  \brief Defines the PS2 voice provider class, which is used to provide voice functionality on PS2.
 */

#ifndef _INCLUDED_PS2_VOICE_PROVIDER_
#define _INCLUDED_PS2_VOICE_PROVIDER_

#include "osivoiceprovider.hpp"
#include "voicechatmanager.hpp"
#include <extensions/avstreams/devices/ps2logitech/ps2logitechdevices.h>
#include <extensions/avstreams/devices/ps2logitech/ps2logitechsource.h>
#include <extensions/avstreams/devices/ps2logitech/ps2logitechsink.h>
#include "Participant.h"

class PS2VoiceProvider : public OSIVoiceProvider, public DemuxEventHandler
{
    public:
        /** \name Destructor */
        //\{
        ~PS2VoiceProvider();
        //\}

        /** \name Application methods */
        //\{
        virtual bool start();
        virtual void shutdown();
        virtual void update();
        virtual bool started();
        //\}

        /** \name VoiceChannel::EventHandler methods */
        //\{
        virtual void StreamOpened(StreamID idStream, DOHandle dohMember);
        virtual void StreamClosed(StreamID idStream, DOHandle dohMember);
        virtual void MembershipChanged();
        //\}

    private:
        friend class OSIVoiceProvider;

        /** \name Constructor */
        //\{
        PS2VoiceProvider();
        //\}

        /** \name Utility methods */
        //\{
        void FinishChannelInit();
        //\}

        Participant         *_participantPtr;     /// < The participant object
        VoiceChatManager    *_chatManagerPtr;     /// < The voice chat management object
        PS2LogitechSource   *_microphonePtr;      /// < The microphone object
        bool                _channelInitialized;  /// < Has the voice channel been initialized?
        bool                _talkerInitialized;   /// < Have we initialized the talker?
        bool                _channelJoined;       /// < Have we joined the chat channel?
};

#endif // _INCLUDED_PS2_VOICE_PROVIDER_