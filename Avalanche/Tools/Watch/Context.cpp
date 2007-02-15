//===================================================================
//  File:  Context.cpp
//  Description:
//     Implementation of the Context class.  This handles all the
//     changes to a context that contains a Context.
//===================================================================
#include <stdafx.h>
#include "Context.h"

#include "resource.h"
#include "WatchSlider.h"
#include "Table.h"

//----------------------------------------------------------------------
// Context::~Context
//    Construct.
//----------------------------------------------------------------------
Context::Context(CView* wnd, int id, char* name, CRect& rect)
{
	fDirty = false;
	fId = id;
	strcpy(fName, name);
	fWnd = wnd;
	startRect = rect;

	staticFont.CreatePointFont(80, "MS Sans Serif");
}

//----------------------------------------------------------------------
// Context::~Context
//    Destruct.
//----------------------------------------------------------------------
Context::~Context()
{
	for (int i = 0; i < elements.GetSize(); i++)
		delete elements[i];
}

//----------------------------------------------------------------------
// Context::~Context
//    Show or hide all the context elements.
//----------------------------------------------------------------------
void Context::Show(int cmdShow)
{
	if (fDirty)
		Place();

	for (int i = 0; i < elements.GetSize(); i++)
		elements[i]->Show(cmdShow);

}

//----------------------------------------------------------------------
// Context::RegisterVariable
//    Register a variable with the watch window.  The watch window
//    will create a slider and make it modifiable.
//----------------------------------------------------------------------
void Context::RegisterVariable(VAR_INFO& varinfo)
{
	fDirty = true;
	if (VARTYPE_TABLE == varinfo.type)
	{
		Table* table = new Table(fWnd, &staticFont, varinfo.address);
		elements.Add(table);
	}
	else
	{
		CWatchSlider* slide = new CWatchSlider();
		slide->Create(WS_TABSTOP | TBS_HORZ | TBS_TOP | TBS_LEFT | TBS_NOTICKS | WS_CHILD,
						  CRect(0,0,0,0), fWnd, IDC_SLIDER1);
		slide->Init(fWnd, varinfo.context, &staticFont, varinfo);
		elements.Add(slide);
	}
}

//----------------------------------------------------------------------
// Context::RegisterLabel
//    Register a label for a table.
//----------------------------------------------------------------------
void Context::RegisterLabel(LABEL_INFO& info)
{
	for (int i = 0; i < elements.GetSize(); i++)
	{
		if (*elements[i] == info.table)
		{
			((Table*)elements[i])->RegisterLabel(info);
		}
	}
}

//----------------------------------------------------------------------
// Context::RegisterTableVariable
//    Register a variable type with a table.
//----------------------------------------------------------------------
void Context::RegisterTableVariable(VAR_INFO& info)
{
	for (int i = 0; i < elements.GetSize(); i++)
	{
		if (*elements[i] == info.address)
		{
			fDirty = true;
			((Table*)elements[i])->RegisterTableVariable(info);
		}
	}
}


//----------------------------------------------------------------------
// Context::Place
//    Put the windows in their proper place.
//----------------------------------------------------------------------
void Context::Place()
{
	fDirty = false;

	CRect rect = startRect;
	for (int i = 0; i < elements.GetSize(); i++)
	{
		elements[i]->Move(rect);
		rect.top += elements[i]->Size();
		rect.bottom += elements[i]->Size();
	}

	scrollSize.cx = 10;
	scrollSize.cy = rect.bottom;
}

//----------------------------------------------------------------------
// Context::RefreshVariable
//    We've gotten a message to refresh a variable with a new value.
//----------------------------------------------------------------------
void Context::RefreshVariable(VAR_INFO& var)
{
	for (int i = 0; i < elements.GetSize(); i++)
	{
		if (*elements[i] == var.address)
		{
			elements[i]->Refresh(var.value);
			break;
		}
	}
}

//----------------------------------------------------------------------
// Context::OnSelchangeList
//    One of the list boxes selection has changed.
//----------------------------------------------------------------------
void Context::OnSelchangeList()
{
	for (int i = 0; i < elements.GetSize(); i++)
		elements[i]->OnSelchangeList();
}

//----------------------------------------------------------------------
// Context::Data
//    One of the list boxes selection has changed.
//----------------------------------------------------------------------
void Context::Data(TABLE_DATA& data)
{
	for (int i = 0; i < elements.GetSize(); i++)
		elements[i]->Data(data);
}
