//===================================================================
//  File:  MessageQueue.cpp
//  Description:
//     Message queue for outgoing network messages.
//===================================================================

#include <new.h>
#include "Layers/Memory.h"
#include "WatchClient/MessageQueue.h"

MessageNode::MessageNode(PKT& p)
{
	pkt = p;
	next = 0;
}


MessageQueue::MessageQueue()
{
	first = last = 0;
}

MessageQueue::~MessageQueue()
{
	MessageNode* tmp;
	while (first)
	{
		tmp = first;
		first = first->next;
		delete tmp;
	}
}

void MessageQueue::Add(PKT& p)
{
#ifndef CDROM
	MEM_SET_ONE_SHOT_NEW_NAME("MsgQueue::Add");
	MessageNode* node = new MessageNode(p);
	if (first)
	{
		last->next = node;
		last = node;
	}
	else
	{
		first = last = node;
	}
#endif
}

void MessageQueue::RemoveFirst()
{
	MessageNode* tmp = first;
	if (first == last)
		first = last = 0;
	else
		first = first->next;
	delete tmp;
}
