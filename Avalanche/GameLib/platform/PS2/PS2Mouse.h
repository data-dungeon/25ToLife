// ----------------------------------------------------------------------------
// File			-> PS2Mouse.h
// Created		-> April 22, 2004
// Author		-> Dwain Skinner
// Description	-> Platform dependent mouse support for PS2
// ----------------------------------------------------------------------------

#include "platform/CMouse.h"
#include <liblgkbm.h>

class CPlatformMouse : public CMouseDriver
{

public:
	CPlatformMouse										( );
	~CPlatformMouse									( );

	bool initialize									( void );
	bool beginInput									( void );
	bool endInput										( void );
	int getMouseX										( void );
	int getMouseY										( void );
	int getMouseButton								( void );
	int getMouseWheel									( void );
	bool isMouseActive								( void ) { return( m_mouseHandle != LGKBM_INVALID_HANDLE ); }
	bool isMouseLoaded								( void ) { return( m_kmLive ); }

private:
	struct mouseData 
	{
		int                 device_handle;
		int                 errorCode;
		int                 fHasWheel;
		int                 nButtons;
		lgMouseDeviceDesc   device_desc;
		lgMouseData         mData;
		char                deviceLocation[50];
	};

	void updateMouseDevice							( void );
	void getMouseData									( void );
	void debugDisplayMouseData						( mouseData *mdata, int mouse_no );


	int													m_mouseX;
	int													m_mouseY;
	int													m_mouseButton;
	int													m_mouseWheel;
	int													m_mouseHint;
	int													m_mouseHandle;
	int													m_keyboardHint;
	int													m_keyboardHandle;
	bool													m_mouseConnected;
	bool													m_keyboardConnected;
	bool													m_kmLive;
	mouseData											m_mouseData;
};