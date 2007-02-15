#ifndef __VU_DYNAMIC_MPG_H
#define __VU_DYNAMIC_MPG_H

#include "renderassert.h"
#include "CompilerControl.h"

class VUDynamicMPG
{

protected:
	void *						p_Code;
	unsigned int				d_VUInstructions;

	static const unsigned int maxMPGVUInst;

	VUDynamicMPG(
		void *					_Code,
		unsigned int			_VUInstructions
		); 

public:
	static const unsigned int bytesPerVUInst;
	
	virtual void			addToDMA_MPG( unsigned int vuAddr, bool bFlush = false) = 0;
	unsigned int			instructions( void ) const { return d_VUInstructions; }

	// static helpers
	static VUDynamicMPG *create( void * pCodeStart, void *pCodeEnd);
	static VUDynamicMPG *create( void * pCodeStart, unsigned int sizeCode);
	
	static unsigned int	instructionsInBlock( void * pCodeStart, void * pCodeEnd)
	{
		RENDER_ASSERT( pCodeEnd > pCodeStart);
		
		return (((unsigned int) pCodeEnd) - ((unsigned int) pCodeStart) >> 3);
	}
	
	static unsigned int	isValidCodeAddress( void * pCodeStart)
	{
		return ((unsigned int) pCodeStart & 0x0f) == 0 ? true : false;
	}
	
	
} ;

class VUDynamicMPGSngl : public VUDynamicMPG
{
															 
	unsigned int				d_dmaQWC;
public:
	VUDynamicMPGSngl(
		void *					_Code,
		unsigned int			_VUInstructions
		) : VUDynamicMPG( _Code, _VUInstructions) { d_dmaQWC = (_VUInstructions+1) / 2; }

	void						addToDMA_MPG( unsigned int vuAddr, bool bFlush);

} ;

class VUDynamicMPGMult : public VUDynamicMPG
{

public:
	VUDynamicMPGMult(
		void *					_Code,
		unsigned int			_VUInstructions
		) : VUDynamicMPG( _Code, _VUInstructions) { }

	void 					addToDMA_MPG( unsigned int vuAddr, bool bFlush);

} ;

class VUDynamicMPGPtr
{

	VUDynamicMPG				*p_MPGPtr;
	
public:
	VUDynamicMPGPtr( 
		void *					_CodeBegin,
		void *					_CodeEnd
		)
	{
		p_MPGPtr = VUDynamicMPG::create( _CodeBegin, _CodeEnd);
	}
	
	~VUDynamicMPGPtr( void )
	{
		delete p_MPGPtr;
	}
	
	inline VUDynamicMPG & get( void ) const { return *p_MPGPtr; }
	
} ;


#endif
