#include "ExportPch.h"
#include <assert.h>
#include "VIFControl.h"

VIFControl::VIFControl( DMAControl &defControl ) :
	d_WAlign( 4), 
	d_UNPACKinProgress( false),
	d_Control( defControl), 
	d_str( defControl.stream( )) 
{


}

void VIFControl::startVIFChunk( void)
{ 
	d_Control.setStreamAligner( &d_DMAAlign);
}

void VIFControl::endVIFChunk( void)
{ 
	if( d_UNPACKinProgress)
	{
		d_str << d_WAlign; 
		d_UNPACKinProgress = false;
	}
}

void VIFControl::vifSTCYCL( VCNum wl, VCNum cl, VCFlag intFlag )
{
	d_str << createSTCYCL( wl, cl, intFlag );
}

void VIFControl::vifMSCAL( VCAddr vuaddr, VCFlag intFlag )
{
	d_str << createMSCAL( vuaddr, intFlag );
}

void VIFControl::vifMSCALF( VCAddr vuaddr, VCFlag intFlag )
{
	d_str << createMSCALF( vuaddr, intFlag );
}

void VIFControl::vifMSCNT( VCFlag intFlag )
{
	d_str << createMSCNT( intFlag );
}

void VIFControl::vifNOP( VCFlag intFlag )
{
	d_str << createNOP( intFlag );
}

void VIFControl::vifUNPACK( VCAddr vuaddr, VIFUnpackCode cmd, VCNum num, VCFlag useTOPS, VCFlag mask, VCFlag isUnsigned, VCFlag intFlag )
{
	d_str << createUNPACK( vuaddr, cmd, num, useTOPS, mask, isUnsigned, intFlag  );
}

void VIFControl::startUNPACK( void )
{
	assert( d_UNPACKinProgress == false);

	d_WAlign.mark( d_str);
	d_UNPACKinProgress = true;
}

void VIFControl::startUNPACK( VCAddr vuaddr, VIFUnpackCode cmd, VCNum num, VCFlag useTOPS, VCFlag mask, VCFlag isUnsigned, VCFlag intFlag )
{
	if( d_UNPACKinProgress)
		d_str << d_WAlign;

	d_str << createUNPACK( vuaddr, cmd, num, useTOPS, mask, isUnsigned, intFlag  );

	d_WAlign.mark( d_str); 
	d_UNPACKinProgress = true;
}

void VIFControl::endUNPACK( void )
{
	assert( d_UNPACKinProgress);

	// align the UNPACK data to 32bits
	d_str << d_WAlign;

	d_UNPACKinProgress = false;

}

void VIFControl::vifFLUSH( VCFlag intFlag )
{
	d_str << createFLUSH( intFlag );
}

void VIFControl::vifFLUSHE( VCFlag intFlag )
{
	d_str << createFLUSHE( intFlag );
}

void VIFControl::vifFLUSHA( VCFlag intFlag )
{
	d_str << createFLUSHA( intFlag );
}

void VIFControl::vifDIRECT( VCNum size, VCFlag flush, VCFlag intFlag )
{
	d_str << createDIRECT( size, flush, intFlag );
}

void VIFControl::vifOFFSET( VCNum offset, VCFlag intFlag )
{
	d_str << createOFFSET( offset, intFlag );
}

void VIFControl::vifBASE( VCNum base, VCFlag intFlag )
{
	d_str << createBASE( base, intFlag );
}

void VIFControl::vifITOP( VCAddr vuaddr, VCFlag intFlag )
{
	d_str << createITOP( vuaddr, intFlag );
}

void VIFControl::vifMODE( VCNum mode, VCFlag intFlag )
{
	d_str << createMODE( mode, intFlag );
}