/*************************************************************************/
// GCN application framework.
/*************************************************************************/
#ifndef GCNAPP_H
#define GCNAPP_H

struct ts_RequestResetMsg
{
	int	resetType;
	u32	resetCode;
	bool	forceMenu;
};

class GCNApp
{
public:
	// The constructor and destructors
	GCNApp(void);
	virtual ~GCNApp(void) {}

	// Who am I?
	static GCNApp *IAm(void)
		{ return(me); }

	// Startup
	virtual bool Initialize(char *commandLine) { return(true); }

	// Shutdown...although shutdown is pointless
	virtual void Terminate(void) {}

	// Called every processing "frame"
	virtual void Process(void) {}

	// We are quitting!
	void Quit(void);
	inline bool KeepProcessing(void)
	{
		return( !quit);
	}

	u32 GetARAMBase( void)
	{
		return( m_aramBase);
	}

	u32 GetVMSize( void)
	{
		return( sm_vmVirtualSize);
	}

	u32 GetVMBase( void)
	{
		return( sm_vmBase);
	}

	void SetMemoryCardInProgress( bool state)
	{
		m_bMemoryCardInProgress = state;
	}
	
	bool IsMemoryCardInProgress( void)
	{
		return( m_bMemoryCardInProgress);
	}
	
	void InitGCNOS( int argc, char *argv[]);

	void GetPADStatus( PADStatus *pPADStatus)
	{
		memcpy( pPADStatus, m_aPADStatus, sizeof m_aPADStatus);
	}

	void RequestReset( ts_RequestResetMsg *pResetRequest);

protected:

	enum
	{
		RESET_THREAD_STACK_SIZE = 4096
	};

	void HandleReset( int reset, u32 resetCode, BOOL forceMenu);

	static void * ResetThread( void *pParam);

	bool						quit;
	static GCNApp			*me;

	u32						m_aramBase;

	static const u32 		sm_vmVirtualSize;

	static const u32		sm_vmBase;

	static const u32		sm_vmPhysicalSize;

	bool						m_bMemoryCardInProgress;

	u8							*m_pResetThreadStack;

	OSThread					m_ResetThread;

	OSThread					*m_pGameThread;

	PADStatus				m_aPADStatus[PAD_MAX_CONTROLLERS];

	OSMessageQueue			m_ResetRequestQueue;

	OSMessage				m_aResetRequestMessages[2];
};

#endif
