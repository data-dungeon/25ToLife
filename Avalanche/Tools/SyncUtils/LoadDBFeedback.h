////////////////////////////////////////////////////////////////////////////
//
// LoadDBFeedback
//
// Class used dealing with the feedback while loading a database
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: LoadDBFeedback.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 10/16/02   Time: 2:44p
 * Updated in $/Avalanche/tools/SyncUtils
 * SyncDialog foundation
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/14/02   Time: 4:00p
 * Updated in $/Avalanche/tools/SyncUtils
 * simple feedback classes
 */

#ifndef SYNC_UTILS_DLL_LOAD_DB_FEEDBACK_H
#define SYNC_UTILS_DLL_LOAD_DB_FEEDBACK_H

//-----------------------------------------------------------------------------
// LoadDBFeedback class declaration:

class AFX_EXT_CLASS LoadDBFeedback
{
public:
	class Stats
	{
	public:
		int m_totalFilesScanned;
		TupString m_fileName;
		TupString m_errorMessage;

		Stats(void) : m_totalFilesScanned(0) {}
	};

	LoadDBFeedback(void);
	virtual ~LoadDBFeedback();
	virtual void Update(const Stats &stats);
	virtual bool GetAbortFlag(void);
};

//-----------------------------------------------------------------------------

#endif

