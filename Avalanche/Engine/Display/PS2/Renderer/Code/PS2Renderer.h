//
// PS2Renderer.h
// Copyright (C) 2001 by Avalanche Software, All Rights Reserved
//
// Author: Tyler Colbert
// Creation Date: Nov 7, 2001
// 

#ifndef __PS2RENDERER_H__
#define __PS2RENDERER_H__

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// Sony headers...
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

// we need to use libvu0 math routines for now...
#include <libvu0.h>

// and I hate not having typedefs for integer variables (or floats for that matter)
#include <eetypes.h>

#include "../Common/CompilerControl.h"
#include "PS2MaterialInfo.h"

// we need a basic definition for 4x4 matrices.
typedef float PS2Matrix4x4[4][4] ALIGNED(16);
typedef float PS2Vector4[4] ALIGNED(16);

// some useful defines... one bit per matrix	
#define MATRIX_IDENTITY 			0x00000001
#define MATRIX_WORLD_TO_CLIP		0x00000002
#define MATRIX_CAMERA_TO_CLIP		0x00000004
#define MATRIX_VIEW_TO_CLIP		0x00000008
#define MATRIX_SCREEN_TO_CLIP		0x00000010
#define MATRIX_CLIP_TO_SCREEN		0x00000020
#define MATRIX_WORLD_TO_CULL		0x00000040
#define MATRIX_CAMERA_TO_CULL		0x00000080
#define MATRIX_VIEW_TO_CULL		0x00000100
#define MATRIX_SCREEN_TO_CULL		0x00000200
#define MATRIX_CULL_TO_SCREEN		0x00000400
#define MATRIX_WORLD_TO_SCREEN	0x00000800
#define MATRIX_CAMERA_TO_SCREEN	0x00001000
#define MATRIX_VIEW_TO_SCREEN		0x00002000
#define MATRIX_CLIP_TO_VIEW		0x00004000
#define MATRIX_CULL_TO_VIEW		0x00008000
#define MATRIX_WORLD_TO_CAMERA	0x00010000
#define MATRIX_CAMERA_TO_VIEW		0x00020000
#define MATRIX_TEXTURE				0x00040000
#define MATRIX_CAMERA_TO_WORLD	0x00080000
#define MATRIX_WORLD_TO_VIEW		0x00100000
#define MATRIX_VIEW_TO_WORLD		0x00200000
#define MATRIX_VIEW_TO_CAMERA		0x00400000
#define MATRIX_VIEW_TO_MODEL	   0x10000000
#define MATRIX_CAMERA_TO_MODEL   0x20000000
#define MATRIX_WORLD_TO_MODEL	   0x40000000

// requirements for the intermediate space (the space that M1 passed to
// the handlers prepDMA will convert the model into).
#define REQ_M1_SPACE_DONT_CARE  0x00000000
#define REQ_M1_SPACE_WORLD		  0x00000001
#define REQ_M1_SPACE_CAMERA	  0x00000002
#define REQ_M1_SPACE_VIEW		  0x00000003
#define REQ_M1_SPACE_SCREEN	  0x00000004
#define REQ_M1_SPACE_CULL		  0x00000005
#define REQ_M1_SPACE_CLIP		  0x00000006
#define REQ_M1_SPACE_MASK		  0x0000000F

// requirements for the final space (the space that M1*M2 passed to
// the handlers prepDMA will convert the model into).
#define REQ_M1M2_SPACE_VIEW	  0x00000010
#define REQ_M1M2_SPACE_CULL	  0x00000020
#define REQ_M1M2_SPACE_CLIP	  0x00000030
#define REQ_M1M2_SPACE_SCREEN	  0x00000040
#define REQ_M1M2_SPACE_MASK	  0x000000F0

// requirements for which lights you'll be using
#define REQ_LIGHT_DIRECTIONAL_0 	0x00001000
#define REQ_LIGHT_DIRECTIONAL_1 	0x00002000
#define REQ_LIGHT_DIRECTIONAL_2 	0x00004000
#define REQ_LIGHT_DIRECTIONAL_3 	0x00008000

#define REQ_LIGHT_POINT_0		  	0x00010000
#define REQ_LIGHT_POINT_1		  	0x00020000

#define REQ_LIGHT_DIRECTIONALS	(REQ_LIGHT_DIRECTIONAL_0 | REQ_LIGHT_DIRECTIONAL_1 | REQ_LIGHT_DIRECTIONAL_2 | REQ_LIGHT_DIRECTIONAL_3)
#define REQ_LIGHT_POINTS			(REQ_LIGHT_POINT_0 | REQ_LIGHT_POINT_1)

#define REQ_LIGHT_MASK			  	0x000FF000

#define REQ_CAMERA_TO_WORLD		0x00100000

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// forward declarations to reduce header dependancies.  NOTE! is it important that
// no headers be included other than sonys & standard ones.  Any others will cause
// the engine to have additional dependancies and complicate the interface.
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
class DMAObject;
class DDMAObject;
class MeshInterface;
class VUDataClass;

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// before you can ever hope to get the renderer up and running, you should create
// global functions that match these prototypes.  The renderer calls them to accomplish
// its memory management requirements.
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
extern void* PS2RendererMalloc(u_int size);
extern void* PS2RendererMemAlign(u_int alignment, u_int size);
extern void PS2RendererFree(void* block);
extern bool PS2RendererCouldAlloc(u_int alignment, u_int size);

/////////////////////////////////////////////////////////////////////////////////////////
// this define controls the maximum number of lights that the API will allow into the
// renderer... 
/////////////////////////////////////////////////////////////////////////////////////////
#define MAX_DIRECTIONAL_LIGHTS	4
#define MAX_POINT_LIGHTS			3
#define MAX_LIGHTS					(MAX_DIRECTIONAL_LIGHTS+MAX_POINT_LIGHTS)

enum
{
	LIGHT_TYPE_DIRECTIONAL = 0,
	LIGHT_TYPE_POINT
} ;

enum EnvMapSpace
{
	ENVMAP_SPACE_CAMERA = 0,
	ENVMAP_SPACE_VIEW,
	ENVMAP_SPACE_WORLD,
	ENVMAP_SPACE_MAP,
	ENVMAP_SPACE_NOCALC = 0xffff
} ;


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// here starts the public interface
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

namespace PS2Renderer {

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// initialize & cleanup the renderer...
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	void Init(u_int vuMemBase, u_int vuMemSize);
	void Cleanup(void);

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// ways to get/set the matrices that the renderer is using.
	//
	// if clipping or culling are on, goes like (with each arrow being a matrix,
	// and each word being a space):
	//                         
	//
	//                        /-> clip -\
	//    world->camera->view             -> screen
	//                        \-> cull -/
	//
	// otherwise, its just
	//
	//    world->camera->view->screen
	//
	//
	// currently the view->clip, view->cull, and view->screen  matrices must
	// be scale & translation only.
	//
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	bool Invert( PS2Matrix4x4 &Dest, const PS2Matrix4x4 &Src) NAMED_SECTION("text.rend1");
	void Normalize( PS2Matrix4x4 &Dest, const PS2Matrix4x4 &Src);
		
	void SetEVMapTextureBind( unsigned int uiTexBind);
	void EnableEnvMapping( unsigned int uiBind, bool bSphericalNormals, EnvMapSpace evSpace, const PS2Matrix4x4 * pPostTransform );
	void EnableSphereMapping( unsigned int uiBind, bool bSphereicalNormals, const PS2Vector4 & eyeWorld, const PS2Matrix4x4 * pWorldToSphere );
	void DisableEnvMapping( void ) NAMED_SECTION("text.rend1");
	
	// set matrices
	void SetWorldToCamera(const PS2Matrix4x4& mtx);
	void SetCameraToView(const PS2Matrix4x4& mtx);
	void SetViewToScreen(const PS2Matrix4x4& mtx);
	void SetViewToClip(const PS2Matrix4x4& mtx);
	void SetViewToCull(const PS2Matrix4x4& mtx);

	// get matrices
	const PS2Matrix4x4& GetWorldToCamera(void);
	const PS2Matrix4x4& GetCameraToView(void);
	const PS2Matrix4x4& GetViewToScreen(void);
	const PS2Matrix4x4& GetViewToClip(void);
	const PS2Matrix4x4& GetViewToCull(void);

	// get generated matrices
	const PS2Matrix4x4& GetWorldToClip(void);
	const PS2Matrix4x4& GetCameraToClip(void);
	const PS2Matrix4x4& GetScreenToClip(void);
	const PS2Matrix4x4& GetWorldToScreen(void);
	const PS2Matrix4x4& GetCameraToScreen(void);
	const PS2Matrix4x4& GetClipToScreen(void);
	const PS2Matrix4x4& GetClipToView(void);
	const PS2Matrix4x4& GetWorldToCull(void);
	const PS2Matrix4x4& GetCameraToCull(void);
	const PS2Matrix4x4& GetScreenToCull(void);
	const PS2Matrix4x4& GetCullToScreen(void);
	const PS2Matrix4x4& GetCullToView(void);

	// then there is the texture matrix
	void 						SetTexture(const PS2Matrix4x4& mtx ) NAMED_SECTION("text.rend1");
	const PS2Matrix4x4& 	GetTexture(void );

	void 						SetConstantAlphaMultiplier( float alphaMultiplier ) NAMED_SECTION("text.rend1");
	float 					GetConstantAlphaMultiplier( void );

	void 						SetFog( float fogNear, float fogFar ) NAMED_SECTION("text.rend1");
	void 						GetFog( float &fogNear, float &fogFar );

	// these are helpers for setting the ViewToClip or ViewToCull
	void SetViewToClip(u_int frameBufferWidth, u_int frameBufferHeight); // full screen support guardband
	void SetViewToCull(u_int frameBufferWidth, u_int frameBufferHeight); // full screen support guardband

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// ways to get/set lights.
	// colors are floats from 0.0f - 128.0f (NOT 127.0f!)
	// flags are below (currently only the LIGHT_SPACE_* ones
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	void SetLight(int number, const PS2Vector4& direction, const PS2Vector4& ambientcolor, const PS2Vector4& diffusecolor, const PS2Vector4& specularcolor, u_int flags, u_int type, float attenuation );
	void EnableLight(int number, bool Enabled, u_int type );
	void SetAmbientLight(const PS2Vector4& color);
	void SetShadowRGB( u_char R, u_char G, u_char B);
	
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// call this to add an object to the MasterList
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	void DrawDMAObject(DDMAObject& object, const PS2Matrix4x4& ModelMatrix, u_int flags) NAMED_SECTION("text.rend1");

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// This routine converts a raw data buffer from a file
	// into a DDMAObject.  Some types of objects take some
	// additional information, hence the patchSet and patchData
	// values.  For normal static objects, these can be
	// 0 and NULL respectively.
	//
	// The creation routine is not generalized.  There is a
	// different patch routine for each handler (VUDataClass)
	// that supports it.  This routine finds the handler and
	// asks it to patch the dma object.
	// This results in a DDMAObject which
	// can be directly drawn using the call above.
	// The new patched object can refer to the data block's data.  
	// So dont free the data block until your done with all the 
	// objects.
	//
	// The patchSet parameter allows you to select which patch
	// set you want, if there are different patch sets for the
	// model.  Currently patches are sets of vertex colors
	// so a model has a specific number of them preburnt into it.
	//
	// There is also a generic void* patchData that can be used
	// by the patch routine for additional data.  The DMAObject
	// flags determine what that data should be.
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	unsigned int 	getDMAObjectVertices( DDMAObject *pObject);
	bool				checkDDMAObjectFlag( DDMAObject * pObject, unsigned int flag );

	void 				fixupDDMAObjectTextures( void * DMAModelHdrs, unsigned int numPtrs);
	DDMAObject * 	createDDMAObject( void * DMAModelHdrs, unsigned int numPtrs, unsigned int patchSet) NAMED_SECTION("text.rend1");
	void 				destroyDDMAObject( DDMAObject* object);

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// As it turns out, we need to do two passes at fixup, one for the model and one for
	// the instance.  so this is the model fixup routine
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	void FixupModel(void* dataBuffer, u_int dataSize);
	
		
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// this returns the number of patch sets available for this DMA Object.
	// if its 0, then calling PatchDMAObject on this object will simply
	// return the a new object identical to the one you passed (it will refer
	// to the others dma data just like a patched object)...
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	u_int getPatchSetCount( void * dataBuffer);

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// the patch routines need to be able to resolve texture IDs into binds
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	typedef u_int TextureBindCallback(u_int textureID, u_int textureLayer);
	void SetTextureBindCallback(TextureBindCallback* callback);

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// the render routines need to be able to resolve texture binds into GS settings.
	// this is how that gets done.
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	typedef void* TextureDataCallback(u_int bind, bool includeAlpha1, PS2MaterialInfo *pMatInfo);
	void SetTextureDataCallback(TextureDataCallback* callback);

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// use this to tell the renderer that you just trashed a chunk of vu instruction mem...
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	void JustTrashedVUInstructionMem(u_int startAddr, u_int instructionCount);
	
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// flags for DrawDMAObject
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////

// microcode flags
// bits  0 -  9					PRIM register data
// bits 10 - 12					MTX type flags
// bits 16 - ??					MICROCODE flags

// bit  31							toggle lights

// an option to force LINE_STRIPs instead of STRIPS/FANS/TRIS
#define WIREFRAME_MODE		(1 << 2)

// PRIM register bits
#define GS_GOURAUD			(1 << 3)
#define GS_TEXTURED			(1 << 4)
#define GS_FOGGING			(1 << 5)
#define GS_ALPHABLEND		(1 << 6)
#define GS_ONE_PASS_AA		(1 << 7)
#define GS_CONTEXT2			(1 << 9)

// PRIM register mask
#define GS_PRIM_MASK			(GS_GOURAUD|GS_TEXTURED|GS_FOGGING|GS_ALPHABLEND|GS_ONE_PASS_AA|GS_CONTEXT2)

// one of these flags must be specified (model matrix format flags)
// specifying any of these but LOCAL_TO_CLIP with clipping enabled incurs
// an additional matrix concat to generate the correct matrix for the microcode...

// matrix space flags
#define LOCAL_TO_WORLD		(0x1 << 10)	//
#define LOCAL_TO_CAMERA		(0x2 << 10)	// LOCAL_TO_WORLD x camera
#define LOCAL_TO_VIEW		(0x3 << 10)	// LOCAL_TO_CAMERA x perspective (should put model into cannonical view volume)
#define LOCAL_TO_SCREEN		(0x4 << 10)	// LOCAL_TO_VIEW x view (should put model into GS mem frame buffer coords)
#define LOCAL_TO_CLIP		(0x5 << 10)	// LOCAL_TO_VIEW x clip (put model into clip space (adds guard band))
#define LOCAL_TO_CULL		(0x6 << 10) // LOCAL_TO_VIEW x cull (put model into cull space (adds guard band))

// masks for flag values
#define LOCAL_MATRIX_MASK	(0x7 << 10)

// microcode flags
#define MICROCODE_CLIP								(1 << 16)
#define MICROCODE_CULL								(1 << 17)

#define MICROCODE_FOG								(1 << 18)

#define MICROCODE_TEXTURE_MTX						(1 << 19)

#define MICROCODE_SHADOWER							(1 << 20)
#define MICROCODE_SHADOWED 						(1 << 21)

#define MICROCODE_VERTEX_TEXTURE_PROJECTION 	(1 << 22)

#define MICROCODE_ENV_MAP							(1 << 23)

#define MICROCODE_ENABLE_LIGHTING				(1 << 24)
	
#define MICROCODE_POINT_LIGHTS					(1 << 25)
#define MICROCODE_DIRECTIONAL_LIGHTS			(1 << 26)

#define MICROCODE_MASK_SECOND_RENDER 			(MICROCODE_SHADOWED | MICROCODE_VERTEX_TEXTURE_PROJECTION )
#define MICROCODE_MASK_MULTIPASS					(MICROCODE_ENV_MAP )

#define USE_LIGHTS									(MICROCODE_ENABLE_LIGHTING)
// TJC - with this flag, be aware that currently, this just forces the lights to be sent
// to the VU as black directionals and all white ambient...  If you want to speed up the
// drawing of models, you probably need 2 different models...
// this flag is handled entirely in PS2Renderer (or at least is was when I wrote this
// comment... :)
#define CARTOON_LIGHTS			(1 << 30)

	

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
// these are flags passed into SetDirectionalLight()
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

// tell us the space that the light was defined in
#define LIGHT_SPACE_WORLD		  0x00000001
#define LIGHT_SPACE_CAMERA		  0x00000002
#define LIGHT_SPACE_VIEW		  0x00000003

// these light space flags are used internally by the renderer and should never be passed to SetDirectionalLight
#define LIGHT_SPACE_MODEL		  0x0000000E
#define LIGHT_SPACE_NONE		  0x0000000F

#define LIGHT_SPACE_MASK		  0x0000000F

} // namespace PS2Renderer

#endif // __PS2RENDERER_H__

