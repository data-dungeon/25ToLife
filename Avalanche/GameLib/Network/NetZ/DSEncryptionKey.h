/*
**
**  File:   DSEncryptionKey.h
**  Date:   October 3, 2005
**  By:     Bryant Collard
**  Desc:   Key used to encrypt session data.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#pragma once

#ifndef DS_ENCRYPTION_KEY_H
#define DS_ENCRYPTION_KEY_H

#include <netz.h>
#include "Network/NetZ/DSEncryptionKeyDDL.h"

class DSEncryptionKey : public DATASET(DSEncryptionKey)
{
  public:
	void OperationEnd(DOOperation* i_operation);
};

#endif // DS_ENCRYPTION_KEY
