//
// These are placed in separate file from the cpp in case anyone needs to access some inline obb functions 
// without linking in the entire library
//
// this should be included only once (already included in obb.cpp)
//

#ifndef OBB_STATICS_H
#define OBB_STATICS_H

uint OBB::vertexIndex[8][3] =
{
	// Vertex			Coordinate
	{ 0, 1, 2 },	//   0				minx, miny, minz
	{ 3, 1, 2 },	//   1				maxx, miny, minz
	{ 0, 1, 5 },	//   2				minx, miny, maxz
	{ 3, 1, 5 },	//   3				maxx, miny, maxz
	{ 0, 4, 2 }, 	//   4            minx, maxy, minz
	{ 3, 4, 2 }, 	//   5            maxx, maxy, minz
	{ 0, 4, 5 }, 	//   6            minx, maxy, maxz
	{ 3, 4, 5 } 	//   7            maxx, maxy, maxz
};
uint OBB::edgeVertexIndex[12][2] =
{
	// Edge				Vertices
	{ 0, 1 },		//   0				0, 1
	{ 1, 3 },		//   1				1, 3
	{ 3, 2 },		//   2            3, 2
	{ 2, 0 },		//   3				2, 0
	{ 5, 4 },		//   4				5, 4
	{ 4, 6 },		//   5				4, 6
	{ 6, 7 },		//   6           	6, 7
	{ 7, 5 },		//   7            7, 5
	{ 0, 4 },		//   8				0, 4
	{ 1, 5 },		//   9            1, 5
	{ 2, 6 },   	//   10           2, 6
	{ 3, 7 } 		//   11				3, 7
};
uint OBB::faceVertexIndex[6][4] =
{
	//	Face				Vertices    	Edges       		Normal
	{ 0, 2, 6, 4 },	//   0				0, 2, 6, 4		-3, 10, -5. -8		  -x
	{ 1, 5, 7, 3 },	//   1				1, 5, 7, 3		9, -7, -11, -1		  +x
	{ 0, 1, 3, 2 },	//   2				0, 1, 3. 2		0, 1, 2, 3			  -y
	{ 4, 6, 7, 5 },	//   3				4, 6, 7, 5		5, 6, 7, 4			  +y
	{ 0, 4, 5, 1 },	//   4				0, 4, 5, 1		8, -4, -9, -0		  -z
	{ 2, 3, 7, 6 }		//   5				2, 3, 7, 6		-2, 11, -6, -10	  +z
};

#endif