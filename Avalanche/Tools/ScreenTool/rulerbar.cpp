#include "pch.h"
#include "rulerbar.h"

BEGIN_MESSAGE_MAP( RulerBar, CWnd )
	ON_WM_PAINT()
END_MESSAGE_MAP()

RulerBar::RulerBar( void )
{
	m_RangeLo = 0;
	m_RangeHi = 100;
	m_Step = 5;

	for( int i = 0; i < sizeof(m_Tab)/sizeof(m_Tab[0]); i++ )
		m_Tab[i] = 0;

	return;
}

RulerBar::~RulerBar()
{
}

void RulerBar::SetRange( int Lo, int Hi )
{
	m_RangeLo = Lo;
	m_RangeHi = Hi;
}

void RulerBar::SetStep( int Step )
{
	m_Step = Step;
}

void RulerBar::SetTab( int Tab, int Position )
{
	m_Tab[Tab] = Position;
}

void RulerBar::OnPaint( void )
{
	PAINTSTRUCT ps;
	CDC *pdc = BeginPaint( &ps );

	RECT rc;
	GetClientRect( &rc );

	int Width = rc.right - rc.left;
	int Height = rc.bottom - rc.top;
	int x = rc.left;
	int y = rc.top;
	int i;
	int Length;
	int Range;
	int Segments;
	int LineLength[5];

	if( Width > Height )
		Length = Height;
	else
		Length = Width;

	Length = Length * 6 / 10;
	
	Range = m_RangeHi - m_RangeLo;

	if( Range < 0 )
		Range = -Range;

	Segments = Range / m_Step;

	Segments = Segments * 5;

	LineLength[0] = Length;
	LineLength[1] = Length * 2 / 5;
	LineLength[2] = Length * 2 / 5;
	LineLength[3] = Length * 2 / 5;
	LineLength[4] = Length * 2 / 5;

	GdiFillRect( pdc->m_hDC, rc, 0xffffff );

	if( Width > Height )
	{
		for( i = 1; i < Segments; i++ )
		{
			x = rc.left + Width * i / Segments;

			GdiLine( pdc->m_hDC, x, y, x, y + LineLength[i%5], 0x000000 );
		}
	}
	else
	{
		for( i = 1; i < Segments; i++ )
		{
			y = rc.top + Height * i / Segments;

			GdiLine( pdc->m_hDC, x, y, x + LineLength[i%5], y, 0x000000 );
		}
	}

	EndPaint( &ps );
}