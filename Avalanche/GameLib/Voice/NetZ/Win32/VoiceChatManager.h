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

#ifndef _VoiceChatManager_H_
#define _VoiceChatManager_H_

#include <netz.h>
#include <extensions/voicechat/voicechatext.h>


BEGIN_NAMESPACE_QUAZAL;

// This class wraps all the calls related to the VoiceChatExtension. It is platform
// independant and has references to Win32.MFC.

class VoiceChatManager: public RootObject {
public:
	VoiceChatManager();
	~VoiceChatManager();

	qBool discoverChannel( int channelNo );
	qBool InitializeChannel(qChar *szChannelName, qUnsignedInt uiNbStreams, 
							VoiceChannel::EventHandler *pEventHandler);
	qBool InitializeTalker(DOHandle dohMember, SourceDevice *pMicrophone);
	void StartTalking();
	void StopTalking();
	qBool IsTalking();
	void Process();
	void Teardown();
	ChannelMemberSink *GetMemberSink() { return m_pMemberSink; }

	void SetCodecID( CodecID idCodec ) { m_idCodec = idCodec; }
	void SetPacketSize( qUnsignedInt uiPacketSize ) { m_uiPacketSize = uiPacketSize; }

	VoiceChannel::EventHandler 	*GetEventHandler() { return m_pEventHandler; }
	VoiceChannel	*GetChannel() const { return m_pChannel; }

private:
	qByte 							*m_pPCMData;
	ChannelMemberSink 			*m_pMemberSink;
	VoiceChannel					*m_pChannel;
	SourceDevice					*m_pMicrophone;
	VoiceChannel::EventHandler	*m_pEventHandler;
	qUnsignedInt					m_uiPacketSize;
	CodecID							m_idCodec;
};

END_NAMESPACE_QUAZAL;

#endif

