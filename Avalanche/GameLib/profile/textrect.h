//*************************************************************************
// A simple Rect class that does everything for text
// - has been overloaded to be intersection
// + has been overloaded to be union
//*************************************************************************
#ifndef TEXTRECT_H
#define TEXTRECT_H

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#ifdef _XBOX
#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"
#else
#include <windows.h>
#endif
#endif

class TextRect
{
public:
	// The constructors/destructor
	inline TextRect(void) {}
	inline TextRect(int left, int top, int right, int bottom)
		{ Set(left, top, right, bottom); }

	// Special quantities
	inline int Width(void)
		{ return(right - left); }
	inline int Height(void)
		{ return(bottom - top); }
	inline int Area(void)
		{ return(Width() * Height()); }

	inline void Set(int newLeft, int newTop, int newRight, int newBottom);
	inline void Clear(void)
		{ left = 0; top = 0; right = 0; bottom = 0; }

	inline void TopLeft(int newTop, int newLeft)
		{ left = newLeft; top = newTop; }
	inline void BottomRight(int newBottom, int newRight)
		{ right = newRight; bottom = newBottom; }

	// Sepecial stuff
	inline void Normalize(void);
	inline void Inflate(int x, int y);

	// Comparisions & tests
	inline int IsEmpty(void)
		{ return(left == right && top == bottom); }
	inline int IsNormal(void)
		{ return(left <= right && top <= bottom); }

	// Overloaded operators
	inline int operator==(const TextRect &r);
	inline int operator!=(const TextRect &r);
	inline TextRect &operator=(const TextRect &r);

	// Find the union
	inline TextRect &operator+=(const TextRect &r);
	inline TextRect operator+(const TextRect &r);

	// Find the intersection
	inline TextRect &operator-=(const TextRect &r);
	inline TextRect operator-(const TextRect &r);

#ifdef WIN32
	// Windows RECT support
	inline TextRect(RECT &rect)
		{ Set(rect.top, rect.left, rect.bottom, rect.right); }
	inline void Set(RECT &rect)
		{ Set(rect.top, rect.left, rect.bottom, rect.right); }
	inline operator RECT() const
		{ RECT value = { top, left, bottom, right }; return(value); }
#endif

	// The rect coordinates
	int   left;
	int   top;
	int   right;
	int   bottom;

private:
	// Swap two ints
	static void Swap(int &a, int &b)
		{ int t; t = b; b = a; a = t; }
};

// No reason for this all to be inline, but I'm lazy
#include "profile/textrect.hpp"

#endif
