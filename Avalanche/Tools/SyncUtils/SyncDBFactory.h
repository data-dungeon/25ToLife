////////////////////////////////////////////////////////////////////////////
//
// SyncDBFactory
//
// Class to creating sync databases
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: SyncDBFactory.h $
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

#ifndef SYNC_UTILS_DLL_SYNC_DB_FACTORY_H
#define SYNC_UTILS_DLL_SYNC_DB_FACTORY_H

//-----------------------------------------------------------------------------
// SyncDBFactory class declaration:

class SyncDBBase;
class LoadDBFeedback;
class LogFile;


class AFX_EXT_CLASS SyncDBFactory  
{
public:
	SyncDBBase *CreateDB(const char *pFilePath,
								LoadDBFeedback &loadDBFeedback,
								LogFile &logFile);

};

//-----------------------------------------------------------------------------

#endif

