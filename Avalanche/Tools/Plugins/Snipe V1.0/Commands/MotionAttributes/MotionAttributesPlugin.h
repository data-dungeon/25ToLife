#pragma once

#define __OBJECT_ATTRIBUTES_PLUGIN_H

class MotionAttributesPluginApp : public CWinApp
{
public:
	MotionAttributesPluginApp();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MotionAttributesPluginApp)
public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(MotionAttributesPluginApp)
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

extern MotionAttributesPluginApp theApp;

extern HINSTANCE hInstance; // global HINSTANCE

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
