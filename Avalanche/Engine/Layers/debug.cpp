/* debug.cpp */

/******************* includes ****************************/

#include "Layers/LayersPCH.h"

#ifndef CONSUMER_BUILD

#if defined( WIN32_TOOLS_DEBUG ) || defined( WIN32_TOOLS_RELEASE ) || defined( WIN32_TOOLS_HYBRID )
__declspec( dllimport ) int vsprintf( char *pcOut, const char *pcFmt, va_list args);
#else
extern int vsprintf( char *pcOut, const char *pcFmt, va_list args);
#endif

/* -----------------7/8/98 5:57PM--------------------
 * Defines
 * --------------------------------------------------*/
#define DBG_BUFFER_SIZE		512
#define DBG_TYPES				1280
#define DBG_TYPE_PER_LEVEL	128
#define DBG_MAX_LEVEL		9

#define DBG_TYPE_LONGS		(DBG_TYPES / sizeof(u32))

enum
{
	DBG_MOD_ON,
	DBG_MOD_OFF,
	DBG_MOD_TOGGLE
} ;

/* -----------------7/8/98 5:58PM--------------------
 * Local Prototypes
 * --------------------------------------------------*/
void _dbgSetTypeValue(u32 u32Type, unsigned char ucMod);

/* -----------------7/8/98 5:58PM--------------------
 * Local Data
 * --------------------------------------------------*/
u32	u32DbgTypeBits[DBG_TYPE_LONGS];
u8		u8CurrentLevel;

char	sDbgBuffer[DBG_BUFFER_SIZE];


/* -----------------7/8/98 5:58PM--------------------
 * Exportable Functions
 * --------------------------------------------------*/
void dbgInit(u8 u8StartLevel, bool bOn)
{
	int	i;
	u32	u32Clear;

	if (bOn)
		u32Clear = MAXUINT;
	else
		u32Clear = 0;

	for( i = 0; i < DBG_TYPE_LONGS; i++)
		u32DbgTypeBits[i] = u32Clear;

	u8CurrentLevel = u8StartLevel;
}

void dbgLevelSet(u8 u8Level)
{
	ASSERT( u8Level <= DBG_MAX_LEVEL);

	u8CurrentLevel = u8Level;
}

u8 dbgLevelGet( void)
{
	return u8CurrentLevel;
}


void dbgTypeOn(u32 u32Type)
{
	_dbgSetTypeValue(u32Type, DBG_MOD_ON);
}

void dbgTypeOff(u32 u32Type)
{
	_dbgSetTypeValue(u32Type, DBG_MOD_OFF);
}

void dbgTypeToggle(u32 u32Type)
{
	_dbgSetTypeValue(u32Type, DBG_MOD_TOGGLE);
}

bool dbgTypeStatus(u32 u32Type)
{
	u32 u32Idx;
	u32 u32Bit;
	u32 u32Mask;

	ASSERT( u32Type < DBG_TYPES);

	u32Idx = u32Type / sizeof(u32);

	u32Bit = (u32Type % sizeof(u32));

	u32Mask = 1 << u32Bit;

	return((u32DbgTypeBits[u32Idx] & u32Mask) ? (bool) TRUE : (bool) FALSE);
}

void dbgTPrint(u32 u32Type, const char *pcFmt, ...)
{
	va_list				PrintArgs;

	if ((u32Type / DBG_TYPE_PER_LEVEL) > u8CurrentLevel)
		return;

	if( dbgTypeStatus( u32Type) == FALSE)
		return;

	va_start( PrintArgs, pcFmt);

#if defined(GCN)
	std::vsprintf( sDbgBuffer, pcFmt, PrintArgs);
#else
	vsprintf( sDbgBuffer, pcFmt, PrintArgs);
#endif
	conPuts( sDbgBuffer);

	va_end( PrintArgs);
}

void dbgPrint( const char *pcFmt, ...)
{
	va_list				PrintArgs;

	va_start( PrintArgs, pcFmt);

#if defined(GCN)
	std::vsprintf( sDbgBuffer, pcFmt, PrintArgs);
#else
	vsprintf( sDbgBuffer, pcFmt, PrintArgs);
#endif
	conPuts( sDbgBuffer);

	va_end( PrintArgs);
}

/* -----------------7/8/98 5:58PM--------------------
 * Internal Functions
 * --------------------------------------------------*/

void _dbgSetTypeValue(u32 u32Type, unsigned char ucMod)
{
	u32 u32Idx;
	u32 u32Bit;
	u32 u32Mask;

	ASSERT(u32Type < (u32) DBG_TYPES);

	u32Idx = u32Type / sizeof(u32);

	u32Bit = (u32Type % sizeof(u32));

	u32Mask = 1 << u32Bit;

	if( ucMod == DBG_MOD_TOGGLE)
	{
		u32DbgTypeBits[u32Idx] ^= ~u32Mask;
	}
	else
	{
		u32DbgTypeBits[u32Idx] &= ~u32Mask;

		if( ucMod == DBG_MOD_ON)
			u32DbgTypeBits[u32Idx] |= u32Mask;
	}

}

#endif