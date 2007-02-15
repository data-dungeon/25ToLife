/*
**
**  File:   Interface.h
**  Date:   February 4, 2004
**  By:     Bryant Collard
**  Desc:   A COM like interface class.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Interfaces/Interface.h $
**  $Revision: #2 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#pragma once

#ifndef INTERFACE_H
#define INTERFACE_H

#include "platform/BaseType.h"

#define INTERFACE_ID_INVALID 0

class CInterface
{
  public:
	static uint GetID(void) {return(CInterface::GetValidID(s_interfaceID));}

  protected:
	inline static uint GetValidID(uint &i_id);

  private:
	static uint s_interfaceID;
	static uint s_lastID;
};


inline uint CInterface::GetValidID(uint &i_id)
{
	if (i_id == INTERFACE_ID_INVALID)
	{
		s_lastID++;
		i_id = s_lastID;
	}
	return(i_id);
}

#endif // INTERFACE_H
