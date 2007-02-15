// WatchSlider.cpp : implementation file
//

#include "stdafx.h"
#include "watch.h"
#include "WatchSlider.h"
#include "watchcomm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWatchSlider

#define SLIDER_MIN 0
#define SLIDER_MAX 100
#define SLIDER_RANGE 100

CWatchSlider::CWatchSlider()
	: fTable(0)
{
	variable = NULL;
}

CWatchSlider::~CWatchSlider()
{
	if (variable)
		delete variable;
}

bool CWatchSlider::Init(CWnd* view, int t, CFont* font, VAR_INFO& vi)
{
	// Are we string only?
	textOnly = (vi.type == VARTYPE_STRING);
	if (textOnly)
		ModifyStyle(WS_TABSTOP, 0, 0);

	variable = WatchVar::GetInstance(vi);
	tab = t;
	namectrl.Create(variable->Name(), SS_LEFTNOWORDWRAP | WS_CHILD, CRect(0,0,50,12), view);
	namectrl.SetFont(font);
	// This looks like an editable static text string.
	valctrl.IsTextOnly(textOnly);
	if (textOnly)
	{
	  	valctrl.CreateEx(WS_EX_CLIENTEDGE, "EDIT", "",
	  						  WS_BORDER | ES_AUTOHSCROLL | ES_LEFT | ES_OEMCONVERT | WS_TABSTOP | WS_CHILD,
	  						  0, 0, 50, 12, view->GetSafeHwnd(), (HMENU)1);
		valctrl.LimitText(VARTYPE_STRING_SIZE - 1);
	}
	else
  		valctrl.CreateEx(0, "EDIT", "", ES_AUTOHSCROLL | ES_LEFT | WS_CHILD,
  								0, 0, 50, 12, view->GetSafeHwnd(), (HMENU)1);
	valctrl.SetFont(font);
	valctrl.SetWindowText(variable->String());
	valctrl.Slider(this);
	SetRange(SLIDER_MIN, SLIDER_MAX);
	SetPos(int(SLIDER_RANGE*variable->Percent()));

	return true;
}

void CWatchSlider::MoveWindow(LPRECT lprect)
{
	RECT rect = *lprect;

	if (!textOnly)
	{
		CSliderCtrl::MoveWindow(lprect);

		rect.left -= 155;
		rect.right = rect.left + 150;
		namectrl.MoveWindow(&rect);
		rect.left = lprect->right + 10;
		rect.right = rect.left + 75;
		// This is best when we have a border.
		//	rect.top -= 3;
		//	rect.bottom += 3;
		valctrl.MoveWindow(&rect);
	}
	else
	{
		CSliderCtrl::ShowWindow(SW_HIDE);
		rect.left -= 155;
		rect.right = rect.left + 150;
		namectrl.MoveWindow(&rect);
		rect.left = rect.right + 10;
		rect.right = lprect->right + 65;
		// This is best when we have a border.
		rect.top -= 3;
		rect.bottom += 3;
		valctrl.MoveWindow(&rect);
	}
}

void CWatchSlider::ShowWindow(int nCmdShow)
{
	CSliderCtrl::ShowWindow(nCmdShow);
	namectrl.ShowWindow(nCmdShow);
	valctrl.ShowWindow(nCmdShow);
}

void CWatchSlider::UpdateVar()
{
	if (!textOnly)
	{
		TRACE("Pos: %d\n", GetPos());
		variable->Update((float)GetPos()/SLIDER_RANGE);
		valctrl.SetWindowText(variable->String());
	}
	SendToApp();
}

void CWatchSlider::SendToApp()
{
	if (!fTable)
	{
		WatchComm::Instance()->ChangeVariable(variable->GetPtr(), variable->GetVal(),
														 variable->Event(), variable->EventDest(), variable->Type());
	}
	else
	{
		fTable->Update();
	}
}

void CWatchSlider::UpdateVar(VAR_VAL& val)
{
	variable->SetVal(val);
	UpdateSlider();
}

void CWatchSlider::UpdateSlider()
{
	valctrl.SetWindowText(variable->String());
	SetPos(int(SLIDER_RANGE*variable->Percent()));
}

void CWatchSlider::Enable(bool enable /*=true*/)
{
	namectrl.EnableWindow(enable);
	valctrl.EnableWindow(enable);
	this->EnableWindow(enable);
}

void CWatchSlider::EditOnChange(const CString& str)
{
	variable->String(str);
	if (!textOnly)
		this->SetPos(int(SLIDER_RANGE*variable->Percent()));
	this->SendToApp();
}


BEGIN_MESSAGE_MAP(CWatchSlider, CSliderCtrl)
	//{{AFX_MSG_MAP(CWatchSlider)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWatchSlider message handlers

