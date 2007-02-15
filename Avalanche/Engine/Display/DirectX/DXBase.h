#ifndef __DX_BASE_H
#define __DX_BASE_H

#include "CSLinkList.h"
#include "DXDeviceObjectClass.h"

class DXBaseC
{
	CSLinkList< DXDeviceObjectClass * >		RestoreList;

public:
	DXBaseC( );
	~DXBaseC( );

	IDirect3DDevice8 *						p_DXDevice;
	D3DCAPS8										DeviceCaps;

	void						initDXDevice( IDirect3DDevice8 * pDXDevice );
	void						releaseDXDevice( void ); 

	void						addDOCallback( DXDeviceObjectClass * restore );
	void						removeDOCallback( DXDeviceObjectClass * restore );

	void						releaseDeviceObjects( void );
	void						restoreDeviceObjects( void );
} ;

extern DXBaseC				DXBase;

#endif