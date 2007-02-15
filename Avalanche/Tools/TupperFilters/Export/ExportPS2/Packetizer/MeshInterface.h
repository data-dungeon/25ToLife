#ifndef __MESH_INTERFACE_H
#define __MESH_INTERFACE_H

#include <assert.h>

#include "Display/PS2/Renderer/Common/Position2.h"
#include "Display/PS2/Renderer/Common/Position3.h"
#include "Display/PS2/Renderer/Common/Color.h"
#include "Display/PS2/Renderer/Common/MeshPassInfo.h"
#include "VUStateProtocol.h"

#undef min
#undef max

class MeshInterface
{

protected:
	int						d_ActiveStateIndex;
	int						d_ActivePassIndex;

public:
	static const unsigned int IDX_MASK;
	static const unsigned int IDX_END_OF_STRIP;
	
	MeshInterface( ) : d_ActiveStateIndex( -1), d_ActivePassIndex( -1) 
	{
	}

	virtual VUStateProtocol & 		peekVUState( unsigned int uiState, unsigned int uiPass ) = 0;
	virtual MeshPassInfo &	  		peekPassInfo( unsigned int uiState, unsigned int uiPass ) = 0;

	virtual VUStateProtocol &				activateState( unsigned int uiState, unsigned int uiPass ) = 0;

	int											getActiveState( void ) const				{ return d_ActiveStateIndex; }
	int											getActivePass( void ) const				{ return d_ActivePassIndex; }			

	VUStateProtocol &							getActiveVUState( void ) 					{ return peekVUState( d_ActiveStateIndex, d_ActivePassIndex) ; }
	MeshPassInfo &								getActivePassInfo( void ) 		 			{ return peekPassInfo( d_ActiveStateIndex, d_ActivePassIndex) ; }

// Ideally, these would be pure virtuals, because you should derive from
// VtxProtocol to use it, but if they were, every derived class would have
// to implement every function.  So, a protocol that doesn't provide matrices
// would still have to implement matrixidx() and matrixcount()

// This way, you can implement just the functions you actually need, and
// rely on defaults for the rest.
	virtual unsigned int						numVertices( void ) const;
	virtual unsigned int						numStates( void ) const;
	virtual unsigned short					numPasses( unsigned int uiState ) const;
	virtual const unsigned int				numColorSets( void) const;					// TJC - get the # of valid color sets for this model
	virtual void 								enableColorSet( unsigned int uiSet);	// JLB - enable a color set.
	virtual const unsigned int				numRequiredAlphaBits( void) const; 	 	// TJC - get the minimum # of alpha bits required (should currently always return 1 or 8, default is 8)

	virtual const	unsigned int			idx( unsigned int uivtx) const;
	virtual const Position3<float> &		vtx( unsigned int uivtx) const;
	virtual const Position3<float> &		nrm( unsigned short usStreamIdx, unsigned int uivtx) const;
	virtual const Position2<float> &		uv( unsigned short usStreamIdx, unsigned int uivtx) const;
	virtual const Position2<float> &		st( unsigned short usStreamIdx, unsigned int uivtx) const;
	virtual const BaseColor<unsigned short> &	clr( unsigned short usStreamIdx, unsigned int uivtx) const;	

	// matrices
	virtual const unsigned int				numMatrices( void) const;
	virtual const unsigned int				idxMatrix( unsigned int uivtx) const;

	// information about the object
	virtual const Position3<float> 		min( void ) const;
	virtual const Position3<float> 		max( void ) const;
	virtual const Position3<float> 		dim( void ) const;
		
} ;

#endif