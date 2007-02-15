#include "VUManagedCode.h"
#include "VUManagedFunction.h"

#include "MasterCodeFactory.h"

#include "PS2Renderer.h"

#include "VUCode.h"
#include "VUCodeSet.h"

// GenericClip Code / Functions
VUManagedCode				&GenericClip = MasterCodeFactory::createManagedCode( &GenericClip_CodeBegin, &GenericClip_CodeEnd);

VUManagedFunction			&gcSetup = MasterCodeFactory::createManagedFunction( &GC_ClipTriangleSetup);
VUManagedFunction			&gcClip = MasterCodeFactory::createManagedFunction( &GC_ClipTriangle);

/*---------------------------------------------------------------------------------------------------------------------------*
 *	Static (Compressed Color)
 *---------------------------------------------------------------------------------------------------------------------------*/
 
// Basic
VUManagedCode				&StaticCC = MasterCodeFactory::createManagedCode( &dbStaticCompress2_CodeBegin, &dbStaticCompress2_CodeEnd);	
VUManagedFunction 		&StaticCC_Entry = MasterCodeFactory::createManagedFunction( &dbStaticCompress2_CodeBegin);

VUManagedCode				&StaticCC_Cull = MasterCodeFactory::createManagedCode( &dbStaticCompress2Cull_CodeBegin, &dbStaticCompress2Cull_CodeEnd);	
VUManagedFunction 		&StaticCC_Cull_Entry = MasterCodeFactory::createManagedFunction( &dbStaticCompress2Cull_CodeBegin);

VUManagedCode				&StaticCC_Clip = MasterCodeFactory::createManagedCode( &dbStaticCompress2Clip_CodeBegin, &dbStaticCompress2Clip_CodeEnd);	
VUManagedFunction 		&StaticCC_Clip_Entry = MasterCodeFactory::createManagedFunction( &dbStaticCompress2Clip_CodeBegin);

// Shadower
VUManagedCode				&StaticCCShadower = MasterCodeFactory::createManagedCode( &dbStaticCompress2Shadower_CodeBegin, &dbStaticCompress2Shadower_CodeEnd);	
VUManagedFunction 		&StaticCCShadower_Entry = MasterCodeFactory::createManagedFunction( &dbStaticCompress2Shadower_CodeBegin);


// All (Fog, AlphaMul, TexMtx) 
VUManagedCode				&StaticCCAll = MasterCodeFactory::createManagedCode( &dbStaticCompress2All_CodeBegin, &dbStaticCompress2All_CodeEnd);
VUManagedFunction			&StaticCCAll_Entry = MasterCodeFactory::createManagedFunction( &dbStaticCompress2All_CodeBegin);


VUManagedCode				&StaticCCAll_Cull = MasterCodeFactory::createManagedCode( &dbStaticCompress2AllCull_CodeBegin, &dbStaticCompress2AllCull_CodeEnd);
VUManagedFunction			&StaticCCAll_Cull_Entry = MasterCodeFactory::createManagedFunction( &dbStaticCompress2AllCull_CodeBegin);

VUManagedCode				&StaticCCAll_Clip = MasterCodeFactory::createManagedCode( &dbStaticCompress2AllClip_CodeBegin, &dbStaticCompress2AllClip_CodeEnd);
VUManagedFunction			&StaticCCAll_Clip_Entry = MasterCodeFactory::createManagedFunction( &dbStaticCompress2AllClip_CodeBegin);

VUManagedCode				&StaticCCAllPL_Clip = MasterCodeFactory::createManagedCode( &dbStaticCompress2AllClipPLight_CodeBegin, &dbStaticCompress2AllClipPLight_CodeEnd);
VUManagedFunction			&StaticCCAllPL_Clip_Entry = MasterCodeFactory::createManagedFunction( &dbStaticCompress2AllClipPLight_CodeBegin);

// Texture Projection (vertex)
VUManagedCode				&StaticCCVTP = MasterCodeFactory::createManagedCode( &dbStaticCompress2VTP_CodeBegin, &dbStaticCompress2VTP_CodeEnd);
VUManagedFunction			&StaticCCVTP_Entry = MasterCodeFactory::createManagedFunction( &dbStaticCompress2VTP_CodeBegin);

VUManagedCode				&StaticCCVTP_Cull = MasterCodeFactory::createManagedCode( &dbStaticCompress2VTPCull_CodeBegin, &dbStaticCompress2VTPCull_CodeEnd);
VUManagedFunction			&StaticCCVTP_Cull_Entry = MasterCodeFactory::createManagedFunction( &dbStaticCompress2VTPCull_CodeBegin);

VUManagedCode				&StaticCCVTP_Clip = MasterCodeFactory::createManagedCode( &dbStaticCompress2VTPClip_CodeBegin, &dbStaticCompress2VTPClip_CodeEnd);
VUManagedFunction			&StaticCCVTP_Clip_Entry = MasterCodeFactory::createManagedFunction( &dbStaticCompress2VTPClip_CodeBegin);

// Shadow Map Custom
VUManagedCode				&StaticCCSMC = MasterCodeFactory::createManagedCode( &dbStaticCompress2SMC_CodeBegin, &dbStaticCompress2SMC_CodeEnd);
VUManagedFunction			&StaticCCSMC_Entry = MasterCodeFactory::createManagedFunction( &dbStaticCompress2SMC_CodeBegin);

VUManagedCode				&StaticCCSMC_Cull = MasterCodeFactory::createManagedCode( &dbStaticCompress2SMCCull_CodeBegin, &dbStaticCompress2SMCCull_CodeEnd);
VUManagedFunction			&StaticCCSMC_Cull_Entry = MasterCodeFactory::createManagedFunction( &dbStaticCompress2SMCCull_CodeBegin);

VUManagedCode				&StaticCCSMC_Clip = MasterCodeFactory::createManagedCode( &dbStaticCompress2SMCClip_CodeBegin, &dbStaticCompress2SMCClip_CodeEnd);
VUManagedFunction			&StaticCCSMC_Clip_Entry = MasterCodeFactory::createManagedFunction( &dbStaticCompress2SMCClip_CodeBegin);

// Multipass Stuff
VUManagedCode				&StaticCCC_MP = MasterCodeFactory::createManagedCode( &dbStaticCompress2Cull_MP_CodeBegin, &dbStaticCompress2Cull_MP_CodeEnd);
VUManagedFunction			&StaticCCC_MP_Entry = MasterCodeFactory::createManagedFunction( &dbStaticCompress2Cull_MP_CodeBegin);

VUManagedCode				&StaticCCAC_MP = MasterCodeFactory::createManagedCode( &dbStaticCompress2AllCull_MP_CodeBegin, &dbStaticCompress2AllCull_MP_CodeEnd);
VUManagedFunction			&StaticCCAC_MP_Entry = MasterCodeFactory::createManagedFunction( &dbStaticCompress2AllCull_MP_CodeBegin);

// Generic Engine
VUManagedCode				&StaticCC_EngA_Cull = MasterCodeFactory::createManagedCode( &dbStaticCompress2AllEngCull_CodeBegin, &dbStaticCompress2AllEngCull_CodeEnd);
VUManagedFunction			&StaticCC_EngA_Cull_Entry = MasterCodeFactory::createManagedFunction( &dbStaticCompress2AllEngCull_CodeBegin);

VUManagedFunction			&StaticCC_EngA_Cull_WO_Entry = MasterCodeFactory::createManagedFunction( &dbSC2AllEngCull_Setup);
VUManagedFunction			&StaticCC_EngA_Cull_DL_Entry = MasterCodeFactory::createManagedFunction( &dbSC2AllEngCull_DL_Setup);
VUManagedFunction			&StaticCC_EngA_Cull_PL_Entry = MasterCodeFactory::createManagedFunction( &dbSC2AllEngCull_PL_Setup);
VUManagedFunction			&StaticCC_EngA_Cull_AL_Entry = MasterCodeFactory::createManagedFunction( &dbSC2AllEngCull_AL_Setup);

VUManagedCode				&StaticCC_EngA_Cull_MP = MasterCodeFactory::createManagedCode( &dbStaticCompress2AllEngCull_MP_CodeBegin, &dbStaticCompress2AllEngCull_MP_CodeEnd);
VUManagedFunction			&StaticCC_EngA_Cull_MP_Entry = MasterCodeFactory::createManagedFunction( &dbStaticCompress2AllEngCull_MP_CodeBegin);

VUManagedFunction			&StaticCC_EngA_Cull_MP_WO_Entry = MasterCodeFactory::createManagedFunction( &dbSC2AllEngCull_MP_Setup);
VUManagedFunction			&StaticCC_EngA_Cull_MP_DL_Entry = MasterCodeFactory::createManagedFunction( &dbSC2AllEngCull_MP_DL_Setup);
VUManagedFunction			&StaticCC_EngA_Cull_MP_PL_Entry = MasterCodeFactory::createManagedFunction( &dbSC2AllEngCull_MP_PL_Setup);
VUManagedFunction			&StaticCC_EngA_Cull_MP_AL_Entry = MasterCodeFactory::createManagedFunction( &dbSC2AllEngCull_MP_AL_Setup);

VUManagedCode				&StaticCC_EngA_Clip = MasterCodeFactory::createManagedCode( &dbStaticCompress2AllEngClip_CodeBegin, &dbStaticCompress2AllEngClip_CodeEnd);
VUManagedFunction			&StaticCC_EngA_Clip_Entry = MasterCodeFactory::createManagedFunction( &dbStaticCompress2AllEngClip_CodeBegin);

VUManagedFunction			&StaticCC_EngA_Clip_DL_Entry = MasterCodeFactory::createManagedFunction( &dbSC2AllEngClip_DL_Setup);
VUManagedFunction			&StaticCC_EngA_Clip_PL_Entry = MasterCodeFactory::createManagedFunction( &dbSC2AllEngClip_PL_Setup);
VUManagedFunction			&StaticCC_EngA_Clip_AL_Entry = MasterCodeFactory::createManagedFunction( &dbSC2AllEngClip_AL_Setup);

VUManagedCode				&StaticCC_Core2_Clip = MasterCodeFactory::createManagedCode( &dbStaticCompress2AllCore2Clip_CodeBegin, &dbStaticCompress2AllCore2Clip_CodeEnd);
VUManagedFunction			&StaticCC_Core2A_Clip_Entry = MasterCodeFactory::createManagedFunction( &dbStaticCompress2AllCore2Clip_CodeBegin);

VUManagedFunction			&StaticCC_Core2A_Clip_PL_Entry = MasterCodeFactory::createManagedFunction( &dbSC2AllCore2Clip_PL_Setup);

/*---------------------------------------------------------------------------------------------------------------------------*
 *	Static (Compressed Color)
 *---------------------------------------------------------------------------------------------------------------------------*/

// Basic
VUManagedCode				&StaticUC = MasterCodeFactory::createManagedCode( &dbVertexColorSetCompress2_CodeBegin, &dbVertexColorSetCompress2_CodeEnd);
VUManagedFunction 		&StaticUC_Entry = MasterCodeFactory::createManagedFunction( &dbVertexColorSetCompress2_CodeBegin);

VUManagedCode				&StaticUC_Cull = MasterCodeFactory::createManagedCode( &dbVertexColorSetCompress2Cull_CodeBegin, &dbVertexColorSetCompress2Cull_CodeEnd);
VUManagedFunction 		&StaticUC_Cull_Entry = MasterCodeFactory::createManagedFunction( &dbVertexColorSetCompress2Cull_CodeBegin);

VUManagedCode				&StaticUC_Clip = MasterCodeFactory::createManagedCode( &dbVertexColorSetCompress2Clip_CodeBegin, &dbVertexColorSetCompress2Clip_CodeEnd);
VUManagedFunction 		&StaticUC_Clip_Entry = MasterCodeFactory::createManagedFunction( &dbVertexColorSetCompress2Clip_CodeBegin);

// Shadower
VUManagedCode				&StaticUCShadower = MasterCodeFactory::createManagedCode( &dbVertexColorSetCompress2Shadower_CodeBegin, &dbVertexColorSetCompress2Shadower_CodeEnd);	
VUManagedFunction 		&StaticUCShadower_Entry = MasterCodeFactory::createManagedFunction( &dbVertexColorSetCompress2Shadower_CodeBegin);

// All (Fog, AlphaMul, TexMtx) 
VUManagedCode				&StaticUCAll = MasterCodeFactory::createManagedCode( &dbVertexColorSetCompress2All_CodeBegin, &dbVertexColorSetCompress2All_CodeEnd);
VUManagedFunction			&StaticUCAll_Entry = MasterCodeFactory::createManagedFunction( &dbVertexColorSetCompress2All_CodeBegin);

VUManagedCode				&StaticUCAllPLight = MasterCodeFactory::createManagedCode( &dbVertexColorSetCompress2AllPLight_CodeBegin, &dbVertexColorSetCompress2AllPLight_CodeEnd);
VUManagedFunction			&StaticUCAllPLight_Entry = MasterCodeFactory::createManagedFunction( &dbVertexColorSetCompress2AllPLight_CodeBegin);

VUManagedCode				&StaticUCAll_Cull = MasterCodeFactory::createManagedCode( &dbVertexColorSetCompress2AllCull_CodeBegin, &dbVertexColorSetCompress2AllCull_CodeEnd);
VUManagedFunction			&StaticUCAll_Cull_Entry = MasterCodeFactory::createManagedFunction( &dbVertexColorSetCompress2AllCull_CodeBegin);

VUManagedCode				&StaticUCAll_Clip = MasterCodeFactory::createManagedCode( &dbVertexColorSetCompress2AllClip_CodeBegin, &dbVertexColorSetCompress2AllClip_CodeEnd);
VUManagedFunction			&StaticUCAll_Clip_Entry = MasterCodeFactory::createManagedFunction( &dbVertexColorSetCompress2AllClip_CodeBegin);

VUManagedCode				&StaticUCAllPL_Clip = MasterCodeFactory::createManagedCode( &dbVertexColorSetCompress2AllClipPLight_CodeBegin, &dbVertexColorSetCompress2AllClipPLight_CodeEnd);
VUManagedFunction			&StaticUCAllPL_Clip_Entry = MasterCodeFactory::createManagedFunction( &dbVertexColorSetCompress2AllClipPLight_CodeBegin);

// Texture Projection (vertex)
VUManagedCode				&StaticUCVTP = MasterCodeFactory::createManagedCode( &dbVertexColorSetCompress2VTP_CodeBegin, &dbVertexColorSetCompress2VTP_CodeEnd);
VUManagedFunction			&StaticUCVTP_Entry = MasterCodeFactory::createManagedFunction( &dbVertexColorSetCompress2VTP_CodeBegin);

VUManagedCode				&StaticUCVTP_Cull = MasterCodeFactory::createManagedCode( &dbVertexColorSetCompress2VTPCull_CodeBegin, &dbVertexColorSetCompress2VTPCull_CodeEnd);
VUManagedFunction			&StaticUCVTP_Cull_Entry = MasterCodeFactory::createManagedFunction( &dbVertexColorSetCompress2VTPCull_CodeBegin);

VUManagedCode				&StaticUCVTP_Clip = MasterCodeFactory::createManagedCode( &dbVertexColorSetCompress2VTPClip_CodeBegin, &dbVertexColorSetCompress2VTPClip_CodeEnd);
VUManagedFunction			&StaticUCVTP_Clip_Entry = MasterCodeFactory::createManagedFunction( &dbVertexColorSetCompress2VTPClip_CodeBegin);

// Shadow Map Custom
VUManagedCode				&StaticUCSMC = MasterCodeFactory::createManagedCode( &dbVertexColorSetCompress2SMC_CodeBegin, &dbVertexColorSetCompress2SMC_CodeEnd);
VUManagedFunction			&StaticUCSMC_Entry = MasterCodeFactory::createManagedFunction( &dbVertexColorSetCompress2SMC_CodeBegin);

VUManagedCode				&StaticUCSMC_Cull = MasterCodeFactory::createManagedCode( &dbVertexColorSetCompress2SMCCull_CodeBegin, &dbVertexColorSetCompress2SMCCull_CodeEnd);
VUManagedFunction			&StaticUCSMC_Cull_Entry = MasterCodeFactory::createManagedFunction( &dbVertexColorSetCompress2SMCCull_CodeBegin);

VUManagedCode				&StaticUCSMC_Clip = MasterCodeFactory::createManagedCode( &dbVertexColorSetCompress2SMCClip_CodeBegin, &dbVertexColorSetCompress2SMCClip_CodeEnd);
VUManagedFunction			&StaticUCSMC_Clip_Entry = MasterCodeFactory::createManagedFunction( &dbVertexColorSetCompress2SMCClip_CodeBegin);

// Multipass Stuff

VUManagedCode				&StaticUCC_MP = MasterCodeFactory::createManagedCode( &dbVertexColorSetCompress2Cull_MP_CodeBegin, &dbVertexColorSetCompress2Cull_MP_CodeEnd);
VUManagedFunction			&StaticUCC_MP_Entry = MasterCodeFactory::createManagedFunction( &dbVertexColorSetCompress2Cull_MP_CodeBegin);

VUManagedCode				&StaticUCAC_MP = MasterCodeFactory::createManagedCode( &dbVertexColorSetCompress2AllCull_MP_CodeBegin, &dbVertexColorSetCompress2AllCull_MP_CodeEnd);
VUManagedFunction			&StaticUCAC_MP_Entry = MasterCodeFactory::createManagedFunction( &dbVertexColorSetCompress2AllCull_MP_CodeBegin);

// Generic Engine
VUManagedCode				&StaticUC_EngA_Cull = MasterCodeFactory::createManagedCode( &dbVertexColorSetCompress2AllEngCull_CodeBegin, &dbVertexColorSetCompress2AllEngCull_CodeEnd);
VUManagedFunction			&StaticUC_EngA_Cull_Entry = MasterCodeFactory::createManagedFunction( &dbVertexColorSetCompress2AllEngCull_CodeBegin);

VUManagedFunction			&StaticUC_EngA_Cull_WO_Entry = MasterCodeFactory::createManagedFunction( &dbVCSC2AllEngCull_Setup);
VUManagedFunction			&StaticUC_EngA_Cull_DL_Entry = MasterCodeFactory::createManagedFunction( &dbVCSC2AllEngCull_DL_Setup);
VUManagedFunction			&StaticUC_EngA_Cull_PL_Entry = MasterCodeFactory::createManagedFunction( &dbVCSC2AllEngCull_PL_Setup);
VUManagedFunction			&StaticUC_EngA_Cull_AL_Entry = MasterCodeFactory::createManagedFunction( &dbVCSC2AllEngCull_AL_Setup);

VUManagedCode				&StaticUC_EngA_Cull_MP = MasterCodeFactory::createManagedCode( &dbVertexColorSetCompress2AllEngCull_MP_CodeBegin, &dbVertexColorSetCompress2AllEngCull_MP_CodeEnd);
VUManagedFunction			&StaticUC_EngA_Cull_MP_Entry = MasterCodeFactory::createManagedFunction( &dbVertexColorSetCompress2AllEngCull_MP_CodeBegin);

VUManagedFunction			&StaticUC_EngA_Cull_MP_WO_Entry = MasterCodeFactory::createManagedFunction( &dbVCSC2AllEngCull_MP_Setup);
VUManagedFunction			&StaticUC_EngA_Cull_MP_DL_Entry = MasterCodeFactory::createManagedFunction( &dbVCSC2AllEngCull_MP_DL_Setup);
VUManagedFunction			&StaticUC_EngA_Cull_MP_PL_Entry = MasterCodeFactory::createManagedFunction( &dbVCSC2AllEngCull_MP_PL_Setup);
VUManagedFunction			&StaticUC_EngA_Cull_MP_AL_Entry = MasterCodeFactory::createManagedFunction( &dbVCSC2AllEngCull_MP_AL_Setup);

VUManagedCode				&StaticUC_EngA_Clip = MasterCodeFactory::createManagedCode( &dbVertexColorSetCompress2AllEngClip_CodeBegin, &dbVertexColorSetCompress2AllEngClip_CodeEnd);

VUManagedFunction			&StaticUC_EngA_Clip_Entry = MasterCodeFactory::createManagedFunction( &dbVertexColorSetCompress2AllEngClip_CodeBegin);

VUManagedFunction			&StaticUC_EngA_Clip_DL_Entry = MasterCodeFactory::createManagedFunction( &dbVCSC2AllEngClip_DL_Setup);
VUManagedFunction			&StaticUC_EngA_Clip_PL_Entry = MasterCodeFactory::createManagedFunction( &dbVCSC2AllEngClip_PL_Setup);
VUManagedFunction			&StaticUC_EngA_Clip_AL_Entry = MasterCodeFactory::createManagedFunction( &dbVCSC2AllEngClip_AL_Setup);

VUManagedCode				&StaticUC_Core2_Clip = MasterCodeFactory::createManagedCode( &dbVertexColorSetCompress2AllCore2Clip_CodeBegin, &dbVertexColorSetCompress2AllCore2Clip_CodeEnd);
VUManagedFunction			&StaticUC_Core2A_Clip_Entry = MasterCodeFactory::createManagedFunction( &dbVertexColorSetCompress2AllCore2Clip_CodeBegin);

VUManagedFunction			&StaticUC_Core2A_Clip_PL_Entry = MasterCodeFactory::createManagedFunction( &dbVCSC2AllCore2Clip_PL_Setup);

// EVMap Stuff
VUManagedCode				&Static_EVMap = MasterCodeFactory::createManagedCode( &dbStaticEVMap_CodeBegin, &dbStaticEVMap_CodeEnd);
VUManagedFunction			&Static_EVMap_Entry = MasterCodeFactory::createManagedFunction( &dbStaticEVMap_CodeBegin);

VUManagedCode				&Static_SphereMap = MasterCodeFactory::createManagedCode( &dbStaticSphereMap_CodeBegin, &dbStaticSphereMap_CodeEnd);
VUManagedFunction			&Static_SphereMap_Entry = MasterCodeFactory::createManagedFunction( &dbStaticSphereMap_CodeBegin);

VUManagedCode				&Static_RefMap = MasterCodeFactory::createManagedCode( &dbStaticRefMap_CodeBegin, &dbStaticRefMap_CodeEnd);
VUManagedFunction			&Static_RefMap_Entry = MasterCodeFactory::createManagedFunction( &dbStaticRefMap_CodeBegin);

VUManagedCode				&Static_SphereRefMap = MasterCodeFactory::createManagedCode( &dbStaticSphereRefMap_CodeBegin, &dbStaticSphereRefMap_CodeEnd);
VUManagedFunction			&Static_SphereRefMap_Entry = MasterCodeFactory::createManagedFunction( &dbStaticSphereRefMap_CodeBegin);

// Clipper Definition --------------------------------------------------------
VUClipperSet				StdClipper = { GenericClip, &gcSetup, &gcClip } ;

// Static Compressed Color Routines ------------------------------------------
// Standard No Features ------------------------------------------------------
VUCodeEntry 				StaticCC_Xfrm_E = 
{ 
	0, 
	&StaticCC_Entry, 
	0, 
	0, 
	0, 
	0 
} ;


VUCodeEntry 				StaticCC_Shadower_E = 
{ 
	0, 
	&StaticCCShadower_Entry, 
	0, 
	VU_USES_REG_VERTEX, 
	0, 
	0 
} ;


VUCodeEntry					StaticCC_Cull_E = 
{ 
	0, 
	&StaticCC_Cull_Entry, 
	VU_SEND_MTX_CLIP_SCREEN, 
	0, 
	0, 
	0 
} ;

VUCodeEntry					StaticCC_Clip_E = 
{ 
	0, 
	&StaticCC_Clip_Entry, 
	VU_SEND_MTX_CLIP_SCREEN, 
	VU_USES_STD_CLIPPER, 
	0, 
	0 
} ;

VUCodeEntry					StaticCC_Cull_MP_E = 
{ 
	0, 
	&StaticCCC_MP_Entry, 
	VU_SEND_MTX_CLIP_SCREEN, 
	VU_USES_MODEL_BASE_OFFSET, 
	0, 
	0 
} ;

VUCodeSet					StaticCC_Std_S		= { &StaticCC_Xfrm_E, &StaticCC_Cull_E, &StaticCC_Clip_E } ;
VUCodeSet					StaticCC_Std_MP_S	= { 0, &StaticCC_Cull_MP_E, 0 } ;

VUCodeSet					StaticCC_Shadower_S		= { &StaticCC_Shadower_E, 0, 0 } ;

// All Features ( Fog, Texture Mtx, Alpha Multiplier ) -----------------------

VUCodeEntry 				StaticCCAll_Xfrm_E = 
{
	0, 
 	&StaticCCAll_Entry, 
	VU_SEND_DATA_FOG | VU_SEND_BLOCK_ALL | VU_SEND_BLOCK_ALL_ALPHA_EARLY, 
	VU_USES_XYZF2, 
	0, 
	0 
} ;

VUCodeEntry					StaticCCAll_Cull_E = 
{ 
	0, 
	&StaticCCAll_Cull_Entry, 
	VU_SEND_MTX_CLIP_SCREEN | VU_SEND_DATA_FOG | VU_SEND_BLOCK_ALL | VU_SEND_BLOCK_ALL_ALPHA_EARLY, 
	VU_USES_XYZF2, 
	0, 
	0 
} ;

VUCodeEntry					StaticCCAll_Clip_E = 
{ 
	0, 
	&StaticCCAll_Clip_Entry, 
	VU_SEND_MTX_CLIP_SCREEN | VU_SEND_DATA_FOG | VU_SEND_BLOCK_ALL | VU_SEND_BLOCK_ALL_ALPHA_EARLY, 
	VU_USES_STD_CLIPPER | VU_USES_XYZF2, 
	0, 
	0 
} ;

VUCodeEntry					StaticCCAll_Cull_MP_E = 
{ 
	0, 
	&StaticCCAC_MP_Entry, 
	VU_SEND_MTX_CLIP_SCREEN | VU_SEND_DATA_FOG | VU_SEND_BLOCK_ALL | VU_SEND_BLOCK_ALL_ALPHA_EARLY, 
	VU_USES_XYZF2 | VU_USES_MODEL_BASE_OFFSET, 
	0, 
	0 
} ;

VUCodeSet					StaticCC_All_S		= { &StaticCCAll_Xfrm_E, &StaticCCAll_Cull_E, &StaticCCAll_Clip_E } ;
VUCodeSet					StaticCC_All_MP_S	= { 0, &StaticCCAll_Cull_MP_E, 0 } ;

// Point Lighting ------------------------------------------------------------

VUCodeEntry					StaticCCPLight_Clip_E = 
{
	0, 
	&StaticCCAllPL_Clip_Entry,
	VU_SEND_DATA_FOG | VU_SEND_BLOCK_ALL | VU_SEND_DATA_PLIGHTS | VU_SEND_BLOCK_ALL_ALPHA_EARLY | VU_SEND_MTX_CLIP_SCREEN,
	VU_USES_STD_CLIPPER | VU_USES_XYZF2,
	0,
	0
} ;

VUCodeSet					StaticCCPLight_S	= { 0, 0, &StaticCCPLight_Clip_E };

// Vertex Texture Projection Rerender ----------------------------------------

VUCodeEntry					StaticCCVTP_Xfrm_E = 
{
	0, 
	&StaticCCVTP_Entry,
	VU_SEND_DATA_FOG | VU_SEND_BLOCK_VTP,
	VU_USES_XYZF2,
	0,
	0
} ;

VUCodeEntry					StaticCCVTP_Cull_E = 
{
	0, 
	&StaticCCVTP_Cull_Entry,
	VU_SEND_DATA_FOG | VU_SEND_BLOCK_VTP | VU_SEND_MTX_CLIP_SCREEN,
	VU_USES_XYZF2,
	0,
	0
} ;


VUCodeEntry					StaticCCVTP_Clip_E = 
{
	0, 
	&StaticCCVTP_Clip_Entry,
	VU_SEND_DATA_FOG | VU_SEND_BLOCK_VTP | VU_SEND_MTX_CLIP_SCREEN,
	VU_USES_STD_CLIPPER | VU_USES_XYZF2,
	0,
	0
} ;

VUCodeSet					StaticCC_VTP_S = { &StaticCCVTP_Xfrm_E, &StaticCCVTP_Cull_E,  &StaticCCVTP_Clip_E } ;

// Shadow Map Rerender -------------------------------------------------------

VUCodeEntry					StaticCCSM_Xfrm_E = 
{
	0, 
	&StaticCCSMC_Entry,
	VU_SEND_DATA_FOG | VU_SEND_BLOCK_SHADOW_MAP,
	VU_USES_XYZF2,
	0,
	0
} ;

VUCodeEntry					StaticCCSM_Cull_E = 
{
	0, 
	&StaticCCSMC_Cull_Entry,
	VU_SEND_DATA_FOG | VU_SEND_BLOCK_SHADOW_MAP | VU_SEND_MTX_CLIP_SCREEN,
	VU_USES_XYZF2,
	0,
	0
} ;


VUCodeEntry					StaticCCSM_Clip_E = 
{
	0, 
	&StaticCCSMC_Clip_Entry,
	VU_SEND_DATA_FOG | VU_SEND_BLOCK_SHADOW_MAP | VU_SEND_MTX_CLIP_SCREEN,
	VU_USES_STD_CLIPPER | VU_USES_XYZF2,
	0,
	0
} ;

VUCodeSet					StaticCC_SM_S = { &StaticCCSM_Xfrm_E, &StaticCCSM_Cull_E,  &StaticCCSM_Clip_E } ;

// Generic Engine -------------------------------------------------------------
VUCodeEntry					StaticCC_EngA_Cull_E = 
{ 
	0, 
	&StaticCC_EngA_Cull_Entry, 
	VU_SEND_MTX_CLIP_SCREEN | VU_SEND_DATA_FOG | VU_SEND_BLOCK_ALL, 
	VU_INFO_CORE | VU_USES_XYZF2, 
	0, 
	0 
} ;

VUCodeEntry					StaticCC_EngA_Clip_E = 
{ 
	0, 
	&StaticCC_EngA_Clip_Entry, 
	VU_SEND_MTX_CLIP_SCREEN | VU_SEND_DATA_FOG | VU_SEND_BLOCK_ALL, 
	VU_INFO_CORE | VU_USES_XYZF2 | VU_USES_STD_CLIPPER, 
	0, 
	0 
} ;

VUCodeSet					StaticCC_EngA_S = { 0, &StaticCC_EngA_Cull_E, &StaticCC_EngA_Clip_E };

VUCodeEntry					StaticCC_EngA_Cull_MP_E = 
{ 
	0,
	&StaticCC_EngA_Cull_MP_Entry, 
	VU_SEND_MTX_CLIP_SCREEN | VU_SEND_DATA_FOG | VU_SEND_BLOCK_ALL, 
	VU_INFO_CORE | VU_USES_XYZF2, 
	0, 
	0 
} ;

VUCodeSet					StaticCC_EngA_MP_S = { 0, &StaticCC_EngA_Cull_MP_E, 0 };


VUCodeEntry					StaticCC_Core2A_Clip_E = 
{ 
	0, 
	&StaticCC_Core2A_Clip_Entry, 
	VU_SEND_MTX_CLIP_SCREEN | VU_SEND_DATA_FOG | VU_SEND_BLOCK_ALL, 
	VU_INFO_CORE | VU_USES_XYZF2 | VU_USES_STD_CLIPPER, 
	0, 
	0 
} ;

// Directional Lights ---------------------------------------------------------
VUCodeEntry					StaticCC_EngA_Cull_DLight_E = 
{ 
	&StaticCC_EngA_Cull_E,
	&StaticCC_EngA_Cull_DL_Entry, 
	VU_SEND_DATA_DLIGHTS | VU_SEND_DATA_ALIGHT, 
	VU_USES_CORE,
	REQ_LIGHT_DIRECTIONAL_0 | REQ_LIGHT_DIRECTIONAL_1 | REQ_LIGHT_DIRECTIONAL_2, 
	0 
} ;

VUCodeEntry					StaticCC_EngA_Clip_DLight_E = 
{ 
	&StaticCC_EngA_Clip_E,
	&StaticCC_EngA_Clip_DL_Entry, 
	VU_SEND_DATA_DLIGHTS | VU_SEND_DATA_ALIGHT, 
	VU_USES_CORE,
	REQ_LIGHT_DIRECTIONAL_0 | REQ_LIGHT_DIRECTIONAL_1 | REQ_LIGHT_DIRECTIONAL_2, 
	0 
} ;

VUCodeSet					StaticCC_DLight_S = { 0, &StaticCC_EngA_Cull_DLight_E, &StaticCC_EngA_Clip_DLight_E };

VUCodeEntry					StaticCC_EngA_Cull_MP_DLight_E = 
{ 
	&StaticCC_EngA_Cull_MP_E, 
	&StaticCC_EngA_Cull_MP_DL_Entry, 
	VU_SEND_DATA_DLIGHTS | VU_SEND_DATA_ALIGHT, 
	VU_USES_CORE,
	REQ_LIGHT_DIRECTIONAL_0 | REQ_LIGHT_DIRECTIONAL_1 | REQ_LIGHT_DIRECTIONAL_2, 
	0 
} ;

VUCodeSet					StaticCC_DLight_MP_S = { 0, &StaticCC_EngA_Cull_MP_DLight_E, 0 };

// Point Lighting -------------------------------------------------------------
VUCodeEntry					StaticCC_EngA_Cull_PLight_E = 
{ 
	&StaticCC_EngA_Cull_E,
	&StaticCC_EngA_Cull_PL_Entry, 
	VU_SEND_DATA_PLIGHTS | VU_SEND_DATA_ALIGHT, 
	VU_USES_CORE,
	REQ_LIGHT_POINT_0 | REQ_LIGHT_POINT_1, 
	0 
} ;

VUCodeEntry					StaticCC_EngA_Clip_PLight_E = 
{ 
	&StaticCC_EngA_Clip_E,
	&StaticCC_EngA_Clip_PL_Entry, 
	VU_SEND_DATA_PLIGHTS | VU_SEND_DATA_ALIGHT, 
	VU_USES_CORE,
	REQ_LIGHT_POINT_0 | REQ_LIGHT_POINT_1, 
	0 
} ;

VUCodeEntry					StaticCC_Core2A_Clip_PLight_E = 
{ 
	&StaticCC_Core2A_Clip_E,
	&StaticCC_Core2A_Clip_PL_Entry, 
	VU_SEND_DATA_PLIGHTS | VU_SEND_DATA_ALIGHT, 
	VU_USES_CORE,
	REQ_LIGHT_POINT_0 | REQ_LIGHT_POINT_1, 
	0 
} ;

VUCodeSet					StaticCC_PLight_S = { 0, &StaticCC_EngA_Cull_PLight_E, &StaticCC_EngA_Clip_PLight_E };

VUCodeEntry					StaticCC_EngA_Cull_MP_PLight_E = 
{ 
	&StaticCC_EngA_Cull_MP_E,
	&StaticCC_EngA_Cull_MP_PL_Entry, 
	VU_SEND_DATA_PLIGHTS | VU_SEND_DATA_ALIGHT, 
	VU_USES_CORE,
	REQ_LIGHT_POINT_0 | REQ_LIGHT_POINT_1, 
	0 
} ;

VUCodeSet					StaticCC_PLight_MP_S = { 0, &StaticCC_EngA_Cull_MP_PLight_E, 0 };

// Directional + Point Lighting -----------------------------------------------
VUCodeEntry					StaticCC_EngA_Cull_ALight_E = 
{ 
	&StaticCC_EngA_Cull_E,
	&StaticCC_EngA_Cull_AL_Entry, 
	VU_SEND_DATA_PLIGHTS | VU_SEND_DATA_DLIGHTS | VU_SEND_DATA_ALIGHT, 
	VU_USES_CORE,
	REQ_LIGHT_POINT_0 | REQ_LIGHT_POINT_1 | REQ_LIGHT_DIRECTIONAL_0 | REQ_LIGHT_DIRECTIONAL_1 | REQ_LIGHT_DIRECTIONAL_2, 
	0 
} ;

VUCodeEntry					StaticCC_EngA_Clip_ALight_E = 
{ 
	&StaticCC_EngA_Clip_E,
	&StaticCC_EngA_Clip_AL_Entry, 
	VU_SEND_DATA_PLIGHTS | VU_SEND_DATA_DLIGHTS | VU_SEND_DATA_ALIGHT, 
	VU_USES_CORE,
	REQ_LIGHT_POINT_0 | REQ_LIGHT_POINT_1 | REQ_LIGHT_DIRECTIONAL_0 | REQ_LIGHT_DIRECTIONAL_1 | REQ_LIGHT_DIRECTIONAL_2, 
	0 
} ;

VUCodeSet					StaticCC_ALight_S = { 0, &StaticCC_EngA_Cull_ALight_E, &StaticCC_EngA_Clip_ALight_E };

VUCodeEntry					StaticCC_EngA_Cull_MP_ALight_E = 
{ 
	&StaticCC_EngA_Cull_MP_E,
	&StaticCC_EngA_Cull_MP_AL_Entry, 
	VU_SEND_DATA_PLIGHTS | VU_SEND_DATA_DLIGHTS | VU_SEND_DATA_ALIGHT, 
	VU_USES_CORE,
	REQ_LIGHT_POINT_0 | REQ_LIGHT_POINT_1 | REQ_LIGHT_DIRECTIONAL_0 | REQ_LIGHT_DIRECTIONAL_1 | REQ_LIGHT_DIRECTIONAL_2, 
	0 
} ;

VUCodeSet					StaticCC_ALight_MP_S = { 0, &StaticCC_EngA_Cull_MP_ALight_E, 0 };
// Test Code
VUCodeEntry					StaticCC_EngA_Cull_WO_E = 
{ 
	&StaticCC_EngA_Cull_E,
	&StaticCC_EngA_Cull_WO_Entry, 
	0, 
	VU_USES_CORE, 
	0, 
	0 
} ;

VUCodeEntry					StaticCC_EngA_Cull_MP_WO_E = 
{ 
	&StaticCC_EngA_Cull_MP_E,
	&StaticCC_EngA_Cull_MP_WO_Entry, 
	0, 
	VU_USES_XYZF2 | VU_USES_CORE,
	0, 
	0 
} ;

VUCodeSet *					StaticCCArray[] =
{
	&StaticCC_Std_S,
	&StaticCC_Std_MP_S,
	
	&StaticCC_All_S,
	&StaticCC_All_MP_S,
	
	&StaticCC_DLight_S,
	&StaticCC_DLight_MP_S,
	&StaticCC_PLight_S,
	&StaticCC_PLight_MP_S,
	&StaticCC_ALight_S,
	&StaticCC_ALight_MP_S,

	&StaticCC_EngA_S,
	&StaticCC_EngA_MP_S,
	
	&StaticCC_SM_S,
	&StaticCC_VTP_S,
	&StaticCC_Shadower_S
} ;

	
// Static Uncompressed Color Routines ----------------------------------------
// Standard No Features ------------------------------------------------------
VUCodeEntry 				StaticUC_Xfrm_E = 
{ 
	0, 
	&StaticUC_Entry, 
	0, 
	0, 
	0, 
	0 
} ;

VUCodeEntry 				StaticUC_Shadower_E = 
{ 
	0, 
	&StaticUCShadower_Entry, 
	0, 
	VU_USES_REG_VERTEX, 
	0, 
	0 
} ;

VUCodeEntry					StaticUC_Cull_E = 
{ 
	0, 
	&StaticUC_Cull_Entry, 
	VU_SEND_MTX_CLIP_SCREEN, 
	0, 
	0, 
	0 
} ;

VUCodeEntry					StaticUC_Clip_E = 
{ 
	0, 
	&StaticUC_Clip_Entry, 
	VU_SEND_MTX_CLIP_SCREEN, 
	VU_USES_STD_CLIPPER, 
	0, 
	0 
} ;

VUCodeEntry					StaticUC_Cull_MP_E = 
{ 
	0, 
	&StaticUCC_MP_Entry, 
	VU_SEND_MTX_CLIP_SCREEN, 
	VU_USES_MODEL_BASE_OFFSET, 
	0, 
	0 
} ;

VUCodeSet					StaticUC_Std_S		= { &StaticUC_Xfrm_E, &StaticUC_Cull_E, &StaticUC_Clip_E } ;
VUCodeSet					StaticUC_Std_MP_S	= { 0, &StaticUC_Cull_MP_E, 0 } ;

VUCodeSet					StaticUC_Shadower_S		= { &StaticUC_Shadower_E, 0, 0 } ;

// All Features ( Fog, Texture Mtx, Alpha Multiplier ) -----------------------

VUCodeEntry 				StaticUCAll_Xfrm_E = 
{ 	
	0, 
	&StaticUCAll_Entry, 
	VU_SEND_DATA_FOG | VU_SEND_BLOCK_ALL, 
	VU_USES_XYZF2, 
	0, 
	0 
} ;

VUCodeEntry					StaticUCAll_Cull_E = 
{ 
	0, 
	&StaticUCAll_Cull_Entry, 
	VU_SEND_MTX_CLIP_SCREEN | VU_SEND_DATA_FOG | VU_SEND_BLOCK_ALL, 
	VU_USES_XYZF2, 
	0, 
	0 
} ;

VUCodeEntry					StaticUCAll_Clip_E = 
{ 
	0, 
	&StaticUCAll_Clip_Entry, 
	VU_SEND_MTX_CLIP_SCREEN | VU_SEND_DATA_FOG | VU_SEND_BLOCK_ALL, 
	VU_USES_STD_CLIPPER | VU_USES_XYZF2, 
	0, 
	0 
} ;

VUCodeEntry					StaticUCAll_Cull_MP_E = 
{ 
	0, 
	&StaticUCAC_MP_Entry, 
	VU_SEND_MTX_CLIP_SCREEN | VU_SEND_DATA_FOG | VU_SEND_BLOCK_ALL, 
	VU_USES_XYZF2 | VU_USES_MODEL_BASE_OFFSET, 
	0, 
	0 
} ;

VUCodeSet					StaticUC_All_S		= { &StaticUCAll_Xfrm_E, &StaticUCAll_Cull_E, &StaticUCAll_Clip_E } ;
VUCodeSet					StaticUC_All_MP_S	= { 0, &StaticUCAll_Cull_MP_E, 0 } ;

// Point Lighting ------------------------------------------------------------
VUCodeEntry 				StaticUCPLight_Xfrm_E = 
{ 	
	0, 
	&StaticUCAllPLight_Entry, 
	VU_SEND_DATA_FOG | VU_SEND_BLOCK_ALL | VU_SEND_DATA_PLIGHTS, 
	VU_USES_XYZF2, 
	REQ_LIGHT_POINT_0 | REQ_LIGHT_POINT_1, 
	0 
} ;

VUCodeEntry					StaticUCPLight_Clip_E = 
{
	0, 
	&StaticUCAllPL_Clip_Entry,
	VU_SEND_DATA_FOG | VU_SEND_BLOCK_ALL | VU_SEND_DATA_PLIGHTS | VU_SEND_MTX_CLIP_SCREEN,
	VU_USES_STD_CLIPPER | VU_USES_XYZF2,
	REQ_LIGHT_POINT_0 | REQ_LIGHT_POINT_1, 
	0
} ;

VUCodeSet					StaticUCPLight_S	= { &StaticUCPLight_Xfrm_E, 0, &StaticUCPLight_Clip_E };

// Vertex Texture Projection Rerender ----------------------------------------

VUCodeEntry					StaticUCVTP_Xfrm_E = 
{
	0, 
	&StaticUCVTP_Entry,
	VU_SEND_DATA_FOG | VU_SEND_BLOCK_VTP,
	VU_USES_XYZF2,
	0,
	0
} ;

VUCodeEntry					StaticUCVTP_Cull_E = 
{
	0, 
	&StaticUCVTP_Cull_Entry,
	VU_SEND_DATA_FOG | VU_SEND_BLOCK_VTP | VU_SEND_MTX_CLIP_SCREEN,
	VU_USES_XYZF2,
	0,
	0
} ;


VUCodeEntry					StaticUCVTP_Clip_E = 
{
	0, 
	&StaticUCVTP_Clip_Entry,
	VU_SEND_DATA_FOG | VU_SEND_BLOCK_VTP | VU_SEND_MTX_CLIP_SCREEN,
	VU_USES_STD_CLIPPER | VU_USES_XYZF2,
	0,
	0
} ;

VUCodeSet					StaticUC_VTP_S = { &StaticUCVTP_Xfrm_E, &StaticUCVTP_Cull_E,  &StaticUCVTP_Clip_E } ;

// Shadow Map Rerender -------------------------------------------------------

VUCodeEntry					StaticUCSM_Xfrm_E = 
{
	0, 
	&StaticUCSMC_Entry,
	VU_SEND_DATA_FOG | VU_SEND_BLOCK_SHADOW_MAP,
	VU_USES_XYZF2,
	0,
	0
} ;

VUCodeEntry					StaticUCSM_Cull_E = 
{
	0, 
	&StaticUCSMC_Cull_Entry,
	VU_SEND_DATA_FOG | VU_SEND_BLOCK_SHADOW_MAP | VU_SEND_MTX_CLIP_SCREEN,
	VU_USES_XYZF2,
	0,
	0
} ;


VUCodeEntry					StaticUCSM_Clip_E = 
{
	0, 
	&StaticUCSMC_Clip_Entry,
	VU_SEND_DATA_FOG | VU_SEND_BLOCK_SHADOW_MAP | VU_SEND_MTX_CLIP_SCREEN,
	VU_USES_STD_CLIPPER | VU_USES_XYZF2,
	0,
	0
} ;

VUCodeSet					StaticUC_SM_S = { &StaticUCSM_Xfrm_E, &StaticUCSM_Cull_E,  &StaticUCSM_Clip_E } ;

// Generic Engine -------------------------------------------------------------
VUCodeEntry					StaticUC_EngA_Cull_E = 
{ 
	0, 
	&StaticUC_EngA_Cull_Entry, 
	VU_SEND_MTX_CLIP_SCREEN | VU_SEND_DATA_FOG | VU_SEND_BLOCK_ALL, 
	VU_INFO_CORE | VU_USES_XYZF2, 
	0, 
	0 
} ;

VUCodeEntry					StaticUC_EngA_Clip_E = 
{ 
	0, 
	&StaticUC_EngA_Clip_Entry, 
	VU_SEND_MTX_CLIP_SCREEN | VU_SEND_DATA_FOG | VU_SEND_BLOCK_ALL, 
	VU_INFO_CORE | VU_USES_XYZF2 | VU_USES_STD_CLIPPER, 
	0, 
	0 
} ;

VUCodeSet					StaticUC_EngA_S = { 0, &StaticUC_EngA_Cull_E, &StaticUC_EngA_Clip_E };

VUCodeEntry					StaticUC_Core2A_Clip_E = 
{ 
	0, 
	&StaticUC_Core2A_Clip_Entry, 
	VU_SEND_MTX_CLIP_SCREEN | VU_SEND_DATA_FOG | VU_SEND_BLOCK_ALL, 
	VU_INFO_CORE | VU_USES_XYZF2 | VU_USES_STD_CLIPPER, 
	0, 
	0 
} ;

VUCodeEntry					StaticUC_EngA_Cull_MP_E = 
{ 
	0,
	&StaticUC_EngA_Cull_MP_Entry, 
	VU_SEND_MTX_CLIP_SCREEN | VU_SEND_DATA_FOG | VU_SEND_BLOCK_ALL, 
	VU_INFO_CORE | VU_USES_XYZF2, 
	0, 
	0 
} ;

VUCodeSet					StaticUC_EngA_MP_S = { 0, &StaticUC_EngA_Cull_MP_E, 0 };

// Directional Lighting -------------------------------------------------------
VUCodeEntry					StaticUC_EngA_Cull_DLight_E = 
{ 
	&StaticUC_EngA_Cull_E, 
	&StaticUC_EngA_Cull_DL_Entry, 
	VU_SEND_DATA_DLIGHTS | VU_SEND_DATA_ALIGHT, 
	VU_USES_CORE,
	REQ_LIGHT_DIRECTIONAL_0 | REQ_LIGHT_DIRECTIONAL_1 | REQ_LIGHT_DIRECTIONAL_2, 
	0 
} ;

VUCodeEntry					StaticUC_EngA_Clip_DLight_E = 
{ 
	&StaticUC_EngA_Clip_E, 
	&StaticUC_EngA_Clip_DL_Entry, 
	VU_SEND_DATA_DLIGHTS | VU_SEND_DATA_ALIGHT, 
	VU_USES_CORE,
	REQ_LIGHT_DIRECTIONAL_0 | REQ_LIGHT_DIRECTIONAL_1 | REQ_LIGHT_DIRECTIONAL_2, 
	0 
} ;

VUCodeSet					StaticUC_DLight_S = { 0, &StaticUC_EngA_Cull_DLight_E, 0 }; // &StaticUC_EngA_Clip_DLight_E };

VUCodeEntry					StaticUC_EngA_Cull_MP_DLight_E = 
{ 
	&StaticUC_EngA_Cull_MP_E, 
	&StaticUC_EngA_Cull_MP_DL_Entry, 
	VU_SEND_DATA_DLIGHTS | VU_SEND_DATA_ALIGHT, 
	VU_USES_CORE,
	REQ_LIGHT_DIRECTIONAL_0 | REQ_LIGHT_DIRECTIONAL_1 | REQ_LIGHT_DIRECTIONAL_2, 
	0 
} ;

VUCodeSet					StaticUC_DLight_MP_S = { 0, &StaticUC_EngA_Cull_MP_DLight_E, 0 };

// Point Lighting -------------------------------------------------------------
VUCodeEntry					StaticUC_EngA_Cull_PLight_E = 
{ 
	&StaticUC_EngA_Cull_E, 
	&StaticUC_EngA_Cull_PL_Entry, 
	VU_SEND_DATA_PLIGHTS | VU_SEND_DATA_ALIGHT, 
	VU_USES_CORE,
	REQ_LIGHT_POINT_0 | REQ_LIGHT_POINT_1, 
	0 
} ;

VUCodeEntry					StaticUC_EngA_Clip_PLight_E = 
{ 
	&StaticUC_EngA_Clip_E, 
	&StaticUC_EngA_Clip_PL_Entry, 
	VU_SEND_DATA_PLIGHTS | VU_SEND_DATA_ALIGHT, 
	VU_USES_CORE,
	REQ_LIGHT_POINT_0 | REQ_LIGHT_POINT_1, 
	0 
} ;

VUCodeEntry					StaticUC_Core2A_Clip_PLight_E = 
{ 
	&StaticUC_Core2A_Clip_E,
	&StaticUC_Core2A_Clip_PL_Entry, 
	VU_SEND_DATA_PLIGHTS | VU_SEND_DATA_ALIGHT, 
	VU_USES_CORE,
	REQ_LIGHT_POINT_0 | REQ_LIGHT_POINT_1, 
	0 
} ;

VUCodeSet					StaticUC_PLight_S = { 0, &StaticUC_EngA_Cull_PLight_E, 0 }; //&StaticUC_EngA_Clip_PLight_E };

VUCodeEntry					StaticUC_EngA_Cull_MP_PLight_E = 
{ 
	&StaticUC_EngA_Cull_MP_E, 
	&StaticUC_EngA_Cull_MP_PL_Entry, 
	VU_SEND_DATA_PLIGHTS | VU_SEND_DATA_ALIGHT, 
	VU_USES_CORE,
	REQ_LIGHT_POINT_0 | REQ_LIGHT_POINT_1, 
	0 
} ;

VUCodeSet					StaticUC_PLight_MP_S = { 0, &StaticUC_EngA_Cull_MP_PLight_E, 0 };

// Directional + Point Lighting -----------------------------------------------
VUCodeEntry					StaticUC_EngA_Cull_ALight_E = 
{ 
	&StaticUC_EngA_Cull_E, 
	&StaticUC_EngA_Cull_AL_Entry, 
	VU_SEND_DATA_PLIGHTS | VU_SEND_DATA_DLIGHTS | VU_SEND_DATA_ALIGHT, 
	VU_USES_CORE,
	REQ_LIGHT_POINT_0 | REQ_LIGHT_POINT_1 | REQ_LIGHT_DIRECTIONAL_0 | REQ_LIGHT_DIRECTIONAL_1 | REQ_LIGHT_DIRECTIONAL_2, 
	0 
} ;

VUCodeEntry					StaticUC_EngA_Clip_ALight_E = 
{ 
	&StaticUC_EngA_Clip_E, 
	&StaticUC_EngA_Clip_AL_Entry, 
	VU_SEND_DATA_PLIGHTS | VU_SEND_DATA_DLIGHTS | VU_SEND_DATA_ALIGHT, 
	VU_USES_CORE,
	REQ_LIGHT_POINT_0 | REQ_LIGHT_POINT_1 | REQ_LIGHT_DIRECTIONAL_0 | REQ_LIGHT_DIRECTIONAL_1 | REQ_LIGHT_DIRECTIONAL_2, 
	0 
} ;

VUCodeSet					StaticUC_ALight_S = { 0, &StaticUC_EngA_Cull_ALight_E, &StaticUC_EngA_Clip_ALight_E };

VUCodeEntry					StaticUC_EngA_Cull_MP_ALight_E = 
{ 
	&StaticUC_EngA_Cull_MP_E, 
	&StaticUC_EngA_Cull_MP_AL_Entry, 
	VU_SEND_DATA_PLIGHTS | VU_SEND_DATA_DLIGHTS | VU_SEND_DATA_ALIGHT, 
	VU_USES_CORE,
	REQ_LIGHT_POINT_0 | REQ_LIGHT_POINT_1 | REQ_LIGHT_DIRECTIONAL_0 | REQ_LIGHT_DIRECTIONAL_1 | REQ_LIGHT_DIRECTIONAL_2, 
	0 
} ;

VUCodeSet					StaticUC_ALight_MP_S = { 0, &StaticUC_EngA_Cull_MP_ALight_E, 0 };

// MP

VUCodeEntry					StaticUC_EngA_WO_E = 
{ 
	&StaticUC_EngA_Cull_E, 
	&StaticUC_EngA_Cull_WO_Entry, 
	0, 
	VU_USES_CORE,
	0, 
	0 
} ;



VUCodeEntry					StaticUC_EngA_Cull_MP_WO_E = 
{ 
	&StaticUC_EngA_Cull_MP_E, 
	&StaticUC_EngA_Cull_MP_WO_Entry, 
	0, 
	VU_USES_CORE,
	0, 
	0 
} ;


VUCodeSet *					StaticUCArray[] =
{
	&StaticUC_Std_S,
	&StaticUC_Std_MP_S,

	&StaticUC_All_S,
	&StaticUC_All_MP_S,

	&StaticUC_DLight_S,
	&StaticUC_DLight_MP_S,
	&StaticUC_PLight_S,
	&StaticUC_PLight_MP_S,
	&StaticUC_ALight_S,
	&StaticUC_ALight_MP_S,
		
	&StaticUC_EngA_S,
	&StaticUC_EngA_MP_S,
	
	&StaticUC_SM_S,
	&StaticUC_VTP_S,
	&StaticUC_Shadower_S
} ;


 
VUCodeEntry					StaticEnvMap_E = 
{
	0,
	&Static_EVMap_Entry,
	VU_SEND_BLOCK_ENV_MAP,
	VU_USES_MODEL_BASE_OFFSET,
	0,
	0
} ;

VUCodeEntry					StaticSphereMap_E = 
{
	0,
	&Static_SphereMap_Entry,
	VU_SEND_BLOCK_ENV_MAP,
	VU_USES_MODEL_BASE_OFFSET,
	0,
	0
} ;

VUCodeEntry					StaticRefMap_E = 
{
	0,
	&Static_RefMap_Entry,
	VU_SEND_BLOCK_ENV_MAP,
	VU_USES_MODEL_BASE_OFFSET,
	REQ_CAMERA_TO_WORLD,
	0
} ;

VUCodeEntry					StaticSphereRefMap_E = 
{
	0,
	&Static_SphereRefMap_Entry,
	VU_SEND_BLOCK_ENV_MAP,
	VU_USES_MODEL_BASE_OFFSET,
	REQ_CAMERA_TO_WORLD,
	0
} ;

VUCodeEntry *				StaticCCMArray[] =
{
	&StaticEnvMap_E,
	&StaticSphereMap_E,
	&StaticRefMap_E,
	&StaticSphereRefMap_E,
} ;

VUCodeEntry *				StaticUCMArray[] =
{
	&StaticEnvMap_E,
	&StaticSphereMap_E,
	&StaticRefMap_E,
	&StaticSphereRefMap_E,
} ;

/*
VUCodeSet					Std 		= { &StaticCC_Entry, &StaticCC_Cull_Entry, &StaticCC_Clip_Entry, &GenericClip, &gcSetup, &gcClip } ;
VUCodeSet					All 		= { &StaticCCAll_Entry, &StaticCCAll_Cull_Entry, &StaticCCAll_Clip_Entry, &GenericClip, &gcSetup, &gcClip } ;
VUCodeSet					PLight 	= { 0, 0, &StaticCCAll_ClipPLight_Entry, &GenericClip, &gcSetup, &gcClip } ;
VUCodeSet					VTP 		= { &StaticCCVTP_Entry, &StaticCCVTP_Cull_Entry, &StaticCCVTP_Clip_Entry, &GenericClip, &gcSetup, &gcClip } ;
VUCodeSet					ShM 		= { &StaticCCSMC_Entry, &StaticCCSMC_Cull_Entry, &StaticCCSMC_Clip_Entry, &GenericClip, &gcSetup, &gcClip } ;

VUCodeSet					StdMP 	= { 0, &StaticCCC_MP_Entry, 0, &GenericClip, &gcSetup, &gcClip } ;
VUCodeSet					AllMP 	= { 0, &StaticCCAC_MP_Entry, 0, &GenericClip, &gcSetup, &gcClip } ;

VUCodeSet					VStd		= { &StaticUC_Entry, &StaticUC_Cull_Entry, &StaticUC_Clip_Entry, &GenericClip, &gcSetup, &gcClip } ;
VUCodeSet					VAll		= { &StaticUCAll_Entry, &StaticUCAll_Cull_Entry, &StaticUCAll_Clip_Entry, &GenericClip, &gcSetup, &gcClip } ;
VUCodeSet					VVTP		= { &StaticUCVTP_Entry, &StaticUCVTP_Cull_Entry, &StaticUCVTP_Clip_Entry, &GenericClip, &gcSetup, &gcClip } ;
VUCodeSet					VShM		= { &StaticUCSMC_Entry, &StaticUCSMC_Cull_Entry, &StaticUCSMC_Clip_Entry, &GenericClip, &gcSetup, &gcClip } ;
VUCodeSet					VPLight	= { 0, 0, &StaticUCAll_ClipPLight_Entry, &GenericClip, &gcSetup, &gcClip } ;

VUCodeSet					VStdMP 	= { 0, &StaticUCC_MP_Entry, 0, &GenericClip, &gcSetup, &gcClip } ;
VUCodeSet					VAllMP 	= { 0, &StaticUCAC_MP_Entry, 0, &GenericClip, &gcSetup, &gcClip } ;
*/