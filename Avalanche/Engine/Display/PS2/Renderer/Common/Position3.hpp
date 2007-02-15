
template <class T> Position3<T>& Position3<T>::operator +=( const Position3<T> &Pos)
{
	dX += Pos.X( );
	dY += Pos.Y( );
	dZ += Pos.Z( );

	return *this;
}

template <class T> Position3<T>& Position3<T>::operator *=( const Position3<T> &Pos)
{
	dX *= Pos.X( );
	dY *= Pos.Y( );
	dZ *= Pos.Z( );

	return *this;
}

template <class T> Position3<T>& Position3<T>::operator *=( T tScale)
{
	dX *= tScale;
	dY *= tScale;
	dZ *= tScale;

	return *this;
}




template<class T> ostream & operator<<( ostream &out, const Position3<T> &Pos)
{

	out << Pos.X( ) << " " << Pos.Y( ) << " " << Pos.Z( ) << "\n";

	return out;
}


