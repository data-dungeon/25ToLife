//////////////////////////////////////////////////////////////////////////////////////////
//
//	UI FLAGS
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __UIFLAGS_H
#define __UIFLAGS_H

struct UIFlags {
	unsigned long
//	TESSELLATION OPTIONS
//					variabletessellation		: 1,
//					planarcheck					: 1,
					triangulate					: 1,

//	VERTEX OPTIONS
					displacevertices			: 1,

//	NORMAL OPTIONS
					recalcnormals				: 1,

//	CRACK SOLUTIONS
					fixtjunctions				: 1,

//	MISC OPTIONS
					verbose						: 1,
					bakeuvtile					: 1,
					relativetexpaths			: 1,
					assignshaders				: 1,
					usehouse					: 1,

//  ASSIGN SMOOTHING GROUP INDICIES
					calcsmoothinggroups			: 1,
//	BONES
					createbones					: 1;
};

#endif
