#ifndef TEXT_H
#define TEXT_H

#include "hud/animator.h"
#include "hud/sprite.h"
#include "hud/font.h"
#include "hud/stringex.h"

#define DEFAULT_TEXT_HEIGHT		(32.0f)

struct Glyph
{
	t_Handle handle;
	struct {
		float x;
		float y;
		float a;
		float r;
		float g;
		float b;
		float u;
		float v;
	} vertex[4];
	float x1;
	float y1;
	float x2;
	float y2;
	int i;
	char ch;
	char sol;
	short priority;
};

struct TextOutput
{
	float x1;
	float y1;
	float x2;
	float y2;
	float Width;
	float Height;
};

class TextEffect;

#define TEXTAPPEARANCE_WORDWRAP		(1<<0)
#define TEXTAPPEARANCE_TRANSLATE		(1<<1)

class Text : public Sprite
{	
	DECLARE_CLASSOBJECT( Text );
	DECLARE_TREENODE( Text );

	public:
		Text();
		virtual ~Text();

		// Control
		virtual void Reset( void );
		virtual void SetFont( Font *pFont, float TextHeight = 0.0f, float Spacing = 0.0f );
		virtual void SetFont2( Font *pFont );
		virtual void SetFont( char *pFontName );
		virtual void SetText( const char *pText );
		virtual void SetTextNoTranslate( const char *pText );
		virtual void Printf( const char *fmt, ... );
		virtual void ClearText( void );
		virtual void Release();
		virtual void Copy( Sprite *pSpriteToCopy );
		virtual void Copy( Text *pTextToCopy );
		virtual void SetTextEffect( TextEffect *pEffectProcessor );
		virtual TextEffect* GetFirstEffect( void );
		virtual TextEffect* GetNextEffect( TextEffect* pTextEffect );
		virtual TextEffect* GetTextEffect( const char* pClassname );
		virtual void EffectsOn( void );
		virtual void EffectsOff( void );

		// Information
		virtual t_Handle GetHandle( int Index = 0 );
		virtual int GetNumHandles( void );
		virtual t_Handle GetShadowHandle( int Index = 0 );
		virtual int GetNumShadowHandles( void );
		virtual Font *GetFont( void );
		virtual char *GetText( void );
		virtual float ComputeTextLength();
		virtual float ComputeTextHeight();
		virtual void ComputeTextOutput( TextOutput *to );
		
		// Resources
		virtual void ReserveGlyphs( int Count );
		virtual void ReleaseGlyphs( void );
		virtual void ReserveShadows( int Count );
		virtual void ReleaseShadows( void );
		virtual void ReleaseHandles( void );

		// Appearance

		virtual void SetTextApperance( float TextHeight, float TextSpacing, float TextAlignment, bool WordWrap, float LineSpacing );
		virtual void SetTextHeight( float Height );
		virtual float GetTextHeight( void );
		virtual void SetTextSpacing( float TextSpacing );
		virtual void SetTextFixedSpacing( float TextFixedSpacing );
		virtual float GetTextSpacing( void );
		virtual void SetLineSpacing( float LineSpacing );
		virtual float GetLineSpacing( void );
		virtual void SetTextAlignment( float Alignment );
		virtual float GetTextAlignment( void );
		virtual void SetWordWrap( bool Enabled );
		virtual bool GetWordWrap( void );
		virtual void SetTranslator( bool Enabled );
		virtual bool GetTranslator( void );
		virtual void Tesselate( int Level );

		//////////////////////////////////////////////////////////////////////////

		virtual void GetSpriteVertices( Vector3 *vertices );
		virtual void GetSpriteRect( float &x1, float &y1, float &x2, float &y2 );
		virtual bool GetCharVertices( char ch, Vector3* vertices );
		virtual bool GetCharRect( char ch, float &x1, float &y1, float &x2, float &y2 );

		//////////////////////////////////////////////////////////////////////////
		
		virtual bool SaveObject( MemoryBank &m, char *texture );
		virtual bool LoadObject( MemoryBank &m, char *texture );

	protected:
		virtual void ComputeTransform( Graphics4x4 &transform );
		virtual void ComputeSprite( void );
		virtual void ComputeText( bool Draw, TextOutput *to );

		// TJC - You MUST override this and provide appropriate calls to
		// InformParentsOurHandlesArentReleased() if you override
		// ReleaseHandles()!!
		virtual bool AreHandlesReleased(void);
	public:
		Font *m_pFont;
		xstring m_Text;
		struct {
			float TextHeight;
			float TextSpacing;
			float TextFixedSpacing;
			float TextAlignment;
			float LineSpacing;
			u8 Flags;
		} m_Appearance;
		
		int m_Tesselation;
		bool m_EffectsOn;
		TextEffect* m_pTextEffects;

	private:
		struct {
			Glyph *pGlyphs;
			int Count;
		} m_Glyphs;
		struct {
			t_Handle *pHandles;
			int Count;
		} m_Shadow;
		
		bool m_ComputeTextShortcut;
		float m_ComputeTextShortcutX;
		float m_ComputeTextShortcutY;
};

class TextEffect : public TreeNode
{
	DECLARE_CLASSOBJECT( TextEffect );
	DECLARE_CLASSFACTORY( TextEffect );
	DECLARE_TREENODE( TextEffect );

	public:
		TextEffect();

	public:
		virtual void Start( void );
		virtual void Stop( void );
		virtual void Resume( void );
		virtual void SetT( float T );
		virtual void Update( float ET );
		virtual void Effect( int c, Glyph *p, float x1, float y1, float x2, float y2 );

	protected:
		float T;
		bool Playing;
};

class ArcEffect : public TextEffect
{
	public:
		ArcEffect( float HorzOrigin = 0.5f, float VertOrigin = 1.0f, float Amplitude = 0.25f, float Frequency = 90.0f, float Phase = 0.0f, bool Animated = false );

	protected:
		virtual void Effect( int c, Glyph *p, float x1, float y1, float x2, float y2 );

	public:
		float m_HorzOrigin;
		float m_VertOrigin;
		float m_Amplitude;
		float m_Frequency;
		float m_Phase;
		bool m_Animated;

	protected:
		Animator m_HorzAnim;
};

#endif
