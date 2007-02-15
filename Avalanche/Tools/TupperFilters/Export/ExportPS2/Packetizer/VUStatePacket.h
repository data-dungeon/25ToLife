#ifndef __VU_STATE_PACKET_H
#define __VU_STATE_PACKET_H

#include "VUStateProtocol.h"
#include "Display/PS2/Renderer/Common/GIFTag.h"

class VUStatePacket : public VUStateProtocol
{
protected:
	typedef struct _StateInfo
	{
		ShadeMode		shading;
		TextureMode		texturing;
	} StateInfo;

	StateInfo 					d_StateInfo;
	unsigned int 				d_ConstantColor;
	unsigned int				d_MaxPacketRegs;

	unsigned int				d_LastPacketWritePos;
	
	unsigned int				d_TexFixupStartDelta;
	unsigned int				d_TexFixupFixupDelta;
	
	GIFTagW						d_StatePacketTag;
	unsigned int *				p_StatePacketMem;

	virtual unsigned int	calcPacketRegs( void ) const = 0;
	virtual unsigned int	fillPacketData( void ) = 0;
	
	void						fixupPacketChain( ostream &str);

public:
	VUStatePacket(
		unsigned int 			_MaxPacketRegs,
		unsigned int			_PacketFixupDelta,
		ShadeMode				_ShadingMode = MODE_SHADE_CONSTANT,
		unsigned int 			_ConstantColor = 0xff808080,
		TextureMode				_TextureMode = MODE_UNTEXTURED
		);

	VUStatePacket(const VUStatePacket &s);
	const VUStatePacket &operator=(const VUStatePacket &s);


	~VUStatePacket( void );

	unsigned int			sizeVUQWords( void ) const
	{
		return calcPacketRegs( ) + 1;
	}

	virtual unsigned int sizeDMAWords( void ) const
	{
		return 1 + (sizeVUQWords( ) * 4);
	}

	virtual unsigned int	numRegs( void )	const			{ return calcPacketRegs( ); }
	
	void *					getPacketMem( unsigned int & numQWs)
	{
		numQWs = fillPacketData( );
		return p_StatePacketMem;
	}
	
	GIFTag & 				getPacketTag( void )
	{
		fillPacketData( );
		
		static GIFTag Temp( d_StatePacketTag);
		
		return Temp;
	}			
	
	ShadeMode				getShadingMode( void ) const						{ return d_StateInfo.shading; }
	TextureMode				getTextureMode( void ) const						{ return d_StateInfo.texturing; }

	unsigned int			getConstantColor( void ) const					{ return d_ConstantColor; }

	void 						setShadingMode( ShadeMode mode ) 				{ d_StateInfo.shading = mode; }
	void 						setTextureMode( TextureMode mode )				{ d_StateInfo.texturing = mode; }

	void 						setConstantColor( unsigned int color ) 		{ d_ConstantColor = color; }

	unsigned int			getTexFixupLastPacketPos( void ) const			{ return d_LastPacketWritePos + d_TexFixupStartDelta; }
	unsigned int			getTexFixupLastPacketFixupPos( void ) const 	{ return d_LastPacketWritePos + d_TexFixupFixupDelta; }

	virtual void			write( ostream &stream );
	virtual void			writeVIF( ostream &stream, unsigned int &baseOffset );
	virtual void *			writeVIF( void *pMem, unsigned int &baseOffset );
	virtual void *			writeData( void *pMem );

} ;

#endif
