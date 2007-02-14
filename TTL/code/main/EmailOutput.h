/*
**
**  File:   EmailOutput.h
**  Date:   May 20, 2005
**  By:     Bryant Collard
**  Desc:   Manage emailing of debug output.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/main/EmailOutput.h $
**  $Revision: #1 $
**  $DateTime: 2005/05/20 12:21:09 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef EMAIL_OUTPUT_H
#define EMAIL_OUTPUT_H

namespace NEmailOutput
{
	int Count(void);
	const char* Label(int i_index);
	void Send(int i_index);
};

#endif // EMAIL_OUTPUT_H
