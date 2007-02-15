//===================================================================
//  File:  psx2comm.cpp
//  Description:
//     Communication piece with playstation II.
//===================================================================

#include "stdafx.h"
#include "resource.h"
#include "WatchDoc.h"
#include "WatchView.h"
#include "watchcomm.h"
#include "stdio.h"

#define DECI2_PROTO_NETMP	0x0400	/* Net Manager */
typedef struct {
	u_char code;
	u_char result;
} NETMP_HDR;

/* Message Code */
#define NETMP_CODE_CONNECT		0
#define NETMP_CODE_CONNECTR	1
#define NETMP_CODE_RESET		2
#define NETMP_CODE_RESETR		3
#define NETMP_CODE_MESSAGE		4
#define NETMP_CODE_MESSAGER	5
#define NETMP_CODE_STATUS 		6
#define NETMP_CODE_STATUSR		7
#define NETMP_CODE_KILL			8
#define NETMP_CODE_KILLR		9
#define NETMP_CODE_VERSION		10
#define NETMP_CODE_VERSIONR	11

/* Result Code */
#define NETMP_ERR_OK				0
#define NETMP_ERR_INVAL			1
#define NETMP_ERR_BUSY			2
#define NETMP_ERR_NOTCONN		3
#define NETMP_ERR_ALREADYCONN	4
#define NETMP_ERR_NOMEM			5
#define NETMP_ERR_NOPROTO		6

typedef struct {
	u_short reserved;
	u_int param[4];
} NETMP_BOOTP;

typedef struct {
	u_char  pri;
	u_char  reserved;
	u_short proto;
} NETMP_PROTOS;

#define NETMP_PRI_DEFAULT	0x80

static void logit( char* dir, unsigned char* buf, int len )
{
#if 0
	static bool first = true;
	int i;
	FILE* fp;

	if ( first )
	{
		fp = fopen( "c:\\watch.log", "wt" );
		first = false;
	}
	else
	{
		fp = fopen( "c:\\watch.log", "at" );
	}
	
	fprintf( fp, "%s: L=%d [", dir, len );
	for ( i = 0 ; i < len ; i++ )
		fprintf( fp, " %02X", buf[ i ] );
	fprintf( fp, " ]\n" );

	fclose( fp );
#endif
}

// Singleton setup variables.
WatchComm* WatchComm::instance = 0;
WatchComm* WatchComm::Instance(void)
{
	if (instance == 0)
		instance = new WatchComm();
	return instance;
}

void WatchComm::DeleteInstance(void)
{
	delete instance;
	instance = 0;
}

WatchComm::~WatchComm(void)
{
	if (sock != -1)
		closesocket(sock);
}

void WatchComm::InitHosts(
char* local,
char* remote)
{
	// Init windows sockets
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
	{
		TRACE("WSAStarup() fail   %d\n", WSAGetLastError());
	}

	// Init our hosts
	InitHost(local, DEFAULT_PORT, host[HOST_LOCAL]);
	InitHost(remote, DEFAULT_PORT, host[HOST_PS2]);
	InitHost(remote, DEFAULT_PORT, host[HOST_XBOX]);

	// create socket
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		TRACE("socket() fail   %d\n", WSAGetLastError());
	}
}

// Fill out the server info
void WatchComm::InitHost(
char* host,
char* strport,
sockaddr_in& server)
{
	u_long addr;
	int port;
	struct hostent *hp;

	// Setup the server record
	ZeroMemory((char *)&server, sizeof(server));
	if ((addr = inet_addr(host)) != -1)
		server.sin_family = AF_INET;
	else
	{
		if ((hp = gethostbyname(host)) == NULL)
		{
			TRACE("gethostbyname() fail\n");
			return;
		}
		addr = *(u_long *)hp->h_addr;
		server.sin_family = hp->h_addrtype;
	}

	// get port number
	if (sscanf(strport, "%d", &port) != 1 || !port)
	{
		TRACE("invalid port number\n");
		return;
	}

	// Finish setting up the host
	server.sin_addr.s_addr = addr;
	server.sin_port = htons(port);
}

bool WatchComm::ConnectPC(void)
{
	return Connect(host[HOST_LOCAL]);
}

bool WatchComm::ConnectPSX2(void)
{
	Connect(host[HOST_PS2]);

	struct
	{
		DECI2_HDR deci2;
		NETMP_HDR netmp;
		NETMP_PROTOS protos;
	} pkt;

	MakeDeci2Hdr(&pkt.deci2, sizeof(pkt), DECI2_PROTO_NETMP,
					 DECI2_NODE_HOST, DECI2_NODE_HOST);
	pkt.netmp.code = NETMP_CODE_CONNECT;
	pkt.netmp.result = 0;
	pkt.protos.pri = NETMP_PRI_DEFAULT;
	pkt.protos.reserved = 0;
	pkt.protos.proto = PROTO_SAMPLE;

	int r = send(sock, (const char *)&pkt, sizeof(pkt), 0);

	return true;
}

bool WatchComm::ConnectXbox(void)
{
	return Connect(host[HOST_XBOX]);
}

bool WatchComm::Connect(sockaddr_in& server)
{
	CWaitCursor wait;
	int test = sizeof(server);
  	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) >= 0)
	{
		Send(PTYPE_CONNECT);
		return true;
	}

	TRACE("connect() fail   %d\n", WSAGetLastError());
	pView->DebugPrint("Connect fail");
	return false;
}

int WatchComm::SendPacket(int val)
{
	PKT* pkt = MakeVal(val);
	int ret = SendPacket(pkt);
	delete pkt;
	return ret;
}

int WatchComm::Send(int type)
{
	PKT pkt;

	MakeDeci2Hdr(&pkt.deci2, sizeof(PKT), PROTO_SAMPLE,
					 DECI2_NODE_HOST, DECI2_NODE_EE);
	pkt.pktType = type;
	int ret = SendPacket(&pkt);
	return ret;
}

bool WatchComm::GetNack(void)
{
	PKT pkt;
//	pkt.data = NULL;

	int count = 0;
	unsigned long n;
	while (1)
	{
		ioctlsocket(sock, FIONREAD, &n);
		if (n > 0)
			break;

		if (count > 20)
			return false;
		count++;
		Sleep(250);
	}
	RecvPacket((char*)&pkt);
	if (pkt.pktType == PTYPE_NACK)
		return true;
	return false;
}


void WatchComm::MakeDeci2Hdr(DECI2_HDR *hdr, u_short l, u_short p, u_char s, u_char d)
{
	hdr->length      = l;
	hdr->reserved    = 0;
	hdr->protocol    = p;
	hdr->source      = s;
	hdr->destination = d;
}

static int SendBytes(SOCKET sock, char* buf, int len)
{
	int sendlen, n;
	for ( sendlen = 0 ; sendlen < len ; sendlen += n )
	{
		n = send(sock, buf + sendlen, len - sendlen, 0);
		if ( n == SOCKET_ERROR )
		{
			// when sending errors occur, it's usually because we can't put the data because the
			// buffers are full.. so wait for consumption and try again.
			Sleep( 1 );
			n = 0;
		}
	}
	return true;
}

int WatchComm::SendPacket(PKT *pkt)
{
	MakeDeci2Hdr(&pkt->deci2, sizeof(PKT), PROTO_SAMPLE, DECI2_NODE_HOST, DECI2_NODE_EE);

	logit( "S", (unsigned char*)&pkt->deci2, pkt->deci2.length );

	return SendBytes( sock, (char*)&pkt->deci2, pkt->deci2.length );
}

static int RecvBytes(SOCKET sock, char* buf, int len)
{
	int recvlen, n;
	for ( recvlen = 0 ; recvlen < len ; recvlen += n )
	{
		n = recv(sock, buf + recvlen, len - recvlen, 0);
		if ( n == SOCKET_ERROR )
		{
			ASSERT( false );
			return false;
		}
	}
	return true;
}

int WatchComm::RecvPacket(char *buf)
{
	DECI2_HDR* pkt = (DECI2_HDR*) buf;

	// get deci2 header part of packet
	if ( !RecvBytes( sock, buf, sizeof(DECI2_HDR) ) )
		return false;

	// verify packet integrity
	if ( pkt->length < sizeof(DECI2_HDR) || pkt->length > sizeof(PKT) )
	{
		ASSERT( false );
		return false;
	}

	// get remaining part of packet
	if ( !RecvBytes( sock, buf + sizeof(DECI2_HDR), pkt->length - sizeof(DECI2_HDR) ) )
		return false;

	logit( "R", (unsigned char*)buf, pkt->length );

	return true;
}

int WatchComm::ShowError(char* err)
{
	TRACE("%s\n", err);
	return 0;
}

PKT* WatchComm::MakeVal(int val)
{
	PKT* pkt;

	if ((pkt=(PKT *)malloc(sizeof(PKT))) == NULL) return NULL;

	MakeDeci2Hdr(&pkt->deci2, sizeof(PKT), PROTO_SAMPLE,
					 DECI2_NODE_HOST, DECI2_NODE_EE);
	pkt->pktType = PTYPE_VAR_UPDATE;
	pkt->val[0] = val;
	return pkt;
}

void WatchComm::Idle(void)
{
	PKT pkt;
	unsigned long n;
	ioctlsocket(sock, FIONREAD, &n);
	while (n > 0)
	{
		RecvPacket((char*)&pkt);
		Packet(&pkt);
		ioctlsocket(sock, FIONREAD, &n);
	}
}

void WatchComm::Packet(PKT* pkt)
{
	static int count = 0;

	switch (pkt->pktType)
	{
		case PTYPE_TEXT:
			pView->DebugPrint(pkt->text);
			break;
		case PTYPE_VARINFO:
			pView->RegisterVariable(pkt->varinfo);
			break;
		case PTYPE_ADDVARWATCH:
			pView->AddVarWatch(pkt->varAddWatch);
			break;
		case PTYPE_UPDATEVARWATCH:
			pView->UpdateVarWatch(pkt->varUpdateWatch);
			break;
		case PTYPE_SENDXY:
			pView->AddXY(pkt->varAddXY);
			break;
		case PTYPE_CLEAR:
			pView->Clear();
			break;
		case PTYPE_EXTENTS:
			pView->PlotDlg()->Extents(pkt->varExtents);
			break;
		case PTYPE_CONTEXT_INFO:
			pView->RegisterContext(pkt->contextInfo);
			break;
		case PTYPE_VAR_REFRESH:
			pView->RefreshVariable(pkt->varinfo);
			break;
		case PTYPE_TABLE_LABEL:
			pView->RegisterLabel(pkt->labelInfo);
			break;
		case PTYPE_TABLE_VARIABLE:
			pView->RegisterTableVariable(pkt->varinfo);
			break;
		case PTYPE_TABLE_DATA:
			pView->TableData(pkt->tableData);
			break;
	}
}

void WatchComm::ChangeVariable(void* ptr, VAR_VAL val, int event, void* eventDest, int type)
{
	PKT pkt;
	pkt.pktType = PTYPE_VARCHANGE;
	pkt.varinfo.type = type;
	pkt.varinfo.address = ptr;
	pkt.varinfo.value.rval = val.rval;
	pkt.varinfo.event = event;
	pkt.varinfo.eventDest = eventDest;
	SendPacket(&pkt);
}

void WatchComm::RequestTableData(void* table, int row, int column)
{
	PKT pkt;
	pkt.pktType = PTYPE_REQUEST_TABLE_DATA;
	pkt.tableData.table = table;
	pkt.tableData.row = row;
	pkt.tableData.column = column;
	SendPacket(&pkt);
}
