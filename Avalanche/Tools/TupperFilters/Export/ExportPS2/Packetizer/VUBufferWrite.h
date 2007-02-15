#ifndef __VU_BUFFER_WRITE_H
#define __VU_BUFFER_WRITE_H

#include <ostream>

#include "VIFControl.h"
#include "MeshInterface.h"
#include "WriteModeTypes.h"

using namespace		std;

class VUBufferWrite
{

protected:
	RGBADataFormat					d_RGBADataFormat;
	NormalDataFormat				d_NormalDataFormat;

	unsigned int					d_VertsPerSet;
	unsigned int					d_VertexSpan;

	unsigned int					d_ADCCount;
	unsigned int					d_ADCValue;

	unsigned int					d_ClrStageIdx, d_TCStageIdx, d_NrmStageIdx;

	VIFControl *					p_VIFControl;
	MeshInterface *					p_Protocol;

	ostream *						p_DMAStream;

	// packet cmd writers
	virtual unsigned int			writeUNPACK( unsigned int &baseOffset, unsigned int &writeQWs, VIFUnpackCode cmd, unsigned int useTOPS = 1, unsigned int isUnsigned = 1 );

	// shared writers
	void								writeVtxAndCompRGB32( const Position3<float> &Pos, const BaseColor<unsigned short> &Col, unsigned int ADCVal);
	void								writeSTPair32( const Position2<float> &LowerTC, const Position2<float> &UpperTC);

	void								writeFloat2( float f1, float f2);
	void								writeFloat3( float f1, float f2, float f3);
	void								writeFloat4( float f1, float f2, float f3, float f4);

	void								writeFloat2Int16( float f1, float f2);
	void								writeFloat2Int8( float f1, float f2);

	void								writeFloat3Int16( float f1, float f2, float f3);
	void								writeFloat3Int8( float f1, float f2, float f3);

	void								writeFloat4Int16( float f1, float f2, float f3, float f4);
	void								writeFloat4Int8( float f1, float f2, float f3, float f4);

	void								writeFloat2( float f1, float f2, float /* fScalar NOT USED! */) 					{ writeFloat2( f1, f2); }
	void								writeFloat2Int16( float f1, float f2, float fScalar)									{ writeFloat2Int16( f1 * fScalar, f2 * fScalar); }								
	void								writeFloat2Int8( float f1, float f2, float fScalar)  									{ writeFloat2Int8( f1 * fScalar, f2 * fScalar); }								

	void								writeFloat3( float f1, float f2, float f3, float /* fScalar NOT USED! */)		{ writeFloat3( f1, f2, f3); }
	void								writeFloat3Int16( float f1, float f2, float f3, float fScalar)						{ writeFloat3Int16( f1 * fScalar, f2 * fScalar, f3 * fScalar); }
	void								writeFloat3Int8( float f1, float f2, float f3, float fScalar)						{ writeFloat3Int8( f1 * fScalar, f2 * fScalar, f3 * fScalar); }

	void								writeFloat4( float f1, float f2, float f3, float f4, float /* fScalar NOT USED! */) { writeFloat4( f1, f2, f3, f4); }
	void								writeFloat4Int16( float f1, float f2, float f3, float f4, float fScalar)		{ writeFloat4Int16( f1 * fScalar, f2 * fScalar, f3 * fScalar, f4 * fScalar); }
	void								writeFloat4Int8( float f1, float f2, float f3, float f4, float fScalar)		{ writeFloat4Int8( f1 * fScalar, f2 * fScalar, f3 * fScalar, f4 * fScalar); }

	void								writePos2Float( const Position2<float> &Pos, float /* fScalar NOT USED! */ ) 	{ writeFloat2( Pos.X( ), Pos.Y( ) ); }
	void								writePos2FloatInt16( const Position2<float> &Pos, float fScalar) 					{ writeFloat2Int16( Pos.X( ) * fScalar, Pos.Y( ) * fScalar); }
	void								writePos2FloatInt8( const Position2<float> &Pos, float fScalar) 					{ writeFloat2Int8( Pos.X( ) * fScalar, Pos.Y( ) * fScalar); }

	void								writePos3Float( const Position3<float> &Pos, float /* fScalar NOT USED! */)	{ writeFloat3( Pos.X( ), Pos.Y( ), Pos.Z( ) ); }
	void								writePos3FloatInt16( const Position3<float> &Pos, float fScalar) 					{ writeFloat3Int16( Pos.X( ) * fScalar, Pos.Y( ) * fScalar, Pos.Z( ) * fScalar); }
	void								writePos3FloatInt8( const Position3<float> &Pos, float fScalar) 					{ writeFloat3Int8( Pos.X( ) * fScalar, Pos.Y( ) * fScalar, Pos.Z( ) * fScalar); }

	void								writeColorPair5551( const BaseColor<unsigned short> &Col1, const BaseColor<unsigned short> &Col2);

public:
	static const unsigned int	STAGE_IDX_DONT_CHANGE;

	unsigned int 		 			lastGeometryStreamVUMemOffset;
	unsigned int					lastRGBAStreamVUMemOffset;
	unsigned int					lastNormalStreamVUMemOffset;

	VUBufferWrite( unsigned int _VertsPerSet, RGBADataFormat _RGBAFormat, NormalDataFormat _NormalFormat) ;

	unsigned int								getVertsPerSet( void )									{ return d_VertsPerSet; }

	void								setRGBAStorageFormat( RGBADataFormat _fmt)		{ d_RGBADataFormat = _fmt; }
	RGBADataSize					getRGBADataSize( void ) 								{ return extractRGBADataSize( d_RGBADataFormat); }
	RGBAStorageMethod				getRGBAStorageMethod( void )							{ return extractRGBAStorageMethod( d_RGBADataFormat); }

	void								setNormalStorageFormat( NormalDataFormat _fmt)	{ d_NormalDataFormat = _fmt; }
	NormalDataSize					getNormalDataSize( void ) 								{ return extractNormalDataSize( d_NormalDataFormat); }
	NormalStorageMethod			getNormalStorageMethod( void )						{ return extractNormalStorageMethod( d_NormalDataFormat); }

	void								setStageIdxs( int idxClrStage, int idxTCStage, int idxNrmStage);
	void								getStageIdxs( unsigned int &idxClrStage, unsigned int &idxTCStage, unsigned int &idxNrmIdx);

	void								setSpan( unsigned int defSpan) { d_VertexSpan = defSpan; }

	void								setADCValue( unsigned int _ADCValue) { d_ADCValue = _ADCValue; }
	unsigned int					getADCValue( void ) { return d_ADCValue; }

	void								setADCCount( unsigned int _ADCCount) { d_ADCCount = _ADCCount; }
	unsigned int					getADCCount( void ) { return d_ADCCount; }

	void								setVIFControl( VIFControl &defControl) { p_VIFControl = &defControl; p_DMAStream = & defControl.stream( ); }
	void								setProtocol( MeshInterface &defProtocol) { p_Protocol = &defProtocol; }


	// must be overridden
	virtual unsigned int			getVUQWordsPerSet( void ) = 0;

	virtual void					writeBlock( unsigned int &baseOffset, unsigned int baseVert, unsigned int readVerts, unsigned int writeVerts) = 0;
	virtual unsigned int 		calcBlockSize( unsigned int readVerts, unsigned int writeVerts) = 0;

} ;

#endif