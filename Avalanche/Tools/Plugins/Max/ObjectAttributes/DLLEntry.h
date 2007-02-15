#ifndef DLL_ENTRY_H
#define DLL_ENTRY_H

//------------------------------------------------------------------------
// ObjAttribWin32App
// See objAttribWin32.cpp for the implementation of this class
//
class ObjectAttributesApp : public CWinApp
{
public:
	ObjectAttributesApp();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ObjectAttributesApp)
public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(ObjectAttributesApp)
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//------------------------------------------------------------------------

extern ObjectAttributesApp theApp;

#endif // DLL_ENTRY_H