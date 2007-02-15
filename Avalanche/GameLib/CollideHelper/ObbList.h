/*
**
**  File:   ObbList.h
**  Date:   March 15, 2003
**  By:     Bryant Collard
**  Desc:   Extract OBBs from a collision event list.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/CollideHelper/ObbList.h $
**  $Revision: 1 $
**      $Date: 3/20/03 8:11p $
**    $Author: Bcollard $
**
*/

#ifndef OBB_LIST_H
#define OBB_LIST_H

class OBB;
class CCollisionList;
class CCollisionEvent;
class CCollisionPair;

class CObbList
{
  public:
	CObbList();
	OBB* GetHead(CCollisionList* i_list);
	OBB* GetNext(void);

  private:
	CCollisionEvent* d_event;
	CCollisionPair* d_pair;
};

#define OBB_LIST_HPP
#include "CollideHelper/ObbList.hpp"

#endif // OBBLIST_H
