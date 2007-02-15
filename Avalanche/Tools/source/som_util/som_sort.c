/* --------------------------------------------------
 * File    : SOM_SORT.C
 * Created : Wed Oct 21 14:32:31 1998
 * Descript:
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/
#include <stdlib.h>
#include <string.h>

#include <som_util.h>
#include <t_macros.h>

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/
void					somBubbleSort( ts_SOMUP *pModel, short (*CompFunc)(ts_SOMFaceUP *p1, ts_SOMFaceUP *p2));
short					somCompText( ts_SOMFaceUP *pL, ts_SOMFaceUP *pR);
short					somCompTextAlpha( ts_SOMFaceUP *pL, ts_SOMFaceUP *pR);

/* --------------------------------------------------
 * Local Data
 * --------------------------------------------------*/
t_lflagbits			gblSortFlags;
ts_RBImage			*gblpTextures;

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/

void somSort( ts_SOMUP *pModel, t_lflagbits SortFlags)
{
	gblSortFlags = SortFlags;

	somBubbleSort( pModel, somCompText);
}

void somSortAlpha( ts_SOMUP *pModel, t_lflagbits SortFlags, ts_RBImage *pImages)
{
	gblSortFlags = SortFlags;
	gblpTextures = pImages;

	somBubbleSort( pModel, somCompTextAlpha);
}

#define SOM_COMP_L_HIGHER (-1)
#define SOM_COMP_R_HIGHER ( 1)

void somBubbleSort( ts_SOMUP *pModel, short (*CompFunc)(ts_SOMFaceUP *p1, ts_SOMFaceUP *p2))
{
unsigned long			l,m;

unsigned long			ulLastFace;

ts_SOMFaceUP			TempFace;
ts_SOMFaceUP			*pFaceList;


	ulLastFace = pModel->ulFaceCount - 1;
	pFaceList = pModel->pFaceBuffer;

	for( l = ulLastFace; l > 0; l--)
	{
		for( m = 0; m < l; m++)
		{
			if( CompFunc( pFaceList + m, pFaceList + m + 1) == SOM_COMP_L_HIGHER)
			{
				//swap
				memcpy( &TempFace, pFaceList + m, sizeof( ts_SOMFaceUP));
				memcpy( pFaceList + m, pFaceList + m + 1, sizeof( ts_SOMFaceUP));
				memcpy( pFaceList + m + 1, &TempFace, sizeof( ts_SOMFaceUP));
			}
		}
	}
}

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/

#define RET_COMPLONG(l,r) \
	if( l > r) \
		return SOM_COMP_L_HIGHER; \
	else if( l < r) \
		return SOM_COMP_R_HIGHER; \
	else \
		return 0;


short somCompbRGBA( ts_SOMFaceUP *pL, ts_SOMFaceUP *pR)
{
ts_bRGBA					L, R;

	L.R = RGBF_TO_B( pL->Color.R);
	L.G = RGBF_TO_B( pL->Color.G);
	L.B = RGBF_TO_B( pL->Color.B);
	L.A = RGBF_TO_B( pL->Color.A);

	R.R = RGBF_TO_B( pR->Color.R);
	R.G = RGBF_TO_B( pR->Color.G);
	R.B = RGBF_TO_B( pR->Color.B);
	R.A = RGBF_TO_B( pR->Color.A);

	RET_COMPLONG( *((long *) &L), *((long *) &R));
}

short AlphaWeight( ts_SOMFaceUP *pFace)
{
ts_RBImage			*pImage;

	if( pFace->ulTexCount == 0)
		return (pFace->Color.A < 1.0) ? 1 : 0;
	else
	{
		pImage = gblpTextures + pFace->pTextures->TexID;

		return ( pImage->IFlags & RB_IMG_RGBA) ? 2 : 0;
	}
}

short somCompTextAlpha( ts_SOMFaceUP *pL, ts_SOMFaceUP *pR)
{
short					sAlphaL, sAlphaR;

	sAlphaL = AlphaWeight( pL);
	sAlphaR = AlphaWeight( pR);

	if( sAlphaL == sAlphaR)
		return somCompText( pL, pR);
	else
		return (sAlphaR > sAlphaL) ? SOM_COMP_R_HIGHER : SOM_COMP_L_HIGHER;
}
	
short somCompText( ts_SOMFaceUP *pL, ts_SOMFaceUP *pR)
{
unsigned long		l;
unsigned long		ulCompTexs;

	// One or Both has Color Only
	if( pL->ulTexCount == 0  &&  pR->ulTexCount == 0)
		return somCompbRGBA( pL, pR);
	else if( pR->ulTexCount == 0)
		return SOM_COMP_L_HIGHER;
	else if( pL->ulTexCount == 0)
		return SOM_COMP_R_HIGHER;

	if( gblSortFlags & SOM_SORT_SNGL_TEXTURE)
		ulCompTexs = 1;
	else
		ulCompTexs = (pR->ulTexCount < pL->ulTexCount) ? pR->ulTexCount : pL->ulTexCount;

	for( l = 0; l < ulCompTexs; l++)
	{
		if( pL->pTextures[l].TexID > pR->pTextures[l].TexID)
			return SOM_COMP_L_HIGHER;
		else if( pR->pTextures[l].TexID > pL->pTextures[l].TexID)
			return SOM_COMP_R_HIGHER;
	}

	if( gblSortFlags & SOM_SORT_SNGL_TEXTURE)
		return 0;

	RET_COMPLONG( pL->ulTexCount, pR->ulTexCount);

}


