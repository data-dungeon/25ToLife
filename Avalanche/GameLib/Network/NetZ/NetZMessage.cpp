/*
**
**  File:   NetZMessage.cpp
**  Date:   February 25, 2004
**  By:     Bryant Collard
**  Desc:   NetZ specific message functionality.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/NetZMessage.cpp $
**  $Revision: #20 $
**  $DateTime: 2005/06/02 16:18:54 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/NetZ/NetZMessage.h"

// Engine includes
#include "Math/Vector.h"
#include "Math/Matrix.h"

// Gamelib includes
#include "stage/cactor.h"
#include "Network/Session.h"
#include "Network/Host.h"
#include "container/LinearHash.h"
#include "cmdcon/cmdcon.h"

// NetZ includes
#include <netz.h>
#include "Network/NetZ/DOMessageOwner.h"
#include "Network/NetZ/DOActor.h"
#include "Network/NetZ/DOHost.h"
#include "Network/NetZ/Translators.h"

/***************************************************************************/
/***************************************************************************/
char* MessageLogNote(CMessagePair* i_pair, void* i_data)
{
	static char msg[32];
	sprintf(msg, "Packed Size = %d bytes", i_pair->GetPackedSize(i_data));
	return(msg);
}

/***************************************************************************/
/***************************************************************************/
char* MessageLogNote(CMessageGroup* i_group, void* i_data, const char* i_host)
{
	static char msg[128];
	sprintf(msg, "Packed Size = %d bytes, host = %s",
			i_group->GetPackedSize(i_data), i_host);
	return(msg);
}

/***************************************************************************/
/***************************************************************************/
bool CNetZMessageDispatcher::InstallBasicDataTypeTranslator(const char* i_name,
		EMessageBasicDataType i_type)
{
	switch(i_type)
	{
	 case MESSAGE_VALUE:
		return(InstallTranslatorInGroup(i_name, CTranslators::PackBasicValue,
				CTranslators::UnpackBasicValue, NULL, sizeof(void*)) != NULL);
	 case MESSAGE_ACTOR_PTR:
		return(InstallTranslatorInGroup(i_name, CTranslators::PackBasicActor,
				CTranslators::UnpackBasicActor, NULL, sizeof(uint32)) != NULL);
	 case MESSAGE_ACTOR_HANDLE:
		return(InstallTranslatorInGroup(i_name, CTranslators::PackBasicActorHandle,
				CTranslators::UnpackBasicActorHandle, NULL, sizeof(uint32)) != NULL);
	 case MESSAGE_STRING:
		return(InstallTranslatorInGroup(i_name, CTranslators::PackBasicString,
				CTranslators::UnpackBasicString, CTranslators::StringSize, 0) !=
				NULL);
	 case MESSAGE_VECTOR3_PTR:
		return(InstallTranslatorInGroup(i_name, CTranslators::PackBasicVector3,
				CTranslators::UnpackBasicVector3, NULL, sizeof(Vector3)) != NULL);
	 case MESSAGE_OWNED_VECTOR3_PTR:
		return(InstallTranslatorInGroup(i_name,
				CTranslators::PackBasicOwnedVector3,
				CTranslators::UnpackBasicOwnedVector3,
				NULL, sizeof(Vector3)) != NULL);
	 case MESSAGE_OWNED_POSITION_PTR:
		return(InstallTranslatorInGroup(i_name,
				CTranslators::PackBasicOwnedPosition,
				CTranslators::UnpackBasicOwnedPosition,
				NULL, sizeof(Vector3)) != NULL);
	 case MESSAGE_MATRIX3X3_PTR:
		return(InstallTranslatorInGroup(i_name, CTranslators::PackBasicMatrix3x3,
				CTranslators::UnpackBasicMatrix3x3, NULL, sizeof(Matrix3x3)) !=
				NULL);
	 case MESSAGE_OWNED_ORIENTATION_PTR:
		return(InstallTranslatorInGroup(i_name,
				CTranslators::PackBasicOwnedOrientation,
				CTranslators::UnpackBasicOwnedOrientation,
				NULL, sizeof(Matrix3x3)) != NULL);
	 case MESSAGE_MATRIX4X4_PTR:
		return(InstallTranslatorInGroup(i_name, CTranslators::PackBasicMatrix4x4,
				CTranslators::UnpackBasicMatrix4x4, NULL, sizeof(Matrix4x4)) !=
				NULL);
	 case MESSAGE_OWNED_TRANSFORM_PTR:
		return(InstallTranslatorInGroup(i_name,
				CTranslators::PackBasicOwnedTransform,
				CTranslators::UnpackBasicOwnedTransform,
				NULL, sizeof(Matrix4x4)) != NULL);
	 case MESSAGE_CUSTOM:
		ASSERTS(false, "Must explicitly install translator for custom type");
		break;
	 default:
		ASSERTS(false, "Cannot install translator for unknown type");
		break;
	}
	return(false);
}

/**********************************************************************/
/**********************************************************************/
void CNetZMessageDispatcher::SendToMaster(void* i_data, ActorHandle i_sender,
		CMessagePair* i_pair)
{
	DOMessageOwner::Ref doRef;
	if (!g_session.IsNetworked() || !GetDORef(i_pair, doRef) ||
			doRef->IsADuplicationMaster())
		return;

#ifdef DEBUG_MESSAGE
	WriteLogEntry(MESSAGE_LOG_SEND_NETWORK, "SendToMaster", *i_pair, NULL, true,
			MessageLogNote(i_pair, i_data));
#endif
	SendMessage(i_data, i_sender, i_pair, doRef, RMCContext::CallOnMaster);
}

/**********************************************************************/
/**********************************************************************/
void CNetZMessageDispatcher::SendToDuplicas(void* i_data, ActorHandle i_sender,
		CMessagePair* i_pair)
{
	DOMessageOwner::Ref doRef;
	if (!g_session.IsNetworked() || !GetDORef(i_pair, doRef) ||
			doRef->IsADuplica())
		return;

#ifdef DEBUG_MESSAGE
	WriteLogEntry(MESSAGE_LOG_SEND_NETWORK, "SendToDuplicas", *i_pair, NULL,
			true, MessageLogNote(i_pair, i_data));
#endif
	SendMessage(i_data, i_sender, i_pair, doRef, RMCContext::CallOnDuplicas);
}

/**********************************************************************/
/**********************************************************************/
void CNetZMessageDispatcher::SendToNeighbors(void* i_data, ActorHandle i_sender,
		CMessagePair* i_pair)
{
	DOMessageOwner::Ref doRef;
	if (!g_session.IsNetworked() || !GetDORef(i_pair, doRef))
		return;

#ifdef DEBUG_MESSAGE
	WriteLogEntry(MESSAGE_LOG_SEND_NETWORK, "SendToNeighbors", *i_pair, NULL,
			true, MessageLogNote(i_pair, i_data));
#endif
	SendMessage(i_data, i_sender, i_pair, doRef,
			RMCContext::CallOnNeighbouringStations);
}

/**********************************************************************/
/**********************************************************************/
void CNetZMessageDispatcher::PostToMaster(void* i_data, ActorHandle i_sender,
		CMessagePair* i_pair)
{
	DOMessageOwner::Ref doRef;
	if (!g_session.IsNetworked() || !GetDORef(i_pair, doRef) ||
			doRef->IsADuplicationMaster())
		return;

#ifdef DEBUG_MESSAGE
	WriteLogEntry(MESSAGE_LOG_SEND_NETWORK, "PostToMaster", *i_pair, NULL, true,
			MessageLogNote(i_pair, i_data));
#endif
	PostMessage(i_data, i_sender, i_pair, doRef, RMCContext::CallOnMaster);
}

/**********************************************************************/
/**********************************************************************/
void CNetZMessageDispatcher::PostToDuplicas(void* i_data, ActorHandle i_sender,
		CMessagePair* i_pair)
{
	DOMessageOwner::Ref doRef;
	if (!g_session.IsNetworked() || !GetDORef(i_pair, doRef) ||
			doRef->IsADuplica())
		return;

#ifdef DEBUG_MESSAGE
	WriteLogEntry(MESSAGE_LOG_SEND_NETWORK, "PostToDuplicas", *i_pair, NULL,
			true, MessageLogNote(i_pair, i_data));
#endif
	PostMessage(i_data, i_sender, i_pair, doRef, RMCContext::CallOnDuplicas);
}

/**********************************************************************/
/**********************************************************************/
void CNetZMessageDispatcher::PostToNeighbors(void* i_data, ActorHandle i_sender,
		CMessagePair* i_pair)
{
	DOMessageOwner::Ref doRef;
	if (!g_session.IsNetworked() || !GetDORef(i_pair, doRef))
		return;

#ifdef DEBUG_MESSAGE
	WriteLogEntry(MESSAGE_LOG_SEND_NETWORK, "PostToNeighbors", *i_pair, NULL,
			true, MessageLogNote(i_pair, i_data));
#endif
	PostMessage(i_data, i_sender, i_pair, doRef,
			RMCContext::CallOnNeighbouringStations);
}

/**********************************************************************/
/**********************************************************************/
bool CNetZMessageDispatcher::GetDORef(CMessagePair* i_pair,
		DOMessageOwner::Ref &o_doRef)
{
	DOHandle doHandle = (qUnsignedInt32)i_pair->GetSessionID();
	if ((doHandle == INVALID_DOHANDLE) ||
			!doHandle.IsAKindOf(DOCLASSID(DOMessageOwner)))
		return(false);

	o_doRef = doHandle;
	return(o_doRef.IsValid());
}

/**********************************************************************/
/**********************************************************************/
void CNetZMessageDispatcher::SendMessage(void* i_data, ActorHandle i_sender,
		CMessagePair* i_pair, DOMessageOwner::Ref &i_doRef,
		qUnsignedInt32 i_target)
{
	RMCContext context;
	context.SetFlag(RMCContext::OneWayCall | RMCContext::SendUnreliableMessage |
			i_target);

	DOHandle doSender;
	CActor* actor = CActor::FromHandle(i_sender);
	if (actor != NULL)
	{
		DOActor* sender = (DOActor*)actor->GetNetActor();
		if (sender != NULL)
			doSender = sender->GetHandle();
	}

	if (i_data != NULL)
	{
		uint32 bufferSize = i_pair->GetPackedSize(i_data);
		ASSERT(bufferSize > 0);
		if (bufferSize > 0)
		{
			Buffer buffer(bufferSize);
			buffer.SetContentSize(bufferSize);
			if (i_pair->Pack(i_data, buffer.GetContentPtr()))
			{
				i_doRef->CallSendBuffer(&context,
						ComputeLinearHash(i_pair->GetMessageName()), buffer, doSender);
				return;
			}
		}
	}
	i_doRef->CallSendFlag(&context, ComputeLinearHash(i_pair->GetMessageName()),
			doSender);
}

/**********************************************************************/
/**********************************************************************/
void CNetZMessageDispatcher::PostMessage(void* i_data, ActorHandle i_sender,
		CMessagePair* i_pair, DOMessageOwner::Ref &i_doRef,
		qUnsignedInt32 i_target)
{
	RMCContext context;
	context.SetFlag(RMCContext::OneWayCall | RMCContext::SendUnreliableMessage |
			i_target);

	DOHandle doSender;
	CActor* actor = CActor::FromHandle(i_sender);
	if (actor != NULL)
	{
		DOActor* sender = (DOActor*)actor->GetNetActor();
		if (sender != NULL)
			doSender = sender->GetHandle();
	}

	if (i_data != NULL)
	{
		uint32 bufferSize = i_pair->GetPackedSize(i_data);
		ASSERT(bufferSize > 0);
		if (bufferSize > 0)
		{
			Buffer buffer(bufferSize);
			buffer.SetContentSize(bufferSize);
			if (i_pair->Pack(i_data, buffer.GetContentPtr()))
			{
				i_doRef->CallPostBuffer(&context,
						ComputeLinearHash(i_pair->GetMessageName()), buffer, doSender);
				return;
			}
		}
	}
	i_doRef->CallPostFlag(&context, ComputeLinearHash(i_pair->GetMessageName()),
			doSender);
}

/**********************************************************************/
/**********************************************************************/
void CNetZMessageDispatcher::SendToSession(const char* i_name, void* i_data,
		ActorHandle i_sender)
{
	// If the session is not active, don't bother.
	if (!g_session.IsNetworked())
		return;

	// Set up the context.
	RMCContext context;
	context.SetFlag(RMCContext::OneWayCall | RMCContext::CallOnMaster);

	// Find the sender handle, if any.
	DOHandle doSender;
	CActor* actor = CActor::FromHandle(i_sender);
	if (actor != NULL)
	{
		DOActor* sender = (DOActor*)actor->GetNetActor();
		if (sender != NULL)
			doSender = sender->GetHandle();
	}

	// There is data to send.
	if (i_data != NULL)
	{
		// Get a group and local host for data packing. The only reason the local
		// host is needed is for its message owner. The owner provides a
		// coordinate system to the packing function. Since all hosts use the
		// world coordinate system, the local host's message owner is as good as
		// any.
		CMessageGroup* group = GetMessageGroup(i_name);
		CHost* localHost = g_session.GetLocalHost();
		if ((group != NULL) && (localHost != NULL))
		{
			// Get the size of the data. Some size must be provided to go on.
			uint32 bufferSize = group->GetPackedSize(i_data);
			ASSERT(bufferSize > 0);
			if (bufferSize > 0)
			{
				// Pack up the data.
				Buffer buffer(bufferSize);
				buffer.SetContentSize(bufferSize);
				if (group->Pack(i_data, buffer.GetContentPtr(),
						localHost->GetMessageOwner()))
				{
					// Send a buffer to each duplica host's master.
					DOHost::SelectionIterator host(DUPLICA);
					host.GotoStart();
					while(!(host.EndReached()))
					{
#ifdef DEBUG_MESSAGE
						WriteLogEntry(MESSAGE_LOG_SEND_NETWORK,
								"SendToSession Buffer to RemoteHost", group, NULL,
								NULL, NULL, true,
								MessageLogNote(group, i_data, host->GetName()));
#endif
						host->CallSendBuffer(&context, ComputeLinearHash(i_name),
								buffer, doSender);
						host++;
					}
					return;
				}
			}
		}
	}

	// Send a flag to each duplica host's master.
	DOHost::SelectionIterator host(DUPLICA);
	host.GotoStart();
	while(!(host.EndReached()))
	{
#ifdef DEBUG_MESSAGE
		WriteLogEntry(MESSAGE_LOG_SEND_NETWORK,
				"SendToSession Flag to RemoteHost", GetMessageGroup(i_name),
				NULL, NULL, NULL, true, host->GetName());
#endif
		host->CallSendFlag(&context, ComputeLinearHash(i_name), doSender);
		host++;
	}
}

/**********************************************************************/
/**********************************************************************/
void CNetZMessageDispatcher::SendToHost(const char* i_name, void* i_data,
		ActorHandle i_sender, CHost* i_host)
{
	// Validate arguments.
	ASSERT_PTR(i_host);

	// If the session is not active, don't bother.
	if (!g_session.IsNetworked())
		return;

	// Set up the context.
	RMCContext context;
	context.SetFlag(RMCContext::OneWayCall | RMCContext::CallOnMaster);

	// Find the sender handle, if any.
	DOHandle doSender;
	CActor* actor = CActor::FromHandle(i_sender);
	if (actor != NULL)
	{
		DOActor* sender = (DOActor*)actor->GetNetActor();
		if (sender != NULL)
			doSender = sender->GetHandle();
	}

	// There is data to send.
	if (i_data != NULL)
	{
		// Get a group for data packing.
		CMessageGroup* group = GetMessageGroup(i_name);
		if (group != NULL)
		{
			// Get the size of the data. Some size must be provided to go on.
			uint32 bufferSize = group->GetPackedSize(i_data);
			ASSERT(bufferSize > 0);
			if (bufferSize > 0)
			{
				// Pack up the data.
				Buffer buffer(bufferSize);
				buffer.SetContentSize(bufferSize);
				if (group->Pack(i_data, buffer.GetContentPtr(),
						i_host->GetMessageOwner()))
				{
#ifdef DEBUG_MESSAGE
					WriteLogEntry(MESSAGE_LOG_SEND_NETWORK, "SendToHost Buffer",
							group, NULL, NULL, NULL, true,
							MessageLogNote(group, i_data, i_host->GetName()));
#endif
					((DOHost*)i_host)->CallSendBuffer(&context,
							ComputeLinearHash(i_name), buffer, doSender);
					return;
				}
			}
		}
	}

	// Send a flag to the host's master.
#ifdef DEBUG_MESSAGE
	WriteLogEntry(MESSAGE_LOG_SEND_NETWORK, "SendToHost Flag",
			GetMessageGroup(i_name), NULL, NULL, NULL, true, i_host->GetName());
#endif
	((DOHost*)i_host)->CallSendFlag(&context, ComputeLinearHash(i_name),
			doSender);
}

/**********************************************************************/
/**********************************************************************/
void CNetZMessageDispatcher::PostToSession(const char* i_name, void* i_data,
		ActorHandle i_sender)
{
	// If the session is not active, don't bother.
	if (!g_session.IsNetworked())
		return;

	// Set up the context.
	RMCContext context;
	context.SetFlag(RMCContext::OneWayCall | RMCContext::CallOnMaster);

	// Find the sender handle, if any.
	DOHandle doSender;
	CActor* actor = CActor::FromHandle(i_sender);
	if (actor != NULL)
	{
		DOActor* sender = (DOActor*)actor->GetNetActor();
		if (sender != NULL)
			doSender = sender->GetHandle();
	}

	// There is data to post.
	if (i_data != NULL)
	{
		// Get a group and local host for data packing. The only reason the local
		// host is needed is for its message owner. The owner provides a
		// coordinate system to the packing function. Since all hosts use the
		// world coordinate system, the local host's message owner is as good as
		// any.
		CMessageGroup* group = GetMessageGroup(i_name);
		CHost* localHost = g_session.GetLocalHost();
		if ((group != NULL) && (localHost != NULL))
		{
			// Get the size of the data. Some size must be provided to go on.
			uint32 bufferSize = group->GetPackedSize(i_data);
			ASSERT(bufferSize > 0);
			if (bufferSize > 0)
			{
				// Pack up the data.
				Buffer buffer(bufferSize);
				buffer.SetContentSize(bufferSize);
				if (group->Pack(i_data, buffer.GetContentPtr(),
						localHost->GetMessageOwner()))
				{
					// Post a buffer to each duplica host's master.
					DOHost::SelectionIterator host(DUPLICA);
					host.GotoStart();
					while(!(host.EndReached()))
					{
#ifdef DEBUG_MESSAGE
						WriteLogEntry(MESSAGE_LOG_SEND_NETWORK,
								"PostToSession Buffer to RemoteHost", group, NULL,
								NULL, NULL, true,
								MessageLogNote(group, i_data, host->GetName()));
#endif
						host->CallPostBuffer(&context, ComputeLinearHash(i_name),
								buffer, doSender);
						host++;
					}
					return;
				}
			}
		}
	}

	// Post a flag to each duplica host's master.
	DOHost::SelectionIterator host(DUPLICA);
	host.GotoStart();
	while(!(host.EndReached()))
	{
#ifdef DEBUG_MESSAGE
		WriteLogEntry(MESSAGE_LOG_SEND_NETWORK,
				"PostToSession Flag to RemoteHost", GetMessageGroup(i_name),
				NULL, NULL, NULL, true, host->GetName());
#endif
		host->CallPostFlag(&context, ComputeLinearHash(i_name), doSender);
		host++;
	}
}

/**********************************************************************/
/**********************************************************************/
void CNetZMessageDispatcher::PostToHost(const char* i_name, void* i_data,
		ActorHandle i_sender, CHost* i_host)
{
	// Validate arguments.
	ASSERT_PTR(i_host);

	// If the session is not active, don't bother.
	if (!g_session.IsNetworked())
		return;

	// Set up the context.
	RMCContext context;
	context.SetFlag(RMCContext::OneWayCall | RMCContext::CallOnMaster);

	// Find the sender handle, if any.
	DOHandle doSender;
	CActor* actor = CActor::FromHandle(i_sender);
	if (actor != NULL)
	{
		DOActor* sender = (DOActor*)actor->GetNetActor();
		if (sender != NULL)
			doSender = sender->GetHandle();
	}

	// There is data to post.
	if (i_data != NULL)
	{
		// Get a group for data packing.
		CMessageGroup* group = GetMessageGroup(i_name);
		if (group != NULL)
		{
			// Get the size of the data. Some size must be provided to go on.
			uint32 bufferSize = group->GetPackedSize(i_data);
			ASSERT(bufferSize > 0);
			if (bufferSize > 0)
			{
				// Pack up the data.
				Buffer buffer(bufferSize);
				buffer.SetContentSize(bufferSize);
				if (group->Pack(i_data, buffer.GetContentPtr(),
						i_host->GetMessageOwner()))
				{
#ifdef DEBUG_MESSAGE
					WriteLogEntry(MESSAGE_LOG_SEND_NETWORK, "PostToHost Buffer",
							group, NULL, NULL, NULL, true,
							MessageLogNote(group, i_data, i_host->GetName()));
#endif
					((DOHost*)i_host)->CallPostBuffer(&context,
							ComputeLinearHash(i_name), buffer, doSender);
					return;
				}
			}
		}
	}

	// Post a flag to the host's master.
#ifdef DEBUG_MESSAGE
	WriteLogEntry(MESSAGE_LOG_SEND_NETWORK, "PostToHost Flag",
			GetMessageGroup(i_name), NULL, NULL, NULL, true, i_host->GetName());
#endif
	((DOHost*)i_host)->CallPostFlag(&context, ComputeLinearHash(i_name),
			doSender);
}
