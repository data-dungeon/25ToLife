// ----------------------------------------------------------------------------
// File			-> CMouse.h
// Created		-> April 22, 2004
// Author		-> Dwain Skinner
// Description	-> Platform independent mouse support
// ----------------------------------------------------------------------------

#ifndef __CMOUSE__H
#define __CMOUSE__H

class CMouseDriver
{

public:
	CMouseDriver										( ) { setMouseInstance( this ); }
	virtual ~CMouseDriver							( ) {};

	virtual bool initialize							( void ){ return false; }
	virtual bool beginInput							( void ){ return false; }
	virtual bool endInput							( void ){ return false; }
	virtual int getMouseX							( void ){ return 0; }
	virtual int getMouseY							( void ){ return 0; }
	virtual int getMouseButton						( void ){ return 0; }
	virtual int getMouseWheel						( void ){ return 0; }
	virtual bool isMouseLoaded						( void ){ return false; }
	virtual bool isMouseActive						( void ){ return false; }

	CMouseDriver *getInstance						( void ){ return( m_pMouseInstance ); }
	void setMouseInstance							( CMouseDriver *pMouseInstance ) { m_pMouseInstance = pMouseInstance; }

private:
	CMouseDriver										*m_pMouseInstance;
};

extern CMouseDriver g_CMouse;

#ifndef __CMOUSE__CPP
#define g_CMouse ( ( CMouseDriver& )( *g_CMouse.getInstance( ) ) )
#endif

#endif
