/////////////////////////////////////////////////////////////////////////////
// Simple time logging api
/////////////////////////////////////////////////////////////////////////////

/******************* includes ****************************/

#include "profile/ProfilePCH.h"





/////////////////////////////////////////////////////////////////////////////
// Output the elapsed seconds to the log
/////////////////////////////////////////////////////////////////////////////
void LogTime::LogElapsed(
char *title)
{
	int calls = Calls();

	if (calls == 0)
		g_log.Write("%s was not called.\n");
	else if (calls == 1)
		g_log.Write("%s spent %4.2fs in 1 call.\n", title, TotalSeconds());
	else
		g_log.Write("%s s[ent %4.2fs in %d calls.  Average: %4.4fs\n", title, TotalSeconds(), calls, AverageSeconds());
}

