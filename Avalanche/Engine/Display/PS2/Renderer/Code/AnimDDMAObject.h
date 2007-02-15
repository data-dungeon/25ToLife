#ifndef __ANIM_DISPLAY_DMA_OBJECT_H
#define __ANIM_DISPLAY_DMA_OBJECT_H

#include "DDMAObject.h"
#include "PS2Renderer.h"

//#define NUM_LIGHT_VECTORS 7
#define NUM_LIGHT_VECTORS 9

class AnimDMAModelHdr;

class AnimDDMAObject : public DDMAObject
{
private:
	enum ALInstType
	{
		AL_INST_FIRST = 0,		// first instruction in VertexData stream
		AL_INST_LAST,				// last instuction in VertexData stream
		AL_INST_OTHER				// any other instruction
	} ;

	static unsigned int 	d_PersistantLit[];
	
	static bool				b_PersistantInited;
	
	unsigned int			d_MatrixBufferFrameID;

	void * 					p_MatrixBuffer;

	void *					p_MatrixBuffers[2];
	void *					p_MatrixDMAChains[2];
	
	void						buildDMAObject( AnimDMAModelHdr &Hdr);
	unsigned int 			fixupAnimLitAddress( unsigned int *pVUInstructionTable, unsigned int inst, ALInstType instType);
	void 						fixupMatrixDMAChain( unsigned int *pMatrixChain, unsigned int *pOriginalChain, void *pMatrixBuffer, unsigned int mtxEntrySize );
	void 						fixupGeometryChain( AnimDMAModelHdr * pDataBuffer, unsigned int instFixupOffset, bool bLit);
	
public:
	inline int				GetBufferIndex()
	{
		return ( p_MatrixBuffer == p_MatrixBuffers[ 0 ] ) ? 0 : 1;
	}

	inline void				SetBufferIndex( int index )
	{
//		ASSERT( index == 0 || index == 1 );
		p_MatrixBuffer = p_MatrixBuffers[ index ];
		p_DMAPtr       = p_MatrixDMAChains[ index ];
	}
	
	void switchBuffers( void )
	{
		SetBufferIndex( 1 - GetBufferIndex() );
	}
	
	bool						b_MatrixBufferLightsSet;
	unsigned int			d_MatrixCount;

	enum
	{
		FLAG_LIGHT = 1,
	} ;
	
	AnimDDMAObject( 
		AnimDMAModelHdr & 			Hdr,

		void *							_Handler = 0,
		DDMAObject *					_Next = 0
	) ;


	~AnimDDMAObject( )
	{
		if(p_MatrixDMAChains[0] < p_MatrixDMAChains[1])
			PS2RendererFree( p_MatrixDMAChains[0]);
		else
			PS2RendererFree( p_MatrixDMAChains[1]);
		p_MatrixBuffer = NULL;
	}

	unsigned int		getMatrixEntrySize( void) const		{ return (d_Flags & FLAG_LIGHT) ? (NUM_LIGHT_VECTORS * 4 * sizeof(float)) : (4 * 4 * sizeof(float)); }
	unsigned int		getMatrixBufferFrameID( void) const	{ return d_MatrixBufferFrameID; }

	void *				getMatrixBuffer( void) const			{ return p_MatrixBuffer; }
	void *				MatrixBuffer( int index ) { return p_MatrixBuffers[ index ]; }

	void * 				activateMatrixBuffer( unsigned int frameID)
	{
		if( frameID != d_MatrixBufferFrameID)
		{
			b_MatrixBufferLightsSet = false;
			d_MatrixBufferFrameID = frameID;
		
			switchBuffers( );
		}

		return p_MatrixBuffer;
	}

	// set one shot overrides for sharing matrix lists
	static void SetSharedMatrices( AnimDDMAObject* sharedMatrixDMAObject )
	{
		s_sharedMatrixDMAObject = sharedMatrixDMAObject;
	}
private:
	static AnimDDMAObject* s_sharedMatrixDMAObject;
} ;


#endif
