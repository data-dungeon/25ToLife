//===================================================================
//  File:  Table.cpp
//  Description:
//     Implementation of the table class.  This handles all the
//     changes to a context that contains a table.
//===================================================================
#include <stdafx.h>

#include "Table.h"
#include "resource.h"
#include "WatchSlider.h"
#include "watchComm.h"
#include "NameDlg.h"

static const int startLeft = -140;
static const int listHeight = 125;
static const int listWidth = 125;
static const int listSpace = 20;
static const int listHorizSpace = 30;
static const int labelHeight = 12;
static const int labelWidth = 50;
static const int buttonWidth = 50;
static const int buttonHeight = 22;
static const int buttonSpace = 10;
static const int buttonOffset = 20;

//----------------------------------------------------------------------
// Table::Table
//    Constructor
//----------------------------------------------------------------------
Table::Table(CView* wnd, CFont* font, void* address)
	: fWnd(wnd), fAddress(address), fFont(font)
{
	fSize = 100;

	// Window creation and binding.
	fRowCtrl.CreateEx(WS_EX_CLIENTEDGE, "LISTBOX", "", 
							WS_CHILD | WS_VSCROLL | WS_TABSTOP | WS_BORDER | LBS_NOTIFY,
							1,1,2,2, wnd->GetSafeHwnd(), (HMENU)1);
	fColumnCtrl.CreateEx(WS_EX_CLIENTEDGE, "LISTBOX", "", 
								WS_CHILD | WS_VSCROLL | WS_TABSTOP | WS_BORDER | LBS_NOTIFY,
								1,1,2,2, wnd->GetSafeHwnd(), (HMENU)1);
	fColumnLabel.Create("Column", SS_LEFTNOWORDWRAP | WS_CHILD, CRect(0,0,50,12), wnd);
	fRowLabel.Create("Row", SS_LEFTNOWORDWRAP | WS_CHILD, CRect(0,0,50,12), wnd);

	// Buttons
	fRowNew.Create("New", WS_CHILD | WS_TABSTOP,
						CRect(1, 1, 2, 2), wnd, ID_ROW_NEW);
	fRowFill.Create("Fill", WS_CHILD | WS_TABSTOP | WS_DISABLED,
						CRect(1, 1, 2, 2), wnd, ID_ROW_FILL);
	fColumnNew.Create("New", WS_CHILD | WS_TABSTOP,
						CRect(1, 1, 2, 2), wnd, ID_COLUMN_NEW);
	fColumnFill.Create("Fill", WS_CHILD | WS_TABSTOP | WS_DISABLED,
						CRect(1, 1, 2, 2), wnd, ID_COLUMN_FILL);

	// Set the fonts.
	fRowCtrl.SetFont(font);
	fColumnCtrl.SetFont(font);
	fColumnLabel.SetFont(font);
	fRowLabel.SetFont(font);
	// Buttons
	fRowNew.SetFont(font);
	fRowFill.SetFont(font);
	fColumnNew.SetFont(font);
	fColumnFill.SetFont(font);
}

//----------------------------------------------------------------------
// Table::~Table
//    Destruct
//----------------------------------------------------------------------
Table::~Table()
{
	for (int i = 0; i < elements.GetSize(); i++)
		delete elements[i];
}

//----------------------------------------------------------------------
// Table::Show
//    Show the table (when the context is being shown).
//----------------------------------------------------------------------
void Table::Show(int cmdShow)
{
	fRowCtrl.ShowWindow(cmdShow);
	fColumnCtrl.ShowWindow(cmdShow);
	fRowLabel.ShowWindow(cmdShow);
	fColumnLabel.ShowWindow(cmdShow);

	// Buttons
	fRowNew.ShowWindow(cmdShow);
	fRowFill.ShowWindow(cmdShow);
	fColumnNew.ShowWindow(cmdShow);
	fColumnFill.ShowWindow(cmdShow);

	for (int i = 0; i < elements.GetSize(); i++)
		elements[i]->Show(cmdShow);
}

//----------------------------------------------------------------------
// Table::Move
//    Move the sliders, list boxes and static text fields into place.
//----------------------------------------------------------------------
void Table::Move(CRect& rect)
{
	CRect listRect = rect;

	// Row Label
	listRect.left += startLeft;
	listRect.bottom = listRect.top + labelHeight;
	listRect.right = listRect.left + labelWidth;
	fRowLabel.MoveWindow(listRect);

	// Column Label
	listRect.left = listRect.left + listWidth + listHorizSpace;
	listRect.right = listRect.left + labelWidth;
	fColumnLabel.MoveWindow(listRect);

	// Row list
	listRect.left = listRect.left - listWidth - listHorizSpace;
	listRect.top += listSpace;
	listRect.bottom = listRect.top + listHeight;
	listRect.right = listRect.left + listWidth;
	fRowCtrl.MoveWindow(listRect);

	// Row Buttons
	CRect buttonRect;
	buttonRect.left = listRect.left + 8;
	buttonRect.top = listRect.bottom + 5;
	buttonRect.bottom = buttonRect.top + buttonHeight;
	buttonRect.right = buttonRect.left + buttonWidth;
	fRowNew.MoveWindow(buttonRect);
	buttonRect.left = buttonRect.right + buttonSpace;
	buttonRect.right = buttonRect.left + buttonWidth;
	fRowFill.MoveWindow(buttonRect);

	// Column List
	listRect.left = listRect.right + listHorizSpace;
	listRect.right = listRect.left + listWidth;
	fColumnCtrl.MoveWindow(listRect);

	// Column Buttons
	buttonRect.left = listRect.left + 8;
	buttonRect.top = listRect.bottom + 5;
	buttonRect.bottom = buttonRect.top + buttonHeight;
	buttonRect.right = buttonRect.left + buttonWidth;
	fColumnNew.MoveWindow(buttonRect);
	buttonRect.left = buttonRect.right + buttonSpace;
	buttonRect.right = buttonRect.left + buttonWidth;
	fColumnFill.MoveWindow(buttonRect);

	// Sliders
	CRect sliderRect = rect;
	sliderRect.top = listRect.bottom + 50;
	sliderRect.bottom = sliderRect.top + (rect.bottom-rect.top);

	for (int i = 0; i < elements.GetSize(); i++)
	{
		elements[i]->Move(sliderRect);
		sliderRect.top += elements[i]->Size();
		sliderRect.bottom += elements[i]->Size();
	}
	fSize = sliderRect.bottom + listSpace;
}

//----------------------------------------------------------------------
// Table::RegisterLabel
//    Register a table label
//----------------------------------------------------------------------
void Table::RegisterLabel(LABEL_INFO& info)
{
	if (fAddress != info.table)
		return;

	CListBox* box;
	if (info.row)
	{
		box = &fRowCtrl;
	}
	else
	{
		box = &fColumnCtrl;
	}
	CString str;
	str.Format("%d -- %s", info.idx, info.name);
	box->InsertString(info.idx, str);
}

//----------------------------------------------------------------------
// Table::RegisterTableVariable
//    Each cell in a table can have several variables.
//----------------------------------------------------------------------
void Table::RegisterTableVariable(VAR_INFO& info)
{
	if (fAddress != info.address)
		return;

	CWatchSlider* slide = new CWatchSlider();
	slide->Create(WS_TABSTOP | TBS_HORZ | TBS_TOP | TBS_LEFT | TBS_NOTICKS | WS_CHILD,
					  CRect(0,0,0,0), fWnd, IDC_SLIDER1);
	info.value.ival = 0;
	slide->Init(fWnd, info.context, fFont, info);
	slide->SetTable(this);
	slide->Enable(false);
	elements.Add(slide);
}

//----------------------------------------------------------------------
// Table::OnSelchangeList
//    The row or column list item has been changed.  Get the data
//    that represents the new row and column.
//----------------------------------------------------------------------
void Table::OnSelchangeList()
{
	EnableWindows(false);
	
	if (fRowCtrl.GetCurSel() >= 0 && fColumnCtrl.GetCurSel() >= 0)
	{
		WatchComm::Instance()->RequestTableData(fAddress, fRowCtrl.GetCurSel(),
															fColumnCtrl.GetCurSel());
	}
}

//----------------------------------------------------------------------
// Table::Data
//    We've gotten data from the host application.  Put this in the
//    watch stuff.
//----------------------------------------------------------------------
void Table::Data(TABLE_DATA& data)
{
	ASSERT(fRowCtrl.GetCurSel() >= 0 && fColumnCtrl.GetCurSel() >= 0);

	if (fRowCtrl.GetCurSel() != data.row || fColumnCtrl.GetCurSel() != data.column)
		return;

	((CWatchSlider*)elements[data.dataIndex])->Refresh(data.data);
	EnableWindows(true);
}

//----------------------------------------------------------------------
// Table::Update
//    One of the sliders has been slid.  Update the variable in the
//    host application.
//----------------------------------------------------------------------
void Table::Update()
{
	for (int i = 0; i < elements.GetSize(); i++)
	{
		PKT pkt;
		pkt.pktType = PTYPE_TABLE_DATA;
		pkt.tableData.table = fAddress;
		pkt.tableData.row = fRowCtrl.GetCurSel();
		pkt.tableData.column = fColumnCtrl.GetCurSel();
		pkt.tableData.dataIndex = i;
		pkt.tableData.data = ((CWatchSlider*)elements[i])->Variable()->GetVal();
	
		WatchComm::Instance()->SendPacket(&pkt);
	}
}

//----------------------------------------------------------------------
// Table::Disable
//    We've updated the data on the application side.  This is the
//    easy way to refresh that data (make the user request it again).
//----------------------------------------------------------------------
void Table::Disable()
{
	EnableWindows(false, true);
}

void Table::EnableWindows(bool bEnable, bool bKillSelection /*=false*/)
{
	if (bKillSelection)
	{
		fRowCtrl.SetCurSel(-1);
		fColumnCtrl.SetCurSel(-1);
	}

	for (int i = 0; i < elements.GetSize(); i++)
		((CWatchSlider*)elements[i])->Enable(bEnable);
	fRowFill.EnableWindow(bEnable);
	fColumnFill.EnableWindow(bEnable);
}

//----------------------------------------------------------------------
// Table::OnListButton
//    One of the New or Fill buttons has been clicked.  Respond to that.
//----------------------------------------------------------------------
void Table::OnListButton(int btn)
{
	PKT pkt;
	switch(btn)
	{
		case ID_ROW_NEW:
			pkt.pktType = PTYPE_TABLE_ROW_NEW;
			pkt.tableLabel.isRow = 1;
			break;
		case ID_ROW_FILL:
			pkt.pktType = PTYPE_TABLE_ROW_FILL;
			pkt.tableData.isRow = 1;
			break;
		case ID_COLUMN_NEW:
			pkt.pktType = PTYPE_TABLE_COLUMN_NEW;
			pkt.tableLabel.isRow = 0;
			break;
		case ID_COLUMN_FILL:
			pkt.pktType = PTYPE_TABLE_COLUMN_FILL;
			pkt.tableData.isRow = 0;
			break;
	}

	switch(btn)
	{
		case ID_COLUMN_NEW:
		case ID_ROW_NEW:
		{
			CNameDlg dlg;
			if (dlg.DoModal() == IDOK)
			{
				pkt.tableLabel.table = fAddress;
				strncpy(pkt.tableLabel.name, dlg.m_name, TABLE_LABEL_SIZE);
				WatchComm::Instance()->SendPacket(&pkt);
			}
			break;
		}
		case ID_ROW_FILL:
		case ID_COLUMN_FILL:
		{
			CString label;
			if (pkt.tableData.isRow == 1)
				fRowCtrl.GetText(fRowCtrl.GetCurSel(), label);
			else
				fColumnCtrl.GetText(fColumnCtrl.GetCurSel(), label);
			label = label.Right(label.GetLength()-5);
			CString str;
			str.Format("Fill all values for \'%s\' to the current values?", label);
			if (IDOK == AfxMessageBox(str, MB_OKCANCEL))
			{
				for (int i = 0; i < elements.GetSize(); i++)
				{
					pkt.tableData.table = fAddress;
					pkt.tableData.row = fRowCtrl.GetCurSel();
					pkt.tableData.column = fColumnCtrl.GetCurSel();
					pkt.tableData.dataIndex = i;
					pkt.tableData.data = ((CWatchSlider*)elements[i])->Variable()->GetVal();
					WatchComm::Instance()->SendPacket(&pkt);
				}
			}
			break;
		}
	}
}
