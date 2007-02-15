//============================================================================
//=
//= TextureCache.h - a system for grouping similar texture arrays
//=                  and giving them a single ID, which can be used to
//=                  sort the instances to give better texture coherency.
//=
//============================================================================

#ifndef _TEXTURE_CACHE_H
#define _TEXTURE_CACHE_H

//============================================================================

class TextureCache
{
public:
	 TextureCache();
	~TextureCache();

	void	Reset();
	bool	IsSubset( TextureArray* a, TextureArray* b );
	void	Add( TextureArray* arr );
	int	GetID( TextureArray* arr );
	void	Dump();
	void	Lock( TextureArray* arr ) NAMED_SECTION("text.rend1");
	void	Unlock();
	void	Optimize();
	void	Begin();
	void	End();

	// stats
	int	Hits()			{ return d_hits; }
	int	Misses()			{ return d_misses; }
	void	ClearStats()	{ d_hits = d_misses = 0; }

private:
	struct CacheEntry
	{
		TextureArray*	textureArray;
	};
	DynamicArray<CacheEntry>	d_cache;
	int								d_lockedIndex;
	int								d_top;
	int								d_base;

	// stats
	inline void IncHits();
	inline void IncMisses();
	int								d_hits;
	int								d_misses;
};

//============================================================================

#endif //_TEXTURE_CACHE_H
