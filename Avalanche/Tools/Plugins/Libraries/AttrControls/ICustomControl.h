//																											 //
//	ICustomControl
//	

/*
 * $History: ICustomControl.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 11/21/02   Time: 4:07p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * spinner, sliders, interfaces, and implemtations
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/20/02   Time: 4:20p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * working on custom control port
 */

#ifndef ATTR_CONTROLS_ICUSTOM_CONTROL_H
#define ATTR_CONTROLS_ICUSTOM_CONTROL_H

namespace AttrControls 
{

class ICustomControl
{
public:
	enum EditSpinnerType 
	{
		EDITTYPE_INT, 
		EDITTYPE_FLOAT
	};

	virtual HWND GetHWnd(void)=0;
	virtual void Enable(BOOL onOff=TRUE)=0;
	virtual void Disable(void)=0;
	virtual BOOL IsEnabled(void)=0;
};

} // AttrControls

#endif