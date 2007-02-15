#include <stdio.h>

#include "avawrite.h"


int main( int iArgC, char **ppcArgL)
{
AVAWrite				*pAVAWrite;


	if( iArgC != 3)
	{
		printf( "Usage: AVA2DBL <infile> <outfile>");
		return -1;
	}

	pAVAWrite = new AVAWrite( );

	if( pAVAWrite->AttachInput( ppcArgL[1]) == false)
		return -2;

	if( pAVAWrite->AttachOutput( ppcArgL[2], AVA_FMT_PC) == false)
		return -3;

	pAVAWrite->Write( AVA_WR_TEXTURES, 0);

	delete pAVAWrite;

	return 0;
}
