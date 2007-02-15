////////////////////////////////////////////////////////////////////////////
//
// SyncDBFactory
//
// Class to creating sync databases
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: SyncDBFactory.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 10/16/03   Time: 12:35p
 * Updated in $/Avalanche/tools/SyncUtils
 * more perforce
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/27/03    Time: 4:34p
 * Updated in $/Avalanche/tools/SyncUtils
 * log file output
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 10/16/02   Time: 2:44p
 * Updated in $/Avalanche/tools/SyncUtils
 * SyncDialog foundation
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/04/02   Time: 4:01p
 * Updated in $/Avalanche/tools/SyncUtils
 * first pass of tree, factories, and syncdb files
 */

//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#include "SyncUtils.h"
#include "SyncDBFactory.h"
#include "SyncDBSimple.h"
#include "SyncDBServerClient.h"
#include "ServerClient.h"
//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


SyncDBBase *SyncDBFactory::CreateDB(const char *pFilePath,
												LoadDBFeedback &loadDBFeedback,
												LogFile &logFile)
{
//	TupString serverName,serverPath;
//	ServerClient::GetServerPath(pFilePath,serverName,serverPath);
//	if (ServerClient::FindServer(serverName,serverPath))
//	{
//		return new SyncDBServerClient(pFilePath,loadDBFeedback,logFile);
//	}
//	else
//	{
		return new SyncDBSimple(pFilePath,loadDBFeedback,logFile);
//	}
}

