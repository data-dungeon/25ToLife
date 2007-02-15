///////////////////////////////////////////////////////////////////////////////
// This is a simple implementation of a platform independent thread
///////////////////////////////////////////////////////////////////////////////
private:
	#define STACK_SIZE (1024*64)
	u128 				m_stack[STACK_SIZE / sizeof(u128)];
	volatile int 	m_threadID;
	volatile bool 	m_dieNow;

	static void ThreadEntry(OSThread* obj);
};

