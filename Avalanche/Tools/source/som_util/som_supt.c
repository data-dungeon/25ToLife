/* --------------------------------------------------
 * File    : SOM_SUPT.C
 * Created : Fri Oct  2 14:34:21 1998
 * Descript:
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <t_types.h>
#include <t_struct.h>
#include <t_som.h>

#include "som_supt.h"

/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Local Data
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/

bool somTriFace( ts_SOMFaceUP *pFace)
{
unsigned long		l;

	for ( l = 0; l < 3; l++)
		if( pFace->usVtxIdxs[l] == pFace->usVtxIdxs[l + 1])
			return true;

	return false;
}


t_lflagbits somGetHasFlags( ts_SOMUP *pModel)
{
unsigned long		l;

t_lflagbits			lHasFlags;
ts_SOMFaceUP		*pFace;

	lHasFlags = 0;

	pFace = pModel->pFaceBuffer;

	for( l = 0; l < pModel->ulFaceCount; l++)
	{
		if( pFace->ulTexCount != 0)
			lHasFlags |= SOM_HAS_TEXTURES;
		else
			lHasFlags |= SOM_HAS_COLORS;

		if( somTriFace( pFace) == true)
			lHasFlags |= SOM_HAS_TRIS;
		else
			lHasFlags |= SOM_HAS_QUADS;

		if( (lHasFlags & SOM_HAS_ALL) == SOM_HAS_ALL)
			break;

		pFace++;
	}


	return lHasFlags;
}


void somNextFace( ts_SOMFace **ppFace)
{
unsigned char		*pFaceData;
unsigned long		ulVtxs;

ts_SOMFace			*pFace;

	pFace = *ppFace;

	pFaceData = (unsigned char *) (pFace + 1);

	ulVtxs = (pFace->Flags & SOMF_TRIANGLE) ? 3 : 4;

	pFaceData += ulVtxs * sizeof( unsigned short);

	if( pFace->Flags & SOMF_CLR_SINGLE)
	{
		if( pFace->Flags & SOMF_CLR_FLOAT)
			pFaceData += sizeof( ts_fRGBA);
		else
			pFaceData += sizeof( ts_bRGBA);
	}
	else if( pFace->Flags & SOMF_CLR_VERTEX)
	{
		if( pFace->Flags & SOMF_CLR_FLOAT)
			pFaceData += sizeof( ts_fRGBA) * ulVtxs;
		else
			pFaceData += sizeof( ts_bRGBA) * ulVtxs;
	}

	if( pFace->Flags & SOMF_NORMAL)
		pFaceData += sizeof( ts_f3DCoord);

	if( pFace->Flags & SOMF_SNGL_TEXTURE)
		pFaceData += ulVtxs * sizeof( ts_f2DCoord);
	else if( pFace->Flags & SOMF_MULT_TEXTURE)
		pFaceData += pFace->ulTexCount * (sizeof( unsigned short) + (ulVtxs * sizeof( ts_f2DCoord)));

	*ppFace = (ts_SOMFace *) &pFaceData;
}


	

