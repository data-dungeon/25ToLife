///////////////////////////////////////////////////////////////////////////////
// The class to load custom tags...
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "hud/hudlib.h"
#include "effects/customtag.h"
#ifdef PS2
#include "Display/PS2/Renderer/Code/MasterList.h"
#endif
#include "Game/Managers/TextureMgr/PlatformImage.h"
#include "Network/Connection.h"
#include "Environ/ConfigMgr.h"
#include "frontend/texteffects.h"
#include "frontend/frontend.h"

// This is hot!
DeclareSingleton(CustomTag);

// Our decal set
DecalSet *CustomTag::s_decalSet = NULL;
int CustomTag::s_decalSetReferences = 0;

#if defined(PS2) || defined(_XBOX)
TextureDatabase* CustomTag::s_textureDB = 0;
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CustomTag::CustomTag()
{
	m_source = TAG_NONE;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CustomTag::~CustomTag()
{
	if (m_source == TAG_DBL)
		ReleaseReference();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CustomTag::Initialize()
{
	if (m_source == TAG_NONE)
	{
		AcquireReference();
		m_source = TAG_DBL;
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CustomTag::PreloadFonts()
{
	Font::LoadFont( "WildStyle4" );
	Font::LoadFont( "WildStyle2" );
	Font::LoadFont( "WildStyle5" );
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Tizag* CustomTag::SetupEmitter(uint8 index)
{
	// Sanity check
	ASSERT(s_decalSet && s_decalSet->Okay() && s_decalSet->Textures() > 0);
	if (!s_decalSet || !s_decalSet->Okay() || s_decalSet->Textures() == 0)
		return NULL;

	// Wee
	Tizag* emitter = new Tizag(*s_decalSet, index, 8, 4);

	return emitter;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CustomTag::AcquireReference()
{
	ASSERT(s_decalSetReferences >= 0);
	if (s_decalSetReferences++ == 0)
	{
		ASSERT(!s_decalSet);

#if defined(PS2) || defined(_XBOX)
		Sprite::DrawCancel();

		const int tw = 64;
		const int th = 64;

		Referee &ref = g_referee;
		ts_TagParams TagParams;
		Text TagText[3];
		Font* pTagFont[3];
		Font* pFont;
		Sprite TagRect;
		Texture* pColorTexture;
		Texture* pGreenTexture;

		float FontHeight;
		float FontScale;
		float ScaleX;
		float ScaleY;
		float Scale;
		float BaseLine;
		float Offset;
		int n;

		float ScreenWidth  = Sprite::SimulatedScreenWidth();
		float ScreenHeight = Sprite::SimulatedScreenHeight();

		t_Handle Background = INVALID_HANDLE;

		if( Font::GetFirstFont() )
			Background = Font::GetFirstFont()->m_texture.handle;
#if !defined(_XBOX )
		TagRect.Create( Background, TS_NO_TEXTURE );
		TagRect.SetSize( ScreenWidth, ScreenHeight );
		TagRect.SetColor( 0, 0, 0, 255 );
		TagRect.SetPriority( 0x7000 );
		TagRect.Update();
#endif

		pTagFont[0] = Font::LoadFont( "WildStyle4" );
		pTagFont[1] = Font::LoadFont( "WildStyle2" );
		pTagFont[2] = Font::LoadFont( "WildStyle5" );

		ASSERT(!s_textureDB);
		s_textureDB = new TextureDatabase("tags");

		// flags
		int	Flags = 0;

#if defined (_XBOX)
		Flags = Image::RENDER_TARGET;
		pGreenTexture = 0;
#else
		pGreenTexture = TextureMgr::CreateSpecialTexture( 0, Flags, Image::SIZE_32_BIT|Image::TYPE_RGBA, tw, th, "GreenTag");
#endif

#ifdef PS2
		MasterList::SwapBufferCallback oldSwapBufferCB = MasterList::GetSwapBufferCallback();
		MasterList::SetSwapBufferCallback(NULL);
		MasterList::SetMinVBlanks(0);
		MasterList::DisplayVSynced(false);
#endif
		RenderMgr__FadeHide();

		// we have 16 unique tag frames for each player
		u32 lastNetUpdate = g_timer.GetOSTime();
		for ( uint8 i = 0 ; i < ref.GetMaxPlayers() ; i++ )
		{
			pColorTexture = TextureMgr::CreateSpecialTexture( 0, Flags, Image::SIZE_32_BIT|Image::TYPE_RGBA, tw, th, "Tag");

			if( pColorTexture )
			{
				CPlayerAgent* agent = ref.GetPlayerAgent(i);
				if (agent != NULL)
				{
					agent->GetTagParams(	TagParams );
				}
				else
				{
					strcpy( TagParams.Text, "ERROR" );

					TagParams.Color[0].R = 0;
					TagParams.Color[0].G = 0;
					TagParams.Color[0].B = 0;
					TagParams.Color[0].A = 255;

					TagParams.Color[1].R = 210;
					TagParams.Color[1].G = 40;
					TagParams.Color[1].B = 40;
					TagParams.Color[1].A = 255;

					TagParams.Color[2].R = 255;
					TagParams.Color[2].G = 255;
					TagParams.Color[2].B = 255;
					TagParams.Color[2].A = 255;
				}

				FontHeight = DEFAULT_TEXT_HEIGHT;
				BaseLine = 0.0f;
				Offset = 0.0f;
				ScaleX = 1.0f;
				ScaleY = 1.0f;
				Scale = 1.0f;

				for( n = 0; n < 3; n++ )
				{
					pFont = pTagFont[n];

					if( pFont )
					{
						FontScale = FontHeight / pFont->GetFontHeight();

						if( n == 0 )
							BaseLine = pFont->m_metricsf.ascent;

						Offset = (BaseLine - pFont->m_metricsf.ascent) * FontScale;
					}

					TagText[n].SetFont( pFont );
					TagText[n].SetText( TagParams.Text );
					TagText[n].SetBaseColor( TagParams.Color[n].R,
						TagParams.Color[n].G,
						TagParams.Color[n].B,
						TagParams.Color[n].A );
					TagText[n].SetTextHeight( FontHeight );
					TagText[n].SetOrigin( 0.5f, 0.5f );
					TagText[n].ScreenPosition( 0.5f, 0.5f );
					TagText[n].SetPriority( 0x7100 + n );
					TagText[n].SetTextSpacing( -6.0f );
					TagText[n].SetTextEffect( new SortTagEffect );

					if( n == 0 )
					{
						TextOutput to;
						TagText[n].ComputeTextOutput( &to );

						ScaleX = to.Width / ScreenWidth;
						ScaleY = to.Height / ScreenHeight;

						if( ScaleX > ScaleY )
							Scale = 1.0f / ScaleX;
						else
							Scale = 1.0f / ScaleY;
					}

					TagText[n].SetScale( Scale, Scale );
					TagText[n].SetOffset( 0.0f, Offset * Scale );
					TagText[n].Update();
				}

				Sprite::DrawAll();

				gfxStartRender();
				pColorTexture->Lock();
				pColorTexture->LockForRender();
#if defined(_XBOX)
				DirectX_Device->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA( 0, 0, 0, 0), 1.0f, 0L );
#endif
				// go!
				SpriteMgr__Render();

				pColorTexture->UnlockForRender();
				pColorTexture->Unlock();
				gfxEndRender();
#ifdef PS2
				MasterList::Flush();
#endif

				if( pGreenTexture )
				{
					for( int n = 0; n < 3; n++ )
					{
						TagText[n].SetBaseColor( 0xff8080ff );
						TagText[n].Update();
					}

					TagText[1].SetBaseColor( 0 );
					TagText[2].SetBaseColor( 0 );

					Sprite::DrawAll();


					gfxStartRender();
					pGreenTexture->Lock();
					pGreenTexture->LockForRender();

					// go!
					SpriteMgr__Render();
#ifdef PS2
					PlatformImage::ZapAlpha(0, 0, tw, th);
#endif

					pGreenTexture->UnlockForRender();
					pGreenTexture->Unlock();
					gfxEndRender();
#ifdef PS2
					MasterList::Flush();
#endif

					gfxStartRender();
					pColorTexture->Lock();
					pColorTexture->LockForRender();
					pGreenTexture->Lock();
					pGreenTexture->Select();
					pGreenTexture->BltGtoA( 0, 0, tw, th);
					//	PlatformImage::ZapAlpha(0, 0, tw, th);
					pGreenTexture->Unselect();
					pGreenTexture->Unlock();

					pColorTexture->UnlockForRender();
					pColorTexture->Unlock();
					gfxEndRender();
#ifdef PS2
					MasterList::Flush();
#endif
				}

				s_textureDB->AppendTexture( pColorTexture );

#if !defined(CDROM)
				//	if( i == 0 )
				//		t_Handle TagSprite = SpriteMgr__ShowDirectBindSprite( pColorTexture );
#endif
			}

			// allow engine some time!
			if (ValidSingleton(CConnection) && (g_timer.GetOSTime() - lastNetUpdate) > 25)
			{
				g_connection.Service();
				lastNetUpdate = g_timer.GetOSTime();

//				// keep updating the numbers
//				g_FrontEnd.Update();
//				Sprite::DrawAll();
//				RenderMgr__Render();
//				g_audio.Daemon();
			}
		}

#ifdef PS2
		MasterList::DisplayVSynced(true);
		MasterList::SetMinVBlanks(RenderMgr__GetNumVBlanksToSyncTo());
		MasterList::SetSwapBufferCallback(oldSwapBufferCB);
#endif
		RenderMgr__FadeShow();

		if( pGreenTexture )
			delete pGreenTexture;

		s_decalSet = new DecalSet();
		s_decalSet->Create(s_textureDB);

#else
//#if 0
		s_decalSet = new DecalSet("tag/tag.dbl");
#endif
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CustomTag::ReleaseReference()
{
	ASSERT(s_decalSetReferences > 0);
	if (s_decalSetReferences > 0)
		--s_decalSetReferences;

	if (s_decalSetReferences == 0)
	{
		delete s_decalSet;
		s_decalSet = NULL;

#if defined(PS2) || defined(_XBOX)
		delete s_textureDB;
		s_textureDB = NULL;
#endif
	}
}

