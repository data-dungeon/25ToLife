//-----------------------------------------------------------------------------
//	File:		FuncPtrController.h
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


#ifndef __FUNCPTRCONTROLLER__
#define __FUNCPTRCONTROLLER__



//-----------------------------------------------------------------------------
// Includes:



//-----------------------------------------------------------------------------
// Pre-declarations, etc.:



//-----------------------------------------------------------------------------
// Class Declaration:

class FuncPtrController
{
public:
	FuncPtrController();
	~FuncPtrController();

	// Accessors:
	WNDPROC GetNewSwatchProc();
	WNDPROC GetOrigSwatchProc();

	// Mutators:
	void SetNewSwatchProc(WNDPROC newColorSwatchProc);
	void SetOrigSwatchProc(WNDPROC origProc);

protected:
	WNDPROC m_wndprocNewColorSwatch;
	WNDPROC m_wndprocOrigColorSwatch;

private:
};

//-----------------------------------------------------------------------------

extern FuncPtrController theFuncPtrController;

#endif
