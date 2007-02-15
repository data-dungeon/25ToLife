#ifndef __VU_CODE_H
#define __VU_CODE_H

#include "VUManagedCode.h"
#include "VUManagedFunction.h"

extern VUManagedCode				&GenericClip;

extern VUManagedFunction		&gcSetup;
extern VUManagedFunction		&gcClip;

/*---------------------------------------------------------------------------------------------------------------------------*
 *	Static (Compressed Color)
 *---------------------------------------------------------------------------------------------------------------------------*/
 
// Basic
extern VUManagedCode				&StaticCC;	
extern VUManagedFunction 		&StaticCC_Entry;

extern VUManagedCode				&StaticCC_Cull;	
extern VUManagedFunction 		&StaticCC_Cull_Entry;

extern VUManagedCode				&StaticCC_Clip;	
extern VUManagedFunction 		&StaticCC_Clip_Entry;

extern VUManagedCode				&StaticCC_Clip2;	
extern VUManagedFunction 		&StaticCC_Clip2_Entry;

// All (Fog, AlphaMul, TexMtx) 
extern VUManagedCode				&StaticCCAll;
extern VUManagedFunction		&StaticCCAll_Entry;

extern VUManagedCode				&StaticCCAll_Cull;
extern VUManagedFunction		&StaticCCAll_Cull_Entry;

extern VUManagedCode				&StaticCCAll_Clip;
extern VUManagedFunction		&StaticCCAll_Clip_Entry;

extern VUManagedCode				&StaticCCAllPL_Clip;
extern VUManagedFunction		&StaticCCAllPL_Clip_Entry;

// Texture Projection (vertex)
extern VUManagedCode				&StaticCCVTP;
extern VUManagedFunction		&StaticCCVTP_Entry;

extern VUManagedCode				&StaticCCVTP_Cull;
extern VUManagedFunction		&StaticCCVTP_Cull_Entry;

extern VUManagedCode				&StaticCCVTP_Clip;
extern VUManagedFunction		&StaticCCVTP_Clip_Entry;

// Shadow Map Custom
extern VUManagedCode				&StaticCCSMC;
extern VUManagedFunction		&StaticCCSMC_Entry;

extern VUManagedCode				&StaticCCSMC_Cull;
extern VUManagedFunction		&StaticCCSMC_Cull_Entry;

extern VUManagedCode				&StaticCCSMC_Clip;
extern VUManagedFunction		&StaticCCSMC_Clip_Entry;


// Multipass
extern VUManagedCode				&StaticCCAC_MP;
extern VUManagedFunction		&StaticCCAC_MP_Entry;

// EVMap Stuff
extern VUManagedCode				&Static_EVMap;
extern VUManagedFunction		&Static_EVMap_Entry;

/*---------------------------------------------------------------------------------------------------------------------------*
 *	Static (Uncompressed Color)
 *---------------------------------------------------------------------------------------------------------------------------*/

// Basic
extern VUManagedCode				&StaticUC;
extern VUManagedFunction 		&StaticUC_Entry;

extern VUManagedCode				&StaticUC_Cull;
extern VUManagedFunction 		&StaticUC_Cull_Entry;

extern VUManagedCode				&StaticUC_Clip;
extern VUManagedFunction 		&StaticUC_Clip_Entry;

// All (Fog, AlphaMul, TexMtx) 
extern VUManagedCode				&StaticUCAll;
extern VUManagedFunction		&StaticUCAll_Entry;

extern VUManagedCode				&StaticUCAllPLight;
extern VUManagedFunction		&StaticUCAllPLight_Entry;

extern VUManagedCode				&StaticUCAll_Cull;
extern VUManagedFunction		&StaticUCAll_Cull_Entry;

extern VUManagedCode				&StaticUCAll_Clip;
extern VUManagedFunction		&StaticUCAll_Clip_Entry;

extern VUManagedCode				&StaticUCAllPL_Clip;
extern VUManagedFunction		&StaticUCAllPL_Clip_Entry;

// Texture Projection (vertex)
extern VUManagedCode				&StaticUCVTP;
extern VUManagedFunction		&StaticUCVTP_Entry;

extern VUManagedCode				&StaticUCVTP_Cull;
extern VUManagedFunction		&StaticUCVTP_Cull_Entry;

extern VUManagedCode				&StaticUCVTP_Clip;
extern VUManagedFunction		&StaticUCVTP_Clip_Entry;

// Shadow Map Custom
extern VUManagedCode				&StaticUCSMC;
extern VUManagedFunction		&StaticUCSMC_Entry;

extern VUManagedCode				&StaticUCSMC_Cull;
extern VUManagedFunction		&StaticUCSMC_Cull_Entry;

extern VUManagedCode				&StaticUCSMC_Clip;
extern VUManagedFunction		&StaticUCSMC_Clip_Entry;

extern VUManagedCode				&StaticUC_EngA_Clip;
#endif