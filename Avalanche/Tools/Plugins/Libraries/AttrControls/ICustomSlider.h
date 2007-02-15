//
//	ICustomSpinner
//	

/*
 * $History: ICustomSlider.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 11/26/02   Time: 9:58a
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * added new controls to numeric type.
 * Fixed color issues with Max
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/21/02   Time: 4:07p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * spinner, sliders, interfaces, and implemtations
 */

#ifndef ATTR_CONTROLS_ICUSTOM_SLIDER_H
#define ATTR_CONTROLS_ICUSTOM_SLIDER_H

#ifndef ATTR_CONTROLS_ICUSTOM_CONTROL_H
#include "ICustomControl.h"
#endif

// LOWORD(wParam) = ctrlID, 
// HIWORD(wParam) = TRUE if user is dragging the slider interactively.
// lParam = pointer to ISliderControl
#define CC_ATSLIDER_CHANGE  		WM_USER + 611

// LOWORD(wParam) = ctrlID, 
// lParam = pointer to ISliderControl
#define CC_ATSLIDER_BUTTONDOWN	WM_USER + 612

// LOWORD(wParam) = ctrlID, 
// HIWORD(wParam) = FALSE if user cancelled - TRUE otherwise
// lParam = pointer to ISliderControl
#define CC_ATSLIDER_BUTTONUP		WM_USER + 613

namespace AttrControls 
{

class ICustomSlider : public ICustomControl
{
public:
	virtual float GetFVal(void)=0;
	virtual int GetIVal(void)=0;
	virtual void SetNumSegs( int num )=0;
	virtual void SetValue( float v, int notify )=0;
	virtual void SetValue( int v, int notify )=0;
	virtual void SetLimits( int min, int max, int limitCurValue = TRUE )=0;
	virtual void SetLimits( float min, float max, int limitCurValue = TRUE )=0;
	virtual void LinkToEdit( HWND hEdit, ICustomControl::EditSpinnerType type )=0;
	virtual void SetIndeterminate(BOOL i=TRUE)=0;
	virtual BOOL IsIndeterminate()=0;
	virtual void SetResetValue(float v)=0;
	virtual void SetResetValue(int v)=0;
	virtual void SetKeyBrackets(BOOL onOff)=0;
	virtual void SetDragNotify(BOOL onOff)=0;
	virtual BOOL GetDragNotify(void)=0;
	virtual void SetSliderFormat(const char *pFormat)=0;
	virtual const char *GetSliderFormat(void)=0;

	static ICustomSlider *GetISlider( HWND hCtrl );
	static void ReleaseISlider( ICustomSlider *pControl );
	static ICustomSlider *SetupIntSlider(HWND hwnd, int idSlider, int idEdit,  int min, int max, int val, int numSegs);
	static ICustomSlider *SetupFloatSlider(HWND hwnd, int idSlider, int idEdit,  float min, float max, float val, int numSegs, const char *pFormat);
};

} // AttrControls

#endif