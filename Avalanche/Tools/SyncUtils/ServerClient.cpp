////////////////////////////////////////////////////////////////////////////
//
// ServerClient
//
// class to handle talking to server
//
////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "ServerClient.h"
#include "ServerMessage.h"

const DWORD DEFAULT_TIMEOUT = 100L;
#define DEFAULT_PORT "2345"

///////////////////////////////////////////////////////////////////////////////
// SockAddrIn Struct

///////////////////////////////////////////////////////////////////////////////
// Copy
SockAddrIn& SockAddrIn::Copy(const SockAddrIn& sin)
{
	memcpy(this, &sin, Size());
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// IsEqual
bool SockAddrIn::IsEqual(const SockAddrIn& sin)
{
	// Is it Equal? - ignore 'sin_zero'
	return (memcmp(this, &sin, Size()-sizeof(sin_zero)) == 0);
}

///////////////////////////////////////////////////////////////////////////////
// IsGreater
bool SockAddrIn::IsGreater(const SockAddrIn& sin)
{
	// Is it Greater? - ignore 'sin_zero'
	return (memcmp(this, &sin, Size()-sizeof(sin_zero)) > 0);
}

///////////////////////////////////////////////////////////////////////////////
// IsLower
bool SockAddrIn::IsLower(const SockAddrIn& sin)
{
	// Is it Lower? - ignore 'sin_zero'
	return (memcmp(this, &sin, Size()-sizeof(sin_zero)) < 0);
}

///////////////////////////////////////////////////////////////////////////////
// CreateFrom
bool SockAddrIn::CreateFrom(LPCTSTR sAddr, LPCTSTR sService, int nFamily /*=AF_INET*/)
{
	Clear();
	sin_addr.s_addr = htonl( ServerClient::GetIPAddress(sAddr) );
	sin_port = htons( ServerClient::GetPortNumber( sService ) );
	sin_family = nFamily;
	return !IsNull();
}

ServerClient::ServerClient(void) : m_hComm(INVALID_HANDLE_VALUE)
{
}

ServerClient::~ServerClient()
{
	StopComm();
}

bool ServerClient::IsOpen(void) const
{
	return ( INVALID_HANDLE_VALUE != m_hComm );
}

SOCKET ServerClient::GetSocket(void) const
{
	return (SOCKET) m_hComm;
}
void ServerClient::GetServerPath(const char *pPath,TupString &serverName,TupString &serverPath)
{
	serverPath = pPath;

	if (serverPath[1] == ':')
	{
		char buffer[1000];
		TupString testPath = serverPath.Left(2);
		int numChars = QueryDosDevice(testPath,buffer,1000);
		if (numChars)
		{
			TupString prefix = buffer;
			if (prefix.Find("\\Device\\LanmanRedirector\\")==0)
			{
				int startPos = prefix.Find("\\",26);
				TupString newPath = "\\\\";
				newPath += prefix.Mid(startPos+1);
				newPath += serverPath.Mid(2);
				serverPath = newPath;
			}
			else if (prefix.Find("\\??\\UNC\\")==0)
			{
				TupString newPath = "\\\\";
				newPath += prefix.Mid(8);
				newPath += serverPath.Mid(2);
				serverPath = newPath;
			}
			else if (prefix.Find("\\??\\")==0)
			{
				TupString newPath = prefix.Mid(4);
				newPath += serverPath.Mid(2);
				serverPath = newPath;
			}
		}
	}

	// if unc name (get server name from string)
	if (serverPath.GetLength()>=2 && serverPath[0] == '\\' && serverPath[1] == '\\')
	{
		int foundEndOfServerIndex = serverPath.Find("\\",2);
		// no backslash at end (just use the characters right of "\\"
		if (foundEndOfServerIndex==-1)
		{
			serverName = serverPath.Mid(2);
			serverPath = "";
		}
		// get computer name from string
		else
		{
			serverName = serverPath.Mid(2,foundEndOfServerIndex-2);
			serverPath = serverPath.Mid(foundEndOfServerIndex);
		}
	}
	// use the name of my machine as the server name
	else
	{
		char nameBuffer[512];
		DWORD maxSize = 512;
		GetComputerName(nameBuffer,&maxSize);
		serverName = nameBuffer;
	}
}

bool ServerClient::FindServer(const char *pServerName,const char *pServerPath)
{
	ServerClient client;
	ULONG serverAddress = GetIPAddress(pServerName);
	if (client.ConnectTo(pServerName,DEFAULT_PORT,AF_INET,SOCK_STREAM))
	{
		if (client.ConfirmServer())
		{
			if (client.SetServerPath(pServerPath))
			{
				return true;
			}
		}
	}
	return false;	
}


///////////////////////////////////////////////////////////////////////////////
// GetPortNumber
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
//				Returns a port number based on service name or port number string
// PARAMETERS:
//	LPCTSTR strServiceName: Service name or port string
///////////////////////////////////////////////////////////////////////////////
USHORT ServerClient::GetPortNumber( const char *pServiceName )
{
	LPSERVENT	lpservent;
	USHORT		nPortNumber = 0;

	if ( _istdigit( pServiceName[0] ) ) {
		nPortNumber = (USHORT) _ttoi( pServiceName );
	}
	else {
		LPCTSTR pstrDevice = pServiceName;
		// Convert network byte order to host byte order
		if ( (lpservent = getservbyname( pstrDevice, NULL )) != NULL )
			nPortNumber = ntohs( lpservent->s_port );
	}

	return nPortNumber;
}

ULONG ServerClient::GetIPAddress( const char *pHostName )
{
	LPHOSTENT lphostent;
	ULONG	uAddr = INADDR_NONE;
	TCHAR strLocal[HOSTNAME_SIZE] = { 0 };

	// if no name specified, get local
	if ( NULL == pHostName )
	{
		GetLocalName(strLocal, sizeof(strLocal));
		pHostName = strLocal;
	}

	LPCTSTR strHost = pHostName;

	// Check for an Internet Protocol dotted address string
	uAddr = inet_addr( strHost );

	if ( (INADDR_NONE == uAddr) && (strcmp( strHost, "255.255.255.255" )) )
	{
		// It's not an address, then try to resolve it as a hostname
		if ( lphostent = gethostbyname( strHost ) )
			uAddr = *((ULONG *) lphostent->h_addr_list[0]);
	}

	return ntohl( uAddr );
}


bool ServerClient::GetLocalName(char *pName, UINT nSize)
{
	if (pName != NULL && nSize > 0)
	{
		char strHost[HOSTNAME_SIZE] = { 0 };

		// get host name, if fail, SetLastError is set
		if (SOCKET_ERROR != gethostname(strHost, sizeof(strHost)))
		{
			struct hostent* hp;
			hp = gethostbyname(strHost);
			if (hp != NULL)	{
				strcpy(strHost, hp->h_name);
			}

			// check if user provide enough buffer
			if (strlen(strHost) > nSize)
			{
				SetLastError(ERROR_INSUFFICIENT_BUFFER);
				return false;
			}
			strcpy(pName, strHost);
			return true;
		}
	}
	else
		SetLastError(ERROR_INVALID_PARAMETER);
	return false;
}

///////////////////////////////////////////////////////////////////////////////
// ConnectTo
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
//				Establish connection with a server service or port
// PARAMETERS:
//	LPCTSTR strDestination: hostname or address to connect (in .dot format)
//	LPCTSTR strServiceName: Service name or port number
//	int nFamily: address family to use (set to AF_INET)
//	int nType: type of socket to create (SOCK_STREAM, SOCK_DGRAM)
///////////////////////////////////////////////////////////////////////////////
bool ServerClient::ConnectTo(const char *pDestination, const char *pServiceName, int nFamily, int nType)
{
	// Socket is already opened
	if ( IsOpen() )
		return false;

	// Create a Socket that is bound to a specific service provide
	// nFamily: (AF_INET)
	// nType: (SOCK_STREAM, SOCK_DGRAM)
	SOCKET sock = socket(nFamily, nType, 0);
	if (INVALID_SOCKET != sock)
	{
		// Associate a local address with the socket
		SockAddrIn sockAddr;
		if (false == sockAddr.CreateFrom(NULL, TEXT("0"), nFamily))
		{
			closesocket( sock );
			return false;
		}

		if ( SOCKET_ERROR == bind(sock, (LPSOCKADDR)sockAddr, sockAddr.Size() ))
		{
			closesocket( sock );
			return false;
		}

		// Now get destination address & port
		sockAddr.CreateFrom( pDestination, pServiceName );

		// try to connect - if fail, server not ready
		if (SOCKET_ERROR == connect( sock, (LPSOCKADDR)sockAddr, sockAddr.Size()))
		{
			closesocket( sock );
			return false;
		}

		// Success, now we may save this socket
		m_hComm = (HANDLE) sock;
	}
	return (INVALID_SOCKET != sock);
}

bool ServerClient::GetLocalAddress(char *pAddress, UINT nSize)
{
	// Get computer local address
	if (pAddress != NULL && nSize > 0)
	{
		char strHost[HOSTNAME_SIZE] = { 0 };

		// get host name, if fail, SetLastError is called
		if (SOCKET_ERROR != gethostname(strHost, sizeof(strHost)))
		{
			struct hostent* hp;
			hp = gethostbyname(strHost);
			if (hp != NULL && hp->h_addr_list[0] != NULL)
			{
				// Address is four bytes (32-bit)
				if ( hp->h_length < 4)
					return false;

				// Convert address to . format
				strHost[0] = 0;

				// Create Address string
				sprintf(strHost, "%u.%u.%u.%u",
					(UINT)(((PBYTE) hp->h_addr_list[0])[0]),
					(UINT)(((PBYTE) hp->h_addr_list[0])[1]),
					(UINT)(((PBYTE) hp->h_addr_list[0])[2]),
					(UINT)(((PBYTE) hp->h_addr_list[0])[3]));

				// check if user provide enough buffer
				if (strlen(strHost) > nSize)
				{
					SetLastError(ERROR_INSUFFICIENT_BUFFER);
					return false;
				}

				// Unicode conversion
				_tcscpy(pAddress, strHost);
				return true;
			}
		}
	}
	else
		SetLastError(ERROR_INVALID_PARAMETER);
	return false;
}

bool ServerClient::ShutdownConnection(SOCKET sock)
{
	shutdown(sock, SD_BOTH);
	return ( 0 == closesocket( sock ));
}


///////////////////////////////////////////////////////////////////////////////
// GetSockName
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
//				retrieves the local name for a socket
// PARAMETERS:
//	SockAddrIn& saddr_in: object to store address
///////////////////////////////////////////////////////////////////////////////
bool ServerClient::GetSockName(SockAddrIn& saddr_in)
{
	if (IsOpen())
	{
		int namelen = saddr_in.Size();
		return (SOCKET_ERROR != getsockname(GetSocket(), (LPSOCKADDR)saddr_in, &namelen));
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// GetPeerName
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
//				retrieves the name of the peer to which a socket is connected
// PARAMETERS:
//	SockAddrIn& saddr_in: object to store address
///////////////////////////////////////////////////////////////////////////////
bool ServerClient::GetPeerName(SockAddrIn& saddr_in)
{
	if (IsOpen())
	{
		int namelen = saddr_in.Size();
		return (SOCKET_ERROR != getpeername(GetSocket(), (LPSOCKADDR)saddr_in, &namelen));	
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
// CloseComm
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
//		Close Socket Communication
// PARAMETERS:
//		None
///////////////////////////////////////////////////////////////////////////////
void ServerClient::CloseComm(void)
{
	if (IsOpen())
	{
		ShutdownConnection((SOCKET)m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
	}
}


///////////////////////////////////////////////////////////////////////////////
// StopComm
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
//		Close Socket and Stop Communication thread
// PARAMETERS:
//		None
///////////////////////////////////////////////////////////////////////////////
void ServerClient::StopComm(void)
{
	// Close Socket
	if (IsOpen())
	{
		CloseComm();
		Sleep(50);
	}
}


///////////////////////////////////////////////////////////////////////////////
// ReadComm
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
//		Reads the Socket Communication
// PARAMETERS:
//		void *lpBuffer: buffer to place new data
//		DWORD dwSize: maximum size of buffer
//		DWORD dwTimeout: timeout to use in millisecond
///////////////////////////////////////////////////////////////////////////////
DWORD ServerClient::ReadComm(void *pBuffer, DWORD dwSize, DWORD dwTimeout)
{
	_ASSERTE( IsOpen() );
	_ASSERTE( pBuffer != NULL );

	if (pBuffer == NULL || dwSize < 1L)
		return 0L;

	fd_set	fdRead  = { 0 };
	TIMEVAL	stTime;
	TIMEVAL	*pstTime = NULL;

	if ( INFINITE != dwTimeout ) 
	{
		stTime.tv_sec = dwTimeout/1000;
		stTime.tv_usec = dwTimeout % 1000;
		pstTime = &stTime;
	}

	SOCKET s = (SOCKET) m_hComm;
	// Set Descriptor
	if ( !FD_ISSET( s, &fdRead ) )
		FD_SET( s, &fdRead );

	// Select function set read timeout
	DWORD dwBytesRead = 0L;
	int res = select( s+1, &fdRead, NULL, NULL, pstTime );
	if ( res > 0)
	{
		res = recv( s, (LPSTR)pBuffer, dwSize, 0);
		dwBytesRead = (DWORD)((res > 0)?(res) : (-1L));
	}

	return dwBytesRead;
}


///////////////////////////////////////////////////////////////////////////////
// WriteComm
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
//		Writes data to the Socket Communication
// PARAMETERS:
//		const BYTE *lpBuffer: data to write
//		DWORD dwCount: maximum characters to write
//		DWORD dwTimeout: timeout to use in millisecond
///////////////////////////////////////////////////////////////////////////////
DWORD ServerClient::WriteComm(const void *pBuffer, DWORD dwCount, DWORD dwTimeout)
{
	_ASSERTE( IsOpen() );
	_ASSERTE( NULL != pBuffer );

	// Accept 0 bytes message
	if (!IsOpen() || NULL == pBuffer)
		return 0L;

	fd_set	fdWrite  = { 0 };
	TIMEVAL	stTime;
	TIMEVAL	*pstTime = NULL;

	if ( INFINITE != dwTimeout ) 
	{
		stTime.tv_sec = dwTimeout/1000;
		stTime.tv_usec = dwTimeout % 1000;
		pstTime = &stTime;
	}

	SOCKET s = (SOCKET) m_hComm;
	// Set Descriptor
	if ( !FD_ISSET( s, &fdWrite ) )
		FD_SET( s, &fdWrite );

	// Select function set write timeout
	DWORD dwBytesWritten = 0L;
	int res = select( s+1, NULL, &fdWrite, NULL, pstTime );
	if ( res > 0)
	{
		res = send( s, (LPCSTR)pBuffer, dwCount, 0);
		dwBytesWritten = (DWORD)((res >= 0)?(res) : (-1));
	}

	return dwBytesWritten;
}

// make sure the correct server it there
bool ServerClient::ConfirmServer(void)
{
	ServerMessage newMessage(ServerMessage::CONFIRM_SERVER);
	if (WriteMessage(newMessage))
	{
		ServerMessage confirmMessage;
		if (ReadMessage(confirmMessage))
		{
			if (confirmMessage.GetCommand()==ServerMessage::CONFIRM_SERVER_ACK && confirmMessage.GetArg(0))
			{
				return true;
			}
			else
			{
				SetLastError(ERROR_BAD_COMMAND);
			}
		}
	}
	return false;
}

// ask the server if this path is supported and sets the path for the connection
bool ServerClient::SetServerPath(const char *pServerPath)
{
	ASSERT(pServerPath);
	ServerMessage newMessage(ServerMessage::SET_SERVER_PATH);
	newMessage.AddBuffer(pServerPath,strlen(pServerPath)+1);

	if (WriteMessage(newMessage))
	{
		ServerMessage confirmMessage;
		if (ReadMessage(confirmMessage))
		{
			if (confirmMessage.GetCommand()==ServerMessage::SET_SERVER_PATH_ACK)
			{
				if (confirmMessage.GetArg(0)==0)
				{
					SetLastError(ERROR_PATH_NOT_FOUND);
					return false;
				}
				else
				{
					return true;
				}
			}
		}
	}
	return false;

}

TupperwareAggregate *ServerClient::GetDirectoryInfo(const char *pServerPath,DWORD flags)
{
	ASSERT(pServerPath);
	ServerMessage newMessage(ServerMessage::GET_DIRECTORY_INFO);
	newMessage.AddBuffer(pServerPath,strlen(pServerPath)+1);
	if (WriteMessage(newMessage))
	{
		ServerMessage confirmMessage;
		if (ReadMessage(confirmMessage))
		{
			if (confirmMessage.GetCommand()==ServerMessage::GET_DIRECTORY_INFO_ACK)
			{
				if (confirmMessage.GetArg(0)==0)
				{
					SetLastError(ERROR_PATH_NOT_FOUND);
					return NULL;
				}
				else
				{
					if (confirmMessage.GetNumBuffers()==1)
					{
						return TupAggregateFromBuffer(confirmMessage.GetBufferData(0),confirmMessage.GetBufferLength(0));
					}
				}
			}
		}
	}
	return NULL;
}

bool ServerClient::ReadEntireBuffer(void *pData,DWORD length,DWORD dwTimeout)
{
	if (pData && length)
	{
		BYTE *pDataRemaining = (BYTE *)pData;
		DWORD sizeRemaining = length;
		while (sizeRemaining)
		{
			DWORD bytesRead = ReadComm(pDataRemaining,sizeRemaining,dwTimeout);
			if (bytesRead==-1L || bytesRead==0)
			{
				return false;
			}
			else
			{
				sizeRemaining-=bytesRead;
				pDataRemaining+=bytesRead;
			}
		}
		return true;
	}
	return false;
}

bool ServerClient::WriteEntireBuffer(const void *pData,DWORD length,DWORD dwTimeout)
{
	if (pData && length)
	{
		const BYTE *pDataRemaining = (const BYTE *)pData;
		DWORD sizeRemaining = length;
		while (sizeRemaining)
		{
			DWORD bytesWritten = WriteComm(pDataRemaining,sizeRemaining,dwTimeout);
			if (bytesWritten==-1L || bytesWritten==0)
			{
				return false;
			}
			else
			{
				sizeRemaining-=bytesWritten;
				pDataRemaining+=bytesWritten;
			}
		}
		return true;
	}
	return false;
}

struct MessageHeader
{
	DWORD m_command;
	int m_args[ServerMessage::NUM_ARGS];
	DWORD m_numBuffers;
};

//
// The message format is as follows:
// 
// command - 4 bytes
// args - 16 bytes
// number of buffers - 4 bytes
// length of buffer 1 - 4 bytes
// length of buffer 2 - 4 bytes
// ...
// length of buffer n - 4 bytes
// flags for buffer 1 - 4 bytes
// flags for buffer 2 - 4 bytes
// ...
// flags for buffer n - 4 bytes
// buffer data for buffer 1 - length of buffer 1
// buffer data for buffer 2 - length of buffer 2
// ...
// buffer data for buffer n - length of buffer n
//

bool ServerClient::WriteMessage(const ServerMessage &message)
{
	MessageHeader header;
	header.m_command = message.GetCommand();
	header.m_numBuffers = message.GetNumBuffers();
	for (int argIndex=0;argIndex<ServerMessage::NUM_ARGS;argIndex++)
	{
		header.m_args[argIndex] = message.GetArg(argIndex);
	}
	// write the header
	if (WriteEntireBuffer((const BYTE *) &header,sizeof(header),DEFAULT_TIMEOUT))
	{
		if (header.m_numBuffers)
		{
			DWORD index;
			TupArray<DWORD> bufferLengths;
			TupArray<DWORD> bufferFlags;

			bufferLengths.SetSize(header.m_numBuffers);
			bufferFlags.SetSize(header.m_numBuffers);

			for (index=0;index<header.m_numBuffers;index++)
			{
				bufferLengths[index] = message.GetBufferLength(index);
				bufferFlags[index] = message.GetBufferFlags(index);
			}
			// write the length of all the buffers
			if (WriteEntireBuffer((const BYTE *)bufferLengths.GetData(),sizeof(DWORD)*header.m_numBuffers,DEFAULT_TIMEOUT))
			{
				if (WriteEntireBuffer((const BYTE *)bufferFlags.GetData(),sizeof(DWORD)*header.m_numBuffers,DEFAULT_TIMEOUT))
				{
					for (index=0;index<header.m_numBuffers;index++)
					{
						if (!WriteEntireBuffer(message.GetBufferData(index),message.GetBufferLength(index),DEFAULT_TIMEOUT))
						{
							return false;
						}
					}
				}
			}
		}
		return true;
	}
	return false;
}

bool ServerClient::ReadMessage(ServerMessage &newMessage)
{
	newMessage.Init();
	MessageHeader header;
	// read in the header
	if (ReadEntireBuffer((BYTE *)&header,sizeof(header),DEFAULT_TIMEOUT))
	{
		newMessage.SetCommand((ServerMessage::Commands)header.m_command);
		for (int argIndex=0;argIndex<ServerMessage::NUM_ARGS;argIndex++)
		{
			newMessage.SetArg(argIndex,header.m_args[argIndex]);
		}
		if (header.m_numBuffers)
		{
			TupArray<DWORD> bufferLengths;
			TupArray<DWORD> bufferFlags;

			bufferLengths.SetSize(header.m_numBuffers);
			bufferFlags.SetSize(header.m_numBuffers);

			// read in the lengths of all the buffers
			if (ReadEntireBuffer(bufferLengths.GetData(),sizeof(DWORD)*header.m_numBuffers,DEFAULT_TIMEOUT))
			{
				// read the flags of all the buffers
				if (ReadEntireBuffer(bufferFlags.GetData(),sizeof(DWORD)*header.m_numBuffers,DEFAULT_TIMEOUT))
				{
					for (DWORD index=0;index<header.m_numBuffers;index++)
					{
						newMessage.AddBuffer(bufferLengths[index],bufferFlags[index]);
						void *pBuffer = newMessage.GetBufferData(index);
						// read in the buffers
						if (!ReadEntireBuffer(pBuffer,bufferLengths[index],DEFAULT_TIMEOUT))
						{
							newMessage.Init();
							return false;
						}
					}
				}
			}
		}
		return true;
	}
	return false;
}

TupperwareAggregate *ServerClient::TupAggregateFromBuffer(const void *pBuffer,DWORD bufferLength)
{
	TupperwareAggregate *pRoot = new TupperwareAggregate;
	int result = TupperwareLoadSaveManager::Read((const char *)pBuffer,bufferLength,&pRoot);
	if (result)
	{
		return pRoot;
	}
	else
	{
		delete pRoot;
		return NULL;
	}

}

bool ServerClient::BufferFromTupAggregate(void **pBuffer,DWORD &bufferLength,TupperwareAggregate *pRoot)
{
	unsigned int bufferLengthTemp;
	int result = TupperwareLoadSaveManager::Write(TupperwareLoadSaveManager::BINARY,(unsigned char **)pBuffer,bufferLengthTemp,pRoot);
	bufferLength = (DWORD)bufferLengthTemp;
	return result ? true : false;
}

