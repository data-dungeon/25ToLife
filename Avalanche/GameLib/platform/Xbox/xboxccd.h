/***************************************************************************/
// Xbox command console display
/***************************************************************************/
#ifndef XboxCCD_H
#define XboxCCD_H

// our base class
#include "cmdcon/ccondisp.h"

class XboxCommandConsoleDisplay : public CommandConsoleDisplay
{
public:
	// The init/shutdown
	XboxCommandConsoleDisplay(CommandConsole &parent);
	~XboxCommandConsoleDisplay(void) {}

	// Display this new line on the console
	bool NewLine(const char *display);
};

#endif

