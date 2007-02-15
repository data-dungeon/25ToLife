
#include "Color.h"

// Constructors

float BaseColor<float>::ClampComponent( float fC) const
{
	if( fC > 1.0f)
		fC = 1.0f;
	else if( fC < 0.0f)
		fC = 0.0f;
		
	return fC;
}

float BaseColor<float>::fR( void ) const { return cR; }
float BaseColor<float>::fG( void ) const { return cG; }
float BaseColor<float>::fB( void ) const { return cB; }
float BaseColor<float>::fA( void ) const { return cA; }

unsigned char BaseColor<float>::bR( void ) const { return (unsigned char) (ClampComponent( cR) * 255.0f); }
unsigned char BaseColor<float>::bG( void ) const { return (unsigned char) (ClampComponent( cG) * 255.0f); }
unsigned char BaseColor<float>::bB( void ) const { return (unsigned char) (ClampComponent( cB) * 255.0f); }
unsigned char BaseColor<float>::bA( void ) const { return (unsigned char) (ClampComponent( cA) * 255.0f); }

void BaseColor<float>::Set( const BaseColor &defColor )
{

	cR = defColor.fR( );
	cG = defColor.fG( );
	cB = defColor.fB( );
	cA = defColor.fA( );
}

BaseColor<float>& BaseColor<float>::operator +=( const BaseColor &defColor)
{
	cR += defColor.fR( );
	cG += defColor.fG( );
	cB += defColor.fB( );
	cA += defColor.fA( );

	return *this;
}
	
BaseColor<float>::BaseColor( const BaseColor &defColor)
{
	Set( defColor);
}
	
BaseColor<float>& BaseColor<float>::operator =(const BaseColor &defColor)
{

	Set( defColor);
	
	return *this;
}
		
