/* --------------------------------------------------
 * File    : Vct_F.C
 * Created : Mon Oct 19 11:16:07 1998
 * Descript: Floating Point Vct Library
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <lvector.h>

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
m_bool vctEqualf( float (*pA)[], float (*pB)[], int iCount)
{
	return memcmp( pA, pB, iCount * sizeof( float)) == 0 ? TRUE : FALSE;
}

void vctAddConstf( float (*pA)[], float fConst, int iCount)
{
int					i;
	for( i = 0; i < iCount; i++)
		(*pA)[i] += fConst;

}

void vctAddf( float (*pA)[], float (*pB)[], float (*pC)[], int iCount)
{
int				i;

	for( i = 0; i < iCount; i++)
		(*pC)[i] = (*pA)[i] + (*pB)[i];
}

void vctSubf( float (*pA)[], float (*pB)[], float (*pC)[], int iCount)
{
int				i;

	for( i = 0; i < iCount; i++)
		(*pC)[i] = (*pA)[i] - (*pB)[i];
}

void vctMulConstf( float (*pA)[], float fConst, int iCount)
{
int				i;
	for( i = 0; i < iCount; i++)
		(*pA)[i] *= fConst;
}

float vctLengthSq3f( l_Vct3f *pA)
{
	return (*pA)[0] * (*pA)[0] + (*pA)[1] * (*pA)[1] + (*pA)[2] * (*pA)[2];
}


float vctLength3f( l_Vct3f *pA)
{
	return (float) sqrt( vctLengthSq3f( pA ));
}


void vctNormalize3f( l_Vct3f *pA)
{
float			fLen;
float			fInvLen;     +

	fLen = vctLength3f( pA);

	if( fLen)
	{
		fInvLen = 1.0f / fLen;

		(*pA)[0] *= fInvLen;
		(*pA)[1] *= fInvLen;
		(*pA)[2] *= fInvLen;

	}
}

float vctDot3f( l_Vct3f *pA, l_Vct3f *pB)
{
	return (*pA)[0] * (*pB)[0] + (*pA)[1] * (*pB)[1] + (*pA)[2] * (*pB)[2];
}


void vctCross3f( l_Vct3f *pA, l_Vct3f *pB, l_Vct3f *pC)
{
	(*pC)[X] = (*pA)[Y] * (*pB)[Z] - (*pA)[Z] * (*pB)[Y];
	(*pC)[Y] = (*pA)[Z] * (*pB)[X] - (*pA)[X] * (*pB)[Z];
	(*pC)[Z] = (*pA)[X] * (*pB)[Y] - (*pA)[Y] * (*pB)[X];
}

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/
