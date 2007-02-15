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

// Enable/disable print statements
//#define PATCH_PRINT_ENABLED 1

#include "PatcherCommon/patchplatform.h"
#include "PatcherCommon/PatchHelper.h"
#include "PatchError.h"
#include "HttpMgr.h"
#include "HttpMgr_platform.h"

#define MAXRESLEN 256	// Max allowed string length for resource name

// *******************************************************************************************
HttpMgr HttpMgr::s_instance;

// *******************************************************************************************
void HttpMgr::Create()
{
}

// *******************************************************************************************
HttpMgr::HttpMgr() :
	m_socket(-1), m_addr(INADDR_NONE)
{
}

// *******************************************************************************************
HttpMgr::~HttpMgr()
{
}

// *******************************************************************************************
void HttpMgr::Destroy()
{
	PATCH_PRINT("HttpMgr::Destroy() called...\n");

	s_instance.Abort();

	// Clean up any socket memory left by our thread.
	SocketCleanup();
}

// *******************************************************************************************
// FindHost -- Given a host name, resolve it to an address
int HttpMgr::FindHost(const char* host)
{
	PATCH_PRINT("HttpMgr::FindHost: running\n");

	// This inet_addr check could be removed later, since we are forbidding
	// numerical addresses in a master build...
	m_addr = inet_addr(host);
	if (m_addr == INADDR_NONE)
	{
		PATCH_PRINT("HttpMgr::FindHost: calling gethostbyname...\n");
		struct hostent* hostaddr = gethostbyname(host);
		if (hostaddr == NULL)
		{
			// Get more detailed failure information
			int err = LastHostnameError();
			switch (err) {
				case HOST_NOT_FOUND:		// Name server indicates it can't find the host
					PATCH_PRINT("HttpMgr::FindHost: gethostbyname returned HOST_NOT_FOUND!!\n");
					return PATCH_ERROR_HOSTNOTFOUND;
				case TRY_AGAIN:				// Probably the network connection or name server is down
					PATCH_PRINT("HttpMgr::FindHost: gethostbyname returned TRY_AGAIN!!\n");
					return PATCH_ERROR_NAMESERVER_ERROR;
				case NO_RECOVERY:
					PATCH_PRINT("HttpMgr::FindHost: gethostbyname returned NO_RECOVERY!!\n");
					return PATCH_ERROR_NAMESERVER_ERROR;
				case NO_DATA:
					PATCH_PRINT("HttpMgr::FindHost: gethostbyname returned NO_DATA!!\n");
					return PATCH_ERROR_HOSTNOTFOUND;
				default:
					PATCH_PRINT("HttpMgr::FindHost: gethostbyname returned unknown error %d!!\n", err);
					return PATCH_ERROR_NAMESERVER_ERROR;
			}
		}
		m_addr = *((u_int *) hostaddr->h_addr_list[0]);
	}
	return 0;
}

// *******************************************************************************************
// RequestHTTP -- Given a valid host name or IP number in host, and a valid resource name in name,
// uses http to fetch the resource. The name variable must at least contain the leading '/'.
// No attempt is made to parse a complete URI. This function sends the request only, it 
// does not read a reply.
// Note: the host name is sent to the previously found host (from FindHost), and so should
// match that host name. We don't save it just to save on memory.
int HttpMgr::RequestHTTP(const char* host, const char* name, int timeout)
{
	// Construct our request from the resource name.
	char resource[MAXRESLEN];
	if ((safestrlcpy(resource, "GET ", MAXRESLEN) >= MAXRESLEN) ||
		(safestrlcat(resource, name, MAXRESLEN) >= MAXRESLEN) ||
		(safestrlcat(resource, " HTTP/1.1\r\nHost: ", MAXRESLEN) >= MAXRESLEN) ||
		(safestrlcat(resource, host, MAXRESLEN) >= MAXRESLEN) ||
		(safestrlcat(resource, "\r\n\r\n", MAXRESLEN) >= MAXRESLEN))
	{
		// Our request is too long; either shorten the resource name or make MAXRESLEN bigger
		PATCH_PRINT("HTTP resource too long; shorten name or increase MAXRESLEN.\n");
		PATCH_PRINT("--Resource name: '%s'\n", name);
		return PATCH_ERROR_BAD_RESOURCE;
	}

	PATCH_PRINT("URL Request: %s\n", resource);

	// Make sure we have a valid socket
	if (m_socket < 0)
	{
		m_socket = socket(AF_INET, SOCK_STREAM, 0);
		PATCH_PRINT("HttpMgr::FindHost: socket %d\n", m_socket);
		if (m_socket < 0) {
			PATCH_PRINT("HttpMgr::FindHost: error from sceInsockSocket: %d\n", LastSocketError());
			return PATCH_ERROR_SOCKET;
		}

		// Set platform-specific options on the socket
		SocketSetup(m_socket);
	}

	sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(80);
	addr.sin_addr.s_addr = m_addr;
#ifdef PATCH_COMPILER_SN_PS2
	addr.sin_len = 16;
#endif

	if (connect(m_socket, (sockaddr *) &addr, 16) < 0)
	{
		Abort();
		return PATCH_ERROR_NOCONNECT;
	}

	////// DFS TMP HACK -- abandon ship for debugging
	////PATCH_PRINT("DFS TMP HACK aborting\n");
	////Abort();
	////return -1;

	if (send(m_socket, resource, strlen(resource), 0) != strlen(resource))
	{
		Abort();
		return PATCH_ERROR_SENDFAIL;
	}

	m_receivingBody = false;
	m_timeout = timeout;
	m_curTimeout = timeout;
	m_header[0] = 0;
	m_incount = 0;
	m_inused = 0;
	return PATCH_ERROR_NONE;
}

// *******************************************************************************************
// Performs blocking read to get the header part of the reply from a previous request.
// Parameters:
//		received	-- out: Number of data bytes actually received
//		datasize	-- out: Size of raw data calculated from server. Should be <= bufsize
//		buffer		--  in: Buffer to receive data
//		bufsize		--  in: Size of buffer. Must be big enough to receive all data
int HttpMgr::ReceiveAll(int& received, int& datasize, char* buffer, int bufsize, bool truncateOK)
{
	int status;
	
	datasize = 0;

	PATCH_PRINT("HttpMgr::ReceiveAll: Starting read\n");

	// Keep reading until we have a complete header
	bool complete;
	do {
		complete = CheckStatus(received, datasize, buffer, 0.0f, status);
	} while ((datasize == 0) && !complete);

	PATCH_PRINT("HttpMgr::ReceiveAll: Done with header loop; datasize=%d, complete=%d\n", datasize, (int)complete);
	// OK, we got the header. Allocate a buffer to hold the data
	if (status >= 0)
	{
		if (complete)
		{
			PATCH_PRINT("***Error: Completed receive of data without getting body!\n");
			status = PATCH_ERROR_PARSE_ERROR;
		}
		else if (!datasize)
		{
			PATCH_PRINT("***Error: Ready to receive data but no datasize calculated!\n");
			PATCH_PRINT("***       This version of the HTTP reader not compatible with server!\n");
			status = PATCH_ERROR_PARSE_ERROR;
		}
		else if (!truncateOK && (datasize > bufsize))
		{
			PATCH_PRINT("***Error: Bad Patch info: buffer size %d, need %d\n", bufsize, datasize);
			status = PATCH_ERROR_BUF_TOO_SMALL;
		}
		else
		{
			// If we are truncating, modify datasize to avoid an overflow
			if (truncateOK && (datasize >= bufsize))
				datasize = bufsize - 1;

			PATCH_PRINT("HttpMgr::ReceiveAll: Reading body...\n");
			while (!CheckStatus(received, datasize, buffer, 0.0f, status));
			PATCH_PRINT("HttpMgr::ReceiveAll: Done with read loop!\n");
		}
	}

	return status;
}

// *******************************************************************************************
// Parameters:
//		received	-- in/out: number of bytes received so far (add received bytes to this)
//		datasize	-- out: When header is complete, filled in with expected data size.
//					-- in:  On subsequent calls, contains that initial value for reference
//		buffer		-- in:  Ptr to output buffer, at least datasize bytes. May be null before datasize is returned.
bool HttpMgr::CheckStatus(int& received, int& datasize, char* buffer, float elapsedTime, int& status)
{
	if ((m_curTimeout -= elapsedTime) < 0.0f)
	{
		status = PATCH_ERROR_TIMEOUT;
		Abort();
		return true;
	}

	int ready;

	for (;;)
	{
		if (m_incount == m_inused)
		{
			ready = PollSocket(m_socket);

			if (ready < 0)
			{
				status = PATCH_ERROR_POLLERROR;
				Abort();
				return true;
			}

			if (ready == 0)
			{
				status = 0;
				return false;
			}

			m_incount = recv(m_socket, m_buffer, 256, 0);
			m_inused = 0;

			if (m_incount < 0)
			{
				status = PATCH_ERROR_RECVERROR;
				Abort();
				return true;
			}

			m_curTimeout = m_timeout;
		}

		while (m_inused < m_incount)
		{
			int c = m_buffer[m_inused++];
			if (m_receivingBody)
			{
				if (received < datasize)
				{
					//PATCH_PRINT("%c", c);
					buffer[received++] = c;
				}
				if (received >= datasize)
				{
					PATCH_PRINT("================================\nEnd of body\n");
					status = 0;
					Abort();
					return true;
				}
			}
			else
			{
				int j = strlen(m_header);

				if (j < 250)
				{
					m_header[j++] = c;
					m_header[j] = 0;
				}
				if (c == '\n')
				{
					m_header[--j] = 0;
					if (j && m_header[j - 1] == '\r');
					{
						m_header[--j] = 0;
					}
					PATCH_PRINT("Received Header: \"%s\"\n", m_header);
					if (j == 0)
					{
						received = 0;
						m_receivingBody = true;
						PATCH_PRINT("Switching to body\n================================\n");
					}
					else if (strncmp(m_header, "HTTP/", 5) == 0)
					{
						char *hp;
						for (hp = m_header; *hp && *hp != ' '; hp++)
						{
						}
						if (*hp == 0)
						{
							status = PATCH_ERROR_FILE_NOT_FOUND;
							Abort();
							return true;
						}
						while (*hp == ' ')
						{
							hp++;
						}
						int res;
						for (res = 0; *hp >= '0' && *hp <= '9'; res = res * 10 + *hp++ - '0')
						{
						}
						if (res != 200)
						{
							// Collapse the possible error codes down
							if (res >= 500)
								status = PATCH_ERROR_HTTP_SERVER;
							else if (res == 404)
								status = PATCH_ERROR_FILE_NOT_FOUND;
							else if (res >= 400)
								status = PATCH_ERROR_HTTP_CLIENT;
							else if (res >= 300)
								status = PATCH_ERROR_REDIRECT;			// Redirects are forbidden
							else
								status = PATCH_ERROR_UNRECOGNIZED_RESPONSE;		// Should never get any 2xx or 1xx errors...
							Abort();
							return true;
						}
						PATCH_PRINT("Main response OK\n");
					}
					else if (strncmp(m_header, "Content-Length: ", 16) == 0)
					{
						char *hp = &m_header[15];
						int len;

						while (*hp == ' ')
						{
							hp++;
						}
						for (len = 0; *hp >= '0' && *hp <= '9'; len = len * 10 + *hp++ - '0')
						{
						}
						datasize = len;
						status = 1;
						PATCH_PRINT("Content length: %d\n", datasize);
						m_header[0] = 0;
						return false;
					}
					else if (strncmp(m_header, "Transfer-Encoding: ", 19) == 0)
					{
						// Only support type "chunked" for now.

					}
					m_header[0] = 0;
				}
			}
		}
	}
	// not reached, but it shuts the compiler up.
	return false;
}

void HttpMgr::Abort()
{
	int stat;	// for debugging; these should never fail, and if they do we should ignore it
	if (m_socket >= 0)
	{
		if ((stat = shutdown(m_socket, SD_BOTH)) < 0)
			PATCH_PRINT("!!!!!!!!!!!!!!! sceInsockShutdown Failed with %d!!!!!!!!!!\n", stat);
		m_socket = -1;
	}
}
