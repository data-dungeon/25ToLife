//===================================================================
//  File:  MessageQueue.h
//  Description:
//     Message queue for outgoing network messages.
//===================================================================

#include "WatchInfo.h"

class MessageQueue;

class MessageNode
{
public:
//	MessageNode();
	MessageNode(PKT&);
	~MessageNode() {};

protected:
	MessageNode* next;
	PKT pkt;

	friend class MessageQueue;
};

class MessageQueue
{
public:
	MessageQueue();
	~MessageQueue();

	void Add(PKT&);
	PKT* First() {if (first) return &first->pkt; return 0;}
	void RemoveFirst();
	int Empty() {return (first == 0);}

protected:
	MessageNode *first, *last;
};
