/*************************************/
/*                                   */
/*   g_AuraMgr.h                     */
/*   big juju prototype  06/03/99    */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   aura manager                    */
/*                                   */
/*************************************/

#ifndef __G_AURAMGR_H
#define __G_AURAMGR_H

/******************* includes ****************************/

/* system includes */

/* engine includes */

#include "platform/BaseType.h"		// low-level include

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/******************* forward declarations ****************/

/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

/******************* global variables ********************/

/******************* global prototypes *******************/

int AuraMgr__Init(int nMaxAuras);
t_Handle AuraMgr__AddAura(void *pOwner, float Distance, float DistanceDamping, float SinAngleStart, float SinAngleInc, float Amplitude, float AmplitudeDamping, float Lifetime, int nFlags);
void AuraMgr__Advance(void);

#ifdef __cplusplus
}
#endif //__cplusplus



#endif // __G_AURAMGR_H
