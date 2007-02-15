#ifndef	__L_TASKING_H
#define	__L_TASKING_H

/******************* includes ****************************/

/* system includes */

/* engine includes */

#include "platform/BaseType.h"
#include "Layers/LayersStruct.h"
#include "Layers/Context.h"

/* The task control block
	This has been bastardized by JG to allow it to match the memory footprint
	of the CTask class defined below.  Sorry Todd.
*/

#define		TASKF_ACTIVE_DURING_PAUSE		0x00000001	//sleep updated during pause
#define		TASKF_REALTIME						0x00000002	//processed even during pause
#define		TASKF_UNSTOPPABLE					0x00000004	//other tasks and code cant kill me

#define		TASKF_SLEEPCOUNT		0x80000000
#define		TASKF_SLEEPTIME		0x40000000
#define		TASKF_SLEEPMASK		0xC0000000

struct ts_TCB
{
	void						*pLocalData;			/* _vftpr in CTask */
	ts_TCB					*prev,
								*next;
	void						( *pTaskDieFunction)( ts_TCB *pTCB);
	s32						s32TaskID,					/* these match exactly */
								s32TaskPriority,			//higher the number, higher the priority
								s32TaskSleepCount,
								s32TaskStackSize,
								s32TaskLocalDataSize;
	u32						u32Flags;
	float						fSleepTime;
	ts_Context				Context;	/* no context in CTask */

	bool						operator<( ts_TCB &operand)
	{
		return( this->s32TaskPriority < operand.s32TaskPriority);
	}
	bool						operator>( ts_TCB &operand)
	{
		return( this->s32TaskPriority > operand.s32TaskPriority);
	}
};

/* Global's */
extern ts_TCB		*pCurrentTask;				/* current task pointer */

/* All the function calls */
t_Error			tskInit( s32 s32NumTCBs);
void				tskReset();
void				tskShutdown();
ts_TCB *			tskCreate(	void ( *pTaskEntryPoint)( void),
									void ( *pTaskDieFunction)( ts_TCB *pTCB),
									s32 s32TaskID,
									s32 s32TaskPriority,
									s32 s32TaskStackSize,
									s32 s32TaskLocalDataSize,
									u32 u32Flags = 0);
void				tskSleep( int iSleepTime = 1);
void				tskSleep( float fSleepTime);
void				tskTransfer( ts_TCB *pTCB, void (*pTaskEntryPoint)( void));
void				tskDie( void);
void				tskDispatchLoop( float fElapsedRealTime, float fElapsedGameTime, bool bInPause);
void				tskKill( ts_TCB *pTCB);
void				tskKillClass( s32 s32Class, s32 s32ClassMask = 0xFFFFFFFF);
ts_TCB *			tskExist( s32 s32Class, s32 s32ClassMask = 0xFFFFFFFF, ts_TCB *pPrevTCB = NULL);

#if 0
//removing CTask for now

/******************************************************************************
 CTask's are a little bit different from standard C tasks in the tasking
 system.  A CTask's Run() function is constructed in the follwoing way:

 class YourTask : public CTask, public CAnotherClass
 {
 public:
	YourTask(void) { foo = 0; }
	~YourTask(void) {}

	// do the work
	void Run(void);

	// For all to see
	int funnything;

 private:
	int foo;
 }

 void YouTask::Run(void)
 {
	// automatic variables (not persistent)
	int bar;

	// do the task code
	++foo;
   bar = foo ^ ~foo;
	funnything ^= bar * 5;

	// Setup for another run
	Continue();  // same as Sleep(1)

	// Notice here we are returning...Continue/Sleep does not switch control
 }

 The equivalent in a normal C task would appear

 int funnything;  // global for everyone to see
 void FunnythingTask(void)
 {
	// these persist
	int bar;
	int foo;

	// do the task code
	while (1)
	{
		++foo;
   	bar = foo ^ ~foo;
		funnything ^= bar * 5;
		tskSleep(1);
	}

 // returning would be bad
 }

 Such is the difference between the two systems.  All persistent data must
 be members of the CTask subclass.  To return control to the tasking
 dispatcher, the actual Run() menthod must return.  To remove a CTask from
 the dispatcher list, Kill(), Die() or the destructor should be called.

 *****************************************************************************/
class CTask
{
public:
	// Empty
	inline CTask(void) { pTCBList = 0; }
	virtual ~CTask(void) { Die(); }

	// Add this task to the list
	// Once you've added it, it dies for three reasons:
	// - it calls Die() (doesn't free memory)
	// - Kill() is called on the task
	// - The destructor is called
	// - tskKill will not work
	inline bool Create(s32 s32TaskID, s32 s32TaskPriority, s32 s32TaskSleepCount);

	// The actual task function (return to sleep)
	virtual void Run(void) = 0;

	// Kill the task (you must return)
	inline void Die(void)
		{ tskKill(pTCBList); pTCBList = 0; }

	// Get/Set the sleep amount
	inline s32 &Sleep(void)
		{ return(s32SleepCount); }
	inline void Sleep(s32 &sleep)
		{ s32SleepCount = sleep; }
	inline void Continue(void)
		{ s32SleepCount = 1; }

	// These are available for outside of a CTask

	// Init the tasking system
	static t_Error Init(s32 s32NumTCBs = 32)
		{ return(tskInit(s32NumTCBs)); }

	// Process the dispatch loop
	static void DispatchLoop(void)
		{ tskDispatchLoop(); }

	// Kill a task
	static void Kill(CTask *task)
		{ delete task; }

	// Kill a class of tasks
	void KillClass(s32 s32Class, s32 s32ClassMask = 0xffffffff)
		{ tskKillClass(s32Class, s32ClassMask); }

	// Does this class exist?
	bool Exist(s32 s32Class, s32 s32ClassMask = 0xffffffff)
		{ return(tskExist(s32Class, s32ClassMask) != 0); }

	// Find the task
	CTask *Find(s32 s32Class, s32 s32ClassMask = 0xffffffff)
	{
		ts_TCBList *find = tskExist(s32Class, s32ClassMask);
		return(find ? GetCTask(find) : 0);
	}

protected:
	// Transfer control to another task (you must return)
	inline void Transfer(CTask &transferTo)
		{ transferTo.Run(); }

	// Given a TCBList entry, is it a CTask or pTCB entry?
	static bool IsCTask(ts_TCBList *pTCBList)
		{ return(pTCBList && ((s32)pTCBList->pTCB & 0x1)); }

	// Return the CTask pointer, (NULL if it is not a CTask)
	static CTask *GetCTask(ts_TCBList *pTCBList)
		{ return((pTCBList && ((s32)pTCBList->pTCB & 0x1)) ?
						(CTask *)((u32)pTCBList->pTCB & 0xfffffffe) : 0); }

private:
	ts_TCBList	*pTCBList;
	s32			s32ID;
	s32 			s32Priority;
	s32			s32SleepCount;
};

//---------------------------------------------------------------------------
// Add this task to the list
// Once you've added it, it dies for three reasons:
// - it sets sleep to 0
// - die is called
// - an external function call kills it
//---------------------------------------------------------------------------
ts_TCBList *tskCreateListEntry(void);
inline bool CTask::Create(
s32 s32TaskID,
s32 s32TaskPriority,
s32 s32TaskSleepCount)
{
	// Add it in the list
	pTCBList = tskCreateListEntry();
	if (!pTCBList)
		return(false);

	// setup our innards
	s32ID = s32TaskID;
	s32Priority = s32TaskPriority;
	s32SleepCount = s32TaskSleepCount;

	// Save it
	pTCBList->pTCB = (ts_TCB *)((u32)this | 0x1);
	return(true);
}
#endif
#endif //__L_TASKING_H

