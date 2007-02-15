/*
**
**  File:   Environment.cpp
**  Date:   April 8, 2004
**  By:     Bryant Collard
**  Desc:   Store information about an environment.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Loop/Environment.cpp $
**  $Revision: #2 $
**  $DateTime: 2004/05/04 00:19:55 $
**    $Author: Food $
**
*/

// Precompiled header.
#include "Loop/LoopPCH.h"

// My header.
#include "Loop/Environment.h"

/***************************************************************************/
/***************************************************************************/
CEnvironment::CEnvironment()
{
	d_name[0] = '\0';
	d_path[0] = '\0';
	d_root[0] = '\0';
	d_temp[0] = '\0';
}

/***************************************************************************/
/***************************************************************************/
void CEnvironment::SetWorldName(const char* i_name)
{
	if ((i_name != NULL) && (i_name[0] != '\0'))
	{
		strcpy(d_name, i_name);

		strcpy(d_path, "levels\\");
		strcat(d_path, d_name);
		strcat(d_path, "\\");

		strcpy(d_root, d_path);
		strcat(d_root, d_name);
	}
	else
	{
		d_name[0] = '\0';
		d_path[0] = '\0';
		d_root[0] = '\0';
	}
	d_temp[0] = '\0';
}

/***************************************************************************/
/***************************************************************************/
char* CEnvironment::WorldPath(const char* i_append, ...)
{
	va_list list;
	va_start(list, i_append);

	strcpy(d_temp, d_path);
	const char* append = i_append;
	while (append != NULL)
	{
		if (append[0] != '\0')
			strcat(d_temp, append);
		append = va_arg(list, const char*);
	}
	va_end(list);

	return(d_temp);
}

/***************************************************************************/
/***************************************************************************/
char* CEnvironment::WorldRoot(const char* i_append, ...)
{
	va_list list;
	va_start(list, i_append);

	strcpy(d_temp, d_root);
	const char* append = i_append;
	while (append != NULL)
	{
		if (append[0] != '\0')
			strcat(d_temp, append);
		append = va_arg(list, const char*);
	}
	va_end(list);

	return(d_temp);
}
