//*************************************************************************
// GCN application framework.
//*************************************************************************

/******************* includes ****************************/

#include "platform/PlatformPCH.h"
#include <dolphin/vm.h>

#ifdef DVDETH
#include <dolphin/dvdeth.h>
#endif

#include "Display/GCN/Shader.h"
#include "Layers/Timing.h"

//#define FPU_CHECK

union DualFloat_LongLong
{
	double f;
	long long   i;
};

u32 const GCNApp::sm_vmBase = 0x7E000000;

u32 const GCNApp::sm_vmVirtualSize = ( 10 * 1024 * 1024);

u32 const GCNApp::sm_vmPhysicalSize = ( ( 3 * 1024 * 1024) / 2);

#ifdef DVDETH
static void ConvertDotAddrToBinary( const char *pStr, u8 *pBinary);
#endif

//*************************************************************************
// Here it is, our very own main
//*************************************************************************
int main( int argc, char *argv[])
{
	register double control;

	GCNApp *app = GCNApp::IAm( );

	app->InitGCNOS( argc, argv);

#ifdef FPU_CHECK

	PPCMtmsr( PPCMfmsr( ) | MSR_FE0 | MSR_FE1);

	union DualFloat_LongLong d;

	#define FP_EX_ENABLE_MASK		0x0090
	
	/*
	 * copy out of the fpscr
	 */
	asm	{ mffs control }
	
	/*
	 * copy out of the register -- we need this in the union for conversion
	 */
	d.f = control; 
	
	/*
	 * turn on the bits specified by the mode
	 */
	d.i |=   FP_EX_ENABLE_MASK;  
	
	/*
	 * Put back into the control register.
	 * It needs to be in a register to copy into the FPSCR
	 */
	control = d.f; 

	// copy into the fpscr
	asm	{mtfsf 255, control	}
#endif //FPU_CHECK

	// How big is the commandline?
	int i, len = 0;

	for( i = 1; i < argc; i++)
		len += strlen( argv[i]) + 1;

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

	// Setup everything
#ifndef CONSUMER_BUILD
	dbgPrint("Command line: %s\n", commandLine);
#endif
	bool ret = app->Initialize( commandLine);
	ASSERT(ret);

	// get rid of the commandline
	delete [] commandLine;

	// Process the game
	while( app->KeepProcessing( ))
	{
		app->Process( );
	}

	// In a non-debug build we should never get here
#ifndef _DEBUG
	ASSERT(false);
#endif

	// Shutdown everything
	app->Terminate( );

	return(0);
}

//*************************************************************************
//*************************************************************************
GCNApp::GCNApp(void)
{
	quit = false;

	g_ShaderMgrPtr = new ShaderMgr;

	m_bMemoryCardInProgress = false;
}

//*?************************************************************************
// We are quitting! (does nothing on GCN)
//*************************************************************************
void GCNApp::Quit(void)
{
	quit = true;
}

//*************************************************************************
//*************************************************************************
void GCNApp::InitGCNOS( int argc, char *argv[])
{
	/*----------------------------------------------------------------*
	 *  Initialize OS, shouldnt be needed, but just in case           *
	 *----------------------------------------------------------------*/
	OSInit( );

	/*----------------------------------------------------------------*
	 *  Initialize DVD                                                *
	 *----------------------------------------------------------------*/
	DVDInit( );

	DVDSetAutoFatalMessaging( true);

	/*----------------------------------------------------------------*
	 *  Initialize VI                                                 *
	 *    After VIInit, screen is black, must call VISetBlack         *
	 *----------------------------------------------------------------*/
	VIInit( );

	/*----------------------------------------------------------------*
	 *  Initialize game PAD                                           *
	 *----------------------------------------------------------------*/
	PADInit( );

	CARDInit( );

	/*----------------------------------------------------------------*
	 *  Initialize virtual memory system for animation data           *
	 *----------------------------------------------------------------*/
	ARInit( NULL, 0);

	u32	aramBase = ARGetBaseAddress( );

#if __option(cats)
	m_aramBase = aramBase;
#else
	VMInit( sm_vmPhysicalSize, aramBase, sm_vmVirtualSize);

	m_aramBase = aramBase + sm_vmVirtualSize;
#endif

#ifdef DVDETH

	char	*pHost = NULL;

	char	*pLocal = NULL;

	char	*pMask = NULL;

	char	*pStr;

	for( int i = 1; i < argc; i++)
	{
		if( ( pStr = strstr( argv[i], "-host~")) != NULL)
		{
			pHost = pStr + strlen( "-host~");
		}
		else
		if( ( pStr = strstr( argv[i], "-local~")) != NULL)
		{
			pLocal = pStr + strlen( "-local~");
		}
		else
		if( ( pStr = strstr( argv[i], "-netmask~")) != NULL)
		{
			pMask = pStr + strlen( "-netmask~");
		}
	}

	ASSERT( pHost != NULL && pLocal != NULL && pMask != NULL);

	u8		hostBinary[4],
			localBinary[4],
			maskBinary[4];

	ConvertDotAddrToBinary( pHost, hostBinary);
	ConvertDotAddrToBinary( pLocal, localBinary);
	ConvertDotAddrToBinary( pMask, maskBinary);

	DVDEthInit( localBinary, maskBinary, localBinary);

	DVDLowInit( hostBinary, 9001);
#endif

	m_pGameThread = OSGetCurrentThread( );

	m_pResetThreadStack = new u8[RESET_THREAD_STACK_SIZE];

	OSCreateThread(	&m_ResetThread,
							ResetThread,
							this,
							m_pResetThreadStack + RESET_THREAD_STACK_SIZE,
							RESET_THREAD_STACK_SIZE,
							1,
							OS_THREAD_ATTR_DETACH);

	OSInitMessageQueue( &m_ResetRequestQueue, m_aResetRequestMessages, 2);

	OSResumeThread( &m_ResetThread);

#ifndef DISABLE_ASSERTS
	Assert::SetHandler( Assert::GCNAssertHandler);
#endif
}

void GCNApp::RequestReset( ts_RequestResetMsg *pResetRequest)
{
	OSSendMessage( &m_ResetRequestQueue, (OSMessage)pResetRequest, OS_MESSAGE_BLOCK);
}

void * GCNApp::ResetThread( void *pParam)
{
	
	GCNApp	*pThis = (GCNApp *)pParam;

	bool		bCurrState,
				bOldState = false,
				bResetQueued = false;

	int		bootUpWait = 500;

	u32		u32PrevTime = EngineTimer__Time( );

	u32		u32ControllerResetTime;

	const u16	controllerResetMask = ( PAD_BUTTON_START | PAD_BUTTON_X | PAD_BUTTON_B);

	while( 1)
	{
		VIWaitForRetrace( );

		OSMessage	msg;

		if( OSReceiveMessage( &pThis->m_ResetRequestQueue, &msg, OS_MESSAGE_NOBLOCK))
		{
			ts_RequestResetMsg	*pResetMsg = (ts_RequestResetMsg *)msg;

			pThis->HandleReset(	pResetMsg->resetType,
										pResetMsg->resetCode,
										pResetMsg->forceMenu);
		}

		u32	u32CurrTime = EngineTimer__Time( );
		
		u32	u32DeltaTime = u32CurrTime - u32PrevTime;

		u32PrevTime = u32CurrTime;
		
		PADRead( pThis->m_aPADStatus);

		if( ( pThis->m_aPADStatus[0].button & controllerResetMask) == controllerResetMask)
		{
			if( bootUpWait == 0)
			{
				u32ControllerResetTime += u32DeltaTime;
			}
		}
		else
		{
			Math::MoveToZero( &bootUpWait, u32DeltaTime);
			u32ControllerResetTime = 0;
		}

		t_Error diskError = MediaHAL::TranslateError( DVDGetDriveStatus( ), MED_ERR_NONE);

		if( diskError != MED_ERR_NONE)
		{
			for( int i = 0; i < PAD_MAX_CONTROLLERS; i++)
			{
				pThis->m_aPADStatus[i].stickX = 0;
				pThis->m_aPADStatus[i].stickY = 0;
				pThis->m_aPADStatus[i].substickX = 0;
				pThis->m_aPADStatus[i].substickY = 0;
				pThis->m_aPADStatus[i].button = 0;
				pThis->m_aPADStatus[i].triggerLeft = 0;
				pThis->m_aPADStatus[i].triggerRight = 0;
				pThis->m_aPADStatus[i].analogA = 0;
				pThis->m_aPADStatus[i].analogB = 0;
			}
		}

		bCurrState = OSGetResetButtonState( );
		if( !bResetQueued)
		{
			if( ( !bCurrState && bOldState) || u32ControllerResetTime >= 750)
			{
				if( pThis->IsMemoryCardInProgress( ))
				{
					bResetQueued = true;
				}
				else
				{
					if( DVDCheckDisk( ))
					{
						pThis->HandleReset( OS_RESET_RESTART, 0, false);
					}
					else
					{
						pThis->HandleReset( OS_RESET_HOTRESET, 0, false);
					}
				}
			}
		}
		else
		{
			if( !pThis->IsMemoryCardInProgress( ))
			{
				if( DVDCheckDisk( ))
				{
					pThis->HandleReset( OS_RESET_RESTART, 0, false);
				}
				else
				{
					pThis->HandleReset( OS_RESET_HOTRESET, 0, false);
				}
			}
		}
		bOldState = bCurrState;
	}
}

void GCNApp::HandleReset( int reset, u32 resetCode, BOOL forceMenu)
{

	OSSuspendThread( m_pGameThread);

// dont do the stop or clear because we'll try to do a DVDCancelStream
//	g_audio.jukebox.Stop( );
//	g_audio.jukebox.Clear( );

// dont do the stop or clear because we'll try to do a DVDCancel
//	g_audio.dialog.Stop( );
//	g_audio.dialog.Clear( );

	GXAbortFrame( );

//	g_audio.Pause( );

	VISetBlack( GX_TRUE);
	VIFlush( );

	VIWaitForRetrace( );

//	g_audio.Daemon( );

	if( g_inputPtr != NULL)
	{
		g_input.BeginInput( );
		
		for( uint i = 0; i < g_input.Controllers( ); i++)
		{
			ControllerInputDriver *controller = g_input.GetController( i);
			
			if( controller != NULL)
			{
				controller->ClearVibration( true);
			}
			
		}
		
		g_input.DoneInput( );
	}

	VIWaitForRetrace( );

	OSResetSystem( reset, resetCode, forceMenu);
}

static void ConvertDotAddrToBinary( const char *pStr, u8 *pBinary)
{
	pBinary[0] = (u8)atoi( pStr);

	pStr = strchr( pStr, '.');
	pStr++;
	pBinary[1] = (u8)atoi( pStr);

	pStr = strchr( pStr, '.');
	pStr++;
	pBinary[2] = (u8)atoi( pStr);

	pStr = strchr( pStr, '.');
	pStr++;
	pBinary[3] = (u8)atoi( pStr);
}

