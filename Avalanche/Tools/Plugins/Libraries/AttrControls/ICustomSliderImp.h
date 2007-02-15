//
//	ICustomSpinnerImp
//	

/*
 * $History: ICustomSliderImp.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/21/02   Time: 4:07p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * spinner, sliders, interfaces, and implemtations
 */

#ifndef ATTR_CONTROLS_ICUSTOM_SLIDER_IMP_H
#define ATTR_CONTROLS_ICUSTOM_SLIDER_IMP_H

#ifndef ATTR_CONTROLS_ICUSTOM_SLIDER_H
#include "ICustomSlider.h"
#endif

namespace AttrControls 
{

class CustomSlider;

class ICustomSliderImp : public ICustomSlider
{
	CustomSlider *m_pControl;
public:
	ICustomSliderImp(CustomSlider *pControl);

	// ICustomControl
	HWND GetHWnd(void);
	void Enable(BOOL onOff=TRUE);
	void Disable(void);
	BOOL IsEnabled(void);

	// ICustomSliderImp
	float GetFVal(void);
	int GetIVal(void);
	void SetNumSegs( int num );
	void SetValue( float v, int notify );
	void SetValue( int v, int notify );
	void SetLimits( int min, int max, int limitCurValue = TRUE );
	void SetLimits( float min, float max, int limitCurValue = TRUE );
	void LinkToEdit( HWND hEdit, ICustomControl::EditSpinnerType type );
	void SetIndeterminate(BOOL i=TRUE);
	BOOL IsIndeterminate();
	void SetResetValue(float v);
	void SetResetValue(int v);
	void SetKeyBrackets(BOOL onOff);
	void SetDragNotify(BOOL onOff);
	BOOL GetDragNotify(void);
	void SetSliderFormat(const char *pFormat);
	const char *GetSliderFormat(void);
};

} // AttrControls

#endif