// WatchCmdLine.cpp: implementation of the CWatchCmdLine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "watch.h"
#include "WatchCmdLine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWatchCmdLine::CWatchCmdLine()
{

}

CWatchCmdLine::~CWatchCmdLine()
{

}

void CWatchCmdLine::ParseParam( LPCTSTR lpszParam, BOOL bFlag, BOOL bLast )
{
	// Fill the global variable that contains the address of the ps2 machine.
	sscanf(lpszParam, "%s", remoteMachine);

//	CCommandLineInfo::ParseParam(lpszParam, bFlag, bLast);
}
