//============================================================================
//=
//= PatchDisp_PS2.cpp - PS2 specific functions for rendering
//=
//============================================================================

#include "Game/Managers/SceneMgr.h"
#include "Game/Managers/TextureMgr/PlatformImage.h"

// array to hold the final local to camera matrix for each bone of this object
// (pre-computed once so it's not computed over and over as bones are re-selected)
static Graphics4x4 pBoneLocal2CamMatrix[MAX_OBJECT_BONES] __attribute__ ((aligned(BYTES_PER_QUAD_WORD)));

//============================================================================

static inline void CopyMatriciesIntoPlace(AnimDDMAObject *p_AnimObject, ts_Body *pBody)
{
	unsigned int	sizeMatrix = p_AnimObject->getMatrixEntrySize( );
	PS2Matrix4x4 * pMatrices = (PS2Matrix4x4 *) p_AnimObject->activateMatrixBuffer( MasterList::GetFrameBeingGenerated( ) );
	
	for( unsigned int i = 0; i < pBody->usBones; i++ )
	{
		ASSERT(i <= p_AnimObject->d_MatrixCount);

		ts_Bone* pBone = &pBody->pBones[ i ];

#ifndef SN
		memcpy( pMatrices, &pBone->Matrix, sizeof( PS2Matrix4x4) );
#else
		// just copy them for now.. this is an attempt to avoid the ICache miss of memcpy...
		u128 temp1;
		u128 temp2;
		u128 temp3;
		u128 temp4;
		Graphics4x4* mtx = (Graphics4x4*)pMatrices;
		asm("lq %0, %1" : "=r" (temp1) : "m" (pBone->Matrix.row0));
		asm("lq %0, %1" : "=r" (temp2) : "m" (pBone->Matrix.row1));
		asm("lq %0, %1" : "=r" (temp3) : "m" (pBone->Matrix.row2));
		asm("lq %0, %1" : "=r" (temp4) : "m" (pBone->Matrix.row3));
		asm("sq %1, %0" : "=m" (mtx->row0) : "r" (temp1));
		asm("sq %1, %0" : "=m" (mtx->row1) : "r" (temp2));
		asm("sq %1, %0" : "=m" (mtx->row2) : "r" (temp3));
		asm("sq %1, %0" : "=m" (mtx->row3) : "r" (temp4));
#endif
		
		pMatrices = (PS2Matrix4x4 *) (((unsigned int) pMatrices) + sizeMatrix);
	}
}

//============================================================================
void PatchDisp__Cleanup( )
{

}

//============================================================================
Matrix4x4		EnvMapTransform;

void PatchSOM__InitEnvMap( ts_PatchSOM * pPatchSOM ) NAMED_SECTION("text.rend1");

void PatchSOM__InitEnvMap( ts_PatchSOM * pPatchSOM )
{
	static ts_TextureLayer	HackedUpEnvMapTextureLayer;


	unsigned int tex;
	for( tex = 0; tex < pPatchSOM->pTextureArray->TextureCount( ); tex++ )
	{
		Texture * pTex = pPatchSOM->pTextureArray->GetTexture( tex );

		unsigned int		flags = pTex->GetFlags( );

		bool					bSpherical = flags & Texture::MAP_SPHERICAL ? true : false;

		if( flags & Texture::ENV_MAP )
		{
			HackedUpEnvMapTextureLayer.u8WrapU = TS_WRAP_REPEAT;
			HackedUpEnvMapTextureLayer.u8WrapV = TS_WRAP_REPEAT;
			HackedUpEnvMapTextureLayer.u8Filter = TS_FILTER_BILINEAR;
			HackedUpEnvMapTextureLayer.u8Blend = RS_BLENDFUNC_ADDITIVE;
			HackedUpEnvMapTextureLayer.u8Method = TS_METHOD_MODULATE;

			HackedUpEnvMapTextureLayer.textureDatabase = pPatchSOM->pTextureDatabase;

			HackedUpEnvMapTextureLayer.s16TextureIndex = (s16) pPatchSOM->pTextureDatabase->FindTexture( pTex );

			if( flags & Texture::REF_MAP )
			{
				Vector4				Cam0( 0.0, 0.0, 0.0, 1.0 );
				Vector4				eyeWorld;

				CScene * pScene = SceneMgr::GetCurrentScene( );
				ASSERT( pScene );
				
				eyeWorld = Cam0 * pScene->d_cameraToWorld;

				PS2Renderer::SetWorldToCamera( * (PS2Matrix4x4 *) & g4WorldToCamera );
				
				PS2Renderer::EnableSphereMapping( (unsigned int) & HackedUpEnvMapTextureLayer, bSpherical, *(PS2Vector4 *) &eyeWorld, NULL );
				return;
			}
			else
			{
				EnvMapSpace space = (EnvMapSpace) ((flags & Texture::MAP_SPACE_MSK ) >> 8);

				EnvMapTransform.Identity( );
				EnvMapTransform.r00 = 0;
				EnvMapTransform.r11 = 0;
				EnvMapTransform.r12 = 0;

				switch( (flags >> 16) & 0x0f )
				{
				default:
				case 0:
					EnvMapTransform.r00 = 1.0f;
					break;
				case 1:
					EnvMapTransform.r10 = 1.0f;
					break;
				case 2:
					EnvMapTransform.r20 = 1.0f;
					break;
				}

				switch( (flags >> 12) & 0x0f )
				{
				default:
				case 0:
					EnvMapTransform.r01 = 1.0f;
					break;
				case 1:
					EnvMapTransform.r11 = 1.0f;
					break;
				case 2:
					EnvMapTransform.r21 = 1.0f;
					break;
				}

				PS2Renderer::EnableEnvMapping( (unsigned int) & HackedUpEnvMapTextureLayer, bSpherical, space, (PS2Matrix4x4 *) &EnvMapTransform );
				return;
			}
		}
	}

		PS2Renderer::DisableEnvMapping( );
}

//============================================================================
void PatchSOM__Render(ts_PatchSOM **pPatchSOMList, int numSOMs, Graphics4x4 *pmLocalToWorld, ts_DisplayListRecord *pDisplayListRecord, void *pVertexColors, ts_PatchSOM__RenderFlags ucFlags, ts_Body *pBody)
{
	ts_PatchSOM *	pPatchSOM = pPatchSOMList[ 0 ];

	// PS2 Specific render function...
	
//	MatrixStack__Mode( MS_MODELVIEW );
//	MatrixStack__Load( pmLocalToCamera );

#ifdef STICK_FIGURES
	/* draw boned models as stick figures for AttachMgr analysis */

	if (RenderMgr__BoneRender() && pPatchSOM->dh.u16Flags & SOMM_HAS_BONES)
	{
		if (PatchSOM__DrawStickFigure(pPatchSOM, pBody))	// returns true if stick figure was drawn
			return;
	}
#endif //STICK_FIGURES

	// get the proper record... (instance record is passed,
	// if its null, then use the som's record...)
	if(!pDisplayListRecord->pDisplayList)
	{
		pDisplayListRecord = &pPatchSOM->DisplayListRecord;
		ASSERT(pDisplayListRecord->u16QuadWordCount == 0); // ya.. its a hack.. ick...0 means that it's been fixed up by _FixDMASOM
	}

	// is there something to draw?
	if(!pDisplayListRecord->pDisplayList)
		return;

	// default flags for all modes..
	u32 renderFlags = LOCAL_TO_CAMERA | GS_TEXTURED | GS_GOURAUD;

	// clipping flags
	if(RenderState__Get(RS_VU_CLIPPING) == RS_CLIPPING_CLIP)
		renderFlags |= MICROCODE_CLIP;
	else if(RenderState__Get(RS_VU_CLIPPING) == RS_CLIPPING_CULL)
		renderFlags |= MICROCODE_CULL;

#ifndef CDROM
	if( ( renderFlags & MICROCODE_CULL ) && ( pPatchSOM->dh.u16Flags & SOMM_HAS_BONES ) && g_replayMgr.IsPlaying( ) )
	{
		renderFlags &= ( ~MICROCODE_CULL );
		renderFlags |= MICROCODE_CLIP;
	}
#endif
	
	// deal with shadows
	if (RenderMgr__RenderingProjectedTextures())
	{
//#ifndef CONSUMER_BUILD
		// dont draw shadows in wireframe...
		if(RenderMgr__WireframeOn())
			return;
//#endif

		// dont draw shadows on boned objects..
		if(pPatchSOM->dh.u16Flags & SOMM_HAS_BONES)
			return;

		// always use alpha blending & shadow code
		if (ShadowMgr__RenderingShadows())
			renderFlags |= GS_ALPHABLEND | MICROCODE_SHADOWED;	// shadow rendering has its own microcode
		else
#ifndef TEST_PROJECTED
			renderFlags |= GS_ALPHABLEND | MICROCODE_SHADOWED;
#else //TEST_PROJECTED
if (RenderMgr__FogEnabled())
	renderFlags |= GS_ALPHABLEND | MICROCODE_VERTEX_TEXTURE_PROJECTION;
else
	renderFlags |= GS_ALPHABLEND | MICROCODE_SHADOWED;
#endif //TEST_PROJECTED
	}
	else
	{
		// TJC - always use alphablending for now...
//		if(pPatchSOM->dh.u16Flags & SOMM_ALPHA_SORT)
		renderFlags |= GS_ALPHABLEND;

		// we must always reload the blend func since some models change it...
		RenderState__Request(RS_BLENDFUNC, RS_BLENDFUNC_DEFAULT);

		// lighting is conditional...
		if ( ( pPatchSOM->dh.u16Flags & SOMM_TRI_STRIP_NORMALS ) &&
			  pRenderInst->DynamicallyLit() 
#ifndef CONSUMER_BUILD
			  && RenderMgr__LightingOn()
#endif
			  )
		{
			if(RenderMgr__CartoonRenderer())
			{
				// setup cartoon mode
				renderFlags |= CARTOON_LIGHTS;
				GSState__Set(SCE_GS_PABE, 1);
				GSState__Set(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1(0, 2, 2, 2, 0xe0));
			}
			
			/* enable lighting */
			RenderState__Request( RS_LIGHTING, RS_TRUE );
			RenderState__Request( RS_SEND_LIGHTS, RS_TRUE );
			renderFlags |= USE_LIGHTS;
		}
		else
		{
			/* disable lighting */
			RenderState__Request( RS_LIGHTING, RS_FALSE );
		}
		
		// set the constant alpha multiplier
		PS2Renderer::SetConstantAlphaMultiplier(RenderState__GetConstantAlpha());

		// AA1 method of wireframing the models...
//#ifndef CONSUMER_BUILD
		if(RenderMgr__WireframeOn())
		{
			// set test1 to only include the alpha stuff...
			GSState__Set(SCE_GS_TEST_1, SCE_GS_SET_TEST_1(
								 1, 	// ate, alpha test enable, 1 = on
								 0x7, 	// atst, alpha test method, 111 = notequal
								 0x80, // aref, alpha reference value for alpha test, 0x80 = opaque
								 0x0, 	// afail, what to do when alpha test fails, 0 = KEEP (transparent wireframe) 10=ZB_ONLY (opaque wireframe)
								 0, 	// date, destination alpha test, 0 = off, 1 = on
								 1, 	// datm, destination alpha test mode, 1 bit
								 1, 	// zte, depth test, 1 = on (TRC Requirement)
								 0x3));	// ztst, depth test method, 0 = never, 1 = always, 2 = gequal, 3 = greater
			// force one pass AA
			renderFlags |= GS_ONE_PASS_AA;
			// turn off standard alpha blending
			renderFlags &= ~GS_ALPHABLEND;
			// draw the lines dark (no alpha blending on the lines)
			GSState__Set(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1(2, 2, 0, 0, 0));
		}
//#endif // CONSUMER_BUILD
	}

	// some statistics
	SceneMgr::GetStats().IncVertices( pPatchSOM->u32TriStripVertexColorCount );
	SceneMgr::GetStats().IncPolys( pPatchSOM->u32TriStripVertexColorCount - (2 * pPatchSOM->u32TriStripCount) );
	SceneMgr::GetStats().IncTriStripPolys( pPatchSOM->u32TriStripVertexColorCount - (2 * pPatchSOM->u32TriStripCount) );
	if(pBody)
		SceneMgr::GetStats().IncBonesRendered( pBody->usBones );

	// get the object
	DDMAObject *	p_DDMAObject = (DDMAObject *) pDisplayListRecord->pDisplayList;

	// copy bones into place...
	if(pPatchSOM->dh.u16Flags & SOMM_HAS_BONES)
	{
		AnimDDMAObject *p_AnimObject = static_cast<AnimDDMAObject *>( p_DDMAObject);
			
		if( p_AnimObject->getMatrixBufferFrameID( ) != MasterList::GetFrameBeingGenerated( ) )
		{
#ifdef PROFILE_RENDERER
			Profiler__Start(PROFILE_COPY_BONES);
#endif // PROFILE_RENDERER

			CopyMatriciesIntoPlace(p_AnimObject, pBody);

#ifdef PROFILE_RENDERER
			Profiler__Stop(PROFILE_COPY_BONES);
#endif // PROFILE_RENDERER
		}
	}

	// is fog on?
	bool bFog = ((RenderState__Get(RS_FOG))
#ifndef CONSUMER_BUILD		
		&& (RenderMgr__FogEnabled())
#endif
		);
	if ((bFog) || (RenderState__Get(RS_FOG_RENDER_PATH)))
	{
		PS2Renderer::SetFog(SceneMgr::GetCurrentScene()->d_sceneView.GetNearFog(), SceneMgr::GetCurrentScene()->d_sceneView.GetFarFog());

		// the texture matrix is only supported in fog rendering path. may want that path without actually fogging
		if (bFog  &&  !ShadowMgr__RenderingShadows( ))
			renderFlags |= GS_FOGGING | MICROCODE_FOG;	// real fogging
		else
			renderFlags |= MICROCODE_FOG;						// texture matrix support, no fogging

		// push up a fog register setting as well...
		GSState__Set(SCE_GS_FOG, SCE_GS_SET_FOG(255));
	}

#ifdef PS2_LABEL_MODELS
	// set the models name into the GS Label register
	GSState__Set(SCE_GS_LABEL, SCE_GS_SET_LABEL((u32)&pPatchSOM->Name[0], 0xFFFFFFFF));
#endif

	// send all the state changes down...
	GSState__Send();
	
	// fire off the model...
#ifdef PROFILE_RENDERER
	Profiler__Start(PROFILE_BUILD_MASTER_LIST);
#endif // PROFILE_RENDERER

	Matrix4x4 LocalToCamera = (*pmLocalToWorld) * g4WorldToCamera;
	PatchSOM__InitEnvMap( pPatchSOM );

	PlatformImage::ClearContextChangeCount();

	PS2Renderer::DrawDMAObject( *p_DDMAObject, *(PS2Matrix4x4 *) &LocalToCamera, renderFlags);

	for ( int somIndex = 1 ; somIndex < numSOMs ; somIndex++ )
	{
		AnimDDMAObject* master = static_cast<AnimDDMAObject*>( p_DDMAObject );
		AnimDDMAObject* dmaObject = (AnimDDMAObject*) pDisplayListRecord[ somIndex ].pDisplayList;
		dmaObject->SetBufferIndex( master->GetBufferIndex() );
		dmaObject->b_MatrixBufferLightsSet = true;	// don't redo lighting, it's already in the base
		PatchSOM__InitEnvMap( pPatchSOMList[ somIndex ] );
		PS2Renderer::DrawDMAObject( *dmaObject, *(PS2Matrix4x4 *) & LocalToCamera, renderFlags );
	}

	u32 contextChanges = PlatformImage::GetContextChangeCount();
	SceneMgr::GetStats().d_ContextChangesPerFrame += contextChanges;
	if(contextChanges > SceneMgr::GetStats().MaxContextChangesPerModel())
		SceneMgr::GetStats().d_MaxContextChangesPerModel = contextChanges;

#ifdef PS2_LABEL_MODELS
	// clear the GS Label register
	GSState__Set(SCE_GS_LABEL, SCE_GS_SET_LABEL(NULL, 0xFFFFFFFF));
#endif

#ifdef PROFILE_RENDERER
	Profiler__Stop(PROFILE_BUILD_MASTER_LIST);
#endif // PROFILE_RENDERER

	// restore lighting if we were in cartoon mode
	if(renderFlags & CARTOON_LIGHTS)
	{
		// restore from cartoon mode
		GSState__Set(SCE_GS_PABE, 0);
		GSState__Set(SCE_GS_ALPHA_1, GSState.ALPHA_1);
	}

	// restore stuff if we were in wireframe...
//#ifndef CONSUMER_BUILD
	if(RenderMgr__WireframeOn())
	{
		// restore gs settings
		GSState__Set(SCE_GS_TEST_1, GSState.TEST_1);
		GSState__Set(SCE_GS_ALPHA_1, GSState.ALPHA_1);
	}
//#endif // CONSUMER_BUILD
}

//============================================================================

void PatchSOM__RenderPS2ShadowModel( ts_PatchSOM **pPatchSOMList, int numSOMs, Graphics4x4 *pmLocalToCamera, ts_DisplayListRecord *pDisplayListRecord, ts_Body *pBody)
{
	ts_PatchSOM* pPatchSOM = pPatchSOMList[ 0 ];

static unsigned int ShowDebug = 0;

	/* we are messing with the modelling transforms at this point */
	MatrixStack__Mode( MS_MODELVIEW );

	/* no alpha test */
	RenderState__Request( RS_ALPHATEST, RS_TRUE);
	RenderState__Request( RS_ALPHAFUNC, RS_ALPHAFUNC_ALWAYS);

	/* no z buffer compare */
	RenderState__Request( RS_ZFUNC, RS_ZFUNC_ALWAYS);

	/* no z buffer writes */
	RenderState__Request( RS_ZWRITE, RS_FALSE);

	/* no - lighting */
	RenderState__Request( RS_LIGHTING, RS_FALSE );

	/* we are running a display list */
	RenderState__Request( RS_PRIMITIVE, RS_PRIMITIVE_DISPLAYLIST );

	/* run display list for non-animated models */
	MatrixStack__Load( pmLocalToCamera );

	u32 renderFlags = LOCAL_TO_CAMERA | MICROCODE_SHADOWER;

	// get the proper record... (instance record is passed, if it's null, then use the som's record...)
	if(!pDisplayListRecord->pDisplayList)
	{
		pDisplayListRecord = &pPatchSOM->DisplayListRecord;
		ASSERT(pDisplayListRecord->u16QuadWordCount == 0); // ya.. its a hack.. ick...0 means that it's been fixed up by _FixDMASOM
	}
	DDMAObject *p_DDMAObject = (DDMAObject *) pDisplayListRecord->pDisplayList;
	ASSERT(p_DDMAObject);	// must have data

	if (pPatchSOM->dh.u16Flags & SOMM_HAS_BONES)
	{
		AnimDDMAObject *p_AnimObject = static_cast<AnimDDMAObject *>( p_DDMAObject);

		if( p_AnimObject->getMatrixBufferFrameID( ) != MasterList::GetFrameBeingGenerated( ) )
		{
			Profiler__Start(PROFILE_COPY_BONES);

			// put bones into the dma object... how...?
			unsigned int	sizeMatrix = p_AnimObject->getMatrixEntrySize( );
			PS2Matrix4x4 * pMatrices = (PS2Matrix4x4 *) p_AnimObject->activateMatrixBuffer( MasterList::GetFrameBeingGenerated( ) );

			for( unsigned int i = 0; i < pBody->usBones; i++)
			{
				ASSERT(i <= p_AnimObject->d_MatrixCount);
				ts_Bone* pBone = &pBody->pBones[ i ];
				memcpy( pMatrices, &pBone->Matrix, sizeof( PS2Matrix4x4) );
				pMatrices = (PS2Matrix4x4 *) (((unsigned int) pMatrices) + sizeMatrix);
			}

			Profiler__Stop(PROFILE_COPY_BONES);
		}
	}

		// AA1 method of wireframing the models...
//#ifndef CONSUMER_BUILD
	if(RenderMgr__WireframeOn( ))
	{
		// set test1 to only include the alpha stuff...
		GSState__Set(SCE_GS_TEST_1, SCE_GS_SET_TEST_1(
			1, 	// ate, alpha test enable, 1 = on
			0x7, 	// atst, alpha test method, 111 = notequal
			0x80, // aref, alpha reference value for alpha test, 0x80 = opaque
			0x0, 	// afail, what to do when alpha test fails, 0 = KEEP (transparent wireframe) 10=ZB_ONLY (opaque wireframe)
			0, 	// date, destination alpha test, 0 = off, 1 = on
			1, 	// datm, destination alpha test mode, 1 bit
			1, 	// zte, depth test, 1 = on (TRC Requirement)
			0x3));	// ztst, depth test method, 0 = never, 1 = always, 2 = gequal, 3 = greater
			// force one pass AA
		renderFlags |= GS_ONE_PASS_AA;
		// turn off standard alpha blending
		renderFlags &= ~GS_ALPHABLEND;
		// draw the lines dark (no alpha blending on the lines)
		GSState__Set(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1(2, 2, 0, 0, 0));
	}
//#endif //CONSUMER_BUILD

	GSState__Set( SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1( 0, 2, 2, 2, 0x80));

#ifdef PS2_LABEL_MODELS
	// set the models name into the GS Label register
	GSState__Set(SCE_GS_LABEL, SCE_GS_SET_LABEL((u32)&pPatchSOM->Name[0], 0xFFFFFFFF));
#endif

	GSState__Send( );

	Profiler__Start(PROFILE_BUILD_MASTER_LIST);
	PS2Renderer::DrawDMAObject( *p_DDMAObject, * (PS2Matrix4x4 *) pmLocalToCamera, renderFlags);
	for ( int somIndex = 1 ; somIndex < numSOMs ; somIndex++ )
	{
		AnimDDMAObject* master = static_cast<AnimDDMAObject*>( p_DDMAObject );
		AnimDDMAObject* dmaObject = (AnimDDMAObject*) pDisplayListRecord[ somIndex ].pDisplayList;
		dmaObject->SetBufferIndex( master->GetBufferIndex() );
		dmaObject->b_MatrixBufferLightsSet = true;	// don't redo lighting, it's already in the base
		PS2Renderer::DrawDMAObject( *dmaObject, *(PS2Matrix4x4 *) pmLocalToCamera, renderFlags );
	}
	Profiler__Stop(PROFILE_BUILD_MASTER_LIST);

#ifdef PS2_LABEL_MODELS
	// clear the GS Label register
	GSState__Set(SCE_GS_LABEL, SCE_GS_SET_LABEL(NULL, 0xFFFFFFFF));
#endif

//#ifndef CONSUMER_BUILD
	if(RenderMgr__WireframeOn( ))
	{
		// restore gs settings
		GSState__Set( SCE_GS_TEST_1, GSState.TEST_1);
		GSState__Set( SCE_GS_ALPHA_1, GSState.ALPHA_1);
	}
//#endif //CONSUMER_BUILD

	// these stats come from the som for PS2 dma format stuff...
	SceneMgr::GetStats().IncVertices( pPatchSOM->u32TriStripVertexColorCount );
	SceneMgr::GetStats().IncPolys( pPatchSOM->u32TriStripVertexColorCount - (2 * pPatchSOM->u32TriStripCount) );
	SceneMgr::GetStats().IncTriStripPolys( pPatchSOM->u32TriStripVertexColorCount - (2 * pPatchSOM->u32TriStripCount) );
	if(pBody)
		SceneMgr::GetStats().IncBonesRendered( pBody->usBones );
}

//============================================================================
