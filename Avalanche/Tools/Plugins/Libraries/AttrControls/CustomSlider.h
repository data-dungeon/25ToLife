//
//	CustomSlider
//	

/*
 * $History: CustomSlider.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 11/26/02   Time: 9:58a
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * added new controls to numeric type.
 * Fixed color issues with Max
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 11/22/02   Time: 1:18p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * all compiling
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/21/02   Time: 4:07p
 * Updated in $/Avalanche/tools/Plugins/Libraries/AttrControls
 * spinner, sliders, interfaces, and implemtations
 */

#ifndef ATTR_CONTROLS_CUSTOM_SLIDER_H
#define ATTR_CONTROLS_CUSTOM_SLIDER_H

#ifndef ATTR_CONTROLS_CUSTOM_CONTROL_H
#include "CustomControl.h"
#endif

#ifndef ATTR_CONTROLS_ICUSTOM_SLIDER_H
#include "ICustomSlider.h"
#endif

namespace AttrControls 
{

class CustomEditBox;

class CustomSlider : public CustomControl
{
public:
	CustomSlider( HWND hWnd, float value=0.0f, int numSegs=10);
	~CustomSlider();

// ICustomSlider methods
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

	// other methods
	void UpdateEdit(void);
	BOOL IsValidCharacter(int ch);
	BOOL UpdateFromEdit(void);
	void CalcKnobRect(RECT &knobRect, float val);
	int XFromVal(float value);
	int XFromPercent(float percent);
	float ValueFromX(int x);
	float PercentFromX(int x);
	float ValueFromPercent(float percent);
	void LimitValue(void);

	// window messages
	LRESULT OnPaint(void);
	LRESULT OnCommand( WORD notify, WORD id, HWND hCtrl );
	LRESULT OnTimer( UINT nIDEvent );
	LRESULT OnRButDown( int x, int y, WORD keys );
	LRESULT OnLButDown( int x, int y, WORD keys );
	LRESULT OnMouseMove( int x, int y, WORD keys );
	LRESULT OnLButUp( int x, int y, WORD keys);
	LRESULT OnWMEnable( BOOL bEnabled );



	// member variables
	float m_value;
	int m_numSegs;
	BOOL m_bDown;
	HWND m_hEdit;
	ICustomControl::EditSpinnerType m_type;
	float m_min;
	float m_max;
	float m_range;
	BOOL m_bIndeterminate;
	float m_oldVal;
	float m_resetVal;
	BOOL m_bKeyBrackets;
	BOOL m_bNeedNotify;
	RECT m_rect;
	int m_radius;
	TupString m_sliderFormat;
	BOOL m_bDragNotify;
};



} // AttrControls

#endif