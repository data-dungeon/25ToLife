/***************************************************************************/
// Xbox command console display
/***************************************************************************/

#include "platform/PlatformPCH.h"

#include "Layers/Debug.h"
#include "platform/Xbox/xboxccd.h"

/***************************************************************************/
/***************************************************************************/
XboxCommandConsoleDisplay::XboxCommandConsoleDisplay(
CommandConsole &parent) :
CommandConsoleDisplay(parent)
{
}

/***************************************************************************/
// Display this new line on the console
/***************************************************************************/
bool XboxCommandConsoleDisplay::NewLine(
const char *display)
{
#if 1
	conPuts("***CONSOLE->");
	conPuts(display);
	conPuts("\n");
#else
	while (*display)
	{
		char temp[128];
		int len = 0;

		while (len < sizeof(temp) - 1 && *display)
		{
			if (*display == '\n')
			{
				temp[len++] = '\r';
				temp[len++] = '\n';
			}
			else if (*display != '\r')
				temp[len++] = *display;

			++display;
		}
		temp[len] = '\0';

		conPuts(temp);
	}
#endif
	return(true);
}

