/* assert.c */

/******************* includes ****************************/

#include "Layers/LayersPCH.h"
#include "platform/systemi.h"

#ifndef DISABLE_ASSERTS

#define ASSERT_SUPPORT_IGNORE
#define ASSERT_SUPPORT_IGNORE_ALWAYS
#define ASSERT_SUPPORT_ABORT
#define ASSERT_SUPPORT_QUIT

#define ASSERT_NOASSERT 0
#define ASSERT_ABORT 1
#define ASSERT_QUIT 2
#define ASSERT_IGNORE 3
#define ASSERT_IGNORE_ALWAYS 4

namespace Assert
{
	static bool disableAsserts = false;
	static const char* sExpr;
	static const char* sFile;
	static const char* sString;
	static int sLine;

	bool GCNAssertHandler( const char *expr, const char *file, int line, const char* string)
	{
		dbgPrint( "Assertion failure:\n");
		if( string)
		{
			dbgPrint( "String: %s\n", string);
		}
		dbgPrint( "Expression: %s\n", expr);
		dbgPrint( "File %s\n", file);
		dbgPrint( "Line %d\n", line);

#ifdef ASSERT_SUPPORT_IGNORE_ALWAYS
		if( IgnoreAlways( expr, file, line, string))
		{
			dbgPrint( "Ignoring\n");
			return( true);
		}
#endif
		if( g_inputPtr == NULL)
		{
			LOCKUP( );
			return( true);
		}

		ControllerInputDriver *controller = g_input.GetController( 0);

		if( controller == NULL)
		{
			LOCKUP( );
			return( true);
		}

		dbgPrint( " Press A to ignore\n");
		dbgPrint( " Press B to ignore always\n");

		while( true)
		{
			VIWaitForRetrace( );

			g_input.BeginInput( );

			g_input.DoneInput( );

			if( controller->ButtonPressed( CB_X))
			{
				return( true);
			}
			else
			if( controller->ButtonPressed( CB_SQUARE))
			{
				SetIgnoreAlways( expr, file, line, string, true);
				return( true);
			}
		}
	}
};

#endif //DISABLE_ASSERTS
