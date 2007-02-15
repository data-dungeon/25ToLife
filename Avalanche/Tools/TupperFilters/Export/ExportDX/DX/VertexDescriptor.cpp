#include "ExportPch.h"

#include "ShaderRecords.h"
#include "VertexDescriptor.h"

bool operator < ( const D3DVERTEXELEMENT9 & e1, const D3DVERTEXELEMENT9 & e2)
{
	if( e1.Stream < e2.Stream )
		return true;
	
	if( e1.Offset < e2.Offset )
		return true; 

	if( e1.Type < e2.Type )
		return true;

	if( e1.Method < e2.Method )
		return true;

	if( e1.Usage < e2.Usage )
		return true;

	if( e1.UsageIndex < e2.UsageIndex )
		return true;
 
	return false;
}

unsigned int bytes( _D3DDECLTYPE type )
{
	unsigned int size[] =
	{
		4,				// D3DDECLTYPE_FLOAT1
		2 * 4,		// D3DDECLTYPE_FLOAT2
		3 * 4,		// D3DDECLTYPE_FLOAT3
		4 * 4,		// D3DDECLTYPE_FLOAT4
		4 * 1,		// D3DDECLTYPE_D3DCOLOR
		4 * 1,		// D3DDECLTYPE_UBYTE4
		2 * 2,		// D3DDECLTYPE_SHORT2
		4 * 4,		// D3DDECLTYPE_SHORT4
		4 * 1,		// D3DDECLTYPE_UBYTE4N
		2 * 2,		// D3DDECLTYPE_SHORT2N
		4 * 2,		// D3DDECLTYPE_SHORT4N
		2 * 2,		// D3DDECLTYPE_USHORT2N
		4 * 2,		// D3DDECLTYPE_USHORT4N
		1 * 4,		// D3DDECLTYPE_UDEC3
		1 * 4,		// D3DDECLTYPE_DEC3N
		2 * 2,		// D3DDECLTYPE_FLOAT16_2
		4 * 2,		// D3DDECLTYPE_FLOAT16_4
		0,				// D3DDECLTYPE_UNUSED
	};

	assert( type <= D3DDECLTYPE_UNUSED );

	return size[type];
}


bool operator == ( const D3DVERTEXELEMENT9 & e1, const D3DVERTEXELEMENT9 & e2 )
{
	int cmpret = memcmp( & e1, & e2, sizeof( D3DVERTEXELEMENT9 ) );
	
	bool retval1 = cmpret == 0 ? true : false;
	bool retval2 = memcmp( & e1, & e2, sizeof( D3DVERTEXELEMENT9 ) ) == 0 ? true : false;
	
	return retval2;
}

unsigned int stride( const VertexDescriptor & desc)
{
	unsigned int	total = 0;

	for( unsigned int i = 0; i < desc.size( ); i++)
		total += bytes( (D3DDECLTYPE) desc[i].Type );

	return total;
}

void convertDescriptors( const VertexDescriptor & vtxDesc, const StreamDescriptor & strDesc, unsigned int & StreamFlags, unsigned int & FVF )
{
	unsigned int			element;

	bool bIllegalFVF = false;

	StreamFlags = 0;
	FVF = 0;

	assert( strDesc.size( ) == (vtxDesc.size( ) - 1) );

	for( element = 0; element < strDesc.size( ); element++ )
	{
		const StreamElement &		thisStream = strDesc[element];

		switch( thisStream.type & StreamElement::STREAM_MASK )
		{
		case StreamElement::STREAM_POSITION_W:
			StreamFlags |= STFLAG_POSITION4;
			break;

		case StreamElement::STREAM_NORMAL:
			StreamFlags |= STFLAG_NORMAL;
			break;

		case StreamElement::STREAM_BINORMAL:
			StreamFlags |= STFLAG_BINORMAL;
			break;

		case StreamElement::STREAM_DIFFUSE:
			StreamFlags += (1 << STFLAG_COLOR_CHANNELS_SHIFT);
			if( thisStream.type & StreamElement::STREAM_FLAG_PAD )
				StreamFlags |= (1 << STFLAG_DUMMY_COLOR_SHIFT);
			break;

		case StreamElement::STREAM_SPECULAR:
			StreamFlags += (1 << STFLAG_COLOR_CHANNELS_SHIFT);
			if( thisStream.type & StreamElement::STREAM_FLAG_PAD )
				StreamFlags |= (2 << STFLAG_DUMMY_COLOR_SHIFT);
			break;

		case StreamElement::STREAM_TEXCOORD:
			StreamFlags += (1 << STFLAG_TEXTURE_CHANNELS_SHIFT);

			unsigned int	tlayer = thisStream.type >> 16;
			unsigned int	tbit = 1 << tlayer;

			if( thisStream.type & StreamElement::STREAM_FLAG_PAD )
				StreamFlags |= (tbit << STFLAG_DUMMY_TEXTURE_SHIFT);
			break;
		} 
	}

	for( element = 0; element < vtxDesc.size( ); element++ )
	{
		const	D3DVERTEXELEMENT9 &	thisVtx = vtxDesc[element];

		switch( thisVtx.Usage )
		{
		case D3DDECLUSAGE_POSITION:
			if( thisVtx.Type == D3DDECLTYPE_FLOAT4 )
				bIllegalFVF = true;
			else
				FVF |= D3DFVF_XYZ;
			break;
		case D3DDECLUSAGE_NORMAL:
			switch( thisVtx.UsageIndex )
			{
			case 0:
				FVF |= D3DFVF_NORMAL;
				break;
			default:
				bIllegalFVF = true;
				break;
			}
			break;
		case D3DDECLUSAGE_TEXCOORD:
			FVF += 0x100;
			break;
		case D3DDECLUSAGE_COLOR:
			switch( thisVtx.UsageIndex )
			{
			case 0:
				FVF |= D3DFVF_DIFFUSE;
				break;
			case 1:
				FVF |= D3DFVF_SPECULAR;
				break;
			default:
				bIllegalFVF = true;
			}
			break;
		default:
			break;
		}

	}
	
	if( bIllegalFVF )
		FVF = 0;
}

D3DVERTEXELEMENT9 createD3DVERTEXELEMENT9( unsigned short stream, unsigned short offset, unsigned char type, unsigned char method, unsigned char usage, unsigned char index)
{
	D3DVERTEXELEMENT9				temp;

	temp.Stream = stream;
	temp.Offset = offset;
	temp.Type = type;
	temp.Method = method;
	temp.Usage = usage;
	temp.UsageIndex = index;

	return temp;
}

StreamElement createStreamElement( unsigned int _type, FVFVertexStream * _ptr )
{
	StreamElement					temp;

	temp.type = _type;
	temp.ptr = _ptr;

	return temp;
}

