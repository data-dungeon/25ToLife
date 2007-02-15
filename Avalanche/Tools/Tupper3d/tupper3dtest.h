////////////////////////////////////////////////////////////////////////////
//
// Tupper3dtest
//
// test the tupperware 3d classes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: tupper3dtest.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/07/01   Time: 9:22a
 * Updated in $/Avalanche/tools/Tupper3d
 * Example of converting a tupperware file from the zup coordinate system
 * (max) to the yup coordinate system (game)
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 9/24/01    Time: 4:16p
 * Updated in $/Avalanche/tools/Tupper3d
 */

#ifndef TUPPER3D_TUPPER3DTEST_H
#define TUPPER3D_TUPPER3DTEST_H

class TupperwareAggregate;

void TransformScene(TupperwareAggregate *pRoot);
void TransformObject(TupperwareAggregate *pObjectAgg);
void TransformNode(TupperwareAggregate *pNodeAgg);


#endif