#include "pch.h"
#include "debug.h"

#include <assert.h>

void Debugf( char *fmt, ... )
{
	static char szOutput[2048+128];

	szOutput[2048] = 0;

	va_list args;
	va_start( args, fmt );
	vsprintf( szOutput, fmt, args );
	va_end( args );

	if( szOutput[2048] != 0 )
		assert(0);

	OutputDebugString( szOutput );
}