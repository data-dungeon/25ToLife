#ifndef _HELPERS_H_
#define _HELPERS_H_

bool PointInRectf( float x, float y, float x1, float y1, float x2, float y2 );
bool RectInRectf( float ax, float ay, float bx, float by, float x1, float y1, float x2, float y2 );
bool RectOnRectf( float ax, float ay, float bx, float by, float x1, float y1, float x2, float y2 );

#endif // _HELPERS_H_