/******************* includes ****************************/

#include "hud/HudPCH.h"
#include "hud/font.h"
#include "hud/stringex.h"
#include "Game/Managers/TextureMgr/TextureMgr.h"

const Font::NameMap Font::s_map[] =
{
	{ "Stenci36", "GazzRegularB" },
	{ "GazzRegular", "GazzRegularB" },
	{ "impact3", "GazzRegularB" },
	{ "GazzRegularB", "CrackHouse" },
	{ NULL, NULL }
};

//=============================================================================
// Font::Font()
//=============================================================================

Font Font::m_FontList;

Font::Font( void )
{
	m_texture.handle = INVALID_HANDLE;
	m_permanent = false;

	memset( m_CustomChars, 0, sizeof(m_CustomChars) );
}

//=============================================================================
// Font::~Font()
//=============================================================================

Font::~Font( void )
{
	Release();
}

//=============================================================================
// Font::LoadSystemFont
//=============================================================================

Font* Font::LoadSystemFont( const char *pFileName )
{
	Font* pFont = LoadFont( pFileName, true );

	if( pFont )
		m_FontList.SetChild( pFont );

	return pFont;
}

//=============================================================================
// Font::FindFont()
//=============================================================================

Font *Font::FindFont( const char *pFileName )
{
	pFileName = MapFontName(pFileName);

	Font* pFont = (Font*) &m_FontList;

	while( pFont )
	{
		if( pFont->m_FontName.compare( pFileName ) )
			break;

		if( pFont->Child() )
			pFont = pFont->Child();
		else
			pFont = pFont->Next();
	}

	return pFont;
}

//=============================================================================
// Font::LoadFont()
//=============================================================================

Font *Font::LoadFont( const char *pFileName, bool permanent )
{
	Font *pFont = FindFont( pFileName );

	if( !pFont )
	{
		pFont = new Font;

		if( pFont )
		{
			if( pFont->Load( pFileName, permanent ) )
				m_FontList.AddChild( pFont );
			else
			{
				delete pFont;
				pFont = 0;
			}
		}
	}

	return pFont;
}

//=============================================================================
// Font::GetFirstFont
//=============================================================================

Font* Font::GetFirstFont( void )
{
	return (Font*) m_FontList.Child();
}

//=============================================================================
// Font::FlushAllFonts
//=============================================================================

void Font::FlushAllFonts( void )
{
	Font *child = m_FontList.Child();
	while( child )
	{
		Font *next = child->Next();
		if (!child->m_permanent)
			delete child;
		child = next;
	}

	return;
}

//=============================================================================
// Font::Load()
//=============================================================================

bool Font::Load( const char *pFontName, bool permanent )
{
	Release();

	pFontName = MapFontName(pFontName);

	FileStream Stream;
	if( !Stream.Open( Printf( "fonts\\%s.fnt", pFontName ), FileStream::READ ) )
		return false;

	Stream.Read( &m_metrics, sizeof(m_metrics) );
	Stream.Close();

   EndianSwap();

	m_FontName.set( pFontName );

	m_texture.handle = dblLoad( Printf( "fonts\\%s.dbl", pFontName ) );

	if( m_texture.handle == INVALID_HANDLE )
		m_texture.handle = dblLoad( Printf( "%s.dbl", pFontName ) );

	bool Result = false;

	if( m_texture.handle != INVALID_HANDLE )
	{
		// Make it permanent
		dblSetPermanent(m_texture.handle, permanent);
		m_permanent = permanent;

		TextureDatabase *pTextureDatabase = (TextureDatabase *) dblGetChunkData( DB_TEXTURE_SET, DEFAULT_CHUNK_KEY_STRING, m_texture.handle );

		if (pTextureDatabase == NULL)
			pTextureDatabase = (TextureDatabase *) dblGetChunkData( DB_MERGED_TEXTURE_SET, DEFAULT_CHUNK_KEY_STRING, m_texture.handle );

		if( pTextureDatabase )
		{
			// Hack!!   This assumes that the font database contains only
			// 1 texture.
			ASSERT_PTR(pTextureDatabase);
			ASSERTS(pTextureDatabase->TextureCount() == 1, "Font texture database has more than one texture in it");

			// get the first texture
			Texture* tex = pTextureDatabase->GetTexture(0);
			ASSERT_PTR(tex);

			// get that textures image.
			Image* img = tex->GetImage();
			ASSERT_PTR(img);

			// get the values we're looking for
			m_texture.width  = (float) img->GetWidth();
			m_texture.height = (float) img->GetHeight();

			// sanity check
			ASSERT(m_texture.width > 0 && m_texture.width <= 1024);
			ASSERT(m_texture.height > 0 && m_texture.height <= 1024);

			Result = true;
		}
	}

	// The numbers are converted to fixed width so that
	// the text doesn't jitter as the numbers change.

	int diff;

	for( int x = '0'; x <= '9'; x++ )
	{
		diff = (m_metrics.table['0'].cw - m_metrics.table[x].cw) >> 1;

		m_metrics.table[x].cx += diff;
		m_metrics.table[x].cs += diff;
	}

	// The metrics stored in the data file are formatted as either integer or
	// 16.16 fixed point number.  We convert them to floating point now so it
	// only needs to be done once.

	for( int i = 0; i < 256; i++ )
	{
		m_metricsf.table[i].x1 = (float) m_metrics.table[i].x1 / m_texture.width;
		m_metricsf.table[i].x2 = (float) m_metrics.table[i].x2 / m_texture.width;
		m_metricsf.table[i].y1 = (float) m_metrics.table[i].y1 / m_texture.height;
		m_metricsf.table[i].y2 = (float) m_metrics.table[i].y2 / m_texture.height;

		m_metricsf.table[i].cx = (float) m_metrics.table[i].cx / 65536.0f;
		m_metricsf.table[i].cy = (float) m_metrics.table[i].cy / 65536.0f;
		m_metricsf.table[i].cw = (float) m_metrics.table[i].cw / 65536.0f;
		m_metricsf.table[i].ch = (float) m_metrics.table[i].ch / 65536.0f;
		m_metricsf.table[i].cs = (float) m_metrics.table[i].cs / 65536.0f;
	}

	m_metricsf.height  = (float) m_metrics.height  / 65536.0f;
	m_metricsf.ascent  = (float) m_metrics.ascent  / 65536.0f;
	m_metricsf.descent = (float) m_metrics.descent / 65536.0f;
	m_metricsf.leadin  = (float) m_metrics.leadin  / 65536.0f;
	m_metricsf.leadout = (float) m_metrics.leadout / 65536.0f;

	m_texture.texture = 0;

	return Result;
}

//=============================================================================
// Font::Release()
//=============================================================================

void Font::Release( void )
{
	ASSERT(!m_permanent);

	if(m_texture.handle != INVALID_HANDLE)
	{
		dblRemove(m_texture.handle);
		m_texture.handle = INVALID_HANDLE;
	}

	m_FontName.release();

	int n = sizeof(m_CustomChars)/sizeof(m_CustomChars[0]);
	int i;

	for( i = 0; i < n; i++ )
	{
		delete m_CustomChars[i];
		m_CustomChars[i] = 0;
	}

	return;
}

//=============================================================================
// Font::Name() - Returns the name of the font.
//=============================================================================

const char* Font::Name( void )
{
	return m_FontName.get();
}

//=============================================================================
// Font::GetTextLength() - Returns the width, in pixels, of the string.
//=============================================================================

float Font::GetTextLength( const char *text )
{
	float Size = 0.0f;
	float MaxSize = 0.0f;

	float A;
	float B;
	float C;

	int i;
	char ch;

	if( text )
	{
		for( i = 0; text[i]; i++ )
		{
			ch = text[i&255];

			if( ch == '\n' )
			{
				if( MaxSize < Size )
					MaxSize = Size;

				Size = 0.0f;
			}
			else
			{
				A = m_metricsf.table[ch].cx;
				B = m_metricsf.table[ch].cw;
				C = m_metricsf.table[ch].cs;

				Size += A+B+C;
			}
		}

		if( MaxSize < Size )
			MaxSize = Size;
	}

	return MaxSize;
}

//=============================================================================
// Font::GetTextHeight() - Returns the height, in pixels, of the string.
//=============================================================================

float Font::GetTextHeight( const char *text )
{
	float Height = 0.0f;

	if( text )
	{
		for( int i = 0; text[i]; i++ )
		{
			if( text[i] == '\n' )
				Height += GetFontHeight() + GetExternalLeading();
		}

		Height += GetFontHeight();
	}

	return Height;
}

//=============================================================================
// Font::GetCharWidth() - Returns the physical width of a character
//=============================================================================

float Font::GetCharWidth( char ch )
{
	return m_metricsf.table[ch].cw;
}

//=============================================================================
// Font::GetCharHeight() - Returns the height of a character, including space
//									above and below (may be smaller or taller than the
//									actual character).
//=============================================================================

float Font::GetCharHeight( char ch )
{
	return m_metricsf.table[ch].ch;
}

//=============================================================================
// Font::GetFontHeight()
//=============================================================================

float Font::GetFontHeight( void )
{
	return m_metricsf.height;
}

//=============================================================================
// Font::GetAscent()
//=============================================================================

float Font::GetAscent( void )
{
	return m_metricsf.ascent;
}

//=============================================================================
// Font::GetDescent()
//=============================================================================

float Font::GetDescent( void )
{
	return m_metricsf.descent;
}

//=============================================================================
// Font::GetInternalLeading()
//=============================================================================

float Font::GetInternalLeading( void )
{
	return m_metricsf.leadin;
}

//=============================================================================
// Font::GetExternalLeading()
//=============================================================================

float Font::GetExternalLeading( void )
{
	return m_metricsf.leadout;
}

//=============================================================================
// Font::SetTextureRect
//
//		Specify the texture coordinates for a character.
//=============================================================================

void Font::SetTextureRect( int ch, int u1, int v1, int u2, int v2 )
{
	float x1 = u1 * m_texture.width;
	float y1 = v1 * m_texture.height;
	float x2 = u2 * m_texture.width;
	float y2 = v2 * m_texture.height;

	SetTextureRect( ch, x1, y1, x2, y2 );
}

//=============================================================================
// Font::GetTextureRect
//
//		Returns the texture coordinates (in pixels) for the specified character.
//=============================================================================

void Font::GetTextureRect( int ch, int &u1, int &v1, int &u2, int &v2 )
{
	float x1, y1;
	float x2, y2;

	GetTextureRect( ch, x1, y1, x2, y2 );

	u1 = int(m_texture.width * x1);
	v1 = int(m_texture.height * y1);
	u2 = int(m_texture.width * x2);
	v2 = int(m_texture.height * y2);
}


//=============================================================================
// Font::SetTextureRect
//
//		Specify the texture coordinates for a character.
//=============================================================================

void Font::SetTextureRect( int ch, float u1, float v1, float u2, float v2 )
{
	m_metricsf.table[ch&255].x1 = u1;
	m_metricsf.table[ch&255].y1 = v1;
	m_metricsf.table[ch&255].x2 = u2;
	m_metricsf.table[ch&255].y2 = v2;
}

//=============================================================================
// Font::GetTextureRect
//
//		Returns the texture coordinates for the specified character.
//=============================================================================

void Font::GetTextureRect( int ch, float &u1, float &v1, float &u2, float &v2 )
{
	u1 = m_metricsf.table[ch&255].x1;
	v1 = m_metricsf.table[ch&255].y1;
	u2 = m_metricsf.table[ch&255].x2;
	v2 = m_metricsf.table[ch&255].y2;
}

//=============================================================================
// Font::SetCustomChar
//=============================================================================

void Font::SetCustomChar( int ch, t_Handle textureset, int texture, int u1, int v1, int u2, int v2 )
{
	ch &= 255;

	CustomChar* pCustomChar = m_CustomChars[ch];

	if( !pCustomChar )
		pCustomChar = new CustomChar;

	if( pCustomChar )
	{
		pCustomChar->textureset = textureset;
		pCustomChar->texture = texture;
		pCustomChar->width = u2 - u1;
		pCustomChar->height = v2 - v1;
		pCustomChar->x1 = u1;
		pCustomChar->y1 = v1;
		pCustomChar->x2 = u2;
		pCustomChar->y2 = v2;

		float tw = 0.0f;
		float th = 0.0f;

		TextureDatabase *pTextureDatabase = (TextureDatabase *) dblGetChunkData( DB_TEXTURE_SET, DEFAULT_CHUNK_KEY_STRING, textureset );

		if( !pTextureDatabase )
			pTextureDatabase = (TextureDatabase *) dblGetChunkData( DB_MERGED_TEXTURE_SET, DEFAULT_CHUNK_KEY_STRING, textureset );

		if( pTextureDatabase )
		{
			Texture* tex = pTextureDatabase->GetTexture( texture );

			if( tex )
			{
				Image* img = tex->GetImage();

				if( img )
				{
					tw = float(img->GetWidth());
					th = float(img->GetHeight());
				}
			}
		}

		float w = float(pCustomChar->width);
		float h = float(pCustomChar->height);
		float t = w / h;

		pCustomChar->widthf  = w;
		pCustomChar->heightf = h;
		pCustomChar->u1 = u1 / tw;
		pCustomChar->v1 = v1 / th;
		pCustomChar->u2 = u2 / tw;
		pCustomChar->v2 = v2 / th;

		pCustomChar->cx = 0.0f;
		pCustomChar->cy = 0.0f;
		pCustomChar->cw = m_metricsf.height * t;
		pCustomChar->ch = m_metricsf.height;
		pCustomChar->cs = 0.0f;
	}

	m_CustomChars[ch] = pCustomChar;
}

Font::CustomChar* Font::GetCustomChar( int ch )
{
	return m_CustomChars[ch&255];
}

void Font::EndianSwap( void)
{
#ifdef GCN
	int i;

	for( i = 0; i < 256; i++)
	{
		ByteSwap( table[i].x1);
		ByteSwap( table[i].y1);
		ByteSwap( table[i].x2);
		ByteSwap( table[i].y2);
		ByteSwap( table[i].cx);
		ByteSwap( table[i].cy);
		ByteSwap( table[i].cw);
		ByteSwap( table[i].ch);
		ByteSwap( table[i].cs);
	}

	ByteSwap( height);
	ByteSwap( ascent);
	ByteSwap( descent);
	ByteSwap( leadin);
	ByteSwap( leadout);
#endif
}

const char *Font::MapFontName(
const char *fontName)
{
	// this hack removes the "data" before font name
	if( 0 == strnicmp( fontName, "data", 4 ) )
		fontName = &fontName[4];

	int index = 0;
	while (s_map[index].request != NULL)
	{
		if (strcmpi(fontName, s_map[index].request) == 0)
			return s_map[index].use;
		++index;
	}

	return fontName;
}
