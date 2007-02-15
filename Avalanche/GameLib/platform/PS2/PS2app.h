/*************************************************************************/
// PS2 application framework.
/*************************************************************************/
#ifndef PS2APP_H
#define PS2APP_H

// Turn this all off for CDROM
#ifndef CONSUMER_BUILD
#define ATMONSUPPORT
#endif

// Power off stack
#define POWEROFF_STACKSZ		(512 * 16)

class PS2App
{
public:
	// The constructor and destructors
	PS2App();
	virtual ~PS2App() {}

	// Who am I?
	static PS2App *IAm()
		{ return(me); }

	// Startup
	virtual bool Initialize(char *commandLine)
		{ return(true); }

	// To get fisrt screen visible as fast as possible, some initialization is deferred - this routine finishes this deferred work
	void FinishInitialize();

	// Shutdown...although shutdown is pointless
	virtual void Terminate() {}

	// Called every processing "frame"
	virtual void Process() {}

	// We are quitting!
	void Quit();
	inline bool KeepProcessing()
		{ return(!m_quit); }

	// Setup assert support
	void SetupAssertSupport(const char *exeName);

	// ATMon support (returns true if it worked)
	const char *SetupATMon(int argc, char *argv[]);
	void CloseATMon();

	// Boot!
	void Boot();

	// Setup the network adapter
	void SetupNetworkAdapter();

	// Build the command line (you delete)
	char *BuildCommandLine(int argc, char *argv[]);

	// Power off handler stuff
	void PreparePowerOff();
	void UndoPowerOff();

	// Module Loader
	bool LoadModule(char *moduleName, int argSize=0, char *argp=0 );
	bool LoadStartModule(char *moduleName, int argSize=0, char *argp=0, int *result=0 );

	// Do we have TTY output?
	bool HasTTYOutput();

	// Do we have a Network Adapter
	bool HasNetworkAdapter( void )
		{ return m_networkAdapterPresent; }
	void SetNetworkAdapter( bool TorF )
		{ m_networkAdapterPresent = TorF; }

	//// Determine refresh rate
	//void DetermineRefreshRate();

	static bool		m_iopResumePending;

protected:
	static PS2App	*me;
	bool				m_quit;
	bool				m_networkAdapterPresent;

	// The types of boots
	bool BootIOP();

	// Check the ROM version
	void CheckROM();

	// verify the correct boot parameters are used.
	void CheckBootParams(const char* romversion);

#ifdef ATMONSUPPORT
	static int m_atMonPowerOffThreadID;
	static int m_mainThreadID;
	static char m_atMonPowerOffStack[4096] __attribute__ ((aligned(16)));

	static void ATMonPowerOffHandler(void* data);
	static void ATMonPowerOffProcess(void* data);
#endif

	// Support for power off thread
	static u_char m_powerOffStack[POWEROFF_STACKSZ];
	int m_sid;
	int m_tid;

	// Power off stuff
	static void PowerOffThread(void *arg);
	static void PowerOffHandler(void *arg);
};

extern	int	g_NoBootIOP;


#endif
