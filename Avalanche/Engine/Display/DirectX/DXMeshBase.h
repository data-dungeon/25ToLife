#ifndef __DX_MESH_BASE_H
#define __DX_MESH_BASE_H

#include <d3d8.h>
#include <d3dx8.h>

#include "FileRecords.h"
#include "DXStreamDescriptor.h"

#include "DynamicIB.h"
#include "DynamicVB.h"

extern void setMaterial( unsigned int textureContext, unsigned int flags );

typedef struct _DXSubMesh
{
	SubMeshRecord *				p_SubMeshRecord;
} DXSubMesh;


typedef struct _MatrixTransfer
{
	unsigned char					m_Index; 
	unsigned char					m_Slot;
} MatrixTransfer;

typedef struct _MatrixList
{
	unsigned int					m_NumTransfers;
	MatrixTransfer *				a_MatrixList;
} MatrixList;

typedef struct _DXVertexStream
{
	DynamicVB *						p_VB;

	void *							p_StreamData;

	unsigned int					m_NumVertices;
	unsigned int					m_VertexStride;
	unsigned int					m_StreamBytes;
} DXVertexStream;

typedef struct _DXIndexStream
{
	DynamicIB *						p_IB;
	D3DFORMAT						m_IndexFormat;
	
	void *							p_IndexData;	
	unsigned int					m_NumIndices;
} DXIndexStream;

typedef struct _DXSingleStream
{
	DXStreamDescriptor			m_Desc;

	unsigned int					m_NumSubMeshes;
	DXSubMesh **					ap_SubMeshes;

	DXVertexStream *				p_Stream;
} DXSingleStream;


typedef struct _DXStreamPair
{
	unsigned int					m_DXStream;
	DXVertexStream *				p_Stream;
} DXStreamPair;

typedef struct _DXMultiStream
{
	DXStreamDescriptor			m_Desc;

	unsigned int					m_NumSubMeshes;
	DXSubMesh **					ap_SubMeshes;

	unsigned int					m_NumStreamPairs;
	DXStreamPair **				p_StreamPairs;
} DXMultiStream;

class DXMeshBase
{

	static DynamicVB *	p_UserVB;
	static DynamicIB *	p_UserIB16;
	static DynamicIB *	p_UserIB32;

	static void *			p_LastIB;

	static void			createDeviceObjects( void );
	static void			deleteDeviceObjects( void );

protected:
	typedef struct _DXStreamRenderInfo
	{
		DXStreamDescriptor *			p_Desc;

		unsigned int					m_NumSubMeshes;
		DXSubMesh **					ap_SubMeshes;
	} DXStreamRenderInfo;

	static IDirect3DDevice8 *	pDXDevice;

	unsigned int		m_DIPBaseVertex;
	unsigned int		m_DIPBaseIndex;

	void 					initDXVertexStream( VertexDataRecord & VBData, char * pBase, DXVertexStream & DXData );
	DynamicVB *			loadDXVertexStream( DXVertexStream & DXStream, bool bNeedZeroBase = false);

	void 					initDXIndexStream( IndexDataRecord & IBData, char * pBase, DXIndexStream & DXData );
	DynamicIB *			loadDXIndexStream( DXIndexStream & DXStream );	

	void					loadMatrixList( const D3DXMATRIX * pMatrices, D3DXMATRIX * pOptTransform, MatrixList & list, unsigned int baseSlot );

	DynamicVB *			getUserVB( unsigned int vbIdx )
	{ 
		return p_UserVB; 
	}

	DynamicIB *			getUserIB( D3DFORMAT fmt )
	{
		#if !defined (_XBOX)
		ASSERT( fmt == D3DFMT_INDEX16  ||  fmt == D3DFMT_INDEX32 );
		#else
		ASSERT( fmt == D3DFMT_INDEX16 );			// INDEX32 not available on XBOX
		#endif

		return fmt == D3DFMT_INDEX16 ? p_UserIB16 : p_UserIB32;
	}

	void					createStreamDescriptor( void * pDescriptorData, void * & pDescriptor );
	void					releaseStreamDescriptor( void * pDescriptor );

	void					setStreamSource( unsigned int stream, DynamicVB & VB, unsigned int offset, unsigned int stride );
	void					setIndices( DynamicIB & IB );
	void					setVertexDeclaration( void * pDeclaration );

public:
	static unsigned int				numUserIB16Indices;
	static unsigned int				numUserIB32Indices;
	static unsigned int				numUserVBVertexBytes;

	static void				initDXDevice( IDirect3DDevice8 * pDXDevice );
	static void				releaseDXDevice( void ); 

	static void				releaseDeviceObjects( void );
	static void				restoreDeviceObjects( void );

	void					setVertexShader( unsigned int FVF );
	void					setVertexShader( void * pShader );

	void					callDIP( DXDIPData & DIPData );
	inline void			endRendering( void )
	{
		*( (int *) & m_DIPBaseVertex) = -1;
		*( (int *) & m_DIPBaseIndex) = -1;
	}


} ;

#endif __DX_MESH_H
