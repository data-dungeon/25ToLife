//============================================================================
//=
//= Instance.cpp
//=
//============================================================================

#include "Game/GamePCH.h"
#include "Display/Common/TextureAnimator.h"
#include "Display/Common/TextureAnimatorArray.h"
#include "Game/Managers/SceneMgr.h"

#define FADE_OUT_SPAN						5.0f
#define FRAMES_TO_DELETE_DISPLAY_LIST	3

//============================================================================

// GLOBAL access to current instance being rendered
CInstance* pRenderInst = NULL;

//============================================================================

void CInstance::Destroy()
{
	RemoveBones();
	RemoveDisplayList();

	if ( d_dynamicObject )
	{
		delete d_dynamicObject;
		d_dynamicObject = NULL;
	}

	if ( d_textureAnimatorArray )
	{
		delete d_textureAnimatorArray;
		d_textureAnimatorArray = NULL;
	}
}

//============================================================================

void CInstance::InitFromGeometry( ts_Geometry& geometry )
{
	ASSERT( !d_SOM );
	ASSERT( !d_dynamicObject );

	ASSERT( geometry.pSOM );
	d_SOM = geometry.pSOM;

	if ( d_SOM->dh.u16Flags & SOMM_ALPHA_SORT )
		SetAlphaSort( true );

	SetName( d_SOM->Name );

	MakeBones();

	d_boundingSphere = d_SOM->BoundingSphere;
	d_boundingBox    = d_SOM->BoundingBox;

	#ifdef PS2
	// we can build a display list for this instance given its SOM DMA object
	ASSERT( d_displayList.u16QuadWordCount == 0 );
	if ( d_SOM->DisplayListRecord.u16QuadWordCount )
	{
		if ( d_SOM->pTextureArray )
			TextureMgr::SetLastTextureContextList( d_SOM->pTextureDatabase->pTextureContextList );
		else
			TextureMgr::SetLastTextureContextList( NULL );

		if ( d_SOM->dh.u16Flags & SOMM_HAS_BONES )
		{
			d_displayList.pDisplayList = NULL;
			d_flags |= INSTANCE_DYNAMIC_ALLOC_DL;
		}
		else
		{
			DDMAObject* dmaObject = PS2Renderer::createDDMAObject( d_SOM->DisplayListRecord.pDisplayList,
																					 d_SOM->DisplayListRecord.u16QuadWordCount,
																					 d_colorSet );
			d_displayList.pDisplayList = (char*) dmaObject;
		}
	}
	#endif //PS2

	#ifdef DIRECTX
	if( d_SOM->DisplayListRecord.u16QuadWordCount )
	{
		ASSERT( d_colorSet < d_SOM->DisplayListRecord.u16QuadWordCount );
		d_displayList.pDisplayList     = (char *) PatchSOM__GetIndexedMesh( d_SOM->DisplayListRecord.pDisplayList, d_colorSet );
		d_displayList.u16PolyCount = d_colorSet;
	}
	#endif

	// animated textures
	if ( !d_textureAnimatorArray && d_SOM->pTextureArray )
		d_textureAnimatorArray = new TextureAnimatorArray( d_SOM->pTextureArray );

	// light influences
	if ( !d_curLightInfluences )
		d_curLightInfluences = new CLightInfluences();
	if ( d_SOM->dh.u16Flags & SOMM_TRI_STRIP_NORMALS )
	{
		d_lightingBits |= INSTANCE_LIGHTING_ENABLED | INSTANCE_LIGHTING_DIRECTIONAL | INSTANCE_LIGHTING_AMBIENT;
		d_attenuationScale = 1.0f;
	}
}

//============================================================================

// init an instance from a dynamic object
void CInstance::InitFromDynamicObject( DynamicObject* dynamicObject )
{
	ASSERT( !d_dynamicObject );
	d_dynamicObject = dynamicObject;
	d_dynamicObject->UpdateName( this );
	MakeBones();
	dynamicObject->UpdateBoundingVolumeAccurate( this );
}

//============================================================================

void CInstance::SetVisualPropertiesFromInstance( CInstance* inst )
{
	ASSERT( inst );
	if ( !inst ) return;

	// copy render flags
	SetRenderBucket( inst->RenderBucket() );
	SetAlphaSort( inst->AlphaSort() );
	SetReceiveShadows( inst->ReceiveShadows() );
	SetZWriteDisabled( inst->ZWriteDisabled() );
	SetAdditiveBlend( inst->AdditiveBlend() );
	SetBillboard( inst->Billboard() );
	SetVertexColor32Bit( inst->VertexColor32Bit() );
	SetFadeOutDistance( inst->FadeOutDistance() );
	SetFogDisabled( inst->FogDisabled() );
	SetSortBias( inst->SortBias() );
	SetVertexColorPtr( inst->VertexColorPtr() );
	SetColorSet( inst->ColorSet() );
	Hide( inst->Hidden() );

	// lighting info
	SetDynamicLightFlags( inst->DynamicLightFlags() );
	SetLightAttenuationScale( inst->LightAttenuationScale() );

	// if our model doesn't have normals disable dynamic lights
	if ( !d_dynamicObject && d_SOM && ( d_SOM->dh.u16Flags & SOMM_TRI_STRIP_NORMALS ) == 0 )
	{
		d_lightingBits = INSTANCE_LIGHTING_NONE;
	}

	// texture animator
	if ( d_textureAnimatorArray )
	{
		delete d_textureAnimatorArray;
		d_textureAnimatorArray = NULL;
	}
	if ( inst->GetTextureAnimatorArray() )
	{
		d_textureAnimatorArray = inst->GetTextureAnimatorArray()->Clone();
	}
}

//============================================================================

void CInstance::RemoveBones()
{
	if ( d_body )
	{
		if ( d_body->pBoneGroupList )
			delete d_body->pBoneGroupList;
		if ( d_body->pBones )
			delete d_body->pBones;
		delete d_body;
		d_body = NULL;
	}
}

//============================================================================
// Update the bounding sphere and bounding box based on the instance's
// current orientation and position

void CInstance::UpdateBoundingVolumes()
{
	if ( d_dynamicObject )
	{
		d_dynamicObject->UpdateBoundingVolume( this );
	}
	else if ( d_SOM )
	{
		// Xform the sphere center into world coords for the instance to use. NOTE: this is not accurate for
		// animated models-- for example, a human figure that falls over. the lying-down positions of the 
		// bones is acheived through animation data, but the 90 degree rotation from upright to prone is
		// not reflected in the instance transform matrix, so the center of the bounding sphere is still 
		// in the same spot as if the figure were still standing
		d_boundingSphere.Center = d_SOM->BoundingSphere.Center * d_localToWorld;

		// we assume that instances have their bounding radius set correctly. the issue is that scaled instances
		// can't just use the model's bounding sphere radius-- the scaling will increase or decrease it. so it is
		// the responsibility of the instance to maintain it's bounding sphere radius accurately. there is another
		// version of this routine (UpdateBoundingVolumesAccurate) that calculates the bounding volume by
		// transforming the corner's of the model's bounding box and recalculating in world space
		ASSERT((d_boundingSphere.Radius > 0.0f) && (d_boundingSphere.Radius < 10000.0f));	// sanity check-- if this hits radius was never initialized correctly

		// Figure out the bounding box from the transformed sphere
		d_boundingBox.Min = d_boundingSphere.Center - d_boundingSphere.Radius;
		d_boundingBox.Max = d_boundingSphere.Center + d_boundingSphere.Radius;
	}
}

//============================================================================
// Update the bounding sphere and bounding box based on the instance's
// current orientation and position
// Allows the input of scaling

void CInstance::UpdateBoundingVolumes( float scaleX, float scaleY, float scaleZ )
{
	// Figure out the max scale
	float scale = Math::Max(scaleX, Math::Max(scaleY, scaleZ));

	if ( d_dynamicObject )
	{
		d_dynamicObject->UpdateBoundingVolume(this, scale);
	}
	else if ( d_SOM )
	{
		// Xform the sphere center into world coords for the instance to use
		d_boundingSphere.Center = d_SOM->BoundingSphere.Center * d_localToWorld;
		d_boundingSphere.Radius = d_SOM->BoundingSphere.Radius * scale;

		// Figure out the bounding box from the transformed sphere
		d_boundingBox.Min = d_boundingSphere.Center - d_boundingSphere.Radius;
		d_boundingBox.Max = d_boundingSphere.Center + d_boundingSphere.Radius;
	}
}

//============================================================================
// Update the bounding sphere and bounding box based on the instance's
// current orientation and position. transform the model's bounding box
// into world space and calculate from that. this is especially useful
// for initializing a dynamic instance's bounding data since it will
// accurately scale the bounding sphere radius

void CInstance::UpdateBoundingVolumesAccurate()
{
	if ( d_dynamicObject )
	{
		d_dynamicObject->UpdateBoundingVolumeAccurate( this );
	}
	else
	{
		// transform 8 corners of model's bounding box into world space
		BoundingBox__Transform( &d_SOM->BoundingBox, &d_boundingBox, &d_localToWorld);

		// calculate bounding sphere that encloses this box
		d_boundingSphere.Center = BoundingBox__GetCenter( &d_boundingBox);

#ifdef _DEBUG
#if 0 //djr: these were hitting as a result of Food's water, so I've commented out...
		// this should match the transformed world position of the model's bounding sphere, since that
		// is how the instance's bounding sphere center is updated in the routines above. typically,
		// the model's bounding sphere center is calculated in just the same way (as the center of
		// the bounding box), so this should be valid

		Vector3 vCenter = d_SOM->BoundingSphere.Center * d_localToWorld;
		ASSERT(Math::Equal(vCenter.x, d_boundingSphere.Center.x, 0.0001f));	// please tell ned if this hits
		ASSERT(Math::Equal(vCenter.y, d_boundingSphere.Center.y, 0.0001f));	// please tell ned if this hits
		ASSERT(Math::Equal(vCenter.z, d_boundingSphere.Center.z, 0.0001f));	// please tell ned if this hits
#endif
#endif //_DEBUG

		// radius of instance bounding sphere is distance from center to one corner of bounding box
		d_boundingSphere.Radius = (d_boundingSphere.Center - d_boundingBox.Min).Length();
	}
}

//============================================================================

ts_BoundingBox* CInstance::ModelBoundingBox()
{
	if ( d_dynamicObject )
		return &d_dynamicObject->GetBoundingBox();
	else if ( d_SOM )
		return &d_SOM->BoundingBox;
	else
		return NULL;
}

//============================================================================

void CInstance::RemoveDisplayList()
{
	// remove light influences
	if ( d_curLightInfluences )
	{
		delete d_curLightInfluences;
		d_curLightInfluences = NULL;
	}

	if ( d_SOM && d_displayList.pDisplayList && !d_dynamicObject )
	{
		#if defined(DIRECTX)
		PatchSOM__ReleaseIndexedMesh( d_displayList.pDisplayList );
		#elif defined(PS2)
		PS2Renderer::destroyDDMAObject( (DDMAObject*)d_displayList.pDisplayList );
		#endif //PS2

		memset( &d_displayList, 0, sizeof( d_displayList ) );
	}
}

//============================================================================

void CInstance::SetupBillboardMatrix()
{
#ifdef _DEBUG
	/* instance local-to-world matrix can't have any y-rotation in order for this code to work. if you hit this */
	/* assert, the most likely candidate is that the instance is owned by an environmental actor, and something */
	/* is telling the actor to rotate in y */
	
	Vector3 vForward;
	vForward = d_localToWorld.GetRow2();
//	ASSERTS(Math::Zero(vForward.x(), 0.001f), "Billboarded instance has y-rotation, billboarding may not work");
#endif //_DEBUG
	
	/* transform instance's world position into camera space */
	
	Vector3 vWorldPosition;
	Vector3 vCameraPosition;
	vWorldPosition = d_localToWorld.GetTranslation();
	// TJC - call fast version of operator* because this code happens A LOT! (well, it did in downhill)
	vCameraPosition.FastMultiply(vWorldPosition, g4WorldToCamera);
	
	/* ok, in camera space, an instance that is directly in front of the camera is on the negative z-axis. we */
	/* can calculate the angle from the camera to the instance, and use that as an adjustment to overall camera */
	/* y-rotation */
	
	float fYAngle = Math::ArcTan2(-vCameraPosition.x(), -vCameraPosition.z());
	
	/* rotate instance's local-to-world matrix by y-rotation angle. this preserves original rotation and scale */
	/* info from instance's placement in world (for example, it allows us to have leaning billboard trees). */
	/* the rule is that the billboard instance has to be placed in the world such that its y-rotation is 0.0f, */
	/* that is, it looks down the positive z-axis */
	
	/* this could be optimized more if necessary...just y-rotate the upper 3 x 3 of m_LocalToWorld and */
	/* preserve the translation...still have to copy result to another matrix and preserve m_LocaltoWorld... */
	
	g4LocalToCamera.SetYRotation(SceneMgr::GetCameraYRotation() + fYAngle);				// calculate y-rotation matrix
	g4LocalToCamera.FastMultiply(d_localToWorld, g4LocalToCamera);	// rotate orientation part of local-to-world
	g4LocalToCamera.PokeTranslation(vWorldPosition);							// reset translation-- should rotate around model origin
}

//============================================================================
// for ps2 memory report, we want to be able to analyze vertex color use. on the ps2,
// the vertex colors are baked into the dma display list. to make them more
// available for memory report, copy the display list pointer into the vertex
// color pointer. on many models, vertex colors are baked into the model vertex
// x/y/z, so they're free, data-wise. don't consider them to be vertex colors.
// DATA_STATIC_UC means uncompressed vertex colors, that is, not baked into
// model vertex x/y/z data

#ifdef PS2
void CInstance::FixForMemoryReport( ts_PatchSOM* som )
{
	DDMAObject* dmaObject = (DDMAObject*) d_displayList.pDisplayList;
	if ( !dmaObject )
		dmaObject = (DDMAObject*) som->DisplayListRecord.pDisplayList;
	if ( dmaObject && ( dmaObject->d_Flags & OBJ_FLAG_VERTEX_COLORS ) && ( dmaObject->getClassID() == DATA_STATIC_UC ) )
		d_vertexColors = (char*) dmaObject;
}
#endif //PS2

//============================================================================

#ifdef PS2
void CInstance::DynamicallyAllocateDL( ts_PatchSOM* som )
{
	memSetUseBit(MEM_DMA_DATA);
	int colorSet = d_colorSet;
	if ( colorSet >= PS2Renderer::getPatchSetCount( som->DisplayListRecord.pDisplayList ) )
		colorSet = 0;
	DDMAObject* dmaObject = PS2Renderer::createDDMAObject( som->DisplayListRecord.pDisplayList,
																			 som->DisplayListRecord.u16QuadWordCount,
																			 colorSet );
	ASSERT( dmaObject );
	memClearUseBit(MEM_DMA_DATA);
			
	// stash it in the instance record
	d_displayList.pDisplayList = (char*) dmaObject;
	FixForMemoryReport( som );

	d_lastRenderFrame = 0;
}
#endif

//============================================================================

void CInstance::RenderOffScreen()
{
	if ( d_dynamicObject )
	{
		d_dynamicObject->RenderOffScreen( this );
	}
	else
	{
		#ifdef PS2
		if ( ( d_flags & INSTANCE_DYNAMIC_ALLOC_DL ) &&
			( d_displayList.pDisplayList ) &&
			( SceneMgr::GetFrameNumber() > d_lastRenderFrame + FRAMES_TO_DELETE_DISPLAY_LIST ) )
		{
			DDMAObject* dmaObject = (DDMAObject*) d_displayList.pDisplayList;
			PS2Renderer::destroyDDMAObject( dmaObject );
			d_displayList.pDisplayList = NULL;
		}
		#endif //PS2
	}
}

//============================================================================

void CInstance::RenderDynamic(float fZCamDist, Graphics4x4 *pmLocalToWorld)
{
	// yuch-- this screws up rendering shadows, (resets texture matrix? enables vertex colors? something...) so for now
	// don't render shadows on dynamic models
	if ( RenderMgr__RenderingProjectedTextures() )
		return;
	
	/* instances marked as INSTANCE_NO_ZWRITE turn off z-buffer writes. have to lock render state since */
	/* StripBuffer render resets it to default after render, and tyler's water does multiple strip buffer */
	/* renders (one per layer) */
	RenderState__Request( RS_ZWRITE, !ZWriteDisabled() );
	RenderState__Lock( RS_ZWRITE );

	#if defined(GCN) || defined(DIRECTX)
	RenderState__Request( RS_OBJECT_CONSTANTALPHA, RenderState__ftoi( d_alpha ) );
	#else
	RenderState__Request( RS_TEXTURE_CONSTANTALPHA, RenderState__ftoi( d_alpha ) );
	#endif

	// let the dynamic geom render itself
	d_dynamicObject->Render( this, pmLocalToWorld, fZCamDist );

	RenderState__Unlock( RS_ZWRITE );
}

//============================================================================

void CInstance::RenderNormal( float fZCamDist )
{
	RenderState__Request( RS_FOG, DisplayState.bFog );
	RenderState__Request( RS_FAR_CLIPPING, true );
	RenderState__Request( RS_ZWRITE, true );
	RenderState__Request( RS_ZBIAS, RenderState__ftoi( 0.0f ) );
	RenderState__Request( RS_ZDIST, RenderState__ftoi( -fZCamDist ) );
	#ifdef PS2
	RenderState__Request( RS_FOG_RENDER_PATH, RS_FALSE );
	RenderState__Request( RS_VU_CLIPPING, RS_CLIPPING_NONE );
	#endif //PS2
	#if defined(WIN32) || defined(GCN)	// not supported on ps2?
	RenderState__Request( RS_FRONTFACE, RS_FRONTFACE_CCW );
	#endif //WIN32
	#if defined(GCN) || defined(DIRECTX)
	RenderState__Request( RS_OBJECT_CONSTANTALPHA, RenderState__ftoi( 1.0f ) );
	#else
	RenderState__Request( RS_TEXTURE_CONSTANTALPHA, RenderState__ftoi( 1.0f ) );
	#endif

	ASSERT_PTR( d_SOM );

	if ( !RenderMgr__RenderingProjectedTextures() )
	{
		TextureMgr::Cache().Lock( d_SOM->pTextureArray );
		TextureState__RequestUnit( 0 );
		MatrixStack__Mode( MS_TEXTURE );
		MatrixStack__Identity();
	}

	#ifdef PS2
	if ( !d_displayList.pDisplayList && ( d_flags & INSTANCE_DYNAMIC_ALLOC_DL ) )
	{
		DynamicallyAllocateDL( d_SOM );
	}
	#endif //PS2

#if defined(DIRECTX)
		if( d_flags & INSTANCE_NO_RECEIVE_SHADOWS )
			ShadowMgr__ClearAllInstanceBits( );
		else
			ShadowMgr__SetInstanceBits( this );
#endif

	static ts_PatchSOM__RenderFlags renderFlags = { 0, 0, 0 };
	PatchSOM__Render( &d_SOM, 1, &d_localToWorld, &d_displayList, d_vertexColors, renderFlags, d_body );

	pRenderInst = NULL;
}

//============================================================================

void CInstance::Render( float fZCamDist )
{
	ts_PatchSOM__RenderFlags ucFlags = { 0, 0, 0 };

	ASSERT( !Hidden() );

	// give global access to instance being rendered
	pRenderInst = this;

	// mark as rendered this frame
	d_lastRenderFrame = SceneMgr::GetFrameNumber();

	// quick render for normal instances
	if ( ( d_dynamicObject == NULL ) &&
		  ( d_flags & ( INSTANCE_BILLBOARD | INSTANCE_NO_FOG | INSTANCE_NO_FAR_CLIP | INSTANCE_CLOCKWISE_VERTICES | INSTANCE_ADDITIVE_BLEND ) ) == 0 &&
		  ( d_viewFlags & ( INSTANCE_VIEW_CLIPPED ) ) == 0 &&
		  ( ( d_curLightInfluences == NULL ) || ( d_lightingBits & INSTANCE_LIGHTING_ENABLED ) == 0 ) &&
		  ( d_alpha >= 1.0f ) &&
		  ( Math::Equal( d_fadeOutDistance, INSTANCE_NO_FADE_DISTANCE ) ) &&
		  ( !d_textureAnimatorArray ) )
	{
		RenderNormal( fZCamDist );
		return;
	}

	// reverse vertex order for front/back facing?
	#if defined(WIN32) || defined(GCN)	// not supported on ps2?
	RenderState__Request( RS_FRONTFACE, ClockwiseVertices() ? RS_FRONTFACE_CW : RS_FRONTFACE_CCW );
	#endif //WIN32

	// instance's overall alpha will be calculated into here
	float fInstanceAlpha = d_alpha;
	if ( !Math::Equal( d_fadeOutDistance, INSTANCE_NO_FADE_DISTANCE ) )
	{
		float t = ( d_fadeOutDistance + FADE_OUT_SPAN - Math::Abs( fZCamDist ) ) * ( 1.0f / FADE_OUT_SPAN );
		if      ( t < 0.0f )  t = 0.0f;
		else if ( t > 1.0f )  t = 1.0f;
		fInstanceAlpha *= t;
	}

	// bail early if not visible
	if ( Math::Zero( fInstanceAlpha ) )
	{
		d_lastRenderFrame = 0;
		return;
	}

	// fogging
	RenderState__Request( RS_FOG, !FogDisabled() && DisplayState.bFog );

	// enable/disable far clipping for this instance. note-- on win32 opengl, you can't explicitly turn off the
	// far clip plane. so this is implemented by changing the view frustum so that the far clip plane is FAR
	// away. this affects the calculation of z-values, so it is a bad idea to have two intersecting models
	// where one is INSTANCE_NO_FAR_CLIP and the other is not
	RenderState__Request( RS_FAR_CLIPPING, !FarClipDisabled() );

	// handle billboarding. billboard instances are assumed to have a translation only-- upper left 3 x 3 matrix is
	// assumed to be identity. add a rotation around the y-axis such that the instance faces the camera. y-angle = 0
	// means instance is facing the unrotated camera (which looks down the negative z-axis)
	Graphics4x4* pg4LocalToWorld = &d_localToWorld;	// default is to just use instance's local-to-world matrix
	if ( Billboard() )
	{
		SetupBillboardMatrix();
		pg4LocalToWorld = &g4LocalToCamera; // and point to final local-to-world matrix
	}

#ifdef PS2
	// enable/disable vu clipping
	if ( d_viewFlags & INSTANCE_VIEW_CLIPPED )
		RenderState__Request( RS_VU_CLIPPING, RS_CLIPPING_CLIP );
	else
		RenderState__Request( RS_VU_CLIPPING, RS_CLIPPING_NONE );
#endif //PS2

	// lighting
	bool bRestoreLights = false;
	CActor *pActor = NULL;
	//if ( d_owner )
	//{
	//	if ( d_owner->LightColorSource() != CActor::LIGHT_COLOR_SOURCE_NONE )
	//	{
	//		d_owner->AdvanceLightColorMult();
	//		LightMgr__AdjustLights( d_owner->AmbientLight(), d_owner->LightColorMult() );
	//		bRestoreLights = true;
	//	}
	//}		
	
	if ( g_lightSet.lightSetIsLoaded() )
	{
		// TJC - Acording to Dwain, we really dont need to call
		// g_lightSet.disableLights(0) multiple times in a row if we
		// havent ever run the 'true' case of this next if statement.
		// So this static bool is used to keep track of when the next if
		// has been true, so that we know if we need to run the else
		// case..
		static bool lightSetDisabled = false;
		if ( d_curLightInfluences && ( d_lightingBits & INSTANCE_LIGHTING_ENABLED ) )
		{
			// TJC - remember we turned something on
			lightSetDisabled = false;
			RenderState__Request( RS_LIGHTING, RS_TRUE );

			bool distCheck = ( d_body && d_body->pBones );
			d_curLightInfluences->useLightInfluences( distCheck, d_attenuationScale, fZCamDist );
			if ( bRestoreLights )
				RenderState__RegisterHardwareAmbientLight( d_owner->AmbientLight() );
		}
		else if ( !lightSetDisabled )
		{
#if defined(DIRECTX)
			ts_fRGB Black = { 0, 0, 0 };
			RenderState__RegisterHardwareAmbientLight( Black );
#endif
			g_lightSet.disableLights( 0 );
			lightSetDisabled = true;
		}
	}

	// if this is a dynamic object bail now to it's render routine
	if ( d_dynamicObject )
	{
		RenderDynamic( fZCamDist, pg4LocalToWorld );
		return;
	}

	// z write?
	bool zWrite = ( !ZWriteDisabled() &&
						 !FarClipDisabled() &&
						 ( d_SOM->dh.u16Flags & SOMM_VISIBLE_INTERNALS ) == 0 );
	RenderState__Request( RS_ZWRITE, zWrite );

	// set alpha level
	#if defined(GCN) || defined(DIRECTX)
	RenderState__Request( RS_OBJECT_CONSTANTALPHA, RenderState__ftoi( fInstanceAlpha ) );
	#else
	RenderState__Request( RS_TEXTURE_CONSTANTALPHA, RenderState__ftoi( fInstanceAlpha ) );
	#endif

	// alloc our display list if we don't have it
	#ifdef PS2
	if ( !d_displayList.pDisplayList && ( d_flags & INSTANCE_DYNAMIC_ALLOC_DL ) )
	{
		DynamicallyAllocateDL( d_SOM );
	}
	#endif

	// verify model
	#ifndef CDROM
	ASSERT((d_SOM->dh.u8DataType == DATA_SOM) || (d_SOM->dh.u8DataType == DATA_PS2_DMA_SOM) || (d_SOM->dh.u8DataType == DATA_GCN_SOM) || (d_SOM->dh.u8DataType == DATA_GCN_SOM2));
	if ( d_SOM->dh.u16Flags & SOMM_HAS_BONES )
	{
		ASSERT( d_body && d_body->pBones );
		if ( !d_body || !d_body->pBones )
			return;
	}
	#endif //CDROM

	// textures (only bother if we aren't rendering projected textures)
	bool bUnlockBlendFunc = false;
	if ( RenderMgr__RenderingProjectedTextures() == false )
	{
		// before rendering this object, make sure that its animated textures are render-ready
		// !!! this must be after the call to TextureMgr__LockTextureSet
		if ( d_textureAnimatorArray )
		{
			d_textureAnimatorArray->Update( (u16)(g_timer.GetFrameSec() * 1000.0f) );
			d_textureAnimatorArray->AdjustTextures();
		}

		// lock textures
		TextureMgr::Cache().Lock( d_SOM->pTextureArray );

		// maybe need to set a texture matrix-- this currently affects the entire model, no way to break it out
		// texture-by-texture
		TextureState__RequestUnit( 0 );	//yuch-- this assumes only texture unit 0 needs texture transform?
		MatrixStack__Mode( MS_TEXTURE );

		// TJC - The new texture manager does not have a solution for
		// this problem yet.  So for now we do something similar to the
		// old method.  So here we are supporting only 1 animator.
		TextureAnimator* animator = NULL;
		if ( d_textureAnimatorArray && d_textureAnimatorArray->Count() > 0 )
			animator = (*d_textureAnimatorArray)[0];

		if ( animator && animator->HasTextureTransform() )
		{
			MatrixStack__Load( animator->GetTextureTransform() );

			#ifdef PS2
			// on ps2, instances that use texture matrix MUST go through the fog rendering path, though they
			// may not want fogging. this request just ends up setting the MICROCODE_FOG in the renderFlags
			// in PatchDisp.cpp-- if instance has also requested fog, no harm done
			RenderState__Request( RS_FOG_RENDER_PATH, RS_TRUE );
			#endif //PS2
		}
		else
		{
			MatrixStack__Identity();
			#ifdef PS2
			RenderState__Request( RS_FOG_RENDER_PATH, RS_FALSE );
			#endif //PS2
		}

		// maybe override and lock blend function
		if ( AdditiveBlend() )
		{
			RenderState__Request( RS_BLENDFUNC, RS_BLENDFUNC_ADDITIVE );
			RenderState__Lock( RS_BLENDFUNC );
			bUnlockBlendFunc = true;
		}
	}

#if defined(DIRECTX)
		if( d_flags & INSTANCE_NO_RECEIVE_SHADOWS )
			ShadowMgr__ClearAllInstanceBits( );
		else
			ShadowMgr__SetInstanceBits( this );
#endif

	// RENDER
	static unsigned int noDraw = 0;
	if( ((unsigned int) d_SOM) != noDraw )
		PatchSOM__Render( &d_SOM, 1, pg4LocalToWorld, &d_displayList, d_vertexColors, ucFlags, d_body );

	// restore render state
	// if using display lists, restore default render state. this is because display lists count
	// on render state being all default values when they are rendered, since they have no way
	// of knowing what the current state will be when it is their turn

	if ( bUnlockBlendFunc )	
		RenderState__Unlock( RS_BLENDFUNC );

	#ifndef PS2
	RenderState__Request( RS_ZWRITE, RS_TRUE );
	#endif
	RenderState__Request( RS_ZBIAS, RenderState__ftoi( 0.0f ) );
	RenderState__Request( RS_ZDIST, RenderState__ftoi( 1.0f ) );
	#if defined(GCN)  ||  defined(DIRECTX)
	RenderState__Request( RS_OBJECT_CONSTANTALPHA, RenderState__ftoi( 1.0f ) );
	#else
	RenderState__Request( RS_TEXTURE_CONSTANTALPHA, RenderState__ftoi( 1.0f ) );
	#endif

	if ( RenderMgr__RenderingProjectedTextures() == false )
	{
		if ( d_SOM->pTextureArray->ContainsAnimatedTexture() )
			TextureMgr::Cache().Unlock();
	}

	// restore lights
	if ( bRestoreLights ) g_lightSet.restoreLights();

	// done rendering, disable global handle to this instance
	pRenderInst = NULL;
}

//============================================================================

void CInstance::RenderShadow( Graphics4x4* localToCamera, float fZCamDist )
{
	if ( d_dynamicObject )
	{
		d_dynamicObject->RenderShadow( this, localToCamera, fZCamDist );
	}
	else
	{
		ASSERT_PTR( d_SOM );
		#ifdef PS2
		if ( !d_displayList.pDisplayList && ( d_flags & INSTANCE_DYNAMIC_ALLOC_DL ) )
			DynamicallyAllocateDL( d_SOM );
		PatchSOM__RenderPS2ShadowModel( &d_SOM, 1, localToCamera, &d_displayList, d_body );
		#else
		static ts_PatchSOM__RenderFlags renderFlags = { 0, 0, 0 };
		ASSERT( d_SOM->dh.u8DataType == DATA_SOM || d_SOM->dh.u8DataType == DATA_GCN_SOM || d_SOM->dh.u8DataType == DATA_GCN_SOM2 );
		PatchSOM__Render(&d_SOM, 1, localToCamera, &d_displayList, d_vertexColors, renderFlags, d_body );
		#endif
	}
}

//============================================================================
// if this is an instance of a boned model, create and init the instance bone data

void CInstance::MakeBones()
{
	if ( ( d_dynamicObject == NULL ) && ( d_SOM ) && ( d_SOM->dh.u16Flags & SOMM_HAS_BONES ) )
	{
		// create a body, fill it in from the geometry body
		d_body = new ts_Body;
		memcpy( d_body, d_SOM->pBody, sizeof( *d_body ) );

		// create bony array
		d_body->pBones = new ts_Bone[ d_body->usBones ];
		memset( d_body->pBones, 0, sizeof( ts_Bone ) * d_body->usBones );

		// fix up the bone array
		FixInstanceBones( d_body->pBones, d_body->pBones, NULL, d_body, d_body->pMBones, d_body->pMBones, NULL );

		// create the bone group list array
		d_body->pBoneGroupList = new ts_BoneGroupList[ d_body->usBoneGroups ];
		memset( d_body->pBoneGroupList, 0, sizeof( ts_BoneGroupList ) * d_body->usBoneGroups );
		FixBoneGroupLists( d_body->pBones, d_body->pBoneGroupList );
	}
}

//============================================================================

void CInstance::FixDiskLoad( ts_RecordList* instList, ts_RecordList* SOMList )
{
	// make sure we don't slip in here for dynamic objects
	ASSERT( !d_dynamicObject );

	// set model pointer from model index
	int modelIndex = (int) d_SOM;
	ASSERT( modelIndex >= 0 && modelIndex < SOMList->usRecordCount );
	d_SOM = (ts_PatchSOM*) GetRecordAddress( SOMList, modelIndex );
	ASSERT_PTR( d_SOM );

	// set vertex color ptr
	int vertexColorOffset = (int) d_vertexColors;
	if ( vertexColorOffset )
	{
		d_vertexColors = (void*) ( (u32)this + vertexColorOffset );
	}

	// build display list from model data.
	#ifdef DIRECTX
	if ( d_SOM->dh.u8DataType == DATA_SOM )
	{
		if ( d_SOM->DisplayListRecord.u16QuadWordCount )
		{
			ASSERT( d_colorSet < d_SOM->DisplayListRecord.u16QuadWordCount );
			d_displayList.pDisplayList = (char*) PatchSOM__GetIndexedMesh( d_SOM->DisplayListRecord.pDisplayList, d_colorSet );
			d_displayList.u16PolyCount = d_colorSet;
		}
		else
		{
			d_displayList.pDisplayList = 0;
		}
	}
	#endif //DIRECTX

	#ifdef PS2
	DDMAObject* dmaObject;
	ASSERT( d_displayList.u16QuadWordCount == 0 );
	if ( d_SOM->dh.u8DataType == DATA_PS2_DMA_SOM )
	{
		// u16QuadWordCount is used as an already-fixed-up flag. 0 is fixed up.
		// SOM's get their display list fixed at load time if they are
		// (1) not animated and
		// (2) they don't use multiple vertex color sets.
		if ( d_SOM->DisplayListRecord.u16QuadWordCount != 0 )
		{
			int colorSet = d_colorSet;
			if ( colorSet >= PS2Renderer::getPatchSetCount( d_SOM->DisplayListRecord.pDisplayList ) )
				colorSet = 0;
			dmaObject = PS2Renderer::createDDMAObject( d_SOM->DisplayListRecord.pDisplayList,
				                                        d_SOM->DisplayListRecord.u16QuadWordCount,
																	 colorSet );
			ASSERT_PTR( dmaObject );
			d_displayList.pDisplayList = (char*) dmaObject;
		}
		else
		{
			// already fixed up.  instance doesn't need a pointer, as the renderer will
			// default to the SOM display list ptr.
			d_displayList.pDisplayList = 0;
		}
		FixForMemoryReport( d_SOM );
	}
	#endif //PS2

	// create texture animator
	d_textureAnimatorArray = NULL;

	// look for any animated textures and create their animators
	for ( int i = 0 ; i < d_SOM->pTextureArray->TextureCount() ; i++ )
	{
		if ( d_SOM->pTextureArray->GetTexture( i )->GetFlags() & Texture::ANIMATED )
		{
			if ( !d_textureAnimatorArray )
				d_textureAnimatorArray = new TextureAnimatorArray( d_SOM->pTextureArray );
			TextureAnimator* anim = d_textureAnimatorArray->GetOrCreateByTextureIndex( i );
			anim->SetActive( true );
			anim->SetCurrentFrame( 0 );
			anim->SetFPS( 30.0f );
		}
	}

	// register reflecting surface (only up to one per instance).
	Texture* reflectData = (Texture*) DataMgr::FindOwner( d_SOM )->FindData( SOM_DATA_REFLECT_BIND );
	if ( reflectData )
		ReflectionMgr__RegisterReflection( this, reflectData );

	// if the instance is marked as billboard, check local-to-world matrix. billboard code (in Instance__Render)
	// makes some assumptions about initial orientation of model. this dates from the downhill biking project,
	// when we wanted trees that could lean but still billboard around the model's y-axis. so they did not have
	// y-rotation-only. the assumption was that the tree would be placed in the world so that y-rotation was 0.0--
	// that is, so it would look down the positive z-axis. this convention is kind of a pain, and unnecessary if
	// the billboard instance is straight up and down-- y-rotation only. so check here to see if we need to adjust
	// it to match the convention
	if ( Billboard() )
	{
		DirCos3x3 mOrient;
		Vector3 vPos;
		Vector3 vScale;
		float shearXY, shearXZ, shearYZ;
		float scalex,scaley,scalez;
		d_localToWorld.Deconstruct( mOrient, vPos, scalex, scaley, scalez, shearXY, shearXZ, shearYZ );
		vScale.Set( scalex, scaley, scalez );

		// if instance is straight up and down, eliminate any y-rotation and rebuild local-to-world so it
		// will work with billboard code
		Vector3 vMaybeUp;
		vMaybeUp = mOrient.GetRow1();
		if ( vMaybeUp.y() >= 0.99f )
		{
			// orientation is y-rotation-only. rebuild local-to-world matrix with no y-rotation
			d_localToWorld.SetScale( vScale.x(), vScale.y(), vScale.z() );
			d_localToWorld.PokeTranslation( vPos );
		}
	}

	// create our light influences array
	if ( DynamicallyLit() )
	{
		d_curLightInfluences = new CLightInfluences();
	}
}

//============================================================================

bool CInstance::LoadInstances( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32*pu32DataSize )
{
	// get pointer to instance list
	ts_RecordList* instList = (ts_RecordList*) pData;
	ResolveRecordList( instList );

	// model data (SOMs) are a previously loaded chunk, get a handle to them.
	ts_RecordList* SOMList;
	SOMList = (ts_RecordList*) dblGetChunkDataCurrentDBL( DB_SOMARRAY, dblGetChunkName( pChunkHdr ) );
	if ( !SOMList )
		SOMList = (ts_RecordList*) dblGetChunkDataCurrentDBL( DB_SOMARRAYBONE, dblGetChunkName( pChunkHdr ) );
	if ( !SOMList )
		SOMList = (ts_RecordList*) dblGetChunkDataCurrentDBL( DB_DMASOMARRAY, dblGetChunkName( pChunkHdr ) );
	ASSERT_PTR( SOMList );

	// fix each instance
	for ( int i = 0 ; i < instList->usRecordCount ; i++ )
	{
		CInstance* inst = (CInstance*) GetRecordAddress( instList, i );
		inst->FixDiskLoad( instList, SOMList );
	}

	// keep this chunk in memory
	pChunkHdr->u16Flags &= ~DBL_FLUSHABLE;
	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	return true;
}

//============================================================================

bool CInstance::LoadInstanceAttributes( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32*pu32DataSize )
{
	/* get instance data (previously loaded chunk in this database) for patching group references */

   ts_RecordList *pInstList = (ts_RecordList *) dblGetChunkDataCurrentDBL(DB_INSTANCE, dblGetChunkName(pChunkHdr));
	if (pInstList == NULL)
	{
		ASSERTS(false, "No instance data to apply instance attributes to");
		return(false);
	}

	/* data is a set of strings. each pair of strings is a key/value pair. the first key is always an instance index so */
	/* that we know what instance to update */

	CInstance *pInst = NULL;	// instance to modify
	TextureAnimator* animator = NULL; // animator to modify
	char *pString = (char *) pData;
	while (*pString != 0)	// stop when we find key == ""
	{
		/* known keys */

		enum
		{
			INSTANCE_INDEX = 0,
			TEXTURE_TRANSFORM_ENABLE,
			TEXTURE_TRANSFORM_START_FRAME,
			TEXTURE_TRANSFORM_FRAME_RATE,
			TEXTURE_TRANSFORM_U,
			TEXTURE_TRANSFORM_UVEL,
			TEXTURE_TRANSFORM_V,
			TEXTURE_TRANSFORM_VVEL,
			TEXTURE_TRANSFORM_ROT,
			TEXTURE_TRANSFORM_ROTVEL,
		};
		typedef struct
		{
			char	*pKeyName;
			int	nKey;
		} ts_KeyList;
		static ts_KeyList KeyList[] = {

			{"instance_index",				INSTANCE_INDEX},

			{"texture_transform_enable",	TEXTURE_TRANSFORM_ENABLE},
			{"texture_transform_start_frame", TEXTURE_TRANSFORM_START_FRAME },
			{"texture_transform_frame_rate", TEXTURE_TRANSFORM_FRAME_RATE },
			{"texture_transform_u",			TEXTURE_TRANSFORM_U},
			{"texture_transform_uvel",		TEXTURE_TRANSFORM_UVEL},
			{"texture_transform_v",			TEXTURE_TRANSFORM_V},
			{"texture_transform_vvel",		TEXTURE_TRANSFORM_VVEL},
			{"texture_transform_rot",		TEXTURE_TRANSFORM_ROT},
			{"texture_transform_rotvel",	TEXTURE_TRANSFORM_ROTVEL},

			/* must be last */

			{"", -1}
		};

		/* process key/value pairs. first pair should be an instance index, followed by attributes for that instance. */
		/* typically, these are grouped with a parent "enabler" key (enables data to be put into this chunk for this */
		/* instance, instances do not add data unless enabler is on), followed by some child data that is the actual */
		/* settings */

		int nIndex = 0;
		while ((KeyList[nIndex].nKey != -1) && (strcmp(KeyList[nIndex].pKeyName, pString) != 0))
			nIndex++;
		pString += strlen(pString) + 1;	// skip key

		/* first key should be the instance index-- check that here so we don't have to put an assert */
		/* in each case statement */

		if ((pInst == NULL) && (KeyList[nIndex].nKey != INSTANCE_INDEX))
		{
			ASSERTS(false, "Invalid instance attribute data");
			return(false);
		}

		/* handle key-- silently ignore unknown keys */

		switch(KeyList[nIndex].nKey)
		{
		case INSTANCE_INDEX:
			nIndex = atoi(pString);
			if ((nIndex < 0) || (nIndex >= (signed) pInstList->usRecordCount))
			{
				ASSERTS(false, "Instance attribute index out of range");
				return(false);
			}
			pInst = (CInstance *) GetRecordAddress(pInstList, nIndex);
			animator = NULL;
			break;

		case TEXTURE_TRANSFORM_ENABLE:
			if ( !pInst->GetTextureAnimatorArray() )
			{
				ASSERT( pInst->Model() );
				pInst->GetTextureAnimatorArray() = new TextureAnimatorArray( pInst->Model()->pTextureArray );
			}
			if ( pInst->GetTextureAnimatorArray()->Count() )
				animator = pInst->GetTextureAnimatorArray()->Get( 0 );
			if ( !animator )
				animator = pInst->GetTextureAnimatorArray()->GetOrCreateByTextureIndex( 0 );	// bogus texture index
			break;
		case TEXTURE_TRANSFORM_U:
			ASSERT(animator != NULL);
			animator->SetTextureTransformU((float) atof(pString));
			break;
		case TEXTURE_TRANSFORM_START_FRAME:
			ASSERT(animator != NULL);
			animator->SetCurrentFrame((u16) atoi(pString));
			break;
		case TEXTURE_TRANSFORM_FRAME_RATE:
			ASSERT(animator != NULL);
			animator->SetFPS((float) atof(pString));
			break;
		case TEXTURE_TRANSFORM_UVEL:
			ASSERT(animator != NULL);
			animator->SetTextureTransformUVel((float) atof(pString));
			break;
		case TEXTURE_TRANSFORM_V:
			ASSERT(animator != NULL);
			animator->SetTextureTransformV((float) atof(pString));
			break;
		case TEXTURE_TRANSFORM_VVEL:
			ASSERT(animator != NULL);
			animator->SetTextureTransformVVel((float) atof(pString));
			break;
		case TEXTURE_TRANSFORM_ROT:
			ASSERT(animator != NULL);
			animator->SetTextureTransformRotation((float) atof(pString));
			break;
		case TEXTURE_TRANSFORM_ROTVEL:
			ASSERT(animator != NULL);
			animator->SetTextureTransformRotationVel((float) atof(pString));
			break;
		}

		/* skip over key value*/

		pString += strlen(pString) + 1;
	}

	/* success */

	return(true);
}

//============================================================================

bool CInstance::RenderedLastFrame() const
{
	 return d_lastRenderFrame == SceneMgr::GetFrameNumber();
}

//============================================================================
