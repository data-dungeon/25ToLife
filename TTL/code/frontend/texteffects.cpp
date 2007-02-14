#include "TTLPCH.h"
#include "texteffects.h"

TextFadeEffect::TextFadeEffect( void )
{
}

void TextFadeEffect::Effect( int c, Glyph* p, float x1, float y1, float x2, float y2 )
{
	Update( g_timer.GetOSFrameSec() );

	float r = T * 150.0f;
	float a = r - 50.000f;
	float b = r - 0.000f;
	float h = b - a;

	float x3, y3;
	float x4, y4;
	float x, y;

	int na = int(a);
	int nb = int(b);
	int v;

	float e;

	for( int i = 0; i < c; i++ )
	{
		if( i < na )
			e = 1.0f;
		else
		if( i < nb )
		{
			e = (i - na) / h;

			e = 1.0f - (e * e);

			x1 = p[i].vertex[0].x;
			y1 = p[i].vertex[0].y;
			x2 = p[i].vertex[2].x;
			y2 = p[i].vertex[2].y;

			x3 = x1 + (x2 - x1) * 0.5f;
			y3 = y1 + (y2 - y1) * 0.5f;

			x1 = p[i].vertex[1].x;
			y1 = p[i].vertex[1].y;
			x2 = p[i].vertex[3].x;
			y2 = p[i].vertex[3].y;

			x4 = x1 + (x2 - x1) * 0.5f;
			y4 = y1 + (y2 - y1) * 0.5f;

			x = x3 + (x4 - x3) * 0.5f;
			y = y3 + (y4 - y3) * 0.5f;

			for( v = 0; v < 4; v++ )
			{
				x1 = p[i].vertex[v].x - x;
				y1 = p[i].vertex[v].y - y;

				p[i].vertex[v].x = x + x1 * (3.0f - e * 2.0f);
			//	p[i].vertex[v].y = y + y1 * (3.0f - e * 2.0f);
			}
		}
		else
		{
			e = 0.0f;
		}

		p[i].vertex[0].a *= e;
		p[i].vertex[1].a *= e;
		p[i].vertex[2].a *= e;
		p[i].vertex[3].a *= e;
	}

	return;
}

SqueezeFilter::SqueezeFilter( void )
{
	Squeeze( 0.5f, 0.0f );
	Pinch( 0.5f, 0.0f );
}

void SqueezeFilter::Squeeze( float center, float factor )
{
	m_SqueezeCenter = center;
	m_SqueezeFactor = factor;
}

void SqueezeFilter::Pinch( float center, float factor )
{
	m_PinchCenter = center;
	m_PinchFactor = factor;
}

void SqueezeFilter::Effect( int Count, Glyph *p, float Left, float Right, float Top, float Bottom )
{
	Glyph* c;

	float Width = Right - Left;
	float Height = Bottom - Top;
	float w = Width * 0.5f;
	float h = Height * 0.5f;
	float cx = Left + Width * m_SqueezeCenter;
	float cy = Top + Height * m_PinchCenter;

	float xmax = Math::Abs(m_SqueezeCenter - 0.5f);
	float ymax = Math::Abs(m_PinchCenter - 0.5f);

	xmax = 1.0f / ((0.5f + xmax) * Width);
	ymax = 1.0f / ((0.5f + ymax) * Height);

	float dx, dy;
	float rx, ry;
	float t1, t2;

	t1 = m_SqueezeFactor * 0.3f;
	t2 = m_PinchFactor * 0.3f;

	int i, n;

	for( i = 0; i < Count; i++ )
	{
		c = &p[i];

		for( n = 0; n < 4; n++ )
		{
			dx = c->vertex[n].x - cx;
			dy = c->vertex[n].y - cy;

			ry = dy;
			rx = dx;

			if( dy < 0 ) ry = -dy;
			if( dx < 0 ) rx = -dx;

			ry = ry * ymax;
			ry = ry * ry;

			rx = rx * xmax;
			rx = rx * rx;

			dx = dx * (ry * t1 + 1.0f - t1);
			dy = dy * (rx * t2 + 1.0f - t2);

			c->vertex[n].x = cx + dx;
			c->vertex[n].y = cy + dy;
		}
	}

	return;
}

BassPumpEffect::BassPumpEffect( void )
{
	x = 0.0f;
	y = 0.0f;

	float T = 20.0f;

	animx.SetFrame( 0.000f * T,-1.0f );
	animx.SetFrame( 0.200f * T, 2.0f );
	animx.SetFrame( 0.400f * T,-1.0f );
	animx.Play( true );
}

void BassPumpEffect::Effect( int Count, Glyph*p, float Left, float Right, float Top, float Bottom )
{
	x = animx.Value();
	y = 0.5f;

	int i, c;
	c = Count;

	float w = Right - Left;
	float h = Bottom - Top;
	float tx = Left + x * w;
	float ty = Top + y * h;
	float rmax = Math::Sqrt(w*w*0.25f + h*h*0.25f) * 1.750f;
	float dx, dy;
	float r, t, o;

	for( i = 0; i < Count; i++ )
	{
		for( c = 0; c < 4; c++ )
		{
			dx = tx - p[i].vertex[c].x;
			dy = ty - p[i].vertex[c].y;

			r = Math::Sqrt(dx*dx + dy*dy);
			t = Math::ArcTan2(dy, dx);

			o = 1.0f - Math::Clamp( r / rmax, 0.0f, 1.0f );
			r = r + o * (h * 0.30f);

			t = Math::Rad2Deg(t) + 5.0f * o;
		
			p[i].vertex[c].x = tx - Math::CosDeg(t) * r;
			p[i].vertex[c].y = ty - Math::SinDeg(t) * r;
		}
	}

	return;
}

void SortTagEffect::Effect( int c, Glyph* p, float x1, float y1, float x2, float y2 )
{
	int priority = c * 4 - 4;

	for( int i = 0; i < c; i++ )
		p[i].priority = c - (i<<2);

	return;
}