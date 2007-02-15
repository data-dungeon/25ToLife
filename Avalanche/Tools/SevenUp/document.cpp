#include "pch.h"
#include "application.h"
#include "document.h"
#include "dibble.h"
#include "fileinfo.h"
#include "mainframe.h"
#include "newscreendlg.h"
#include "projectpaths.h"
#include "sprite.h"
#include "texture.h"
#include "view.h"
#include "resource.h"

IMPLEMENT_DYNCREATE( SevenupDoc, CDocument );

BEGIN_MESSAGE_MAP( SevenupDoc, CDocument )
	ON_COMMAND( ID_FILE_COMBINE, OnFileCombine )
	ON_COMMAND( ID_FILE_EXPORT, OnFileExport )
	ON_COMMAND( ID_PROJECT_SETTINGS, OnProjectSettings )
END_MESSAGE_MAP()

//=============================================================================
// SevenupDoc::SevenupDoc
//=============================================================================

SevenupDoc::SevenupDoc()
{
	m_pPrimaryView = 0;
	m_TextureHandle = INVALID_HANDLE;
	m_TextureIndex = TS_NO_TEXTURE;
	m_pSwapChain = 0;
	m_pSelectedSprite = 0;

	memset( &m_Params, 0, sizeof(m_Params) );
	m_Params.m_Size = sizeof(m_Params);
}

//=============================================================================
// SevenupDoc::~SevenupDoc
//=============================================================================

SevenupDoc::~SevenupDoc()
{
	Destroy();
}

//=============================================================================
// SevenupDoc::NewDocument
//=============================================================================

void SevenupDoc::NewDocument( void )
{
	if( m_pPrimaryView )
		m_pPrimaryView->OnNewDocument();

	Destroy();

	//
	// Initialize the default parameters
	//

	m_Params.m_ScreenWidth = 640;
	m_Params.m_ScreenHeight = 480;
	m_Params.m_SafeZoneX = 5;
	m_Params.m_SafeZoneY = 5;
	m_Params.m_DrawBorder = TRUE;
	m_Params.m_DrawSafeZone = TRUE;

	SyncProjectViews();

	UpdateAllViews(0);
}

//=============================================================================
// SevenupDoc::OpenDocument
//
//		Opens the specified file and inserts it into the project.
//=============================================================================

BOOL SevenupDoc::OpenDocument( LPCTSTR pPathName, Sprite* pSceneList )
{
	FormattedFile File;
	MemoryBank m;

	if( !File.OpenFile( pPathName, FileStream::READ ) )
		return FALSE;

	BOOL Result = LoadProject( File, pSceneList );

	File.CloseFile();

	SyncProjectViews();

	return Result;
}

//=============================================================================
// SevenupDoc::SaveDocument
//=============================================================================

BOOL SevenupDoc::SaveDocument( LPCTSTR pFileName, Sprite* pSceneList )
{
	FormattedFile File;

	if( !File.OpenFile( pFileName, FileStream::WRITE ) )
		return FALSE;

	BOOL Result = SaveProject( File, pSceneList );

	File.CloseFile();

	return Result;
}

//=============================================================================
// SevenupDoc::SaveTexture
//=============================================================================

BOOL SevenupDoc::SaveTexture( LPCTSTR pTextureDir, t_Handle Handle )
{
	if( !pTextureDir )
		pTextureDir = "";

	return TextureMgr__SaveTexture( (char *) pTextureDir, Handle, 0 );
}

//=============================================================================
// SevenupDoc::Destroy
//=============================================================================

void SevenupDoc::Destroy( void )
{
	while( m_SceneList.Child() )
		DeleteSprite( m_SceneList.Child() );

	return;
}

//=============================================================================
// SevenupDoc::ExportDocument
//=============================================================================

BOOL SevenupDoc::ExportGameFiles( LPCTSTR pFileName )
{
	FormattedFile File;
	FileNameEx FileName(pFileName);

	tstring name;
	name.set( Printf( "%s\\%s.sc", FileName.Path(), FileName.Title() ) );

	if( !File.OpenFile( name.get(), FileStream::WRITE ) )
		return FALSE;

	BOOL bResult = ExportScreen( File );

	File.CloseFile();

	if( !bResult )
		return FALSE;

	if( bResult )
	{
		name.set( Printf( "%s\\%s.h", FileName.Path(), FileName.Title() ) );
		bResult = ExportHeaders( name.get() );
	}

	if( bResult )
	{
		name.set( Printf( "%s\\%s.sct", FileName.Path(), FileName.Title() ) );
		bResult = ExportStrings( name.get() );
	}

	return bResult;
}

HBITMAP SevenupDoc::CreateThumbnail( Sprite* pSceneList, int Width, int Height, BOOL SuperSample )
{
	if( pSceneList == 0 )
		pSceneList = GetSceneList();

	if( !pSceneList )
		return FALSE;

	float x1, y1;
	float x2, y2;

	pSceneList->GetFamilyRect( x1, y1, x2, y2 );

	if( Width < 1 )
		Width = (int) (x2 - x1);

	if( Height < 1 )
		Height = (int) (y2 - y1);

	if( SuperSample )
	{
		Width = Width * 4;
		Height = Height * 4;
	}

	HBITMAP hbm = CreateDIB( Width, Height, 32 );

	if( hbm )
		DrawThumbnail( hbm, pSceneList );

	if( SuperSample )
	{
		HBITMAP hbmTemp = ResampleDIB( hbm, 0, 0, Width, Height, (Width/4), (Height/4) );

		ReleaseDIB( hbm );

		hbm = hbmTemp;
	}

	return hbm;
}

BOOL SevenupDoc::DrawThumbnail( HBITMAP hbm, Sprite* pSceneList )
{
	if( pSceneList == 0 )
		pSceneList = GetSceneList();

	if( !hbm || !pSceneList )
		return FALSE;

	float x1, y1;
	float x2, y2;
	float Width, Height;

	pSceneList->GetFamilyRect( x1, y1, x2, y2 );

	Width = x2 - x1;
	Height = y2 - y1;

	Sprite::SimulateScreenResolution( Width, Height );
	Sprite::SetViewPortOrigin( x1, y1 );

	if( !SetupRenderTarget( GetBitmapWidth( hbm ), GetBitmapHeight( hbm ) ) )
		return FALSE;

	HRESULT hresult = D3DRESULT( d3d.Clear( 0, 0, D3DCLEAR_TARGET, 0x000000, 1.0f, 0 ) );

	m_SceneList.ReleaseHandlesRecursively();

	pSceneList->Update();

	Sprite::DrawAll();

	sm.RenderAllSprites( d3d );

	LPDIRECT3DSURFACE9 pRenderTarget;
	D3DSURFACE_DESC SurfaceDesc;
	D3DLOCKED_RECT Lock;
	
	hresult = D3DRESULT( d3d.GetRenderTarget( 0, &pRenderTarget ) );

	if( !FAILED(hresult) )
	{
		hresult = D3DRESULT( pRenderTarget->GetDesc( &SurfaceDesc ) );

		if( !FAILED(hresult) )
		{
			hresult = D3DRESULT( pRenderTarget->LockRect( &Lock, NULL, 0 ) );

			if( !FAILED(hresult) )
			{
				BITMAP2 bmDst = GetBitmap2Struct( hbm, FALSE );
				BITMAP2 bmSrc;

				bmSrc.Width = SurfaceDesc.Width;
				bmSrc.Height = SurfaceDesc.Height;
				bmSrc.Format = PixelFormatFromDirectX( SurfaceDesc.Format );
				bmSrc.Stride = Lock.Pitch;
				bmSrc.pBits = (BYTE *) Lock.pBits;

				BitmapToBitmap( &bmDst, 0, 0, bmDst.Width, bmDst.Height, &bmSrc, 0, 0, bmSrc.Width, bmSrc.Height );

				pRenderTarget->UnlockRect();
			}
		}

		pRenderTarget->Release();
	}

	m_SceneList.ReleaseHandlesRecursively();

	return TRUE;
}

//=============================================================================
// SevenupDoc::CreateObject
//=============================================================================

Sprite* SevenupDoc::CreateObject( HUDOBJECTTYPE Type )
{
	Sprite* pSprite = Project.NewSprite( Type );

	if( pSprite )
		InitObject( pSprite );

	return pSprite;
}

//=============================================================================
// SevenupDoc::InitObject
//=============================================================================

void SevenupDoc::InitObject( Sprite* pSprite )
{
	if( pSprite )
	{
		const char* pClassname = pSprite->Classname();

		if( pClassname )
		{
			if( 0 == strcmp( "Sprite", pClassname ) )
			{
				InitSprite( pSprite );
			}
			else
			if( 0 == strcmp( "Text", pClassname ) )
			{
				InitText( (Text*) pSprite );
			}
		}
	}

	return;
}

//=============================================================================
// SevenupDoc::InitSprite
//=============================================================================

void SevenupDoc::InitSprite( Sprite* pSprite )
{
	if( pSprite )
	{
		t_Handle Handle = GetTextureHandle();
		int Texture = GetTextureIndex();

		float cx = 0.5f * m_Params.m_ScreenWidth;
		float cy = 0.5f * m_Params.m_ScreenHeight;

		pSprite->Create( Handle, Texture );
		pSprite->SetOrigin( 0.5f, 0.5f );
		pSprite->SetPosition( cx, cy );
	}

	return;
}

//=============================================================================
// SevenupDoc::InitText
//=============================================================================

void SevenupDoc::InitText( Text* pText )
{
	MainFrame* pMainFrame = (MainFrame*) AfxGetMainWnd();

	if( pText && pMainFrame )
	{
		float cx = 0.5f * m_Params.m_ScreenWidth;
		float cy = 0.5f * m_Params.m_ScreenHeight;

		Font* pFont = Font::FindFont( pMainFrame->m_ToolBar.GetCurrentFont() );

		pText->SetFont( pFont );
		pText->SetPosition( cx, cy );
		pText->SetOrigin( 0.5f, 0.5f );
		pText->SetTextHeight( 32.0f );
		
		if( pFont )
			pText->SetText( pFont->Name() );
	}

	return;
}

//=============================================================================
// SevenupDoc::Insert
//=============================================================================

void SevenupDoc::Insert( Sprite* pSprite, Sprite* pParent )
{
	if( pSprite )
	{
		ProjectItem* pProjectItem = Project.GetProjectItem( pSprite );

		if( !pProjectItem )
			Project.CreateProjectItem( pSprite );

		if( pParent == 0 )
			pParent = GetSceneList();

		pParent->AddChild( pSprite );

		SyncProjectViews();
	}

	return;
}

//=============================================================================
// SevenupDoc::SelectSprite
//=============================================================================

void SevenupDoc::SelectSprite( Sprite* pSprite )
{
	if( m_pSelectedSprite != pSprite )
		UpdateAllViews(0);

	m_pSelectedSprite = pSprite;
}

//=============================================================================
// SevenupDoc::GetSelectedSprite
//=============================================================================

Sprite* SevenupDoc::GetSelectedSprite( void )
{
	return m_pSelectedSprite;
}

//=============================================================================
// SevenupDoc::DeleteSprite
//=============================================================================

Sprite * SevenupDoc::DeleteSprite( Sprite *pSprite )
{
	if( pSprite && pSprite->m_pToolData )
	{
		if( m_pSelectedSprite == pSprite )
			m_pSelectedSprite = 0;

		Sprite *pChild = pSprite->Child();
		Sprite *pNext;

		while( pChild )
		{
			pNext = pChild->Next();

			if( pChild->m_pToolData )
				DeleteSprite( pChild );

			pChild = pNext;
		}

		ProjectView* pProjectView = ProjectView::GetFirst();

		while( pProjectView )
		{
			pProjectView->RemoveSprite( pSprite );
			pProjectView = ProjectView::GetNext( pProjectView );
		}

		delete (ProjectItem*) pSprite->m_pToolData;
		delete pSprite;
	}

	return 0;
}


//=============================================================================
// SevenupDoc::DuplicateSprite
//=============================================================================

Sprite * SevenupDoc::DuplicateSprite( Sprite *pSprite )
{
	Sprite *pNewSprite = Project.DuplicateSprite( pSprite );

	if( pNewSprite )
		AddToProject( pNewSprite );

	return pNewSprite;
}

//=============================================================================
// SevenupDoc::DuplicateSpriteFamily
//=============================================================================

Sprite * SevenupDoc::DuplicateSpriteFamily( Sprite *pSprite )
{
	Sprite *pNewSprite = 0;

	if( pSprite )
	{
		pNewSprite = Project.DuplicateSpriteFamily( pSprite );

		if( pNewSprite )
			Insert( pNewSprite, 0 );
	}

	return pNewSprite;
}

//=============================================================================
// SevenupDoc::GetSceneList
//=============================================================================

Sprite* SevenupDoc::GetSceneList( void )
	{
	return &m_SceneList;
}

//=============================================================================
// SevenupDoc::GetSpriteAtPosition
//=============================================================================

Sprite * SevenupDoc::GetSpriteAtPosition( float x, float y, Sprite *pNotThisSprite )
{
	Sprite *pSprite = (Sprite *) m_SceneList.Child();

	float x1, y1;
	float x2, y2;

	if( pSprite )
	{
		while( pSprite->Child() )
			pSprite = (Sprite *) pSprite->Child();

		while( pSprite )
		{
			pSprite->GetSpriteRect( x1, y1, x2, y2 );

			if( x >= x1 && x <= x2 && y >= y1 && y <= y2 )
			{
				if( pSprite != pNotThisSprite )
					return pSprite;
			}

			if( !pSprite->Next() )
				pSprite = (Sprite *) pSprite->Parent();
			else
			{
				pSprite = (Sprite *) pSprite->Next();

				if( pSprite )
				{
					while( pSprite->Child() )
						pSprite = (Sprite *) pSprite->Child();
				}
			}
		}
	}

	return 0;
}

//=============================================================================
// SevenupDoc::AddSpriteToList
//=============================================================================

void SevenupDoc::AddSpriteToList( Sprite* pSprite )
{
	SpriteItem* pNewItem = new SpriteItem;

	if( pNewItem )
		pNewItem->m_pSprite = pSprite;

	m_SpriteList.AddChild( pNewItem );
}

//=============================================================================
// SevenupDoc::ClearSpriteList
//=============================================================================

void SevenupDoc::ClearSpriteList( void )
{
	while( m_SpriteList.Child() )
		delete m_SpriteList.Child();

	return;
}

//=============================================================================
// SevenupDoc::GetSpritesAtPosition
//=============================================================================

SpriteItem* SevenupDoc::GetSpritesAtPosition( float x, float y, Sprite* pRoot )
{
	ClearSpriteList();

	Sprite* pSceneList = GetSceneList();
	Sprite* pSprite;

	if( pRoot )
		pSceneList = pRoot;

	AddSpritesAtPosition( pRoot, x, y );

	if( pSceneList )
	{
		pSprite = pSceneList->Child();

		while( pSprite )
		{
			AddSpritesAtPosition( pSprite, x, y );

			pSprite = pSprite->Next();
		}
	}

	return &m_SpriteList;
}

void SevenupDoc::AddSpritesAtPosition( Sprite* pSprite, float x, float y )
{
	float x1, y1;
	float x2, y2;

	if( pSprite )
	{
		pSprite->GetSpriteRect( x1, y1, x2, y2 );

		if( x >= x1 && x <= x2 && y >= y1 && y <= y2 )
			AddSpriteToList( pSprite );

		pSprite = pSprite->Child();

		while( pSprite )
		{
			AddSpritesAtPosition( pSprite, x, y );

			pSprite = pSprite->Next();
		}
	}

	return;
}

//=============================================================================
// SevenupDoc::SetTexture
//=============================================================================

void SevenupDoc::SetTexture( t_Handle Handle, int TextureIndex )
{
	m_TextureHandle = Handle;
	m_TextureIndex = TextureIndex;

	SyncThumbnailBrowser();
}

//=============================================================================
// SevenupDoc::GetTextureHandle
//=============================================================================

t_Handle SevenupDoc::GetTextureHandle( void )
{
	return m_TextureHandle;
}

//=============================================================================
// SevenupDoc::GetTextureIndex
//=============================================================================

int SevenupDoc::GetTextureIndex( void )
{
	return m_TextureIndex;
}

void SevenupDoc::NewObjectInitialize( Sprite *pSprite )
{
	if( pSprite )
	{
		HUDOBJECTTYPE Type = Project.GetObjectType( pSprite );

		switch( Type )
		{
		case HUDOBJECT_SPRITE:
			NewSpriteInitialize( pSprite );
			break;
		case HUDOBJECT_TEXT:
			NewTextInitialize( (Text *) pSprite );
			break;
		}
	}

	return;
}

//=============================================================================
// SevenupDoc::NewSpriteInitialize
//=============================================================================

void SevenupDoc::NewSpriteInitialize( Sprite *pSprite )
{
	if( pSprite )
	{
		float cx = 0.5f * Sprite::SimulatedScreenWidth();
		float cy = 0.5f * Sprite::SimulatedScreenHeight();

		pSprite->SetPosition( cx, cy );
		pSprite->SetOrigin( 0.5f, 0.5f );
		pSprite->SetTextureSet( m_TextureHandle, m_TextureIndex );
	}

	return;
}

//=============================================================================
// SevenupDoc::NewTextInitialize
//=============================================================================

void SevenupDoc::NewTextInitialize( Text *pText )
{
	if( pText )
	{
		float cx = 0.5f * Sprite::SimulatedScreenWidth();
		float cy = 0.5f * Sprite::SimulatedScreenHeight();

		char* pDefaultText = "Text";
		Font* pGameFont = App.GetGameFont();

		if( pGameFont )
			pDefaultText = pGameFont->m_FontName.get();

		pText->SetFont( pGameFont );
		pText->SetPosition( cx, cy );
		pText->SetOrigin( 0.5f, 0.5f );
		pText->SetTextHeight( 32.0f );
		pText->SetText( pDefaultText );
	}

	return;
}

//=============================================================================
// SevenupDoc::AddToProject
//=============================================================================

BOOL SevenupDoc::AddToProject( Sprite* pSprite, Sprite* pParent )
{
	if( pSprite )
	{
		if( !pSprite->m_pToolData )
		{
			ProjectItem *pProjectItem = new ProjectItem;

			if( !pProjectItem )
				return FALSE;

			pSprite->m_pToolData = pProjectItem;

			pProjectItem->SetName( "Unnamed" );
			pProjectItem->SetDescription( "" );
			pProjectItem->SetId( 0 );
			pProjectItem->SetSprite( pSprite );
		}

		if( !pParent )
			pParent = &m_SceneList;

		pParent->AddChild( pSprite );

		SyncProjectViews();
	}
	
	return TRUE;
}

//=============================================================================
// SevenupDoc::SyncProjectView
//=============================================================================

void SevenupDoc::SyncProjectViews( void )
{
	ProjectView::SyncProjectViews( this );
}

//=============================================================================
// SevenupDoc::SyncThumbnailBrowser
//=============================================================================

void SevenupDoc::SyncThumbnailBrowser( void )
{
	if( m_pPrimaryView )
	{
		ThumbnailView *pThumbnailView = m_pPrimaryView->m_pThumbnailView;

		if( pThumbnailView )
		{
			// har har its not implemented yet.
		}
	}

	return;
}

//=============================================================================
// SevenupDoc::OnNewDocument
//=============================================================================

BOOL SevenupDoc::OnNewDocument()
{
	if( !CDocument::OnNewDocument() )
		return FALSE;

	NewDocument();

	return TRUE;
}

//=============================================================================
// SevenupDoc::OnOpenDocument
//
//		This is called by the framework when a file is to be opened.
//=============================================================================

BOOL SevenupDoc::OnOpenDocument( LPCTSTR pPathName )
{
	NewDocument();

	return OpenDocument( pPathName, 0 );
}

//=============================================================================
// SevenupDoc::OnSaveDocument
//
//		This is called by the framework when the project is to be saved.
//=============================================================================

BOOL SevenupDoc::OnSaveDocument( LPCTSTR pPathName )
{
	if( !SaveDocument( pPathName, 0 ) )
		return FALSE;

	return ExportGameFiles( pPathName );
}

//=============================================================================
// SevenupDoc::OnCloseDocument
//
//		This is called by the framework the document is closed.
//=============================================================================

void SevenupDoc::OnCloseDocument( void )
{
	CDocument::OnCloseDocument();
}

//=============================================================================
// SevenupDoc::SaveProject
//=============================================================================

BOOL SevenupDoc::SaveProject( FormattedFile &File, Sprite* pSceneList )
{
	MemoryBank ThumbnailBank;
	MemoryBank ProjectBank;
	MemoryBank SpriteBank;

	u32 SpriteCode1 = PROJECTFILE_SPRITECODE1;
	u32 SpriteCode2 = PROJECTFILE_SPRITECODE2;
	u32 SpriteCount = m_SceneList.GetNumChildren();

	SpriteBank.BeginWrite();
	SpriteBank.Write( &SpriteCode1, sizeof(SpriteCode1) );
	SpriteBank.Write( &SpriteCode2, sizeof(SpriteCode2) );
	SpriteBank.Write( &SpriteCount, sizeof(SpriteCount) );

	BOOL bResult = TRUE;

	Sprite *pSprite = pSceneList;

	if( !pSprite )
	{
		pSprite = GetSceneList();

		if( pSprite )
			pSprite = pSprite->Child();
	}

	ProjectItem *p;

	while( pSprite )
	{
		p = (ProjectItem *) pSprite->m_pToolData;

		if( p )
		{
			if( !p->SaveObject( SpriteBank, m_DataPath.get(), FALSE ) )
			{
				SpriteBank.Destroy();
				break;
			}
		}

		if( pSprite != pSceneList )
			pSprite = pSprite->Next();
		else
			pSprite = 0;
	}

	HBITMAP hbm = CreateThumbnail( pSceneList, 128, 128, TRUE );

	if( hbm )
	{
		BITMAP2 bm = GetBitmap2Struct( hbm, FALSE );
		int Stride = bm.Stride;
		bm.Stride = GetImageStride( bm.Width, PixelFormatToBPP(bm.Format) );

		ThumbnailBank.BeginWrite();
		ThumbnailBank.Write( &bm, sizeof(bm) );
		
		for( int y = 0; y < bm.Height; y++ )
			ThumbnailBank.Write( &bm.pBits[y*Stride], bm.Stride );
	}

	ProjectBank.BeginWrite();
	ProjectBank.WriteString( m_DataPath.get() );
	ProjectBank.WriteString( App.m_ThumbnailPath.get() );
	ProjectBank.Write( &m_Params, sizeof(m_Params) );

	File.Branch( PROJECTFILE_MARKER, PROJECTFILE_VERSION );
	File.Branch( PROJECTFILE_THUMBNAILMARKER, PROJECTFILE_THUMBNAILVERSION );
	File.Write( ThumbnailBank );
	File.Return();
	File.Branch( PROJECTFILE_SPRITEMARKER, PROJECTFILE_SPRITEVERSION );
	File.Write( SpriteBank );
	File.Return();
	File.Branch( PROJECTFILE_PROJECTMARKER, PROJECTFILE_PROJECTVERSION );
	File.Write( ProjectBank );
	File.Return();
	File.Return();

	return TRUE;
}

//=============================================================================
// SevenupDoc::LoadProject
//=============================================================================

BOOL SevenupDoc::LoadProject( FormattedFile &File, Sprite* pSceneList )
{
	MemoryBank m;
	BOOL Result = FALSE;

	if( File.Branch( PROJECTFILE_MARKER, PROJECTFILE_VERSION ) )
	{
		if( File.Branch( PROJECTFILE_THUMBNAILMARKER, PROJECTFILE_THUMBNAILVERSION ) )
		{
			if( File.Read( m ) )
			{
			}

			File.Return();
		}

		if( File.Branch( PROJECTFILE_SPRITEMARKER, PROJECTFILE_SPRITEVERSION_V1 ) )
		{
			if( File.Read( m ) )
			{
				u32 SpriteCode1;
				u32 SpriteCode2;
				u32 SpriteCount;

				m.BeginRead();
				m.Read( &SpriteCode1, sizeof(SpriteCode1) );
				m.Read( &SpriteCode2, sizeof(SpriteCode2) );
				m.Read( &SpriteCount, sizeof(SpriteCount) );

				if(SpriteCode1 == PROJECTFILE_SPRITECODE1 &&
					SpriteCode2 == PROJECTFILE_SPRITECODE2 )
				{
					Result = LoadProject( m, SpriteCount, pSceneList );
				}
			}

			File.Return();
		}

		if( Result == TRUE )
		{
			if( File.Branch( PROJECTFILE_PROJECTMARKER, PROJECTFILE_PROJECTVERSION_V4 ) )
			{
				if( File.Read( m ) )
				{
					int ProjectSize;

					xstring DataPath;
					xstring ThumbnailPath;

					m.BeginRead();
					m.ReadString( DataPath );
					m.ReadString( ThumbnailPath );
					m_DataPath.set( DataPath.get() );

					App.SetThumbnailDirectory( ThumbnailPath.get() );

					if( m_pPrimaryView )
					{
						ImageExplorer* ie = m_pPrimaryView->ThumbnailGetImageExplorer();

					//	if( ie )
					//		ie->SetCurrentDirectory( ThumbnailPath.get() );
					}

					m.Read( &ProjectSize, sizeof(ProjectSize) );

					if( ProjectSize == sizeof(PROJECTPARAMS) )
					{
						m.Read( &(&m_Params.m_Size)[1], ProjectSize - sizeof(ProjectSize) );
					}
					else
					{
						AfxMessageBox( "The project format has changed since this file was created.\nSome project information will be discarded." );
					}
				}
			}
			else
			if( File.Branch( PROJECTFILE_PROJECTMARKER, PROJECTFILE_PROJECTVERSION_V3 ) )
			{
				if( File.Read( m ) )
				{
					int ProjectSize;

					xstring GameDir;
					xstring DataDir;
					xstring DataPC;
					xstring DataDX;
					xstring DataPS2;
					xstring DataGCN;
					xstring DataXBOX;
					xstring DataPath;
					xstring ThumbnailPath;

					m.BeginRead();
					m.ReadString( GameDir );
					m.ReadString( DataDir );
					m.ReadString( DataPC );
					m.ReadString( DataDX );
					m.ReadString( DataPS2 );
					m.ReadString( DataGCN );
					m.ReadString( DataXBOX );
					m.ReadString( DataPath );
					m.ReadString( ThumbnailPath );

					m_DataPath.set( DataPath.get() );
					m_ThumbnailDir.set( ThumbnailPath.get() );

					App.SetThumbnailDirectory( ThumbnailPath.get() );

					if( m_pPrimaryView )
					{
						ImageExplorer* ie = m_pPrimaryView->ThumbnailGetImageExplorer();

					//	if( ie )
					//		ie->SetCurrentDirectory( ThumbnailPath.get() );
					}

					m.Read( &ProjectSize, sizeof(ProjectSize) );

					if( ProjectSize == sizeof(PROJECTPARAMS) )
					{
						m.Read( &(&m_Params.m_Size)[1], ProjectSize - sizeof(ProjectSize) );
					}
					else
					{
						AfxMessageBox( "The project format has changed since this file was created.\nSome project information will be discarded." );
					}
				}
			}
			else
			if( File.Branch( PROJECTFILE_PROJECTMARKER, PROJECTFILE_PROJECTVERSION_V2 ) )
			{
				if( File.Read( m ) )
				{
					int ProjectSize;

					xstring GameDir;
					xstring DataDir;
					xstring DataPC;
					xstring DataPS2;
					xstring DataGCN;
					xstring DataXBOX;
					xstring DataPath;
					xstring ThumbnailPath;

					m.BeginRead();
					m.ReadString( GameDir );
					m.ReadString( DataDir );
					m.ReadString( DataPC );
					m.ReadString( DataPS2 );
					m.ReadString( DataGCN );
					m.ReadString( DataXBOX );
					m.ReadString( DataPath );
					m.ReadString( ThumbnailPath );

					m_DataPath.set( DataPath.get() );
					m_ThumbnailDir.set( ThumbnailPath.get() );

					App.SetThumbnailDirectory( ThumbnailPath.get() );

					if( m_pPrimaryView )
					{
						ImageExplorer* ie = m_pPrimaryView->ThumbnailGetImageExplorer();

					//	if( ie )
					//		ie->SetCurrentDirectory( ThumbnailPath.get() );
					}

					m.Read( &ProjectSize, sizeof(ProjectSize) );

					if( ProjectSize == sizeof(PROJECTPARAMS) )
					{
						m.Read( &(&m_Params.m_Size)[1], ProjectSize - sizeof(ProjectSize) );
					}
					else
					{
						AfxMessageBox( "The project format has changed since this file was created.\nSome project information will be discarded." );
					}
				}
			}
			else
			if( File.Branch( PROJECTFILE_PROJECTMARKER, PROJECTFILE_PROJECTVERSION_V1 ) )
			{
				AfxMessageBox( "This project was saved using an out date version of the software.\nBe sure to save the project to bring it up-to-date." );

				if( File.Read( m ) )
				{
					int ProjectSize;

					xstring GameDir;
					xstring DataDir;
					xstring DataPC;
					xstring DataPS2;
					xstring DataGCN;
					xstring DataXBOX;
					xstring DataPath;

					m.BeginRead();
					m.ReadString( GameDir );
					m.ReadString( DataDir );
					m.ReadString( DataPC );
					m.ReadString( DataPS2 );
					m.ReadString( DataGCN );
					m.ReadString( DataXBOX );
					m.ReadString( DataPath );

					m_DataPath.set( DataPath.get() );

					m.Read( &ProjectSize, sizeof(ProjectSize) );

					if( ProjectSize == sizeof(PROJECTPARAMS) )
					{
						m.Read( &(&m_Params.m_Size)[1], ProjectSize - sizeof(ProjectSize) );
					}
					else
					{
						AfxMessageBox( "The project format has changed since this file was created.\nSome project information will be discarded." );
					}
				}
			}
			else
			{
				AfxMessageBox( "The project version is out of date and is no longer supported.\nSome project information will be discarded." );
			}

			File.Return();
		}

		File.Return();
	}

	return Result;
}

//=============================================================================
// SevenupDoc::LoadProject
//=============================================================================

BOOL SevenupDoc::LoadProject( MemoryBank &m, int SpriteCount, Sprite* pSceneList )
{
	ProjectItem *pProjectItem;

	if( pSceneList == 0 )
		pSceneList = &m_SceneList;

	for( int i = 0; i < SpriteCount; i++ )
	{
		pProjectItem = new ProjectItem;

		if( !pProjectItem )
			return FALSE;

		if( !pProjectItem->LoadObject( m, pSceneList ) )
		{
			delete pProjectItem;
			return FALSE;
		}
	}

	return TRUE;
}

//=============================================================================
// SevenupDoc::ExportScreen
//=============================================================================

BOOL SevenupDoc::ExportScreen( FormattedFile &File )
{
	NamedNode TextureList;
	MemoryBank TextureBank;
	MemoryBank SpriteBank;

	TextureBank.BeginWrite();

	CreateTextureList( TextureList );
	CreateTextureBank( TextureBank, TextureList );

	u32 SpriteCode1 = PROJECTFILE_SPRITECODE1;
	u32 SpriteCode2 = PROJECTFILE_SPRITECODE2;
	u32 SpriteCount = m_SceneList.GetNumChildren();

	SpriteBank.BeginWrite();
	SpriteBank.Write( &SpriteCode1, sizeof(SpriteCode1) );
	SpriteBank.Write( &SpriteCode2, sizeof(SpriteCode2) );
	SpriteBank.Write( &SpriteCount, sizeof(SpriteCount) );

	BOOL bResult = TRUE;

	Sprite *pSprite = m_SceneList.Child();
	ProjectItem *pProjectItem;

	while( pSprite )
	{
		pProjectItem = (ProjectItem *) pSprite->m_pToolData;

		if( pProjectItem )
		{
			if( !pProjectItem->SaveObject( SpriteBank, m_DataPath.get(), TRUE ) )
			{
				SpriteBank.Destroy();
				break;
			}
		}

		pSprite = pSprite->Next();
	}

	File.Branch( PROJECTFILE_MARKER, PROJECTFILE_VERSION );
	File.Branch( PROJECTFILE_TEXTUREMARKER, PROJECTFILE_TEXTUREVERSION );
	File.Write( TextureBank );
	File.Return();
	File.Branch( PROJECTFILE_SPRITEMARKER, PROJECTFILE_SPRITEVERSION );
	File.Write( SpriteBank );
	File.Return();
	File.Return();

	return TRUE;
}

//=============================================================================
// SevenupDoc::ExportHeaders
//=============================================================================

BOOL SevenupDoc::ExportHeaders( LPCTSTR pFileName )
{
	HANDLE hFile = FileCreate( pFileName );

	if( hFile == INVALID_HANDLE_VALUE )
		return FALSE;

	BOOL bResult = ExportHeaders( hFile );

	CloseHandle( hFile );

	return bResult;
}

//=============================================================================
// SevenupDoc::ExportHeaders
//=============================================================================

BOOL SevenupDoc::ExportHeaders( HANDLE hFile )
{
	if( hFile == INVALID_HANDLE_VALUE )
		return FALSE;

	ItemName *pItemName = (ItemName *) ProjectItem::m_ItemNames.Child();

	while( pItemName )
	{
		if( 0 != pItemName->m_Name.stricmp( "UNNAMED" ) )
		{
			if( !FileWriteStr( hFile, Printf( "#define %-16s (%d)\r\n", pItemName->m_Name.get(), pItemName->ObjectId() ) ) )
				return FALSE;
		}

		pItemName = (ItemName *) pItemName->Next();
	}

	return TRUE;
}

//=============================================================================
// SevenupDoc::ExportStrings
//=============================================================================

BOOL SevenupDoc::ExportStrings( LPCTSTR pFileName )
{
	HANDLE hFile = FileCreate( pFileName );

	if( hFile == INVALID_HANDLE_VALUE )
		return FALSE;

	BOOL bResult = ExportStrings( hFile );

	CloseHandle( hFile );

	return bResult;
}


//=============================================================================
// SevenupDoc::ExportStrings
//=============================================================================

BOOL SevenupDoc::ExportStrings( HANDLE hFile )
{
	Sprite* pSprite = m_SceneList.Child();
	char* pstring = 0;
	int size = 2048;
	Text* pText;
	BOOL FindNext = FALSE;

	while( pSprite )
	{
		if( !FindNext )
		{
			if( 0 == stricmp( pSprite->Classname(), Text::ClassnameStatic() ) )
			{
				pText = (Text*) pSprite;

				if( size < pText->m_Text.length() + 128 )
				{
					size = pText->m_Text.length() + 128;
					delete [] pstring;
					pstring = 0;
				}

				if( !pstring )
					pstring = new char[size];

				if( pstring )
				{
					strcpy( pstring, "#$" );
					strcat( pstring, pText->GetText() );
					strcat( pstring, "#%\r\n" );

					FileWriteStr( hFile, pstring );
				}
			}
		}

		if( !FindNext && pSprite->Child() )
			pSprite = pSprite->Child();
		else
		if( pSprite->Next() )
		{
			FindNext = FALSE;
			pSprite = pSprite->Next();
		}
		else
		{
			FindNext = TRUE;
			pSprite = pSprite->Parent();
		}
	}

	if( pstring )
		delete [] pstring;

	return TRUE;
}

//=============================================================================
// SevenupDoc::CreateTextureList
//=============================================================================

void SevenupDoc::CreateTextureList( NamedNode &TextureList )
{
	Sprite* pSprite = m_SceneList.Child();

	while( pSprite )
	{
		AddTexturesToList( TextureList, pSprite );

		pSprite = pSprite->Next();
	}

	return;
}

//=============================================================================
// SevenupDoc::CreateTextureBank
//=============================================================================

void SevenupDoc::CreateTextureBank( MemoryBank &m, NamedNode &TextureList )
{
	NamedNode* pTexture;

	u32 c = TextureList.GetNumChildren();

	m.Write( &c, sizeof(c) );

	pTexture = TextureList.Child();

	for( int i = 0; pTexture; i++ )
	{
		m.WriteString( (void*) pTexture->ObjectName() );

		pTexture = pTexture->Next();
	}

	return;
}

//=============================================================================
// SevenupDoc::AddTexturesToList
//=============================================================================

void SevenupDoc::AddTexturesToList( NamedNode& TextureList, Sprite* pSprite )
{
	NamedNode *pTexture;
	t_Handle TextureSetHandle;
	tstring TextureName;

	if( pSprite )
	{
		TextureSetHandle = pSprite->GetTextureSet();

		if( TextureSetHandle != INVALID_HANDLE )
		{
			TextureName.set( dblGetFileName( TextureSetHandle ) );
			TextureName.tolower();

			pTexture = TextureList.FindChildByName( TextureName.get() );

			if( !pTexture )
			{
				pTexture = new NamedNode;

				if( pTexture )
				{
					pTexture->SetParent( &TextureList );
					pTexture->SetObjectName( TextureName.get() );
				}
			}
		}

		Sprite* pChild = pSprite->Child();

		while( pChild )
		{
			AddTexturesToList( TextureList, pChild );

			pChild = pChild->Next();
		}
	}

	return;
}

//=============================================================================
// SevenupDoc::CreateSwapChain
//=============================================================================

BOOL SevenupDoc::CreateSwapChain( int Width, int Height )
{
	D3DDISPLAYMODE DisplayMode;
	HRESULT hresult;

	hresult = D3DRESULT( d3d.GetDisplayMode( 0, &DisplayMode ) );

	if( FAILED(hresult) )
		return FALSE;

	if( Width < 1 )
		Width = m_Params.m_ScreenWidth;

	if( Height < 1 )
		Height = m_Params.m_ScreenHeight;

	if( m_pSwapChain )
	{
		if(m_PresentParameters.BackBufferWidth  != Width ||
			m_PresentParameters.BackBufferHeight != Height ||
			m_PresentParameters.BackBufferFormat != DisplayMode.Format )
		{
			DestroySwapChain();
		}
	}

	if( !m_pSwapChain )
	{
		m_PresentParameters.BackBufferWidth = Width;
		m_PresentParameters.BackBufferHeight = Height;
		m_PresentParameters.BackBufferFormat = DisplayMode.Format;
		m_PresentParameters.BackBufferCount = 1;
		m_PresentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
		m_PresentParameters.MultiSampleQuality = 0;
		m_PresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_PresentParameters.hDeviceWindow = 0;
		m_PresentParameters.Windowed = TRUE;
		m_PresentParameters.EnableAutoDepthStencil = TRUE;
		m_PresentParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
		m_PresentParameters.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
		m_PresentParameters.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		m_PresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

		hresult = D3DRESULT( d3d.CreateAdditionalSwapChain( &m_PresentParameters, &m_pSwapChain ) );

		if( FAILED(hresult) )
			return FALSE;
	}

	return TRUE;
}

//=============================================================================
// SevenupDoc::DestroySwapChain
//=============================================================================

void SevenupDoc::DestroySwapChain( void )
{
	if( m_pSwapChain )
	{
		m_pSwapChain->Release();
		m_pSwapChain = 0;
	}

	return;
}

//=============================================================================
// SevenupDoc::SetupRenderTarget
//=============================================================================

BOOL SevenupDoc::SetupRenderTarget( int Width, int Height )
{
	if( !CreateSwapChain( Width, Height ) )
		return FALSE;

	LPDIRECT3DSURFACE9 pBackBuffer;
	HRESULT hresult;

	hresult = D3DRESULT( m_pSwapChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer ) );

	if( FAILED(hresult) )
		return FALSE;

	pBackBuffer->Release();

	hresult = D3DRESULT( d3d.SetRenderTarget( 0, pBackBuffer ) );

	if( FAILED(hresult) )
		return FALSE;

	return TRUE;
}

//=============================================================================
// SevenupDoc::OnFileCombine
//=============================================================================

void SevenupDoc::OnFileCombine( void )
{
	CString FileName;

	if( App.AskDocOpen( FileName ) )
	{
		OpenDocument( FileName, 0 );
	}

	return;
}

//=============================================================================
// SevenupDoc::OnFileExport
//=============================================================================

void SevenupDoc::OnFileExport( void )
{
	CWnd *pMainWnd = AfxGetMainWnd();
	HWND hMainWnd = 0;

	if( pMainWnd )
		hMainWnd = pMainWnd->m_hWnd;

	LPCTSTR pFileName = AskFileSave( hMainWnd, 0, "Screen Binary (*.sc)\0*.sc\0\0" );

	if( pFileName )
	{
		BOOL bResult = ExportGameFiles( pFileName );

		if( !bResult )
			AfxMessageBox( Printf( "An error occured while saving files.\n%s", pFileName ), MB_OK|MB_ICONSTOP );
	}

	return;
}

//=============================================================================
// SevenupDoc::OnProjectSettings
//=============================================================================

void SevenupDoc::OnProjectSettings( void )
{
	ProjectSettings PropertySheet;

	ProjectPaths &paths = PropertySheet.m_ProjectPaths;

	paths.m_DataPath.set( m_DataPath.get() );

	ScreenSettings &screen = PropertySheet.m_ScreenSettings;

	screen.m_ScreenWidth = m_Params.m_ScreenWidth;
	screen.m_ScreenHeight = m_Params.m_ScreenHeight;
	screen.m_SafeZoneX = m_Params.m_SafeZoneX;
	screen.m_SafeZoneY = m_Params.m_SafeZoneY;
	screen.m_DrawBorder = m_Params.m_DrawBorder;
	screen.m_DrawSafeZone = m_Params.m_DrawSafeZone;

	UINT_PTR Result = PropertySheet.DoModal();

	if( Result == IDOK )
	{
		m_DataPath.set( paths.m_DataPath.get() );

		m_Params.m_ScreenWidth = screen.m_ScreenWidth;
		m_Params.m_ScreenHeight = screen.m_ScreenHeight;
		m_Params.m_SafeZoneX = screen.m_SafeZoneX;
		m_Params.m_SafeZoneY = screen.m_SafeZoneY;
		m_Params.m_DrawBorder = screen.m_DrawBorder;
		m_Params.m_DrawSafeZone = screen.m_DrawSafeZone;

		UpdateAllViews(0);
	}

	return;
}