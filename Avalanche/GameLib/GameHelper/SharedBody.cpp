//============================================================================
//=
//= SharedBody.cpp - a dynamic render object that allows several different
//=    pieces to share a single skeleton, saving excess matrices and
//=    optimizing performance.  Useful for create-a-player.
//=
//============================================================================

#include "GameHelper/GameHelperPCH.h"
#include "GameHelper/SharedBody.h"
#include "Game/Managers/TextureMgr/TextureMgr.h"
#include "Display/Common/PatchDisp.h"
#include "collide/Capsule.h"

#ifdef PS2
#include "Display/PS2/Renderer/Code/AnimDDMAObject.h"
#endif //PS2

//============================================================================

#define MAX_ACTIVE_SWAPS 128

struct SwapRestore
{
	ts_TextureLayer*	layer;
	TextureDatabase*	database;
	int					index;
};

static SwapRestore	s_activeSwaps[ MAX_ACTIVE_SWAPS ];
static int				s_numActiveSwaps = 0;
#ifndef CDROM
static int				s_maxActiveSwaps = 0;
#endif

//============================================================================

SharedBody::SharedBody()
{
	BoundingBox.Min.Set( -0.5, 0.0f, -0.5 );
	BoundingBox.Max.Set(  0.5, 2.0f,  0.5 );
	BoundingSphere.Center.Set( 0.0f, 1.0f, 0.0f );
	BoundingSphere.Radius = 0.0f;	// unused
	d_disableRender = false;

	d_textureDatabase = new TextureDatabase( "sharedbody" );

	// managed textures
	d_managedTextures = new TextureDatabase( "managed" );
}

//============================================================================

SharedBody::~SharedBody()
{
	int i;

	for ( i = d_soms.Count() - 1 ; i >= 0 ; i-- )
		RemoveSOM( i );

	ResetTextureSwaps();
	RefreshTextureSwaps();

	delete d_textureDatabase;
	delete d_managedTextures;
	d_textureDatabase = NULL;
	d_managedTextures = NULL;
}

//============================================================================

void SharedBody::AddSOM( ts_PatchSOM* som, t_Handle fileHandle, const char* name )
{
	SomRecord r;
	r.name                         = new char[ strlen( name ) + 1 ];
	strcpy( r.name, name );
	r.som                          = som;
	r.fileHandle						 = fileHandle;
	r.visible                      = true;
	r.textureContexts              = NULL;
	r.displayList.pDisplayList     = NULL;
	r.displayList.u16PolyCount     = 0;
	r.displayList.u16QuadWordCount = 0;

	// lock us as a user
	dblLock( fileHandle );

	#if defined( PS2 )
//	if ( d_soms.Count() > 0 )
//		AnimDDMAObject::SetSharedMatrices( (AnimDDMAObject*) d_soms[ 0 ].displayList.pDisplayList );
//	r.displayList.pDisplayList = (char*) PS2Renderer::createDDMAObject( som->DisplayListRecord.pDisplayList, som->DisplayListRecord.u16QuadWordCount, 0 );
//	ASSERT_PTR( r.displayList.pDisplayList );
//	AnimDDMAObject::SetSharedMatrices( NULL );
	#endif //PS2
	#if defined( DIRECTX )
	r.displayList.pDisplayList = (char*) PatchSOM__GetIndexedMesh( som->DisplayListRecord.pDisplayList, 0 );
	ASSERT_PTR( r.displayList.pDisplayList );
	#endif //DIRECTX

	// lookup the texture context
	ts_Geometry geom;
	geom.pSOM = som;
	geom.pLOD = NULL;
	t_Handle dblFile = dblGetHandleFromGeometry( &geom );
	ASSERT( dblFile != INVALID_HANDLE );
	r.textureContexts = (ts_RecordList*) dblGetChunkData( DB_TEXTURE_CONTEXT, DEFAULT_CHUNK_KEY_STRING, dblFile );
	ASSERT_PTR( r.textureContexts );

	d_soms.Add( r );

	RefreshVisible();
}

//============================================================================

void SharedBody::RemoveSOM( int index )
{
	ASSERT( index >= 0 && index < d_soms.Count() );
	if( index < 0 || index >= d_soms.Count() ) return;

	// free display list
	#if defined( PS2 )
	if ( d_soms[ index ].displayList.pDisplayList )
	{
		PS2Renderer::destroyDDMAObject( (DDMAObject*) d_soms[ index ].displayList.pDisplayList );
		d_soms[ index ].displayList.pDisplayList = NULL;
	}
	#endif //PS2
	#if defined( DIRECTX )
	PatchSOM__ReleaseIndexedMesh( d_soms[ index ].displayList.pDisplayList );
	#endif //DIRECTX

	// release our lock
	dblUnlock( d_soms[ index ].fileHandle );

	// free name
	if ( d_soms[ index ].name )
	{
		delete d_soms[ index ].name;
		d_soms[ index ].name = NULL;
	}

	d_soms.Delete( index );

	RefreshVisible();
}

//============================================================================

void SharedBody::Render( const CInstance* pInstance, const Graphics4x4* pmLocalToWorld, float fZCamDist )
{
	int i;

	if ( d_disableRender || d_visibleSoms.Count() == 0 )
		return;

	// update counter
	d_lastRenderFrame = SceneMgr::GetFrameNumber();

	#if defined( PS2 )
	// rebuild display lists if necessary
	RebuildDisplayLists();
	#endif

	// set up the texture matrix
	TextureState__RequestUnit( 0 );
	MatrixStack__Mode( MS_TEXTURE );
	MatrixStack__Identity();

	// lock the textures
	TextureMgr::Cache().Unlock();
	for ( i = 0 ; i < d_visibleSoms.Count() ; i++ )
		if ( d_visibleSoms[ i ]->pTextureArray )
			d_visibleSoms[ i ]->pTextureArray->Lock();
	LockSwappedTextures();

	// alpha
	#ifdef GCN
	RenderState__Request( RS_OBJECT_CONSTANTALPHA, RenderState__ftoi( pInstance->Alpha() ) );
	#else
	RenderState__Request( RS_TEXTURE_CONSTANTALPHA, RenderState__ftoi( pInstance->Alpha() ) );
	#endif

	// render
	ts_PatchSOM__RenderFlags renderFlags;
	renderFlags.bFogAlpha = 0;
	renderFlags.bIsLODModel = 0;
	PatchSOM__Render( d_visibleSoms.GetPtr(), d_visibleSoms.Count(), (Graphics4x4*) pmLocalToWorld, d_visibleDisplayLists.GetPtr(), NULL /*vert colors*/, renderFlags, pInstance->Body() );

	// unlock the textures
	TextureState__Request( 0, TS_BIND, 0 );
	for ( i = 0 ; i < d_visibleSoms.Count() ; i++ )
		if ( d_visibleSoms[ i ]->pTextureArray )
			d_visibleSoms[ i ]->pTextureArray->Unlock();
	UnlockSwappedTextures();
}

//============================================================================

void SharedBody::RenderShadow( const CInstance* pInstance, const Graphics4x4* pmLocalToCamera, float fZCamDist )
{
	if ( d_disableRender || d_visibleSoms.Count() == 0 )
		return;

	#if defined( PS2 )
	RebuildDisplayLists();
	PatchSOM__RenderPS2ShadowModel( d_visibleSoms.GetPtr(), d_visibleSoms.Count(), (Graphics4x4*) pmLocalToCamera, d_visibleDisplayLists.GetPtr(), pInstance->Body() );
	#else
	ts_PatchSOM__RenderFlags renderFlags;
	renderFlags.bFogAlpha = 0;
	renderFlags.bIsLODModel = 0;
	PatchSOM__Render( d_visibleSoms.GetPtr(), d_visibleSoms.Count(), (Graphics4x4*) pmLocalToCamera, d_visibleDisplayLists.GetPtr(), NULL /*vert colors*/, renderFlags, pInstance->Body() );
	#endif
}

//============================================================================

void SharedBody::RenderOffScreen( const CInstance* pInstance )
{
	#if defined( PS2 )
	if ( SceneMgr::GetFrameNumber() > d_lastRenderFrame + 3 )
		PurgeDisplayLists();
	#endif
}

//============================================================================

void SharedBody::LockSwappedTextures()
{
	ASSERT( s_numActiveSwaps == 0 );

	// commit the texture swaps
	for ( int i = 0 ; i < d_textureSwaps.Count() ; i++ )
	{
		// lock it
		d_textureSwaps[ i ].texture->Lock();

		// force layers to use it
		for ( int swapLayer = 0 ; swapLayer < MAX_SWAP_LAYERS ; swapLayer++ )
		{
			ts_TextureLayer* layer = d_textureSwaps[ i ].layer[ swapLayer ];
			if ( layer )
			{
				// save the original data
				ASSERT( s_numActiveSwaps < MAX_ACTIVE_SWAPS );
				s_activeSwaps[ s_numActiveSwaps ].layer    = layer;
				s_activeSwaps[ s_numActiveSwaps ].database = layer->textureDatabase;
				s_activeSwaps[ s_numActiveSwaps ].index    = layer->s16TextureIndex;
				s_numActiveSwaps++;
#ifndef CDROM
				if (s_numActiveSwaps > s_maxActiveSwaps)
					s_maxActiveSwaps = s_numActiveSwaps;
#endif

				// force new database and index
				layer->textureDatabase = d_textureDatabase;
				layer->s16TextureIndex = d_textureSwaps[ i ].textureIndex;
			}
		}
	}
}

//============================================================================

void SharedBody::UnlockSwappedTextures()
{
	int i;

	// unlock textures
	for ( i = 0 ; i < d_textureSwaps.Count() ; i++ )
		d_textureSwaps[ i ].texture->Unlock();

	// restore layers, walk in reverse order just in case a layer is hit twice
	// the last restored is the first one saved.
	for ( i = s_numActiveSwaps - 1 ; i >= 0 ; i-- )
	{
		ts_TextureLayer* layer = s_activeSwaps[ i ].layer;
		layer->textureDatabase = s_activeSwaps[ i ].database;
		layer->s16TextureIndex = s_activeSwaps[ i ].index;
	}
	
	// reset
	s_numActiveSwaps = 0;
}

//============================================================================

void SharedBody::AddManagedTexture( Texture* tex )
{
	d_managedTextures->AppendTexture( tex );
}

//============================================================================

void SharedBody::ResetManagedTextures()
{
	d_managedTextures->RemoveAllTextures();
}

//============================================================================

int SharedBody::FindSOMIndexByName( const char* name, int startIndex )
{
	int i;
	for ( i = startIndex ; i < d_soms.Count() ; i++ )
	{
		const char *somName = d_soms[ i ].name;
		if ( !stricmp( somName, name ) )
			return i;
	}
	return -1;
}

//============================================================================

void SharedBody::Hide( const char* name, bool hide )
{
	bool changed = false;

	// set hidden flag for all objects of a given name
	for ( int i = 0 ; i < d_soms.Count() ; i++ )
	{
		if ( !stricmp( d_soms[ i ].name, name ) && d_soms[ i ].visible != !hide )
		{
         d_soms[ i ].visible = !hide;
			changed = true;
		}
	}

	if ( changed )
	{
		RefreshVisible();
	}
}

//============================================================================

bool SharedBody::Hidden( const char* name )
{
	// find hidden flag for the object of a given name
	for ( int i = 0 ; i < d_soms.Count() ; i++ )
		if ( !stricmp( d_soms[ i ].name, name ) )
			return !d_soms[ i ].visible;

	return true;
}

//============================================================================

void SharedBody::Hide( int index, bool hide )
{
	if ( index >= 0 && index < d_soms.Count() && d_soms[ index ].visible != !hide )
	{
		d_soms[ index ].visible = !hide;
		RefreshVisible();
	}
}

//============================================================================
bool SharedBody:: Hidden( int index )
{
	if (index < 0 || index >= d_soms.Count())
		return true;
	else
		return !d_soms[ index ].visible;
}

//============================================================================

void SharedBody::ResetTextureSwaps()
{
	for ( int i = d_textureSwaps.Count() - 1 ; i >= 0 ; i-- )
	{
		dblUnlock( d_textureSwaps[ i ].fileHandle );
		delete [] d_textureSwaps[ i ].name;
		d_textureSwaps.Delete( i );
	}

	ResetManagedTextures();
}

//============================================================================

void SharedBody::TextureSwap( const char* name, Texture* texture, t_Handle fileHandle )
{
	// find if we're already swapping this name
	int i;
	for ( i = 0 ; i < d_textureSwaps.Count() ; i++ )
		if ( !stricmp( d_textureSwaps[ i ].name, name ) )
			break;

	if ( i < d_textureSwaps.Count() )
	{
		dblUnlock( d_textureSwaps[ i ].fileHandle );
		d_textureSwaps[ i ].texture = texture;
		d_textureSwaps[ i ].fileHandle = fileHandle;
		dblLock( fileHandle );
	}
	else
	{
		// create a new entry
		SwapRecord r;
		r.name         = new char[ strlen(name) + 1 ];
		strcpy( r.name, name );
		strlwr( r.name );
		r.texture      = texture;
		r.fileHandle   = fileHandle;
		d_textureSwaps.Add( r );
		dblLock( fileHandle );
	}
}

//============================================================================

void SharedBody::RemoveTextureSwap( const char* name )
{
	// find the swap index
	int i;
	for ( i = d_textureSwaps.Count() - 1 ; i >= 0 ; i-- )
		if ( !stricmp( d_textureSwaps[ i ].name, name ) )
			break;

	if ( i >= 0 )
	{
		dblUnlock( d_textureSwaps[ i ].fileHandle );
		delete [] d_textureSwaps[ i ].name;
		d_textureSwaps.Delete( i );
	}
}

//============================================================================

void SharedBody::RefreshVisible()
{
	d_visibleSoms.Clear();
	d_visibleDisplayLists.Clear();

	if ( d_soms.Count() )
	{
		// the master is always index 0.
		d_visibleSoms.Add( d_soms[ 0 ].som );
		d_visibleDisplayLists.Add( d_soms[ 0 ].displayList );

		// add the rest of the visible pieces, putting all alpha pieces last
		for ( int i = 1 ; i < d_soms.Count() ; i++ )
		{
			if ( d_soms[ i ].visible )
			{
				bool hasAlpha = ( d_soms[ i ].som->dh.u16Flags & SOMM_ALPHA_SORT ) != 0;

				// hack, push "wear" (headwear, torsowear, ...) items to end for alpha sorting,
				// because the alpha bit isn't coming though the filter chain for some reason.
				if ( strstr( d_soms[ i ].name, "wear" ) || strstr( d_soms[ i ].name, "facialhair" ) )
					hasAlpha = true;

				int  addPosition = ( hasAlpha ) ? -1 : 1;
				d_visibleSoms.Add( d_soms[ i ].som, addPosition );
				d_visibleDisplayLists.Add( d_soms[ i ].displayList, addPosition );
			}
		}
	}
}

//============================================================================

void SharedBody::RefreshTextureSwaps()
{
	int i;

	// clean the database without deleting the textures.  we're going to make new indices.
	while ( d_textureDatabase->TextureCount() )
		d_textureDatabase->RemoveTexture( d_textureDatabase->GetTexture( 0 ) );

	// fill in the texture context for each of the swaps
	for ( i = 0 ; i < d_textureSwaps.Count() ; i++ )
	{
		int swapLayer;

		for ( swapLayer = 0 ; swapLayer < MAX_SWAP_LAYERS ; swapLayer++ )
			d_textureSwaps[ i ].layer[ swapLayer ] = NULL;

		swapLayer = 0;

		for ( int somIndex = 0 ; somIndex < d_soms.Count() ; somIndex++ )
		{
//			if ( d_soms[ somIndex ].visible )
			{
				ts_RecordList* contextList = d_soms[ somIndex ].textureContexts;
				for ( int contextIndex = 0 ; contextIndex < contextList->usRecordCount ; contextIndex++ )
				{
					ts_TextureContext* context = (ts_TextureContext*) GetRecordAddress( contextList, contextIndex );
					for ( int layerIndex = 0 ; layerIndex < context->TextureCount ; layerIndex++ )
					{
						ts_TextureLayer* layer = &context->TextureLayer[ layerIndex ];
						if ( !strcmp( layer->TextureName, d_textureSwaps[ i ].name ) )
						{
							if ( swapLayer < MAX_SWAP_LAYERS )
							{
								d_textureSwaps[ i ].layer[ swapLayer++ ] = layer;
								//dbgPrint( "bound '%s' to '%s' (som=%d context=%d layer=%d)\n", layer->TextureName, d_textureSwaps[ i ].texture->GetTextureName(), somIndex, contextIndex, layerIndex );
							}
							else
								ASSERTF( false, ( "texture '%s' can't be swapped in more than %d times", d_textureSwaps[ i ].name, MAX_SWAP_LAYERS ) );
						}
					}
				}
			}
		}

		if ( swapLayer > 0 )
		{
			d_textureSwaps[ i ].textureIndex = d_textureDatabase->AppendTexture( d_textureSwaps[ i ].texture );
		}
	}
}

//============================================================================

void SharedBody::ShowTextureNames()
{
	for ( int somIndex = 0 ; somIndex < d_soms.Count() ; somIndex++ )
	{
		if ( d_soms[ somIndex ].visible )
		{
			ts_RecordList* contextList = d_soms[ somIndex ].textureContexts;
			for ( int contextIndex = 0 ; contextIndex < contextList->usRecordCount ; contextIndex++ )
			{
				ts_TextureContext* context = (ts_TextureContext*) GetRecordAddress( contextList, contextIndex );
				for ( int layerIndex = 0 ; layerIndex < context->TextureCount ; layerIndex++ )
				{
					ts_TextureLayer* layer = &context->TextureLayer[ layerIndex ];
					dbgPrint( "som=%d '%s' context=%d layer=%d '%s'\n", somIndex, d_soms[ somIndex ].name,
								contextIndex, layerIndex, layer->TextureName );
				}
			}
			dbgPrint( "\n" );
		}
	}
}

//============================================================================

void SharedBody::PurgeDisplayLists()
{
#if defined( PS2 )
	// free display lists, we're not on screen
	bool changed = false;
	for ( int i = 0 ; i < d_soms.Count() ; i++ )
	{
		AnimDDMAObject* dmaObject = (AnimDDMAObject*) d_soms[ i ].displayList.pDisplayList;
		if ( dmaObject )
		{
			PS2Renderer::destroyDDMAObject( dmaObject );
			d_soms[ i ].displayList.pDisplayList = NULL;
			changed = true;
		}
	}
	if ( changed )
	{
		RefreshVisible();
	}
#endif
}

//============================================================================

void SharedBody::RebuildDisplayLists()
{
#if defined( PS2 )
	bool changed = false;
	for ( int i = 0 ; i < d_soms.Count() ; i++ )
	{
		if ( !d_soms[ i ].displayList.pDisplayList && d_soms[ i ].visible )
		{
			AnimDDMAObject::SetSharedMatrices( i == 0 ? NULL : (AnimDDMAObject*) d_soms[ 0 ].displayList.pDisplayList );
			d_soms[ i ].displayList.pDisplayList = (char*) PS2Renderer::createDDMAObject( d_soms[ i ].som->DisplayListRecord.pDisplayList,
				                                                                           d_soms[ i ].som->DisplayListRecord.u16QuadWordCount, 0 );
			AnimDDMAObject::SetSharedMatrices( NULL );
			changed = true;
		}
	}
	if ( changed )
	{
		RefreshVisible();
	}
#endif
}

//============================================================================

void SharedBody::SetDisableRender( int flag )
{
	d_disableRender = ( flag != 0 );
}

//============================================================================

void SharedBody::UpdateBoundingVolume(CInstance* pInstance)
{
	CActor *actor = pInstance->Owner();
	CCollisionObject *obj = actor->GetCollisionObject();
	bool betterVolume = false;
	if( obj )
	{
		CCollisionObject::ECollisionObjectType type = actor->GetCollisionObject()->GetObjectType();

		if( type == CCollisionObject::CAPSULE )
		{
			Capsule *capsule = (Capsule *)obj;
			pInstance->BoundingSphere().Center = capsule->Center();
			pInstance->BoundingSphere().Radius = capsule->SphericalRadius();
			betterVolume = true;
		}
		else if( type == CCollisionObject::OBB_OBJECT )
		{
			OBB *obb = (OBB *)obj;
			pInstance->BoundingSphere().Center = *obb->Center();
			pInstance->BoundingSphere().Radius = obb->Radius();
			betterVolume = true;
		}
	}
	if( !betterVolume )
	{
		pInstance->BoundingSphere().Center = actor->GetBodyInWorld();
	}

	pInstance->BoundingBox().Min = pInstance->BoundingSphere().Center - pInstance->BoundingSphere().Radius;
	pInstance->BoundingBox().Max = pInstance->BoundingSphere().Center + pInstance->BoundingSphere().Radius;
}
