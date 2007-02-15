//============================================================================
//=
//= TextureCache.cpp - a system for grouping similar texture arrays
//=                    and giving them a single ID, which can be used to
//=                    sort the instances to give better texture coherency.
//=
//============================================================================

#include "Game/GamePCH.h"

#if defined(PS2) && !defined(CONSUMER_BUILD)
//#define RAM_CLOBBER_DEBUG
#include "Layers/Breakpoint/HardwareBreak.h"
#endif

//============================================================================

TextureCache::TextureCache()
{
	d_lockedIndex = -1;
	d_top = 0;
	d_base = 0;
	d_hits = 0;
	d_misses = 0;
}

//============================================================================

TextureCache::~TextureCache()
{
	Reset();
}

//============================================================================

void TextureCache::Reset()
{
	#if defined(RAM_CLOBBER_DEBUG)
	HardwareBreak::Disable();
	#endif

	d_base += d_cache.Count();
	Unlock();
	for ( int i = d_cache.Count() - 1 ; i >= 0 ; i-- )
	{
		delete d_cache[ i ].textureArray;
		d_cache.Delete( i );
	}
	d_lockedIndex = -1;
	d_cache.Reset();
}

//============================================================================

bool TextureCache::IsSubset( TextureArray* a, TextureArray* b )
{
	int i;

	// don't let animated arrays be subsets of non animated arrays,
	// or visa versa.
	bool aAnimated = a->ContainsAnimatedTexture();
	bool bAnimated = b->ContainsAnimatedTexture();
	if ( aAnimated != bAnimated )
		return false;

	for ( i = 0 ; i < a->TextureCount() ; i++ )
	{
		Texture* imgA = a->GetTexture( i );
		int j;
		for ( j = 0 ; j < b->TextureCount() ; j++ )
			if ( b->GetTexture( j ) == imgA )
				break;
		if ( j >= b->TextureCount() )
			return false;
	}
	return true;
}

//============================================================================

void TextureCache::Add( TextureArray* arr )
{
	int i;

	// find if it can fit in any we already have
	for ( i = d_top ; i < d_cache.Count() ; i++ )
		if ( IsSubset( arr, d_cache[ i ].textureArray ) )
			return;

	CacheEntry ent;
	ent.textureArray = arr->Clone();

	#if defined(RAM_CLOBBER_DEBUG)
	HardwareBreak::Disable();
	#endif

	// remove any that fit inside the new entry
	for ( i = d_cache.Count() - 1 ; i >= d_top ; i-- )
	{
		if ( IsSubset( d_cache[ i ].textureArray, ent.textureArray ) )
		{
			delete d_cache[ i ].textureArray;
			d_cache.Delete( i );
		}
	}

	d_cache.Add( ent );

	#if defined(RAM_CLOBBER_DEBUG)
	HardwareBreak::Enable(true, false, (u32)d_cache.GetPtr(), (u32)d_cache.Count() * sizeof(CacheEntry));
	for (int k = 0; k < d_cache.Count(); k++)
		if (!d_cache[k].textureArray)
			*(volatile int*)1 = 2;
	#endif
}

//============================================================================

void TextureCache::Begin()
{
	d_top = d_cache.Count();
}

//============================================================================

void TextureCache::End()
{
	// could optimize and do a final merge here... but not needed right now.
}

//============================================================================

int TextureCache::GetID( TextureArray* arr )
{
	for ( int i = d_top ; i < d_cache.Count() ; i++ )
		if ( IsSubset( arr, d_cache[ i ].textureArray ) )
			return d_base + i;
	return -1;
}

//============================================================================

void TextureCache::Dump()
{
}

//============================================================================

//static int TextureArrayIntersects( TextureArray* a, TextureArray* b )
//{
//	int common = 0;
//
//	for ( int i = 0 ; i < a->TextureCount() ; i++ )
//		for ( int j = 0 ; j < b->TextureCount() ; j++ )
//			if ( a->GetTexture( i ) == b->GetTexture( j ) )
//				common++;
//
//	return common;
//}
//
//struct TextureCluster
//{
//	TextureArray* textureArray;
////	DynamicArray<int> originalSets;
//};
//
//void MergeClusterInto( TextureCluster& src, TextureCluster& dest )
//{
//	// the textures
//	int i;
//	for ( i = 0 ; i < src.textureArray->TextureCount() ; i++ )
//	{
//		Texture* texture = src.textureArray->GetTexture( i );
//		int j;
//		for ( j = 0 ; j < dest.textureArray->TextureCount() ; j++ )
//			if ( texture == dest.textureArray->GetTexture( j ) )
//				break;
//		if ( j >= dest.textureArray->TextureCount() )
//			dest.textureArray->AddTexture( texture );
//	}
//
//	// the set of source arrays
////	for ( i = 0 ; i < src.originalSets.Count() ; i++ )
////		dest.originalSets.Add( src.originalSets[ i ] );
//}
//
//void TextureCache::Optimize()
//{
//	int i;
//	int j;
//	DynamicArray<TextureCluster> clusters;
//
//	// build a cluster for each original cache entry
//	for ( i = 0 ; i < d_cache.Count() ; i++ )
//	{
//		TextureCluster cluster;
//		cluster.textureArray = new TextureArray;
//		for ( j = 0 ; j < d_cache[ i ].textureArray->TextureCount() ; j++ )
//			if ( d_cache[ i ].textureArray->GetTexture( j ) != TextureMgr::GetWhiteTexture() )
//				cluster.textureArray->AddTexture( d_cache[ i ].textureArray->GetTexture( j ) );
////		cluster.originalSets.Add( i );
//		clusters.Add( cluster );
//	}
//
//	// try to fold them all together
//	i = 0;
//	while ( i < clusters.Count() )
//	{
//		j = clusters.Count() - 1;
//		while ( j > i )
//		{
//			if ( TextureArrayIntersects( clusters[ i ].textureArray, clusters[ j ].textureArray ) )
//			{
//				MergeClusterInto( clusters[ j ], clusters[ i ] );
//				delete clusters[ j ].textureArray;
//				clusters.Delete( j );
//				i = 0;
//				j = clusters.Count() - 1;
//			}
//			else
//			{
//				j--;
//			}
//		}
//		i++;
//	}
//
//	// show how we did
//	for ( i = 0 ; i < clusters.Count() ; i++ )
//	{
//		int size = 0;
//		for ( j = 0 ; j < clusters[ i ].textureArray->TextureCount() ; j++ )
//			size += clusters[ i ].textureArray->GetTexture( j )->GetImage()->GetSize();
//
//		dbgPrint( "cluster %d/%d (SIZE=%d):\n", i + 1, clusters.Count(), size );
////		dbgPrint( "  sets:" );
////		for ( j = 0 ; j < clusters[ i ].originalSets.Count() ; j++ )
////			dbgPrint( " %d", clusters[ i ].originalSets[ j ] );
////		dbgPrint( "\n" );
//		dbgPrint( "  textures:" );
//		for ( j = 0 ; j < clusters[ i ].textureArray->TextureCount() ; j++ )
//			dbgPrint( " %08X", clusters[ i ].textureArray->GetTexture( j ) );
//		dbgPrint( "\n" );
//	}
//
//	// kill all the original cache entries
//	Reset();
//
//	// fill them back in using the clusters
//	for ( i = 0 ; i < clusters.Count() ; i++ )
//	{
//		CacheEntry ent;
//		ent.ID = i;
//		ent.textureArray = clusters[ i ].textureArray;
//		ent.textureArray->AddTexture( TextureMgr::GetWhiteTexture() );
//		d_cache.Add( ent );
//	}
//}

//============================================================================

inline void TextureCache::IncHits()
{
	d_hits++;
}

//============================================================================

inline void TextureCache::IncMisses()
{
	d_misses++;
}

//============================================================================

void TextureCache::Lock( TextureArray* arr )
{
	if ( arr )
	{
		if ( ( d_lockedIndex < 0 ) ||
			  ( d_lockedIndex != arr->CacheID() - d_base ) ||
			  ( arr->ContainsAnimatedTexture() ) ||
			  ( d_cache[ d_lockedIndex ].textureArray->ContainsAnimatedTexture() ) )
		{
			IncMisses();

			Unlock();

			// auto add to cache if we don't have an id
			if ( arr->CacheID() < d_base )
			{
				arr->SetCacheID( d_base + d_cache.Count() );
				CacheEntry ent;
				ent.textureArray = arr->Clone();

				#if defined(RAM_CLOBBER_DEBUG)
				HardwareBreak::Disable();
				#endif

				d_cache.Add( ent );

				#if defined(RAM_CLOBBER_DEBUG)
				HardwareBreak::Enable(true, false, (u32)d_cache.GetPtr(), (u32)d_cache.Count() * sizeof(CacheEntry));
				for (int k = 0; k < d_cache.Count(); k++)
					if (!d_cache[k].textureArray)
						*(volatile int*)1 = 2;
				#endif
			}

			d_lockedIndex = arr->CacheID() - d_base;
			d_cache[ d_lockedIndex ].textureArray->Lock();
		}
		else
		{
			IncHits();
		}
	}
}

//============================================================================

void TextureCache::Unlock()
{
	if ( d_lockedIndex >= 0 )
	{
		TextureState__Request( 0, TS_BIND, 0 );
		d_cache[ d_lockedIndex ].textureArray->Unlock();
		d_lockedIndex = -1;
	} 
}

//============================================================================
