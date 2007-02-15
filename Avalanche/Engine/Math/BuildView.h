/*************************************/
/*                                   */
/*   BuildView.h                     */
/*   new game engine  03/20/01       */
/*   ned martin  avalanche software  */
/*   misc. math function             */
/*                                   */
/*************************************/

#ifndef _BUILDVIEW_H
#define _BUILDVIEW_H

#include "Math/Matrix.h"
#include "Math/Vector.h"

void BuildCTM(Vector3CRef pEye, Vector3CRef pFocal, Vector3CRef pUp, Graphics4x4 *pMatrix);

#endif //_BUILDVIEW_H

