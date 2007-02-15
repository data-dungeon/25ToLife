//////////////////////////////////////////////////////////////////////////////
// TextStream derived class using BSD sockets (or something very similar) as
// a network API
//////////////////////////////////////////////////////////////////////////////

#include "Layers/LayersPCH.h"
#include "Layers/EMail/SocketTextStream.h"
#include "GameHelper/DebugOutput.h"

//////////////////////////////////// ALL ////////////////////////////////////

static uint8 outputCategory;

//////////////////////////////////// PS2 ////////////////////////////////////

#if defined( PS2 )

#include <Platform/Stack/PS2QNet/QNet.h>
#define socketerror (sceInsockHErrno)
#define doclosesocket(x) shutdown( x, SHUT_RDWR )
#define setconnecttimeout(s, x) do { sceInsockSetRecvTimeout( s, x*10 ); sceInsockSetSendTimeout( s, x ); } while( 0 )
#define setnonblocking(s) do { sceInsockSetRecvTimeout( s, 0 ); sceInsockSetSendTimeout( s, 0 ); } while( 0 )
#define SocketValid(s) (s >= 0)
#define INVALID_SOCKET -1
static u8 net_buf[sceLIBNET_CTRL_DATA_SIZE] __attribute__((aligned(64)));

/////////////////////////////////// WIN32 ///////////////////////////////////

#elif defined( WIN32 )

// I really want this header, not the WinSock.h thats already been included,
// but its a mutex kind of thing...  So instead I'll just define what I need.
//#include <winsock2.h>
#define SD_RECEIVE      0x00
#define SD_SEND         0x01
#define SD_BOTH         0x02

#define socketerror WSAGetLastError()
#define setconnecttimeout(s, x)
#define setnonblocking(s) do { u_long nonblocking = 1; ioctlsocket( s, FIONBIO, &nonblocking ); } while( 0 )
#define SocketValid(s) (s != INVALID_SOCKET)

static void doclosesocket( int s )
{
	g_debugOutput.Print( outputCategory, DEBUG_BORE, "Shutting down socket" );
	shutdown( s, SD_SEND );
	char buff;
	while ( recv( s, &buff, 1, 0 ) > 0 || socketerror == WSAEWOULDBLOCK );
	shutdown( s, SD_RECEIVE );
	closesocket( s );
}

/////////////////////////////////// OTHER ///////////////////////////////////

#else // platform

// these should pretty much match BSD.
#define socketerror errno
#define setconnecttimeout(s, x)
#define setnonblocking(s) do { int arg = 1; ioctl(s, FIOSNBIO, &arg); } while(0)
#define SocketValid(s) (s >= 0)
#define INVALID_SOCKET -1

static void doclosesocket( int s )
{
	g_debugOutput.Print( outputCategory, DEBUG_BORE, "Shutting down socket" );
	shutdown( s, SHUT_SEND );
	char buff;
	while ( recv( s, &buff, 1, 0 ) > 0 || socketerror == EWOULDBLOCK );
	shutdown( s, SHUT_RECEIVE );
	closesocket( s );
}

#endif // platform

//////////////////////////////////////////////////////////////////////////////

SocketTextStream::SocketTextStream()
{
	mSocket = INVALID_SOCKET;

	mRecvCount = 0;

	outputCategory = g_debugOutput.RegisterCategory( "Socket Text Stream", "SOCK" );

	#ifdef WIN32
	WSADATA wsaData;
	WSAStartup( MAKEWORD( 2, 0 ), &wsaData );
	#endif
}

//////////////////////////////////////////////////////////////////////////////

SocketTextStream::~SocketTextStream()
{
	if ( SocketValid( mSocket ) )
		Close();

	#ifdef WIN32
	WSACleanup( );
	#endif
}

//////////////////////////////////////////////////////////////////////////////

bool SocketTextStream::LookupServer(const char* server, sockaddr_in* addr)
{
	#if defined(_XBOX)
	#error gethostbyname() is not implimented on the xbox... what to do?
	#else
	
	hostent* h = gethostbyname( server );
	if ( h == NULL )
	{
		#ifdef PS2
		// try dotted notation (xxx.xxx.xxx.xxx)
		addr->sin_family = AF_INET;
		int result = sceInsockInetAton( server, &addr->sin_addr );
	
		if ( result > 0 )
			return true;
		#endif

		g_debugOutput.Print( outputCategory, DEBUG_ALERT, "Failed to lookup server %s (%d)", server, socketerror );
		return false;
	}

	addr->sin_family = h->h_addrtype;
	memcpy( (char *) &addr->sin_addr.s_addr, h->h_addr_list[0], h->h_length );

	#endif
	
	return true;
}

//////////////////////////////////////////////////////////////////////////////

bool SocketTextStream::CreateSocket()
{
	mSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	if ( !SocketValid( mSocket ) )
		g_debugOutput.Print( outputCategory, DEBUG_ALERT, "Failed to create socket (%d)", socketerror );

	return SocketValid( mSocket );
}

//////////////////////////////////////////////////////////////////////////////

bool SocketTextStream::BindToAnyPort()
{
	sockaddr_in localAddr;
	memset( &localAddr, 0, sizeof( localAddr ) );
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = htonl( INADDR_ANY );
	localAddr.sin_port = htons( 0 );

	#ifndef WIN32
	localAddr.sin_len = sizeof( localAddr );
	#endif

	int result = bind( mSocket, (struct sockaddr *) &localAddr, sizeof( localAddr ) );
	if ( result < 0 )
		g_debugOutput.Print( outputCategory, DEBUG_ALERT, "Failed to bind to port (%d)", socketerror );

	return result >= 0;
}

//////////////////////////////////////////////////////////////////////////////

bool SocketTextStream::ConnectToServer(sockaddr_in *serverAddr)
{
	setconnecttimeout( mSocket, 10000 );

	int result = connect( mSocket, (struct sockaddr *) serverAddr, sizeof( *serverAddr ) );
	if ( result < 0 )
		g_debugOutput.Print( outputCategory, DEBUG_ALERT, "Failed to connect to server (%d)", socketerror );
	else
		setnonblocking( mSocket );

	return result >= 0;
}

//////////////////////////////////////////////////////////////////////////////

bool SocketTextStream::Connect(const char* server, int port)
{
	sockaddr_in addr;
	memset( &addr, 0, sizeof( addr ) );

	#ifndef WIN32
	addr.sin_len = sizeof( addr );
	#endif

	if ( !LookupServer( server, &addr ) )
		return false;

	addr.sin_port = htons( port );

	if ( !CreateSocket() )
		return false;

	if ( !BindToAnyPort() )
		return false;

	if ( !ConnectToServer( &addr ) )
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////////

bool SocketTextStream::Close()
{
	doclosesocket( mSocket );
	mSocket = INVALID_SOCKET;
	return true;
}

//////////////////////////////////////////////////////////////////////////////

bool SocketTextStream::Transmit(const void* data, int datalen)
{
	int sentlen = send( mSocket, (const char*) data, datalen, 0 );
	if ( sentlen != datalen )
	{
		g_debugOutput.Print( outputCategory, DEBUG_NOTIFY, "tried to send %d, but sent %d", datalen, sentlen );
	}
	return sentlen == datalen;
}

//////////////////////////////////////////////////////////////////////////////

int SocketTextStream::Receive(void* data, int maxLength)
{
	return recv( mSocket, (char*) data, maxLength, 0 );
}

//////////////////////////////////////////////////////////////////////////////

bool SocketTextStream::ConnectToNameServer( void )
{
	#ifdef PS2
	sceInetAddress_t addr;
	sceSifMClientData* cd;
    
	cd = Quazal::QNet::GetInstance()->GetClientData();

	if ( cd && sceInetGetNameServers( cd, net_buf, &addr, 1 ) == 1 )
	{
		char addressbuffer[16];
		if ( sceInetAddress2String( cd, net_buf, addressbuffer, 16, &addr ) == sceLIBNETE_OK )
		{
			g_debugOutput.Print( outputCategory, DEBUG_BORE, "Found DNS server %s", addressbuffer );
			if ( Connect( addressbuffer, 53 ) )
				return true;
			g_debugOutput.Print( outputCategory, DEBUG_NOTIFY, "Failed to connect to DNS Server %s, trying default", addressbuffer );
		}
	}
	#endif

	// fall through to default implimentation...
	return TextStream::ConnectToNameServer();
}

//////////////////////////////////////////////////////////////////////////////

bool SocketTextStream::ConnectToHost( int port )
{
	#if defined( PS2 )
	// for now this is hardcoded to my system (we need a way of getting this)
	return Connect( "10.0.4.104", port );
	#endif

	// use the default
	return TextStream::ConnectToHost( port );
}

