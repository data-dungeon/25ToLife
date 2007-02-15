#include "ExportPch.h"
#include "VUBufferWrite.h"

const unsigned int VUBufferWrite::STAGE_IDX_DONT_CHANGE = -1;

VUBufferWrite::VUBufferWrite( unsigned int _VertsPerSet, RGBADataFormat defRGBAFormat, NormalDataFormat defNormalFormat) :
	d_VertsPerSet( _VertsPerSet),
	p_VIFControl( 0), p_DMAStream( 0), p_Protocol( 0),
	d_RGBADataFormat( defRGBAFormat), d_NormalDataFormat( defNormalFormat)
{ 
	d_ADCValue = 0x8000; 
	
	d_ClrStageIdx = 0; 
	d_TCStageIdx = 0; 
	d_NrmStageIdx = 0; 
	
	lastGeometryStreamVUMemOffset = 0;
	lastRGBAStreamVUMemOffset = 0;
	lastNormalStreamVUMemOffset = 0;
	
}

void VUBufferWrite::setStageIdxs( int idxClrStage, int idxTCStage, int idxNrmStage)
{
	if( idxClrStage >= 0)
		d_ClrStageIdx = idxClrStage;

	if( idxTCStage >= 0)
		d_TCStageIdx = idxTCStage;

	if( idxNrmStage >= 0)
		d_NrmStageIdx = idxNrmStage;

}

void VUBufferWrite::getStageIdxs( unsigned int &idxClrStage, unsigned int &idxTCStage, unsigned int &idxNrmStage)
{
	idxClrStage = d_ClrStageIdx;
	idxTCStage = d_TCStageIdx;
	idxNrmStage = d_NrmStageIdx;
}

unsigned int VUBufferWrite::writeUNPACK( unsigned int &baseOffset, unsigned int &writeQWs, VIFUnpackCode cmd, unsigned int useTOPS, unsigned int isUnsigned )
{

	unsigned int writeCount = (writeQWs > 256) ? 256 : writeQWs;

	p_VIFControl->startUNPACK( baseOffset, cmd, (writeCount == 256) ?  0 : writeCount, useTOPS, 0, isUnsigned );

	writeQWs -= writeCount;
	baseOffset += writeCount;

	return writeCount;
}

void VUBufferWrite::writeVtxAndCompRGB32( const Position3<float> &Pos, const BaseColor<unsigned short> &Col, unsigned int ADCVal)
{
float							fVal;
unsigned int			ulCombined;

		fVal = Pos.X( );
		ulCombined = (*((unsigned int *) &fVal)) & 0xffffff00;
		ulCombined |= Col.bR( ) & 0x0ff;
		p_DMAStream->write( (char *) &ulCombined, 4);
		
		fVal = Pos.Y( );
		ulCombined = (*((unsigned int *) &fVal)) & 0xffffff00;
		ulCombined |= Col.bG( ) & 0xff;
		p_DMAStream->write( (char *) &ulCombined, 4);

		fVal = Pos.Z( );
		ulCombined = (*((unsigned int *) &fVal)) & 0xffffff00;
		ulCombined |= Col.bB( ) & 0xff;
		p_DMAStream->write( (char *) &ulCombined, 4);

		ulCombined = (ADCVal & 0xffffff00) | (Col.bA( ) & 0xff);
		p_DMAStream->write( (char *) &ulCombined, 4);

}


void VUBufferWrite::writeSTPair32( const Position2<float> &LowerTC, const Position2<float> &UpperTC)
{
float							fVal;

	fVal = LowerTC.X( );
	p_DMAStream->write( (char *) &fVal, 4);

	fVal = LowerTC.Y( );
	p_DMAStream->write( (char *) &fVal, 4);


	fVal = UpperTC.X( );
	p_DMAStream->write( (char *) &fVal, 4);

	fVal = UpperTC.Y( );
	p_DMAStream->write( (char *) &fVal, 4);

}

void VUBufferWrite::writeFloat2( float f1, float f2)
{

	p_DMAStream->write( (char *) &f1, 4);
	p_DMAStream->write( (char *) &f2, 4);
}

void VUBufferWrite::writeFloat2Int16( float f1, float f2)
{
	short						sInt16Val;

	sInt16Val = (short) f1;
	p_DMAStream->write( (char *) &sInt16Val, 2);

	sInt16Val = (short) f2;
	p_DMAStream->write( (char *) &sInt16Val, 2);

}

void VUBufferWrite::writeFloat2Int8( float f1, float f2)
{
	signed char				sInt8Val;

	sInt8Val = (signed char) f1;
	p_DMAStream->write( (char *) &sInt8Val, 1);

	sInt8Val = (signed char) f2;
	p_DMAStream->write( (char *) &sInt8Val, 1);

}

void VUBufferWrite::writeFloat3( float f1, float f2, float f3)
{

	p_DMAStream->write( (char *) &f1, 4);
	p_DMAStream->write( (char *) &f2, 4);
	p_DMAStream->write( (char *) &f3, 4);

}

void VUBufferWrite::writeFloat3Int16( float f1, float f2, float f3)
{
	short						sInt16Val;

	sInt16Val = (short) f1;
	p_DMAStream->write( (char *) &sInt16Val, 2);

	sInt16Val = (short) f2;
	p_DMAStream->write( (char *) &sInt16Val, 2);

	sInt16Val = (short) f3;
	p_DMAStream->write( (char *) &sInt16Val, 2);
}

void VUBufferWrite::writeFloat3Int8( float f1, float f2, float f3)
{
	signed char				sInt8Val;

	sInt8Val = (signed char) f1;
	p_DMAStream->write( (char *) &sInt8Val, 1);

	sInt8Val = (signed char) f2;
	p_DMAStream->write( (char *) &sInt8Val, 1);

	sInt8Val = (signed char) f3;
	p_DMAStream->write( (char *) &sInt8Val, 1);

}

void VUBufferWrite::writeFloat4( float f1, float f2, float f3, float f4)
{

	p_DMAStream->write( (char *) &f1, 4);
	p_DMAStream->write( (char *) &f2, 4);
	p_DMAStream->write( (char *) &f3, 4);
	p_DMAStream->write( (char *) &f4, 4);

}

void VUBufferWrite::writeFloat4Int16( float f1, float f2, float f3, float f4)
{
	short						sInt16Val;

	sInt16Val = (short) f1;
	p_DMAStream->write( (char *) &sInt16Val, 2);

	sInt16Val = (short) f2;
	p_DMAStream->write( (char *) &sInt16Val, 2);

	sInt16Val = (short) f3;
	p_DMAStream->write( (char *) &sInt16Val, 2);

	sInt16Val = (short) f4;
	p_DMAStream->write( (char *) &sInt16Val, 2);

}

void VUBufferWrite::writeFloat4Int8( float f1, float f2, float f3, float f4)
{
	signed char				sInt8Val;

	sInt8Val = (signed char) f1;
	p_DMAStream->write( (char *) &sInt8Val, 1);

	sInt8Val = (signed char) f2;
	p_DMAStream->write( (char *) &sInt8Val, 1);

	sInt8Val = (signed char) f3;
	p_DMAStream->write( (char *) &sInt8Val, 1);

	sInt8Val = (signed char) f4;
	p_DMAStream->write( (char *) &sInt8Val, 1);

}

void VUBufferWrite::writeColorPair5551( const BaseColor<unsigned short> &Col1, const BaseColor<unsigned short> &Col2)
{
	unsigned short Color;

	Color = Col1.ColorRGBA5551( );
	p_DMAStream->write( (char *) &Color, 2);


	Color = Col2.ColorRGBA5551( );
	p_DMAStream->write( (char *) &Color, 2);

}
