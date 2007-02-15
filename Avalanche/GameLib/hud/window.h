#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "hud/sprite.h"
#include "hud/text.h"

#define WINDOW_EDGEOFFSET 6.0f
#define WINDOW_NUM_ACTIVE_REGIONS 3
#define WINDOW_NUM_BORDER_PIECES 5

class Window : public Sprite
{
	DECLARE_CLASSOBJECT( Window );

	public:
		Window( void );
		virtual ~Window();

	public:
		virtual void ShowInterior( void );
		virtual void HideInterior( void );
		virtual void SetCaption( const char *pCaption, float HorzAlign = 0.5f, float VertAlign = 0.0f );
		virtual void ShowCaptionBack( void );
		virtual void HideCaptionBack( void );
		virtual void Reset( void );

	protected:
		virtual void ComputeSprite( void );

	public:
		float GetBorderWidth( void );
		float GetBorderHeight( void );
		float GetCaptionHeight( void );

	public:
		Sprite m_Interior;
		Sprite m_BorderTL;
		Sprite m_BorderT[ WINDOW_NUM_BORDER_PIECES ];
		Sprite m_BorderTR;
		Sprite m_BorderR[ WINDOW_NUM_BORDER_PIECES ];
		Sprite m_BorderBR;
		Sprite m_BorderB[ WINDOW_NUM_BORDER_PIECES ];
		Sprite m_BorderBL;
		Sprite m_BorderL[ WINDOW_NUM_BORDER_PIECES ];
		Sprite m_CaptionBack;
		Text   m_Caption;
		float m_HorzAlign;
		float m_VertAlign;
};

class FrameWindow : public Sprite
{
	DECLARE_CLASSOBJECT( FrameWindow );

	public:
		FrameWindow( void );
		virtual ~FrameWindow();

	public:
		virtual void ShowInterior( void );
		virtual void HideInterior( void );
		virtual void SetCaption( const char *pCaption, float HorzAlign = 0.5f, float VertAlign = 0.0f );
		virtual void Reset( void );

	protected:
		virtual void ComputeSprite( void );

	public:
		float GetBorderWidth( void );
		float GetBorderHeight( void );
		float GetCaptionHeight( void );

	public:
		Sprite m_Interior;
		Sprite m_BorderTL;
		Sprite m_BorderT;
		Sprite m_BorderTR;
		Sprite m_BorderR;
		Sprite m_BorderBR;
		Sprite m_BorderB;
		Sprite m_BorderBL;
		Sprite m_BorderL;
		Text   m_Caption;
		float m_HorzAlign;
		float m_VertAlign;
};

#endif // _WINDOW_H_