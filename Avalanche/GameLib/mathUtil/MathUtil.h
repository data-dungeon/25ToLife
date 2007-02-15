/************************************/
/*                                  */
/* MathUtil.h                       */
/* gamelib  10/11/02                */
/* ned martin  avalanche software   */
/* visual c++ 6.0                   */
/* miscellaneous math routines      */
/*                                  */
/************************************/

/* a routine to help solve ballistic motion problems */

#ifndef __MATHUTIL_H
#define __MATHUTIL_H

/******************* includes *****************************/

/* system includes */

/* engine includes */

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/******************* forward declarations ****************/

/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

/******************* global variables ********************/

/******************* global prototypes *******************/

/******************* class *******************************/

namespace MathUtil
{
	void SetPointPos(float yRot, float radius, Vector3 *pStart, Vector3 *pEnd);

};



#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __MATHUTIL_H
