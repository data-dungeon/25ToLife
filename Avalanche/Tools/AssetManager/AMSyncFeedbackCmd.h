//
//	AMSyncFeedbackCmd.h
//

#ifndef __AMSyncFeedbackCmd__
#define __AMSyncFeedbackCmd__


//-----------------------------------------------------------------------------
// Class Declaration:

class AMSyncFeedbackCmd: public SyncFeedback
{
public:
	// Constructors & Destructors:
	AMSyncFeedbackCmd();
	~AMSyncFeedbackCmd();

	void UpdateStats(void);
	void UpdateError(void);
	void ClearStats(void);
	bool GetAbortFlag(void) const;

	void AddToLog(int flags,const char *pFormatString, ...);
	void ClearLog(void);
};

#endif
