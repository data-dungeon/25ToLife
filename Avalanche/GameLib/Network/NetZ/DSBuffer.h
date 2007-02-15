/*
**
**  File:   DSBuffer.h
**  Date:   February 16, 2005
**  By:     Bryant Collard
**  Desc:   Implementation general buffer dataset.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DSBuffer.h $
**  $Revision: #5 $
**  $DateTime: 2005/08/25 14:58:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DS_BUFFER_H
#define DS_BUFFER_H

#include <netz.h>
#include "Network/NetZ/DSBufferDDL.h"
#include "GameHelper/Callback.h"

class DSBuffer : public DATASET(DSBuffer)
{
  public:
	DSBuffer();
	void SetCallback(CCallbackBase *i_callback);
	void SetSize(uint32 i_size);
	uint32 GetSize(void) const;
	void* GetBuffer(void) const;

	void OperationBegin(DOOperation* i_operation);
	void OperationEnd(DOOperation* i_operation);

  private:
	CCallbackBase *d_callback;
};

#endif // DS_BUFFER_H
