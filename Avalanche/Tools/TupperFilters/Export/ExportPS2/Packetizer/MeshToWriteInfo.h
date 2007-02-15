#ifndef __MESH_TO_WRITE_INFO_H
#define __MESH_TO_WRITE_INFO_H

#include "MeshInterface.h"
#include "WriteModeTypes.h"
#include "StatesWriteInfo.h"

class MeshToWriteInfo
{
	typedef struct _SortElement
	{
		struct _SortElement *	pNext;
		void *						pElement;
	} SortElement;

	SortElement *					p_SortList;

	unsigned int					d_SortElementSize;
	unsigned char *				p_SortBuffer;

protected:
	static unsigned int			d_RGBASourceBits, d_AlphaSourceBits;

	static RGBADataSize			d_DefRGBASize;
	static RGBAStorageMethod	d_DefRGBAStorage;

	static NormalDataSize		d_DefNormalSize;
	static NormalStorageMethod	d_DefNormalStorage;

	void								sortSetup( unsigned int sizeElement);
	void								sortAddPointer( void * pSortElement);
	void								sortCleanup( void );
	void								SortElements( void );

	void								SortElementList( unsigned int numElements, void *pElementList); 

	virtual int						sortCompareFunction( void *pElement1, void *pElement2) = 0;
	
public:
	MeshToWriteInfo( void )
	{
		p_SortList = 0;
		d_SortElementSize = 0;
	}

	~MeshToWriteInfo( )
	{
		sortCleanup( );
	}

	static void						setRGBSourceBits( unsigned int _RGBASourceBits)								{ d_RGBASourceBits = _RGBASourceBits; }
	static void						setAlphaSourceBits( unsigned int _AlphaSourceBits)							{ d_AlphaSourceBits = _AlphaSourceBits; }

	static void						setDefaultRGBADataSize( RGBADataSize _RGBASize)			  					{ d_DefRGBASize = _RGBASize; }
	static RGBADataSize			getDefaultRGBADataSize( void ) 								  					{ return d_DefRGBASize; }

	static void						setDefaultRGBAStorageMethod( RGBAStorageMethod _RGBAStorage) 			{ d_DefRGBAStorage = _RGBAStorage; }
	static RGBAStorageMethod	getDefaultRGBAStorageMethod( void )				  			  					{ return d_DefRGBAStorage; }

	static void						setDefaultNormalDataSize( NormalDataSize _NormalSize)		  				{ d_DefNormalSize = _NormalSize; }
	static NormalDataSize		getDefaultNormalDataSize( void )								  					{ return d_DefNormalSize; }

	static void						setDefaultNormalStorageMethod( NormalStorageMethod _NormalStorage) 	{ d_DefNormalStorage = _NormalStorage; }
	static NormalStorageMethod	getDefaultNormalStorageMethod( void )				  			  				{ return d_DefNormalStorage; }

	unsigned int					countTotalPasses( MeshInterface &Protocol)
	{
		unsigned int				totalPasses = 0;

		for( unsigned int i = 0; i < Protocol.numStates( ) ; i++)
			totalPasses += Protocol.numPasses( i);

		return totalPasses;
	}

	// work function
	virtual void					freeStatesWriteInfo( StatesWriteInfo *pWriteInfo, unsigned int numStates) = 0;
	virtual StatesWriteInfo * 	buildStatesWriteInfo( MeshInterface &Protocol, unsigned int &returnStateCount, unsigned int &modelHasFlags) = 0;
} ;

#endif

