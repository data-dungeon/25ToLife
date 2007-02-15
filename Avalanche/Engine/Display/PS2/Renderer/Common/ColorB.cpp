
#include "Color.h"

// Constructors

float BaseColor<unsigned short>::fR( void ) const { return cR; }
float BaseColor<unsigned short>::fG( void ) const { return cG; }
float BaseColor<unsigned short>::fB( void ) const { return cB; }
float BaseColor<unsigned short>::fA( void ) const { return cA; }

unsigned short BaseColor<unsigned short>::ClampComponent( unsigned short fC) const
{
	
	if( fC > 255)
		fC = 255;

	return fC;
}

unsigned char BaseColor<unsigned short>::bR( void ) const { return (unsigned char) ClampComponent( cR); }
unsigned char BaseColor<unsigned short>::bG( void ) const { return (unsigned char) ClampComponent( cG); }
unsigned char BaseColor<unsigned short>::bB( void ) const { return (unsigned char) ClampComponent( cB); }
unsigned char BaseColor<unsigned short>::bA( void ) const { return (unsigned char) ClampComponent( cA); }

void BaseColor<unsigned short>::Set( const BaseColor &defColor )
{

	cR = defColor.bR( );
	cG = defColor.bG( );
	cB = defColor.bB( );
	cA = defColor.bA( );
}

BaseColor<unsigned short>::BaseColor( const BaseColor &defColor)
{
	Set( defColor);
}
	
BaseColor<unsigned short>& BaseColor<unsigned short>::operator =(const BaseColor &defColor)
{

	Set( defColor);
	
	return *this;
}
		
BaseColor<unsigned short>& BaseColor<unsigned short>::operator +=( const BaseColor &defColor)
{
	cR += defColor.bR( );
	cG += defColor.bG( );
	cB += defColor.bB( );
	cA += defColor.bA( );

	return *this;
}
