#ifndef __WRITE_VST_AND_Z_32_H
#define __WRITE_VST_AND_Z_32_H

#include "VUBufferWrite.h"

class writeVSTandZ32 : public VUBufferWrite
{

	void							writePrimaryGeometryBlock( unsigned int &baseOffset, unsigned int baseVert, unsigned int readVerts);

	void							writeNormalBlock_Column( unsigned int &baseOffset, unsigned int baseVert, unsigned int readVerts);
	void							writeNormalBlock_Row( unsigned int &baseOffset, unsigned int baseVert, unsigned int readVerts);
	void							writeNormalBlock_RowCompact( unsigned int &baseOffset, unsigned int baseVert, unsigned int readVerts);

	void 							writeRGBABlock_8888( unsigned int &baseOffset, unsigned int baseVert, unsigned int readVerts);
	void 							writeRGBABlock_5551( unsigned int &baseOffset, unsigned int baseVert, unsigned int readVerts);

	void 							writeZ4andADC( float fZ1, float fZ2, float fZ3, float fZ4, unsigned int ADCBits);
	void 							writeSTandXY( const Position2<float> &TC, const Position3<float> &Pos);
	void 							writeSTandXYandCompRGB32( const Position2<float> &TC, const Position3<float> &Pos, const BaseColor<unsigned short> &Clr);

public:
	writeVSTandZ32( void );
	
	virtual unsigned int		getVUQWordsPerSet( void );
	virtual void				writeBlock( unsigned int &baseOffset, unsigned int baseVert, unsigned int readVerts, unsigned int writeVerts);
	virtual unsigned int 	calcBlockSize( unsigned int readVerts, unsigned int writeVerts);

} ;


#endif

