//------------------------------------------------------------------------------
// HttpMgr_ps2.h
//
// PS2-specific functions to support HttpMgr
//
// Copyright (c) 2004-2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------

#include "HttpMgr.h"
#include "HttpMgr_platform.h"

#define HTTPMGR_RECV_TIMEOUT		 3000	// Timeout for recv in ms
#define HTTPMGR_SEND_TIMEOUT		10000	// Timeout for send in ms
#define HTTPMGR_SHUTDOWN_TIMEOUT	 1000	// Timeout for shutdown in ms

//------------------------------------------------------------------------------
// PollSocket -- Test the socket to see if it is ready to be read from. Returns
// -1 on error, zero if not ready, and 1 if ready.
int PollSocket(int s)
{
	sceInetPollFd_t fds;
	fds.cid = s;
	fds.events = sceINET_POLLIN;
	fds.revents = 0;

	return sceInsockPoll(&fds, 1, 1);
}

//------------------------------------------------------------------------------
// Call SocketSetup when starting a thread using the socket library
void SocketSetup(int sock)
{
	// For some reason the PS2 has situations where select (sceInsockPoll) indicates
	// that there is data available for reading, but recv will still hang or get an
	// error. Since we only call recv when there is data ready, we set a short
	// timeout (since Sony lets us; no one else does).
	sceInsockSetRecvTimeout(sock, HTTPMGR_RECV_TIMEOUT);
	sceInsockSetSendTimeout(sock, HTTPMGR_SEND_TIMEOUT);

	// The PS2 also does not have a simple close() function for the socket, instead
	// it insists on finishing the TCP handshakes, even at the expense of hanging
	// forever. This should fix that...
	sceInsockSetShutdownTimeout(sock, HTTPMGR_SHUTDOWN_TIMEOUT);
}

//------------------------------------------------------------------------------
// Call SocketCleanup when terminating a thread that was using the socket library.
void SocketCleanup(void)
{
	sceInsockTerminate(0);
}
