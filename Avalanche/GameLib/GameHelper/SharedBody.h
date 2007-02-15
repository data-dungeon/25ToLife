//============================================================================
//=
//= SharedBody.h - a dynamic render object that allows several different
//=    pieces to share a single skeleton, saving excess matrices and
//=    optimizing performance.  Useful for create-a-player.
//=
//============================================================================

#ifndef _SHAREDBODY_H
#define _SHAREDBODY_H

#include "Game/Database/DynamicObject.h"
#include "container/Darray.h"
#include "Game/Managers/TextureMgr/TextureMgr.h"
#include "Game/Managers/TextureMgr/Image.h"

//============================================================================

class SharedBody : public GenericDynamicObject
{
public:
	SharedBody();
	virtual ~SharedBody();
	virtual void Render( const CInstance* pInstance, const Graphics4x4* pmLocalToCamera, float fZCamDist );
	virtual void RenderShadow( const CInstance* pInstance, const Graphics4x4* pmLocalToCamera, float fZCamDist );
	virtual void RenderOffScreen( const CInstance* pInstance );

	void	AddSOM( ts_PatchSOM* som, t_Handle fileHandle, const char* name );
	void	RemoveSOM( const int index );
	int	FindSOMIndexByName( const char* name, int startIndex = 0 );

	void	Hide( const char* name, bool hide );
	bool	Hidden( const char* name );

	void	Hide( int index, bool hide );
	bool	Hidden( int index );

	ts_PatchSOM* SOM( int index ) { ASSERT( index >= 0 && index < d_soms.Count() ); return d_soms[ index ].som; }
	int          NumSOMs()        { return d_soms.Count();  }

	void	TextureSwap( const char* textureName, Texture* texture, t_Handle fileHandle );
	void	RemoveTextureSwap( const char* textureName );
	void	ResetTextureSwaps();
	void	RefreshTextureSwaps();
	void	PurgeDisplayLists();
	void	RebuildDisplayLists();

	void	AddManagedTexture( Texture* tex );
	void	ResetManagedTextures();

	void	SetDisableRender( int flag );
	bool	DisableRender() { return d_disableRender; };

	virtual void UpdateBoundingVolume(CInstance* pInstance);

private:
	void	RefreshVisible();
	void	LockSwappedTextures();
	void	UnlockSwappedTextures();
	void	ShowTextureNames();

private:
	struct SomRecord
	{
		char*							name;
		ts_PatchSOM*				som;
		t_Handle						fileHandle;
		ts_DisplayListRecord		displayList;
		ts_RecordList*				textureContexts;
		char							visible;
	};
	#ifdef PS2
		#define MAX_SWAP_LAYERS 4
	#else
		#define MAX_SWAP_LAYERS 32
	#endif
	struct SwapRecord
	{
		char*							name;
		Texture*						texture;
		t_Handle						fileHandle;
		ts_TextureLayer*			layer[ MAX_SWAP_LAYERS ];
		int							textureIndex;
	};
	DynamicArray<SomRecord>					d_soms;
	DynamicArray<ts_PatchSOM*>				d_visibleSoms;
	DynamicArray<ts_DisplayListRecord>  d_visibleDisplayLists;
	DynamicArray<SwapRecord>				d_textureSwaps;
	TextureDatabase*							d_textureDatabase;
	u32											d_lastRenderFrame;
	bool											d_disableRender;

public:
	TextureDatabase*							d_managedTextures;
};

//============================================================================

#endif //_SHAREDBODY_H
