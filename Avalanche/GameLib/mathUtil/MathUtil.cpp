/************************************/
/*                                  */
/* MathUtil.cpp                     */
/* gamelib  10/11/02                */
/* ned martin  avalanche software   */
/* visual c++ 6.0                   */
/* miscellaneous math routines      */
/*                                  */
/************************************/

/* a place to put miscellaneous math routines */

/******************* includes ****************************/

#include "mathUtil/MathUtilPCH.h"



#include "mathUtil/MathUtil.h"



/******************* defines *****************************/

/******************* externs *****************************/

/******************* global variables ********************/

/******************* local variables *********************/

/******************* local prototypes ********************/

/******************* functions ***************************/

/* note that this doesn't set y-value of pEnd */

void MathUtil::SetPointPos(float yRot, float radius, Vector3 *pStart, Vector3 *pEnd)
{
	float	fSin, fCos;
	Math::SinCos(Math::Deg2Rad(yRot), fSin, fCos);
	pEnd->x(pStart->x() + (radius * fSin));
	pEnd->z(pStart->z() + (radius * fCos));
}




