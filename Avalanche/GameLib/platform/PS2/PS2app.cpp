//*************************************************************************
// PS2 application framework.
//*************************************************************************
#include "platform/PlatformPCH.h"
#include "platform/wantpal.h"
#include "platform/PS2/DisplaySimple.h"
#include "platform/PS2/bootlogo.h"
#include "platform/PS2/bootlogoimage.h"
#include "platform/PS2/VU0RegisterPreserver.h"
//#include "platform/PS2/hertzimage.h"

#ifdef ASSERT_SUPPORT_ONSCREEN
#include <libdev.h>
#endif

#if defined(JJSFIX)
#include <sntty.h>
#endif

#if defined(SN)
#include <libsn.h>
#endif

// Define the IOPBOOTIMAGE as null to not reboot the iop
#define IOPBOOTIMAGE (g_regionUSA ? "DATA_PS2\\IOP\\SCEA\\DNAS300.IMG" : "DATA_PS2\\IOP\\SCEE\\DNAS300.IMG" )

// for getting region and initial PAL flag
extern void SetRegionAndPAL();

// If this is set, ATMon should be around
const char* g_RemoteIPAddress = NULL;

VU0RegisterPreserver g_VU0RegisterPreserver;

// No ATMon support in CD ROM builds
#ifdef ATMONSUPPORT
char PS2App::m_atMonPowerOffStack[4096] __attribute__ ((aligned(16)));
int PS2App::m_atMonPowerOffThreadID = -1;
int PS2App::m_mainThreadID = -1;
static int g_remoteFileHandle = -1;
#endif //ATMONSUPPORT

bool PS2App::m_iopResumePending = false;

static int g_remoteTTYHandle = -1;

int	g_NoBootIOP = 0;

// Support for poweroff
u_char PS2App::m_powerOffStack[POWEROFF_STACKSZ] __attribute__ ((aligned(16)));
void ProcessIOPResume( bool force );


//*************************************************************************
// A lean and mean class to get a bitmap displayed
//*************************************************************************

//*************************************************************************
// Here it is, our very own WinMain
//*************************************************************************

#define STACK_SIZE (0x40000 - 8192)	// should match _stack_size in .cmd file, minus some fudge because we don't start at the top necessarily.
#define STACK_FILL 0x01234567
#define STACK_VAR  u32

STACK_VAR* g_stackEnd;
STACK_VAR* g_stackStart;
int        g_stackFree = STACK_SIZE;

void cleanstack()
{
#if !defined(CONSUMER_BUILD)
	u32 end;
	__asm__ __volatile__( "\tmove\t%0, $29\n" : "=r" (end) : : "memory" );
	g_stackEnd = (STACK_VAR*) ((end - 256) & ~15);
	g_stackStart = (STACK_VAR*) ((int)g_stackEnd - STACK_SIZE);
	for (STACK_VAR* f = g_stackStart; f < g_stackEnd; f++)
		*f = STACK_FILL;
#endif
}

int stackcheck()
{
#if !defined(CONSUMER_BUILD)
	STACK_VAR* f;
	for (f = g_stackStart; f < g_stackEnd; f++)
		if (*f != STACK_FILL)
			break;
	int free = (int)f - (int)g_stackStart;
	if (free < g_stackFree)
		g_stackFree = free;
	return free;
#else
	return -1;
#endif
}

int main(int argc, char* argv[])
{
	cleanstack();

	// FIRST THIGS FIRST -- set the region and pal flag appropriately
	SetRegionAndPAL();

	// throw SOMETHING on the screeen immediately.
	sceGsResetGraph(0, SCE_GS_INTERLACE, g_wantPAL ? SCE_GS_PAL : SCE_GS_NTSC, SCE_GS_FIELD);
	PS2DisplaySimple display;
	display.Init(640, g_wantPAL ? 512 : 448, g_wantPAL);
	BootLogo::DisplayImage(&display, bootlogoimage);

	// reset scratchpad ram
	memset((void *)0x70000000, 0, 16 * 1024);

	// Who am I?
	PS2App *app = PS2App::IAm();

	// Setup assert support
	app->SetupAssertSupport(argc > 0 ? argv[0] : NULL);

	// set the main thread priority
	ChangeThreadPriority(GetThreadId(), APPLICATION_THREAD_PRIORITY);

	// To support ATMON & ATMON, we need to see if we have a atfile: based argv[0], if so
	// we need to save it off for MediaHAL:: to use...
	// this needs to be done before modules are loaded
	g_RemoteIPAddress = app->SetupATMon(argc, argv);

	// check for bootstrap loading
	for (int i=0; i< argc; i++)
	{
		if (!strcmp (argv[i], "-noiop"))
			g_NoBootIOP = 1;
	}

	// Do the boot shit
	app->Boot();

	// Build the commandline
	char *commandLine = app->BuildCommandLine(argc, argv);

	//// If we're pal, get user input if we should run at 50 or 60 hertz
	//app->DetermineRefreshRate();

	// Setup everything
	bool ret = app->Initialize(commandLine);


	ASSERT(ret);

	// get rid of the commandline
	delete [] commandLine;

	// Process the game
	while (app->KeepProcessing())
	{
		app->Process();
		ProcessIOPResume( false );
	}

	// Shouldn't get here...unless we are Xbox
	ASSERT(g_RemoteIPAddress);

	// Shutdown everything
	app->Terminate();

	// Do we really need to call this?
	sceSifExitCmd();

	// Close ATMon
	app->CloseATMon();

	// Undo it!
	//app->UndoPowerOff();

	return 0;
}

//*************************************************************************
//*************************************************************************
PS2App::PS2App()
{
	m_quit = false;
	m_networkAdapterPresent = false;
	m_sid = -1;
	m_tid = -1;
	m_iopResumePending = false;
}

//*************************************************************************
// We are quitting! (does nothing on PS2)
//*************************************************************************
void PS2App::Quit()
{
	m_quit = true;
}

//*************************************************************************
	// Do we have TTY output?
//*************************************************************************
bool PS2App::HasTTYOutput()
{
#ifdef CDROM
	return g_RemoteIPAddress;
#else
	return true;
#endif
}

//*************************************************************************
// Finish any deferred initialization
//*************************************************************************
void PS2App::FinishInitialize()
{
	// Setup the network stuff
	SetupNetworkAdapter();

	//initialize memory card subsys
	sceMcInit();

	m_iopResumePending = false;
}

//*************************************************************************
// Call to load PS2 modules
//*************************************************************************
bool PS2App::LoadModule(char *moduleName, int argSize, char *argp )
{
#ifdef CDROM
#define MAX_RETRIES 10
#else
#define MAX_RETRIES 1
#endif

	int nResult, nTries = 0;
  	while ( ((nResult = sceSifLoadModule(Media::GetFileName(moduleName), argSize, argp ) ) < 0) && (nTries++ < MAX_RETRIES) )
	{
		dbgPrint("Can't load module %s\n", Media::GetFileName(moduleName));
  	}

	ASSERTS(nResult >= 0, "Error loading IOP module.  Make sure your file server root is set right");

	return (nResult >= 0);
}

/*************************************************************************/
/*************************************************************************/
bool PS2App::LoadStartModule(
char *moduleName,
int argSize,
char *argp,
int *result )
{
#ifdef CDROM
#define MAX_RETRIES 10
#else
#define MAX_RETRIES 1
#endif

	int nResult, nTries = 0;
	while ( ((nResult = sceSifLoadStartModule(Media::GetFileName(moduleName), argSize, argp, result ) ) < 0) && (nTries++ < MAX_RETRIES) )
	{
		dbgPrint("Can't load module %s\n", Media::GetFileName(moduleName));
	}

	ASSERTS(nResult >= 0, "Error loading IOP module.  Make sure your file server root is set right");

	return (nResult >= 0);
}

/*************************************************************************/
// Setup assert support
/*************************************************************************/
void PS2App::SetupAssertSupport(
const char *executable)
{
	// if this is not the case, I sure hope that
	// assert strings are loaded with the elf..
	// since otherwise, everything would be broken...
#ifndef DISABLE_ASSERTS
	if (executable[1] != ':')
		Assert::SetExecutableName(executable);
	else
	{
		char buffer[200];
		strcpy(buffer, "host0:");
		strcat(buffer, executable);
		Assert::SetExecutableName(buffer);
	}

	// if you want onscreen asserts:
	Assert::SetHandler(&Assert::OnScreen);
#endif

#ifdef ASSERT_SUPPORT_ONSCREEN
	sceDevConsInit();
#endif
}

/*************************************************************************/
/*************************************************************************/
void PS2App::Boot()
{
	// Set the driver
	if (!g_RemoteIPAddress)
	{
#ifdef CDROM
		MediaHAL::SetDriverName("cdrom0:\\");
#else
		MediaHAL::SetDriverName("host0:");
#endif
	}

	// Replace the IOP image?
	bool rebootedIOP = BootIOP();

	// Boot multistream
	bool warmBoot = (g_RemoteIPAddress != NULL);
	MultiStream::Boot(GameApp::Application()->IsDVD(), warmBoot);

	// load IOP images
	if (!g_RemoteIPAddress)
	{
		//the MCMAN and MCSERV needed for memory card
		LoadModule("IRX\\MCMAN.IRX");
		LoadModule("IRX\\MCSERV.IRX");
		LoadModule("IRX\\PADMAN.IRX");
	}

	// Check the ROM
	CheckROM();

#if defined(PS2) && defined(SN)
	// initialize vu0 imemory...
	Math::UploadVU0Microcode();

	// Initialize SN DLL system
	snInitDllSystem(NULL);

#if 0
	static u128 profBuffer[2048];
	LoadModule("IRX\\SNPROFIL.IRX");
	snProfInit(_10KHZ, &profBuffer[0], sizeof(profBuffer));
#endif

#endif
}

/*************************************************************************/
	// Replace the IOP image
/*************************************************************************/
bool PS2App::BootIOP()
{
	// --- DAS, dont load the iop image if debugging CDROM under ProView
	// Are we going to reboot the IOP
#ifndef CONSUMER_BUILD
#if defined(JJSFIX)
	if (snIsProView())
		return false;
#endif
	if (g_RemoteIPAddress || IOPBOOTIMAGE == NULL)
		return false;
#endif

	// Dirt
	sceSifInitRpc(0);

//#define PS2TUNER
#ifdef PS2TUNER
	return false;
#endif

#ifdef CDROM
	// Wait for a proper media type
	sceCdInit(SCECdINIT);
	if (GameApp::Application()->IsDVD())
		sceCdMmode(SCECdDVD);
	else
		sceCdMmode(SCECdCD);
	sceCdDiskReady(0);
#endif

	// Create the boot image replacement
	char bootImage[64];
#ifdef CDROM
	sprintf(bootImage, "%s%s", MediaHAL::GetDriverName(), IOPBOOTIMAGE);
#else
	sprintf(bootImage, "%s./%s", MediaHAL::GetDriverName(), IOPBOOTIMAGE);
#endif

	// Do the reboot (this is going to hang like a mofo if the file isn't there)

	if (g_NoBootIOP == 0)
	{
		scePrintf("Rebooting with IOP image...%s\n", bootImage);
		while(!sceSifRebootIop(bootImage));
		while(!sceSifSyncIop());
	}

	return true;
}

/*************************************************************************/
/*************************************************************************/
void PS2App::CheckBootParams(const char* romversion)
{
#ifndef CONSUMER_BUILD
#ifdef SN
	// this code verifies that everyone boots with 0x1 on the EE if they are
	// running on a T10K.
	// THIS CODE IS NOT TRC COMPIANT!!! DO NOT PUT IT IN A CONSUMER BUILD!
	// YOU WILL BE HUNG OUT THE WINDOW BY YOUR LITTLE TOE NAILS IF YOU DO!
	unsigned int* pnEEBootParams = snGetEEBootParams();
	if(pnEEBootParams)
	{
		ASSERTS((pnEEBootParams[EEPARAM_OFFSET] & EEPARAM_MEM_CONTROLLER_FIX_MASK) == EEPARAM_MEM_CONTROLLER_FIX_MASK,
			"EE Memory Controller Workaround *NOT* enabled. You must reboot with EE Boot param 0x1 enabled");
		if((pnEEBootParams[EEPARAM_OFFSET] & EEPARAM_MEM_CONTROLLER_FIX_MASK) == 0)
			LOCKUP();
		else
			dbgPrint("EE Memory Controller Workaround is enabled.  Your ok :)\n");
	}
	else
	{
		ASSERTS(pnEEBootParams, "Failed to get EE boot parameters!");
	}

#endif
#endif
}

/*************************************************************************/
/*************************************************************************/
void PS2App::CheckROM()
{
#ifndef CONSUMER_BUILD
	// this is to verify that everyone has the right flash on their devtool...
	// THIS CODE IS NOT TRC COMPIANT!!! DO NOT PUT IT IN A CONSUMER BUILD!
	// YOU WILL BE HUNG OUT THE WINDOW BY YOUR LITTLE TOE NAILS IF YOU DO!

	// it has been pointed out to me that this code wont work if the versions
	// dont match.  the returned strings should be empty or we'll get failure
	// codes in that case.
	int handle = sceOpen("rom0:ROMVER", SCE_RDONLY);
	ASSERT(handle >= 0); // versions dont match.
	if(handle >= 0);
	{
		char buffer[6];
		sceRead(handle, buffer, 6);
		sceClose(handle);
		printf("\nROMVER: %6.6s\n", buffer);

		if(buffer[4] == 'T')
		{
			printf("Running on a Tool! Checking flash\n");
			handle = sceOpen("rom0:EXTINFO", SCE_RDONLY);
			char buffer[53];
			ASSERT(handle >= 0); // versions dont match.
			if(handle >= 0)
			{
				const char* iopimg = IOPBOOTIMAGE;
				sceRead(handle, buffer, 53);
				sceClose(handle);
				printf("Flash = %3.3s\nLibs = %3.3s\n", &buffer[50], &iopimg[17]);
				/*if(buffer[50] != iopimg[5] ||
					buffer[51] != iopimg[6] ||
					buffer[52] != iopimg[7])
				{
					ASSERTS(false,
							  "It appears that your using the wrong flash on this tool."
							  " Please consider reflashing the tool!\n");
				}*/
				CheckBootParams(buffer);
			}
		}
		else if(buffer[5] == 'C')
		{
			printf("Running on a Consumer system!\n");
		}
		else if(buffer[5] == 'D')
		{
			if(g_RemoteIPAddress)
				dbgPrint("Running on a Test system Via ATMON to PC @ %s!\n", g_RemoteIPAddress);
			else
				conPuts("Running on a Test system\n");
		}
	}
#endif
}

/*************************************************************************/
// Setup the network adapter
/*************************************************************************/
void PS2App::SetupNetworkAdapter()
{
	// Load dev9 and setup a power-off thread
	int result;
   LoadStartModule("IRX/DEV9.irx", 0, NULL, &result);
	if ( result == RESIDENT_END )
		SetNetworkAdapter( true );

	PreparePowerOff();
}

/*************************************************************************/
// Build the command line (you delete)
/*************************************************************************/
char *PS2App::BuildCommandLine(
int argc,
char *argv[])
{
	// How big is the commandline?
	int i, len = 0;
	for (i = 1; i < argc; i++)
		len += strlen(argv[i]) + 1;

#ifndef CONSUMER_BUILD
	// Print out parameters
	for (i = 0; i < argc; i++)
		dbgPrint("arg %d = %s\n", i, argv[i]);
#endif

	// Make a command line
	char *commandLine = new char[len + 1];
	commandLine[0] = '\0';
	for (i = 1; i < argc; i++)
	{
		strcat(commandLine, argv[i]);
		if (i != argc - 1)
			strcat(commandLine, " ");
	}

#ifndef CONSUMER_BUILD
	dbgPrint("Command line: %s\n", commandLine);
#endif

	return commandLine;
}

/*************************************************************************/
// To support ATMON & ATMON, we need to see if we have a atfile: based argv[0], if so
// we need to save it off for MediaHAL:: to use...
// this needs to be done before modules are loaded
/*************************************************************************/
const char *PS2App::SetupATMon(
int argc,
char *argv[])
{
#ifdef ATMONSUPPORT
	if((argc >= 1) && (strnicmp(argv[0], "atfile:", 7) == 0) && (strchr(argv[0], ',') != NULL))
	{
		// note that this is destructive to argv[0]... thats ok though because we dont use it after here..
		char *ptr = argv[0];
		while(*ptr++ != ',');
		*ptr = '\0';

		MediaHAL::SetDriverName(argv[0]);

		ptr = argv[0];
		while(*ptr++ != ':');
		const char *remoteAddress = ptr;

		while(*ptr >= '0' && *ptr <= '9') ptr++;
		ptr++;
		while(*ptr >= '0' && *ptr <= '9') ptr++;
		ptr++;
		while(*ptr >= '0' && *ptr <= '9') ptr++;
		ptr++;
		while(*ptr >= '0' && *ptr <= '9') ptr++;
		*ptr = '\0';

		ThreadParam params;
		params.entry = ATMonPowerOffProcess;
		params.stack = m_atMonPowerOffStack;
		params.stackSize = 4096;
		params.gpReg = &_gp;
		params.initPriority = 0;
		params.option = 0;

		m_atMonPowerOffThreadID = CreateThread(&params);
		ASSERT(m_atMonPowerOffThreadID != -1);
		m_mainThreadID = GetThreadId();
		dbgPrint("power off thread id = %d\n", m_atMonPowerOffThreadID);
		StartThread(m_atMonPowerOffThreadID, NULL); // this thread sleeps immediately
		scePowerOffHandler(ATMonPowerOffHandler, NULL);

		SetNetworkAdapter( true );
		return remoteAddress;
	}
#endif

	// No ATMON
	return NULL;
}

/*************************************************************************/
/*************************************************************************/
void PS2App::CloseATMon()
{
#ifdef ATMONSUPPORT
	if (g_RemoteIPAddress)
	{
		WakeupThread(m_atMonPowerOffThreadID);
		SleepThread();
	}
#endif
}

#ifdef ATMONSUPPORT
/*************************************************************************/
/*************************************************************************/
void PS2App::ATMonPowerOffProcess(void* data)
{
	conPuts("Power Off thread ready!\n");

	// fall asleep until someone wakes us up...
	SleepThread();

	conPuts("Power off thread activated!\n");

	// kill ATmon handles...
	conPuts("Shutting down ATMON File handle\n");
	if(g_remoteFileHandle >= 0)
		sceClose(g_remoteFileHandle);
	conPuts("Shutting down ATMON TTY handle\n");
	if(g_remoteTTYHandle >= 0)
		sceClose(g_remoteTTYHandle);

	// reset ps2...
	LoadExecPS2("cdrom0:\\ATMON.ELF;1", 0, NULL);
}
#endif

#ifdef ATMONSUPPORT
/*************************************************************************/
/*************************************************************************/
void PS2App::ATMonPowerOffHandler(void* data)
{
	// hehe... just start up the thread that we all love...
	iWakeupThread(m_atMonPowerOffThreadID);
	iSuspendThread(m_mainThreadID);
}
#endif

/*************************************************************************/
/*************************************************************************/
void PS2App::PowerOffThread(void *arg)
{
	int sid = (int)arg;
	int stat;
	while(1)
	{
		WaitSema(sid);
		scePrintf("power off request has come.\n");

		// dev9 power off, need to power off PS2
		while(sceDevctl("dev9x:", DDIOC_OFF, NULL, 0, NULL, 0) < 0);

		// PS2 power off
		while(!sceCdPowerOff(&stat) || stat);
	}
}

/*************************************************************************/
/*************************************************************************/
void PS2App::PowerOffHandler(void *arg)
{
    int sid = (int)arg;
    iSignalSema(sid);
}

/*************************************************************************/
/*************************************************************************/
void PS2App::PreparePowerOff()
{
	if (g_RemoteIPAddress)
		return;

    struct ThreadParam tparam;
    struct SemaParam   sparam;

    sparam.initCount = 0;
    sparam.maxCount  = 1;
    sparam.option    = 0;
    m_sid = CreateSema(&sparam);

    tparam.stackSize = POWEROFF_STACKSZ;
    tparam.gpReg = &_gp;
    tparam.entry = PowerOffThread;
    tparam.stack = (void *)m_powerOffStack;
    tparam.initPriority = 1;
    m_tid = CreateThread(&tparam);
    StartThread(m_tid, (void *)m_sid);

    sceCdPOffCallback(PowerOffHandler, (void *)m_sid);
}

/*************************************************************************/
/*************************************************************************/
void PS2App::UndoPowerOff()
{
	if (g_RemoteIPAddress)
		return;

	// sanity check
	if (sceSifSearchModuleByName("dev9") >= 0)
	{
		// remove poweroff handler, thread, and semaphore
		sceCdPOffCallback( NULL, NULL );

		if (m_tid != -1)
		{
			TerminateThread( m_tid );
			DeleteThread( m_tid );
		}
		if (m_sid != -1)
			DeleteSema( m_sid );

		// close any and all open files
		sceDevctl("pfs:", PDIOC_CLOSEALL, NULL, 0, NULL, 0);

		// turn off the expansion bus
		while (sceDevctl("dev9x:", DDIOC_OFF, NULL, 0, NULL, 0) < 0) {};
	}
}

/*************************************************************************/
/*************************************************************************/
void SendToRemoteTerminal(const char* string)
{
	if (g_RemoteIPAddress && g_remoteTTYHandle < 0)
	{
		char buffer[256];
		sprintf(buffer, "atstream:%s!9124", g_RemoteIPAddress);
		g_remoteTTYHandle = sceOpen(buffer, SCE_RDWR, 0);
	}

	if (g_remoteTTYHandle >= 0)
		sceWrite(g_remoteTTYHandle, string, strlen(string));
#if !defined(CONSUMER_BUILD)
	else
		printf("%s", string);
#endif
}

/*************************************************************************/
/*************************************************************************/
int AttachToRemoteFileServer()
{
#ifdef ATMONSUPPORT
	if (g_RemoteIPAddress)
	{
		while (g_remoteFileHandle < 0)
		{
			char buffer[256];
			sprintf(buffer, "atstream:%s!9123", g_RemoteIPAddress);
			g_remoteFileHandle = sceOpen(buffer, SCE_RDWR, 0);
		}
	}

	if (g_remoteFileHandle >= 0)
		return g_remoteFileHandle;
	else
#endif
		return -1;
}

/*************************************************************************/
/*************************************************************************/
void SuspendGameIOPUse()
{
	// Need to suspend our IOP processing
	if (g_audioPtr)
		g_audio.Pause();
	MultiStream::Suspend();
}

/*************************************************************************/
/*************************************************************************/
void ResumeGameIOPUse()
{
	PS2App::m_iopResumePending = true;
}

/*************************************************************************/
/*************************************************************************/
void ProcessIOPResume( bool force )
{
	if( PS2App::m_iopResumePending || force )
	{
#ifdef CDROM
		if( !force )
		{
			sceCdSync(0x10);						// Wait for completion of any other CD commands..BLOCKING
			int temp = sceCdGetDiskType();
			if( temp == SCECdNODISC )			// No-Disc, Tray opened
			{
				// we cannot resume the iop until the tray is closed
				return;
			}
		}
#endif
		// Need to resume our IOP processing
		while(MultiStream::Suspended())
		{

			MultiStream::Resume();
			if (g_audioPtr)
				g_audio.Unpause();
		}
		PS2App::m_iopResumePending = false;
	}
}

/*************************************************************************/
/*************************************************************************/
//void PS2App::DetermineRefreshRate()
//{
//	if (g_wantPAL)
//	{
//		gameApp.SetupInput();
//
//		bool pal = g_wantPAL;
//
//		PS2DisplaySimple display;
//		display.Init(640, pal ? 512 : 448, pal);
//		BootLogo::DisplayImage(&display, hertzimage);
//
//		while ( 1 )
//		{
//			g_input.BeginInput();
//			ControllerInputDriver* controller = g_input.GetController(0);
//			if (controller->ButtonPressed(CB_X))
//			{
//				pal = true;
//				break;
//			}
//			else if (controller->ButtonPressed(CB_TRI))
//			{
//				pal = false;
//				break;
//			}
//			g_input.DoneInput();
//			display.Flip();
//		}
//
//		// set pal flag as appropriate
//		g_wantPAL = pal;
//	}
//}
