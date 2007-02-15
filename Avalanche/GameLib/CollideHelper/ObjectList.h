/*
**
**  File:   ObjectList.h
**  Date:   June 17, 2003
**  By:     Bryant Collard
**  Desc:   A list of collision objects.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/CollideHelper/ObjectList.h $
**  $Revision: 1 $
**      $Date: 6/18/03 12:27p $
**    $Author: Bcollard $
**
*/

#ifndef OBJECT_LIST_H
#define OBJECT_LIST_H

#include "collide/Object.h"

class CCollisionEvent;
class CCollisionList;
class CStageEntity;

class CObjectList
{
  public:
	CObjectList();
	void Clear(void);
	bool Add(CCollisionEvent* i_event);
	bool Add(CCollisionEvent* i_event, CStageEntity* i_subject);
	bool Add(CCollisionList* i_list);
	bool Add(CCollisionList* i_list, CStageEntity* i_subject);
	bool Add(CCollisionObject* i_object);
	CCollisionObject* GetNext(void);
//	CCollisionObject* GetNext(CCollisionObject::ECollisionObjectType i_type);
	CCollisionObject* GetNext(int i_type);
	CCollisionObject* GetHead(void);
//	CCollisionObject* GetHead(CCollisionObject::ECollisionObjectType i_type);
	CCollisionObject* GetHead(int i_type);

	// For some reason, VisualC generates an internal compiler error if the
	// enum is used as an argument and the template definition follows.

  protected:
	int d_currObject;
	int d_numObjects;
	int d_maxObjects;
	CCollisionObject** d_objects;
};

template <int SIZE>
class CObjectListArray : public CObjectList
{
  public:
	CObjectListArray();

  private:
	CCollisionObject* d_objectArray[SIZE];
};

#define OBJECT_LIST_HPP
#include "CollideHelper/ObjectList.hpp"

#endif // OBJECT_LIST_H
