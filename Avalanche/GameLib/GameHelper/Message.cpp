/*
**
**  File:   Message.cpp
**  Date:   February 18, 2004
**  By:     Bryant Collard
**  Desc:   A system for sending, posting, and receiving messages.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/GameHelper/Message.cpp $
**  $Revision: #31 $
**  $DateTime: 2005/08/15 12:27:12 $
**    $Author: Bryant_Collard $
**
*/

// Pre-compiled header
#include "GameHelper/GameHelperPCH.h"

// My header
#include "GameHelper/Message.h"

// System headers
#include <new.h>

// Gamelib headers
#include "stage/cactor.h"
#include "Network/Session.h"
#include "cmdcon/cmdcon.h"

// Setup the singleton
DeclareSingleton(CMessageDispatcher);

// The invalid session id depends on the network layer.
#ifdef NETZ
#include <netz.h>
#define INVALID_SESSION_ID INVALID_DO_HANDLE_VALUE
#else
#define INVALID_SESSION_ID 0
#endif

const char* CMessageDispatcher::s_activeGroup = "NONE";
const char* CMessageDispatcher::s_activeOwner = "NONE";

#ifdef DEBUG_MESSAGE
const char* CMessageDispatcher::s_debugFile = "NONE";
int CMessageDispatcher::s_debugLine = 0;
#endif

////////////////////////////// CMessageHandler //////////////////////////////

/***************************************************************************/
/***************************************************************************/
CMessageHandler::CMessageHandler()
{
	d_pair = NULL;
	d_next = NULL;

	d_rules = MESSAGE_HANDLE_IF_MASTER | MESSAGE_ROUTE_TO_MASTER;

	d_deleteWhenDefunct = false;
}

/***************************************************************************/
/***************************************************************************/
CMessageHandler::~CMessageHandler()
{
	Unregister();
}

/***************************************************************************/
/***************************************************************************/
void CMessageHandler::SetRules(uint32 i_rules)
{
	d_rules = i_rules;
#ifdef DEBUG_MESSAGE
	char notes[100];
	notes[0] = '\0';
	if ((d_rules & MESSAGE_HANDLE_IF_MASTER) != 0)
		strcat(notes, " | HANDLE_IF_MASTER");
	if ((d_rules & MESSAGE_HANDLE_IF_DUPLICA) != 0)
		strcat(notes, " | HANDLE_IF_DUPLICA");
	if ((d_rules & MESSAGE_ROUTE_TO_MASTER) != 0)
		strcat(notes, " | ROUTE_TO_MASTER");
	if ((d_rules & MESSAGE_ROUTE_TO_DUPLICAS) != 0)
		strcat(notes, " | ROUTE_TO_DUPLICAS");
	if ((d_rules & MESSAGE_ROUTE_TO_NEIGHBORS) != 0)
		strcat(notes, " | ROUTE_TO_NEIGHBORS");
	if (notes[0] == '\0')
		strcat(notes, " | DISABLE");
	if (ValidSingleton(CMessageDispatcher))
		GetSingleton(CMessageDispatcher).WriteLogEntry(MESSAGE_LOG_CONFIG,
				"SetRules", *this, true, notes + 3);
#endif
}

/***************************************************************************/
/***************************************************************************/
void CMessageHandler::DeleteWhenDefunct(bool i_deleteWhenDefunct)
{
	d_deleteWhenDefunct = i_deleteWhenDefunct;
#ifdef DEBUG_MESSAGE
	if (ValidSingleton(CMessageDispatcher))
		GetSingleton(CMessageDispatcher).WriteLogEntry(MESSAGE_LOG_CONFIG,
				"DeleteWhenDefunct", *this, true,
				d_deleteWhenDefunct ? "Set to TRUE." : "Set to FALSE.");
#endif
}

/***************************************************************************/
/***************************************************************************/
void CMessageHandler::Register(CMessageOwner &i_owner, CMessageGroup &i_group)
{
/*
 *	Make sure the handler is unregistered.
 */
	ASSERT(d_pair == NULL);
/*
 *	Try to find an existing pair for this owner and group. If found, insert this
 *	handler at its head and return.
 */
	d_pair = i_owner.d_pairHead;
	while(d_pair != NULL)
	{
		if (&d_pair->d_group == &i_group)
		{
			ASSERTS(!d_pair->d_active,
					"Registering handler with active message pair");
			d_next = d_pair->d_handlerHead;
			d_pair->d_handlerHead = this;
#ifdef DEBUG_MESSAGE
			char notes[100];
			sprintf(notes, "Add to pair. Next handler is 0x%x.", d_next);
			GetSingleton(CMessageDispatcher).WriteLogEntry(MESSAGE_LOG_CONFIG,
					"Register Handler", *this, true, notes);
#endif
			return;
		}
		d_pair = d_pair->d_ownerNext;
	}
/*
 *	Create a new pair if none was found and make this handler its handler list.
 */
	d_pair = new CMessagePair(i_owner, i_group);
	ASSERT_PTR(d_pair);
	d_next = NULL;
	d_pair->d_handlerHead = this;
/*
 *	Insert the pair at the head of the owner's pair list.
 */
	d_pair->d_ownerPrev = NULL;
	d_pair->d_ownerNext = i_owner.d_pairHead;
	if (d_pair->d_ownerNext != NULL)
	{
		ASSERTS(!d_pair->d_ownerNext->d_active,
				"Linking new message pair in owner with active pair");
		d_pair->d_ownerNext->d_ownerPrev = d_pair;
	}
	i_owner.d_pairHead = d_pair;
/*
 *	Insert the pair at the head of the group's pair list.
 */
	d_pair->d_groupPrev = NULL;
	d_pair->d_groupNext = i_group.d_pairHead;
	if (d_pair->d_groupNext != NULL)
	{
		ASSERTS(!d_pair->d_groupNext->d_active,
				"Linking new message pair in group with active pair");
		d_pair->d_groupNext->d_groupPrev = d_pair;
	}
	i_group.d_pairHead = d_pair;
#ifdef DEBUG_MESSAGE
	GetSingleton(CMessageDispatcher).WriteLogEntry(MESSAGE_LOG_MEMORY,
			"Pair Create", *this, true, "In CMessageHandler::Register()");
	GetSingleton(CMessageDispatcher).WriteLogEntry(MESSAGE_LOG_CONFIG,
			"Register Handler", *this, true, "Add to newly created pair.");
#endif
}

/***************************************************************************/
/***************************************************************************/
void CMessageHandler::Unregister(void)
{
/*
 *	Do nothing if the handler is not registered.
 */
	if (d_pair == NULL)
	{
#ifdef DEBUG_MESSAGE
		if (ValidSingleton(CMessageDispatcher))
			GetSingleton(CMessageDispatcher).WriteLogEntry(MESSAGE_LOG_CONFIG,
					"Unregister Handler", *this, false, "Already not registered.");
#endif
		return;
	}
	ASSERTS(!d_pair->d_active, "Unregistering handler in active message pair");
/*
 *	Make sure no messsages using this handler are posted.
 */
	d_pair->d_owner.RemoveFromQueue(this);
/*
 *	Remove the handler from its pair. First, deal with the case where this
 *	handler is not the first in the pair's list.
 */
	if (d_pair->d_handlerHead != this)
	{
		CMessageHandler* handler = d_pair->d_handlerHead;
		while(handler->d_next != this)
		{
			ASSERT(handler->d_next != NULL);
			handler = handler->d_next;
		}
		handler->d_next = d_next;
#ifdef DEBUG_MESSAGE
		char notes[100];
		sprintf(notes, "Remove from pair (not head). Next handler is 0x%x.",
				d_next);
		GetSingleton(CMessageDispatcher).WriteLogEntry(MESSAGE_LOG_CONFIG,
				"Unregister Handler", *this, false, notes);
#endif
	}
/*
 *	If this handler is the first in the pair's list, make the following handler
 *	the first in the pair's list.
 */
	else
	{
		d_pair->d_handlerHead = d_next;
#ifdef DEBUG_MESSAGE
		char notes[100];
		sprintf(notes, "Remove from pair (head). Next handler is 0x%x.", d_next);
		GetSingleton(CMessageDispatcher).WriteLogEntry(MESSAGE_LOG_CONFIG,
				"Unregister Handler", *this, false, notes);
#endif
/*
 *		If the pair now has no handlers, delete it.
 */
		if (d_pair->d_handlerHead == NULL)
		{
#ifdef DEBUG_MESSAGE
			GetSingleton(CMessageDispatcher).WriteLogEntry(MESSAGE_LOG_MEMORY,
					"Pair Delete", *this, false, "In CMessageHandler::Unregister().");
#endif
			delete d_pair;
		}
	}
/*
 *	Clear the pointers.
 */
	d_pair = NULL;
	d_next = NULL;
}

/***************************************************************************/
/***************************************************************************/
void CMessageHandler::QueueMessage(void* i_data, ActorHandle i_sender)
{
#ifdef DEBUG_MESSAGE
	GetSingleton(CMessageDispatcher).WriteLogEntry(MESSAGE_LOG_MEMORY,
			"PostedMessage Create", *this, true, NULL);
#endif
	CPostedMessage* message = new CPostedMessage(*this, i_sender);
	ASSERT_PTR(message);
	if (!PostData(i_data, *message))
	{
		ASSERTS(false, "Handler cannot handle posted messages");
#ifdef DEBUG_MESSAGE
		GetSingleton(CMessageDispatcher).WriteLogEntry(MESSAGE_LOG_MEMORY,
				"PostedMessage Delete", *this, true, "Failed to post.");
#endif
		delete message;
	}
}

/***************************************************************************/
/***************************************************************************/
const char* CMessageHandler::GetName(void)
{
	if (d_pair != NULL)
		return(d_pair->d_group.d_name);
	return(NULL);
}

/////////////////////////////// CPostedMessage //////////////////////////////

/***************************************************************************/
/***************************************************************************/
CPostedMessage::CPostedMessage(CMessageHandler &i_handler,
		ActorHandle i_sender) :
	d_handler(i_handler)
{
	ASSERT(d_handler.d_pair != NULL);

	d_prevPost = d_handler.d_pair->d_owner.d_queueTail;
	d_nextPost = NULL;

	if (d_handler.d_pair->d_owner.d_queueHead == NULL)
		d_handler.d_pair->d_owner.d_queueHead = this;
	d_handler.d_pair->d_owner.d_queueTail = this;

	d_ownData = false;
	d_data = NULL;

	d_sender = i_sender;
}

/***************************************************************************/
/***************************************************************************/
CPostedMessage::~CPostedMessage()
{
	ASSERT(d_handler.d_pair != NULL);

	if (d_prevPost != NULL)
		d_prevPost->d_nextPost = d_nextPost;
	else
		d_handler.d_pair->d_owner.d_queueHead = d_nextPost;

	if (d_nextPost != NULL)
		d_nextPost->d_prevPost = d_prevPost;
	else
		d_handler.d_pair->d_owner.d_queueTail = d_prevPost;

	if (d_ownData)
	{
#ifdef DEBUG_MESSAGE
		GetSingleton(CMessageDispatcher).WriteLogEntry(MESSAGE_LOG_MEMORY,
				"PostedData Free", d_handler, false,
				"In CPostedMessage::~CPostedMessage()");
#endif
		memFree(d_data);
	}
}

/***************************************************************************/
/***************************************************************************/
void* CPostedMessage::AllocateData(uint32 i_size)
{
	if (d_ownData)
	{
#ifdef DEBUG_MESSAGE
		GetSingleton(CMessageDispatcher).WriteLogEntry(MESSAGE_LOG_MEMORY,
				"PostedData Free", d_handler, true, "In AllocateData().");
#endif
		memFree(d_data);
	}

#ifdef DEBUG_MESSAGE
	GetSingleton(CMessageDispatcher).WriteLogEntry(MESSAGE_LOG_MEMORY,
			"PostedData Alloc", d_handler, true, "In AllocateData().");
#endif
	d_data = memAlloc(i_size);
	ASSERT_PTR(d_data);
	d_ownData = true;

	return(d_data);
}

/***************************************************************************/
/***************************************************************************/
void CPostedMessage::HandleMessage(void)
{
#ifdef DEBUG_MESSAGE
	CMessageDispatcher &dispatcher = GetSingleton(CMessageDispatcher);
	dispatcher.WriteLogEntry(MESSAGE_LOG_SEND_LOCAL, "HandleMessage",
			d_handler, true, NULL);
	dispatcher.d_sendDepth++;
	const char* debugFile = CMessageDispatcher::s_debugFile;
	int debugLine = CMessageDispatcher::s_debugLine;
#endif
	if (d_handler.d_pair != NULL)
	{
		d_handler.d_pair->d_active = true;
		CMessageDispatcher::s_activeGroup = d_handler.d_pair->d_group.d_name;
#ifdef DEBUG_MESSAGE
		CMessageDispatcher::s_activeOwner = d_handler.d_pair->d_owner.d_ownerName;
#endif
	}
	d_handler.HandleMessage(d_data, d_sender, true);
	if (d_handler.d_pair != NULL)
		d_handler.d_pair->d_active = false;
	CMessageDispatcher::s_activeGroup = "NONE";
	CMessageDispatcher::s_activeOwner = "NONE";
#ifdef DEBUG_MESSAGE
	CMessageDispatcher::s_debugFile = debugFile;
	CMessageDispatcher::s_debugLine = debugLine;
	dispatcher.d_sendDepth--;
#endif
}

/////////////////////////////// CMessageOwner ///////////////////////////////

static Vector3 l_defaultOwnerInWorld(0.0f, 0.0f, 0.0f);
static DirCos3x3 l_defaultOwnerToWorld(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f);

/***************************************************************************/
/***************************************************************************/
CMessageOwner::CMessageOwner()
{
	d_pairHead = NULL;

	d_queueHead = NULL;
	d_queueTail = NULL;

	d_isMaster = true;
	d_sessionID = INVALID_SESSION_ID;

	d_ownerInWorld = &l_defaultOwnerInWorld;
	d_ownerToWorld = &l_defaultOwnerToWorld;

	SetOwnerName(NULL);
}

/***************************************************************************/
/***************************************************************************/
CMessageOwner::CMessageOwner(const char* i_ownerName)
{
	d_pairHead = NULL;

	d_queueHead = NULL;
	d_queueTail = NULL;

	d_isMaster = true;
	d_sessionID = INVALID_SESSION_ID;

	d_ownerInWorld = &l_defaultOwnerInWorld;
	d_ownerToWorld = &l_defaultOwnerToWorld;

	SetOwnerName(i_ownerName);
}

/***************************************************************************/
/***************************************************************************/
CMessageOwner::~CMessageOwner()
{
	while(d_pairHead != NULL)
	{
#ifdef DEBUG_MESSAGE
		if (ValidSingleton(CMessageDispatcher))
			GetSingleton(CMessageDispatcher).WriteLogEntry(MESSAGE_LOG_MEMORY,
					"Pair Delete", *d_pairHead, NULL, false,
					"In CMessageOwner::~CMessageOwner().");
#endif
		delete d_pairHead;
	}
}

/***************************************************************************/
/***************************************************************************/
void CMessageOwner::SetOwnerName(const char* i_ownerName)
{
#ifdef DEBUG_MESSAGE
	if (i_ownerName != NULL)
	{
		strncpy(d_ownerName, i_ownerName, MAX_OWNER_NAME_LENGTH - 1);
		d_ownerName[MAX_OWNER_NAME_LENGTH - 1] = '\0';
	}
	else
		d_ownerName[0] = '\0';
#endif
}

/***************************************************************************/
/***************************************************************************/
void CMessageOwner::SetOwnerName(const char* i_prefix, const char* i_suffix)
{
#ifdef DEBUG_MESSAGE
	if ((i_prefix == NULL) || (i_prefix[0] == '\0'))
		SetOwnerName(i_suffix);
	else if ((i_suffix == NULL) || (i_suffix[0] == '\0'))
		SetOwnerName(i_prefix);
	else
	{
		int available = MAX_OWNER_NAME_LENGTH - 1;
		strncpy(d_ownerName, i_prefix, available);
		available -= strlen(i_prefix);
		if (available > 1)
		{
			strcat(d_ownerName, ":");
			strncat(d_ownerName, i_suffix, available - 1);
		}
		d_ownerName[MAX_OWNER_NAME_LENGTH - 1] = '\0';
	}
#endif
}

/***************************************************************************/
/***************************************************************************/
void CMessageOwner::HandlePostedMessages(void)
{
#ifdef DEBUG_MESSAGE
	bool foundMessage = false;
	CMessageDispatcher &dispatcher = GetSingleton(CMessageDispatcher);
#endif
	while(d_queueHead != NULL)
	{
#ifdef DEBUG_MESSAGE
		if (!foundMessage)
		{
			foundMessage = true;
			dispatcher.WriteLogEntry(MESSAGE_LOG_SEND_LOCAL,
					"HandlePostedMessages All", NULL, NULL, NULL, NULL, true, NULL);
			dispatcher.d_sendDepth++;
		}
#endif
		d_queueHead->HandleMessage();
#ifdef DEBUG_MESSAGE
		dispatcher.WriteLogEntry(MESSAGE_LOG_MEMORY, "PostedMessage Delete",
				d_queueHead->d_handler, true, "When handling all.");
#endif
		delete d_queueHead;
	}
#ifdef DEBUG_MESSAGE
	if (foundMessage)
		dispatcher.d_sendDepth--;
#endif
}

/***************************************************************************/
/***************************************************************************/
void CMessageOwner::HandlePostedMessages(const char* i_name)
{
#ifdef DEBUG_MESSAGE
	bool foundMessage = false;
	CMessageDispatcher &dispatcher = GetSingleton(CMessageDispatcher);
#endif
	CPostedMessage* message = d_queueHead;
	while(message != NULL)
	{
		CPostedMessage* curr = message;
		message = message->d_nextPost;
		if (!strcmp(i_name, curr->d_handler.GetName()))
		{
#ifdef DEBUG_MESSAGE
			if (!foundMessage)
			{
				foundMessage = true;
				dispatcher.WriteLogEntry(MESSAGE_LOG_SEND_LOCAL,
						"HandlePostedMessages Named", NULL, NULL, NULL, NULL, true,
						i_name);
				dispatcher.d_sendDepth++;
			}
#endif
			curr->HandleMessage();
#ifdef DEBUG_MESSAGE
			dispatcher.WriteLogEntry(MESSAGE_LOG_MEMORY, "PostedMessage Delete",
					curr->d_handler, true, "When handling named.");
#endif
			delete curr;
		}
	}
#ifdef DEBUG_MESSAGE
	if (foundMessage)
		dispatcher.d_sendDepth--;
#endif
}

/***************************************************************************/
/***************************************************************************/
void CMessageOwner::SetMaster(bool i_isMaster)
{
	d_isMaster = i_isMaster;
#ifdef DEBUG_MESSAGE
	if (ValidSingleton(CMessageDispatcher))
	{
		char notes[20];
		sprintf(notes, "Owner is %s", d_isMaster ? "Master" : "Duplica");
		GetSingleton(CMessageDispatcher).WriteLogEntry(MESSAGE_LOG_CONFIG,
				"SetMaster", NULL, this, NULL, NULL, true, notes);
	}
#endif
}

/***************************************************************************/
/***************************************************************************/
void CMessageOwner::SetSessionID(uint32 i_sessionID)
{
	d_sessionID = i_sessionID;
#ifdef DEBUG_MESSAGE
	if (ValidSingleton(CMessageDispatcher))
	{
		char notes[20];
		sprintf(notes, "New ID = 0x%x", d_sessionID);
		GetSingleton(CMessageDispatcher).WriteLogEntry(MESSAGE_LOG_CONFIG,
				"SetSessionID", NULL, this, NULL, NULL, true, notes);
	}
#endif
}

/***************************************************************************/
/***************************************************************************/
void CMessageOwner::UnregisterAll(void)
{
#ifdef DEBUG_MESSAGE
	if (ValidSingleton(CMessageDispatcher))
		GetSingleton(CMessageDispatcher).WriteLogEntry(MESSAGE_LOG_CONFIG, 
				"UnregisterAll Owner\'s Handlers", NULL, this, NULL, NULL, true,
				NULL);
#endif
	while(d_pairHead != NULL)
	{
#ifdef DEBUG_MESSAGE
		if (ValidSingleton(CMessageDispatcher))
			GetSingleton(CMessageDispatcher).WriteLogEntry(MESSAGE_LOG_MEMORY,
					"Pair Delete", *d_pairHead, NULL, true,
					"In CMessageOwner::UnregisterAll().");
#endif
		delete d_pairHead;
	}
}

/***************************************************************************/
/***************************************************************************/
void CMessageOwner::RemoveFromQueue(CMessageHandler* i_handler)
{
	CPostedMessage* message = d_queueHead;
	while(message != NULL)
	{
		CPostedMessage* curr = message;
		message = message->d_nextPost;
		if (&(curr->d_handler) == i_handler)
		{
#ifdef DEBUG_MESSAGE
			if (ValidSingleton(CMessageDispatcher))
				GetSingleton(CMessageDispatcher).WriteLogEntry(MESSAGE_LOG_MEMORY,
						"PostedMessage Delete", *i_handler, false, "RemoveFromQueue");
#endif
			delete curr;
		}
	}
}

/////////////////////////////// CMessageGroup ///////////////////////////////

/***************************************************************************/
/***************************************************************************/
CMessageGroup::CMessageGroup(const char* i_name)
{
	if (i_name != NULL)
	{
		ASSERT((sizeof(this) - sizeof(CMessageGroup)) >= strlen(i_name));
		strcpy(d_name, i_name);
	}
	else
		d_name[0] = '\0';
	d_pairHead = NULL;
	d_packedSize = 0;
	d_sizeFunc = NULL;
	d_packFunc = NULL;
	d_unpackFunc = NULL;

#ifdef DEBUG_MESSAGE
	d_registered = false;
	d_sent = false;
	d_registerBeforeSend = 0;
	d_registerAfterSend = 0;
	d_sendBeforeRegister = 0;
	d_sendAfterRegister = 0;
#endif
}

/***************************************************************************/
/***************************************************************************/
CMessageGroup::~CMessageGroup()
{
/*
 *	Clean up each pair.
 */
	while(d_pairHead != NULL)
	{
#ifdef DEBUG_MESSAGE
		GetSingleton(CMessageDispatcher).WriteLogEntry(MESSAGE_LOG_MEMORY,
				"Pair Delete", *d_pairHead, NULL, false,
				"In CMessageGroup::~CMessageGroup().");
#endif
		delete d_pairHead;
	}
}

/***************************************************************************/
/***************************************************************************/
bool CMessageGroup::InstallTranslator(TTranslatorPack i_packFunc,
		TTranslatorUnpack i_unpackFunc, TTranslatorSize i_sizeFunc,
		uint32 i_packedSize)
{
#ifdef DEBUG_MESSAGE
	GetSingleton(CMessageDispatcher).WriteLogEntry(MESSAGE_LOG_CONFIG,
			"InstallTranslator", this, NULL, NULL, NULL, true, NULL);
#endif
	if ((d_packFunc == NULL) && (d_unpackFunc == NULL))
	{
		d_packedSize = i_packedSize;
		d_sizeFunc = i_sizeFunc;
		d_packFunc = i_packFunc;
		d_unpackFunc = i_unpackFunc;
		return(true);
	}

	ASSERT((i_packFunc == d_packFunc) && (i_unpackFunc == d_unpackFunc) &&
			(i_packedSize == d_packedSize) && (i_sizeFunc == d_sizeFunc));

	return((i_packFunc == d_packFunc) && (i_unpackFunc == d_unpackFunc) &&
			(i_packedSize == d_packedSize) && (i_sizeFunc == d_sizeFunc));
}

/***************************************************************************/
/***************************************************************************/
uint32 CMessageGroup::GetPackedSize(void* i_data)
{
	if (d_sizeFunc != NULL)
		return(d_sizeFunc(i_data));
	return(d_packedSize);
}

/***************************************************************************/
/***************************************************************************/
bool CMessageGroup::Pack(void* i_dataIn, void* i_dataOut, CMessageOwner &i_owner)
{
	if (d_packFunc != NULL)
	{
		d_packFunc(i_dataIn, i_dataOut, i_owner);
		return(true);
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
void* CMessageGroup::Unpack(void* i_data, CMessageOwner &i_owner)
{
	if (d_unpackFunc != NULL)
		return(d_unpackFunc(i_data, i_owner));
	return(NULL);
}

#ifdef DEBUG_MESSAGE

/***************************************************************************/
/***************************************************************************/
void CMessageGroup::LogRegister(void)
{
	if (!d_sent)
		d_registerBeforeSend++;
	else
		d_registerAfterSend++;
	d_registered = true;
}

/***************************************************************************/
/***************************************************************************/
void CMessageGroup::LogSend(void)
{
	if (!d_registered)
		d_sendBeforeRegister++;
	else
		d_sendAfterRegister++;
	d_sent = true;
}

#endif

//////////////////////////////// CMessagePair ///////////////////////////////

/***************************************************************************/
/***************************************************************************/
CMessagePair::CMessagePair(CMessageOwner &i_owner, CMessageGroup &i_group) :
	d_owner(i_owner),
	d_group(i_group)
{
	d_handlerHead = NULL;
	d_groupPrev = NULL;
	d_groupNext = NULL;
	d_ownerPrev = NULL;
	d_ownerNext = NULL;
	d_active = false;
}

/***************************************************************************/
/***************************************************************************/
CMessagePair::~CMessagePair()
{
	ASSERTS(!d_active, "Deleting message pair while active");
/*
 *	Unregister each of the pair's handlers.
 */
	while(d_handlerHead != NULL)
	{
		CMessageHandler* next = d_handlerHead->d_next;
		d_owner.RemoveFromQueue(d_handlerHead);
		d_handlerHead->d_pair = NULL;
		d_handlerHead->d_next = NULL;
		if (d_handlerHead->d_deleteWhenDefunct)
			delete d_handlerHead;
		d_handlerHead = next;
	}
/*
 *	Remove the pair from the owner's pair list.
 */
	if (d_ownerNext != NULL)
		d_ownerNext->d_ownerPrev = d_ownerPrev;
	if (d_ownerPrev != NULL)
		d_ownerPrev->d_ownerNext = d_ownerNext;
	else
		d_owner.d_pairHead = d_ownerNext;
/*
 *	Remove the pair from the group's pair list.
 */
	if (d_groupNext != NULL)
		d_groupNext->d_groupPrev = d_groupPrev;
	if (d_groupPrev != NULL)
		d_groupPrev->d_groupNext = d_groupNext;
	else
		d_group.d_pairHead = d_groupNext;
}

/***************************************************************************/
/***************************************************************************/
uint32 CMessagePair::SendMessage(void* i_data, ActorHandle i_sender,
		uint32 i_handleRules)
{
#ifdef _DEBUG
	if (d_active)
	{
		dbgPrint("Sending message to active message pair: Group = %s",
				d_group.d_name);
#ifdef DEBUG_MESSAGE
		dbgPrint(", Owner = %s", d_owner.d_ownerName);
#endif
		dbgPrint("\n");
	}
#endif
	CMessageDispatcher::s_activeGroup = d_group.d_name;
#ifdef DEBUG_MESSAGE
	CMessageDispatcher::s_activeOwner = d_owner.d_ownerName;
#endif

	uint32 routeRules = MESSAGE_DISABLE;
	CMessageHandler* handler = d_handlerHead;
	while(handler != NULL)
	{
		if (handler->d_rules & i_handleRules)
		{
#ifdef DEBUG_MESSAGE
			CMessageDispatcher &dispatcher = GetSingleton(CMessageDispatcher);
			dispatcher.WriteLogEntry(MESSAGE_LOG_SEND_LOCAL, "HandleMessage",
					*handler, true, NULL);
			dispatcher.d_sendDepth++;
			const char* debugFile = CMessageDispatcher::s_debugFile;
			int debugLine = CMessageDispatcher::s_debugLine;
#endif
			d_active = true;
			handler->HandleMessage(i_data, i_sender, false);
			d_active = false;
#ifdef DEBUG_MESSAGE
			CMessageDispatcher::s_debugFile = debugFile;
			CMessageDispatcher::s_debugLine = debugLine;
			dispatcher.d_sendDepth--;
#endif
		}
		routeRules |= handler->d_rules;
		handler = handler->d_next;
	}

	if (!ValidSingleton(CSession) || !g_session.IsNetworked())
		routeRules = MESSAGE_DISABLE;
		
	CMessageDispatcher::s_activeGroup = "NONE";
	CMessageDispatcher::s_activeOwner = "NONE";

	return(routeRules);
}

/***************************************************************************/
/***************************************************************************/
uint32 CMessagePair::PostMessage(void* i_data, ActorHandle i_sender,
		uint32 i_handleRules)
{
	uint32 routeRules = MESSAGE_DISABLE;
	CMessageHandler* handler = d_handlerHead;
	while(handler != NULL)
	{
		if (handler->d_rules & i_handleRules)
		{
#ifdef DEBUG_MESSAGE
			CMessageDispatcher &dispatcher = GetSingleton(CMessageDispatcher);
			dispatcher.WriteLogEntry(MESSAGE_LOG_SEND_LOCAL, "QueueMessage",
					*handler, true, NULL);
			dispatcher.d_sendDepth++;
			const char* debugFile = CMessageDispatcher::s_debugFile;
			int debugLine = CMessageDispatcher::s_debugLine;
#endif
			d_active = true;
			handler->QueueMessage(i_data, i_sender);
			d_active = false;
#ifdef DEBUG_MESSAGE
			CMessageDispatcher::s_debugFile = debugFile;
			CMessageDispatcher::s_debugLine = debugLine;
			dispatcher.d_sendDepth--;
#endif
		}
		routeRules |= handler->d_rules;
		handler = handler->d_next;
	}

	if (!ValidSingleton(CSession) || !g_session.IsNetworked())
		routeRules = MESSAGE_DISABLE;

	return(routeRules);
}

/***************************************************************************/
/***************************************************************************/
uint32 CMessagePair::GetPackedSize(void* i_data)
{
	return(d_group.GetPackedSize(i_data));
}

/***************************************************************************/
/***************************************************************************/
bool CMessagePair::Pack(void* i_dataIn, void* i_dataOut)
{
	return(d_group.Pack(i_dataIn, i_dataOut, d_owner));
}

/***************************************************************************/
/***************************************************************************/
void* CMessagePair::Unpack(void* i_data)
{
	return(d_group.Unpack(i_data, d_owner));
}

///////////////////////////// CMessageDispatcher ////////////////////////////

/***************************************************************************/
/***************************************************************************/
CMessageDispatcher::CMessageDispatcher() :
	d_groupTable(6, 32, 0)
{
	// HACK!!! provide mechanism to block outgoing messages.
	d_blockOutgoing = false;

#ifdef DEBUG_MESSAGE
	d_sendDepth = 0;
	d_writeLog = false;
	d_logFilter = ~0x0;
#endif
}

/***************************************************************************/
/***************************************************************************/
CMessageDispatcher::~CMessageDispatcher()
{
	CLinearHash<CMessageGroup*>::CIterator it;
	it.Init(&d_groupTable);
	while(!it.End())
	{
		CMessageGroup** group = it.Get();
		ASSERT(group != NULL);
#ifdef DEBUG_MESSAGE
		WriteLogEntry(MESSAGE_LOG_MEMORY, "Group Delete", *group, NULL, NULL,
				NULL, false, "In CMessageDispatcher::~CMessageDispatcher().");
#endif
		(*group)->~CMessageGroup();
		memFree(*group);
		it.Set(NULL, NULL);
		++it;
	}
}

/***************************************************************************/
// Remove groups without handlers. Note that a translator may have been
// installed and it will need to be installed again if a handler is later
// registered for the group.
/***************************************************************************/
void CMessageDispatcher::Purge(void)
{
/*
 *	The iterator is invalid if a entry in the hash table is added or removed;
 *	so, the table cannot be simple walked and cleaned up.
 */
	bool foundEmptyGroup = true;
	while(foundEmptyGroup)
	{
		foundEmptyGroup = false;
		CLinearHash<CMessageGroup*>::CIterator it;
		it.Init(&d_groupTable);
		while(!it.End())
		{
			CMessageGroup** curr = it.Get();
			ASSERT((curr != NULL) && (*curr != NULL));
			if ((*curr)->d_pairHead == NULL)
			{
				foundEmptyGroup = true;
				CMessageGroup* group = *curr;
				d_groupTable.Remove(group->d_name);
#ifdef DEBUG_MESSAGE
				WriteLogEntry(MESSAGE_LOG_MEMORY, "Group Delete", group, NULL,
						NULL, NULL, true, "In CMessageDispatcher::Purge().");
#endif
				group->~CMessageGroup();
				memFree(group);
				break;
			}
			++it;
		}
	}
#ifdef _DEBUG
	CLinearHash<CMessageGroup*>::CIterator it;
	it.Init(&d_groupTable);
	while(!it.End())
	{
		CMessageGroup** curr = it.Get();
		ASSERT((curr != NULL) && (*curr != NULL));
		CMessagePair* pair = (*curr)->d_pairHead;
		while(pair != NULL)
		{
			pair = pair->d_groupNext;
		}
		++it;
	}
#endif
}

/***************************************************************************/
/***************************************************************************/
void CMessageDispatcher::Register(CMessageHandler* i_handler,
		const char* i_name, CMessageOwner &i_owner)
{
/*
 *	Sanity check.
 */
	ASSERT(i_handler != NULL);
/*
 *	Find an existing group or create a new one.
 */
	CMessageGroup* group = GetGroup(i_name);
/*
 *	Register.
 */
	i_handler->Register(i_owner, *group);

#ifdef DEBUG_MESSAGE
	group->LogRegister();
#endif
}

/***************************************************************************/
/***************************************************************************/
void CMessageDispatcher::Unregister(CMessageHandler* i_handler)
{
	ASSERT(i_handler != NULL);

	i_handler->Unregister();
	if (i_handler->d_deleteWhenDefunct)
		delete i_handler;
}

/***************************************************************************/
/***************************************************************************/
bool CMessageDispatcher::InstallFixedSizeTranslator(const char* i_name,
		TTranslatorPack i_packFunc, TTranslatorUnpack i_unpackFunc,
		uint32 i_packedSize)
{
	return(InstallTranslatorInGroup(i_name, i_packFunc, i_unpackFunc, NULL,
			i_packedSize) != NULL);
}

/***************************************************************************/
/***************************************************************************/
bool CMessageDispatcher::InstallVariableSizeTranslator(const char* i_name,
		TTranslatorPack i_packFunc, TTranslatorUnpack i_unpackFunc,
		TTranslatorSize i_sizeFunc)
{
	return(InstallTranslatorInGroup(i_name, i_packFunc, i_unpackFunc,
			i_sizeFunc, 0) != NULL);
}

/***************************************************************************/
/***************************************************************************/
CMessageGroup* CMessageDispatcher::InstallTranslatorInGroup(const char* i_name,
		TTranslatorPack i_packFunc, TTranslatorUnpack i_unpackFunc,
		TTranslatorSize i_sizeFunc, uint32 i_packedSize)
{
/*
 *	Find an existing group or create a new one.
 */
	CMessageGroup* group = GetGroup(i_name);
/*
 *	Install the translator.
 */
	if (group->InstallTranslator(i_packFunc, i_unpackFunc, i_sizeFunc,
			i_packedSize))
		return(group);

	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
CMessagePair* CMessageDispatcher::GetMessagePair(const char* i_name,
		CMessageOwner &i_owner)
{
	CMessageGroup** group = d_groupTable.Get(i_name);
	if ((group != NULL) && (*group != NULL))
	{
		CMessagePair* pair = i_owner.d_pairHead;
		while(pair != NULL)
		{
			if (&pair->d_group == *group)
				return(pair);
			pair = pair->d_ownerNext;
		}
	}
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
CMessagePair* CMessageDispatcher::GetMessagePair(const char* i_name,
		CMessageOwner* i_owner)
{
	if (i_owner != NULL)
		return(GetMessagePair(i_name, *i_owner));
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
CMessagePair* CMessageDispatcher::GetMessagePair(const char* i_name,
		ActorHandle i_actor)
{
	CActor* actor = CActor::FromHandle(i_actor);
	if (actor != NULL)
		return(GetMessagePair(i_name, actor->GetMessageOwner()));
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
CMessagePair* CMessageDispatcher::GetMessagePair(uint32 i_hash,
		CMessageOwner &i_owner)
{
	CMessageGroup** group = d_groupTable.Get(i_hash);
	if ((group != NULL) && (*group != NULL))
	{
		CMessagePair* pair = i_owner.d_pairHead;
		while(pair != NULL)
		{
			if (&pair->d_group == *group)
				return(pair);
			pair = pair->d_ownerNext;
		}
	}
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
CMessagePair* CMessageDispatcher::GetMessagePair(uint32 i_hash,
		CMessageOwner* i_owner)
{
	if (i_owner != NULL)
		return(GetMessagePair(i_hash, *i_owner));
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
CMessagePair* CMessageDispatcher::GetMessagePair(uint32 i_hash,
		ActorHandle i_actor)
{
	CActor* actor = CActor::FromHandle(i_actor);
	if (actor != NULL)
		return(GetMessagePair(i_hash, actor->GetMessageOwner()));
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
CMessageGroup* CMessageDispatcher::GetMessageGroup(const char* i_name)
{
	CMessageGroup** group = d_groupTable.Get(i_name);
	if (group != NULL)
		return(*group);
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
CMessageGroup* CMessageDispatcher::GetMessageGroup(uint32 i_hash)
{
	CMessageGroup** group = d_groupTable.Get(i_hash);
	if (group != NULL)
		return(*group);
	return(NULL);
}

/***************************************************************************/
/***************************************************************************/
void CMessageDispatcher::SendMessageToAll(const char* i_name, void* i_data,
		ActorHandle i_sender)
{
#ifdef DEBUG_MESSAGE
	GetGroup(i_name)->LogSend();
#endif
	CMessageGroup** group = d_groupTable.Get(i_name);
	if ((group != NULL) && (*group != NULL))
	{
#ifdef DEBUG_MESSAGE
		WriteLogEntry(MESSAGE_LOG_SEND_LOCAL, "SendMessageToAll Root", *group,
				NULL, NULL, NULL, true, NULL);
		d_sendDepth++;
#endif
		CMessagePair* pair = (*group)->d_pairHead;
		while(pair != NULL)
		{
#ifdef DEBUG_MESSAGE
			WriteLogEntry(MESSAGE_LOG_SEND_LOCAL, "SendMessageToAll Pair", *pair,
					NULL, true, NULL);
			d_sendDepth++;
#endif
			SendMessage(i_data, i_sender, pair);
			pair = pair->d_groupNext;
#ifdef DEBUG_MESSAGE
			d_sendDepth--;
#endif
		}
#ifdef DEBUG_MESSAGE
		d_sendDepth--;
#endif
	}
}

/***************************************************************************/
/***************************************************************************/
bool CMessageDispatcher::SendMessageToOwner(const char* i_name, void* i_data,
		ActorHandle i_sender, CMessageOwner &i_owner)
{
#ifdef DEBUG_MESSAGE
	GetGroup(i_name)->LogSend();
#endif
	CMessagePair* pair = GetMessagePair(i_name, i_owner);
	if (pair != NULL)
	{
#ifdef DEBUG_MESSAGE
		WriteLogEntry(MESSAGE_LOG_SEND_LOCAL, "SendMessageToOwner Ref", *pair,
				NULL, true, NULL);
		d_sendDepth++;
#endif
		SendMessage(i_data, i_sender, pair);
#ifdef DEBUG_MESSAGE
		d_sendDepth--;
#endif
		return(true);
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
bool CMessageDispatcher::SendMessageToOwner(const char* i_name, void* i_data,
		ActorHandle i_sender, CMessageOwner* i_owner)
{
#ifdef DEBUG_MESSAGE
	GetGroup(i_name)->LogSend();
#endif
	CMessagePair* pair = GetMessagePair(i_name, i_owner);
	if (pair != NULL)
	{
#ifdef DEBUG_MESSAGE
		WriteLogEntry(MESSAGE_LOG_SEND_LOCAL, "SendMessageToOwner Ptr", *pair,
				NULL, true, NULL);
		d_sendDepth++;
#endif
		SendMessage(i_data, i_sender, pair);
#ifdef DEBUG_MESSAGE
		d_sendDepth--;
#endif
		return(true);
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
bool CMessageDispatcher::SendMessageToActor(const char* i_name, void* i_data,
		ActorHandle i_sender, ActorHandle i_actor)
{
#ifdef DEBUG_MESSAGE
	GetGroup(i_name)->LogSend();
#endif
	CMessagePair* pair = GetMessagePair(i_name, i_actor);
	if (pair != NULL)
	{
#ifdef DEBUG_MESSAGE
		WriteLogEntry(MESSAGE_LOG_SEND_LOCAL, "SendMessageToActor", *pair, NULL,
				true, NULL);
		d_sendDepth++;
#endif
		SendMessage(i_data, i_sender, pair);
#ifdef DEBUG_MESSAGE
		d_sendDepth--;
#endif
		return(true);
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
void CMessageDispatcher::SendMessageToSession(const char* i_name, void* i_data,
		ActorHandle i_sender)
{
#ifdef DEBUG_MESSAGE
	CMessageGroup* group = GetGroup(i_name);
	group->LogSend();
	WriteLogEntry(MESSAGE_LOG_SEND_LOCAL, "SendMessageToSession", group, NULL,
			NULL, NULL, true, NULL);
	d_sendDepth++;
#endif
	// HACK!!! provide mechanism to block outgoing messages.
	if (!d_blockOutgoing)
		SendToSession(i_name, i_data, i_sender);
	SendMessageToLocalHost(i_name, i_data, i_sender);
#ifdef DEBUG_MESSAGE
	d_sendDepth--;
#endif
}

/***************************************************************************/
/***************************************************************************/
void CMessageDispatcher::SendMessageToHost(const char* i_name, void* i_data,
		ActorHandle i_sender, CHost* i_host)
{
#ifdef DEBUG_MESSAGE
	CMessageGroup* group = GetGroup(i_name);
	group->LogSend();
	WriteLogEntry(MESSAGE_LOG_SEND_LOCAL, "SendMessageToHost", group, NULL,
			NULL, NULL, true, NULL);
	d_sendDepth++;
#endif
	if ((i_host == NULL) ||
			(g_session.IsNetworked() && (i_host == g_session.GetLocalHost())))
		SendMessageToLocalHost(i_name, i_data, i_sender);
	else
	{
		// HACK!!! provide mechanism to block outgoing messages.
		if (!d_blockOutgoing)
			SendToHost(i_name, i_data, i_sender, i_host);
	}
#ifdef DEBUG_MESSAGE
	d_sendDepth--;
#endif
}

/***************************************************************************/
/***************************************************************************/
void CMessageDispatcher::SendMessageToLocalHost(const char* i_name,
		void* i_data, ActorHandle i_sender)
{
	CMessageGroup** group = d_groupTable.Get(i_name);
	if ((group != NULL) && (*group != NULL))
	{
#ifdef DEBUG_MESSAGE
		WriteLogEntry(MESSAGE_LOG_SEND_LOCAL, "SendMessageToLocalHost Root",
				*group, NULL, NULL, NULL, true, NULL);
		d_sendDepth++;
#endif
		CMessagePair* pair = (*group)->d_pairHead;
		while(pair != NULL)
		{
#ifdef DEBUG_MESSAGE
			WriteLogEntry(MESSAGE_LOG_SEND_LOCAL, "SendMessageToLocalHost Pair",
					*pair, NULL, true, NULL);
			d_sendDepth++;
#endif
			pair->SendMessage(i_data, i_sender, pair->d_owner.IsMaster() ?
					MESSAGE_HANDLE_IF_MASTER : MESSAGE_HANDLE_IF_DUPLICA);
			pair = pair->d_groupNext;
#ifdef DEBUG_MESSAGE
			d_sendDepth--;
#endif
		}
#ifdef DEBUG_MESSAGE
		d_sendDepth--;
#endif
	}
}

/***************************************************************************/
/***************************************************************************/
void CMessageDispatcher::SendMessageToLocalHost(uint32 i_hash, void* i_data,
		ActorHandle i_sender)
{
	CMessageGroup** group = d_groupTable.Get(i_hash);
	if ((group != NULL) && (*group != NULL))
	{
#ifdef DEBUG_MESSAGE
		WriteLogEntry(MESSAGE_LOG_SEND_NETWORK, "SendMessage Route to Host Root",
				*group, NULL, NULL, NULL, false, NULL);
		d_sendDepth++;
#endif
		CMessagePair* pair = (*group)->d_pairHead;
		while(pair != NULL)
		{
#ifdef DEBUG_MESSAGE
			WriteLogEntry(MESSAGE_LOG_SEND_NETWORK,
					"SendMessage Route to Host Pair", *pair, NULL, false, NULL);
			d_sendDepth++;
#endif
			pair->SendMessage(i_data, i_sender, pair->d_owner.IsMaster() ?
					MESSAGE_HANDLE_IF_MASTER : MESSAGE_HANDLE_IF_DUPLICA);
			pair = pair->d_groupNext;
#ifdef DEBUG_MESSAGE
			d_sendDepth--;
#endif
		}
#ifdef DEBUG_MESSAGE
		d_sendDepth--;
#endif
	}
}

/***************************************************************************/
/***************************************************************************/
void CMessageDispatcher::SendMessage(void* i_data, ActorHandle i_sender,
		CMessagePair* i_pair)
{
	if (i_pair->d_owner.IsMaster())
	{
		uint32 routeRules = i_pair->SendMessage(i_data, i_sender,
				MESSAGE_HANDLE_IF_MASTER);

		// HACK!!! provide mechanism to block outgoing messages.
		if (d_blockOutgoing)
			return;

		if (routeRules & (MESSAGE_ROUTE_TO_DUPLICAS | MESSAGE_ROUTE_TO_NEIGHBORS))
			SendToDuplicas(i_data, i_sender, i_pair);
	}
	else
	{
		uint32 routeRules = i_pair->SendMessage(i_data, i_sender,
				MESSAGE_HANDLE_IF_DUPLICA);

		// HACK!!! provide mechanism to block outgoing messages.
		if (d_blockOutgoing)
			return;

		if (routeRules & MESSAGE_ROUTE_TO_NEIGHBORS)
			SendToNeighbors(i_data, i_sender, i_pair);
		else if (routeRules & MESSAGE_ROUTE_TO_MASTER)
			SendToMaster(i_data, i_sender, i_pair);
	}
}

/***************************************************************************/
/***************************************************************************/
void CMessageDispatcher::PostMessageToAll(const char* i_name, void* i_data,
		ActorHandle i_sender)
{
#ifdef DEBUG_MESSAGE
	GetGroup(i_name)->LogSend();
#endif
	CMessageGroup** group = d_groupTable.Get(i_name);
	if ((group != NULL) && (*group != NULL))
	{
#ifdef DEBUG_MESSAGE
		WriteLogEntry(MESSAGE_LOG_SEND_LOCAL, "PostMessageToAll Root", *group,
				NULL, NULL, NULL, true, NULL);
		d_sendDepth++;
#endif
		CMessagePair* pair = (*group)->d_pairHead;
		while(pair != NULL)
		{
#ifdef DEBUG_MESSAGE
			WriteLogEntry(MESSAGE_LOG_SEND_LOCAL, "PostMessageToAll Pair", *pair,
					NULL, true, NULL);
			d_sendDepth++;
#endif
			PostMessage(i_data, i_sender, pair);
			pair = pair->d_groupNext;
#ifdef DEBUG_MESSAGE
			d_sendDepth--;
#endif
		}
#ifdef DEBUG_MESSAGE
		d_sendDepth--;
#endif
	}
}

/***************************************************************************/
/***************************************************************************/
bool CMessageDispatcher::PostMessageToOwner(const char* i_name, void* i_data,
		ActorHandle i_sender, CMessageOwner &i_owner)
{
#ifdef DEBUG_MESSAGE
	GetGroup(i_name)->LogSend();
#endif
	CMessagePair* pair = GetMessagePair(i_name, i_owner);
	if (pair != NULL)
	{
#ifdef DEBUG_MESSAGE
		WriteLogEntry(MESSAGE_LOG_SEND_LOCAL, "PostMessageToOwner Ref", *pair,
				NULL, true, NULL);
		d_sendDepth++;
#endif
		PostMessage(i_data, i_sender, pair);
#ifdef DEBUG_MESSAGE
		d_sendDepth--;
#endif
		return(true);
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
bool CMessageDispatcher::PostMessageToOwner(const char* i_name, void* i_data,
		ActorHandle i_sender, CMessageOwner* i_owner)
{
#ifdef DEBUG_MESSAGE
	GetGroup(i_name)->LogSend();
#endif
	CMessagePair* pair = GetMessagePair(i_name, i_owner);
	if (pair != NULL)
	{
#ifdef DEBUG_MESSAGE
		WriteLogEntry(MESSAGE_LOG_SEND_LOCAL, "PostMessageToOwner Ptr", *pair,
				NULL, true, NULL);
		d_sendDepth++;
#endif
		PostMessage(i_data, i_sender, pair);
#ifdef DEBUG_MESSAGE
		d_sendDepth--;
#endif
		return(true);
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
bool CMessageDispatcher::PostMessageToActor(const char* i_name, void* i_data,
		ActorHandle i_sender, ActorHandle i_actor)
{
#ifdef DEBUG_MESSAGE
	GetGroup(i_name)->LogSend();
#endif
	CMessagePair* pair = GetMessagePair(i_name, i_actor);
	if (pair != NULL)
	{
#ifdef DEBUG_MESSAGE
		WriteLogEntry(MESSAGE_LOG_SEND_LOCAL, "PostMessageToActor", *pair, NULL,
				true, NULL);
		d_sendDepth++;
#endif
		PostMessage(i_data, i_sender, pair);
#ifdef DEBUG_MESSAGE
		d_sendDepth--;
#endif
		return(true);
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
void CMessageDispatcher::PostMessageToSession(const char* i_name, void* i_data,
		ActorHandle i_sender)
{
#ifdef DEBUG_MESSAGE
	CMessageGroup* group = GetGroup(i_name);
	group->LogSend();
	WriteLogEntry(MESSAGE_LOG_SEND_LOCAL, "PostMessageToSession", group, NULL,
			NULL, NULL, true, NULL);
	d_sendDepth++;
#endif
	// HACK!!! provide mechanism to block outgoing messages.
	if (!d_blockOutgoing)
		PostToSession(i_name, i_data, i_sender);
	PostMessageToLocalHost(i_name, i_data, i_sender);
#ifdef DEBUG_MESSAGE
	d_sendDepth--;
#endif
}

/***************************************************************************/
/***************************************************************************/
void CMessageDispatcher::PostMessageToHost(const char* i_name, void* i_data,
		ActorHandle i_sender, CHost* i_host)
{
#ifdef DEBUG_MESSAGE
	CMessageGroup* group = GetGroup(i_name);
	group->LogSend();
	WriteLogEntry(MESSAGE_LOG_SEND_LOCAL, "PostMessageToHost", group, NULL,
			NULL, NULL, true, NULL);
	d_sendDepth++;
#endif
	if ((i_host == NULL) ||
			(g_session.IsNetworked() && (i_host == g_session.GetLocalHost())))
		PostMessageToLocalHost(i_name, i_data, i_sender);
	else
	{
		// HACK!!! provide mechanism to block outgoing messages.
		if (!d_blockOutgoing)
			PostToHost(i_name, i_data, i_sender, i_host);
	}
#ifdef DEBUG_MESSAGE
	d_sendDepth--;
#endif
}

/***************************************************************************/
/***************************************************************************/
void CMessageDispatcher::PostMessageToLocalHost(const char* i_name,
		void* i_data, ActorHandle i_sender)
{
	CMessageGroup** group = d_groupTable.Get(i_name);
	if ((group != NULL) && (*group != NULL))
	{
#ifdef DEBUG_MESSAGE
		WriteLogEntry(MESSAGE_LOG_SEND_LOCAL, "PostMessageToLocalHost Root",
				*group, NULL, NULL, NULL, true, NULL);
		d_sendDepth++;
#endif
		CMessagePair* pair = (*group)->d_pairHead;
		while(pair != NULL)
		{
#ifdef DEBUG_MESSAGE
			WriteLogEntry(MESSAGE_LOG_SEND_LOCAL, "PostMessageToLocalHost Pair",
					*pair, NULL, true, NULL);
			d_sendDepth++;
#endif
			pair->PostMessage(i_data, i_sender, pair->d_owner.IsMaster() ?
					MESSAGE_HANDLE_IF_MASTER : MESSAGE_HANDLE_IF_DUPLICA);
			pair = pair->d_groupNext;
#ifdef DEBUG_MESSAGE
			d_sendDepth--;
#endif
		}
#ifdef DEBUG_MESSAGE
		d_sendDepth--;
#endif
	}
}

/***************************************************************************/
/***************************************************************************/
void CMessageDispatcher::PostMessageToLocalHost(uint32 i_hash,
		void* i_data, ActorHandle i_sender)
{
	CMessageGroup** group = d_groupTable.Get(i_hash);
	if ((group != NULL) && (*group != NULL))
	{
#ifdef DEBUG_MESSAGE
		WriteLogEntry(MESSAGE_LOG_SEND_NETWORK, "PostMessage Route to Host Root",
				*group, NULL, NULL, NULL, false, NULL);
		d_sendDepth++;
#endif
		CMessagePair* pair = (*group)->d_pairHead;
		while(pair != NULL)
		{
#ifdef DEBUG_MESSAGE
			WriteLogEntry(MESSAGE_LOG_SEND_NETWORK,
					"PostMessage Route to Host Pair", *pair, NULL, false, NULL);
			d_sendDepth++;
#endif
			pair->PostMessage(i_data, i_sender, pair->d_owner.IsMaster() ?
					MESSAGE_HANDLE_IF_MASTER : MESSAGE_HANDLE_IF_DUPLICA);
			pair = pair->d_groupNext;
#ifdef DEBUG_MESSAGE
			d_sendDepth--;
#endif
		}
#ifdef DEBUG_MESSAGE
		d_sendDepth--;
#endif
	}
}

/***************************************************************************/
/***************************************************************************/
void CMessageDispatcher::PostMessage(void* i_data, ActorHandle i_sender,
		CMessagePair* i_pair)
{
	if (i_pair->d_owner.IsMaster())
	{
		uint32 routeRules = i_pair->PostMessage(i_data, i_sender,
				MESSAGE_HANDLE_IF_MASTER);

		// HACK!!! provide mechanism to block outgoing messages.
		if (d_blockOutgoing)
			return;

		if (routeRules & (MESSAGE_ROUTE_TO_DUPLICAS | MESSAGE_ROUTE_TO_NEIGHBORS))
			PostToDuplicas(i_data, i_sender, i_pair);
	}
	else
	{
		uint32 routeRules = i_pair->PostMessage(i_data, i_sender,
				MESSAGE_HANDLE_IF_DUPLICA);

		// HACK!!! provide mechanism to block outgoing messages.
		if (d_blockOutgoing)
			return;

		if (routeRules & MESSAGE_ROUTE_TO_NEIGHBORS)
			PostToNeighbors(i_data, i_sender, i_pair);
		else if (routeRules & MESSAGE_ROUTE_TO_MASTER)
			PostToMaster(i_data, i_sender, i_pair);
	}
}

/***************************************************************************/
/***************************************************************************/
CMessageGroup* CMessageDispatcher::GetGroup(const char* i_name)
{
	CMessageGroup** group = d_groupTable.Get(i_name);
	if ((group == NULL) || (*group == NULL))
	{
#ifndef CDROM
		CLinearHash<CMessageGroup*>::CIterator it;
		it.Init(&d_groupTable);
		while(!it.End())
		{
			CMessageGroup** testGroup = it.Get();
			ASSERTF((*testGroup == NULL) || stricmp((*testGroup)->d_name, i_name),
					("New message name (%s) differs only by case (%s).",
					i_name, (*testGroup)->d_name));
			++it;
		}
#endif
		CMessageGroup* newGroup;
		MEM_SET_ALLOC_NAME((char*)i_name);
		newGroup = (CMessageGroup*)memAlloc(sizeof(CMessageGroup) +
				strlen(i_name));
		ASSERT_PTR(newGroup);
		new(newGroup) CMessageGroup(i_name);
		if (group == NULL)
			group = d_groupTable.Add(i_name, newGroup, newGroup->d_name);
		else
			d_groupTable.Set(i_name, newGroup, newGroup->d_name);
#ifdef DEBUG_MESSAGE
		WriteLogEntry(MESSAGE_LOG_MEMORY, "Group Create", newGroup, NULL, NULL,
				NULL, true, NULL);
#endif
	}
	return(*group);
}

#ifdef DEBUG_MESSAGE
#if defined(WIN32) && !defined(_XBOX)
#define MESSAGE_PRINT g_console.Echo
#else
#define MESSAGE_PRINT dbgPrint
#endif
#endif

/***************************************************************************/
/***************************************************************************/
void CMessageDispatcher::DumpDebug(void)
{
#ifdef DEBUG_MESSAGE
	MESSAGE_PRINT("\n");
	MESSAGE_PRINT("Message Stats\n");
	MESSAGE_PRINT("------- -----\n\n");
	MESSAGE_PRINT("DC S R  +S   -S   +R   -R  Name\n");
	MESSAGE_PRINT("-- - - ---- ---- ---- ---- ----\n");

	CLinearHash<CMessageGroup*>::CIterator it;
	it.Init(&d_groupTable);
	while(!it.End())
	{
		CMessageGroup** group = it.Get();

		uint32 numCaseDup = 0;
		CLinearHash<CMessageGroup*>::CIterator itCase;
		itCase.Init(&d_groupTable);
		while(!itCase.End())
		{
			CMessageGroup** groupCase = itCase.Get();
			if (groupCase != group)
			{
				if (!stricmp((*groupCase)->d_name, (*group)->d_name))
					numCaseDup++;
			}
			++itCase;
		}

		if (numCaseDup > 0)
			MESSAGE_PRINT("%2d %c %c %4d %4d %4d %4d %s\n", numCaseDup,
					((*group)->d_sent && !(*group)->d_registered) ? 'x' : ' ',
					(!(*group)->d_sent && (*group)->d_registered) ? 'x' : ' ',
					(*group)->d_sendBeforeRegister, (*group)->d_sendAfterRegister,
					(*group)->d_registerBeforeSend, (*group)->d_registerAfterSend,
					(*group)->d_name);
		else
			MESSAGE_PRINT("   %c %c %4d %4d %4d %4d %s\n",
					((*group)->d_sent && !(*group)->d_registered) ? 'x' : ' ',
					(!(*group)->d_sent && (*group)->d_registered) ? 'x' : ' ',
					(*group)->d_sendBeforeRegister, (*group)->d_sendAfterRegister,
					(*group)->d_registerBeforeSend, (*group)->d_registerAfterSend,
					(*group)->d_name);

		++it;
	}

	MESSAGE_PRINT("\nKey\n---\n");
	MESSAGE_PRINT("DC - Number other message names differing only by case.\n");
	MESSAGE_PRINT(" S - Messages sent to but no handlers registered with name.\n");
	MESSAGE_PRINT(" R - Handlers registered with but no messages sent to name.\n");
	MESSAGE_PRINT("+S - Number of messages sent to name before a handler is registered.\n");
	MESSAGE_PRINT("-S - Number of messages sent to name after a handler is registered.\n");
	MESSAGE_PRINT("+R - Number of handlers registered with name before a message is sent.\n");
	MESSAGE_PRINT("+R - Number of handlers registered with name after a message is sent.\n");
#endif
}

/***************************************************************************/
/***************************************************************************/
void CMessageDispatcher::EnableLog(bool i_writeLog)
{
#ifdef DEBUG_MESSAGE
	d_writeLog = i_writeLog;
#endif
}

/***************************************************************************/
/***************************************************************************/
void CMessageDispatcher::ToggleLog(void)
{
#ifdef DEBUG_MESSAGE
	d_writeLog = !d_writeLog;
#endif
}

/***************************************************************************/
/***************************************************************************/
void CMessageDispatcher::SetLogFilter(uint8 i_logFilter)
{
#ifdef DEBUG_MESSAGE
	d_logFilter = i_logFilter;
#endif
}

#ifdef DEBUG_MESSAGE

/***************************************************************************/
/***************************************************************************/
void CMessageDispatcher::WriteLogEntry(uint8 i_category, const char* i_action,
		CMessageHandler &i_handler, bool i_writeLocation, const char* i_notes)
{
	if (i_handler.d_pair != NULL)
		WriteLogEntry(i_category, i_action, *i_handler.d_pair, &i_handler,
				i_writeLocation, i_notes);
	else
		WriteLogEntry(i_category, i_action, NULL, NULL, NULL, &i_handler,
				i_writeLocation, i_notes);
}

/***************************************************************************/
/***************************************************************************/
void CMessageDispatcher::WriteLogEntry(uint8 i_category, const char* i_action,
		CMessagePair &i_pair, CMessageHandler* i_handler, bool i_writeLocation,
		const char* i_notes)
{
	WriteLogEntry(i_category, i_action, &i_pair.d_group, &i_pair.d_owner,
			&i_pair, i_handler, i_writeLocation, i_notes);
}

/***************************************************************************/
/***************************************************************************/
void CMessageDispatcher::WriteLogEntry(uint8 i_category, const char* i_action,
		CMessageGroup* i_group, CMessageOwner* i_owner, CMessagePair* i_pair,
		CMessageHandler* i_handler, bool i_writeLocation, const char* i_notes)
{
	if (d_writeLog && ((i_category & d_logFilter) != 0) && !d_logFile.IsInvalid())
	{
		uint index;
		if (!d_logFile.IsOpen())
		{
			char rootName[] = "Message.txt";
			const char* name = g_logUtil.Enabled() ?
					g_logUtil.MakeFullName(rootName) : rootName;
			if (!d_logFile.Open(name, 12, true))
				return;

			index = 0;
			d_logFile.LabelRecordNumberField("Record");
			d_logFile.SetField(index++, "Time");
			d_logFile.SetField(index++, "Category");
			d_logFile.SetField(index++, "Active");
			d_logFile.SetField(index++, "Action");
			d_logFile.SetField(index++, "Group");
			d_logFile.SetField(index++, "Owner");
			d_logFile.SetField(index++, "&Owner");
			d_logFile.SetField(index++, "&Pair");
			d_logFile.SetField(index++, "&Handler");
			d_logFile.SetField(index++, "File");
			d_logFile.SetField(index++, "Line");
			d_logFile.SetField(index++, "Notes");

			d_logFile.Print();
		}

		index = 0;
		d_logFile.SetField(index, (int)(ValidSingleton(CSession) ?
				g_session.GetSessionTimeMS() : g_timer.GetStartMS()));
		index++;
		switch(i_category)
		{
		 case MESSAGE_LOG_CONFIG:
			d_logFile.SetField(index, "Config");
			break;
		 case MESSAGE_LOG_MEMORY:
			d_logFile.SetField(index, "Memory");
			break;
		 case MESSAGE_LOG_SEND_LOCAL:
			d_logFile.SetField(index, "SendLocal");
			break;
		 case MESSAGE_LOG_SEND_NETWORK:
			d_logFile.SetField(index, "SendNetwork");
			break;
		 default:
			ASSERT(false);
			break;
		}
		index++;
		if ((i_pair != NULL) && i_pair->d_active)
			d_logFile.SetField(index, "*");
		index++;
		d_logFile.SetField(index, i_action, 2 * d_sendDepth);
		index++;
		if (i_group != NULL)
			d_logFile.SetField(index, i_group->d_name);
		index++;
		if (i_owner != NULL)
		{
			if (i_owner->d_ownerName[0] != '\0')
				d_logFile.SetField(index, i_owner->d_ownerName);
			d_logFile.SetField(index + 1, (uint)i_owner);
		}
		index += 2;
		if (i_pair != NULL)
			d_logFile.SetField(index, (uint)i_pair);
		index++;
		if (i_handler != NULL)
			d_logFile.SetField(index, (uint)i_handler);
		index++;
		if (i_writeLocation)
		{
			d_logFile.SetField(index, s_debugFile);
			d_logFile.SetField(index + 1, s_debugLine);
		}
		index += 2;
		if (i_notes != NULL)
			d_logFile.SetField(index, i_notes);

		d_logFile.Print();
	}
}

#endif

//////////////////////// Basic Data Message Handlers ////////////////////////

/***************************************************************************/
/***************************************************************************/
bool C_MESSAGE_VALUE_MessageHandler::PostData(void *i_data,
		CPostedMessage &i_postedMessage)
{
	i_postedMessage.SetData(i_data);
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool C_MESSAGE_ACTOR_PTR_MessageHandler::PostData(void *i_data,
		CPostedMessage &i_postedMessage)
{
	i_postedMessage.SetData(i_data);
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool C_MESSAGE_ACTOR_HANDLE_MessageHandler::PostData(void *i_data,
		CPostedMessage &i_postedMessage)
{
	i_postedMessage.SetData(i_data);
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool C_MESSAGE_STRING_MessageHandler::PostData(void *i_data,
		CPostedMessage &i_postedMessage)
{
	ASSERT(i_data != NULL);
	void* postData = i_postedMessage.AllocateData(strlen((char*)i_data) + 1);
	strcpy((char*)postData, (char*)i_data);
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool C_MESSAGE_VECTOR3_PTR_MessageHandler::PostData(void *i_data,
		CPostedMessage &i_postedMessage)
{
	ASSERT(i_data != NULL);
	void* postData = i_postedMessage.AllocateData(sizeof(Vector3));
	*((Vector3*)postData) = *((Vector3*)i_data);
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool C_MESSAGE_OWNED_VECTOR3_PTR_MessageHandler::PostData(void *i_data,
		CPostedMessage &i_postedMessage)
{
	ASSERT(i_data != NULL);
	void* postData = i_postedMessage.AllocateData(sizeof(Vector3));
	*((Vector3*)postData) = *((Vector3*)i_data);
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool C_MESSAGE_OWNED_POSITION_PTR_MessageHandler::PostData(void *i_data,
		CPostedMessage &i_postedMessage)
{
	ASSERT(i_data != NULL);
	void* postData = i_postedMessage.AllocateData(sizeof(Vector3));
	*((Vector3*)postData) = *((Vector3*)i_data);
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool C_MESSAGE_MATRIX3X3_PTR_MessageHandler::PostData(void *i_data,
		CPostedMessage &i_postedMessage)
{
	ASSERT(i_data != NULL);
	void* postData = i_postedMessage.AllocateData(sizeof(Matrix3x3));
	*((Matrix3x3*)postData) = *((Matrix3x3*)i_data);
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool C_MESSAGE_OWNED_ORIENTATION_PTR_MessageHandler::PostData(void *i_data,
		CPostedMessage &i_postedMessage)
{
	ASSERT(i_data != NULL);
	void* postData = i_postedMessage.AllocateData(sizeof(Matrix3x3));
	*((Matrix3x3*)postData) = *((Matrix3x3*)i_data);
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool C_MESSAGE_MATRIX4X4_PTR_MessageHandler::PostData(void *i_data,
		CPostedMessage &i_postedMessage)
{
	ASSERT(i_data != NULL);
	void* postData = i_postedMessage.AllocateData(sizeof(Matrix4x4));
	*((Matrix4x4*)postData) = *((Matrix4x4*)i_data);
	return(true);
}

/***************************************************************************/
/***************************************************************************/
bool C_MESSAGE_OWNED_TRANSFORM_PTR_MessageHandler::PostData(void *i_data,
		CPostedMessage &i_postedMessage)
{
	ASSERT(i_data != NULL);
	void* postData = i_postedMessage.AllocateData(sizeof(Matrix4x4));
	*((Matrix4x4*)postData) = *((Matrix4x4*)i_data);
	return(true);
}
