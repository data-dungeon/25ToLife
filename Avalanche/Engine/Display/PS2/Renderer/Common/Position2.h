#ifndef __POSITION2_H
#define __POSITION2_H

#if !defined(PS2) || !defined(SN)
#include <ostream>
#endif

using namespace std;

template <class T> class Position2
{

protected:
	T				dX;
	T				dY;

public:
	Position2( ) : dX( 0), dY( 0) { }
	Position2( T defX, T defY) : dX( defX), dY( defY) { }
	Position2( const Position2 &Pos) { dX = Pos.X( ); dY = Pos.Y( ); }
	
	Position2& operator=( const Position2 &Pos) { dX = Pos.X( ); dY = Pos.Y( ); return *this; }

	Position2& operator+=( const Position2 &Pos);
	Position2& operator*=( T fScale);

	void 					set( T defX, T defY) { dX = defX; dY = defY; }
	T X( ) const { return dX; }
	T Y( ) const { return dY; }
	
	T & x( ) { return dX; }
	T & y( ) { return dY; }
	
} ;

// Standard Operators

#if !defined(PS2) || !defined(SN)
template <class T> extern ostream& operator<<( ostream &out, const Position2<T> &Pos);
#endif

// Inline Operators

template <class T> Position2<T> operator +( const Position2<T> &Left, const Position2<T> &Right)
{
	return Position2<T>( Left.X( ) + Right.X( ), Left.Y( ) + Right.Y( ));
}

template <class T> Position2<T> operator -( const Position2<T> &Left, const Position2<T> &Right)
{
	return Position2<T>( Left.X( ) - Right.X( ), Left.Y( ) - Right.Y( ));
}

template <class T> Position2<T> operator *( const Position2<T> &Left, const Position2<T> &Right)
{
	return Position2<T>( Left.X( ) * Right.X( ), Left.Y( ) * Right.Y( ));
}

template <class T> Position2<T> operator /( const Position2<T> &Left, const Position2<T> &Right)
{
	return Position2<T>( Left.X( ) / Right.X( ), Left.Y( ) / Right.Y( ));
}

template <class T> inline bool operator==( const Position2<T> &Left, const Position2<T> &Right)
{
	return Left.X( ) == Right.X( )  &&  Left.Y( ) == Right.Y( );
}

template <class T> inline bool operator!=( const Position2<T> &Left, const Position2<T> &Right)
{
	return Left.X( ) != Right.X( )  ||  Left.Y( ) != Right.Y( );
}

#include "Position2.hpp"

#endif __POSITION2_H
