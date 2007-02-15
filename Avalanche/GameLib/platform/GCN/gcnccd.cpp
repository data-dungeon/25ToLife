/***************************************************************************/
// GCN command console display
/***************************************************************************/

/******************* includes ****************************/

#include "platform/PlatformPCH.h"





/***************************************************************************/
/***************************************************************************/
GCNCommandConsoleDisplay::GCNCommandConsoleDisplay( CommandConsole &parent) : CommandConsoleDisplay( parent)
{

}

/***************************************************************************/
// Display this new line on the console
/***************************************************************************/
bool GCNCommandConsoleDisplay::NewLine( const char *display)
{
	conPuts( "***CONSOLE->");
	conPuts( display);
	conPuts( "\n");
	return( true);
}

