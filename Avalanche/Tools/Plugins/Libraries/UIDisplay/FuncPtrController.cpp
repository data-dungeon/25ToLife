//-----------------------------------------------------------------------------
//	File:		FuncPtrController.cpp
//	Created:	Jan. 15, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The FuncPtrController class acts as a controller for function
//			pointers.  It will store and retrieve specific function pointers as
//			needed.
//
//	History:	1/15/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "FuncPtrController.h"



//-----------------------------------------------------------------------------
// Defines, globals, etc.:

FuncPtrController theFuncPtrController;



//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// FuncPtrController::FuncPtrController
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
FuncPtrController::FuncPtrController()
{
	m_wndprocNewColorSwatch = NULL;
	m_wndprocOrigColorSwatch = NULL;
}


//-----------------------------------------------------------------------------
// FuncPtrController::~FuncPtrController
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
FuncPtrController::~FuncPtrController()
{
}


//-----------------------------------------------------------------------------
// FuncPtrController::SetNewSwatchProc
// This method will set the m_wndprocNewColorSwatch to the WNDPROC passed in.
//
// Input:
//  WNDPROC newColorSwatchProc -- The new color swatch proc.
//
// Output: None.
//-----------------------------------------------------------------------------
void FuncPtrController::SetNewSwatchProc(WNDPROC newColorSwatchProc)
{
	m_wndprocNewColorSwatch = newColorSwatchProc;
}


//-----------------------------------------------------------------------------
// FuncPtrController::SetOrigSwatchProc
// This method will set the m_wndprocOrigColorSwatch.
//
// Input:
//  WNDPROC origProc -- the original color swatch proc to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void FuncPtrController::SetOrigSwatchProc(WNDPROC origProc)
{
	m_wndprocOrigColorSwatch = origProc;
}


//-----------------------------------------------------------------------------
// FuncPtrController::GetNewSwatchProc
// This method will return the m_wndprocNewColorSwatch.
//
// Input: None.
//
// Output: WNDPROC representing the m_wndprocNewColorSwatch.
//-----------------------------------------------------------------------------
WNDPROC FuncPtrController::GetNewSwatchProc()
{
	return m_wndprocNewColorSwatch;
}


//-----------------------------------------------------------------------------
// FuncPtrController::GetOrigSwatchProc
// This method will return the m_fpColorSwatchProc
//
// Input: None.
//
// Output: WNDPROC representing the m_fpColorSwatchProc.
//-----------------------------------------------------------------------------
WNDPROC FuncPtrController::GetOrigSwatchProc()
{
	return m_wndprocOrigColorSwatch;
}
