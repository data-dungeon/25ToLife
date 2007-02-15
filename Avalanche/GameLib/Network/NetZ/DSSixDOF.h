/*
**
**  File:   DSSixDOF.h
**  Date:   April 30, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of the generic state dataset.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DSSixDOF.h $
**  $Revision: #11 $
**  $DateTime: 2005/08/15 17:04:26 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DS_SIX_DOF_H
#define DS_SIX_DOF_H

#include <netz.h>
#include "Network/NetZ/DSSixDOFDDL.h"
#include "Network/NetZ/DRModel.h"

class DRSixDOFModel;
class DSVector;

class DSSixDOF : public DATASET(DSSixDOF)
{
  public:
	DSSixDOF() {d_acc = NULL;}
	void SetAccelerationDataset(DSVector &i_acc) {d_acc = &i_acc;}
	DSVector &GetAccelerationDataset(void) const
			{ASSERT_PTR(d_acc); return(*d_acc);}

	// Macros to make dead reckoning accessible.
	DeclareDRMembers(DSSixDOF, DRSixDOFModel, DRSixDOFModelPolicy)
	DROperationBegin

  private:
	DSVector* d_acc;
};

#endif // DS_SIX_DOF
