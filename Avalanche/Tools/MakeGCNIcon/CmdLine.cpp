// CmdLine.cpp: implementation of the CCmdLine class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "CmdLine.h"

CCmdLine		g_CmdLine;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdLine::CCmdLine( void)
{
	m_pInputFile = NULL;
	m_pOutputFile = NULL;
	m_Verbose = false;
}

CCmdLine::~CCmdLine( void)
{
	if( m_pInputFile != NULL)
	{
		delete [] m_pInputFile;
	}
	if( m_pOutputFile != NULL)
	{
		delete [] m_pOutputFile;
	}
}

bool CCmdLine::Initialize( int argc, char *argv[])
{
	if( argc < 3)
	{
		return( false);
	}

	while( argv[1][0] == '-')
	{
		if( IsArg( "verbose", argv[1] + 1, 1))
		{
			m_Verbose = true;
		}
		else
		{
			printf( "Invalid command line option: %s\n", argv[1]);
			return( false);
		}
		argv++;
	}

	m_pInputFile = new char[ strlen( argv[1]) + 1];
	strcpy( m_pInputFile, argv[1]);

	m_pOutputFile = new char[ strlen( argv[2]) + 1];
	strcpy( m_pOutputFile, argv[2]);

	return( true);
}

bool CCmdLine::IsArg(const char *pArgLabel, const char *pArgValue, int iMinMatchLength) const
{

	int	iMatchLength = 0;

	if( strlen( pArgValue) > strlen( pArgLabel))
	{
		return( false);
	}

	for( size_t iChar = 0; iChar < strlen( pArgValue); iChar++)
	{
		if( tolower( pArgLabel[iChar]) != tolower( pArgValue[iChar]))
		{
			return( false);
		}
		iMatchLength++;
	}

	return( iMatchLength >= iMinMatchLength);
}
