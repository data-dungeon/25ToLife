//
//	AMSyncFeedback.h
//

#ifndef __AMSYNCFEEDBACK__
#define __AMSYNCFEEDBACK__

class SyncDialog;

//-----------------------------------------------------------------------------
// Class Declaration:

class AMSyncFeedback: public SyncFeedback
{
	SyncDialog *m_pDialog;
public:
	// Constructors & Destructors:
	AMSyncFeedback();
	~AMSyncFeedback();

	void SetDialog(SyncDialog *pDialog);
	void UpdateStats(void);
	void UpdateError(void);
	void ClearStats(void);
	bool GetAbortFlag(void) const;

	void AddToLog(int flags,const char *pFormatString, ...);
	void ClearLog(void);
};

#endif
