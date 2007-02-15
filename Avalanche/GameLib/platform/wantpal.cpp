///////////////////////////////////////////////////////////////////////////////
// do you want pal or not?
///////////////////////////////////////////////////////////////////////////////
#include "platform/PlatformPCH.h"

// Much ado about one byte
bool g_wantPAL = false;
bool g_regionUSA = true;

// We only compile one ELF, and then patch it for NTSC or PAL.  The
// following key is the search pattern for the SetElfNtscPal.pl perl script.
// The last byte should be "N" for Ntsc or "P" for Pal.
#define KEY_LEN 64
#define KEY_MODE (KEY_LEN-2)	//-1 is '\0' terminator, -2 is last char.
unsigned char s_PALSearchKey[ KEY_LEN ] =
	"9ux5RRtl6x9b4iKOCWPT0wn8t2tA5Ve7ZX0WcfRLSrMAEWszGNrtDB2YxYCn__P";

void SetRegionAndPAL()
{
	if ( s_PALSearchKey[ KEY_MODE ] == 'N' )
	{
		g_regionUSA = true;
		g_wantPAL = false;
	}
	else if ( s_PALSearchKey[ KEY_MODE ] == 'P' )
	{
		g_regionUSA = false;
		g_wantPAL = true;
	}
}
