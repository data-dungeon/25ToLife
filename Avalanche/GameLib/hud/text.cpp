/******************* includes ****************************/

#include "hud/HudPCH.h"
#include "hud/text.h"

#define TRANSLATE_TEXT

#ifdef TRANSLATE_TEXT
#include "GameHelper/Translator.h"
#endif

#define TEXT_LOADSAVE_VERSION1		(0x0100)
#define TEXT_LOADSAVE_VERSION2		(0x0101)
#define TEXT_LOADSAVE_VERSION			(TEXT_LOADSAVE_VERSION2)

//=============================================================================
// Text::Text()
//=============================================================================

Text::Text()
{
	Reset();

	m_Glyphs.pGlyphs = 0;
	m_Glyphs.Count = 0;

	m_Shadow.pHandles = 0;
	m_Shadow.Count = 0;

	m_ComputeTextShortcut = false;
	m_ComputeTextShortcutX = 0.0f;
	m_ComputeTextShortcutY = 0.0f;

	m_pTextEffects = 0;

	return;
}

//=============================================================================
// Text::~Text()
//=============================================================================

Text::~Text()
{
	RemoveFromDrawList();
	Release();
}

//=============================================================================
// Text::Reset()
//=============================================================================

void Text::Reset( void )
{
	Sprite::Reset();

	// remove the text
	ClearText();

	// default font
	SetFont( Font::GetFirstFont() );

	// default shadow intensity
	ShadowIntensity( 128 );

	// default text formatting
	SetTextHeight( 0.0f );
	SetTextSpacing( 0.0f );
	SetTextFixedSpacing( 0.0f );
	SetLineSpacing( 0.0f );
	SetTextAlignment( 0.0f );
	SetWordWrap( false );
	SetTranslator( true );
	Tesselate(1);

	// No mutators
	EffectsOff();
}

//=============================================================================
// Text::SetFont()
//=============================================================================

void Text::SetFont( Font *pFont, float TextHeight, float Spacing )
{
	m_pFont = pFont;

	if( !m_pFont )
		m_pFont = Font::GetFirstFont(); // fail safe

	SetTextHeight( TextHeight );
	SetTextSpacing( Spacing );
}

//=============================================================================
// Text::SetFont2()
//=============================================================================

void Text::SetFont2( Font *pFont )
{
	if( m_pFont != pFont )
	{
		m_pFont = pFont;

		if( !m_pFont )
			m_pFont = Font::GetFirstFont(); // fail safe

		ReleaseHandles();
	}

	return;
}

//=============================================================================
// Text::SetFont2()
//=============================================================================

void Text::SetFont( char *pFontName )
{
	SetFont2( Font::LoadFont( pFontName ) );
}

//=============================================================================
// Text::SetText()
//=============================================================================

void Text::SetText( const char *pText )
{
	if( !GetTranslator() )
		SetTextNoTranslate( pText );
	else
	{
	#ifdef TRANSLATE_TEXT
		m_Text.set( g_translator.Translate( pText ) );
	#else
		m_Text.set( pText );
	#endif
	}

	return;
}

//=============================================================================
// Text::SetTextNoTranslate()
//=============================================================================

void Text::SetTextNoTranslate( const char *pText )
{
	m_Text.set( pText );
}

//=============================================================================
// Text::Printf
//=============================================================================

void Text::Printf( const char *fmt, ... )
{
	static int i = 0;
	static int len;
	static char szPrintf[1024];

	va_list args;
	va_start( args, fmt );
#ifdef TRANSLATE_TEXT
	len = vsprintf( szPrintf, g_translator.Translate( fmt ), args );
#else
	len = vsprintf( szPrintf, fmt, args );
#endif
	va_end( args );
	ASSERTF( len + 1 < sizeof(szPrintf), ( "Text::Printf buffer overflow (len=%d,max=%d)", len, sizeof(szPrintf) ) );

	m_Text.set( szPrintf );
}

//=============================================================================
// Text::ClearText()
//=============================================================================

void Text::ClearText( void )
{
	m_Text.set( "" );
}

//=============================================================================
// Text::Release()
//=============================================================================

void Text::Release( void )
{
	Sprite::Release();
	m_Text.release();
	ReleaseHandles();

	if( m_pTextEffects )
	{
		while( m_pTextEffects->Next() )
			delete m_pTextEffects->Next();

		delete m_pTextEffects;

		m_pTextEffects = 0;
	}

	return;
}

//=============================================================================
// Text::ComputeTransform
//
//		We need to understand exactly what is going on here and why this is
//		necessary.  Whenever the width or the height of a sprite is zero, then
//		its size is calculated during the ComputeSprite or ComputeText phase.
//		When the transform is calculated, its origin is factored in.  The origin
//		cannot be computed until with size of the sprite is known.  A vanilla
//		sprite calculates its size quickly by taking the textures native
//		dimensions.  A text object, however, is far more complicated because the
//		entire tring must be computed before the size is known.
//
//		The optimization here happens when ComputeText requests the transform.
//		Since ComputeText has already figured out the dimensions of this string,
//		it stores them in private members before calling this function.
//=============================================================================

void Text::ComputeTransform( Graphics4x4 &transform )
{
	if( FLOATTOINT(m_state.size.x) == 0 || FLOATTOINT(m_state.size.y) == 0 )
	{
		float width = 0.0f;
		float height = 0.0f;

		if( m_ComputeTextShortcut )
		{
			// This is done as a speed optimization...
			width  = m_ComputeTextShortcutX;
			height = m_ComputeTextShortcutY;
		}
		else
		{
			// This is actually the right way
			TextOutput to;
			ComputeTextOutput( &to );
 
			width  = to.Width;
			height = to.Height;
		}

		// we're evil...

		float oldx = m_state.size.x;
		float oldy = m_state.size.y;
		m_state.size.x = width;
		m_state.size.y = height;

		Sprite::ComputeTransform( transform );

		// very very evil...
		m_state.size.x = oldx;
		m_state.size.y = oldy;
	}
	else
	{
		Sprite::ComputeTransform( transform );
	}

	return;
}

//=============================================================================
// Text::ComputeSprite
//=============================================================================

void Text::ComputeSprite( void )
{
	Sprite::ComputeSprite();

	ComputeText( true, 0 );
}

//=============================================================================
// Text::ComputeText
//=============================================================================

void Text::ComputeText( bool Draw, TextOutput *to )
{
	if( !Draw && !to )
		return;

	if( to )
	{
		if( m_EffectsOn && m_pTextEffects )
			Draw = true;

		to->x1     = 0.0f;
		to->y1     = 0.0f;
		to->x2     = 0.0f;
		to->y2     = 0.0f;
		to->Width  = 0.0f;
		to->Height = m_Appearance.TextHeight;	
	}

	if( m_pFont && m_pFont->m_texture.handle != INVALID_HANDLE )
	{
		Sprite *parent = this;

		struct {
			float a;
			float r;
			float g;
			float b;
		} color;

		struct {
			float a;
			float r;
			float g;
			float b;
		} familycolor;

		//=======================================================================
		// To calculate the final color, start with the sprite's own base color.
		//=======================================================================

		color.a = (1.0f/255.0f) * m_state.color.a;
		color.r = (1.0f/255.0f) * m_state.color.r;
		color.g = (1.0f/255.0f) * m_state.color.g;
		color.b = (1.0f/255.0f) * m_state.color.b;

		//=======================================================================
		// Then multiply by its parent's family color.
		//=======================================================================

		while( parent )
		{
			familycolor.a = (1.0f/255.0f) * parent->m_state.familycolor.a;
			familycolor.r = (1.0f/255.0f) * parent->m_state.familycolor.r;
			familycolor.g = (1.0f/255.0f) * parent->m_state.familycolor.g;
			familycolor.b = (1.0f/255.0f) * parent->m_state.familycolor.b;

			color.a = color.a * familycolor.a;
			color.r = color.r * familycolor.r;
			color.g = color.g * familycolor.g;
			color.b = color.b * familycolor.b;

			parent = parent->Parent();
		}

		//=======================================================================
		// If the sprite is not visible, or any parent in the chain had a family
		// alpha color of zero, then the sprite cannot be seen and will not
		// rendered.
		//=======================================================================

		if( (!m_state.sprite.visible || color.a == 0.0f) && !to )
			ReleaseHandles();
		else
		{
			//====================================================================
			// Quite a few variables are used during the main loop.
			//====================================================================

			Glyph *p;
			Font::CustomChar* cc;
			Graphics4x4 transform;		
			Vector3 vectors[4];
			Vector2 texuvs[4];
			ts_bRGBA colors[4];
			t_Handle textureset;
			int texture;
			float left;
			float right;
			float top;
			float bottom;
			float height;
			float screenx;
			float screeny;
			float spacing;
			
			float y0;
			float x1, y1;
			float x2, y2;
			float u1 = 0.0f, v1 = 0.0f;
			float u2 = 0.0f, v2 = 0.0f;
			float cx = 0.0f;
			float cy = 0.0f;
			float cw = 0.0f;
			float cs = 0.0f;
			float th;
			float fh;
			float oh;
			float el;
			float ts;
			float w, h;
			float s;
			float ls;
			float hs;
			float gl, gr;
			float gt, gb;
			int i, c, n, t;
			int length;
			int glyphs;
			int sol;
			int wrappos;
			u32 flags;
			u16 basepriority;
			u16 priority = 0;
			u8 blendmode;
			bool wrap;
			bool effects;
			unsigned char ch;
			const char* string;

			string = m_Text.get();
			length = m_Text.length();
			glyphs = length * m_Tesselation * m_Tesselation;

			//====================================================================
			// Reserve some sprites.
			//====================================================================

			ReserveGlyphs( glyphs );

			if( !m_Glyphs.pGlyphs )
				return;

			p = m_Glyphs.pGlyphs;

			//====================================================================
			// At the start, we indicate that word wrapping has not occured.
			//====================================================================

			wrap = false;
			wrappos = 0;

			//====================================================================
			// Begin at the top-left position.
			//====================================================================

			y0 = 0.0f;
			x1 = 0.0f;
			y1 = 0.0f;

			fh = m_pFont->GetFontHeight();
			el = m_pFont->GetExternalLeading();

			th = m_Appearance.TextHeight;
			ts = m_Appearance.LineSpacing;

			if( th == 0.0f ) th = DEFAULT_TEXT_HEIGHT;

			s = th / fh;

			spacing = m_Appearance.TextSpacing * th * (1.0f/DEFAULT_TEXT_HEIGHT);

			//====================================================================
			// When a normalized floating point number is multiplied by the
			// following constant, and then adding one (that gets done later), the
			// lowest 8 bits of the result happen to correspond exactly to the
			// 8-bit integer we'd get if we multipled it by 255 and cast it to an
			// integer.
			//
			// This is good because it allows us to convert the floating point
			// RGBA values to unsigned 8-bit integer values (which the engine
			// requires) much faster than if I simply casted those floats to
			// 8-bit integers.  In fact, this optimization produced a whopping 40%
			// speed up in the text driver.
			//====================================================================

			#ifdef PS2
				color.a *= 128.0f / (256.0f*256.0f*128.0f) / 255.0f;
				color.r *= 128.0f / (256.0f*256.0f*128.0f) / 255.0f;
				color.g *= 128.0f / (256.0f*256.0f*128.0f) / 255.0f;
				color.b *= 128.0f / (256.0f*256.0f*128.0f) / 255.0f;
			#else
				color.a *= 255.0f / (256.0f*256.0f*128.0f) / 255.0f;
				color.r *= 255.0f / (256.0f*256.0f*128.0f) / 255.0f;
				color.g *= 255.0f / (256.0f*256.0f*128.0f) / 255.0f;
				color.b *= 255.0f / (256.0f*256.0f*128.0f) / 255.0f;
			#endif

			//====================================================================
			// Are text effects enabled?
			//====================================================================

			effects = false;

			if( m_EffectsOn && m_pTextEffects )
				effects = true;

			//====================================================================
			// Almost ready...
			//====================================================================

			oh = 0.0f;
			sol = 1;

			left   = 0.0f;
			top    = 0.0f;
			right  = m_state.size.x;
			bottom = m_state.size.y;

			//====================================================================
			// Check for fixed spacing...
			//====================================================================

			if( !Math::Zero(m_Appearance.TextFixedSpacing) )
			{
				cx = 0.0f;
				cs = 0.0f;
				cw = m_Appearance.TextFixedSpacing;
			}

			//====================================================================
			// Loop through each character, generating glyphs (sprites) as we go.
			//====================================================================

			for( i = 0, t = 0; i < length; i++ )
			{
				ch = string[i];

				cc = m_pFont->GetCustomChar(ch);

				//=================================================================
				// Look up the uv positions
				//=================================================================

				if( Draw )
				{
					if( cc )
					{
						u1 = cc->u1; // get uv position from custom char
						v1 = cc->v1;
						u2 = cc->u2;
						v2 = cc->v2;
					}
					else
					{
						u1 = m_pFont->m_metricsf.table[ch].x1;
						v1 = m_pFont->m_metricsf.table[ch].y1;
						u2 = m_pFont->m_metricsf.table[ch].x2;
						v2 = m_pFont->m_metricsf.table[ch].y2;
					}
				}

				//=================================================================
				// Look up the offsets
				//=================================================================

				if( !Math::Zero(m_Appearance.TextFixedSpacing) )
				{
					cy = m_pFont->m_metricsf.table[ch].cy;
				}
				else
				if( cc )
				{
					cx = cc->cx;
					cy = cc->cy;
					cw = cc->cw;
					cs = cc->cs;
				}
				else
				{
					cx = m_pFont->m_metricsf.table[ch].cx;
					cy = m_pFont->m_metricsf.table[ch].cy;
					cw = m_pFont->m_metricsf.table[ch].cw;
					cs = m_pFont->m_metricsf.table[ch].cs;
				}

				//=================================================================
				// Compute the size, in pixels
				//=================================================================

				if( cc )
				{
					w = cc->cw;
					h = cc->ch;
				}
				else
				{
					w = (float) (m_pFont->m_metrics.table[ch].x2 - m_pFont->m_metrics.table[ch].x1);
					h = (float) (m_pFont->m_metrics.table[ch].y2 - m_pFont->m_metrics.table[ch].y1);
				}

				//=================================================================
				// Finally, compute the pre-transformed screen coordinates
				//=================================================================

				gl = x1;
				gr = gl + cw * s;
				gt = y0 * s;
				gb = y1 + h * s;

				x1 = x1 + cx * s;
				x2 = x1 + w * s;

				y1 = (y0 + cy) * s;
				y2 = y1 + h * s;

				//=================================================================
				// Check for word wrap
				//=================================================================

				if( GetWordWrap() && x2 > m_state.size.x && ch > 32 )
				{
					//=============================================================
					// When word wrap occures, walk backward until the beginning of
					// a word is reached.  In very special circumstances, large
					// words may have to be split.
					//=============================================================

					for( n = i; n > wrappos; n-- )
					{
						if( (unsigned char) string[n] < 33 )
						{
							i = n;
							wrappos = n;
							wrap = true;
						}
					}

					if( !wrap && i > wrappos )
					{
						i--; wrap = true;
					}
				}

				if( !wrap )
				{
					if( Draw )
					{
						//==============================================================
						// Here, the pre-transformed, pre-mutated vertex coordinates are
						// stuffed into the layout registers.  They will be transformed
						// (and mutated, if applicable) shortly.
						//==============================================================

						if( m_Tesselation	== 1 )
						{
							p[i].vertex[0].x = x1;
							p[i].vertex[0].y = y1;
							p[i].vertex[0].u = u1;
							p[i].vertex[0].v = v1;
							p[i].vertex[0].a = m_state.vertexcolor[0].a;
							p[i].vertex[0].r = m_state.vertexcolor[0].r;
							p[i].vertex[0].g = m_state.vertexcolor[0].g;
							p[i].vertex[0].b = m_state.vertexcolor[0].b;

							p[i].vertex[1].x = x2;
							p[i].vertex[1].y = y1;
							p[i].vertex[1].u = u2;
							p[i].vertex[1].v = v1;
							p[i].vertex[1].a = m_state.vertexcolor[1].a;
							p[i].vertex[1].r = m_state.vertexcolor[1].r;
							p[i].vertex[1].g = m_state.vertexcolor[1].g;
							p[i].vertex[1].b = m_state.vertexcolor[1].b;

							p[i].vertex[2].x = x2;
							p[i].vertex[2].y = y2;
							p[i].vertex[2].u = u2;
							p[i].vertex[2].v = v2;
							p[i].vertex[2].a = m_state.vertexcolor[2].a;
							p[i].vertex[2].r = m_state.vertexcolor[2].r;
							p[i].vertex[2].g = m_state.vertexcolor[2].g;
							p[i].vertex[2].b = m_state.vertexcolor[2].b;

							p[i].vertex[3].x = x1;
							p[i].vertex[3].y = y2;
							p[i].vertex[3].u = u1;
							p[i].vertex[3].v = v2;
							p[i].vertex[3].a = m_state.vertexcolor[3].a;
							p[i].vertex[3].r = m_state.vertexcolor[3].r;
							p[i].vertex[3].g = m_state.vertexcolor[3].g;
							p[i].vertex[3].b = m_state.vertexcolor[3].b;

							p[i].x1 = gl;
							p[i].y1 = gt;
							p[i].x2 = gr;
							p[i].y2 = gb;
							p[i].i = i;
							p[i].ch = ch;
							p[i].sol = sol;
							p[i].priority = 0;

							sol = 0;
						}
						else
						{
							float tes = 1.0f / m_Tesselation;
							float dx = x2 - x1;
							float dy = y2 - y1;
							float du = u2 - u1;
							float dv = v2 - v1;
							float tx1 = 0.0f;
							float ty1 = 0.0f;
							float tx2 = tes;
							float ty2 = tes;
							int x, y;

							t = i * m_Tesselation * m_Tesselation;

							for( y = 0; y < m_Tesselation; y++ )
							{
								tx1 = 0.0f, tx2 = tes;
								
								for( x = 0; x < m_Tesselation; x++ )
								{
									p[t].vertex[0].x = x1 + dx * tx1;
									p[t].vertex[0].y = y1 + dy * ty1;
									p[t].vertex[0].u = u1 + du * tx1;
									p[t].vertex[0].v = v1 + dv * ty1;

									p[t].vertex[0].a = m_state.vertexcolor[0].a;
									p[t].vertex[0].r = m_state.vertexcolor[0].r;
									p[t].vertex[0].g = m_state.vertexcolor[0].g;
									p[t].vertex[0].b = m_state.vertexcolor[0].b;

									p[t].vertex[1].x = x1 + dx * tx2;
									p[t].vertex[1].y = y1 + dy * ty1;
									p[t].vertex[1].u = u1 + du * tx2;
									p[t].vertex[1].v = v1 + dv * ty1;

									p[t].vertex[1].a = m_state.vertexcolor[1].a;
									p[t].vertex[1].r = m_state.vertexcolor[1].r;
									p[t].vertex[1].g = m_state.vertexcolor[1].g;
									p[t].vertex[1].b = m_state.vertexcolor[1].b;

									p[t].vertex[2].x = x1 + dx * tx2;
									p[t].vertex[2].y = y1 + dy * ty2;
									p[t].vertex[2].u = u1 + du * tx2;
									p[t].vertex[2].v = v1 + dv * ty2;

									p[t].vertex[2].a = m_state.vertexcolor[2].a;
									p[t].vertex[2].r = m_state.vertexcolor[2].r;
									p[t].vertex[2].g = m_state.vertexcolor[2].g;
									p[t].vertex[2].b = m_state.vertexcolor[2].b;

									p[t].vertex[3].x = x1 + dx * tx1;
									p[t].vertex[3].y = y1 + dy * ty2;
									p[t].vertex[3].u = u1 + du * tx1;
									p[t].vertex[3].v = v1 + dv * ty2;

									p[t].vertex[3].a = m_state.vertexcolor[3].a;
									p[t].vertex[3].r = m_state.vertexcolor[3].r;
									p[t].vertex[3].g = m_state.vertexcolor[3].g;
									p[t].vertex[3].b = m_state.vertexcolor[3].b;

									p[t].x1 = gl;
									p[t].y1 = gt;
									p[t].x2 = gr;
									p[t].y2 = gb;
									p[t].i = i;
									p[t].ch = ch;
									p[t].sol = sol;

									sol = 0;

									t++, tx1 += tes, tx2 += tes;
								}
								ty1 += tes, ty2 += tes;
							}
						}
					}

					oh = th;
					x1 = x1 + s * (cw + cs);

					if( FLOATTOINT(right) < FLOATTOINT(x1) )
						right = x1;

					x1 = x1 + spacing;
				}

				//=================================================================
				// Here, we process a line break.  This can happen when a word wrap
				// occures or an end-of-line character is processed.
				//=================================================================

				if( wrap || ch == '\n' )
				{
					oh = 0.0f;
					x1 = 0.0f;
					y0 = y0 + fh + el + ts;
					sol = 1;

					// word wrap
					wrap = false;

					if( wrappos == i )
					{
						//===========================================================
						// This is a special case where a large word has to be split
						// because it will not fit on a line by itself.  Although
						// perfectly leagal, it should be avoided.
						//===========================================================
						wrappos++;	
					}
					else
						wrappos = i;
				}
			}

			height = y0 * s + oh;

			if( bottom < height )
				bottom = height;

			//====================================================================
			// See if the bounding box has exceeded a pre-defined limit, and scale
			// text as necessary.
			//====================================================================

			ls = 1.0f; hs = 1.0f;

			if( FLOATTOINT(m_state.size.x) != 0 )
				ls = m_state.size.x / right;

			if( FLOATTOINT(m_state.size.y) != 0 )
				hs = m_state.size.y / bottom;

			//====================================================================
			// Adjust the alignment of each line
			//====================================================================

			if( FLOATTOINT(m_Appearance.TextAlignment) != 0 && Draw )
			{
				float lx, rx;
				float cx, dx;
				
				for( i = 0; i < glyphs; i = c )
				{
					lx = p[i].x1;
					rx = p[i].x2;

					for( c = i + 1; c < glyphs && !p[c].sol; c++ )
						rx = p[c].x2;
			
					cx = rx - lx;

					dx = (right - cx) * m_Appearance.TextAlignment;

					for( n = i; n < c; n++ )
					{
						p[n].vertex[0].x += dx;
						p[n].vertex[1].x += dx;
						p[n].vertex[2].x += dx;
						p[n].vertex[3].x += dx;
					}
				}
			}

			//====================================================================
			// Here we've got to scale the vertices to make sure it fits the
			// specified rectangle.
			//====================================================================

			if( FLOATTOINT(ls) != 0x3f80000 || FLOATTOINT(hs) != 0x3f800000)
			{
				if( Draw )
				{
					for( i = 0; i < glyphs; i++ )
					{
						p[i].vertex[0].x *= ls;
						p[i].vertex[0].y *= hs;
						p[i].vertex[1].x *= ls;
						p[i].vertex[1].y *= hs;
						p[i].vertex[2].x *= ls;
						p[i].vertex[2].y *= hs;
						p[i].vertex[3].x *= ls;
						p[i].vertex[3].y *= hs;
					}
				}

				left   *= ls;
				right  *= ls;
				top    *= hs;
				bottom *= hs;
			}

			if( to )
			{
				to->x1     = left;
				to->x2     = right;
				to->y1     = top;
				to->y2     = bottom;
				to->Width  = to->x2 - to->x1;
				to->Height = to->y2 - to->y1;
			}

			if( !Draw )
				return;

			if( m_EffectsOn && m_pTextEffects )
			{
				TextEffect* pTextEffect = m_pTextEffects;

				while( pTextEffect )
				{
					pTextEffect->Effect( glyphs, p, left, right, top, bottom );

					/*
					if( to || pTextEffect->Next() )
					{
						float rect_t, rect_b;
						float rect_l, rect_r;
						float x, y;
						int i, n;

						float frame_t;
						float frame_b;
						float frame_l;
						float frame_r;

						for( i = 0; i < glyphs; i++ )
						{
							rect_t = p[i].vertex[0].y;
							rect_l = p[i].vertex[0].x;
							rect_b = rect_t;
							rect_r = rect_l;

							for( n = 1; n < 4; n++ )
							{
								x = p[i].vertex[n].x;
								y = p[i].vertex[n].y;

								if( rect_t > y ) rect_t = y;
								if( rect_b < y ) rect_b = y;
								if( rect_l > x ) rect_l = x;
								if( rect_r < x ) rect_r = x;
							}

							p[i].x1 = rect_l;
							p[i].y1 = rect_t;
							p[i].x2 = rect_r;
							p[i].y2 = rect_b;
						}

						frame_t = p[0].y1;
						frame_b = p[0].y2;
						frame_l = p[0].x1;
						frame_r = p[0].x2;

						for( i = 1; i < glyphs; i++ )
						{
							if( frame_t > p[i].y1 ) frame_t = p[i].y1;
							if( frame_b < p[i].y2 ) frame_b = p[i].y2;
							if( frame_l > p[i].x1 ) frame_l = p[i].x1;
							if( frame_r < p[i].x2 ) frame_r = p[i].x2;
						}

						if( to )
						{
							to->x1 = frame_l;
							to->y1 = frame_t;
							to->x2 = frame_r;
							to->y2 = frame_b;
							to->Width = frame_r - frame_l;
							to->Height = frame_b - frame_t;
						}

						top    = frame_t;
						bottom = frame_b;
						left   = frame_l;
						right  = frame_r;
					}
					*/

					pTextEffect = pTextEffect->Next();
				}
			}

			for( t = 0; t < glyphs; t++ )
			{
				for( c = 0; c < 4; c++ )
				{
					//===========================================================
					// Here, I multiply the invidiual vertex colors by the base
					// color, then add one.  This allows me to perform floating
					// point to 8-bit integer conversion quickly by reading the
					// lowest 8-bits of the raw floating point data.
					//===========================================================

					p[t].vertex[c].a = 1.0f + color.a * p[t].vertex[c].a;
					p[t].vertex[c].r = 1.0f + color.r * p[t].vertex[c].r;
					p[t].vertex[c].g = 1.0f + color.g * p[t].vertex[c].g;
					p[t].vertex[c].b = 1.0f + color.b * p[t].vertex[c].b;
				}
			}

			//====================================================================
			// Get ready to the screen space transformations.
			//====================================================================

			screenx = m_screenx;
			screeny = m_screeny;

			if( screenx == 0.0f ) screenx = (float) Display__GetScreenWidth();
			if( screeny == 0.0f ) screeny = (float) Display__GetScreenHeight();

			screenx = 1.0f / screenx;
			screeny = 1.0f / screeny;

			m_ComputeTextShortcutX = right - left;
			m_ComputeTextShortcutY = bottom - top;
			m_ComputeTextShortcut = true;
			transform = GetFamilyTransform();
			m_ComputeTextShortcut = false;

			transform.CatScale( screenx, screeny, 1.0f );
			transform.CatTranslation( -m_viewportx * screenx, -m_viewporty * screeny, 0.0f );

			SpriteMgr__SetTextureDbl( m_pFont->m_texture.handle );

			basepriority = GetSortedPriority();
			blendmode = m_state.sprite.blendmode;
			flags = ComputeFlags();

			for( i = 0; i < glyphs; i++ )
			{
				cc = m_pFont->GetCustomChar(p[i].ch);

				if( cc )
				{
					textureset = cc->textureset;
					texture = cc->texture;
				}
				else
				{
					textureset = m_pFont->m_texture.handle;
					texture = m_pFont->m_texture.texture;
				}

				SpriteMgr__SetTextureDbl( textureset );

				for( c = 0; c < 4; c++ )
				{
					//==============================================================
					// Stuff the sprite registers with the transformed data.
					//==============================================================

					vectors[c] = transform.Transform( p[i].vertex[c].x, p[i].vertex[c].y, 0.0f );

					texuvs[c].x( p[i].vertex[c].u);
					texuvs[c].y( p[i].vertex[c].v);

					//===========================================================
					// This was terribly slow, so I've replaced with highly
					// optimized code.
					//===========================================================

				//	#ifdef PS2
				//		colors[c].R = (u8) (128.0f * p[i].vertex[c].r);
				//		colors[c].G = (u8) (128.0f * p[i].vertex[c].g);
				//		colors[c].B = (u8) (128.0f * p[i].vertex[c].b);
				//		colors[c].A = (u8) (128.0f * p[i].vertex[c].a);
				//	#else
				//		colors[c].R = (u8) (255.0f * p[i].vertex[c].r);
				//		colors[c].G = (u8) (255.0f * p[i].vertex[c].g);
				//		colors[c].B = (u8) (255.0f * p[i].vertex[c].b);
				//		colors[c].A = (u8) (255.0f * p[i].vertex[c].a);
				//	#endif // _PS2
						
					
					//===========================================================
					// Here, the lowest 8-bits of each vertex color are extracted
					// and sent to the sprite registers.  Look at the difference
					// in execution speed here as opposed to the commented lines
					// of code it replaces (above).
					//===========================================================
						
					colors[c].R = (u8) FLOATTOINT(p[i].vertex[c].r);
					colors[c].G = (u8) FLOATTOINT(p[i].vertex[c].g);
					colors[c].B = (u8) FLOATTOINT(p[i].vertex[c].b);
					colors[c].A = (u8) FLOATTOINT(p[i].vertex[c].a);
				}

				priority = basepriority + p[i].priority;

				if( p[i].handle == INVALID_HANDLE )
					p[i].handle = SpriteMgr__RegisterSprite( texture, 0, priority, flags, vectors, texuvs, colors );
				else
				{
					SpriteMgr__SetTextureByIndex( p[i].handle, texture );
					SpriteMgr__UpdateSprite( p[i].handle, 0, priority, flags, vectors, texuvs, colors );
				}

				if( p[i].handle != INVALID_HANDLE )
				{
					SpriteMgr__SetBlendMode( p[i].handle, blendmode );
					SpriteMgr__EnableSprite( p[i].handle );
				}
			}

			for( i = glyphs; i < m_Glyphs.Count; i++ )
			{
				if( p[i].handle != INVALID_HANDLE )
					SpriteMgr__DisableSprite( p[i].handle );
			}
			
			//====================================================================
			// This is where the drop shadow is computed.
			//====================================================================

			if( !m_state.shadow.enabled )
				ReleaseShadows();
			else
			{
				Vector3 *pvectors;
				Vector2 *ptexuvs;
				ts_bRGBA *pcolors;
				u32 flags;

				//=================================================================
				// Set all vertex colors to black.
				//=================================================================

				colors[0].R = colors[0].G = colors[0].B = 0;
				colors[1].R = colors[1].G = colors[1].B = 0;
				colors[2].R = colors[2].G = colors[2].B = 0;
				colors[3].R = colors[3].G = colors[3].B = 0;

				//=================================================================
				// Get sprites from the Sprite Manager.
				//=================================================================

				ReserveShadows( m_Glyphs.Count );

				t_Handle *handles = m_Shadow.pHandles;

				//=================================================================
				// Loop through each glyph, setting up the drop shadow as we go.
				//=================================================================

				for( i = 0; i < m_Glyphs.Count; i++  )
				{
					if( p[i].handle == INVALID_HANDLE )
					{
						//===========================================================
						// When a glyph was not created (there are some special
						// special circumstances where this is possible), do not
						// attempt to setup a shadow for that glyph.
						//===========================================================

						if( handles[i] != INVALID_HANDLE )
							SpriteMgr__KillSprite( handles[i] ), handles[i] = INVALID_HANDLE;
					}
					else
					{
						SpriteMgr__GetDataPointers( p[i].handle, &flags, &pvectors, &ptexuvs, &pcolors );

						if( flags & SPRITE_DISABLED )
						{
							//========================================================
							// When a glyph is hidden or disabled (there are some
							// special circumstances where this is possible), do not
							// setup a shadow for that glyph.
							//========================================================

							if( handles[i] != INVALID_HANDLE )
								SpriteMgr__KillSprite( handles[i] ), handles[i] = INVALID_HANDLE;
						}
						else
						{
							//========================================================
							// Setup each vertex.
							//========================================================

							for( int c = 0; c < 4; c++ )
							{
								vectors[c].Set(pvectors[c].x() + m_state.shadow.x * screenx,
													pvectors[c].y() + m_state.shadow.y * screeny,
													pvectors[c].z());

								texuvs[c].x( ptexuvs[c].x());
								texuvs[c].y( ptexuvs[c].y());

								colors[c].A = (u8) ((int) pcolors[c].A * (int) m_state.shadow.intensity / 255);	// intensity is 0..255
							}

							if( handles[i] == INVALID_HANDLE )
								handles[i] = SpriteMgr__RegisterSprite( m_pFont->m_texture.texture, 0, priority - 1, 0, vectors, texuvs, colors );
							else
							{
								SpriteMgr__UpdateSprite( handles[i], 0, priority - 1, 0, vectors, texuvs, colors );
								SpriteMgr__EnableSprite( handles[i] );
							}
						}
					}
				}
			}
		}
	}
	return;
}

//=============================================================================
// Text::Copy()
//=============================================================================

void Text::Copy( Sprite *pSpriteToCopy )
{
	Copy( (Text *) pSpriteToCopy );
}

//=============================================================================
// Text::Copy()
//=============================================================================

void Text::Copy( Text *pTextToCopy )
{
	if( pTextToCopy )
	{
		Sprite::Copy( pTextToCopy );

		SetFont( pTextToCopy->m_pFont );
		SetText( pTextToCopy->m_Text.get() );

		memcpy( &m_Appearance, &pTextToCopy->m_Appearance, sizeof(m_Appearance) );
	}

	return;
}

//=============================================================================
// Text::SetTextEffect()
//=============================================================================

void Text::SetTextEffect( TextEffect *pTextEffect )
{
	if( pTextEffect )
	{
		if( !m_pTextEffects )
			m_pTextEffects = pTextEffect;
		else
		{
			TextEffect* pLastEffect = m_pTextEffects;

			while( pLastEffect && pLastEffect->Next() )
				pLastEffect = pLastEffect->Next();

			pLastEffect->SetNext( pTextEffect );
		}
	}

	EffectsOn();
}

//=============================================================================
// Text::GetFirstEffect()
//=============================================================================

TextEffect *Text::GetFirstEffect( void )
{
	return m_pTextEffects;
}

//=============================================================================
// Text::GetNextEffect
//=============================================================================

TextEffect* Text::GetNextEffect( TextEffect* pTextEffect )
{
	TextEffect* pNextEffect = 0;

	if( pTextEffect )
		pNextEffect = pTextEffect->Next();

	return pNextEffect;
}

//=============================================================================
// TextEffect::TextEffect
//=============================================================================

TextEffect::TextEffect( void )
{
	T = 0.0f;
	Playing = false;
}

//=============================================================================
// TextEffect::GetTextEffect
//=============================================================================

TextEffect* Text::GetTextEffect( const char* pClassname )
{
	TextEffect* pTextEffect = GetFirstEffect();

	while( pTextEffect )
	{
		if( 0 == strcmp( pTextEffect->Classname(), pClassname ) )
			break;

		pTextEffect = pTextEffect->Next();
	}

	return pTextEffect;
}

//=============================================================================
// Text::EffectsOn()
//=============================================================================

void Text::EffectsOn( void )
{
	m_EffectsOn = true;
}

//=============================================================================
// Text::EffectsOff()
//=============================================================================

void Text::EffectsOff( void )
{
	m_EffectsOn = false;
}

//=============================================================================
// Text::GetHandle()
//=============================================================================

t_Handle Text::GetHandle( int Index )
{
	t_Handle Handle = INVALID_HANDLE;

	if( m_Glyphs.pGlyphs && Index < m_Glyphs.Count )
		Handle = m_Glyphs.pGlyphs[Index].handle;

	return Handle;
}

//=============================================================================
// Text::GetNumHandles()
//=============================================================================

int Text::GetNumHandles( void )
{
	return m_Glyphs.Count;
}

//=============================================================================
// Text::GetShadowHandle()
//=============================================================================

t_Handle Text::GetShadowHandle( int Index )
{
	t_Handle Handle = INVALID_HANDLE;

	if( m_Shadow.pHandles && Index < m_Shadow.Count )
		Handle = m_Shadow.pHandles[Index];

	return Handle;
}

//=============================================================================
// Text::GetNumShadowHandles()
//=============================================================================

int Text::GetNumShadowHandles( void )
{
	return m_Shadow.Count;
}

//=============================================================================
// Text::GetFont()
//=============================================================================

Font *Text::GetFont( void )
{
	return m_pFont;
}

//=============================================================================
// Text::GetText()
//=============================================================================

char *Text::GetText( void )
{
	return m_Text.get();
}

//=============================================================================
// Text::ComputeTextLength()
//=============================================================================

float Text::ComputeTextLength()
{
	TextOutput Output;

	ComputeText( false, &Output );

	return Output.Width;
}

//=============================================================================
// Text::ComputeTextHeight()
//=============================================================================

float Text::ComputeTextHeight()
{
	TextOutput Output;

	ComputeText( false, &Output );

	return Output.Height;
}

//=============================================================================
// Text::ComputeTextOutput()
//=============================================================================

void Text::ComputeTextOutput( TextOutput *to )
{
	if( to )
		ComputeText( false, to );

	return;
}

//=============================================================================
// Text::ReserveGlyphs()
//=============================================================================

void Text::ReserveGlyphs( int Count )
{
	if( Count == 0)
	{
		return;
	}

	if( m_Glyphs.pGlyphs && m_Glyphs.Count >= Count && m_Glyphs.Count < Count + 8 )
		return;
	
	Glyph *pGlyphs = new Glyph[Count];

	if( pGlyphs )
	{
		if( Count >= m_Glyphs.Count )
		{
			if( m_Glyphs.pGlyphs )
				memcpy( pGlyphs, m_Glyphs.pGlyphs, sizeof(Glyph) * m_Glyphs.Count );

			for( int i = m_Glyphs.Count; i < Count; i++ )
				pGlyphs[i].handle = INVALID_HANDLE;
		}
		else
		{
			memcpy( pGlyphs, m_Glyphs.pGlyphs, sizeof(Glyph) * Count );

			for( int i = Count; i < m_Glyphs.Count; i++ )
			{
				if( m_Glyphs.pGlyphs[i].handle != INVALID_HANDLE )
					SpriteMgr__KillSprite( m_Glyphs.pGlyphs[i].handle );
			}
		}

		delete [] m_Glyphs.pGlyphs;

		m_Glyphs.pGlyphs = pGlyphs;
		m_Glyphs.Count = Count;

		// TJC - we have to tell our parents that we need releasing
		// eventually.
		InformParentsOurHandlesArentReleased();
	}

	return;
}

//=============================================================================
// Text::ReleaseGlyphs()
//=============================================================================

void Text::ReleaseGlyphs( void )
{
	if( m_Glyphs.pGlyphs )
	{
		for( int i = 0; i < m_Glyphs.Count; i++ )
		{
			if( m_Glyphs.pGlyphs[i].handle != INVALID_HANDLE )
				SpriteMgr__KillSprite( m_Glyphs.pGlyphs[i].handle );
		}

		delete [] m_Glyphs.pGlyphs;

		m_Glyphs.pGlyphs = 0;
		m_Glyphs.Count = 0;
	}

	return;
}

//=============================================================================
// Text::ReserveShadows() : Reserves some sprites for use in the drop shadow.
//=============================================================================

void Text::ReserveShadows( int Count )
{
	if( Count == 0)
	{
		return;
	}
	if( m_Shadow.Count == Count && m_Shadow.pHandles )
		return; // shadows already available

	t_Handle *pHandles = new t_Handle[Count];

	if( pHandles )
	{
		if( Count >= m_Shadow.Count )
		{
			if( m_Shadow.pHandles )
				memcpy( pHandles, m_Shadow.pHandles, sizeof(t_Handle) * m_Shadow.Count );

			for( int i = m_Shadow.Count; i < Count; i++ )
				pHandles[i] = INVALID_HANDLE;
		}
		else
		{
			memcpy( pHandles, m_Shadow.pHandles, sizeof(t_Handle) * Count );

			for( int i = Count; i < m_Shadow.Count; i++ )
			{
				if( m_Shadow.pHandles[i] != INVALID_HANDLE )
					SpriteMgr__KillSprite( m_Shadow.pHandles[i] );
			}
		}

		delete [] m_Shadow.pHandles;

		m_Shadow.pHandles = pHandles;
		m_Shadow.Count = Count;

		// TJC - Inform our parents that the job of releasing us is not
		// complete yet.
		InformParentsOurHandlesArentReleased();
	}

	return;
}

//=============================================================================
// Text::ReleaseShadows()
//=============================================================================

void Text::ReleaseShadows( void )
{
	if( m_Shadow.pHandles )
	{
		for( int i = 0; i < m_Shadow.Count; i++ )
		{
			if( m_Shadow.pHandles[i] != INVALID_HANDLE )
				SpriteMgr__KillSprite( m_Shadow.pHandles[i] );

			m_Shadow.pHandles[0] = INVALID_HANDLE;
		}

		delete [] m_Shadow.pHandles;

		m_Shadow.pHandles = 0;
		m_Shadow.Count = 0;
	}

	return;
}

//=============================================================================
// Text::AreHandlesReleased()
// TJC - check to see if ReleaseHandles() would have any effect.
//=============================================================================
bool Text::AreHandlesReleased( void )
{
	return Sprite::AreHandlesReleased() && 
		m_Glyphs.Count == 0 &&
		m_Shadow.Count == 0;
}

//=============================================================================
// ReleaseHandles()
//=============================================================================

void Text::ReleaseHandles( void )
{
	Sprite::ReleaseHandles();
	
	ReleaseGlyphs();
	ReleaseShadows();
}

//=============================================================================
// Text::SetTextAppearance()
//=============================================================================

void Text::SetTextApperance( float TextHeight, float TextSpacing, float TextAlignment, bool WordWrap, float LineSpacing )
{
	SetTextHeight( TextHeight );
	SetTextSpacing( TextSpacing );
	SetTextAlignment( TextAlignment );
	SetWordWrap( WordWrap );
}

//=============================================================================
// Text::SetTextHeight()
//=============================================================================

void Text::SetTextHeight( float Height )
{
	m_Appearance.TextHeight = Height;
}

//=============================================================================
// Text::GetTextHeight()
//=============================================================================

float Text::GetTextHeight( void )
{
	if( m_Appearance.TextHeight > 0.0f )
		return m_Appearance.TextHeight;

	return ComputeTextHeight();
}

//=============================================================================
// Text::SetTextSpacing()
//=============================================================================

void Text::SetTextSpacing( float TextSpacing )
{
	m_Appearance.TextSpacing = TextSpacing;
}

//=============================================================================
// Text::SetTextFixedSpacing()
//=============================================================================

void Text::SetTextFixedSpacing( float TextFixedSpacing )
{
	m_Appearance.TextFixedSpacing = TextFixedSpacing;
}

//=============================================================================
// Text::SetLineSpacing()
//=============================================================================

void Text::SetLineSpacing( float LineSpacing )
{
	m_Appearance.LineSpacing = LineSpacing;
}

//=============================================================================
// Text::GetLineSpacing()
//=============================================================================

float Text::GetLineSpacing( void )
{
	return m_Appearance.LineSpacing;
}

//=============================================================================
// Text::GetTextSpacing()
//=============================================================================

float Text::GetTextSpacing( void )
{
	return m_Appearance.TextSpacing;
}

//=============================================================================
// Text::SetWordWrap()
//=============================================================================

void Text::SetWordWrap( bool Enabled )
{
	if( Enabled )
		m_Appearance.Flags |=  TEXTAPPEARANCE_WORDWRAP;
	else
		m_Appearance.Flags &= ~TEXTAPPEARANCE_WORDWRAP;

	return;
}

//=============================================================================
// Text::GetWordWrap()
//=============================================================================

bool Text::GetWordWrap( void )
{
	if( m_Appearance.Flags & TEXTAPPEARANCE_WORDWRAP )
		return true;

	return false;
}


//=============================================================================
// Text::SetTranslator()
//=============================================================================

void Text::SetTranslator( bool Enabled )
{
	if( Enabled )
		m_Appearance.Flags |=  TEXTAPPEARANCE_TRANSLATE;
	else
		m_Appearance.Flags &= ~TEXTAPPEARANCE_TRANSLATE;

	return;
}

//=============================================================================
// Text::GetTranslator()
//=============================================================================

bool Text::GetTranslator( void )
{
	if( m_Appearance.Flags & TEXTAPPEARANCE_TRANSLATE )
		return true;

	return false;
}

//=============================================================================
// Text::SetTextAlignment()
//=============================================================================

void Text::SetTextAlignment( float TextAlignment )
{
	m_Appearance.TextAlignment = TextAlignment;
}

//=============================================================================
// Text::GetTextAlignment()
//=============================================================================

float Text::GetTextAlignment( void )
{
	return m_Appearance.TextAlignment;
}

//=============================================================================
// Text::Tesselate()
//=============================================================================

void Text::Tesselate( int Level )
{
	if( Level < 1 ) Level = 1;
	if( Level > 8 ) Level = 8;

	m_Tesselation = Level;
}

//=============================================================================
// Text::ComputeVertices()
//=============================================================================

void Text::GetSpriteVertices( Vector3 *vertices )
{
	if( vertices )
	{
		TextOutput Output;
		ComputeText( false, &Output );

		Graphics4x4 transform = GetFamilyTransform();

		vertices[0] = transform.Transform( Output.x1, Output.y1, 0.0f );
		vertices[1] = transform.Transform( Output.x2, Output.y1, 0.0f );
		vertices[2] = transform.Transform( Output.x2, Output.y2, 0.0f );
		vertices[3] = transform.Transform( Output.x1, Output.y2, 0.0f );
	}

	return;
}

//=============================================================================
// Text::GetSpriteRect()
//=============================================================================

void Text::GetSpriteRect( float &x1, float &y1, float &x2, float &y2 )
{
	Sprite::GetSpriteRect( x1, y1, x2, y2 );

	Vector3 vertices[4];
	int n;

	GetSpriteVertices( vertices );

	for( n = 0; n < 4; n++ )
	{
		if( x1 > vertices[n].x() ) x1 = vertices[n].x();
		if( y1 > vertices[n].y() ) y1 = vertices[n].y();
		if( x2 < vertices[n].x() ) x2 = vertices[n].x();
		if( y2 < vertices[n].y() ) y2 = vertices[n].y();
	}

	return;
}

//=============================================================================
// Text::GetCharVertices
//=============================================================================

bool Text::GetCharVertices( char ch, Vector3 *vertices )
{
	int i;
	float x, y;

	if( ch < 0 )
	{
		for( i = 0; i < 4; i++ )
			vertices[i].Set( 0.0f, 0.0f, 0.0f );

		return false;
	}

	TextOutput Output;
	ComputeText( false, &Output );

	if( m_Glyphs.Count < ch )
	{
		for( i = 0; i < 4; i++ )
			vertices[i].Set( 0.0f, 0.0f, 0.0f );

		return false;
	}

	Graphics4x4 transform = GetFamilyTransform();

	for( i = 0; i < 4; i++ )
	{
		x = m_Glyphs.pGlyphs[ch].vertex[i].x;
		y = m_Glyphs.pGlyphs[ch].vertex[i].y;

		vertices[i] = transform.Transform( x, y, 0.0f );
	}

	return true;
}

//=============================================================================
// Text::GetCharRect
//=============================================================================

bool Text::GetCharRect( char ch, float &x1, float &y1, float &x2, float &y2 )
{
	Vector3 vertices[4];

	if( !GetCharVertices( ch, vertices ) )
	{
		x1 = y1 = 0.0f;
		x2 = y2 = 0.0f;

		return false;
	}

	x1 = vertices[0].x();
	y1 = vertices[0].y();
	x2 = x1;
	y2 = y1;

	for( int n = 1; n < 4; n++ )
	{
		if( x1 > vertices[n].x() ) x1 = vertices[n].x();
		if( y1 > vertices[n].y() ) y1 = vertices[n].y();
		if( x2 < vertices[n].x() ) x2 = vertices[n].x();
		if( y2 < vertices[n].y() ) y2 = vertices[n].y();
	}

	return true;
}

//=============================================================================
// Text::SaveObject()
//=============================================================================

bool Text::SaveObject( MemoryBank &m, char *texture )
{
	u32 Version = TEXT_LOADSAVE_VERSION;

	if( !Sprite::SaveObject( m, texture ) )
		return false;

	if( !m.WriteString( m_pFont->m_FontName.get() ) )
		return false;

	if( !m.WriteString( m_Text.get() ) )
		return false;

	if( !m.Write( &Version, sizeof(Version) ) )
		return false;

	if( !m.Write( &m_Appearance, sizeof(m_Appearance) ) )
		return false;

	return true;
}

//=============================================================================
// Text::LoadObject
//=============================================================================

bool Text::LoadObject( MemoryBank &m, char *texture )
{
	xstring szFont;
	xstring szText;

	if( !Sprite::LoadObject( m, texture ) )
		return false;

	if( !m.ReadString( szFont ) )
		return false;

	if( !m.ReadString( szText ) )
		return false;

	u32 Version;

	if( !m.Read( &Version, sizeof(Version) ) )
		return false;

	if( Version == TEXT_LOADSAVE_VERSION2 )
	{
		if( !m.Read( &m_Appearance, sizeof(m_Appearance) ) )
			return false;
	}
	else
	if( Version == TEXT_LOADSAVE_VERSION1 )
	{
		if( !m.Read( &m_Appearance, sizeof(m_Appearance) ) )
			return false;

		SetTranslator( true );
	}
	else
	{
		m.m_ReadPosition -= sizeof(Version);

		struct
		{
			float TextHeight;
			float TextSpacing;
			float TextFixedSpacing;
			float TextAlignment;
			float LineSpacing;
			float WordWrap;
			float MaxLength;
			float MaxHeight;
		}
		Appearance;

		if( !m.Read( &Appearance, sizeof(Appearance) ) )
			return false;

		m_Appearance.TextHeight       = Appearance.TextHeight;
		m_Appearance.TextSpacing      = Appearance.TextSpacing;
		m_Appearance.TextFixedSpacing = Appearance.TextFixedSpacing;
		m_Appearance.TextAlignment    = Appearance.TextAlignment;
		m_Appearance.LineSpacing		= Appearance.LineSpacing;
		m_Appearance.Flags            = 0;

		if( Appearance.WordWrap != 0.0f )
		{
			SetSize( Appearance.WordWrap, Appearance.MaxHeight );
			SetWordWrap( true );
		}
		else
		{
			SetSize( Appearance.MaxLength, Appearance.MaxHeight );
			SetWordWrap( false );
		}
	}


	int length = szFont.length();

	// HACK: remove the .tga extension from font names

	if(tolower(szFont.getch(length-4)) == '.' &&
		tolower(szFont.getch(length-3)) == 't' &&
		tolower(szFont.getch(length-2)) == 'g' &&
		tolower(szFont.getch(length-1)) == 'a' )
	{
		szFont.setch(length-4, 0);
	}

	SetFont( szFont.get() );
	SetText( szText.get() );

	return true;
}

//=============================================================================
// TextEffect::Start
//=============================================================================

void TextEffect::Start( void )
{
	SetT(0.0f);
	Resume();
}

//=============================================================================
// TextEffect::Stop
//=============================================================================

void TextEffect::Stop( void )
{
	Playing = false;
}

//=============================================================================
// TextEffect::Resume
//=============================================================================

void TextEffect::Resume( void )
{
	Playing = true;
}

//=============================================================================
// TextEffect::SetT
//=============================================================================

void TextEffect::SetT( float T )
{
	this->T = T;
}

//=============================================================================
// TextEffect::Update
//=============================================================================

void TextEffect::Update( float ET )
{
	if( Playing )
		SetT( T + ET );

	return;
}

//=============================================================================
// TextEffect::Effect
//=============================================================================

void TextEffect::Effect( int c, Glyph* p, float x1, float y1, float x2, float y2 )
{
}

//=============================================================================
// ArcEffect::ArcEffect()
//=============================================================================

ArcEffect::ArcEffect( float HorzOrigin, float VertOrigin, float Amplitude, float Frequency, float Phase, bool Animated )
{
	m_HorzOrigin = HorzOrigin;
	m_VertOrigin = VertOrigin;
	m_Amplitude = Amplitude;
	m_Frequency = Frequency;
	m_Phase = Phase;

	if( Animated ) 
	{
		m_HorzAnim.SetTarget( &m_HorzOrigin );
		m_HorzAnim.SetFrame( 0.000f, 0.50f );
		m_HorzAnim.SetFrame( 0.500f, 1.00f );
		m_HorzAnim.SetFrame( 1.500f, 0.00f );
		m_HorzAnim.SetFrame( 2.500f, 1.00f );
		m_HorzAnim.SetLoopPoint( 0.5f );
		m_HorzAnim.TimeCurve( true );
		m_HorzAnim.Play( true );
	}

	return;
}

//=============================================================================
// ArcEffect::Effect()
//=============================================================================

void ArcEffect::Effect( int Count, Glyph *p, float Left, float Right, float Top, float Bottom )
{
	float w = Right - Left;
	float h = Bottom - Top;
	float l = Math::Sqrt(w*w + h*h) * 0.5f;
	float cx = Left + w * m_HorzOrigin;
	float cy = Top + h * m_VertOrigin;
	float dx, dy;
	float x, y;
	float r, t;

	for( int i = 0; i < Count; i++ )
	{
		for( int n = 0; n < 4; n++ )
		{
			dx = p[i].vertex[n].x - cx;
			dy = p[i].vertex[n].y - cy;

			r = Math::Sqrt( dx*dx + dy*dy ) / l;
			t = Math::ArcTan2( dy, dx );

			x = cx + dx - r * Math::Cos(t + Math::Deg2Rad( dx/w ) * m_Frequency + Math::Deg2Rad( m_Phase )) * dy/h * 25.0f * m_Amplitude;
			y = cy + dy - r * Math::Sin(t + Math::Deg2Rad( dx/w ) * m_Frequency + Math::Deg2Rad( m_Phase )) * dy/h * 50.0f * m_Amplitude;

			p[i].vertex[n].x = x;
			p[i].vertex[n].y = y;
		}
	}

	return;
}
