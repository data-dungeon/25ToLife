#include "ExportPch.h"
#include <assert.h>

#include "writeVSTandZ32.h"


writeVSTandZ32::writeVSTandZ32( void ) : 
	VUBufferWrite( 4, createRGBADataFormat( RGBA_STORE_COMPRESSED, RGBA_8888), createNormalDataFormat( NORMAL_STORE_NONE, NORMAL_INT16))
{

}

unsigned int writeVSTandZ32::getVUQWordsPerSet( void )
{
unsigned int			setQWords = 5;

	if( getRGBAStorageMethod( ) == RGBA_STORE_UNCOMPRESSED)
		setQWords += 4;

	if( getNormalStorageMethod( ) != NORMAL_STORE_NONE)
	{
		if( getNormalStorageMethod( ) == NORMAL_STORE_ROW_STANDARD)
			setQWords += 4;
		else 
			setQWords += 3;
	}

	return setQWords;
}


unsigned int writeVSTandZ32::calcBlockSize( unsigned int readVerts, unsigned int writeVerts)
{
	assert( readVerts == writeVerts);

unsigned int			writeWords = 0;
unsigned int			writeQWords = 0;
unsigned int			writeUNPACKs = 0;


	// primary geometry block
	writeQWords = readVerts + ((readVerts + 3) / 4);
	writeUNPACKs = (readVerts + 255) / 256;

	// optional uncompressed RGBAs
	if( getRGBAStorageMethod( ) == RGBA_STORE_UNCOMPRESSED)
	{
		if( getRGBADataSize( ) == RGBA_8888)
		{
			writeWords += readVerts;
			writeUNPACKs += (readVerts + 255) / 256;
		}
		else if( getRGBADataSize( ) == RGBA_5551)
		{
			writeWords += (readVerts + 1) / 2;
			writeUNPACKs += (readVerts + 255) / 256;
		}
	}

	// optional normals
	if( getNormalStorageMethod( ) != NORMAL_STORE_NONE)
	{
		unsigned int		writtenSets = 0;

		writeUNPACKs += (readVerts + 255) / 256;

		if( getNormalStorageMethod( ) == NORMAL_STORE_COLUMN)
			writtenSets = 4 * ((readVerts + 3) / 4);
		else if( getNormalStorageMethod( ) == NORMAL_STORE_ROW_COMPACT)
			writtenSets = 4 * ((readVerts + 3) / 4);
		else
			writtenSets = readVerts;

		if( getNormalDataSize( ) == NORMAL_FLOAT)
			writeWords += writtenSets * 3;
		else if( getNormalDataSize( ) == NORMAL_INT8)
			writeWords += ((writtenSets * 3) + 3) / 4;
		else
			writeWords += ((writtenSets * 3) + 1) / 2;
	}

	return writeUNPACKs + writeWords + (writeQWords * 4);
}


void writeVSTandZ32::writeBlock( unsigned int &baseOffset, unsigned int baseVert, unsigned int readVerts, unsigned int writeVerts)
{

	assert( p_Protocol != 0);
	assert( p_DMAStream != 0);
	assert( d_VertexSpan != 0);
	assert( readVerts == writeVerts);

	// write the primary geometry block
	lastGeometryStreamVUMemOffset = baseOffset;

	writePrimaryGeometryBlock( baseOffset, baseVert, readVerts);

	// write optional uncompressed RGBAs
	if( getRGBAStorageMethod( ) == RGBA_STORE_UNCOMPRESSED)
	{
		lastRGBAStreamVUMemOffset = baseOffset;

		if( getRGBADataSize( ) == RGBA_8888)
			writeRGBABlock_8888( baseOffset, baseVert, readVerts);
		else 
			writeRGBABlock_5551( baseOffset, baseVert, readVerts);
	}
	else
	{
		lastRGBAStreamVUMemOffset = 0;
	}
	
	// write optional normals
	switch( getNormalStorageMethod( ))
	{
		case NORMAL_STORE_COLUMN:
			lastNormalStreamVUMemOffset = baseOffset;
			writeNormalBlock_Column( baseOffset, baseVert, readVerts);
			break;
		case NORMAL_STORE_ROW_COMPACT:
			lastNormalStreamVUMemOffset = baseOffset;
			writeNormalBlock_RowCompact( baseOffset, baseVert, readVerts);
			break;
		case NORMAL_STORE_ROW_STANDARD:
			lastNormalStreamVUMemOffset = baseOffset;
			writeNormalBlock_Row( baseOffset, baseVert, readVerts);
			break;
		default:
			lastNormalStreamVUMemOffset = 0;
			break;
	}

}

void writeVSTandZ32::writeNormalBlock_Column( unsigned int &baseOffset, unsigned int baseVert, unsigned int readVerts)
{
VIFUnpackCode			unpackType;
float					fScalar;
void 					(VUBufferWrite::*writeMethod)( float, float, float, float, float);

	switch( getNormalDataSize( ))
	{
	case NORMAL_FLOAT:
		unpackType = V4_32;
		writeMethod = writeFloat4;
		fScalar = 1.0f;
		break;

	default:
	case NORMAL_INT16:
		unpackType = V4_16;
		writeMethod = writeFloat4Int16;
		fScalar = 32767.0f;
		break;

	case NORMAL_INT8:
		unpackType = V4_8;
		writeMethod = writeFloat4Int8;
		fScalar = 127.0f;
		break;
	}

	unsigned int writeQWs = 4 * ((readVerts + 3) / 4);

	unsigned int writeCount = writeUNPACK( baseOffset, writeQWs, unpackType);
	
	for( unsigned int i = 0; i < readVerts; i += 4)
	{
		const Position3<float>	Fill( 0.0f, 0.0f, 0.0f);

		unsigned int 			nrmIdx = baseVert + i;
		
		const	Position3<float> 	Nrm1 = p_Protocol->nrm( d_NrmStageIdx, nrmIdx);
		const	Position3<float> 	Nrm2 = ((i + 1) < readVerts) ? p_Protocol->nrm( d_NrmStageIdx, nrmIdx + 1) : Fill;
		const	Position3<float> 	Nrm3 = ((i + 2) < readVerts) ? p_Protocol->nrm( d_NrmStageIdx, nrmIdx + 2) : Fill;
		const	Position3<float> 	Nrm4 = ((i + 3) < readVerts) ? p_Protocol->nrm( d_NrmStageIdx, nrmIdx + 3) : Fill;

		(this->*writeMethod)( Nrm1.X( ), Nrm2.X( ), Nrm3.X( ), Nrm4.X( ), fScalar);

		if( (--writeCount == 0)  &&  writeQWs)
			writeCount = writeUNPACK( baseOffset, writeQWs, unpackType, 1, 0);

		(this->*writeMethod)( Nrm1.Y( ), Nrm2.Y( ), Nrm3.Y( ), Nrm4.Y( ), fScalar);

		if( (--writeCount == 0)  &&  writeQWs)
			writeCount = writeUNPACK( baseOffset, writeQWs, unpackType, 1, 0);

		(this->*writeMethod)( Nrm1.Z( ), Nrm2.Z( ), Nrm3.Z( ), Nrm4.Z( ), fScalar);

		if( (--writeCount == 0)  &&  writeQWs)
			writeCount = writeUNPACK( baseOffset, writeQWs, unpackType, 1, 0);
	}

	p_VIFControl->endUNPACK( );

}

void writeVSTandZ32::writeNormalBlock_RowCompact( unsigned int &baseOffset, unsigned int baseVert, unsigned int readVerts)
{
VIFUnpackCode			unpackType;
float					fScalar;
void 						(VUBufferWrite::*writeMethod)( float, float, float, float, float);

	switch( getNormalDataSize( ))
	{
	case NORMAL_FLOAT:
		unpackType = V4_32;
		writeMethod = writeFloat4;
		fScalar = 1.0f;
		break;

	default:
	case NORMAL_INT16:
		unpackType = V4_16;
		writeMethod = writeFloat4Int16;
		fScalar = 32767.0f;
		break;

	case NORMAL_INT8:
		unpackType = V4_8;
		writeMethod = writeFloat4Int8;
		fScalar = 127.0f;
		break;
	}

	unsigned int writeQWs = 4 * ((readVerts + 3) / 4);

	unsigned int writeCount = writeUNPACK( baseOffset, writeQWs, unpackType);
	
	for( unsigned int i = 0; i < readVerts; i += 4)
	{
		const Position3<float>	Fill( 0.0f, 0.0f, 0.0f);

		unsigned int 			nrmIdx = baseVert + i;
		
		const	Position3<float> 	Nrm1 = p_Protocol->nrm( d_NrmStageIdx, nrmIdx);
		const	Position3<float> 	Nrm2 = ((i + 1) < readVerts) ? p_Protocol->nrm( d_NrmStageIdx, nrmIdx + 1) : Fill;
		const	Position3<float> 	Nrm3 = ((i + 2) < readVerts) ? p_Protocol->nrm( d_NrmStageIdx, nrmIdx + 2) : Fill;
		const	Position3<float> 	Nrm4 = ((i + 3) < readVerts) ? p_Protocol->nrm( d_NrmStageIdx, nrmIdx + 3) : Fill;

		(this->*writeMethod)( Nrm1.X( ), Nrm1.Y( ), Nrm1.Z( ), Nrm4.X( ), fScalar);

		if( (--writeCount == 0)  &&  writeQWs)
			writeCount = writeUNPACK( baseOffset, writeQWs, unpackType, 1, 0);

		(this->*writeMethod)( Nrm2.X( ), Nrm2.Y( ), Nrm3.Z( ), Nrm4.Y( ), fScalar);

		if( (--writeCount == 0)  &&  writeQWs)
			writeCount = writeUNPACK( baseOffset, writeQWs, unpackType, 1, 0);

		(this->*writeMethod)( Nrm3.X( ), Nrm3.Y( ), Nrm3.Z( ), Nrm4.Z( ), fScalar);

		if( (--writeCount == 0)  &&  writeQWs)
			writeCount = writeUNPACK( baseOffset, writeQWs, unpackType, 1, 0);
	}

	p_VIFControl->endUNPACK( );

}

void writeVSTandZ32::writeNormalBlock_Row( unsigned int &baseOffset, unsigned int baseVert, unsigned int readVerts)
{
VIFUnpackCode			unpackType;
float					fScalar;
void 						(VUBufferWrite::*writeMethod)( const Position3<float> &Pos3, float);

	switch( getNormalDataSize( ))
	{
	case NORMAL_FLOAT:
		unpackType = V3_32;
		writeMethod = writePos3Float;
		fScalar = 1.0f;
		break;

	default:
	case NORMAL_INT16:
		unpackType = V3_16;
		writeMethod = writePos3FloatInt16;
		fScalar = 32767.0f;
		break;

	case NORMAL_INT8:
		unpackType = V3_8;
		writeMethod = writePos3FloatInt8;
		fScalar = 127.0f;
		break;
	}

	unsigned int writeQWs = readVerts;

	unsigned int writeCount = writeUNPACK( baseOffset, writeQWs, unpackType);
	
	for( unsigned int i = 0; i < readVerts; i++)
	{
		unsigned int 			nrmIdx = baseVert + i;
		
		const	Position3<float> 	Nrm = p_Protocol->nrm( d_NrmStageIdx, nrmIdx);

		(this->*writeMethod)( Nrm, fScalar);

		if( (--writeCount == 0)  &&  writeQWs)
			writeCount = writeUNPACK( baseOffset, writeQWs, unpackType, 1, 0);
	}

	p_VIFControl->endUNPACK( );

}



void writeVSTandZ32::writePrimaryGeometryBlock( unsigned int &baseOffset, unsigned int baseVert, unsigned int readVerts)
{
unsigned int 	i;

	bool writeRGBAData = (getRGBAStorageMethod( ) == RGBA_STORE_COMPRESSED);

	unsigned int writeQWsSTandXY = readVerts;
	unsigned int writeQWsZADC = ((readVerts + 3) / 4);

	unsigned int writeQWs = writeQWsSTandXY + writeQWsZADC;

	unsigned int writeCount = writeUNPACK( baseOffset, writeQWs, V4_32);

	// write primary geometry block 
	//	Y1 | X1 | T1 | S1	(w / optional CompressedColor)
	//	Y2 | X2 | T2 | S2	
	//	Y3 | X3 | T3 | S3	
	//	Y4 | X4 | T4 | S4	
	// Z4 | Z3 | Z2 | Z1 (w / ADC bits)

	// write Y | X | T | S
	for( i = 0; i < readVerts; i++)
	{
		unsigned int				vtxIdx, clrIdx;

		clrIdx = baseVert + i;
		vtxIdx = p_Protocol->idx( baseVert + i);

		const Position3<float> Pos = p_Protocol->vtx( vtxIdx & MeshInterface::IDX_MASK);
		const Position2<float> TC = p_Protocol->st( d_TCStageIdx, baseVert + i + 0);

		if( writeRGBAData)
		{
			const BaseColor<unsigned short> Clr = p_Protocol->clr( d_ClrStageIdx, clrIdx);

			writeSTandXYandCompRGB32( TC, Pos, Clr);
		}
		else
		{
			BaseColor<unsigned short> Clr( 0, 0, 0, 128 );
			writeSTandXYandCompRGB32( TC, Pos, Clr);
//			writeFloat4( TC.X( ), TC.Y( ), Pos.X( ), Pos.Y( ) );
		}

 		if( (--writeCount == 0)  &&  writeQWs)
			writeCount = writeUNPACK( baseOffset, writeQWs, V4_32);
	}

	// write Z | Z | Z | Z
	for( i = 0; i < readVerts; i += 4)
	{
		unsigned int				vtxIdx;
		unsigned int				ADCBits = 0;
		float							fZ1, fZ2, fZ3, fZ4;
		
		vtxIdx = p_Protocol->idx( baseVert + i);
		
		const Position3<float> Vtx1 = p_Protocol->vtx( vtxIdx & MeshInterface::IDX_MASK);
		fZ1 = Vtx1.Z( );
		
		if( d_ADCCount)
		{
			ADCBits |= 0x01;
			d_ADCCount--;
		}

		if( vtxIdx & MeshInterface::IDX_END_OF_STRIP)
			d_ADCCount = 2;

		if( (i + 1) < readVerts)
		{
			vtxIdx = p_Protocol->idx( baseVert + i + 1);
		
			const Position3<float> Vtx = p_Protocol->vtx( vtxIdx & MeshInterface::IDX_MASK);
			fZ2 = Vtx.Z( );
		
			if( d_ADCCount)
			{
				ADCBits |= 0x02;
				d_ADCCount--;
			}

			if( vtxIdx & MeshInterface::IDX_END_OF_STRIP)
				d_ADCCount = 2;
		}
		else
			fZ2 = 0.0f;

		if( (i + 2) < readVerts)
		{
			vtxIdx = p_Protocol->idx( baseVert + i + 2);
		
			const Position3<float> Vtx = p_Protocol->vtx( vtxIdx & MeshInterface::IDX_MASK);
			fZ3 = Vtx.Z( );
		
			if( d_ADCCount)
			{
				ADCBits |= 0x04;
				d_ADCCount--;
			}

			if( vtxIdx & MeshInterface::IDX_END_OF_STRIP)
				d_ADCCount = 2;
		}
		else
			fZ3 = 0.0f;

		if( (i + 3) < readVerts)
		{
			vtxIdx = p_Protocol->idx( baseVert + i + 3);
		
			const Position3<float> Vtx = p_Protocol->vtx( vtxIdx & MeshInterface::IDX_MASK);
			fZ4 = Vtx.Z( );
		
			if( d_ADCCount)
			{
				ADCBits |= 0x08;
				d_ADCCount--;
			}

			if( vtxIdx & MeshInterface::IDX_END_OF_STRIP)
				d_ADCCount = 2;
		}
		else
			fZ4 = 0.0f;

		writeZ4andADC( fZ1, fZ2, fZ3, fZ4, ADCBits);
		
		if( (--writeCount == 0)  &&  writeQWs)
			writeCount = writeUNPACK( baseOffset, writeQWs, V4_32);
	}

	p_VIFControl->endUNPACK( );
}

void writeVSTandZ32::writeRGBABlock_8888( unsigned int &baseOffset, unsigned int baseVert, unsigned int readVerts)
{
unsigned int			i;

	unsigned int			writeQWs = readVerts;
	unsigned int			writeCount = writeUNPACK( baseOffset, writeQWs, V4_8);

	for( i = 0; i < readVerts; i++)
	{
		unsigned int				clrIdx;

		clrIdx = baseVert + i;

		const BaseColor<unsigned short> Clr = p_Protocol->clr( d_ClrStageIdx, clrIdx);

		unsigned int	writeVal = Clr.ColorRGBA8888( );

		p_DMAStream->write( (char *) &writeVal, 4);

		if( (--writeCount == 0)  &&  writeQWs)
			writeCount = writeUNPACK( baseOffset, writeQWs, V4_8);
	}

	p_VIFControl->endUNPACK( );
}

void writeVSTandZ32::writeRGBABlock_5551( unsigned int &baseOffset, unsigned int baseVert, unsigned int readVerts)
{
unsigned int			i;

	unsigned int writeQWs = readVerts;
	unsigned int writeCount = writeUNPACK( baseOffset, writeQWs, V4_5);

	for( i = 0; i < readVerts; i++)
	{
		unsigned int				clrIdx;

		clrIdx = baseVert + i;

		const BaseColor<unsigned short> Clr = p_Protocol->clr( d_ClrStageIdx, clrIdx);

		unsigned short	writeVal = Clr.ColorRGBA5551( );

		p_DMAStream->write( (char *) &writeVal, 2);

		if( (--writeCount == 0)  &&  writeQWs)
			writeCount = writeUNPACK( baseOffset, writeQWs, V4_5);
	}

	p_VIFControl->endUNPACK( );
}

void writeVSTandZ32::writeZ4andADC( float fZ1, float fZ2, float fZ3, float fZ4, unsigned int ADCBits)
{
unsigned int			uiCombined;

	uiCombined = (*((unsigned int *) &fZ1)) & 0xfffffff0;
	uiCombined |= ADCBits;
	p_DMAStream->write( (char *) &uiCombined, 4);
	
	p_DMAStream->write( (char *) &fZ2, 4);
	p_DMAStream->write( (char *) &fZ3, 4);
	p_DMAStream->write( (char *) &fZ4, 4);
}
	
void writeVSTandZ32::writeSTandXYandCompRGB32( const Position2<float> &TC, const Position3<float> &Pos, 
	const BaseColor<unsigned short> & Clr)
{
float							fVal;
unsigned int			ulCombined;

	fVal = TC.X( );
	ulCombined = (*((unsigned int *) &fVal)) & 0xffffff00;
	ulCombined |= Clr.bR( ) & 0x0ff;
	p_DMAStream->write( (char *) &ulCombined, 4);
		
	fVal = TC.Y( );
	ulCombined = (*((unsigned int *) &fVal)) & 0xffffff00;
	ulCombined |= Clr.bG( ) & 0xff;
	p_DMAStream->write( (char *) &ulCombined, 4);

	fVal = Pos.X( );
	ulCombined = (*((unsigned int *) &fVal)) & 0xffffff00;
	ulCombined |= Clr.bB( ) & 0xff;
	p_DMAStream->write( (char *) &ulCombined, 4);

	fVal = Pos.Y( );
	ulCombined = (*((unsigned int *) &fVal)) & 0xffffff00;
	ulCombined |= Clr.bA( ) & 0xff;
	p_DMAStream->write( (char *) &ulCombined, 4);


}
	
void writeVSTandZ32::writeSTandXY( const Position2<float> &TC, const Position3<float> &Pos)
{
		writeFloat4( TC.X( ), TC.Y( ), Pos.X( ), Pos.Y( ) );
}
