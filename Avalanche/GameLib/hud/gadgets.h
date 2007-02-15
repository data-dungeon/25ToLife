#ifndef _GADGETS_H_
#define _GADGETS_H_

#include "hud/animator.h"
#include "hud/sprite.h"
#include "hud/text.h"
#include "hud/window.h"

class PictureText : public Sprite
{
	DECLARE_CLASSOBJECT( PictureText );

	public:
		typedef enum
		{
			TEXTPOS_RIGHT = 0,
			TEXTPOS_LEFT,
			TEXTPOS_BOTTOMRIGHT,
			TEXTPOS_BOTTOMLEFT,
		} TextPos;

		PictureText( void );
		virtual void Create( const char *pCaption, t_Handle TextureSet, int OnTexture, int OffTexture );
		virtual void Draw( void );
		virtual void Check( void );
		virtual void Uncheck( void );
		virtual void Toggle( void );
		virtual bool Checked( void );
		virtual float GetWidth( void );
		virtual float GetHeight( void );
		virtual void SetTextPosition( TextPos pos ) { textPos = pos; }
		
	protected:
		virtual void ComputeSprite( void );
	
	public:
		bool m_Checked;
		t_Handle m_TextureSet;
		int m_OnTexture;
		int m_OffTexture;
		Sprite m_Picture;
		Text m_Caption;
		float scalex;
		float scaley;
		TextPos textPos;
};

class ScrollBar : public Sprite
{
	DECLARE_CLASSOBJECT( ScrollBar );

	public:
		ScrollBar( void );

	public:
		virtual void Draw( void );

	public:
		virtual void Create( float x, float y, float Width, float Height, t_Handle TextureSet,
			int BarTexture, int ArrowUpTexture, int ArrowDownTexture, int ShaftTexture );
		virtual void SetRange( int Low, int Hi );
		virtual void SetPageSize( int LinesPerPage );
		virtual void SetThumbPos( int Pos );
		virtual int GetThumbPos( void );
		virtual void MoveThumbPos( int Change );

	protected:
		virtual void SyncComponents( void );
		
	public:
		Sprite m_Shaft;
		Sprite m_ArrowUp;
		Sprite m_ArrowDown;
		Sprite m_ScrollBar;
		int m_RangeHi;
		int m_RangeLow;
		int m_Position;
		int m_LinesPerPage;
};

class GamePadButtons : public Sprite
{
	DECLARE_CLASSOBJECT( GamePadButtons );

	public:
		GamePadButtons();

	public:
		virtual void CreateButton( int Button, const char *pText, t_Handle TextureSet, int Texture );
		virtual void RemoveButton( int Button );
		virtual void RemoveAllButtons( void );
		virtual void SetButtonText( int Button, const char *pText );
		virtual void SetButtonPicture( int Button, t_Handle TextureSet, int Texture );
		virtual void SetPictureSize( int Button, float Width, float Height );
		virtual void SetTextAppearance( float TextHeight, float TextSpacing, float TextAlignment );
		virtual void SetTextAppearanceButton( int Button, float TextHeight, float TextSpacing, float TextAlignment );
		virtual void SetTextColor( float r, float g, float b, float a );
		virtual void SetTextColorButton ( int Button, float r, float g, float b, float a );
		virtual void SetButtonTextAlpha( int Button, float a );
		virtual void SetButtonAlpha( int Button, float a );
		virtual void SetButtonTextOffset ( int Button, float x, float y );
		virtual void SetTextShadow( bool Enabled, u8 Intensity = 255, s8 x = 2, s8 y = 2 );
		virtual void SetButtonOffset( int Button, float x, float y );
		virtual void SetButtonScale( int Button, float scalex, float scaley );
		virtual void SetButtonColor ( int Button, float r, float g, float b, float a );
		virtual void SetFixedPositionLock( bool Enabled );

	protected:
		virtual void ComputeSprite( void );

	public:
		PictureText m_Buttons[12];
		bool m_FixedPositionLock;
};

#endif // _GADGETS_H_
