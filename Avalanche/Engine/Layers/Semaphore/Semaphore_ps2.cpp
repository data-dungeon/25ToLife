#include "Layers/LayersPCH.h"
#include "Layers/Semaphore/Semaphore.h"

#if 0
// return true if interrupts are enabled
asm  bool bInterruptsEnabled()
{
	u64 retval;
	asm __volatile__ (".set noreorder");
	asm __volatile__ ("sync.p");
	asm __volatile__ ("mfc0 %0, $12" : "=r" (retval));
	asm __volatile__ (".set reorder");
	return (retval & (1<<16)) != 0x0000;
}
#endif

#if 0
// return true if we are not in an interrupt
asm bool bNotInInterrupt()
{
	u64 retval;
	asm __volatile__ (".set noreorder");
	asm __volatile__ ("sync.p");
	asm __volatile__ ("mfc0 %0, $12" : "=r" (retval));
	asm __volatile__ (".set reorder");
	return (retval & 1) != 0x0000;
}
#endif

// return true if we are not in an interrupt and interrupts are enabled
bool bInterruptable()
{
	u64 retval;
	asm __volatile__ (".set noreorder");
	asm __volatile__ ("sync.p");
	asm __volatile__ ("mfc0 %0, $12" : "=r" (retval));
	asm __volatile__ (".set reorder");
	return (retval & ((1<<16) | 1)) == ((1<<16) | 1);
}


CSemaphore::CSemaphore(bool initialize)
{
	semaphoreID = -1;
	threadID = -1;
	lockCount = 0;
	awaitingLock = 0;
	if(initialize)
		Initialize();
}

CSemaphore::~CSemaphore()
{
	ASSERT(semaphoreID != -1);
	DeleteSema(semaphoreID);
}

// initialize and uninited semaphore
void CSemaphore::Initialize()
{
	ASSERTS(semaphoreID == -1, "Semaphore already initialized");
	SemaParam params;
	params.initCount = 0;
	params.maxCount = 1;
	params.option = 0;
	semaphoreID = CreateSema(&params);
	ASSERT(semaphoreID != -1);
}

// determine if initialize still needs to be called
bool CSemaphore::IsInitialized()
{
	return semaphoreID != -1;
}

// aquire the semaphore or return false if its not available.
bool CSemaphore::Aquire()
{
	// semaphores can't be used with interrupts disabled
	ASSERT(bInterruptable());

	int tID = GetThreadId();

	DI();

	if (threadID < 0)
	{
		threadID = tID;
		lockCount = 1;
	}
	else if (threadID == tID)
	{
		// we're holding this semaphore
		lockCount++;
	}
	else
	{
		// another thread is holding this semaphore
		asm("ei");
		return false;
	}

	asm("ei");
	return true;
}

// aquire the semaphore and block until its available
void CSemaphore::AquireUnconditionally()
{
	// semaphores can't be used with interrupts disabled
	ASSERT(bInterruptable());

	int tID = GetThreadId();

TryLock:

	DI();

	if (threadID < 0)
	{
		threadID = tID;
		lockCount = 1;
	}
	else if (threadID == tID)
	{
		// we're holding this semaphore
		lockCount++;
	}
	else
	{
		// another thread is holding this semaphore
		awaitingLock++;
		asm("ei");
		int result = WaitSema(semaphoreID);
		goto TryLock;
	}

	asm("ei");
}

// release the semaphore.
void CSemaphore::Release()
{
	// semaphores can't be used with interrupts disabled
	ASSERT(bInterruptable());

	int tID = GetThreadId();

	DI();

	if (threadID == tID)
	{
		// we're holding this semaphore
		lockCount--;
		if (lockCount == 0)
		{
			threadID = -1;
			if (awaitingLock)
			{
				awaitingLock--;
				asm("ei");
				SignalSema(semaphoreID);
			}
		}
		asm("ei");
	}
	else
	{
		LOCKUP();
	}
}
