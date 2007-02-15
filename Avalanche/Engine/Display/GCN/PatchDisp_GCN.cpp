//============================================================================
//=
//= PatchDisp_GCN.cpp - GameCube specific functions for rendering
//=
//============================================================================

#include "Display/GCN/FrameData.h"

//============================================================================

typedef f32 Mtx3x3[3][3];

//============================================================================

extern Graphics4x4	mtxEnvBase;

//============================================================================

#if !defined( CDROM )
u16	u16CurrToken = 0;
#endif

//============================================================================

void PatchDisp__Cleanup( )
{

}

//============================================================================

void GCNPatchSOM2__RenderTriStripsShadows( ts_PatchSOM *pPatchSOM, Graphics4x4 *pmLocalToCamera);

//============================================================================

void GCNPatchSOM2__RenderTriStrips( ts_PatchSOM *pPatchSOM, Graphics4x4 *pmLocalToCamera, void *pVertexColors, ts_Body *pBody)
{

	ts_GCNTriStripHeader	*pTriStripHeader;

	u16						u16Flags = pPatchSOM->dh.u16Flags;

	u16						u16TextureCount;

	u32						channelFlags = 0;

	bool						bRenderingShadows = ShadowMgr__RenderingShadows( );

	if( pPatchSOM->dh.u8DataType != DATA_GCN_SOM2)
	{
		return;
	}

	if( bRenderingShadows)
	{
		if( u16Flags & SOMM_HAS_BONES)
		{
			return;
		}

		GCNPatchSOM2__RenderTriStripsShadows( pPatchSOM, pmLocalToCamera);
		return;
	}

	Profiler__Start( PROFILE_RENDER_IMMEDIATE);
#if 0
	if( stricmp( pPatchSOM->Name, "Outhouse_meshShape") == 0)
	{
		LOCKUP( );
	}
#endif

	/* lighting? */
	if ( pRenderInst->DynamicallyLit() && ( u16Flags & SOMM_TRI_STRIP_NORMALS ) && RenderMgr__LightingOn() )
	{
		channelFlags |= ShaderMgr::GCN_CHANNEL_HAS_LIGHTS;
	}
	
	if( pVertexColors != NULL)
	{
		channelFlags |= ShaderMgr::GCN_CHANNEL_HAS_VTXCOLOR;
	}

	GXColor	materialColor;

	materialColor.r = 255;
	materialColor.g = 255;
	materialColor.b = 255;
	materialColor.a = 255;
	
	g_ShaderMgr.SetupLightingChannel( channelFlags, materialColor);

	if( u16Flags & SOMM_HAS_BONES) //motion data?
	{

		Mtx		*pmtxBone2Camera;
		Mtx3x3	*pmtxBone2CameraInv;
		Mtx		*pmtxBone2EnvMap;
		
		ASSERT( pBody->usBones <= MAX_OBJECT_BONES);

		pmtxBone2Camera = (Mtx *)g_pFrameData->memAlloc( pBody->usBones * sizeof(Mtx));
		pmtxBone2CameraInv = (Mtx3x3 *)g_pFrameData->memAlloc( pBody->usBones * sizeof(Mtx3x3));

		if( u16Flags & SOMM_NEEDS_ENV_MAPPING)
		{
			pmtxBone2EnvMap = (Mtx *)g_pFrameData->memAlloc( pBody->usBones * sizeof(Mtx));
		}

		for( u16 iBone = 0; iBone < pBody->usBones; iBone++)
		{
			ts_Bone* pBone = &pBody->pBones[ i ];

			Graphics4x4		bone2Camera;

			Mtx				inv;

			bone2Camera.FastMultiply( pBone->Matrix, *pmLocalToCamera);	// calculate into matrix array set up for this purpose
			MatrixStack__Matrix4x4ToMtx( &bone2Camera, pmtxBone2Camera[iBone]);
			MTXInvXpose( pmtxBone2Camera[iBone], inv);

			for( int row = 0; row < 3; row++)
			{
				for( int col = 0; col < 3; col++)
				{
					pmtxBone2CameraInv[iBone][row][col] = inv[row][col];
				}
			}

			if( u16Flags & SOMM_NEEDS_ENV_MAPPING)
			{
				
				Matrix3x3		mtxTemp = bone2Camera;
				
				Graphics4x4		mtxEnv;

				mtxEnv = mtxTemp * mtxEnvBase;
				MatrixStack__Matrix4x4ToMtx( &mtxEnv, pmtxBone2EnvMap[iBone]);
			}

		}
		DCFlushRangeNoSync( pmtxBone2Camera, pBody->usBones * sizeof(Mtx));
		DCFlushRange( pmtxBone2CameraInv, pBody->usBones * sizeof(Mtx3x3));

		GXSetArray( GX_POS_MTX_ARRAY, pmtxBone2Camera, sizeof(Mtx));
		GXSetArray( GX_NRM_MTX_ARRAY, pmtxBone2CameraInv, sizeof(Mtx3x3));

		if( u16Flags & SOMM_NEEDS_ENV_MAPPING)
		{
			DCFlushRange( pmtxBone2EnvMap, pBody->usBones * sizeof(Mtx));
			GXSetArray( GX_TEX_MTX_ARRAY, pmtxBone2EnvMap, sizeof(Mtx));
		}
	}
	else
	{
		/* no bones, set model matrix for transforms */
		MatrixStack__Mode( MS_MODELVIEW );
		MatrixStack__Load( pmLocalToCamera);

		if( u16Flags & SOMM_NEEDS_ENV_MAPPING)
		{
			Matrix3x3		mtxTemp = *pmLocalToCamera;
			Mtx				*pmtxEnvMap;
			Graphics4x4		mtxEnv;
			
			mtxEnv = mtxTemp * mtxEnvBase;

			pmtxEnvMap = (Mtx *)g_pFrameData->memAlloc( sizeof(Mtx));

			MatrixStack__Matrix4x4ToMtx( &mtxEnv, pmtxEnvMap[0]);
			DCFlushRange( pmtxEnvMap, sizeof(Mtx));

			GXSetArray( GX_TEX_MTX_ARRAY, pmtxEnvMap, sizeof(Mtx));
		}
	}

	GXSetArray( GX_VA_POS, pPatchSOM->pVertexBuffer, sizeof(Vector3));
	GXSetArray( GX_VA_NRM, pPatchSOM->pNormalBuffer, 3);
	GXSetArray( GX_VA_TEX0, pPatchSOM->pUVBuffer, sizeof(Vector2));
	GXSetArray( GX_VA_TEX1, pPatchSOM->pUVBuffer, sizeof(Vector2));
	GXSetArray( GX_VA_TEX2, pPatchSOM->pUVBuffer, sizeof(Vector2));
	GXSetArray( GX_VA_TEX3, pPatchSOM->pUVBuffer, sizeof(Vector2));
	GXSetArray( GX_VA_TEX4, pPatchSOM->pUVBuffer, sizeof(Vector2));
	GXSetArray( GX_VA_TEX5, pPatchSOM->pUVBuffer, sizeof(Vector2));
	GXSetArray( GX_VA_TEX6, pPatchSOM->pUVBuffer, sizeof(Vector2));
	GXSetArray( GX_VA_TEX7, pPatchSOM->pUVBuffer, sizeof(Vector2));
	GXSetArray( GX_VA_CLR0, pVertexColors, ( pRenderInst->VertexColor32Bit() ) ? sizeof(ts_bRGBA) : sizeof(u16));

	GXFlush( );

	/* loop through strips in this model */
	pTriStripHeader = (ts_GCNTriStripHeader *)pPatchSOM->pTriStripBuffer;
	for( ; pTriStripHeader != NULL; pTriStripHeader = pTriStripHeader->pNext)
	{
		ASSERT( ( ( (u32)pTriStripHeader) & 0x1F) == 0);

		u8		u8Flags = pTriStripHeader->u8Flags;

		u16TextureCount = pTriStripHeader->pTextureContext->TextureCount;

		ts_TextureLayer *pLayer = pTriStripHeader->pTextureContext->TextureLayer;

		RenderState__Request( RS_CULL, u8Flags & TRI_STRIP_TWO_SIDED ? RS_CULL_NONE : RS_CULL_BACK);

		RenderState__Request( RS_BLENDFUNC, pLayer->u8Blend);	// always use the first layer's blend function

		/* draw-- loop through texture contexts */
		for( int iTex = 0, iTexMap = 0; iTex < u16TextureCount; iTex++, pLayer++)
		{
			TextureBind__SetWrap( pLayer->s16TextureBind, pLayer->u8WrapU, pLayer->u8WrapV);
			TextureBind__SetFilter( pLayer->s16TextureBind, pLayer->u8Filter);

			iTexMap = TextureBind__Load( pLayer->s16TextureBind, iTexMap, (GXTexCoordID)iTex);
		}

		/* set texture context and render state */
		g_ShaderMgr.UseShader( pTriStripHeader->pShader);

#if 0
#ifndef CDROM
		GXSetDrawSync( u16CurrToken);
		OSReport( "Rendering %s, TriStripHeader 0x%08x as token %d\n", pPatchSOM->Name, pTriStripHeader, u16CurrToken++);
		if( u16CurrToken >= 0x8000)
		{
			u16CurrToken = 0;
		}
#endif
#endif

		GXCallDisplayList( (void *)( pTriStripHeader + 1), pTriStripHeader->u32DisplayListSize);

		SceneMgr::GetStats().IncPolys( pTriStripHeader->u16TriangleCount );
		SceneMgr::GetStats().IncTripStripPolys( pTriStripHeader->u16TriangleCount );
	}

	/* stats */
	SceneMgr::GetStats().IncTriStrips( pPatchSOM->u32TriStripCount);
	if(pBody)
		SceneMgr::GetStats().IncBonesRendered( pBody->usBones );
	Profiler__Stop( PROFILE_RENDER_IMMEDIATE);
}

//============================================================================

void GCNPatchSOM2__RenderTriStripsShadows( ts_PatchSOM *pPatchSOM, Graphics4x4 *pmLocalToCamera)
{

	ts_GCNTriStripHeader	*pTriStripHeader;

	u16						u16Flags = pPatchSOM->dh.u16Flags;

	u16						u16TextureCount;

	u32						channelFlags = 0;

	Profiler__Start( PROFILE_RENDER_IMMEDIATE);

	MatrixStack__Mode( MS_MODELVIEW );
	MatrixStack__Load( pmLocalToCamera);

	GXSetArray( GX_VA_POS, pPatchSOM->pVertexBuffer, sizeof(Vector3));
	GXSetArray( GX_VA_TEX0, pPatchSOM->pUVBuffer, sizeof(Vector2));

	GXFlush( );

	/* loop through strips in this model */
	pTriStripHeader = (ts_GCNTriStripHeader *)pPatchSOM->pTriStripBuffer;
	for( ; pTriStripHeader != NULL; pTriStripHeader = pTriStripHeader->pNext)
	{
		ASSERT( ( ( (u32)pTriStripHeader) & 0x1F) == 0);

		u8		u8Flags = pTriStripHeader->u8Flags;

		if( !( u8Flags & TRI_STRIP_GCN_RECEIVE_SHADOWS))
		{
			continue;
		}

		u16TextureCount = pTriStripHeader->pTextureContext->TextureCount;

		ts_TextureLayer *pLayer = pTriStripHeader->pTextureContext->TextureLayer;

		RenderState__Request( RS_CULL, u8Flags & TRI_STRIP_TWO_SIDED ? RS_CULL_NONE : RS_CULL_BACK);

		//we need to reset the vtx desc for each strip
		//we always have a position and a color if object receives shadows
		GXClearVtxDesc( );
		GXSetVtxDesc( GX_VA_CLR0, GX_INDEX16);
		if( pPatchSOM->u32VertexCount < 256)
		{
			GXSetVtxDesc( GX_VA_POS, GX_INDEX8);
		}
		else
		{
			GXSetVtxDesc( GX_VA_POS, GX_INDEX16);
		}
		
		if( u8Flags & TRI_STRIP_HAS_NORMALS)
		{
			if( pPatchSOM->u32NormalCount < 256)
			{
				GXSetVtxDesc( GX_VA_NRM, GX_INDEX8);
			}
			else
			{
				GXSetVtxDesc( GX_VA_NRM, GX_INDEX16);
			}
		}
		
		for( int iTex = 0, iUV = 0; iTex < u16TextureCount; iTex++, pLayer++)
		{
			if( u8Flags & TRI_STRIP_ENVIRONMENT_MAP)
			{
				if( pLayer->u8Flags & TEXTURE_IS_ENVIRONMENT_MAP)
				{
					continue;
				}
			}
			if( pPatchSOM->u32UVCount < 256)
			{
				GXSetVtxDesc( (GXAttr)( GX_VA_TEX0 + iUV), GX_INDEX8);
			}
			else
			{
				GXSetVtxDesc( (GXAttr)( GX_VA_TEX0 + iUV), GX_INDEX16);
			}
		}

#if 0
#ifndef CDROM
		GXSetDrawSync( u16CurrToken);
		OSReport( "Rendering %s, TriStripHeader 0x%08x as token %d\n", pPatchSOM->Name, pTriStripHeader, u16CurrToken++);
		if( u16CurrToken >= 0x8000)
		{
			u16CurrToken = 0;
		}
#endif
#endif

		GXCallDisplayList( (void *)( pTriStripHeader + 1), pTriStripHeader->u32DisplayListSize);

		SceneMgr::GetStats().IncPolys( pTriStripHeader->u16TriangleCount );
		SceneMgr::GetStats().IncTripStripPolys( pTriStripHeader->u16TriangleCount );
	}

	/* stats */
	SceneMgr::GetStats().IncTriStrips( pPatchSOM->u32TriStripCount );
	Profiler__Stop( PROFILE_RENDER_IMMEDIATE);
}

//============================================================================
