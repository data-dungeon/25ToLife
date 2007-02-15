// ProfStudioTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TupViewer.h"
#include "TupViewerTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TupViewerTreeCtrl

TupViewerTreeCtrl::TupViewerTreeCtrl()
{
	VERIFY(
		m_ImageList.Create(
		IDB_TREECTRL,
		16,
		1,
		RGB(0,255,0)
		)
		);
}

TupViewerTreeCtrl::~TupViewerTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(TupViewerTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(TupViewerTreeCtrl)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TupViewerTreeCtrl message handlers

HTREEITEM TupViewerTreeCtrl::InsertItem(
	LPCTSTR lpszItem,
	int nImage,
	int nExpandedImage,
	HTREEITEM hParent, // = TVI_ROOT
	HTREEITEM hInsertAfter // = TVI_LAST
	)
{
	HTREEITEM hti =
		CTreeCtrl::InsertItem(
		lpszItem,
		nImage,
		nImage,
		hParent,
		hInsertAfter
		);
	ASSERT( hti != NULL );
	VERIFY(
		CTreeCtrl::SetItemData(
		hti,
		DWORD( nExpandedImage )
		)
		);
	return hti;
}

void TupViewerTreeCtrl::Refresh(void)
{
	DeleteAllItems();

	TupperwareAggregate *pRootAgg = theApp.m_pTupRoot;
	if (!pRootAgg)
	return;

	ASSERT( m_ImageList.GetSafeHandle() != NULL );
	SetImageList( &m_ImageList, TVSIL_NORMAL );
	SetImageList( &m_ImageList, TVSIL_STATE );

	for( TupperwareIterator tupIterator = pRootAgg->FindAll(); !tupIterator.Finished(); tupIterator.Next() )
	{
		AddTupData(tupIterator.GetAsAtom(),TVI_ROOT);
	}
}

void TupViewerTreeCtrl::AddTupData(TupperwareAtom *pAtom,HTREEITEM hParent)
{
	TupString name = pAtom->GetKey();
	if (pAtom->GetName())
	{
		name += " - ";
		name += pAtom->GetName();
	}

	switch (pAtom->GetAtomType())
	{
	case TupperwareAtom::ATOM_AGGREGATE:
		{
			TupperwareAggregate *pAgg = (TupperwareAggregate *)pAtom;
			bool bExpanded = false;
			int stateFlags = bExpanded ? TVIS_EXPANDED : 0;
			int stateMask = TVIS_EXPANDED;

			HTREEITEM hItem = InsertItem(name,__IMG_FILE_FOLDER_CLOSE,__IMG_FILE_FOLDER_OPEN,hParent);
			SetItemData(hItem,(DWORD_PTR) pAtom);
			pAtom->SetData((void *)hItem);

			for( TupperwareIterator tupIterator = pAgg->FindAll(); !tupIterator.Finished(); tupIterator.Next() )
			{
				// Check to see what the current atom's key is:
				AddTupData(tupIterator.GetAsAtom(),hItem);
			}
		}
		break;
	case TupperwareAtom::ATOM_LIST:
		{
			HTREEITEM hItem = InsertItem(name,__IMG_SE_FILE_CPP,__IMG_SE_FILE_CPP,hParent);
			SetItemData(hItem,(DWORD_PTR) pAtom);
			pAtom->SetData((void *)hItem);
		}
		break;
	case TupperwareAtom::ATOM_SCALAR:
		{
			HTREEITEM hItem = InsertItem(name,__IMG_SE_FILE_HDR,__IMG_SE_FILE_HDR,hParent);
			SetItemData(hItem,(DWORD_PTR) pAtom);
			pAtom->SetData((void *)hItem);
		}
	}
}

void TupViewerTreeCtrl::UpdateTreeItem(HTREEITEM hItem)
{
	TupperwareAtom *pAtom = (TupperwareAtom *)GetItemData(hItem);
	TupString name = pAtom->GetKey();
	if (pAtom->GetName())
	{
		name += " - ";
		name += pAtom->GetName();
	}
	SetItemText(hItem,name);
}
