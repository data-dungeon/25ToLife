//
//	SetupCustomControls
//	

/*
 * $History: SetupCustomControls.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/27/02   Time: 10:22a
 * Updated in $/Avalanche/tools/Plugins/Max/ObjectAttributes
 * new setup custom controls class
 */

#ifndef OBJECT_ATTRIBUTES_SETUP_CUSTOM_CONTROLS_H
#define OBJECT_ATTRIBUTES_SETUP_CUSTOM_CONTROLS_H

class SetupCustomControls
{
public:
	static void Init(HINSTANCE hInstance);
	static void CALLBACK EditSetFocusCallback(void);
	static void CALLBACK EditKillFocusCallback(void);
};

#endif