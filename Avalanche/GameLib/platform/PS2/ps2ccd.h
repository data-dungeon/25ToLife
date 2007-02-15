/***************************************************************************/
// PS2 command console display
/***************************************************************************/
#ifndef PS2CCD_H
#define PS2CCD_H

// our base class
#include "cmdcon/ccondisp.h"

class PS2CommandConsoleDisplay : public CommandConsoleDisplay
{
public:
	// The init/shutdown
	PS2CommandConsoleDisplay(CommandConsole &parent);
	~PS2CommandConsoleDisplay(void) {}

	// Display this new line on the console
	bool NewLine(const char *display);
};

#endif

