/*
**
**  File:   DSBuffer.cpp
**  Date:   February 16, 2005
**  By:     Bryant Collard
**  Desc:   Implementation general buffer dataset.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DSBuffer.cpp $
**  $Revision: #3 $
**  $DateTime: 2005/02/16 19:08:37 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/NetZ/DSBuffer.h"

/******************************************************************************/
/******************************************************************************/
DSBuffer::DSBuffer()
{
	d_buffer.SetContentSize(0);
	d_callback = NULL;
}

/******************************************************************************/
/******************************************************************************/
void DSBuffer::SetCallback(CCallbackBase *i_callback)
{
	d_callback = i_callback;
}

/******************************************************************************/
/******************************************************************************/
void DSBuffer::SetSize(uint32 i_size)
{
	d_buffer.SetContentSize(i_size);
}

/******************************************************************************/
/******************************************************************************/
uint32 DSBuffer::GetSize(void) const
{
	return(d_buffer.GetContentSize());
}

/******************************************************************************/
/******************************************************************************/
void* DSBuffer::GetBuffer(void) const
{
	return((void*)d_buffer.GetContentPtr());
}

/******************************************************************************/
/******************************************************************************/
void DSBuffer::OperationBegin(DOOperation* i_operation)
{
	// Clear the buffer so the pending update will write data at the begining
	// of the buffer instead of appending it.
	if (i_operation->GetType() == Operation::UpdateDataSet)
		d_buffer.Clear();
}

/******************************************************************************/
/******************************************************************************/
void DSBuffer::OperationEnd(DOOperation* i_operation)
{
	if ((d_callback != NULL) &&
			(i_operation->GetType() == Operation::UpdateDataSet))
		d_callback->Call();
}
