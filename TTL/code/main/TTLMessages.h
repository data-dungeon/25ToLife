/*
**
**  File:   TTLMessages.h
**  Date:   April 8, 2004
**  By:     Bryant Collard
**  Desc:   Message structures used in TTL.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/main/TTLMessages.h $
**  $Revision: #1 $
**  $DateTime: 2004/04/12 10:36:06 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef TTL_MESSAGES_H
#define TTL_MESSAGES_H

struct SPause
{
	enum
	{
		PAUSE,
		UNPAUSE,
		TOGGLE
	} d_type;
	char* d_menu;
	bool d_affectAudio;
};

#endif  // TTL_MESSAGES_H
