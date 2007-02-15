/**********************************************/
/*                                            */
/* LOD.h                                      */
/* big juju prototype  1/02/02                */
/* ned martin  avalanche software             */
/* visual c++ 6.0                             */
/* level-of-detail management                 */
/*                                            */
/**********************************************/

/* this file handles instances-- an instance is an occurrence of a model in the world, with a transform matrix to transform
from model space to world space */

#ifndef __LOD_H
#define __LOD_H

/* system includes */

/* engine includes */

/******************* forward declarations ****************/

struct ts_PatchSOM;							// in Game/Database/SOM.h

/******************* defines *****************************/

/* max levels-of-detail supported */

#define MAX_LODS	2

/* infinite switch distance */

#define LOD_INFINITY 10000000.0f

/******************* macros ******************************/

/******************* structures **************************/

/* if there are fewer than MAX_LODS levels-of-detail for a particular group, then the switch range should be */
/* essentially infinite for the far range of the last valid model */

struct ts_LOD
{
/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	

	union
	{
		u32					u32SOMIndex[MAX_LODS];	// value in database
		ts_PatchSOM			*pSOM[MAX_LODS];			// pointers to model that make up LOD group, patched by FixLOD
	};
	float	fSwitchRange[MAX_LODS - 1];				// switch distances

/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	
};

/******************* global variables ********************/

/******************* global prototypes *******************/



#endif // __LOD_H

