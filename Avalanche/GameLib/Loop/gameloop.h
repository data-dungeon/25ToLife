/***************************************************************************/
// Simple game loop abstract base class
/***************************************************************************/
#ifndef GAMELOOP_H
#define GAMELOOP_H

#include "Layers/Assert/Assert.h"

class GameLoop;
typedef void (GameLoop::*LoopFunc)(void);
typedef bool (GameLoop::*TestMemberFunc)(void);
typedef bool (*TestStaticFunc)(void);
struct SLoopEntry
{
	const char* d_name;
	LoopFunc d_func;
};

class GameLoop
{
  public:
	virtual ~GameLoop(void) {}

	bool RunLoop(void);
	virtual void RequestExit(void) {d_requestExit = true;}
	virtual void ClearRequestExit(void) {d_requestExit = false;}

  protected:
	GameLoop(SLoopEntry* i_stateArray, int i_loopState);
	virtual int DetermineState(int i_state) = 0;
	int GetLoopState(void) {return(d_loopState);}

	int WaitForTask(TestMemberFunc i_waitExitTestFunc, LoopFunc i_waitFunc,
			int i_waitState);
	int WaitForTask(TestStaticFunc i_waitExitTestFunc, LoopFunc i_waitFunc,
			int i_waitState);
	enum
	{
		UNDEFINED_STATE = -3,
		WAIT_ON_MEMBER_FUNC = -2,
		WAIT_ON_STATIC_FUNC = -1
	};

	bool d_requestExit;

	// Some debugging stuff
	virtual const char *LoopName() = 0;

  private:
	GameLoop(void);

	SLoopEntry* d_stateArray;
	int d_loopState;

	int d_waitState;
	LoopFunc d_waitFunc;
	union
	{
		TestMemberFunc d_waitExitTestMemberFunc;
		TestStaticFunc d_waitExitTestStaticFunc;
	};

	// Debugging
	int d_lastStateTime;
	int d_lastLoopState;
	int d_lastWaitState;
};

#endif
