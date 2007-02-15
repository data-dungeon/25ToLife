// ----------------------------------------------------------------------------
// File			-> PS2Mouse.cpp
// Created		-> April 22, 2004
// Author		-> Dwain Skinner
// Description	-> Platform dependent mouse support for PS2
// ----------------------------------------------------------------------------

#include "platform/PlatformPCH.h"
#include "platform/PS2/PS2Mouse.h"
#include "platform/PS2/util.h"

#define MAX_KBDS                      1
#define MAX_MICE                      1

static void DebugDisplayMouseSryPacket( lgMouseSummaryPacket *mpkt, int fWheel);
static void DebugDisplayMousePacket( lgMousePacket *mpkt, int fWheel);
static bool initialized = false;

// ----------------------------------------------------------------------------
CPlatformMouse::CPlatformMouse( void )
{
	m_mouseX					= 0;
	m_mouseY					= 0;
	m_kmLive					= false;
	m_keyboardConnected	= false;
	m_mouseConnected		= false;
	m_keyboardHint			= LGKBM_HINT_ENUMNEEDED;
	m_mouseHint				= LGKBM_HINT_ENUMNEEDED;
	m_keyboardHandle		= LGKBM_INVALID_HANDLE;
	m_mouseHandle			= LGKBM_INVALID_HANDLE;
	g_CMouse.setMouseInstance( this );
}

// ----------------------------------------------------------------------------
CPlatformMouse::~CPlatformMouse( void )
{
}

// ----------------------------------------------------------------------------
bool CPlatformMouse::initialize( void	)
{
	bool success = false;

	if( initialized ) return true;

//	if( !CPlatformKeyboard::isKeyboardActive( ) )
	if( true )
	{
		initialized = true;
		PS2App *app = PS2App::IAm();
		char usbd_param[] = "dev=11\0ed=32\0gtd=64\0itd=64\0ioreq=128\0hub=2\0";
		char  lgkbm_param[100];               // cmd line to be passed to lgKdmdrv
		int   size_param = 0;
		char  *thpri_param = "thpri=40";      // Initial Thread Priority is 40
		char  buf[10];

		// --- load the usb drivers
		if( ( success = app->LoadModule( "IRX\\USBD.IRX", sizeof( usbd_param ), usbd_param ) ) )
		{

			// Prepare command line parameter list for mouse+keyboard driver.
			myitoa(MAX_KBDS, buf);
			size_param += len_strcpy(lgkbm_param+size_param, LGKBM_PARAM_MAX_NUM_KBDS);
			size_param += len_strcpy(lgkbm_param+size_param,"=");
			size_param += len_strcpy(lgkbm_param+size_param, buf);
			size_param++;      // skip past the null terminator

			myitoa(MAX_MICE, buf);
			size_param += len_strcpy(lgkbm_param+size_param, LGKBM_PARAM_MAX_NUM_MICE);
			size_param += len_strcpy(lgkbm_param+size_param,"=");
			size_param += len_strcpy(lgkbm_param+size_param, buf);
			size_param++;      // skip past the null terminator

			size_param += len_strcpy(lgkbm_param+size_param, thpri_param);
			size_param++;      // increment the length to include null terminator

			// --- load the logitech keyboard and mouse driver
			success = app->LoadModule( "IRX\\LGKBM.IRX", size_param,  lgkbm_param );
			if( success )
			{
				success = ( lgKbMInit(MAX_KBDS, MAX_MICE, APPLICATION_THREAD_PRIORITY - 1, NULL) == LGKBM_SUCCESS );
			}
		}
	}
	else
	{
		initialized = true;
		success = true;
	}
	m_kmLive = success;
	return( success );
}

// ----------------------------------------------------------------------------
bool CPlatformMouse::beginInput( void	)
{
	// --- lets test for our devices first
	if( m_kmLive )
	{
		// --- see if we need to find the mouse device
		lgMouseEnumHint( &m_mouseHint );
		if( m_mouseHint == LGKBM_HINT_ENUMNEEDED )
		{
			// --- we've either plugged or unplugged the mouse
			updateMouseDevice( );
		}

		// --- make sure we have a valid mouse
		if( m_mouseHandle != LGKBM_INVALID_HANDLE )
		{
			// --- read the mouse
			getMouseData( );
			return( true );
		}
	}
	return( false );
}

// ----------------------------------------------------------------------------
bool CPlatformMouse::endInput( void	)
{
	return( false );
}

// ----------------------------------------------------------------------------
int CPlatformMouse::getMouseX( void	)
{
	return( m_mouseX );
}

// ----------------------------------------------------------------------------
int CPlatformMouse::getMouseY( void	)
{
	return( m_mouseY );
}

// ----------------------------------------------------------------------------
int CPlatformMouse::getMouseButton( void	)
{
	return( m_mouseButton );
}

// ----------------------------------------------------------------------------
int CPlatformMouse::getMouseWheel( void	)
{
	return( m_mouseWheel );
}

// ----------------------------------------------------------------------------
void CPlatformMouse::updateMouseDevice( void ) 
{
	int                device_handle;
	int                rc;
	lgMouseDeviceDesc *mouse_desc;
	char              *pDeviceLocation;

	// --- enumerate the devices
	rc = lgMouseEnumerate( 0, &m_mouseData.device_desc );
	if( rc != LGKBM_SUCCESS ) 
	{
		// --- we didn't find any devices
		m_mouseHandle = LGKBM_INVALID_HANDLE;
		return ;
	}


	pDeviceLocation = m_mouseData.deviceLocation;
	mouse_desc      = &m_mouseData.device_desc;
	GetDeviceLocationString( pDeviceLocation, mouse_desc->device_location );

	// --- Enumeration Succeeded. Lets open the device.
	rc = lgMouseOpen( 0, &device_handle );
	if( rc != LGKBM_SUCCESS ) 
	{
		ASSERTS( false, "mouse open failed" );
		return ;
	}

	m_mouseData.device_handle = device_handle;
	m_mouseData.fHasWheel     = mouse_desc->fHasWheel;
	m_mouseData.nButtons      = mouse_desc->nButtons;
	printf("Mouse #1 opened. device handle = %d ",	0, device_handle);
	printf("nButtons = %d, fHasWheel = %d. flag = 0x%x\n", m_mouseData.nButtons ,	m_mouseData.fHasWheel, mouse_desc->flag ); 
	m_mouseHandle	= device_handle;
	m_mouseHint		= LGKBM_HINT_NOENUMNEEDED;
	return;
}

// ----------------------------------------------------------------------------
void CPlatformMouse::getMouseData( void )
{
	int rc = LGKBM_SUCCESS;

	// --- we know we have a valid mouse so lets get its data
	rc = lgMouseRead( m_mouseHandle, &m_mouseData.mData );

	// --- error?
	if( rc ) 
	{     
		if( rc == LGKBM_ERR_DEVICE_LOST ) 
		{
			printf( "Mouse device handle #%d lost closing the device\n", m_mouseHandle );
			return;
		}

		if( rc != m_mouseData.errorCode ) 
		{ 
			printf( "Mouse error on device handle #%d, error #%d\n", m_mouseHandle, get_error( rc ) );
			m_mouseData.errorCode = rc;
			return;
		}
	}
	else 
	{
		m_mouseData.errorCode = 0;
	}

	// --- time to get at our data, use the summary packet for now
	m_mouseX			= m_mouseData.mData.summary_pkt.X;
	m_mouseY			= m_mouseData.mData.summary_pkt.Y;
	for( int i = 0; i<m_mouseData.mData.num_pkts; ++i ) 
	{
		if( m_mouseData.mData.mouse_pkts[ i ].button )
		{
			m_mouseButton = m_mouseData.mData.mouse_pkts[ i ].button;
		}
		else if( m_mouseButton )
		{
			m_mouseButton = 0;
		}
	}
	m_mouseWheel	= 0;
	if( m_mouseData.fHasWheel )
		m_mouseWheel	= m_mouseData.mData.summary_pkt.Wheel;

	// Display the mouse data on the debug terminal.
	//debugDisplayMouseData( &m_mouseData, 0 );
	return;
}








/*********************************************************************************
* DebugDisplayMouseSryPacket() : Helper function which prints the mouse summary *
*                                packet data on the debug terminal.             *
* ----------------------------------------------------------------------------- *
* Input        : mpkt      : Pointer to the mosue summary packet                *
*              : fWheel    : flag which indicates if the mouse has a wheel      *
* Output       : Nothing   :                                                    *
* Returns      : Nothing   :                                                    *
*********************************************************************************/
static void DebugDisplayMouseSryPacket(lgMouseSummaryPacket *mpkt, int fWheel)
{
	printf(" time_stamp = %10x X = %d, Y = %d, Button = 0x%x", 
		mpkt->time_stamp, mpkt->X, mpkt->Y, mpkt->button); 
	if(fWheel) {
		printf(" Wheel = %3d", mpkt->Wheel);
	}
	printf("\n");
	return;
}


/*********************************************************************************
* DebugDisplayMousePacket() : Helper function which prints the mouse data packet*
*                             on the debug terminal.                            *
* ----------------------------------------------------------------------------- *
* Input        : mpkt      : Pointer to the mosue data packet                   *
*              : fWheel    : flag which indicates if the mouse has a wheel      *
* Output       : Nothing   :                                                    *
* Returns      : Nothing   :                                                    *
*********************************************************************************/
static void DebugDisplayMousePacket(lgMousePacket *mpkt, int fWheel)
{
	printf(" time_stamp = %10x X = %d, Y = %d, Button = 0x%x", 
		mpkt->time_stamp, mpkt->X, mpkt->Y, mpkt->button); 
	if(fWheel) {
		printf(" Wheel = %3d", mpkt->Wheel);
	}
	printf("\n");
	return;
}


/*********************************************************************************
* DebugDisplayMouseData()  : This function iterates through all the mouse data  *
*                          : packets and calls DebugDisplayMousePacket to output*
*                          : the mouse data on the debug terminal               *
* ----------------------------------------------------------------------------- *
* Input        : mdata     : Pointer to the mouse data structure.               *
*              : mouse_no  : Mouse number                                       *
* Output       : Nothing   :                                                    *
* Returns      : Nothing   :                                                    *
*********************************************************************************/
void CPlatformMouse::debugDisplayMouseData( mouseData *mdata, int mouse_no )
{
	lgMouseData         *mouse_data   = &mdata->mData;
	int                 i;

	if(!mdata->mData.num_pkts)
		return;

	printf("Main:: Mouse[%d]: Summary Packet", mouse_no);
	DebugDisplayMouseSryPacket(&mouse_data->summary_pkt, mdata->fHasWheel);
	for(i=0; i<mouse_data->num_pkts; ++i) {
		printf("Main:: Mouse[%d]: Packet# %d     ", mouse_no, i);
		DebugDisplayMousePacket(&mouse_data->mouse_pkts[i], mdata->fHasWheel);
	}
	printf("\n");
	return;
}
