//===================================================================
//  File:  WatchClient.h
//  Description:
//     Watch and change variables on a running PSXII application.
//===================================================================
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "WatchClient/WatchClient.h"
#ifdef ENABLE_WATCHCLIENT

#if defined(GCN) && defined(CW)
#include "platform/GCN/GCNMWPort.h"
#endif

#include "Layers/Debug.h"
#include "cmdcon/cmdcon.h"

#include "WatchClient/WatchClientPS2.h"
#include "WatchClient/WatchClientWin32.h"
#include "WatchClient/WatchClientGCN.h"
#include "WatchClient/watchclientxbox.h"
#include "WatchClient/MessageQueue.h"
#include "WatchClient/WatchTable.h"

#include "GameHelper/Message.h"

// number of ACK packets to send to confirm communication with Watch program
#define ACK_PACKET_COUNT	5

// Singleton setup variables.
WatchClient* WatchClient::instance = 0;
WatchClient* WatchClient::Instance()
{
    if (instance == 0)
	 {
        MEM_SET_ONE_SHOT_NEW_NAME("WatchClient");
#ifdef PS2
        instance = new WatchClientPS2();
#elif defined(GCN)
        instance = new WatchClientGCN();
#elif defined(_XBOX)
        instance = new WatchClientXbox(); 
#elif defined (WIN32) && !defined (_XBOX)
        instance = new WatchClientWin32();
#endif
	  }
    return instance;
}

void WatchClient::DeleteInstance()
{
    delete instance;
    instance = 0;
}

WatchClient::WatchClient()
{
	contextCommand = 0;
	bNetActive = false;
	MEM_SET_ONE_SHOT_NEW_NAME("MessageQueue");
	mq = new MessageQueue;
	deltaX = 0;

	/* when communication with watch program is first established (when bNetActive becomes true), we send a number */
	/* of ACK packets to confirm. these used to be added to the message queue in the usual way, but this involves */
	/* calls to new, and for memory leak purposes i want to avoid that. so have an ACK packet count to be sent */
	/* before starting to send packets from the queue */

	nACKPacketCount = ACK_PACKET_COUNT;
	ACKPacket.pktType = PTYPE_ACK;	// don't need to set anything else in packet

	/* confirm structure sizes */

	ASSERT(sizeof(TABLE_LABEL) == PKT_SIZE);
	ASSERT(sizeof(TABLE_DATA) == PKT_SIZE);
	ASSERT(sizeof(LABEL_INFO) == PKT_SIZE);
	ASSERT(sizeof(CONTEXT_INFO) == PKT_SIZE);
	ASSERT(sizeof(VAR_EXTENTS) == PKT_SIZE);
	ASSERT(sizeof(VAR_ADD_XY) == PKT_SIZE);
	ASSERT(sizeof(VAR_UPDATE_WATCH) == PKT_SIZE);
	ASSERT(sizeof(VAR_ADD_WATCH) == PKT_SIZE);
	ASSERT(sizeof(VAR_INFO) == PKT_SIZE);
}

WatchClient::~WatchClient()
{
	/* free memory */

	Reset();

	/* delete message queue */

	delete mq;
}

//----------------------------------------------------------------------
// WatchClient::Run
//    This is called in each run loop of the application.
//----------------------------------------------------------------------
void WatchClient::Run()
{
	ProcessContextCommand();
	int count = watchVars.Count();
	if (count && mq->Empty())
	{
#if defined (WIN32) && !defined (_XBOX)
		PKT pkt;
		pkt.pktType = PTYPE_UPDATEVARWATCH;
		pkt.varUpdateWatch.count = count;
		for (int i = 0; i < count; i++)
		{
			pkt.varUpdateWatch.vals[i] = *watchVars[i];
		}

		// Set the x position in the stripchart.
		if (deltaX)
			pkt.varUpdateWatch.x = *deltaX;
		else
			pkt.varUpdateWatch.x = 0;

		mq->Add(pkt);
#endif
	}

	if (bNetActive)
	{
		/* if we have ACK packets to send, do that first before starting to process message queue */

		if (nACKPacketCount > 0)
		{
			if (SendPacket(ACKPacket.pktType, PKT_SIZE, ACKPacket.text))
				nACKPacketCount--;
		}
		else
		{
			PKT* pkt = mq->First();
			while ((pkt) && (SendPacket(pkt->pktType, PKT_SIZE, pkt->text)))
			{
				/* packet has been sent. this also frees message node, which contains contents of packet */

				mq->RemoveFirst();

				/* on pc, send all pending messages. on other systems, send one at a time */

#if defined (WIN32) && !defined (_XBOX)
				pkt = mq->First();
#else //WIN32
				pkt = NULL;
#endif //WIN32
			}
		}
	}
}

//----------------------------------------------------------------------
// WatchClient::Packet
//    We've received a packet.
//----------------------------------------------------------------------
void WatchClient::Packet(PKT* pkt)
{
	switch (pkt->pktType)
	{
		case PTYPE_CONNECT:
			bNetActive = true;
			break;
		case PTYPE_ACK:
			break;
		case PTYPE_NACK:
			// This is a problem.
			break;
		case PTYPE_VARCHANGE:
			*(float*)pkt->varinfo.address = pkt->varinfo.value.rval;
			if (pkt->varinfo.messageOwner)
			{
				if (pkt->varinfo.messageData == (void*)~0x0)
					g_messageDispatcher.SendMessageToOwner("ConsoleVarModified",
							pkt->varinfo.name, INVALID_OBJECT_HANDLE,
							(CMessageOwner*)pkt->varinfo.messageOwner);
				else
					g_messageDispatcher.SendMessageToOwner("ConsoleVarModified",
							pkt->varinfo.messageData, INVALID_OBJECT_HANDLE,
							(CMessageOwner*)pkt->varinfo.messageOwner);
			}
			break;
		case PTYPE_SAVE_CONTEXT:
		case PTYPE_OPEN_CONTEXT:
			// For the PS2 version we can't process these here because we're
			// being called from within the interrupt handler and any further
			// communication between the PSX at the EE or the PC will hang the
			// playstation.  So we want to save this out and run it on the next
			// run command.
			memcpy(&contextInfo, &pkt->contextInfo, sizeof(contextInfo));
			contextCommand = pkt->pktType;
			break;
		case PTYPE_CONTEXT_REFRESH:
			this->Refresh(contexts[pkt->contextInfo.context]);
			break;
		case PTYPE_REQUEST_TABLE_DATA:
			((WatchTable*)pkt->tableData.table)->RequestData(pkt->tableData.row,
																			 pkt->tableData.column);
			break;
		case PTYPE_TABLE_DATA:
			((WatchTable*)pkt->tableData.table)->SetData(pkt->tableData.row,
																		pkt->tableData.column,
																		pkt->tableData.dataIndex,
																		pkt->tableData.data);
			break;
		case PTYPE_TABLE_ROW_NEW:
		case PTYPE_TABLE_COLUMN_NEW:
			((WatchTable*)pkt->tableData.table)->NewRowColumn(pkt->tableLabel.name,
																			  pkt->tableLabel.isRow == 1);
			break;
		case PTYPE_TABLE_ROW_FILL:
		case PTYPE_TABLE_COLUMN_FILL:
			((WatchTable*)pkt->tableData.table)->FillRowColumn(pkt->tableData);
			break;
	}
}

void WatchClient::Print(char* format, ...)
{
	char msg[1024];
	va_list args;
	va_start(args, format);
	vsprintf(msg, format, args);
	va_end(args);
  	SendPacket(PTYPE_TEXT, strlen(msg)+1, msg);
}

//------------------------------------------------------------------------
// WatchClient::RegisterVariable
//    Tell the watch program that we have a variable we want to modify.
//------------------------------------------------------------------------
void WatchClient::RegisterVariable(char* context, float* var, float val, float varLow,
											  float varHigh, char* txt,
											  CMessageOwner *i_messageOwner, void *i_messageData)
{
	VAR_VAL value;
	value.rval = val;

	RegisterVariable(context, VARTYPE_REAL, var, value, varLow, varHigh, txt, i_messageOwner, i_messageData);
}

//------------------------------------------------------------------------
// WatchClient::RegisterVariable
//    Tell the watch program that we have a variable we want to modify.
//------------------------------------------------------------------------
void WatchClient::RegisterVariable(char* context, int* var, int val, float varLow,
											  float varHigh, char* txt,
											  CMessageOwner *i_messageOwner, void *i_messageData)
{
	VAR_VAL value;
	value.ival = val;

	RegisterVariable(context, VARTYPE_INT, var, value, varLow, varHigh, txt, i_messageOwner, i_messageData);
}

//------------------------------------------------------------------------
// WatchClient::RegisterVariable
//    Tell the watch program that we have a variable we want to modify.
//------------------------------------------------------------------------
void WatchClient::RegisterVariable(char* context, char* var, char *val, char* txt,
											  CMessageOwner *i_messageOwner, void *i_messageData)
{
	VAR_VAL value;
	SetVARVAL(value, val);

	RegisterVariable(context, VARTYPE_STRING, var, value, 0.0f, 1.0f, txt, i_messageOwner, i_messageData);
}

//------------------------------------------------------------------------
// WatchClient::RegisterVariable
//    Tell the watch program that we have a variable we want to modify.
//------------------------------------------------------------------------
void WatchClient::RegisterVariable(char* context, int type, void* var, VAR_VAL val,
											  float varLow, float varHigh, char* txt,
											  CMessageOwner *i_messageOwner, void *i_messageData)
{
	int contextIdx = FindContext(context);
	
	AddRefreshVariable(contextIdx, var, type);
	
	// Create the packet and put it in the queue.
  	PKT pkt;
	pkt.pktType = PTYPE_VARINFO;
  	pkt.varinfo.address = var;
  	pkt.varinfo.type = type;
  	pkt.varinfo.value = val;
  	pkt.varinfo.max = varHigh;
  	pkt.varinfo.min = varLow;
	pkt.varinfo.messageOwner = i_messageOwner;
	pkt.varinfo.messageData = i_messageData;
	pkt.varinfo.context = contextIdx;
  	strncpy(pkt.varinfo.name, txt, VARINFO_NAME_SIZE-1);
	mq->Add(pkt);
}

//------------------------------------------------------------------------
// WatchClient::RegisterTable
//    Notify the watch program of incoming table data.
//------------------------------------------------------------------------
void WatchClient::RegisterTable(char* context, WatchTable* table,
										  CMessageOwner *i_messageOwner, void *i_messageData)
{
	int contextIdx = FindContext(context);

	AddRefreshVariable(contextIdx, table, VARTYPE_TABLE);

	// Create the packet and put it in the queue.
  	PKT pkt;
	pkt.pktType = PTYPE_VARINFO;
  	pkt.varinfo.address = table;
  	pkt.varinfo.type = VARTYPE_TABLE;
	pkt.varinfo.messageOwner = i_messageOwner;
	pkt.varinfo.messageData = i_messageData;
	pkt.varinfo.context = contextIdx;
	mq->Add(pkt);
}

//------------------------------------------------------------------------
// WatchClient::RegisterTableVariable
//    Tell the watch program that we have a variable we want to associate
//    with a table.
//------------------------------------------------------------------------
void WatchClient::RegisterTableVariable(char* context, WatchTable* table, int type,
													 float varLow, float varHigh, int idx, char* txt,
													 CMessageOwner *i_messageOwner, void *i_messageData)
{
	// Create the packet and put it in the queue.
  	PKT pkt;
	pkt.pktType = PTYPE_TABLE_VARIABLE;
  	pkt.varinfo.address = table;
  	pkt.varinfo.type = type;
  	pkt.varinfo.max = varHigh;
  	pkt.varinfo.min = varLow;
	pkt.varinfo.messageOwner = i_messageOwner;
	pkt.varinfo.messageData = i_messageData;
	pkt.varinfo.context = FindContext(context);
	pkt.varinfo.index = idx;
  	strncpy(pkt.varinfo.name, txt, VARINFO_NAME_SIZE-1);
	mq->Add(pkt);
}

//------------------------------------------------------------------------
// WatchClient::RegisterLabel
//    Tell the watch program what the labels are for a 1d or 2d table.
//------------------------------------------------------------------------
void WatchClient::RegisterLabel(char* context, WatchTable* table, int idx, char* txt,
										  bool bRow)
{
	PKT pkt;
	pkt.pktType = PTYPE_TABLE_LABEL;
	pkt.labelInfo.context = FindContext(context);
	pkt.labelInfo.table = table;
	pkt.labelInfo.idx = idx;
	pkt.labelInfo.row = (int)bRow;
	strncpy(pkt.labelInfo.name, txt, LabelNameSize);
	mq->Add(pkt);
}

void WatchClient::AddRefreshVariable(int contextId, void* address, int type)
{
#ifndef CDROM
	// Save the variable in the variable list.
	VAR_REFRESH *variable;
	MEM_SET_ONE_SHOT_NEW_NAME("Watch::AddRefreshVar");
	variable = new VAR_REFRESH;
	variable->address = address;
	variable->type = type;
	variable->context = contextId;
	variables[contextId].Add(variable);
#endif
}

//----------------------------------------------------------------------
// WatchClient::WatchVariable
//    Tell the watch program that we have a variable we want to track
//    in the strip charting tool.
//----------------------------------------------------------------------
void WatchClient::WatchVariable(float* var, char* name)
{
	// We only handle about 25 variables
	if (VAR_WATCH_COUNT == watchVars.Count())
		return;

	// Add the address to our list.
	watchVars.Add(var); // This must be before the Count() call

	// Add the packet to the queue.
	PKT pkt;
	pkt.pktType = PTYPE_ADDVARWATCH;
	pkt.varAddWatch.index = watchVars.Count()-1;
	strncpy(pkt.varAddWatch.name, name, PKT_SIZE - 5);
	mq->Add(pkt);
}

void WatchClient::SendXY(int variable, float x, float y)
{
#if defined (_WIN32)
	PKT pkt;
	pkt.pktType = PTYPE_SENDXY;
	pkt.varAddXY.variable = variable;
	pkt.varAddXY.count = 1;
	pkt.varAddXY.vals[0].x = x;
	pkt.varAddXY.vals[0].y = y;
	SendPacket(pkt.pktType, PKT_SIZE, pkt.text);
#else
	conPuts("  *** SendXY doesn't work on the Playstation.");
#endif
}

void WatchClient::SendXY(int variable, int count, float* x, float* y)
{
#if defined (_WIN32)
	PKT pkt;
	pkt.pktType = PTYPE_SENDXY;
	pkt.varAddXY.variable = variable;
	int j = 0;
	for (int i = 0; i < count; i++)
	{
		pkt.varAddXY.vals[j].x = x[i];
		pkt.varAddXY.vals[j].y = y[i];
		j++;


		if (j >= ADD_XY_MAX)
		{
			pkt.varAddXY.count = j;
			j = 0;
			SendPacket(pkt.pktType, PKT_SIZE, pkt.text);
		}
	}
	if (j > 0)
	{
		pkt.varAddXY.count = j;
		SendPacket(pkt.pktType, PKT_SIZE, pkt.text);
	}
#else
	conPuts("  *** SendXY doesn't work on the Playstation.");
#endif
}

void WatchClient::SetPlotExtents(float xmin, float xmax, float ymin, float ymax)
{
	PKT pkt;
	pkt.pktType = PTYPE_EXTENTS;
	pkt.varExtents.xmin = xmin;
	pkt.varExtents.xmax = xmax;
	pkt.varExtents.ymin = ymin;
	pkt.varExtents.ymax = ymax;
	SendPacket(pkt.pktType, PKT_SIZE, pkt.text);
}

void WatchClient::Clear()
{
	SendPacket(PTYPE_CLEAR, 0, NULL);
}

int WatchClient::FindContext(char* name)
{
#ifndef CDROM
	for (int i = 0; i < contexts.Count(); i++)
	{
		if (0 == strcmpi(name, contexts[i]))
			return i;
	}


	// It's not there.  Add it.
	MEM_SET_ONE_SHOT_NEW_NAME("Watch::FindContext");
	char* tmp = new char[strlen(name)+1];
	strcpy(tmp, name);
	contexts.Add(tmp);
	ASSERT(0 == strcmpi(name, contexts[contexts.Count()-1]));
	int context = contexts.Count()-1;

	printf("-------  Add context %d: %s\n", context, name);

	PKT pkt;
	pkt.pktType = PTYPE_CONTEXT_INFO;
	pkt.contextInfo.context = context;
	strncpy(pkt.contextInfo.name, name, CONTEXT_NAME_SIZE-1);
	mq->Add(pkt);
	return context;
#else
	return NULL;
#endif
}

void WatchClient::Refresh(char* context)
{
	// Create the packet and put it in the queue.
  	PKT pkt;
	pkt.pktType = PTYPE_VAR_REFRESH;

	int contextIdx = FindContext(context);
	for (int i = 0; i < variables[contextIdx].Count(); i++)
	{
		pkt.varinfo.address = variables[contextIdx][i]->address;
		pkt.varinfo.type = variables[contextIdx][i]->type;
		ASSERT(contextIdx == variables[contextIdx][i]->context);
		pkt.varinfo.context = contextIdx;
		pkt.varinfo.value.rval = *((float*)(pkt.varinfo.address));
		mq->Add(pkt);
	}
}

void WatchClient::ProcessContextCommand()
{
	switch(contextCommand)
	{
		case PTYPE_SAVE_CONTEXT:
		{
			if (contextInfo.table)
			{
				((WatchTable*)contextInfo.table)->Save(contextInfo.name);
			}
			else
			{
				char name[256];
				strcpy(name, "./");
				strcat(name, contextInfo.name);
				g_console.SaveContext(contexts[contextInfo.context], name);
			}
			break;
		}
		case PTYPE_OPEN_CONTEXT:
		{
			if (contextInfo.table)
			{
				((WatchTable*)contextInfo.table)->Open(contextInfo.name);
			}
			else
			{
				char filename[256];
				sscanf(contextInfo.name, "%[^.]s", &filename);
				g_console.ExecuteFile(filename);
				this->Refresh(contexts[contextInfo.context]);
			}
			break;
		}
	}
	contextCommand = 0;  // We've processed this command.
}

//----------------------------------------------------------------------
// WatchClient::TableData
//    Send table data to the watch program.
//----------------------------------------------------------------------
void WatchClient::TableData(void* table, int row, int column, VAR_VAL* data, int count)
{
	for (int i = 0; i < count; i++)
	{
		PKT pkt;
		pkt.pktType = PTYPE_TABLE_DATA;
		pkt.tableData.table = table;
		pkt.tableData.row = row;
		pkt.tableData.column = column;
		pkt.tableData.dataIndex = i;
		pkt.tableData.data = data[i];
		mq->Add(pkt);
	}
}

/* clear out pending messages. ned added this to clean up memory leaks. watchclient packets get added to a message queue,
but are only sent when the bNetActive variable is TRUE (presumably when we detect that the Watch program has hooked up). if
Watch is never run, then the messages keep queuing up forever. specifically, new messages are added to the queue when a
new level is started. so when a level ends, we should remove unsent packets.

also, clean up all watch client contexts, variables, and watchVars-- they are level-specific */

void WatchClient::Reset()
{
	/* don't have to remove packets if Watch is running-- we will keep sending them and removing them, don't */
	/* want to possibly miss one */

	if (bNetActive == FALSE)
	{
		/* remove all pending packets */

		while (mq->First())
			mq->RemoveFirst();

		/* reset ACK packet count */

		nACKPacketCount = ACK_PACKET_COUNT;
	}

	/* remove existing contexts */
	
	int i;
	for (i = 0; i < contexts.Count(); i++)
		delete contexts[i];
	contexts.Reset();	// free list memory

	/* remove existing variables */

	int j;
	for (i = 0; i < watchMaxContexts; i++)
	{
		for (j = 0; j < variables[i].Count(); j++)
			delete variables[i][j];
		variables[i].Reset();	// free list memory
	}

	/* WatchVars is dynamic array of float pointers, but they are not pointers to new'd floats, so no */
	/* need to delete */

	watchVars.Reset();	// free list memory
}

#endif //ENABLE_WATCHCLIENT
	
