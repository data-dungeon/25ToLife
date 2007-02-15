/*
**
**  File:   DSVector.cpp
**  Date:   March 1, 2005
**  By:     Bryant Collard
**  Desc:   Implementation general vector dataset.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DSVector.cpp $
**  $Revision: #3 $
**  $DateTime: 2005/06/02 20:03:22 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/NetZ/DSVector.h"

/******************************************************************************/
/******************************************************************************/
DSVector::DSVector()
{
	d_isMaster = false;
	d_vector.Clear();
}

/******************************************************************************/
/******************************************************************************/
void DSVector::SetVector(Vector3CRef i_vector)
{
	if (d_isMaster && ((Vector3)d_vector != i_vector))
	{
		d_vector = i_vector;
		d_x = d_vector.X();
		d_y = d_vector.Y();
		d_z = d_vector.Z();
		RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
void DSVector::OperationEnd(DOOperation* i_operation)
{
	// This seems like a lot of work.
	switch(i_operation->GetType())
	{
	 case Operation::AddToStore:
	 case Operation::ChangeMasterStation:
		d_isMaster = i_operation->GetAssociatedDO()->IsADuplicationMaster();
		break;
	 case Operation::UpdateDataSet:
		d_vector.Set(d_x, d_y, d_z);
		break;
	}
}
