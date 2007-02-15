#include "Display/DisplayPCH.h"

#include "DXBase.h"

DXBaseC				DXBase;

DXBaseC::DXBaseC( ) :
	RestoreList( ),
	p_DXDevice( 0 )
{

}

DXBaseC::~DXBaseC( )
{
	assert( RestoreList.count( ) == 0 );
}

void DXBaseC::initDXDevice( IDirect3DDevice8 * pDXDevice )
{
	assert( pDXDevice );

	p_DXDevice = pDXDevice;

	HRESULT result = p_DXDevice->GetDeviceCaps( & DeviceCaps );
	assert( result == D3D_OK );
}

void DXBaseC::releaseDXDevice( void )
{
	p_DXDevice = 0;
}

void DXBaseC::addDOCallback( DXDeviceObjectClass * doClass )
{
	assert( doClass );

	RestoreList.add( doClass );
}

void DXBaseC::removeDOCallback( DXDeviceObjectClass * doClass )
{
	assert( doClass );
	RestoreList.remove( doClass );
}

void DXBaseC::releaseDeviceObjects( void )
{
	for( unsigned int i = 0; i < RestoreList.count( ); i++ )
	{
		if( RestoreList[i] )
			RestoreList[i]->releaseDeviceObjects( );
	}
}

void DXBaseC::restoreDeviceObjects( void )
{
	for( unsigned int i = 0; i < RestoreList.count( ); i++ )
		if( RestoreList[i] )
			RestoreList[i]->restoreDeviceObjects( );
}
