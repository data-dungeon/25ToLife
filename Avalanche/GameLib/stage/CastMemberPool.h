/*
**
**  File:   CastMemberPool.h
**  Date:   January 30, 2002
**  By:     Bryant Collard
**  Desc:   Manages pools of cast members.
**
**  Copyright (c) 2002, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLib/stage/CastMemberPool.h $
**  $Revision: 1 $
**      $Date: 1/31/02 3:12p $
**    $Author: Bcollard $
**
*/

#ifndef CAST_MEMBER_POOL_H
#define CAST_MEMBER_POOL_H

class CCastMember;
class CActor;
class CCast;

class CCastMemberPool
{
  public:
	virtual ~CCastMemberPool() {}
	virtual CCastMember* NewMember(CActor* i_actor, CCast* i_cast) = 0;
	virtual void DeleteMember(CCastMember* i_castMember) = 0;
};

#endif // CAST_MEMBER_POOL_H
