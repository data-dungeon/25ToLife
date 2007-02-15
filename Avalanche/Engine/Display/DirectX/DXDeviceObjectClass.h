#ifndef __DX_DEVICE_OBJECT_CLASS
#define __DX_DEVICE_OBJECT_CLASS

class	DXDeviceObjectClass
{

public:
	virtual void	releaseDeviceObjects( void ) = 0;
	virtual void	restoreDeviceObjects( void ) = 0;

} ;

#endif
