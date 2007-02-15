//--------------------------------------------------------------//
// Thread.h
//
// Cross platform Thread API
//
//--------------------------------------------------------------//

#ifndef _CTHREAD_H
#define _CTHREAD_H

//--------------------------------------------------------------//



//--------------------------------------------------------------//

class CThread
{
public:

	//defines
	enum {
		CTHREAD_PRIORITY_LOWEST,
		CTHREAD_PRIORITY_BELOW_NORMAL,
		CTHREAD_PRIORITY_NORMAL,
		CTHREAD_PRIORITY_ABOVE_NORMAL,
		CTHREAD_PRIORITY_HIGHEST,
		CTHREAD_PRIORITY_TIME_CRITICAL,
		CTHREAD_PRIORITY_IDLE,
	};

	// funcs
	CThread();
	~CThread();

	#if defined (PS2)
	bool	Create( void ThreadFunction(void*), int stacksize = 16384, int Priority = CTHREAD_PRIORITY_NORMAL, int arg = 0);		// create a new thread
	#elif defined (WIN32)
	bool	Create( int* ThreadFunction, int stacksize = 0, int Priority = CTHREAD_PRIORITY_NORMAL, int arg = 0);					// create a new thread
	#endif

	bool	Destroy();									// destroy the thread
	void	Exit();										// Exit a thread
	void	ExitDelete();								// Exit a thread and delete it
	bool	Start();										// start thread executing
	bool	Stop();										// stop thread executing
	bool	Execute();									// execute a thread that is in a wait state (already started)
	void	YieldThread (int time);					// yield to other tasks
	bool	SetPriority(int Priority);				// set thread priority
	int	GetPriority();								// get thread priority
	void	SleepTime(int Time);						// make this thread sleep specified time
	int	GetStatus();								// get status of thread (PS2)
	int	GetWaitStatus();							// get wait status (PS2)

	#if defined (PS2)
	unsigned int	GetHandle() { return (Handle); };		// return handle of thread
	#endif

	#if defined (WIN32)
	HANDLE			GetHandle() { return (Handle); };		//	return handle of thread
	#endif


private:

	#if defined (PS2)
	ThreadParam		m_ThreadParams;													// thread params
	char		m_ThreadStack[16384] __attribute__ ((aligned(16)));		// thread stack
	unsigned int	Handle;																// thread handle
	#endif

	#if defined (WIN32)
	HANDLE			Handle;																// thread handle
	#endif

	int				m_Arg;																// Argument passed to thread
};

#endif // _THREAD_H
