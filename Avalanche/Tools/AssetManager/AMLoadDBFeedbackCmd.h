//
//	AMLoadDBFeedbackCmd.h
//

#ifndef __AMLOADDBFEEDBACKCMD__
#define __AMLOADDBFEEDBACKCMD__


//-----------------------------------------------------------------------------
// Class Declaration:

class AMLoadDBFeedbackCmd: public LoadDBFeedback
{
public:
	// Constructors & Destructors:
	AMLoadDBFeedbackCmd();
	~AMLoadDBFeedbackCmd();

	void Update(const LoadDBFeedback::Stats &stats);
	bool GetAbortFlag(void);
};

#endif
