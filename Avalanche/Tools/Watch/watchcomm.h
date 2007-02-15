//===================================================================
//  File:  psx2comm.cpp
//  Description:
//     Communication piece with playstation II.
//===================================================================

#ifndef __WATCHCOMM_H__
#define __WATCHCOMM_H__

#include "watchInfo.h"

class CWatchView;

// Communication class
class WatchComm
{
public:
	static WatchComm* Instance(void);
	static void DeleteInstance(void);

	WatchComm(void) { pView = NULL; sock = -1; }
	~WatchComm(void);

	bool ConnectPSX2(void);
	bool ConnectPC(void);
	bool ConnectXbox(void);
	bool Connect(sockaddr_in&);
	void MakeDeci2Hdr(DECI2_HDR *hdr, u_short l, u_short p, u_char s, u_char d);
	int SendPacket(PKT *pkt);
	int SendPacket(int val);
	int RecvPacket(char* buf);
	int ShowError(char*);
	PKT* MakeVal(int);

	int Send(int);
	bool GetNack(void);

	void InitHosts(char*, char*);
	void InitHost(char*, char*, sockaddr_in&);
	void SetView(CWatchView* v) {pView = v;}

	void Idle(void);
	void Packet(PKT* pkt);

	void ChangeVariable(void*, VAR_VAL, int, void*, int);
	void RequestTableData(void* table, int row, int column);

protected:
	// Our socket
	int		sock;

	// Our host
	typedef enum
	{
		HOST_LOCAL,
		HOST_PS2,
		HOST_XBOX,
		HOST_HOSTS
	} HOST_PLATFORM;
	struct sockaddr_in host[HOST_HOSTS];

	static WatchComm* instance;

	CWatchView* pView;
};

#endif // __PSX2COMM_H__
