/*************************************/
/*                                   */
/*   DummyWatchclient.cpp            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   fix-up for dbl viewer           */
/*                                   */
/*************************************/

/* the dbl viewer uses the Game project, which calls WatchClient::Reset. currently the dbl viewer is very simple, and
separate from the Gamelib projects. including the WatchClient project means including lots of other Gamelib projects,
so I'm going to just cobble up a dummy class to satisfy the WatchClient reference at link time */

/******************* includes ****************************/

/* system includes */

/* engine includes */

#include "DummyWatchclient.h"

/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

/******************* externs *****************************/

/******************* global variables ********************/

/******************* local variables *********************/

static WatchClient DummyWatchclient;

/******************* local prototypes ********************/

/******************* functions ***************************/

WatchClient *WatchClient::Instance()
{
	return(&DummyWatchclient);
}

void WatchClient::Reset()
{
}
