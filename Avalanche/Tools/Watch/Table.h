//===================================================================
//  File:  Table.h
//  Description:
//     Declaration of the table class.  This handles all the
//     changes to a context that contains a table.
//===================================================================
#ifndef _fTable
#define _fTable

#include "Context.h"

#define ID_ROW_NEW 201
#define ID_ROW_FILL 202
#define ID_COLUMN_NEW 203
#define ID_COLUMN_FILL 204

class Table : public ContextElement
{
public:
	Table(CView* wnd, CFont* font, void* address);
	~Table();

	// Interface inherited from ContextElement
	void Show(int);
	void Move(CRect&);
	void Refresh(VAR_VAL&) {}
	bool operator==(void* address) {return fAddress == address;}
	int  Size() {return fSize;}
	void OnSelchangeList();
	void OnListButton(int);
	void* GetTable() {return fAddress;}
	void Disable();
	void Clear();

	// Other
	void RegisterLabel(LABEL_INFO&);
	void RegisterTableVariable(VAR_INFO&);
	void Data(TABLE_DATA&);
	void Update();
	void EnableWindows(bool bEnable, bool bKillSelection = false);

private:
	CView* fWnd;
	CListBox fRowCtrl;
	CListBox fColumnCtrl;
	CButton fRowNew, fRowFill, fColumnNew, fColumnFill;
	CStatic fColumnLabel;
	CStatic fRowLabel;

	void* fAddress;
	CFont* fFont;
	CElementArray elements;
	int fSize;
};

inline void Table::Clear()
{
	fRowCtrl.ResetContent();
	fColumnCtrl.ResetContent();
}

#endif // _fTable
