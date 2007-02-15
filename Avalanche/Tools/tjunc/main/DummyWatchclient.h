/*************************************/
/*                                   */
/*   DummyWatchclient.h              */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   fix-up for dbl viewer           */
/*                                   */
/*************************************/

/* the dbl viewer uses the Game project, which calls WatchClient::Reset. currently the dbl viewer is very simple, and
separate from the Gamelib projects. including the WatchClient project means including lots of other Gamelib projects,
so I'm going to just cobble up a dummy class to satisfy the WatchClient reference at link time */

#ifndef __DUMMYWATCHCLIENT_H
#define __DUMMYWATCHCLIENT_H

/******************* includes ****************************/

/* system includes */

/* engine includes */

/******************* forward declarations ****************/

/******************* defines *****************************/

/******************* compile control *********************/

/******************* macros ******************************/

/******************* structures **************************/

/******************* global variables ********************/

/******************* global prototypes *******************/

/******************* class *******************************/

class WatchClient
{
public:

	/* Constructor/destructor. xml node provides name of instance/model for actor to use */

	WatchClient() {}
	~WatchClient() {}

	/* to satisfy link */

	static WatchClient* Instance();
	void Reset();
};

#endif __DUMMYWATCHCLIENT_H
