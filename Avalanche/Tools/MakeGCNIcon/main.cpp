#include <stdio.h>
#include <stdarg.h>

#include "CmdLine.h"
#include "GCNIconData.h"
#include "MakeGCNIcon.h"

int main( int argc, char *argv[])
{

	CGCNIconData		gcnIconData;

	if( !g_CmdLine.Initialize( argc, argv))
	{
		return( Usage( ));
	}

	gcnIconData.ReadCommandFile( g_CmdLine.m_pInputFile);
	gcnIconData.OutputIconFile( g_CmdLine.m_pOutputFile);
}

int Usage( void)
{
	printf( "MakeGCNIcon - creates a GCN icon data file for use with the Avalanche Engine\n");
	printf( "  Usage: MakeGCNIcon [<option> [<option>...]] <InputCommandFile> <OutputFile>\n");
	printf( "  Where:\n");
	printf( "    <option> is one of:\n");
	printf( "       -v[erbose] -- print processing messages\n");
	printf( "    <InputCommandFile> is a text file containing icon file creation parameters\n");
	printf( "    <OutputFile> is the name of the file to be created\n");
	return( -1);
}

void OutputMsg( const char *pFormat, ...)
{

	va_list	argList;

	if( !g_CmdLine.m_Verbose)
	{
		return;
	}

	va_start( argList, pFormat);

	vprintf( pFormat, argList);
}

void ErrorMsg( const char *pFormat, ...)
{

	va_list	argList;

	va_start( argList, pFormat);

	vprintf( pFormat, argList);
}

void FatalMsg( const char *pFormat, ...)
{

	va_list	argList;

	va_start( argList, pFormat);

	vprintf( pFormat, argList);

	exit( -1);
}

