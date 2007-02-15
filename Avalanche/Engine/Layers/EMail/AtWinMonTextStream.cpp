//////////////////////////////////////////////////////////////////////////////
// TextStream derived class using BSD sockets (or something very similar) as
// a network API
//////////////////////////////////////////////////////////////////////////////

#include "Layers/LayersPCH.h"
#include "Layers/EMail/AtWinMonTextStream.h"
#include "GameHelper/DebugOutput.h"

//////////////////////////////////// ALL ////////////////////////////////////

//static uint8 atOutputCategory;

//////////////////////////////////////////////////////////////////////////////

AtWinMonTextStream::AtWinMonTextStream()
{
//	atOutputCategory = g_debugOutput.RegisterCategory( "AtWinMon Text Stream", "SOCK" );
	mRemoteFileHandle = -1;
	mBufferByteCount = 0;
}

//////////////////////////////////////////////////////////////////////////////

AtWinMonTextStream::~AtWinMonTextStream()
{
}

//////////////////////////////////////////////////////////////////////////////

bool AtWinMonTextStream::Connect(const char* server, int port)
{
	char buffer[256];
	sprintf(buffer, "atstream:%s!%d", server, port);
	mRemoteFileHandle = sceOpen(buffer, SCE_RDWR, 0);

	if (mRemoteFileHandle >= 0)
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////////

bool AtWinMonTextStream::Close()
{
	sceClose( mRemoteFileHandle );
	return true;
}

//////////////////////////////////////////////////////////////////////////////

bool AtWinMonTextStream::Transmit(const void* data, int datalen)
{
	int bytes = sceWrite( mRemoteFileHandle, data,  datalen );
	return bytes == datalen;
}

//////////////////////////////////////////////////////////////////////////////

int AtWinMonTextStream::Receive(void* data, int maxLength)
{
	int total = 0;

	if ( mBufferByteCount > 0 )
	{
		int count = Math::Min( maxLength, mBufferByteCount );
		memcpy( data, mBuffer, count );
		maxLength -= count;
		mBufferByteCount -= count;
		total += count;
		data = &((u8*)data)[count];
	}

	if ( maxLength == 0 )
	{
		return total;
	}

	if ( maxLength < 4 )
	{
		mBufferByteCount = sceRead( mRemoteFileHandle, mBuffer, 4 );

		if ( mBufferByteCount == 0 )
			return total;

		return total + Receive( data, maxLength );
	}

	total += sceRead( mRemoteFileHandle, data, maxLength );

	return total;
}

//////////////////////////////////////////////////////////////////////////////

bool AtWinMonTextStream::ConnectToHost( int port )
{
	extern const char* g_RemoteIPAddress;
	if ( g_RemoteIPAddress == NULL )
		return false;

	return Connect( g_RemoteIPAddress, port );
}

