#ifndef __T_SOM_H
#define __T_SOM_H

#include <t_types.h>
#include <t_struct.h>

typedef unsigned long	t_TexID;

/*-----------------9/16/98 3:33PM-------------------
 * Single Object Model (SOM) Basic Polygonal Model for
 * display code.
 * --------------------------------------------------*/

typedef struct SOMMultTex
{
	t_TexID				TexID;
	ts_f2DCoord			TexUV[4];
} ts_SOMMultTex;

/*-----------------9/16/98 3:34PM-------------------
 * Packed SOMFace
 * --------------------------------------------------*/
#define SOMF_SNGL_TEXTURE			0x00000001
#define SOMF_MULT_TEXTURE			0x00000002

#define SOMF_SOME_TEXTURE			(SOMF_SNGL_TEXTURE | SOMF_MULT_TEXTURE)

#define SOMF_TRIANGLE				0x00000004
#define SOMF_TRISTRIP				0x00000008

#define SOMF_CLR_PACKED				0x00000010
#define SOMF_CLR_SINGLE				0x00000020
#define SOMF_CLR_VERTEX				0x00000040

#define SOMF_CLR_FLOAT				0x00000080

#define SOMF_UV_BYTE					0x00000100

#define SOMF_NORMAL					0x00001000

typedef struct SOMFace
{
	t_lflagbits			Flags;     			 //  4
	union
	{
		unsigned long	ulTexCount;			 // If SOMF_MULT_TEXTURE
		t_TexID			TexID;				 // If SOMF_SNGL_TEXTURE
		ts_bRGBA			PackedColor;		 // If SOMF_CLR_PACKED
	} ;                                  // 4

// if Flags & SOMF_TRIANGLE
// 	VERTS = 3
// else
//		VERTS = 4
//
//	unsigned short		usVtxIdxs[VERTS];		 //  8 (4 * 2)
//
// if Flags & SOMF_CLR_SINGLE
//   if Flags & SOMF_CLR_BYTE  &&
//	   	ts_bRGBA			Color;			 // 4
//  	if Flags * SOMF_CLR_FLOAT
//	   	ts_fRGBA			Color;			 // 12 (3 * 4)
// if Flags & SOMF_CLR_VERTEX
//   if Flags & SOMF_CLR_BYTE  &&
//	   	ts_bRGBA			VtxColor[VERTS];// 4
//  	if Flags * SOMF_CLR_FLOAT
//	   	ts_fRGBA			VtxColor[VERTS];// 12 (3 * 4)
//
// if Flags & SOMF_NORMAL
// 	ts_f3DCoord			Normal;			 // 12 (3 * 4)
//
// if Flags & SOMF_SNGL_TEXTURE
//		ts_f2DCoord			TexUV[VERTS];   // VERTS * 8
// if Flags & SOMF_MULT_TEXTURE
//	 	t_TexID				ulTexID; 		 //  4
//		ts_f2DCoord			TexUV[VERTS];   // VERTS * 8
//     .
//     .
//
} ts_SOMFace;


/*-----------------9/16/98 3:43PM-------------------
 * Unpacked SOMFace
 * --------------------------------------------------*/
typedef struct SOMFace_UP
{
	t_lflagbits			Flags;     			 //  4
	unsigned short		usVtxIdxs[4];		 //  8 (4 * 2)

	unsigned long		ulTexCount;			 // If SOMF_MULT_TEXTURE
	ts_fRGBA				Color;		 		 // If SOMF_CLR_SINGLE

// if Flags & SOMF_TRIANGLE
// 	VERTS = 3
// else
//		VERTS = 4

// if Flags & SOMF_NORMAL
 	ts_f3DCoord			Normal;			 	 // 12 (3 * 4)

// if Flags & SOMF_CLR_VERTEX
 	ts_fRGBA				VtxColor[4]; 		 // 64 (4 * 4 * 4)

// if Flags & SOMF_VTX_NORMALS
	ts_f3DCoord			VtxNormal[4];	 	 // VERTS * 12

// if Flags & SOMF_MULT_TEXTURE ||
//		Flags & SOMF_SNGL_TEXTURE
	ts_SOMMultTex		*pTextures;
} ts_SOMFaceUP;


/*-----------------9/16/98 3:35PM-------------------
 * SOMModel - Packed Faces - Game Model
 * --------------------------------------------------*/
#define SOMM_SET_FACE					0x00000001
#define SOMM_FACE_NORMALS				0x00000010
#define SOMM_VTX_NORMALS				0x00000020

typedef struct SOMModel
{
	t_lflagbits			ModelFlags;
	ts_Sphere			BoundingSphere;

	unsigned long		ulVtxCount;
	unsigned long		ulFaceCount;

	union
	{
		unsigned long		ulFaceBufferSize;
		unsigned long		ulSetFaceSize;
	} ;

	ts_f3DCoord			*pVtxBuffer;
	ts_SOMFace			*pFaceBuffer;

	ts_f3DCoord			*pNormals;
} ts_SOM;

/*-----------------9/16/98 5:30PM-------------------
 * SOMModelUP - Unpacked Faces - Tool Model
 * --------------------------------------------------*/
typedef struct SOMModel_UP
{
	t_lflagbits			ModelFlags;
	ts_Sphere			BoundingSphere;

	unsigned long		ulVtxCount;
	unsigned long		ulFaceCount;

	ts_f3DCoord			*pVtxBuffer;
	ts_SOMFaceUP		*pFaceBuffer;

	ts_f3DCoord			*pNormals;
} ts_SOMUP;

#endif
