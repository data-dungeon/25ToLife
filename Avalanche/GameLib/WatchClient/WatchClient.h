//===================================================================
//  File:  WatchClient.h
//  Description:
//     This is the client side of the Watch tool.  This will allow
//     the developer to change and watch variables on a running
//     PSXII application.
//===================================================================

#ifndef __WATCHCLIENT_H__
#define __WATCHCLIENT_H__

/*** we can turn the whole thing off ***/

#define ENABLE_WATCHCLIENT
#if defined( CONSUMER_BUILD ) || defined( CDROM ) || defined( PS2 ) || defined( GCN ) || defined (_XBOX)
#undef ENABLE_WATCHCLIENT
#endif //CONSUMER_BUILD

#ifdef ENABLE_WATCHCLIENT

#include <stdarg.h>
#include "container/Darray.h"

#include "WatchClient/WatchInfo.h"

class WatchTable;

// Forwards
class MessageQueue;
class CMessageOwner;

const int watchMaxContexts = 70;

class WatchClient
{
// Setup
protected:
	WatchClient();
	virtual ~WatchClient();

// Member functions
public:
	static WatchClient* Instance();
	static void DeleteInstance();

	virtual void Start() {};
	virtual void End() {};
	virtual void Run();
	virtual bool SendPacket(int, int, char*) {return false;}
	virtual void SendPacketWait(int, int, char*) {}
	
	virtual void Packet(PKT*);
	
	void Print(char*, ...);
	// All the incarnations of RegisterVariable.
	void RegisterVariable(char* context, float* var, float val, float varLow,
								 float varHigh, char* txt,
								 CMessageOwner *i_messageOwner = NULL,
								 void *i_messageData = (void*)~0x0);
	void RegisterVariable(char* context, int* var, int val, float varLow,
								 float varHigh, char* txt,
								 CMessageOwner *i_messageOwner = NULL,
								 void *i_messageData = (void*)~0x0);
	void RegisterVariable(char* context, char* var, char *val, char* txt,
								 CMessageOwner *i_messageOwner = NULL,
								 void *i_messageData = (void*)~0x0);
	void RegisterVariable(char* context, int type, void* var, VAR_VAL val,
								 float varLow, float varHigh, char* txt,
								 CMessageOwner *i_messageOwner = NULL,
								 void *i_messageData = (void*)~0x0);
	void RegisterTable(char* context, WatchTable* table,
							 CMessageOwner *i_messageOwner = NULL,
							 void *i_messageData = (void*)~0x0);
	void RegisterTableVariable(char* context, WatchTable* table, int type,
										float varLow, float varHigh, int idx, char* txt,
										CMessageOwner *i_messageOwner = NULL,
										void *i_messageData = (void*)~0x0);
	void RegisterColumnLabel(char* context, WatchTable* table, int idx, char* txt);
	void RegisterRowLabel(char* context, WatchTable* table, int idx, char* txt);
	void RegisterLabel(char* context, WatchTable* table, int idx, char* txt, bool bRow);
	void TableData(void* table, int, int, VAR_VAL* data, int count);
	void WatchVariable(float* var, char* name);
	void SetStripX(float* x) {deltaX = x;}
	void SendXY(int variable, float x, float y);
	void SendXY(int variable, int count, float* x, float* y);
	void Clear();
	void SetPlotExtents(float xmin, float xmax, float ymin, float ymax);
	void AddRefreshVariable(int contextId, void* address, int type);

	int  FindContext(char*);
	void Refresh(char* context);

	void Reset();	// reset when game engine is reset

	// Debug stuff.
//	SimLoop* simloop;

protected:
	void ProcessContextCommand();

// Attributes
protected:
	static WatchClient* instance;

	bool bNetActive;

	DynamicArray<float*> watchVars;
	DynamicArray<char*> contexts;
	DynamicArray<VAR_REFRESH*> variables[watchMaxContexts];  // Limited to 64 contexts.
	MessageQueue* mq;

	// number of ACK packets to be sent before sending packets from the queue
	int nACKPacketCount;
	PKT ACKPacket;

	// Needed for watching variables in the strip chart.
	float* deltaX;

	// Signal to do something at the next run loop
	int contextCommand;
	CONTEXT_INFO contextInfo;
};

//----------------------------------------------------------------------
// WatchClient::RegisterColumnLabel
//    Register a column label for a table in watch.
//----------------------------------------------------------------------
inline void WatchClient::RegisterColumnLabel(char* context, WatchTable* table,
															int idx, char* txt)
{
	RegisterLabel(context, table, idx, txt, false);
}

//----------------------------------------------------------------------
// WatchClient::RegisterRowLabel
//    Register a row label for a table in watch.
//----------------------------------------------------------------------
inline void WatchClient::RegisterRowLabel(char* context, WatchTable* table,
														int idx, char* txt)
{
	RegisterLabel(context, table, idx, txt, true);
}

#endif //ENABLE_WATCHCLIENT
#endif // __WATCHCLIENT_H__
