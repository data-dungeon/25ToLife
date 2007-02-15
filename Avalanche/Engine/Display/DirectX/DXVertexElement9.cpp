#include <Assert.h>
#include "DXVertexElement9.h"

unsigned int numDWORDs( D3DDECLTYPE type )
{
	assert( type < D3DDECLTYPE_UNUSED );

	switch( type )
	{
	case D3DDECLTYPE_FLOAT1:
	case D3DDECLTYPE_D3DCOLOR:
	case D3DDECLTYPE_UBYTE4:
	case D3DDECLTYPE_UBYTE4N:
	case D3DDECLTYPE_SHORT2:
	case D3DDECLTYPE_SHORT2N:
	case D3DDECLTYPE_USHORT2N:
	case D3DDECLTYPE_UDEC3:
	case D3DDECLTYPE_DEC3N:
	case D3DDECLTYPE_FLOAT16_2:
		return 1;
	case D3DDECLTYPE_FLOAT2:
	case D3DDECLTYPE_SHORT4:
	case D3DDECLTYPE_SHORT4N:
	case D3DDECLTYPE_USHORT4N:
	case D3DDECLTYPE_FLOAT16_4:
		return 2;
	case D3DDECLTYPE_FLOAT3:
		return 3;
	case D3DDECLTYPE_FLOAT4:
		return 4;
	default:
	case D3DDECLTYPE_UNUSED:
		assert( 0 );
		return 0;
	}

}

