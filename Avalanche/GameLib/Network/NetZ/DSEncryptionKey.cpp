/*
**
**  File:   DSEncryptionKey.cpp
**  Date:   October 3, 2005
**  By:     Bryant Collard
**  Desc:   Key used to encrypt session data.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/NetZ/DSEncryptionKey.h"

// Local headers
#include "Network/Session.h"
#include "Network/NetZ/NetZSession.h"
#include "Network/NetZ/DOHost.h"

/******************************************************************************/
/******************************************************************************/
void DSEncryptionKey::OperationEnd(DOOperation* i_operation)
{
	switch(i_operation->GetType())
	{
	 case Operation::AddToStore:
	 case Operation::RemoveFromStore:
	 case Operation::UpdateDataSet:
		if (i_operation->GetAssociatedDO()->IsAKindOf(DOCLASSID(DOHost)))
		{
			// Yech!
			((CNetZSession*)&g_session)->TestEncryptionKey(
					(DOHost*)i_operation->GetAssociatedDO());
		}
		break;
	}
}
