#ifndef __DMA_OBJECT_H
#define __DMA_OBJECT_H

#include "PRIMReg.h"
#include "DDMAObjectClassIDs.h"

#ifndef PS2
#include <malloc.h>
#define PS2RendererFree free
#else
extern void PS2RendererFree(void *block);
#endif

class DMAObject
{
public:

protected:
	DMAObject *					p_Next;

	unsigned int				d_DMASize;
	void *						p_DMAData;

public:
	PRIMReg						d_PRIMReg;

	const unsigned int		d_Vertices;
	const unsigned int		d_ObjectClass;
	const unsigned int		d_ObjectFlags;

	DMAObject(
		const PRIMReg &		_PRIMReg,
		DMAObject *				_Next,
		unsigned int			_Vertices,
		unsigned int 			_ObjectClass,
		unsigned int			_ObjectFlags,
		unsigned int			_DMASize,
		void *					_DMAData
		) :
	d_PRIMReg( _PRIMReg),
	d_Vertices( _Vertices),
	d_ObjectClass( _ObjectClass),
	d_ObjectFlags( _ObjectFlags)

	{
		p_Next = _Next;
		d_DMASize = _DMASize;
		p_DMAData = _DMAData;
	}

	~DMAObject()
	{
		if(d_ObjectFlags & OBJ_FLAG_OWNS_DMADATA){
			PS2RendererFree(p_DMAData);
		}
	}


	inline DMAObject *	next( void ) 				{ return p_Next; }
	inline void *			getDMAData( void ) 		{ return p_DMAData; }

	inline const DMAObject *	next( void ) const				{ return p_Next; }
	inline const void *			getDMAData( void ) const		{ return p_DMAData; }
	inline       unsigned int	getDMASize( void ) const		{ return d_DMASize; }
} ;

#endif