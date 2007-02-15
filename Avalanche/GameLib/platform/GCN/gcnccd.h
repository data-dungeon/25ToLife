/***************************************************************************/
// GCN command console display
/***************************************************************************/
#ifndef GCNCCD_H
#define GCNCCD_H

// our base class
#include "cmdcon/ccondisp.h"

class GCNCommandConsoleDisplay : public CommandConsoleDisplay
{
public:
	// The init/shutdown
	GCNCommandConsoleDisplay( CommandConsole &parent);
	~GCNCommandConsoleDisplay( void) { }

	// Display this new line on the console
	bool NewLine( const char *display);
};

#endif

