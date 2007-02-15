#ifndef __DISPLAY_DMA_OBJECT_H
#define __DISPLAY_DMA_OBJECT_H

#include "../Common/DDMAObjectClassIDs.h"
#include "../Common/PRIMReg.h"
#include "PS2Renderer.h"

class DMAModelHdr;

using namespace PS2Renderer;

class DDMAObject
{
private:
	DDMAObject *			p_Next;
	void *					p_Handler;	
	
	unsigned int			d_DMASize;
	unsigned int			d_VertCount;
	DDMAObject_DCID		d_ClassID;
	
protected:
	static TextureBindCallback* p_TextureBindCallback;
	void *					p_DMAPtr;
	void *					p_DTTableData;

public:
	enum _ObjectFlags
	{
		OBJ_FLAG_VERTEX_COLORS		= 0x00000001,
		OBJ_FLAG_TEX_COORDS			= 0x00000002,
		OBJ_FLAG_VERTEX_NORMALS		= 0x00000004,

		OBJ_FLAG_ENV_MAPPED			= 0x00000010,

		OBJ_FLAG_OWNS_DMA_DATA		= 0x00200000,

		OBJ_FLAG_VU_ALLOC_MASK		= 0xff000000
	} ;

	unsigned int			d_Flags;

	PRIMReg					d_PRIMReg;
	
	DDMAObject(
		DDMAObject_DCID		_ClassID,
		unsigned int		_Flags,
		const PRIMReg		_PRIMReg,
		void *				_DMAPtr,
		unsigned int		_DMASize,
		void *				_DTTableData,
		unsigned int		_VertCount,
		void *				_Handler = 0,
		DDMAObject *		_Next = 0
	) ;

	DDMAObject( 
		const DMAModelHdr & Hdr, 
		unsigned int 		_ExtraFlags, 
		void * 				_DMAPtr,
		unsigned int 		_DMASize,
		void *				_DTTableData,
		void *				_Handler = 0,
		DDMAObject *  		_Next = 0
	) ;

	virtual ~DDMAObject();

	inline unsigned int		getVertCount( void ) const		{ return d_VertCount; }
	inline void *				getDMAPtr( void ) const			{ return p_DMAPtr; }
	inline unsigned int		getDMASize( void ) const		{ return d_DMASize; }
	inline void *				getDTTableData( void ) const	{ return p_DTTableData; }
	inline unsigned int		getClassID( void ) const		{ return d_ClassID; }
	inline const PRIMReg	&	getPRIMReg( void ) const		{ return d_PRIMReg; }
	
	void							next( DDMAObject * _Next)		{ p_Next = _Next; }
	DDMAObject *				next( void ) const				{ return p_Next; }

	void							handler( void * _Handler)		{ p_Handler = _Handler; }
	void *						handler( void ) const			{ return p_Handler; }
	
	static void					standardFixup( DMAModelHdr *pHdr);

	static void							setTextureBindCallback( TextureBindCallback* _TextureBindCallback) 
		{ p_TextureBindCallback = _TextureBindCallback; }

	static TextureBindCallback* 	getTextureBindCallback( )						
		{ return p_TextureBindCallback; }

	void							reInit( 
		DDMAObject_DCID		_ClassID,
		unsigned int		_Flags,
		const PRIMReg		_PRIMReg,
		void *				_DMAPtr,
		unsigned int		_DMASize,
		void *				_DTTableData,
		unsigned int		_VertCount,
		void *				_Handler = 0,
		DDMAObject *		_Next = 0
	) ;
	
} ;

#endif
