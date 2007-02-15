//------------------------------------------------------------------------------
// HttpMgr.h
//
// Very simplified fetch-one-URL-at-a-time http class.
//
// Original Author:					David Goodenough, Crystal Dynamics
// Modified for Updater library:	Dan Stanfill, Pinniped Software
//									Under contract to Eidos, Inc.
// 
// Copyright (c) 2004-2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------
#ifndef __HTTPMGR_H__
#define __HTTPMGR_H__ 1

#include "PatcherCommon/patchplatform.h"

// Manages HTTP downloads.
//
class HttpMgr
{
public:
	// If this engine/platform requires it, do its per-class preamble
	PLATFORM_PREAMBLE;

	static void Create();
	static void Destroy();
	static HttpMgr* GetPtr() { return &s_instance; }

	int		FindHost(const char* host);
	int		RequestHTTP(const char* host, const char* name, int timeout);
	int		ReceiveAll(int& received, int& datasize, char* buffer, int bufsize, bool truncateOK);
	bool	CheckStatus(int& received, int& datasize, char *buffer, float elapsedTime, int& status);
	void	Abort();

private:
	void*				m_memoryPool;

	float				m_timeout;
	float				m_curTimeout;
	int					m_socket;
	unsigned int		m_addr;
	int					m_incount;
	int					m_inused;
	char 				m_buffer[256];
	char 				m_header[256];
	int 				m_receivingBody;

private:
	HttpMgr();
	~HttpMgr();

	static HttpMgr				s_instance;
};

#endif // __HTTPMGR_H__
