#ifndef __TEXTSTREAM_H__
#define __TEXTSTREAM_H__

class TextStream
{
protected:
	TextStream();

	// base64 encoding gunk.
	bool base64EncodeEnabled;
	char encodeBuffer[3];
	int encodeBufferBytes;
	void EncodeData( char* outputBuffer );
	void FillEncodeBuffer(const void* &data, int &datalen);

	char mRecvBuffer[256];
	int mRecvCount;

	bool mServiceNetwork;

	virtual bool Transmit( const void* data, int datalen ) = 0L;
	virtual int Receive( void* data, int maxLength ) = 0L;

	// Ick
	void Sleep();

public:

	static TextStream* Create();

	virtual ~TextStream(){}
	virtual bool Connect( const char* server, int port ) = 0L;
	virtual bool Close( void ) = 0L;

	// this is a shortcut to Connect while looking up a valid nameserver
	// its used by the email system to query MX records for sending email.
	virtual bool ConnectToNameServer( void );

	// this is a shortcut to Connect.  Used for connecting to the current
	// host debugging system.
	virtual bool ConnectToHost( int port );

	int RecvLine( char* line, int maxLength );
	bool RecvBlock( void* data, int blockLength, bool waitforit = true );
	bool Send( const void* data, int datalen );
	bool Send( const char* line ) { return Send( line, strlen( line ) ); }

	template <class T>
		bool Recv( T& var, bool waitforit = true ) { return RecvBlock( &var, sizeof( var ), waitforit ); }
	template <class T>
		bool Send( const T& var ) { return Send( &var, sizeof( var ) ); }

	void Base64Encode(bool encodeEnable);

	void ServiceNetwork(bool doservice);
};

#endif // __TEXTSTREAM_H__

