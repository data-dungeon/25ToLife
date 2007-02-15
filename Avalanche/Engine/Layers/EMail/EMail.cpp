#include "Layers/LayersPCH.h"
#include "EMail.h"
#include "TextStream.h"
#include "MXLookup.h"
#include "GameHelper/DebugOutput.h"

static const char* boundaryString = "=======---PS2 MAIL BOUNDARY---=======";
static EMailRecipient* sFirstRecipient = NULL;
static int sRecipientCount = 0;

//////////////////////////////////////////////////////////////////////////////

EMail::EMail()
{
	mStream = NULL;

	mOutputCategory = INVALID_CATEGORY;
}

//////////////////////////////////////////////////////////////////////////////

EMail::~EMail()
{
	if ( mStream )
		delete mStream;
}

//////////////////////////////////////////////////////////////////////////////

static bool isnum(char c)
{
	return c >= '0' && c <= '9';
}

bool EMail::ExpectCode(int code)
{
	if ( mStream == NULL )
		return false;

	char linebuffer[256];

	if ( mStream->RecvLine( linebuffer, 256 ) <= 0 )
	{
		g_debugOutput.Print( mOutputCategory, DEBUG_ALERT, "Failed to recieve line containing result code");
		return false;
	}

	char* start = &linebuffer[0];
	while ( !isnum( *start ) ) start++;
	char* ptr = start;
	while ( isnum( *ptr ) ) ptr++;
	*ptr = '\0';

	int gotcode = atoi( start );

	if ( code == gotcode )
		g_debugOutput.Print( mOutputCategory, DEBUG_BORE, "Recieved result %d", code);
	else
		g_debugOutput.Print( mOutputCategory, DEBUG_ALERT, "Expected result %d, recieved result %d", code, gotcode);

	return code == gotcode;
}

//////////////////////////////////////////////////////////////////////////////

bool EMail::Connect(const char* server)
{
	mStream = TextStream::Create();

	if ( mStream == NULL )
		return false;

	mOutputCategory = g_debugOutput.RegisterCategory( "EMail System", "EMAIL" );

	g_debugOutput.Print( mOutputCategory, DEBUG_BORE, "Connecting to MX %s", server );
	if ( !mStream->Connect( server, 25 ) )
	{
		g_debugOutput.Print( mOutputCategory, DEBUG_ALERT, "Failed to connect to server" );
		return false;
	}

	if ( !ExpectCode( 220 ) )
	{
		Close();
		return false;
	}

	g_debugOutput.Print( mOutputCategory, DEBUG_BORE, "Saying Hello" );
	mStream->Send( "HELO PS2.avalanchesoftware.com\r\n" );

	if ( !ExpectCode( 250 ) )
	{
		g_debugOutput.Print( mOutputCategory, DEBUG_BORE, "Server didnt reply 250" );
		Close();
		return false;
	}
	g_debugOutput.Print( mOutputCategory, DEBUG_BORE, "Server is happy with us" );

	return true;
}

//////////////////////////////////////////////////////////////////////////////

bool EMail::Begin(const char* from, const char* to, const char* subject)
{
	char mx[256];

	g_debugOutput.Print( mOutputCategory, DEBUG_BORE, "Let see what the MX is for %s", to );
	if ( !MXLookup::ResearchEMailAddress( to, mx ) )
	{
		g_debugOutput.Print( mOutputCategory, DEBUG_ALERT, "Failed to find MX for %s", to );
		return false;
	}

	if ( !Connect( mx ) )
	{
		g_debugOutput.Print( mOutputCategory, DEBUG_ALERT, "Failed to connect to MX" );
		return false;
	}

	if ( mStream == NULL )
	{
		g_debugOutput.Print( mOutputCategory, DEBUG_ALERT, "Success in connecting to MX, but mStream is NULL... What??" );
		return false;
	}

	g_debugOutput.Print( mOutputCategory, DEBUG_BORE, "sending MAIL FROM packet" );
	mStream->Send( "MAIL FROM:<" );
	mStream->Send( from );
	mStream->Send( ">\r\n" );
	if ( !ExpectCode( 250 ) )
	{
		Close();
		g_debugOutput.Print( mOutputCategory, DEBUG_ALERT, "Server didnt like email sent from %s", from );
		return false;
	}
	g_debugOutput.Print( mOutputCategory, DEBUG_BORE, "sending RCPT TO packet", from );
	mStream->Send( "RCPT TO:<" );
	mStream->Send( to );
	mStream->Send( ">\r\n" );
	if ( !ExpectCode( 250 ) )
	{
		Close();
		g_debugOutput.Print( mOutputCategory, DEBUG_ALERT, "Server didnt like recipient address %s", to );
		return false;
	}
	g_debugOutput.Print( mOutputCategory, DEBUG_BORE, "Starting data stream");
	mStream->Send( "DATA\r\n" );
	if ( !ExpectCode( 354 ) )
	{
		Close();
		g_debugOutput.Print( mOutputCategory, DEBUG_BORE, "Server didn't want us to start a data stream");
		return false;
	}
	g_debugOutput.Print( mOutputCategory, DEBUG_BORE, "Writing email header");
	mStream->Send( "From: " );
	mStream->Send( from );
	mStream->Send( "\r\nTo: " );
	mStream->Send( to );
	mStream->Send( "\r\nMIME-Version: 1.0\r\nContent-Type: multipart/mixed; boundary=\"" );
	mStream->Send( boundaryString );
	mStream->Send( "\"\r\nSubject: ");
	mStream->Send( subject );
	mStream->Send( "\r\n\r\nThis is a multi-part message in MIME format.\r\n\r\n--");
	mStream->Send( boundaryString );
	mStream->Send( "\r\nContent-Type: text/plain\r\n\r\n" );

	return true;
}

//////////////////////////////////////////////////////////////////////////////

void EMail::AppendLine( const char* bodyLine )
{
	if ( mStream == NULL )
		return;

	mStream->Send( bodyLine );
	mStream->Send( "\r\n" );
}

//////////////////////////////////////////////////////////////////////////////

void EMail::Attachment(const char* filename, const char* contenttype, bool inlineAttachment, bool base64encode)
{
	mStream->Base64Encode(false);
	mStream->Send( "\r\n--" );
   mStream->Send( boundaryString );
	mStream->Send( "\r\nContent-Type: " );
	mStream->Send( contenttype );
	mStream->Send( " name=\"" );
	mStream->Send( filename );
	mStream->Send( "\"\r\nContent-Transfer-Encoding: " );
	if ( base64encode )
		mStream->Send( "base64" );
	else
		mStream->Send( "7bit" );
	mStream->Send( "\r\nContent-Disposition: " );
	if ( inlineAttachment )
		mStream->Send( "inline" );
	else
		mStream->Send( "attachment" );
	mStream->Send( "; filename=\"");
	mStream->Send( filename );
	mStream->Send( "\"\r\n\r\n" );

	if ( base64encode )
		mStream->Base64Encode(true);
}

//////////////////////////////////////////////////////////////////////////////

void EMail::AppendRaw( const void* data, int len )
{
	if ( mStream == NULL )
		return;

	mStream->Send( data, len );
}

//////////////////////////////////////////////////////////////////////////////

bool EMail::End()
{
	if ( mStream == NULL )
		return false;

	mStream->Base64Encode(false);
	mStream->Send( "\r\n--" );
   mStream->Send( boundaryString );
	mStream->Send( "--\r\n\r\n.\r\n" );
	g_debugOutput.Print( mOutputCategory, DEBUG_BORE, "Ending data stream");
	if ( !ExpectCode( 250 ) )
	{
		Close();
		g_debugOutput.Print( mOutputCategory, DEBUG_ALERT, "Server didn't accept the email.  Perhaps it doesnt like your spam?");
		return false;
	}
	Close();
	return true;
}

//////////////////////////////////////////////////////////////////////////////

void EMail::Close()
{
	if ( mStream == NULL )
		return;

	g_debugOutput.Print( mOutputCategory, DEBUG_BORE, "Sending QUIT message");
	mStream->Send( "QUIT\r\n" );
	if ( !ExpectCode( 221 ) )
	{
		g_debugOutput.Print( mOutputCategory, DEBUG_ALERT, "Server didn't want us to leave yet.. sorry server..");
	}

	mStream->Close();
	delete mStream;
	mStream = NULL;
}

//////////////////////////////////////////////////////////////////////////////

int EMail::GetRecipientCount()
{
	return sRecipientCount;
}

//////////////////////////////////////////////////////////////////////////////

const EMailRecipient* EMail::GetRecipient(int index)
{
	ASSERT(index < sRecipientCount);

	const EMailRecipient* curr;

	for ( curr = sFirstRecipient; index > 0; index-- )
		curr = curr->GetNextRecipient( );

	return curr;
}

//////////////////////////////////////////////////////////////////////////////

EMailRecipient::EMailRecipient( const char* DisplayName, const char* EMailAddress )
{
	mAddress = EMailAddress;
	mDisplayName = DisplayName;

	// link us in..
	// just taking it on the beginning leads to reversing the list, which
	// is counter-intuitive elsewhere...
	mNextRecipient = NULL;
	EMailRecipient* curr = sFirstRecipient;
	if ( curr == NULL )
	{
		sFirstRecipient = this;
	}
	else
	{
		while ( curr->mNextRecipient )
		{
			curr = curr->mNextRecipient;
		}
		curr->mNextRecipient = this;
	}

	sRecipientCount++;
}

//////////////////////////////////////////////////////////////////////////////

EMailRecipient::~EMailRecipient( )
{
	// remove us from the list...
	if ( sFirstRecipient == this)
	{
		sFirstRecipient = mNextRecipient;
	}
	else
	{
		EMailRecipient* prior = sFirstRecipient;
		while ( prior && prior->mNextRecipient != this )
		{
			prior = prior->mNextRecipient;
		}
		if ( prior )
		{
			prior->mNextRecipient = mNextRecipient;
		}
	}

	sRecipientCount--;
}

//////////////////////////////////////////////////////////////////////////////

const char* EMailRecipient::GetEMailAddress( ) const
{
	return mAddress;
}

//////////////////////////////////////////////////////////////////////////////

const char* EMailRecipient::GetDomain( ) const
{
	const char* addy = GetEMailAddress( );

	while ( *addy && *addy != '@' )
		addy++;

	if ( *addy == '@' )
		return addy+1;

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////

const char* EMailRecipient::GetDisplayName( ) const
{
	return mDisplayName;
}

//////////////////////////////////////////////////////////////////////////////

const EMailRecipient* EMailRecipient::GetNextRecipient( ) const
{
	return mNextRecipient;
}

