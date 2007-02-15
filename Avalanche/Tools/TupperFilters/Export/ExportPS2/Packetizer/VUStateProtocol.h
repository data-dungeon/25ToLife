#ifndef __VU_STATE_PROTOCOL_H
#define __VU_STATE_PROTOCOL_H

#include "Display/PS2/Renderer/Common/PRIMReg.h"

#include <ostream>
using namespace 			std;

class VUStateProtocol
{
	PRIMReg					PRIM;

	static int				d_LastPacketStartStreamPos;
	static int				d_LastPacketFixupStreamPos;

	unsigned int			d_StatePacketOffset;
	unsigned int			d_StatePacketFixupOffset;

public:
	typedef enum _ShadeMode
	{
		MODE_SHADE_UNSHADED = 0,
		MODE_SHADE_CONSTANT,
		MODE_SHADE_FLAT,
		MODE_SHADE_GOURAUD
	} ShadeMode;

	typedef enum _TextureMode
	{
		MODE_UNTEXTURED = 0,
		MODE_DECAL_TEXTURED,
		MODE_ALPHA_TEXTURED,
		MODE_DIFFUSE_TEXTURED,
	} TextureMode;

public:
	VUStateProtocol( 
		unsigned int				_StatePacketOffset, 
		unsigned int				_StatePacketFixupOffset
		) :
	d_StatePacketOffset( _StatePacketOffset),
	d_StatePacketFixupOffset( _StatePacketFixupOffset)
	{
	}

	virtual unsigned int		sizeVUQWords( void ) const = 0;
	virtual unsigned int 	sizeDMAWords( void ) const = 0;
	virtual unsigned int 		numRegs( void ) const = 0;
	
	virtual ShadeMode			getShadingMode( void ) const = 0;
	virtual TextureMode		getTextureMode( void ) const = 0;
	
	virtual unsigned int		getConstantColor( void ) const = 0;

	virtual unsigned int		getDynamicTextureID( void) const = 0;


	static void					clearFixupChain( void )						{ d_LastPacketStartStreamPos = d_LastPacketFixupStreamPos = -1; }

	static int					getLastPacketStartPos( void )				{ return d_LastPacketStartStreamPos; }
	static int					getLastPacketFixupPos( void )				{ return d_LastPacketFixupStreamPos; }


	unsigned int				getPacketStartOffset( void ) const		{ return d_StatePacketOffset; }
	unsigned int				getPacketFixupOffset( void ) const		{ return d_StatePacketFixupOffset; }


	void							setPacketStartPos( unsigned int pos)	
	{ 
		d_LastPacketStartStreamPos = pos; 
		d_LastPacketFixupStreamPos = pos + (d_StatePacketFixupOffset - d_StatePacketOffset);
	}

	virtual void				write( ostream &stream ) = 0;
	virtual void				writeVIF( ostream &VIFStream, unsigned int &baseOffset) = 0;
	virtual void *				writeVIF( void *pVIFMem, unsigned int &baseOffset) = 0;
	virtual void *				writeData( void *pVIFMem) = 0;

	const PRIMReg &			getPRIM( void )
	{
		switch( getTextureMode( ) )
		{
		case MODE_UNTEXTURED:
			PRIM.setTME( 0);
			PRIM.setABE( 0);

		break;
		case MODE_DECAL_TEXTURED:
			PRIM.setTME( 1);
			PRIM.setABE( 0);
		break;
		case MODE_DIFFUSE_TEXTURED:
		case MODE_ALPHA_TEXTURED:
			PRIM.setTME( 1);
			PRIM.setABE( 1);
			break;
		}
	
		if( getShadingMode( ) == MODE_SHADE_GOURAUD )
			PRIM.setIIP( 1);
		else
			PRIM.setIIP( 0);

		return PRIM;
	}
} ;

#endif