//-----------------------------------------------------------------------------
//	File:		ApplyColorsCommunicator.h
//	Created:	May 1, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The ApplyColorsCommunicator class derives from KeyValCommunicatorBase
//			class in the KeyVal shared library.  This class will handle all
//			communication between the plugin and the shared library.  The types
//			of communication necessary would be things such as redrawing the
//			dynamic UI, purging keys, remaping keys or values, etc.
//
//	History:	5/01/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __APPLYCOLORSCOMMUNICATOR__
#define __APPLYCOLORSCOMMUNICATOR__



//-----------------------------------------------------------------------------
// Includes:


//-----------------------------------------------------------------------------
// Pre-declarations, etc.:

class FaceAttrController;

//-----------------------------------------------------------------------------
// Class Declaration:

class ApplyColorsCommunicator : public ApplyColorsCommunicatorBase
{
	FaceAttrController* m_pFaceAttrController;

public:
	// Constructors and destructors:
	ApplyColorsCommunicator(FaceAttrController* pFaceAttrController);
	~ApplyColorsCommunicator();

	void HandleApplyColor(void);
	void HandleRemoveColor(void);
};

//-----------------------------------------------------------------------------


#endif
