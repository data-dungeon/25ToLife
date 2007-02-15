/***************************************************************************/
// Simple game loop abstract base class
/***************************************************************************/

// Precompiled header.
#include "Loop/LoopPCH.h"

// My header
#include "Loop/gameloop.h"

/******************************************************************************/
/******************************************************************************/
GameLoop::GameLoop(SLoopEntry* i_stateArray, int i_loopState) :
	d_stateArray(i_stateArray),
	d_loopState(i_loopState)
{
	ASSERT_PTR(d_stateArray);
	d_requestExit = false;

	d_lastStateTime = 0;
	d_lastLoopState = d_loopState;
}

/******************************************************************************/
/******************************************************************************/
bool GameLoop::RunLoop(void)
{
	// Find the new state given the previous state.
	d_loopState = DetermineState(d_loopState);

	// If in a wait and the exit conditions have been met, use the wait state.
	if (((d_loopState == WAIT_ON_MEMBER_FUNC) &&
			(this->*d_waitExitTestMemberFunc)()) ||
			((d_loopState == WAIT_ON_STATIC_FUNC) &&
			(*d_waitExitTestStaticFunc)()))
	{
		d_loopState = d_waitState;
	}

//#ifdef _DEBUG
#if 1
	if ((d_loopState != d_lastLoopState) || ((d_loopState < 0) &&
			(d_loopState > UNDEFINED_STATE) && (d_waitState != d_lastWaitState)))
	{
		int now = g_timer.GetOSEndMS();
		if (d_lastLoopState >= 0)
		{
			float duration = (float)(now - d_lastStateTime) * 0.001f;
			g_debugOutput.TSPrint(g_debugOutput.Category("LOOP"),
					"Loop %s: Exited %s after %.2fs", LoopName(),
					d_stateArray[d_lastLoopState].d_name, duration);
		}
		else if (d_lastLoopState > UNDEFINED_STATE)
		{
			float duration = (float)(now - d_lastStateTime) * 0.001f;
			g_debugOutput.TSPrint(g_debugOutput.Category("LOOP"),
					"Loop %s: Stopped waiting for %s after %.2fs", LoopName(),
					d_stateArray[d_lastWaitState].d_name, duration);
		}
		else
		{
			g_debugOutput.TSPrint(g_debugOutput.Category("LOOP"),
					"Loop %s active", LoopName());
		}

		if (d_loopState >= 0)
		{
			g_debugOutput.TSPrint(g_debugOutput.Category("LOOP"),
					"Loop %s: Entered %s", LoopName(),
					d_stateArray[d_loopState].d_name);
		}
		else if (d_loopState > UNDEFINED_STATE)
		{
			g_debugOutput.TSPrint(g_debugOutput.Category("LOOP"),
					"Loop %s: Started waiting for %s", LoopName(),
					d_stateArray[d_waitState].d_name);
			d_lastWaitState = d_waitState;
		}
		else
		{
			g_debugOutput.TSPrint(g_debugOutput.Category("LOOP"),
					"Loop %s inactive", LoopName());
		}

		d_lastLoopState = d_loopState;
		d_lastStateTime = now;
	}
#endif

	// Discontinue if in an undefined state.
	if (d_loopState <= UNDEFINED_STATE)
		return(false);

	// Call the wait function if in a wait.
	if (d_loopState < 0)
	{
		if (d_waitFunc != (LoopFunc)NULL)
			(this->*d_waitFunc)();
	}

	// Call the function associated with the state.
	else if (d_stateArray[d_loopState].d_func != (LoopFunc)NULL)
		(this->*d_stateArray[d_loopState].d_func)();

	return(true);
}

/******************************************************************************/
/******************************************************************************/
int GameLoop::WaitForTask(TestMemberFunc i_waitExitTestFunc,
		LoopFunc i_waitFunc, int i_waitState)
{
	// Validate.
	ASSERT((i_waitState <= UNDEFINED_STATE) || (i_waitState >= 0));
	ASSERT(i_waitExitTestFunc != (TestMemberFunc)NULL);

	// Setup for wait.
	d_waitState = i_waitState;
	d_waitExitTestMemberFunc = i_waitExitTestFunc;
	d_waitFunc = i_waitFunc;

	return(WAIT_ON_MEMBER_FUNC);
}

/******************************************************************************/
/******************************************************************************/
int GameLoop::WaitForTask(TestStaticFunc i_waitExitTestFunc,
		LoopFunc i_waitFunc, int i_waitState)
{
	// Validate.
	ASSERT((i_waitState <= UNDEFINED_STATE) || (i_waitState >= 0));
	ASSERT(i_waitExitTestFunc != (TestStaticFunc)NULL);

	// Setup for wait.
	d_waitState = i_waitState;
	d_waitExitTestStaticFunc = i_waitExitTestFunc;
	d_waitFunc = i_waitFunc;

	return(WAIT_ON_STATIC_FUNC);
}
