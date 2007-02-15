
#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

// this is a simple single owner semaphore.
class CSemaphore
{
public:
	CSemaphore(bool initialize = true);
	~CSemaphore();

	// initialize and uninited semaphore
	void Initialize();

	// determine if initialize still needs to be called
	bool IsInitialized();

	// aquire the semaphore or return false if its not available.
	bool Aquire();

	// aquire the semaphore and block until its available
	void AquireUnconditionally();

	// release the semaphore.
	void Release();


private:

	// Include the rest of the definitions
#if defined(PS2)
#include "Semaphore_ps2.h"
#elif defined(GCN)
#include "Semaphore_gcn.h"
#elif defined (WIN32)
#include "Semaphore_win.h"
#endif


};

// pointer constructor supports null for a semaphore to mean dont do locking...
class CScopeLockSemaphore
{
	CSemaphore* m_Sema;
public:
	CScopeLockSemaphore(CSemaphore& sema)
		: m_Sema(&sema)
	{
		m_Sema->AquireUnconditionally();
	}
	CScopeLockSemaphore(CSemaphore* sema)
		: m_Sema(sema)
	{
		if(m_Sema)
			m_Sema->AquireUnconditionally();
	}
	~CScopeLockSemaphore()
	{
		if(m_Sema)
			m_Sema->Release();
	}
};

#endif //__SEMAPHORE_H__

