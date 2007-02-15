#include "ExportPch.h"
#include "ByteAligner.h"

unsigned int ByteAligner::offset( streampos pos)
{
unsigned int 		uiBytes;

	uiBytes = ( pos - marker( )) % d_Alignment;

	return (uiBytes == 0) ? 0 : d_Alignment - uiBytes;
}

void ByteAligner::align( ostream &str)
{
unsigned int		writeBytes;

	if( (&str != p_str)  ||  (d_Alignment < 2))
		return;

	writeBytes = offset( str.tellp( ));

	while( writeBytes--)
		str << d_AlignmentData;
}

