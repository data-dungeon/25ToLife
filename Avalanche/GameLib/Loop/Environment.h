/*
**
**  File:   Environment.h
**  Date:   April 8, 2004
**  By:     Bryant Collard
**  Desc:   Store information about an environment.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Loop/Environment.h $
**  $Revision: #3 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#pragma once

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "platform/BaseType.h"

#define MAX_NAME_LEN  128

class CEnvironment
{
  public:
	// Construct
	CEnvironment();

	// Set the world name.
	void SetWorldName(const char* i_name);

	// Get name of the current world.
	const char* WorldName(void) {return(d_name);}

	// Get the path of the current environment, i.e. "levels\<env>\".
	char* WorldPath(void) {return(d_path);}

	// Get the root file name of the current environment, i.e.
	// "levels\<env>\<env>".
	char* WorldRoot(void) {return(d_root);}

	// Get a the path or root file of the current environment with the given
	// strings appended. The final argument must be NULL. The result invalidated
	// when either function is called again.
	char* WorldPath(const char* i_append, ...);
	char* WorldRoot(const char* i_append, ...);

  private:
	char d_name[MAX_NAME_LEN];
	char d_path[MAX_NAME_LEN];
	char d_root[MAX_NAME_LEN];
	char d_temp[MAX_NAME_LEN];
};

#endif // ENVIRONMENT_H
