#include <fstream.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>

#include <read_tga.h>


void main( int iArgC, char **ppcArgL)
{
char					cOutFile[128];
ReadTGA				*pReader;
ts_RBImage			*pImages[2];
ts_bRGB				*pBuf[2];

ofstream				*pOut;

	for( int i = 1; i < iArgC; i++)
	{

		pReader = new ReadTGA( (const char *) ppcArgL[i]);
		strcpy( cOutFile, ppcArgL[i]);
		strtok( cOutFile, ".");
		strcat( cOutFile, ".raw");

		pOut = new ofstream( cOutFile, ios::binary);

		pReader->Prep( );

		pReader->Read( );

		pImages[i - 1] = pReader->GetImage( 0);
		
		pBuf[i - 1] = pImages[i - 1]->pRGBImage;

		pOut->write( (const char *) pImages[i - 1]->pImage, pImages[i - 1]->usWidth * pImages[i - 1]->usHeight * 3);

		delete pOut;
		delete pReader;
	}

}
