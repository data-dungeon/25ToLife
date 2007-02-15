//
//	AMLoadDBFeedback.h
//

#ifndef __AMLOADDBFEEDBACK__
#define __AMLOADDBFEEDBACK__

class LoadDBDialog;

//-----------------------------------------------------------------------------
// Class Declaration:

class AMLoadDBFeedback: public LoadDBFeedback
{
public:
	enum LoadStatus
	{
		STATUS_NULL,
		STATUS_LOADING_LEFT_DB,
		STATUS_LOADING_RIGHT_DB,
		STATUS_BUILDING_SYNC_TREE,
		STATUS_BUILDING_SYNC_TREE_WITH_CRC,
		STATUS_BUILDING_TREE_CONTROLS,
	};
private:
	LoadStatus m_loadStatus;
	LoadDBDialog *m_pDialog;

public:
	// Constructors & Destructors:
	AMLoadDBFeedback();
	~AMLoadDBFeedback();

	void SetDialog(LoadDBDialog *pDialog);
	void SetStatus(LoadStatus loadStatus);
	LoadStatus GetStatus(void) const;
	void Update(const LoadDBFeedback::Stats &stats);
	bool GetAbortFlag(void);
};

#endif
