#ifndef __XBOX_H
#define __XBOX_H


#define D3DCOMMON_TYPE_SHIFT         16
#define D3DCOMMON_TYPE_VERTEXBUFFER  0x00000000
#define D3DCOMMON_TYPE_INDEXBUFFER   0x00010000
#define D3DCOMMON_TYPE_PALETTE       0x00030000
#define D3DCOMMON_TYPE_TEXTURE       0x00040000
#define D3DCOMMON_TYPE_SURFACE       0x00050000
#define D3DCOMMON_VIDEOMEMORY        0x00800000

#define D3DVERTEXBUFFER_ALIGNMENT     4

// VertexBuffer Format
typedef struct 
{
    DWORD Common;
    DWORD Data;
    DWORD Lock;
} D3DVertexBuffer;

#define D3DINDEXBUFFER_ALIGNMENT     4

// IndexBuffer Format
typedef struct 
{
    DWORD Common;
    DWORD Data;
    DWORD Lock;
} D3DIndexBuffer;

// Vertex attribute formats
#define XD3DVSDT_FLOAT1      0x00    // 1D float expanded to (value, 0., 0., 1.)
#define XD3DVSDT_FLOAT2      0x01    // 2D float expanded to (value, value, 0., 1.)
#define XD3DVSDT_FLOAT3      0x02    // 3D float expanded to (value, value, value, 1.)
#define XD3DVSDT_FLOAT4      0x03    // 4D float
#define XD3DVSDT_D3DCOLOR    0x04    // 4D packed unsigned bytes mapped to 0. to 1. range
#define XD3DVSDT_UNUSED      0x05    // Unused - placeholder
#define XD3DVSDT_SHORT2      0x06    // 2D signed short expanded to (value, value, 0., 1.)
#define XD3DVSDT_SHORT4      0x07    // 4D signed short
#define XD3DVSDT_NORMSHORT1  0x08    // 1D signed, normalized short expanded to (value, 0, 0., 1.)
#define XD3DVSDT_NORMSHORT2  0x09    // 2D signed, normalized short expanded to (value, value, 0., 1.)
#define XD3DVSDT_NORMSHORT3  0x0A    // 3D signed, normalized short expanded to (value, value, value, 1.)  
#define XD3DVSDT_NORMSHORT4  0x0B    // 4D signed, normalized short expanded to (value, value, value, value)  
#define XD3DVSDT_NORMPACKED3 0x0C    // 3 signed, normalized components packed in 32-bits.  (11,11,10).  
#define XD3DVSDT_SHORT1      0x0D    // 1D signed short expanded to (value, 0., 0., 1.)  
#define XD3DVSDT_SHORT3      0x0E    // 3D signed short expanded to (value, value, value, 1.)
#define XD3DVSDT_PBYTE1      0x0F    // 1D packed byte expanded to (value, 0., 0., 1.)  
#define XD3DVSDT_PBYTE2      0x10    // 2D packed byte expanded to (value, value, 0., 1.)
#define XD3DVSDT_PBYTE3      0x11    // 3D packed byte expanded to (value, value, value, 1.)
#define XD3DVSDT_PBYTE4      0x12    // 4D packed byte expanded to (value, value, value, value) 
#define XD3DVSDT_FLOAT2H     0x13    // 2D homogeneous float expanded to (value, value,0., value.)


#endif
