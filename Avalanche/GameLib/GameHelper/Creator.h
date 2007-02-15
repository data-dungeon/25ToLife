/*
**
**  File:   Creator.h
**  Date:   May 21, 2004
**  By:     Bryant Collard
**  Desc:   A mechanism for creating an object with a given name.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/GameHelper/Creator.h $
**  $Revision: #3 $
**  $DateTime: 2004/12/16 19:58:42 $
**    $Author: Rob $
**
*/

#pragma once

#ifndef CREATOR_H
#define CREATOR_H

class CCreator
{
  protected:
	void Add(CCreator** io_head);
	static CCreator* Find(const char* i_name, CCreator* i_head);
	static bool Query(const char* i_name, CCreator* i_head);
	virtual const char* Name(void) = 0;

  private:
	CCreator* d_next;
};

#endif // CREATOR_H
