#include <stdio.h>


#include <fn_wrap.h>


void main( void)
{

FNWrapper			Wrap1( "joe.tga");
FNWrapper			Wrap2;

	Wrap1.Debug( );
	Wrap2.Debug( );

	Wrap1.ChangeExt( ".bmp");
	Wrap2.ChangeExt( ".tif");

	Wrap1.Debug( );
	Wrap2.Debug( );

	Wrap1.ChangeName( "dave");
	Wrap2.ChangeName( "dave");

	Wrap1.Debug( );
	Wrap2.Debug( );

	Wrap1.ChangePath( "\\joe\\");
	Wrap2.ChangePath( "\\joe\\");

	Wrap1.Debug( );
	Wrap2.Debug( );


}
	