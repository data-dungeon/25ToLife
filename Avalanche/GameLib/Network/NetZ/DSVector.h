/*
**
**  File:   DSVector.h
**  Date:   March 1, 2005
**  By:     Bryant Collard
**  Desc:   Implementation general vector dataset.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DSVector.h $
**  $Revision: #5 $
**  $DateTime: 2005/08/25 14:58:50 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DS_VECTOR_H
#define DS_VECTOR_H

#include <netz.h>
#include "Network/NetZ/DSVectorDDL.h"
#include "Math/Vector.h"

class DSVector : public DATASET(DSVector)
{
  public:
	DSVector();
	void SetVector(Vector3CRef i_vector);
	Vector3 GetVector(void) const {return(d_vector);}

	void OperationEnd(DOOperation* i_operation);

  private:
	bool d_isMaster;
	Vector3Packed d_vector;
};

#endif // DS_VECTOR_H
