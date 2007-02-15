/***************************************************************************/
// Command console
// All of this is written for simplicity, not for speed
/***************************************************************************/
#ifndef CCONDISP_H
#define CCONDISP_H

class CommandConsole;

// Used for displaying the command console
class CommandConsoleDisplay
{
public:
	CommandConsoleDisplay(CommandConsole &parent) : cmdCon(parent) {}
	virtual ~CommandConsoleDisplay(void) {}

	// Display this new line on the console
	virtual bool NewLine(const char *display) = 0;

protected:
	CommandConsole		&cmdCon;
};

#endif
