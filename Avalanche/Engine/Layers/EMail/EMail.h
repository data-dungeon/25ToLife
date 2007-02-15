#ifndef __EMAIL_H__
#define __EMAIL_H__


#include "TextStream.h"

// recipient storage
// these are self registering objects.  Just create them globally, and
// they add themselves to the email system.  It just references the 2
// string pointers passed to the constructor, so its up to you to make
// sure they are valid when the recipient gets used.  I suggest just
// using constant strings.
class EMailRecipient
{
	const char* mAddress;
	const char* mDisplayName;
	EMailRecipient* mNextRecipient;

public:

	const EMailRecipient* GetNextRecipient( ) const;

	EMailRecipient( const char* DisplayName, const char* EMailAddress );
	~EMailRecipient( );

	const char* GetEMailAddress( ) const;
	const char* GetDisplayName( ) const;
	const char* GetDomain( ) const;
};


class EMail
{
	TextStream* mStream;
	uint8 mOutputCategory;

	bool ExpectCode(int code);
	bool Connect( const char* server );
	void Close();

public:

	EMail();
	~EMail();

	bool Begin( const char* from, const char* to, const char* subject );
	void AppendLine( const char* bodyLine );
	bool End();

	void Attachment( const char* filename, const char* contenttype, bool inlineAttachment, bool base64encode );
	void AppendRaw( const void* data, int len );

	static int GetRecipientCount();
	static const EMailRecipient* GetRecipient(int index);
};
#endif // __EMAIL_H__

