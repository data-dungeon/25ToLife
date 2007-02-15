///////////////////////////////////////////////////////////////////////////////
// This is a simple implementation of a platform independent thread
///////////////////////////////////////////////////////////////////////////////
private:
	enum
	{
		STACK_SIZE = ( 32 * 1024)
	};

	u8					m_stack[OSThreadObject::STACK_SIZE];
	OSThread			m_thread;
	OSThreadQueue	m_threadQueue;
	OSAlarm			m_alarm;
	bool				m_running;

	static void 	*ThreadEntry( void *pParam );

	static void		ThreadAlarmHandler( OSAlarm* alarm, OSContext* context );

public:
	// called by the thread to suspend its execution for milliSecs milliseconds
	static void DelayThread( u32 milliSecs );
};

