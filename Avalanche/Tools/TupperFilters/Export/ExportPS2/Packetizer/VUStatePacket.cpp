#include "ExportPch.h"
#include <assert.h>

#include "VUStatePacket.h"
#include "Display/PS2/Renderer/Common/VIFCreator.h"

using namespace			VIFCreator;

VUStatePacket::VUStatePacket( unsigned int maxPacketRegs, unsigned int _PacketFixupOffset,
	ShadeMode _ShadingMode,	unsigned int _ConstantColor, TextureMode _TextureMode) :
	d_MaxPacketRegs( maxPacketRegs),
	d_ConstantColor( _ConstantColor),
	VUStateProtocol( 20, 20 + _PacketFixupOffset)
{
	d_LastPacketWritePos = 0;
	d_StateInfo.shading = _ShadingMode;
	d_StateInfo.texturing = _TextureMode;

	p_StatePacketMem = new unsigned int[maxPacketRegs * 4];

	d_StatePacketTag.setNLOOP( 1);
}


VUStatePacket::~VUStatePacket( void )
{
	delete [] p_StatePacketMem;
}

VUStatePacket::VUStatePacket(const VUStatePacket &s) : 
	VUStateProtocol(s),
	d_StateInfo(s.d_StateInfo),
	d_ConstantColor(s.d_ConstantColor),
	d_MaxPacketRegs(s.d_MaxPacketRegs),
	d_LastPacketWritePos(s.d_LastPacketWritePos),
	d_TexFixupStartDelta(s.d_TexFixupStartDelta),
	d_TexFixupFixupDelta(s.d_TexFixupFixupDelta),
	d_StatePacketTag(s.d_StatePacketTag)
{
	p_StatePacketMem = new unsigned int[d_MaxPacketRegs*4];
	memcpy(p_StatePacketMem,s.p_StatePacketMem,sizeof(int)*d_MaxPacketRegs*4);
}

const VUStatePacket &VUStatePacket::operator=(const VUStatePacket &s)
{
	d_StateInfo = s.d_StateInfo;
	d_ConstantColor = s.d_ConstantColor;
	d_MaxPacketRegs = s.d_MaxPacketRegs;
	d_LastPacketWritePos = s.d_LastPacketWritePos;
	d_TexFixupStartDelta = s.d_TexFixupStartDelta;
	d_TexFixupFixupDelta = s.d_TexFixupFixupDelta;
	d_StatePacketTag = s.d_StatePacketTag;

	delete p_StatePacketMem;
	p_StatePacketMem = new unsigned int[d_MaxPacketRegs*4];
	memcpy(p_StatePacketMem,s.p_StatePacketMem,sizeof(int)*d_MaxPacketRegs*4);
	
	return *this;
}

void VUStatePacket::write( ostream &str )
{
	// write directly on the stream without unpack instructions
	unsigned int nRegs = fillPacketData( );

	assert( nRegs == d_StatePacketTag.NREG() );

	if( nRegs )
	{
		str << d_StatePacketTag;
		
		if( getLastPacketFixupPos( ) != -1)
			fixupPacketChain( str);

		setPacketStartPos( str.tellp( ) );
		str.write( (char *) p_StatePacketMem, 16 * nRegs );
	}
}

void VUStatePacket::writeVIF( ostream & str, unsigned int &baseOffset)
{
	// write directly on the stream with the necessary unpack instruction
	unsigned int nRegs = fillPacketData( );

	assert( nRegs == d_StatePacketTag.NREG( ) );

	if( nRegs)
	{
		str << createUNPACK( baseOffset, V4_32, nRegs + 1, 1);
		str << d_StatePacketTag;

		if( getLastPacketFixupPos( ) != -1)
			fixupPacketChain( str);

		setPacketStartPos( str.tellp( ) );

		str.write( (char *) p_StatePacketMem, 16 * nRegs);

		baseOffset += nRegs + 1;
	}
}

void VUStatePacket::fixupPacketChain( ostream &str)
{
	assert( getLastPacketFixupPos( ) != -1);

	unsigned int currPos = str.tellp( );
	unsigned int fixupDelta = currPos - getLastPacketStartPos( );

	str.seekp( getLastPacketFixupPos( ) );

	str.write( (char *) &fixupDelta, sizeof( unsigned int ) );

	str.seekp( currPos);
}

void *VUStatePacket::writeVIF( void *pMem, unsigned int &baseOffset)
{
	unsigned int nRegs = fillPacketData( );
	unsigned int *pRegMem = (unsigned int *) pMem;
	
	assert(nRegs <= d_MaxPacketRegs);

	*pRegMem++ = createUNPACK( baseOffset, V4_32, nRegs + 1, 1);
	
	GIFTag TempTag( d_StatePacketTag);

	memcpy( pRegMem, &TempTag, 16);
	pRegMem = (unsigned int *) ((unsigned int) pRegMem + 16);

	memcpy( pRegMem, p_StatePacketMem, nRegs * 16);
	pRegMem = (unsigned int *) ((unsigned int) pRegMem + (nRegs * 16));

	baseOffset += nRegs + 1;
	
	return pRegMem;
}

void *VUStatePacket::writeData( void *pMem)
{
	unsigned int nRegs = fillPacketData( );
	void *pRegMem = pMem;
	
	assert(nRegs <= d_MaxPacketRegs);

	GIFTag TempTag( d_StatePacketTag);

	memcpy( pRegMem, &TempTag, 16);
	pRegMem = (void *) ((unsigned int) pRegMem + 16);

	memcpy( pRegMem, p_StatePacketMem, nRegs * 16);
	pRegMem = (void *) ((unsigned int) pRegMem + (nRegs * 16));

	return pRegMem;
}
