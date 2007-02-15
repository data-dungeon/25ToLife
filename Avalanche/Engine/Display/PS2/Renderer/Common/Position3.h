#ifndef __POSITION3_H
#define __POSITION3_H

#if !defined(PS2) || !defined(SN)
#include <ostream>
#endif

using namespace std;

template <class T> class Position3
{

protected:
	T				dX;
	T				dY;
	T				dZ;

public:
	Position3( ) : dX( 0), dY( 0), dZ( 0) { }
	Position3( T defX, T defY, T defZ) : dX( defX), dY( defY), dZ( defZ) { }
	Position3( const Position3 &Pos) { dX = Pos.X( ); dY = Pos.Y( ); dZ = Pos.Z( ); }
	
	Position3& operator=( const Position3 &Pos) { dX = Pos.X( ); dY = Pos.Y( ); dZ = Pos.Z( ); return *this; }

	Position3& operator+=( const Position3 &Pos);
	Position3& operator*=( const Position3 &Pos);
	Position3& operator*=( T fScale);

	void 					set( T defX, T defY, T defZ) { dX = defX; dY = defY; dZ = defZ; }

	T X( ) const { return dX; }
	T Y( ) const { return dY; }
	T Z( ) const { return dZ; }

	T& x( ) { return dX; }
	T& y( ) { return dY; }
	T& z( ) { return dZ; }
} ;

// Standard Operators

#if !defined(PS2) || !defined(SN)
template <class T> extern ostream& operator<<( ostream &out, const Position3<T> &Pos);
#endif

template <class T> const Position3<T> operator +( const Position3<T> &Left, const Position3<T> &Right)
{
	return Position3<T>( Left.X( ) + Right.X( ), Left.Y( ) + Right.Y( ), Left.Z( ) + Right.Z( ));
}

template <class T> const Position3<T> operator -( const Position3<T> &Left, const Position3<T> &Right)
{
	return Position3<T>( Left.X( ) - Right.X( ), Left.Y( ) - Right.Y( ), Left.Z( ) - Right.Z( ));
}

template <class T> Position3<T> operator *( const Position3<T> &Left, const Position3<T> &Right)
{
	return Position3<T>( Left.X( ) * Right.X( ), Left.Y( ) * Right.Y( ), Left.Z( ) * Right.Z( ));
}

template <class T> Position3<T> operator /( const Position3<T> &Left, const Position3<T> &Right)
{
	return Position3<T>( Left.X( ) / Right.X( ), Left.Y( ) / Right.Y( ), Left.Z( ) / Right.Z( ));
}



// Inline Operators

template <class T> inline bool operator==( const Position3<T> &Left, const Position3<T> &Right)
{
	return Left.X( ) == Right.X( )  &&  Left.Y( ) == Right.Y( )  &&  Left.Z( ) == Right.Z( );
}

template <class T> inline bool operator!=( const Position3<T> &Left, const Position3<T> &Right)
{
	return Left.X( ) != Right.X( )  ||  Left.Y( ) != Right.Y( )  ||  Left.Z( ) != Right.Z( );
}

// Template Operators

#include "Position3.hpp"

#endif __POSITION3_H
