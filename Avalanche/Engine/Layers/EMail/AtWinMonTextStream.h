#ifndef __ATWINMONTEXTSTREAM_H__
#define __ATWINMONTEXTSTREAM_H__

#include "TextStream.h"

class AtWinMonTextStream : public TextStream
{
	int mRemoteFileHandle;

	// shortcust to Connect for connecting to whatever host is designated
	// as the debugging system.
	virtual bool ConnectToHost( int port );

	char mBuffer[4];
	int mBufferByteCount;

public:
	AtWinMonTextStream();
	~AtWinMonTextStream();
	virtual bool Connect(const char* server, int port);
	virtual bool Close();
	virtual bool Transmit(const void* data, int datalen);
	virtual int Receive(void* data, int maxLength);
};

#endif // __ATWINMONTEXTSTREAM_H__

