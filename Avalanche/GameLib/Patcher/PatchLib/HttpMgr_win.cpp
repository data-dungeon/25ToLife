//------------------------------------------------------------------------------
// HttpMgr_ps2.h
//
// PS2-specific functions to support HttpMgr
//
// Copyright (c) 2004-2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------

#include "HttpMgr.h"
#include "HttpMgr_platform.h"

//------------------------------------------------------------------------------
// PollSocket -- Test the socket to see if it is ready to be read from. Returns
// -1 on error, zero if not ready, and 1 if ready.
int PollSocket(int s)
{
	if (s < 0)
		return -1;

	// Set up the descriptor set
	fd_set rset;
	FD_ZERO(&rset);
	FD_SET(s, &rset);

	// And wait
	return select(s + 1, &rset, NULL, NULL, NULL);
}
