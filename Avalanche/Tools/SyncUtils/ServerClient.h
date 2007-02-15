////////////////////////////////////////////////////////////////////////////
//
// ServerClient
//
// class to handle talking to server
//
////////////////////////////////////////////////////////////////////////////


#ifndef SYNC_UTILS_DLL_SERVER_CLIENT_H
#define SYNC_UTILS_DLL_SERVER_CLIENT_H

#define HOSTNAME_SIZE	MAX_PATH

struct SockAddrIn : public SOCKADDR_IN {
public:
	SockAddrIn() { Clear(); }
	SockAddrIn(const SockAddrIn& sin) { Copy( sin ); }
	~SockAddrIn() { }
	SockAddrIn& Copy(const SockAddrIn& sin);
	void	Clear() { memset(this, 0, sizeof(SOCKADDR_IN)); }
	bool	IsEqual(const SockAddrIn& sin);
	bool	IsGreater(const SockAddrIn& sin);
	bool	IsLower(const SockAddrIn& pm);
	bool	IsNull() const { return ((sin_addr.s_addr==0L)&&(sin_port==0)); }
	ULONG	GetIPAddr() const { return sin_addr.s_addr; }
	short	GetPort() const { return sin_port; }
	bool	CreateFrom(LPCTSTR sAddr, LPCTSTR sService, int nFamily = AF_INET);
	SockAddrIn& operator=(const SockAddrIn& sin) { return Copy( sin ); }
	bool	operator==(const SockAddrIn& sin) { return IsEqual( sin ); }
	bool	operator!=(const SockAddrIn& sin) { return !IsEqual( sin ); }
	bool	operator<(const SockAddrIn& sin)  { return IsLower( sin ); }
	bool	operator>(const SockAddrIn& sin)  { return IsGreater( sin ); }
	bool	operator<=(const SockAddrIn& sin) { return !IsGreater( sin ); }
	bool	operator>=(const SockAddrIn& sin) { return !IsLower( sin ); }
	operator LPSOCKADDR() { return (LPSOCKADDR)(this); }
	size_t	Size() const { return sizeof(SOCKADDR_IN); }
	void	SetAddr(SOCKADDR_IN* psin) { memcpy(this, psin, Size()); }
};

class ServerMessage;

class ServerClient
{
public:
	 ServerClient(void);
	 ~ServerClient();

	bool ConfirmServer(void);
	bool SetServerPath(const char *pServerPath);
	TupperwareAggregate *GetDirectoryInfo(const char *pServerPath,DWORD flags);

	static void GetServerPath(const char *pPath,TupString &serverName,TupString &serverPath);

private:

	bool WriteMessage(const ServerMessage &newMessage);
	bool ReadMessage(ServerMessage &newMessage);

	bool IsOpen(void) const;
	SOCKET GetSocket(void) const;
	bool ConnectTo(const char *pDestination, const char *pServiceName, int nFamily, int nType);
	void CloseComm(void);
	void StopComm(void);

	static bool FindServer(const char *pServerName,const char *pServerPath);

	static ULONG GetIPAddress( const char *pHostName );
	static bool GetLocalName(char *pName, UINT nSize);
	static USHORT GetPortNumber( const char *pServiceName );
	static bool GetLocalAddress(char *pAddress, UINT nSize);

	bool ShutdownConnection(SOCKET sock);
	bool GetSockName(SockAddrIn& saddr_in);
	bool GetPeerName(SockAddrIn& saddr_in);

	DWORD ReadComm(void *pBuffer, DWORD dwSize, DWORD dwTimeout);
	DWORD WriteComm(const void *pBuffer, DWORD dwCount, DWORD dwTimeout);

	bool ReadEntireBuffer(void *pData,DWORD length,DWORD dwTimeout);
	bool WriteEntireBuffer(const void *pData,DWORD length,DWORD dwTimeout);

	static TupperwareAggregate *TupAggregateFromBuffer(const void *pBuffer,DWORD bufferLength);
	static bool BufferFromTupAggregate(void **pBuffer,DWORD &bufferLength,TupperwareAggregate *pRoot);

	HANDLE m_hComm;		// Serial Comm handle

	friend struct SockAddrIn;
};

//-----------------------------------------------------------------------------

#endif

