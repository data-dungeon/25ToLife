//
//	CustomSpinner
//	

/*
 * $History: CustomSpinner.h $
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

#ifndef ATTR_CONTROLS_CUSTOM_SPINNER_H
#define ATTR_CONTROLS_CUSTOM_SPINNER_H

#ifndef ATTR_CONTROLS_CUSTOM_CONTROL_H
#include "CustomControl.h"
#endif

#ifndef ATTR_CONTROLS_ICUSTOM_SPINNER_H
#include "ICustomSpinner.h"
#endif

#define SPINMOVE_THRESH		4
#define SPINNER_REPEATTIME	50
#define SPINNER_DELAYTIME	500
#define AUTOSCALE_FACTOR	0.01f

namespace AttrControls 
{

class CustomEditBox;

class CustomSpinner : public CustomControl
{
public:
	CustomSpinner( HWND hWnd, float value=0.0f, float scale=0.1f);
	~CustomSpinner();
	
// ICustomSpinner methods
	float GetFVal(void);
	int GetIVal(void);
	void SetAutoScale(BOOL on=TRUE);
	void SetScale( float s );
	void SetValue( float v, BOOL bNotify );
	void SetValue( int v, BOOL bNotify );
	void SetLimits( int min, int max, BOOL bLimitCurValue = TRUE );
	void SetLimits( float min, float max, BOOL bLimitCurValue = TRUE );
	void LinkToEdit( HWND hEdit, ICustomControl::EditSpinnerType type );
	void SetIndeterminate(BOOL i=TRUE);
	BOOL IsIndeterminate(void);
	void SetResetValue(float v);
	void SetResetValue(int v);
	void SetKeyBrackets(BOOL onOff);
	void SetSnapSpinner(BOOL b);
	BOOL GetSnapSpinner(void);
	void SetSnapSpinValue(float f);
	float GetSnapSpinValue(void);
	void SetSpinnerFormat(const char *pFormat);
	const char *GetSpinnerFormat(void);
	void SetSpinnerWrap(BOOL b);
	BOOL GetSpinnerWrap(void);
	void SetDragNotify(BOOL b);
	BOOL GetDragNotify(void);

// other methods
	void UpdateEdit(void);
	BOOL IsValidCharacter(int ch);
	BOOL UpdateFromEdit(void);
	void LimitValue(void);
	void Increment(void);
	void Decrement(void);

// messages
	LRESULT OnPaint(void);
	LRESULT OnCommand( WORD notify, WORD id, HWND hCtrl );
	LRESULT OnTimer( UINT nIDEvent );
	LRESULT OnRButDown( int x, int y, WORD keys );
	LRESULT OnLButDown( int x, int y, WORD keys );
	LRESULT OnMouseMove( int x, int y, WORD keys );
	LRESULT OnLButUp( int x, int y, WORD keys );
	LRESULT OnCaptureReleased( HWND hwnd );
	LRESULT OnWMEnable( BOOL enabled );

// member variables
	float m_value;
	float m_scale;
	float m_min;
	float m_max;

	BOOL m_bDown;
	HWND m_hEdit;
	ICustomSpinner::EditSpinnerType m_type;

	enum ButtonStatus
	{
		BUT_OUT,
		BUT_IN,
		TOPBUT_IN,
		BOTBUT_IN,
	};

	ButtonStatus m_buttonStatus;
	BOOL m_bIndeterminate;
	BOOL m_bAutoScale;
	float m_reset;
	BOOL m_bKeyBrackets;
	BOOL m_bNeedNotify;
	BOOL m_bSnapSpinner;
	float m_spinSnapValue;
	TupString m_spinnerFormat;
	BOOL m_bSpinWrap;
	BOOL m_bDragNotify;

	int m_originalX;
	int m_originalY;
	float m_originalValue;


};



} // AttrControls

#endif
