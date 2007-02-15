template <class T> Position2<T>& Position2<T>::operator +=( const Position2<T> &Pos)
{
	dX += Pos.X( );
	dY += Pos.Y( );

	return *this;
}

template <class T> Position2<T>& Position2<T>::operator *=( T tScale)
{
	dX *= tScale;
	dY *= tScale;

	return *this;
}




template<class T> ostream & operator<<( ostream &out, const Position2<T> &Pos)
{

	out << Pos.X( ) << " " << Pos.Y( ) << "\n";

	return out;
}


