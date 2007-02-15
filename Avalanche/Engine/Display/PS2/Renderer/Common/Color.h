#ifndef __COLOR_H
#define __COLOR_H

#include "ColorTypes.h"

template <class T> class BaseColor
{
	T				cR, cG, cB, cA;

	void Set( const BaseColor &defColor);
	
public:
	BaseColor( );
	BaseColor( T defR, T defG, T defB, T defA ) { cR = defR; cG = defG; cB = defB; cA = defA; }
	BaseColor( const BaseColor &defColor );

	BaseColor& operator =( const BaseColor &defColor);
	
	BaseColor& operator +=( const BaseColor &Other);

	void					Set( T defR, T defG, T defB, T defA ) { cR = defR; cG = defG; cB = defB; cA = defA; }
	T 						ClampComponent( T Val) const;
	void 					Clamp( void);

	unsigned short 	ColorRGBA5551( ) const;
	unsigned int		ColorRGBA8888( ) const;

	float 				fR( ) const;
	float 				fG( ) const;
	float 				fB( ) const;
	float					fA( ) const;

	unsigned char bR( ) const;
	unsigned char bG( ) const;
	unsigned char bB( ) const;
	unsigned char bA( ) const;
} ;

template<class T> inline bool operator==( const BaseColor<T> &Left, const BaseColor<T> &Right)
{
	return Left.ColorRGBA8888( ) == Right.ColorRGBA8888( );
}
	
#include "color.hpp"
#endif

	