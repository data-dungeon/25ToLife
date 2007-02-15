//===================================================================
//  File:  Context.h
//  Description:
//     Declaration of the Context class.  This handles all the
//     changes to a context that contains a Context.
//===================================================================
#ifndef _fContext
#define _fContext

#include "WatchInfo.h"

class ContextElement
{
public:
	virtual ~ContextElement() {};
	virtual void Show(int) = 0;
	virtual void Move(CRect&) = 0;
	virtual void Refresh(VAR_VAL&) = 0;
	virtual bool operator==(void*) = 0;
	virtual int  Size() = 0;
	virtual void OnSelchangeList() {};
	virtual void OnListButton(int) {};
	virtual void Data(TABLE_DATA&) {};
	virtual void* GetTable() {return 0;}
	virtual void Disable() {};
	virtual void Clear() {};
};

class Context;

typedef CArray<ContextElement*, ContextElement*> CElementArray;
typedef CArray<Context*, Context*> ContextArray;

class Context
{
public:
	Context(CView* wnd, int id, char* name, CRect& rect);
	~Context();

	void Show(int);
	void Place();
	CSize& ScrollSize() {return scrollSize;}

	// Info
	char* Name() {return fName;}
	int Id() {return fId;}

	void RegisterVariable(VAR_INFO& varinfo);
	void RefreshVariable(VAR_INFO& var);
	void RegisterLabel(LABEL_INFO& info);
	void RegisterTableVariable(VAR_INFO&);

	void OnSelchangeList();
	void OnListButton(int);
	void Data(TABLE_DATA&);
	void* GetTable();
	void Disable();
	void Clear();

private:
	int fId;
	char fName[256];
	CView* fWnd;

	CElementArray elements;
	CFont staticFont;
	CRect startRect;

	bool fDirty;
	CSize scrollSize;
};

inline void* Context::GetTable()
{
	ASSERT(elements.GetSize() > 0);
	return elements[0]->GetTable();
}

inline void Context::Disable()
{
	for (int i = 0; i < elements.GetSize(); i++)
		elements[i]->Disable();
}

inline void Context::Clear()
{
	for (int i = 0; i < elements.GetSize(); i++)
		elements[i]->Clear();
}

inline void Context::OnListButton(int idx)
{
	for (int i = 0; i < elements.GetSize(); i++)
		elements[i]->OnListButton(idx);
}

#endif // _fContext
