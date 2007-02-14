#ifndef _TEXTEFFECTS_H_
#define _TEXTEFFECTS_H_

#include "hud/hudlib.h"

class TextFadeEffect : public TextEffect
{
	DECLARE_CLASSOBJECT( TextFadeEffect );

	public:
		TextFadeEffect( void );

	public:
		virtual void Effect( int c, Glyph* p, float x1, float y1, float x2, float y2 );
};

class SqueezeFilter : public TextEffect
{
	DECLARE_CLASSOBJECT( SqueezeFilter );

	public:
		SqueezeFilter( void );

		void Squeeze( float center, float factor );
		void Pinch( float center, float factor );
		
	public:
		virtual void Effect( int Count, Glyph *p, float Left, float Right, float Top, float Bottom );

		float m_SqueezeCenter;
		float m_SqueezeFactor;
		float m_PinchCenter;
		float m_PinchFactor;
};

class BassPumpEffect : public TextEffect
{
	DECLARE_CLASSOBJECT( BassPumpEffect );

	public:
		BassPumpEffect( void );

	public:
		virtual void Effect( int Count, Glyph*p, float Left, float Right, float Top, float Bottom );

		Animator animx;
		Animator animy;

		float x;
		float y;
};

class SortTagEffect : public TextEffect
{
	DECLARE_CLASSOBJECT( SortTagEffect );

	public:
		virtual void Effect( int c, Glyph* p, float x1, float y1, float x2, float y2 );
};

#endif // _TEXTEFFECTS_H_