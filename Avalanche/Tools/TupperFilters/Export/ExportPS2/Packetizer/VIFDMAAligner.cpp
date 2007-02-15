#include "ExportPch.h"
#include "VIFDMAAligner.h"

void VIFDMAAligner::align( ostream &str)
{
unsigned int		writeBytes;

	if( &str != p_str)
		return;

	writeBytes = offset( str.tellp( ));

	while( writeBytes & 0x03)
	{
		str << (unsigned char) 0;
		writeBytes--;
	}

	while( writeBytes > 0)
	{
		str << createNOP( );
		writeBytes -= 4;
	}

}
