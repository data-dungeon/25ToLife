#include "pch.h"
#include "helpers.h"

bool PointInRectf( float x, float y, float x1, float y1, float x2, float y2 )
{
	if( x >= x1 && x <= x2 && y >= y1 && y <= y2 )
		return true;

	return false;
}

bool RectInRectf( float ax, float ay, float bx, float by, float x1, float y1, float x2, float y2 )
{
	if( ax > bx )
		return RectInRectf( bx, ay, ax, by, x1, y1, x2, y2 );

	if( ay > by )
		return RectInRectf( ax, by, bx, ay, x1, y1, x2, y2 );

	if( x1 > x2 )
		return RectInRectf( ax, ay, bx, by, x2, y1, x1, y2 );

	if( y1 > y2 )
		return RectInRectf( ax, ay, bx, by, x1, y2, x2, y1 );

	if( !PointInRectf( ax, ay, x1, y1, x2, y2 ) )
		return false;

	if( !PointInRectf( bx, ay, x1, y1, x2, y2 ) )
		return false;

	if( !PointInRectf( bx, by, x1, y1, x2, y2 ) )
		return false;

	if( !PointInRectf( ax, by, x1, y1, x2, y2 ) )
		return false;

	return true;
}

bool RectOnRectf( float ax, float ay, float bx, float by, float x1, float y1, float x2, float y2 )
{
	if( ax > bx )
		return RectOnRectf( bx, ay, ax, by, x1, y1, x2, y2 );

	if( ay > by )
		return RectOnRectf( ax, by, bx, ay, x1, y1, x2, y2 );

	if( x1 > x2 )
		return RectOnRectf( ax, ay, bx, by, x2, y1, x1, y2 );

	if( y1 > y2 )
		return RectOnRectf( ax, ay, bx, by, x1, y2, x2, y1 );

	if( PointInRectf( ax, ay, x1, y1, x2, y2 ) )
		return true;

	if( PointInRectf( bx, ay, x1, y1, x2, y2 ) )
		return true;

	if( PointInRectf( bx, by, x1, y1, x2, y2 ) )
		return true;

	if( PointInRectf( ax, by, x1, y1, x2, y2 ) )
		return true;

	return false;
}