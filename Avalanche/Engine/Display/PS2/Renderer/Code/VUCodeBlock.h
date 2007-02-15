#ifndef __VU_CODE_BLOCK_H
#define __VU_CODE_BLOCK_H

#include "renderassert.h"

class VUCodeBlock
{

protected:
	void *						p_Begin;
	void *						p_End;
	
public:
	VUCodeBlock(
		void *					_CodeBegin,
		void *					_CodeEnd
		) :
	p_Begin( _CodeBegin),
	p_End( _CodeEnd)
	{
	
	}
	
	void *					baseAddr( void ) const { return p_Begin; }
	
	unsigned int			sizeBytes( void ) const { return ((unsigned int) p_End - (unsigned int) p_Begin); }
	unsigned int			sizeInstructions( void ) const { return ((unsigned int) p_End - (unsigned int) p_Begin) >> 3; }
	
	
	bool						operator == ( const VUCodeBlock &Other) const
	{
		return ( (p_Begin == Other.p_Begin)  &&  (p_End == Other.p_End) );
	}
	
	inline bool				inBlock( void *pAddr) const
	{
		return ((pAddr >= p_Begin)  &&  (pAddr < p_End));
	}

	bool						isValidEntry( void *pEntry) const
	{
		RENDER_ASSERT( inBlock( pEntry) );
		
		return (((unsigned int) pEntry - (unsigned int) p_Begin) % 8) ? false : true;
	}
	
	unsigned int			offsetInstructions( void *pEntry) const
	{
		RENDER_ASSERT( isValidEntry( pEntry) );
		
		return ((unsigned int ) pEntry - (unsigned int) p_Begin) >> 3;
	}
	
	// static helpers
	static unsigned int	isValidInstructionOffset( void *pEntry, void *pBegin)
	{
		RENDER_ASSERT( pEntry >= pBegin);
		
		return (((unsigned int) pEntry - (unsigned int) pBegin) % 8) ? false : true;
	}
	
	static unsigned int 	offsetInstructions( void *pEntry, void *pBegin)
	{
		RENDER_ASSERT( isValidInstructionOffset( pEntry, pBegin) );
		
		return ((unsigned int ) pEntry - (unsigned int) pBegin) >> 3;
	}
	
} ;

#endif


