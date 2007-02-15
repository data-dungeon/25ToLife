//===========================================================================
//=
//= ScriptAssert.h - special assert routines for console driven app
//=
//===========================================================================

#ifndef _SCRIPT_ASSERT_H
#define _SCRIPT_ASSERT_H

#include <stdarg.h>

static const char* _assertFile;
static int         _assertLine;
static const char* _assertExpr;

static bool _assertdata( const char* file, int line, const char* expr )
{
	_assertFile = file;
	_assertLine = line;
	_assertExpr = expr;
	return false;
}

static bool _assertdie( const char* fmt, ... )
{
	fprintf( stderr, "ERROR: ASSERTION FAILED: " );

	va_list args;
	va_start( args, fmt );
	vfprintf( stderr, fmt, args );
	va_end( args );

	fprintf( stderr, "\n  FILE: %s(%d)\n  EXPR: %s\n", _assertFile, _assertLine, _assertExpr );
	exit( 1 );
	return false;
}

#define ASSERT(  expr         ) ( expr ) || _assertdata( __FILE__, __LINE__, #expr ) || _assertdie( "" )
#define ASSERTS( expr, msg    ) ( expr ) || _assertdata( __FILE__, __LINE__, #expr ) || _assertdie( "%s", msg )
#define ASSERTF( expr, fmtmsg ) ( expr ) || _assertdata( __FILE__, __LINE__, #expr ) || _assertdie fmtmsg;

#endif //_SCRIPT_ASSERT_H
