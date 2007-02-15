#ifndef __SOCKETTEXTSTREAM_H__
#define __SOCKETTEXTSTREAM_H__

#ifdef PS2
#include <libinsck.h>
#endif

#include "TextStream.h"

class SocketTextStream : public TextStream
{
	int mSocket;

	bool LookupServer(const char* server, sockaddr_in* addr);
	bool BindToAnyPort();
	bool CreateSocket();
	bool ConnectToServer(sockaddr_in *serverAddr);

	// this is a shortcut to Connect while looking up a valid nameserver
	// its used by the email system to query MX records for sending email.
	// TextStream provides an OK default... so you dont technically need
	// to write this if your creating a new TextStream type.  But in this
	// case, for the ps2, we have a better solution than a hardcoded server.
	virtual bool ConnectToNameServer( void );

	// shortcust to Connect for connecting to whatever host is designated
	// as the debugging system.
	virtual bool ConnectToHost( int port );

public:
	SocketTextStream();
	~SocketTextStream();
	virtual bool Connect(const char* server, int port);
	virtual bool Close();
	virtual bool Transmit(const void* data, int datalen);
	virtual int Receive(void* data, int maxLength);
};

#endif // __SOCKETTEXTSTREAM_H__

