#ifndef __VU_CODE_SET_H
#define __VU_CODE_SET_H

#include "VUManagedFunction.h"

#define SETUP_MAX_EXTRA_PASSES	2

enum SendFlags
{
	VU_SEND_MTX_CLIP_SCREEN			= 0x00000001,
	VU_SEND_MTX_TEXTURE_4x4			= 0x00000002,	// 4x4 Texture Mtx (Mtx * Vtx)
	VU_SEND_MTX_TEXTURE_3x4			= 0x00000004,	// 3x4 Texture Mtx (Mtx * ST )
	
	VU_SEND_DATA_FOG					= 0x00000010,	// Base Pass

	VU_SEND_BLOCK_ALL					= 0x00000100,
	VU_SEND_BLOCK_VTP					= 0x00000200,
	VU_SEND_BLOCK_SHADOW_MAP		= 0x00000400,
	
	VU_SEND_BLOCK_ALL_ALPHA_EARLY	= 0x00008000,	// Misc

// Secondary Pass Data
	VU_SEND_DATA_PLIGHTS				= 0x00010000,	
	VU_SEND_DATA_DLIGHTS				= 0x00020000,	
	VU_SEND_DATA_ALIGHT				= 0x00040000,	

	VU_SEND_BLOCK_ENV_MAP			= 0x00100000,	// Secondary Passes
	VU_SEND_BLOCK_SPHERE_MAP		= 0x00200000
} ;

#define VU_SEND_DATA_BASE_MASK 	0x0000ffff
#define VU_SEND_DATA_PASS_MASK	0xffff0000
#define VU_SEND_DATA_LIGHTS 		(VU_SEND_DATA_PLIGHTS | VU_SEND_DATA_DLIGHTS | VU_SEND_DATA_ALIGHT)

enum InfoFlags
{
	VU_USES_STD_CLIPPER				= 0x00000001,
	VU_USES_ALT_CLIPPER			 	= 0x00000002,
		
	VU_USES_XYZF2						= 0x00000004,
	VU_USES_MODEL_BASE_OFFSET 		= 0x00000008,

	VU_USES_REG_VERTEX				= 0x00000010,
	
	VU_INFO_CORE						= 0x00010000,
	VU_USES_CORE						= 0x00020000
	
} ;

#define VU_USES_CLIPPER_MASK		( VU_USES_STD_CLIPPER | VU_USES_ALT_CLIPPER )

typedef struct _VUCodeEntry
{
	struct _VUCodeEntry *	pParent;
	
	VUManagedFunction *		pFunc;
	unsigned int				sendFlags;
	unsigned int				infoFlags;
	unsigned int				reqFlags;
	
	unsigned int 				extraPRIMBits;
} VUCodeEntry;
	

typedef struct _VUCodeSet
{
	VUCodeEntry *				pXfrm;
	VUCodeEntry *				pCull;
	VUCodeEntry *				pClip;
} VUCodeSet;

typedef struct _VUClipperSet
{
	VUManagedCode & 			Code;
	
	VUManagedFunction * 		pSetup;
	VUManagedFunction * 		pClip;
} VUClipperSet;
	
typedef struct _VUCodeSetup
{
	void *						pCachedModelPtr;
	
	unsigned int 				sendFlagsAll;
	unsigned int				infoFlagsAll;
	unsigned int				reqFlagsAll;
	
	unsigned int				numExtraPasses;
	
	VUCodeEntry *				pBasePass;
	VUCodeEntry *				pExtraPasses[SETUP_MAX_EXTRA_PASSES];
	
} VUCodeSetup;

enum CodeSetArrayIdxs
{
	CSET_BASIC = 0,
	CSET_BASIC_MP,
	CSET_ALL,
	CSET_ALL_MP,
	
	CSET_DLIGHT,
	CSET_DLIGHT_MP,
	CSET_PLIGHT,
	CSET_PLIGHT_MP,
	CSET_ALIGHT,
	CSET_ALIGHT_MP,
	
	CSET_CORE,
	CSET_CORE_MP,
	
	CSET_SHADOWED,
	CSET_VERTEX_TEXTURE_PROJECTION,
	CSET_SHADOWER
} ;

enum CodeEntArrayIndxs
{
	CENT_ENVMAP = 0,
	CENT_SPHEREMAP,
	CENT_ENVREFMAP,
	CENT_SPHEREREFMAP,
} ;

#endif