template <class T> BaseColor<T>::BaseColor( )
{
	cR = (T) 0;
	cG = (T) 0;
	cB = (T) 0;
	cA = (T) 0;
}
	

template <class T> unsigned short BaseColor<T>::ColorRGBA5551( void ) const
{
Color1555		Color;

	Color  = ((unsigned short) (bR( ) & 0xf8) >> 3);
	Color |= ((unsigned short) (bG( ) & 0xf8) << 2);
	Color |= ((unsigned short) (bB( ) & 0xf8) << 7);

	if( bA( ) > 0)
		Color |= 0x8000;
		
	return Color;
}

template <class T> unsigned int BaseColor<T>::ColorRGBA8888( void ) const
{
Color8888		Color;

	Color  = (unsigned int) (bR( ) & 0xff);
	Color |= (unsigned int) ((bG( ) & 0xff) <<  8);
	Color |= (unsigned int) ((bB( ) & 0xff) << 16);
	Color |= (unsigned int) ((bA( ) & 0xff) << 24);
		
	return Color;
}

template <class T> void BaseColor<T>::Clamp( void )
{
	cR = ClampComponent( cR);
	cG = ClampComponent( cG);
	cB = ClampComponent( cB);
	cA = ClampComponent( cA);
	
}



