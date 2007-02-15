/*
**
**  File:   ObjectQT.h
**  Date:   March 6, 2003
**  By:     Bryant Collard
**  Desc:   The static objects in the world stored in a quad tree (this was
**          OBBQuadTree). It is now generalized to support collision objects
**          in addition to OBBs.
**
**          Quadtrees reside in the XZ plane.
**          Quadrants have the following nomenclature:
**                     | +z
**               2     |     1
**                     |
**          -----------+----------- +x
**                     |
**               3     |     4
**                     |
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/gamelibNew/collide/ObjectQT.h $
**  $Revision: 4 $
**      $Date: 6/11/03 2:33p $
**    $Author: Tyler Colbert $
**
*/

#ifndef OBJECT_QT_H
#define OBJECT_QT_H

#include "Math/Vector.h"
#include "collide/xzrect.h"
#include "collide/quadtree.h"
#include "collide/Object.h"
#include "collide/obb.h"
#include "EngineHelper/drawutil.h"

#ifdef GCN
#include "platform/GCN/GCNMWPort.h"
#endif

#ifdef WIN32
//#define OBJECT_QT_WRITE_FILE
#endif
#ifdef OBJECT_QT_WRITE_FILE
#include <stdio.h>
#endif

// Forwards
class OBBQuadTreeFactoryNode;  // For OBBWorld
template<class T> class CCollisionStackArray;

/**********************************************************************
 *
 *	CLASS CStaticCollisionObject
 *
 *	Essentially a union of collision objects that can be in the static
 * object quadtree. Since unions cannot contain classes with constructors,
 *	The largest of the allowed types is used. This establishes a fixed size
 *	for all the objects so a mixture of them can be put in arrays. Also,
 *	this class provides routines that are unique to objects stored in a
 *	quadtree while avoiding virtual functions (so these things can be
 *	stored in files).
 *
 **********************************************************************/
class CStaticCollisionObject
{
  public:
	void Fixup(void);
	uint16 Color(void);
	uint8 Material(void);
	void Draw(DrawUtility::COLOR i_color);
	bool Validate(XZRect &i_rect);

  private:
	CCollisionObject::ECollisionObjectType Type(void)
		{return(((CCollisionObject*)&d_header)->GetObjectType());}
	OBBStaticInstance &Obb(void)
		{return(*(OBBStaticInstance*)&d_obb);}

	union
	{
		char d_header[sizeof(CCollisionObject)];
		char d_obb[sizeof(OBBStaticInstance)];
	};
};

/**********************************************************************
 *
 *	CLASS ObjectQuadTreeNode
 *
 **********************************************************************/
class ObjectQuadTreeNode
{
public:
	// Constructor does nothing (we are loaded from disk)
	ObjectQuadTreeNode(void) {}
	~ObjectQuadTreeNode(void) {}

	// Build a list of objects
	void BuildList(CCollisionStackArray<CStaticCollisionObject*>* io_list,
			Vector3CRef i_start, Vector3CRef i_chord) const;
	void BuildList(CCollisionStackArray<CStaticCollisionObject*>* io_list,
			const XZRect &i_rect) const;

	// Fixup the data (after a load)
	void Fixup(void);

	// A check
	void Validate(XZRect *parentRect = NULL,
			QUADTREECHILD which = QUADTREECHILD_NE);

#ifdef OBJECT_QT_WRITE_FILE
	// Print out quad tree info
	int PrintTree(int i_level, const char* i_dir, FILE* i_fp);
#endif

#ifdef GCN
	// Prep data for GCN
	inline void EndianSwap(void)
	{
		rect.EndianSwap();
		for (int i = 0; i < QUADTREECHILDREN; i++)
			ByteSwap(child[i]);
		ByteSwap(objectList);
		ByteSwap(objects);
	}
#endif

private:
	//*** structure must match disk image!
	// 	if you add anything you must update tool chain! ***

	// Our defining rect
	XZRect 							rect;

	// The four children
	ObjectQuadTreeNode			*child[QUADTREECHILDREN];

	// Our list of object instances
	CStaticCollisionObject		*objectList;
	uint								objects;

	// We are their friend!
	friend class OBBQuadTreeFactoryNode;

	static XZRect ChildRect(XZRect &parentRect, QUADTREECHILD which);
	static bool RectsSame(XZRect &a, XZRect &b);
};

#endif // OBJECT_QT_H
