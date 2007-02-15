#ifndef __DX_FILE_RECORDS_H
#define __DX_FILE_RECORDS_H

#include "ShaderRecords.h"

typedef struct _MeshRecord
{
	unsigned int				d_MeshFlags;

	unsigned int				d_NumIDRecords;
	unsigned int				d_OffsetIDRecords;

	unsigned int				d_NumVDRecords;
	unsigned int				d_OffsetVDRecords;

	unsigned int				d_NumMSRecords;
	unsigned int				d_OffsetMSRecords;

	unsigned int				d_NumSSRecords;
	unsigned int				d_OffsetSSRecords;

	unsigned int				d_MatrixCacheSize;
	unsigned int				d_MatrixIndexMax;

	unsigned int				d_NumMLRecords;
	unsigned int				d_OffsetMLRecords;

	unsigned int				d_NumShaderRecords;
	unsigned int				d_OffsetShaderRecords;

	unsigned int				d_NumSubMeshRecords;
	unsigned int				d_OffsetSubMeshRecords;
} MeshRecord;
		
typedef struct _IndexDataRecord
{
	unsigned int				d_OffsetIndexData;

	unsigned int				d_NumIndices;
	unsigned int				d_CIBFormat;
} IndexDataRecord;

// IBData1 < unsigned short idx >
// 	IBEntry1
//		IBEntry2
//		...
//
// IBData2 < unsigned short idx >
// 	IBEntry1
//		IBEntry2
//		...
//

typedef struct _VertexDataRecord
{
	unsigned int				d_OffsetVertexData;

	unsigned int				d_NumVertices;
	unsigned int				d_VertexBytes;
	unsigned int				d_BufferBytes;
} VertexDataRecord;

// VBData1 < ... >
//		Vertex1
//		Vertex2
//		...
//
// VBData2 < .... >
//		Vertex1
//		Vertex2
//		...
//

typedef struct _SSDataRecord
{
	unsigned int				d_StreamFlags;
	unsigned int				d_FVF;

	unsigned int				d_OffsetDescriptor;
	unsigned int				d_NumSubMeshes;

	unsigned int				d_VBDataIndex;
} SSDataRecord;

// a_VertexDescriptor		< D3DVERTEXELEMENT9[] >
// ...
//

typedef struct _MSDataRecord
{
	unsigned int				d_StreamFlags;
	unsigned int				d_FVF;
	unsigned int				d_OffsetDescriptor;
	unsigned int				d_NumSubMeshes;

	unsigned int				d_NumStreamSets;
	unsigned int				d_OffsetStreamSetData;
} MSDataRecord;

// StreamSet < unsigned int stream, unsigned int VBDataIdx >
// ...

// a_VertexDescriptor		< D3DVERTEXELEMENT9[] >
// ...
//

typedef struct _MLDataRecord
{
	unsigned int				d_NumMatrices;

	unsigned int				d_OffsetML;
} MLDataRecord;

// MLData1 < unsigned short idx, unsigned short slot >
//		Idx1, Slot1
//		Idx2, Slot2
//		...
//

// MLData2 < unsigned short idx, unsigned short slot >
//		Idx1, Slot1
//		Idx2, Slot2
//		...
//

// ShaderIndices < unsigned int idx >
// 	ShaderIndex1
//		ShaderIndex2
//		...
//

typedef struct _DXDIPData
{
	unsigned int				d_IBBase;

	unsigned int				d_DIPMinIndex;
	unsigned int				d_DIPNumVertices;
	unsigned int				d_DIPPrimitiveCount;
	unsigned int				d_DIPType;
} DXDIPData;

typedef struct _SubMeshRecord
{
	unsigned int				d_SubMeshFlags;

	unsigned int				d_ShaderListIndex;
	
	unsigned int				d_VertexStreamIndex;
	unsigned int				d_IndexStreamIndex;

	int							d_MatrixListIndex;

	DXDIPData					d_DIPData;
} SubMeshRecord;

#endif
