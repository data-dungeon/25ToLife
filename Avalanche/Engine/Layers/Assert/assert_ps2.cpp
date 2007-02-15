/* assert.c */

/******************* includes ****************************/

#include "Layers/LayersPCH.h"

#ifndef DISABLE_ASSERTS

#include <libgraph.h>
#include <libdev.h>
#include <eekernel.h>
#include <libpad.h>
#include "Display/Common/DisplayPlatform.h"
#include "Display/PS2/Renderer/Code/MasterList.h"
#include "EngineHelper/LoadScreen.h"

#include "Layers/Assert/parseelf.h"

#include "Network/Connection.h"
#include "cmdcon/cmdcon.h"

#define ASSERT_SUPPORT_IGNORE
#define ASSERT_SUPPORT_IGNORE_ALWAYS
#define ASSERT_SUPPORT_ABORT
#define ASSERT_SUPPORT_QUIT
#define ASSERT_SUPPORT_EMAIL

#define ASSERT_NOASSERT			0x00
#define ASSERT_ABORT				0x01
#define ASSERT_QUIT				0x02
#define ASSERT_IGNORE			0x04
#define ASSERT_IGNORE_ALWAYS	0x08
#define ASSERT_EMAIL				0x10

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 224
#define OFFX (((4096-SCREEN_WIDTH)/2)<<4)
#define OFFY (((4096-SCREEN_HEIGHT)/2)<<4)

#define PAUSETIME 10
#define PAUSETIMESTR "10"

#define CONSWIDTH 38
#define CONSHEIGHT 27

#define CONSOLE_BLACK 0
#define CONSOLE_BLUE 1
#define CONSOLE_RED 2
#define CONSOLE_MAGENTA 3
#define CONSOLE_GREEN 4
#define CONSOLE_CYAN 5
#define CONSOLE_YELLOW 6
#define CONSOLE_WHITE 7

#define MAX_EXECUTABLE_NAME_SIZE 100

extern "C" 
{
	extern char _end[];
	extern char ENTRYPOINT[];
};


namespace Assert
{

	// Digital Buttons
	enum
	{
		BUTTON_LEFT = 0x80,
		BUTTON_DOWN = 0x40,
		BUTTON_RIGHT = 0x20,
		BUTTON_UP = 0x10,
		BUTTON_START = 0x08,
		BUTTON_R3 = 0x04,
		BUTTON_L3 = 0x02,
		BUTTON_SEL = 0x01
	};
	enum
	{
		BUTTON_SQUARE = 0x80,
		BUTTON_X = 0x40,
		BUTTON_O = 0x20,
		BUTTON_TRIANGLE = 0x10,
		BUTTON_R1 = 0x08,
		BUTTON_L1 = 0x04,
		BUTTON_R2 = 0x02,
		BUTTON_L2 = 0x01
	};
	enum
	{
		CTRL_NOTHING   = 0x00000000,

		CTRL_DLEFT     = 0x00008000,
		CTRL_DDOWN     = 0x00004000,
		CTRL_DRIGHT    = 0x00002000,
		CTRL_DUP       = 0x00001000,
		CTRL_START     = 0x00000800,
		CTRL_R3        = 0x00000400,
		CTRL_L3        = 0x00000200,
		CTRL_SELECT    = 0x00000100,

		CTRL_SQUARE    = 0x00000080,
		CTRL_X         = 0x00000040,
		CTRL_O         = 0x00000020,
		CTRL_TRIANGLE  = 0x00000010,
		CTRL_R1        = 0x00000008,
		CTRL_L1        = 0x00000004,
		CTRL_R2        = 0x00000002,
		CTRL_L2        = 0x00000001,

		CTRL_TIMEOUT   = 0x00010000,
	};

	static const int ASSERT_IGNORE_KEYS = CTRL_X;
	static const int ASSERT_IGNORE_ALWAYS_KEYS = CTRL_START;
	static const int ASSERT_ABORT_KEYS = CTRL_SELECT;
	static const int ASSERT_QUIT_KEYS = CTRL_L1|CTRL_L2|CTRL_R1|CTRL_R2;
	static const int ASSERT_EMAIL_KEYS = CTRL_TRIANGLE;

	// we need to disable asserts while we cleanup.. otherwise, we 
	// may assert forever..
	static bool disableAsserts = false;

	// controller support
	static int hasControllerIdled;
	static bool hasControllerSupport;

	// storage of the executable name
	static char executableName[MAX_EXECUTABLE_NAME_SIZE] = "";

	// storage for which addy to send emails to
	static int CurrEmailAddy = 0;

	void SetExecutableName( const char* executable )
	{
		// executable name is too long, increase the define above...
		if ( strlen( executable ) > MAX_EXECUTABLE_NAME_SIZE-1 )
			LOCKUP();

		strcpy( executableName, executable );
	}

#if defined(PS2) && defined(SN)
	static char stringBuffer[ASSERT_BUFFER_LEN];

	// this reuses a static buffer, so each call wipes the prior call
	builtstring StringLookup( const char* string )
	{
		// 0x20000000 is used as a flag to say that it should not be lookedup again
		// the memory address is still valid (its just through uncached memory)...
		if ( (u32)string & 0x20000000 )
			return string;

		// see if our file name is set... if not, assume string is valid...
		if ( executableName[0] == '\0' )
			return string;

		// can we open the elf file?, if not, we just return the string
		// and hope for the best...:(
		ElfParser elfParser;
		if ( !elfParser.Open( executableName ) )
			return "Cant load ASSERT string from the elf file!  Perhaps your fileserver root is bad?";

		// see if we have a section by the requested name
		if ( !elfParser.LoadSectionHeader( ".assert" ) )
			return string;

		// see if section is loaded...
		if ( elfParser.IsSectionLoadable( ) )
			return string;

		// this is not foolpoof, but until your data segment starts at an address == size of
		// your strings section, it should work fine.  For the PS2, thats around 2-3 MB of strings
		// so for now we are ok...
		if ( !elfParser.IsInSection( (u32)string ) )
			return string;

		// read the string in ...
		if ( !elfParser.ReadCString( string, stringBuffer, ASSERT_BUFFER_LEN ) )
			return "Cant load ASSERT string from the elf file!  Read error!";

		// 0x20000000 is used as a flag to say that it should not be lookedup again
		// the memory address is still valid (its just through uncached memory)...
		return (const char*) ( ((u32)stringBuffer) | 0x20000000 );
	}
#endif

	static void SetupDisplay( )
	{
		// shut down the load screen...
		if ( LoadScreen::IsRunning() )
		{
			LoadScreen::Kill();
		}
		// stall for everything to be complete..
		MasterList::Flush( );
		MasterList::BlockDMA( true );
		asm("EI"); // we dont need interupts disabled, just masterlist to be blocked.
		sceGsSyncPath( 0, 0 );

		// reset the PS2 video stuff to base defaults!!!!
		// we want 320x240 for this stuff..
		sceGsResetPath( );
		sceGsResetGraph( 0, SCE_GS_NOINTERLACE, SCE_GS_NTSC, SCE_GS_FRAME );

		u64 setting_chain[] = {
			0x0000000070000005l,	// dmatag
			0x5000000500000000l,	// viftag
			0x4000000000008001l, 0xEEEEEEEEEEEEEEEEl,	// giftag
			SCE_GS_SET_ALPHA_1( 0, 1, 0, 1, 0x80 ), SCE_GS_ALPHA_1,
			SCE_GS_SET_PABE( 0 ), SCE_GS_PABE,
			SCE_GS_SET_TEXA( 0xFF, 1, 0x00 ), SCE_GS_TEXA,
			SCE_GS_SET_FBA( 0 ), SCE_GS_FBA_1,
		};
		FlushCache( 0 );
		DMA_Wait( D1_CHCR );
		DPUT_D1_QWC( 0 );
		DPUT_D1_TADR( (u_int) (u_int) &setting_chain[0] );	// 0x80000000 means buffer is in scratchpad
		DPUT_D1_CHCR( D_CHCR_DIR_M | D_CHCR_STR_M | D_CHCR_SCHAIN_MODE_M | D_CHCR_TTE_M );
		DMA_Wait( D1_CHCR );

		// reset the controller...
		hasControllerSupport = sceSifSearchModuleByName( "padman" ) >= 0;
	}

	static void RestoreDisplay( )
	{
		// reset to games video modes
		disableAsserts = true;
		MasterList::BlockDMA( false );
		Display__ResetGraphics( );
		disableAsserts = false;
	}

	static int ReadController()
	{
		if ( !hasControllerSupport )
		{
			hasControllerIdled++;
			if ( (hasControllerIdled >= 60*PAUSETIME) )
				return CTRL_TIMEOUT;
			return CTRL_NOTHING;
		}

		int state = scePadGetState( 0, 0 );
		if ( state != scePadStateStable &&
			  state != scePadStateFindCTP1 )
			return CTRL_NOTHING;

		// lets be a bit overcautious
		unsigned char databuff[64] __attribute__((aligned(64)));
		FlushCache( 0 );

		if ( scePadRead( 0, 0, databuff ) == 0 )
			return CTRL_NOTHING;

		FlushCache( 0 );

		unsigned char* data = (unsigned char*)((u32)databuff | 0x20000000);

		u32 keys = 0;
		if ( data[0] == 0x00 )
		{
			if ( hasControllerIdled )
			{
				return( ((~data[2]) << 8) & 0xFF00) | ((~data[3]) & 0x00FF );
			}
			else
			{
				if ( (data[2] == 0xFF) && (data[3] == 0xFF) )
					hasControllerIdled = true;
			}
		}
		return CTRL_NOTHING;
	}

	static int GetControllerResults( int assertType )
	{
		int ctrl = ReadController( );

		if ( ctrl == CTRL_TIMEOUT )
		{
			if ( assertType & ASSERT_IGNORE )
				return ASSERT_IGNORE;
			if ( assertType & ASSERT_ABORT )
				return ASSERT_ABORT;
			if ( assertType & ASSERT_QUIT )
				return ASSERT_QUIT;
			return ASSERT_NOASSERT;
		}

		if ( ctrl == ASSERT_IGNORE_KEYS && (assertType & ASSERT_IGNORE) )
			return ASSERT_IGNORE;
		if ( ctrl == ASSERT_ABORT_KEYS && (assertType & ASSERT_ABORT) )
			return ASSERT_ABORT;
		if ( ctrl == ASSERT_IGNORE_ALWAYS_KEYS && (assertType & ASSERT_IGNORE_ALWAYS) )
			return ASSERT_IGNORE_ALWAYS;
		if ( ctrl == ASSERT_QUIT_KEYS && (assertType & ASSERT_QUIT) )
			return ASSERT_QUIT;
		if ( ctrl == ASSERT_EMAIL_KEYS && (assertType & ASSERT_EMAIL) )
			return ASSERT_EMAIL;

		return ASSERT_NOASSERT;
	}

	static void leftString( int console, int line, const char* string, int offset = 0 )
	{
		sceDevConsLocate( console, offset, line );
		sceDevConsPrintf( console, "%s", string );
	}

	static void centerMsg( int console, int line, const char* string )
	{
		int x = (CONSWIDTH - strlen( string )) / 2;
		sceDevConsMessage( console, x, line, string );
	}

	static int GenerateAssertText( int console, const char* expr, const char* file, int line, int assertType, const char* artistString )
	{
		char buffer[400];

		hasControllerIdled = false;

		sceDevConsClear( console );

//	sceDevConsFrame(console, 0, 0, CONSWIDTH, CONSHEIGHT);
		int position = 1;
		sceDevConsAttribute( console, CONSOLE_RED );
		centerMsg( console, position++, "Assertion failure!" );
		position+=2;

		if ( artistString )
		{
			sceDevConsAttribute( console, CONSOLE_GREEN );
			centerMsg( console, position, "Hopefully Readable String" );
			sceDevConsFrame( console, -1, position-1, CONSWIDTH+2, 8 );
			sceDevConsAttribute( console, CONSOLE_YELLOW );
			leftString( console, position+2, StringLookup( artistString ) );
			position += 7;
		}

		sceDevConsAttribute( console, CONSOLE_GREEN );
		centerMsg( console, position, "Expression and Location" );
		sceDevConsFrame( console, -1, position-1, CONSWIDTH+2, 100 );
		position += 2;

		sceDevConsAttribute( console, CONSOLE_GREEN );
		leftString( console, position, "Expr" );
		sceDevConsAttribute( console, CONSOLE_YELLOW );
		leftString( console, position, StringLookup( expr ), 5 );
		position += 3;

		sceDevConsAttribute( console, CONSOLE_GREEN );
		leftString( console, position, "Line" );
		sprintf( buffer, "%d", line );
		sceDevConsAttribute( console, CONSOLE_YELLOW );
		leftString( console, position, buffer, 5 );

		position += 2;
		sceDevConsAttribute( console, CONSOLE_GREEN );
		leftString( console, position, "File" );
		sceDevConsAttribute( console, CONSOLE_YELLOW );
		leftString( console, position, StringLookup( file ), 5 );

		sceDevConsAttribute( console, CONSOLE_YELLOW );

		return CONSHEIGHT-1;
	}
	static void GenerateAssertScreen( int console, const char* expr, const char* file, int line, int assertType, const char* artistString )
	{
		int position = GenerateAssertText(console, expr, file, line, assertType, artistString );
		if ( hasControllerSupport )
		{
			if ( assertType & ASSERT_QUIT )
				leftString( console, position--, "L1+R1+L2+R2 - Exit the application", 2 );

			if ( assertType & ASSERT_ABORT )
				leftString( console, position--, "Select - Stop in debugger", 2 );

			if ( assertType & ASSERT_IGNORE_ALWAYS )
				leftString( console, position--, "Start - Always ignore this assert", 2 );

			if ( (assertType & ASSERT_EMAIL) && (assertType & ASSERT_IGNORE) )
				leftString( console, position--, "X - continue    TRIANGLE - Email", 2 );
			else if ( assertType & ASSERT_IGNORE )
				leftString( console, position--, "X - continue", 2 );
			else if ( assertType & ASSERT_EMAIL )
				leftString( console, position--, "                TRIANGLE - Email", 2 );

			position--;
		}
		else
		{
			char* pasmsgline2 = NULL;
			if ( assertType & ASSERT_IGNORE )
				pasmsgline2 = "then ignore this assert";
			else if ( assertType & ASSERT_ABORT )
				pasmsgline2 = "then stop in the debugger";
			else if ( assertType & ASSERT_QUIT )
				pasmsgline2 = "then exit the app";
			else
				pasmsgline2	= "then I'll just keep waiting";

			leftString( console, position--, pasmsgline2, 2 );
			leftString( console, position--, "yet!  I'll pause for " PAUSETIMESTR " seconds,", 2 );
			leftString( console, position--, "The controller hasn't been inited", 2 ), position--;
		}

		sceDevConsAttribute( console, CONSOLE_GREEN );
		centerMsg( console, position, "Controller Commands" );
		sceDevConsFrame( console, -1, position-1, CONSWIDTH+2, CONSHEIGHT - position + 2 );
	}

	static void GenerateSendingScreen( int console, const char* expr, const char* file, int line, int assertType, const char* artistString )
	{
		int position = GenerateAssertText(console, expr, file, line, assertType, artistString );

		sceDevConsAttribute( console, CONSOLE_GREEN );
		centerMsg( console, position, "Sending Email" );
		sceDevConsFrame( console, -1, position-1, CONSWIDTH+2, CONSHEIGHT - position + 2 );
	}

	static void GenerateEmailScreen( int console, const char* expr, const char* file, int line, int assertType, const char* artistString )
	{
		char buffer[400];

		hasControllerIdled = false;

		sceDevConsClear( console );

		int position = 1;
		sceDevConsAttribute( console, CONSOLE_RED );
		centerMsg( console, position++, "Assertion failure!" );
		position+=2;

		sceDevConsAttribute( console, CONSOLE_GREEN );
		centerMsg( console, position, "Select the person to send email to" );
		sceDevConsFrame( console, -1, position-1, CONSWIDTH+2, 20 );
		position+=2;

		int emailCount = EMail::GetRecipientCount( );
		for ( int i = 0; position < 22 && i < emailCount; position++, i++ )
		{
			if ( CurrEmailAddy == i )
			{
				sprintf( buffer,  "* %s",  EMail::GetRecipient( i )->GetDisplayName( ) );
				sceDevConsAttribute( console, CONSOLE_RED );
			}
			else
			{
				sprintf( buffer,  "  %s",  EMail::GetRecipient( i )->GetDisplayName( ) );
				sceDevConsAttribute( console, CONSOLE_YELLOW );
			}
			leftString( console, position, buffer );
		}

		position = 26;
		sceDevConsAttribute( console, CONSOLE_YELLOW );
		leftString( console, position--, "TRIANGLE - Cancel Email", 2 );
		leftString( console, position--, "X - Send the email", 2 );
		position--;

		sceDevConsAttribute( console, CONSOLE_GREEN );
		centerMsg( console, position, "Controller Commands" );
		sceDevConsFrame( console, -1, position-1, CONSWIDTH+2, CONSHEIGHT - position + 2 );
	}

	static void RenderAssert( sceGsDBuff* dbuff, int console )
	{
		static int toggle = 0;
		sceDevConsDraw( console );
		sceGsSyncPath( 0, 0 );
		sceGsSyncV( 0 );
		sceGsSwapDBuff( dbuff, toggle ^= 1 );
		sceGsSyncPath( 0, 0 );
	}

	static void HandleEmail( sceGsDBuff* dbuff, int console, const char* expr, const char* file, int line, int assertType, const char* artistString )
	{
		GenerateEmailScreen( console, expr, file, line, assertType, artistString );
		int result = CTRL_NOTHING;

		while ( result == CTRL_NOTHING )
		{
			// --- check with Dwain about why this is being removed.  It's causing us grief because multiple asserts are hitting
#if 0
			if ( ValidSingleton( CConnection ) )
			{
				g_connection.Service( );
			}
#endif
			result = ReadController( );

			switch ( result )
			{
			case CTRL_TIMEOUT:
			case CTRL_TRIANGLE:
				break;
			case CTRL_X:
				GenerateSendingScreen( console, expr, file, line, assertType, artistString );
				RenderAssert( dbuff, console );
				RenderAssert( dbuff, console );
				EmailAssert( CurrEmailAddy, expr, file, line, artistString );
				break;
			case CTRL_DDOWN:
				if ( CurrEmailAddy > 0 )
					CurrEmailAddy--;
				GenerateEmailScreen( console, expr, file, line, assertType, artistString );
				result = CTRL_NOTHING;
				break;
			case CTRL_DUP:
				if ( CurrEmailAddy+1 < EMail::GetRecipientCount() )
					CurrEmailAddy++;
				GenerateEmailScreen( console, expr, file, line, assertType, artistString );
				result = CTRL_NOTHING;
				break;
			default:
				result = CTRL_NOTHING;
				break;
			}

			RenderAssert( dbuff, console );
		}
		GenerateAssertScreen( console, expr, file, line, assertType, artistString );
	}

	static int DisplayAssert( const char* expr, const char* file, int line, int assertType, const char* artistString )
	{
		// get ready
		SetupDisplay( );

		// ok.. now for the fun part...
		// build the console buffer...
		int console = sceDevConsOpen( OFFX + (8 << 4), OFFY + (6 << 4), CONSWIDTH, CONSHEIGHT );

		#define BRIT 0x90
		sceDevConsSetColor( console, CONSOLE_BLACK,   0x00, 0x00, 0x00 );
		sceDevConsSetColor( console, CONSOLE_BLUE,    0x00, 0x00, 0xff );
		sceDevConsSetColor( console, CONSOLE_RED,     0xff, 0x00, 0x00 );
		sceDevConsSetColor( console, CONSOLE_MAGENTA, BRIT, 0x00, BRIT );
		sceDevConsSetColor( console, CONSOLE_GREEN,   0x00, BRIT, 0x00 );
		sceDevConsSetColor( console, CONSOLE_CYAN,    0x00, BRIT, BRIT );
		sceDevConsSetColor( console, CONSOLE_YELLOW,  BRIT, BRIT, 0x00 );
		sceDevConsSetColor( console, CONSOLE_WHITE,   BRIT, BRIT, BRIT );

		GenerateAssertScreen( console, expr, file, line, assertType, artistString );

		// loop until we get input...
		int result = ASSERT_NOASSERT;
		sceGsDBuff dbuff;
		sceGsSetDefDBuff(&dbuff, 2, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 2, 1);
		FlushCache(0);

		while ( result == ASSERT_NOASSERT )
		{
// --- check with Dwain about why this is being removed.  It's causing us grief because multiple asserts are hitting
#if 0
			if ( ValidSingleton( CConnection ) )
			{
				g_connection.Service( );
			}
#endif
			result = GetControllerResults( assertType );

			if ( result == ASSERT_EMAIL )
			{
				HandleEmail( &dbuff, console, expr, file, line, assertType, artistString );
				result = ASSERT_NOASSERT;
			}
			RenderAssert( &dbuff, console );
		}

		// close the console buffer
		sceDevConsClose( console );

		// put things back
		RestoreDisplay( );

		return result;
	}

	bool OnScreen( const char *expr, const char *file, int line, const char* string )
	{
		// provide asserts on screen with abort-ignore functionality...

		// we only get in here if an assert has failed... so search the table of file/line
		// pairs so we can see if ignore has happened for this assert...

		int assertType = 0;

		if ( disableAsserts )
			return true;

#ifdef ASSERT_SUPPORT_EMAIL
		if ( EMail::GetRecipientCount( ) != 0 )
		{
			assertType |= ASSERT_EMAIL;
		}
#endif

#ifdef ASSERT_SUPPORT_ABORT
		assertType |= ASSERT_ABORT;
#endif

#ifdef ASSERT_SUPPORT_IGNORE
		assertType |= ASSERT_IGNORE;
#endif

#ifdef ASSERT_SUPPORT_IGNORE_ALWAYS
		assertType |= ASSERT_IGNORE_ALWAYS;
		if ( IgnoreAlways( expr, file, line, string ) )
			return true;
#endif

		const char* loadedstring;
		// this must be split up because StringLookup can only handle 1 string at a time
		loadedstring = StringLookup( file );
		dbgPrint( "Assertion failed.  file: %s, line: %d\n", loadedstring, line );
		loadedstring = StringLookup( expr );
		dbgPrint( " expr: %s\n", loadedstring );
		if ( string )
		{
			loadedstring = StringLookup( string );
			dbgPrint( "String: %s\n",  loadedstring );
		}


		int result = DisplayAssert( expr, file, line, assertType, string );

#ifdef ASSERT_SUPPORT_IGNORE_ALWAYS
		if ( result == ASSERT_IGNORE_ALWAYS )
			SetIgnoreAlways( expr, file, line, string, true );
#endif // ASSERT_SUPPORT_IGNORE

#ifdef ASSERT_SUPPORT_QUIT
		if ( result == ASSERT_QUIT )
			exit( 0 );
#endif

#ifdef ASSERT_SUPPORT_ABORT
		if ( result == ASSERT_ABORT )
			return false;
#endif

		return true;
	}


};

// ----------------------------------------------------------------------------
#define DRE 30
#define DWE 29
#define DVE 28
#define DUE 21
#define BED 15
static u32 s_badr;
static bool s_read;
static bool s_write;
static bool s_setVal;
static u32 s_value;
static u32 s_blockSize;
static bool bOutput = false;


void Assert::HardwareBreakpoint(u32 addr, bool read, bool write, bool setVal, u32 value, u32 blockSize )
{
	register u32 temp1 = 0;

	// --- save last set of data
	s_badr = addr;
	s_read = read;
	s_write = write;
	s_setVal = setVal;
	s_value = value;
	s_blockSize = blockSize;
	if( bOutput )
		g_console.Echo( "Add Hardware Breakpoint -> adr=%x, read=%d, write=%d, val=%d, size=%x\n", addr, read, write, value, blockSize);

	asm("di");
	asm("mfbpc %0" : : "r" (temp1) );

#if 0
	// --- address mask
	temp1 = 0xffffffff;
	u32 mask = 1;
	while( blockSize > 1)
	{
		temp1 ^= mask;
		mask <<= 1;
		blockSize >>= 1;
	}
	u32 newaddr = addr;
#else
	// this will find the biggest block that we can protect using address masking
	// that is completely contained inside [addr .. addr+blockSize-1]
	u32 end = addr + blockSize - 1;
	u32 ignore = ~0;
	u32 newaddr;
	u32 newend;
	do {
		ignore >>= 1;
		newaddr = (addr & ignore) ? ((addr | ignore) + 1) : addr;
		newend  = newaddr | ignore;
	} while (newaddr < addr || newend > end);
	u32 mask = ~ignore;
#endif

	// --- set the address of our hardware breakpoint
	temp1 = newaddr;
	asm("mtdab %0" : : "r" (temp1) );

	// --- set the address mask
	temp1 = mask;
	asm("mtdabm %0" : : "r" (temp1) );


	// --- set data value if we want one
	if( setVal )
	{
		temp1 = value;
		asm("mtdvb %0" : : "r" (temp1) );

		temp1 = ~0;
		asm("mtdvbm %0" : : "r" (temp1) );
	}

	// --- enable read, write, value, and user mode in control register
	temp1 = 0;
	if( read ) 
		temp1 |= ( 1 << DRE );
	if( write ) 
		temp1 |= ( 1 << DWE );
	if( setVal ) 
		temp1 |= ( 1 << DVE );
	if( temp1 == 0 ) 
		temp1 |= ( 1 << BED );
	if( read || write  )
		temp1 |= ( 1 << DUE );
	asm("mtbpc %0" : : "r" (temp1) );
	asm("ei");
	asm("sync.p" );

}
#endif // DISABLE_ASSERTS
