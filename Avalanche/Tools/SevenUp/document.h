#ifndef _DOCUMENT_H_
#define _DOCUMENT_H_

class SevenupDoc;

#include <afxwin.h>
#include <hud/hudlib.h>
#include "formattedfile.h"
#include "project.h"
#include "projectdlg.h"

#define PROJECTFILE_MARKER					(0x5c8e3c1b)
#define PROJECTFILE_VERSION				(0x00000100)

#define PROJECTFILE_TEXTUREMARKER		(0x887c3b1a)
#define PROJECTFILE_TEXTUREVERSION		(0x00000100)

#define PROJECTFILE_SPRITEMARKER			(0x3c5e9b1c)
#define PROJECTFILE_SPRITEVERSION_V1	(0x00000100)
#define PROJECTFILE_SPRITECODE1			(0x12345678)
#define PROJECTFILE_SPRITECODE2			(0x13579bdf)

#define PROJECTFILE_SPRITEVERSION	PROJECTFILE_SPRITEVERSION_V1

#define PROJECTFILE_PROJECTMARKER		(0x77c3509b)
#define PROJECTFILE_PROJECTVERSION_V1	(0x00000100)
#define PROJECTFILE_PROJECTVERSION_V2	(0x00000101)
#define PROJECTFILE_PROJECTVERSION_V3	(0x00000102)
#define PROJECTFILE_PROJECTVERSION_V4	(0x00000103)

#define PROJECTFILE_THUMBNAILMARKER		(0x44cb95e1)
#define PROJECTFILE_THUMBNAILVERSION	(0x00000100)

#define PROJECTFILE_PROJECTVERSION (PROJECTFILE_PROJECTVERSION_V4)

typedef struct
{
	int m_Size;
	int m_ScreenWidth;
	int m_ScreenHeight;
	int m_SafeZoneX;
	int m_SafeZoneY;
	BOOL m_DrawBorder;
	BOOL m_DrawSafeZone;
}
PROJECTPARAMS;

class SevenupView;
class SevenupDoc : public CDocument
{
	DECLARE_DYNCREATE( SevenupDoc );

	public:
		SevenupDoc( void );
		virtual ~SevenupDoc();

		//////////////////////////////////////////////////////////////////////////
		
		void NewDocument( void );
		BOOL OpenDocument( LPCTSTR pPathName, Sprite* pSceneList );
		BOOL SaveDocument( LPCTSTR pFileName, Sprite* pSceneList );
		BOOL SaveTexture( LPCTSTR pTextureDir, t_Handle Handle );
		void Destroy( void );

		BOOL ExportGameFiles( LPCTSTR pFileName );
		HBITMAP CreateThumbnail( Sprite* pSceneList, int Width, int Height, BOOL SuperSample = FALSE );
		BOOL DrawThumbnail( HBITMAP hbm, Sprite* pSceneList );

		//////////////////////////////////////////////////////////////////////////

		Sprite* CreateObject( HUDOBJECTTYPE Type );
		void InitObject( Sprite* pSprite );
		void InitSprite( Sprite* pSprite );
		void InitText( Text* pText );
		void Insert( Sprite* pSprite, Sprite* pParent );
		
		void SelectSprite( Sprite* pSprite );
		Sprite* GetSelectedSprite( void );
		Sprite* DeleteSprite( Sprite *pSprite );
		Sprite* DuplicateSprite( Sprite *pSprite );
		Sprite* DuplicateSpriteFamily( Sprite *pSprite );
		
		//////////////////////////////////////////////////////////////////////////
		
		Sprite* GetSceneList( void );
		Sprite* GetSpriteAtPosition( float x, float y, Sprite *pNotThisSprite = 0 );

		//////////////////////////////////////////////////////////////////////////

		void AddSpriteToList( Sprite* pSprite );
		void ClearSpriteList( void );
		SpriteItem* GetSpritesAtPosition( float x, float y, Sprite* pParent );
		void AddSpritesAtPosition( Sprite* pSprite, float x, float y );

		//////////////////////////////////////////////////////////////////////////
		
		void SetTexture( t_Handle Handle, int TextureIndex );
		t_Handle GetTextureHandle( void );
		int GetTextureIndex( void );

		//////////////////////////////////////////////////////////////////////////
		
		void NewObjectInitialize( Sprite *pSprite );
		void NewSpriteInitialize( Sprite *pSprite );
		void NewTextInitialize( Text *pText );
		
		//////////////////////////////////////////////////////////////////////////

		BOOL AddToProject( Sprite* pSprite, Sprite* pParent = 0 );
		void SyncProjectViews( void );
		void SyncThumbnailBrowser( void );

	public:
		SevenupView *m_pPrimaryView;
		t_Handle m_TextureHandle;
		int m_TextureIndex;
		Sprite m_SceneList;
		SpriteItem m_SpriteList;
		Sprite* m_pSelectedSprite;

		PROJECTPARAMS m_Params;
		tstring m_DataPath;
		tstring m_ThumbnailDir;

	protected:
		virtual BOOL OnNewDocument();
		virtual BOOL OnOpenDocument( LPCTSTR pPathName );
		virtual BOOL OnSaveDocument( LPCTSTR pPathName );
		virtual void OnCloseDocument( void );

		BOOL SaveProject( FormattedFile &File, Sprite* pSceneList );
		BOOL LoadProject( FormattedFile &File, Sprite* pSceneList );
		BOOL LoadProject( MemoryBank &m, int SpriteCount, Sprite* pSceneList );
		BOOL SaveSpriteFamily( MemoryBank &m, Sprite *pSprite );

		BOOL ExportScreen( FormattedFile &File );
		BOOL ExportHeaders( LPCTSTR pFileName );
		BOOL ExportHeaders( HANDLE hFile );
		BOOL ExportStrings( LPCTSTR pFileName );
		BOOL ExportStrings( HANDLE hFile );

		void CreateTextureList( void );
		void CreateTextureList( NamedNode &TextureList );
		void CreateTextureBank( MemoryBank &m, NamedNode &TextureList );
		void AddTexturesToList( NamedNode& TextureList, Sprite* pSprite );

		BOOL CreateSwapChain( int Width, int Height );
		void DestroySwapChain( void );
		BOOL SetupRenderTarget( int Width, int Height );

		D3DPRESENT_PARAMETERS m_PresentParameters;
		LPDIRECT3DSWAPCHAIN9 m_pSwapChain;

	protected:
		afx_msg void OnFileCombine( void );
		afx_msg void OnFileExport( void );
		afx_msg void OnProjectSettings( void );


	protected:
		DECLARE_MESSAGE_MAP();
};

#endif // _DOCUMENT_H_