/*************************************************************************/
// Xbox application framework.
/*************************************************************************/
#ifndef XBOXAPP_H
#define XBOXAPP_H

#include "platform/Xbox/xbfont.h"
#include <xfont.h>

class XboxApp
{
public:
	// The constructor and destructors
	XboxApp(void);
	virtual ~XboxApp(void) {}

	// Who am I?
	static XboxApp *IAm(void)
		{ return(me); }

	// Startup
	virtual bool Initialize(char *commandLine) { return(true); }

	// Shutdown...although shutdown is pointless
	virtual void Terminate(void) {}

	// Called every processing "frame"
	virtual void Process(void) {}

	// We are quitting!
	void Quit(void);
	inline bool KeepProcessing(void)
		{ return(!quit); }
	
protected:
	bool				quit;
	static XboxApp	*me;
};

extern	CXBFont			g_Font;
extern	XFONT*			g_pDefaultTrueTypeFont;  // Pointer to the Default Arial TrueTypeFont

#endif
