/*
**
**  File:   ProtagonistNode.h
**  Date:   July 12, 2004
**  By:     Bryant Collard
**  Desc:   A base class for collision nodes that also provide some
**          functionality expected from protagonists.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/CollideHelper/ProtagonistNode.h $
**  $Revision: #4 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#ifndef PROTAGONIST_NODE_H
#define PROTAGONIST_NODE_H

#include "collide/Node.h"

class CProtagonist;
class CCollisionObject;
class CInterface;
class CCollisionCull;
class XZRect;
class CCollisionPair;
class CCollisionEvent;

class CProtagonistNode : public CCollisionNode
{
  public:
	CProtagonistNode() {d_protagonist = NULL;}
	virtual ~CProtagonistNode() {}
	void Set(CProtagonist* i_protagonist) {d_protagonist = i_protagonist;}

	virtual CCollisionObject* GetCollisionObject(void) {return(NULL);}
	virtual CInterface* GetInterface(uint i_id) {return(NULL);}
	virtual bool GetCollisionCull(CCollisionCull &o_cull) {return(false);}
	virtual void UpdateCollisionHull(float i_deltaSec, XZRect &i_collisionHull) {}
	virtual void SyncWithActor(void) {}
	virtual void SyncWithAnimation(void) {}
	virtual uint16 Color(CCollisionPair* i_pair);
	virtual uint8 Material(CCollisionPair* i_pair);
	virtual void Draw(CCollisionEvent* i_event) {}

  protected:
	CProtagonist* d_protagonist;
};

#endif // PROTAGONIST_NODE_H
