// OptionTreeListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OptionTreeListDlg.h"
#include "GridCellNumeric.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OptionTreeListDlg dialog

OptionTreeListDlg::OptionTreeListDlg(CWnd* pParent /*=NULL*/) :
	CDialog(OptionTreeListDlg::IDD, pParent),
	m_pValues(NULL)
{
	m_OldSize = CSize(-1,-1);

	//{{AFX_DATA_INIT(OptionTreeListDlg)
	//}}AFX_DATA_INIT
}

void OptionTreeListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OptionTreeListDlg)
	DDX_Control(pDX, IDC_GRID, m_grid);             // associate the grid window with a C++ object
	DDX_Control(pDX, IDC_NUM_COLUMNS_COMBO, m_numColumnsCombo);
	DDX_Control(pDX, IDC_NUM_COLUMNS_STATIC, m_numColumnsStatic);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(OptionTreeListDlg, CDialog)
	//{{AFX_MSG_MAP(OptionTreeListDlg)
	ON_WM_SIZE()
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnGridCellChanged)
	ON_CBN_SELCHANGE(IDC_NUM_COLUMNS_COMBO, OnCbnSelchangeNumColumnsCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OptionTreeListDlg message handlers

BOOL OptionTreeListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

//	SetIcon(m_hIcon, TRUE);			// Set big icon
//	SetIcon(m_hIcon, FALSE);		// Set small icon

	CRect rect;
	GetClientRect(rect);
	m_OldSize = CSize(rect.Width(), rect.Height());

	/////////////////////////////////////////////////////////////////////////
	// initialise grid properties
	/////////////////////////////////////////////////////////////////////////

	m_grid.EnableDragAndDrop(FALSE);
	m_grid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));

	m_grid.SetFixedColumnSelection(FALSE);
	m_grid.SetFixedRowSelection(FALSE);
	m_grid.EnableColumnHide();

	m_grid.AutoSize();

	m_grid.SetCompareFunction(CGridCtrl::pfnCellNumericCompare);

	m_grid.SetAutoSizeStyle();

	m_numColumnsCombo.SetCurSel(m_nCols-1);
	if (!m_bAdjustColumns)
	{
		m_numColumnsStatic.EnableWindow(FALSE);
		m_numColumnsCombo.EnableWindow(FALSE);
	}

	PopulateGrid();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void OptionTreeListDlg::PopulateGrid(void)
{
	m_grid.DeleteAllItems();
	int numElements = m_pValues->GetSize();
	int numRows = numElements/m_nCols;
	if (numRows*m_nCols<numElements)
		numRows++;

	TRY 
	{
		m_grid.SetRowCount(numRows+1);
		m_grid.SetColumnCount(m_nCols+1);
		m_grid.SetFixedRowCount(1);
		m_grid.SetFixedColumnCount(1);
	}
	CATCH (CMemoryException, e)
	{
		e->ReportError();
		return;
	}
	END_CATCH

	// fill rows/cols with text
	GV_ITEM item;
	item.mask = GVIF_TEXT;
	item.row = 0;
	item.col = 0;
	item.strText = "";
	m_grid.SetItem(&item);
	m_grid.SetColumnWidth(0,30);

	for (int row = 1; row < m_grid.GetRowCount(); row++)
	{
		item.mask = GVIF_TEXT | GVIF_FORMAT;
		item.row = row;
		item.col = 0;
		item.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX | DT_END_ELLIPSIS | DT_CENTER;
		item.strText.Format("%d",row-1);
		m_grid.SetItem(&item);
	}
	for (int col = 1; col < m_grid.GetColumnCount(); col++)
	{ 
		item.mask = GVIF_TEXT | GVIF_FORMAT;
		item.row = 0;
		item.col = col;
		item.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX | DT_END_ELLIPSIS | DT_CENTER;
		if (col-1<27)
		{
			item.strText.Format("%c",'A'+(BYTE(col-1)));
		}
		else
		{
			BYTE first = (BYTE)((col-1)/26);
			BYTE second = (BYTE)((col-1)%26);
			item.strText.Format("%c%c",'A'+first,'A'+second);
		}
		m_grid.SetItem(&item);
	}

	int index = 0;
	for (int row = 1; row < m_grid.GetRowCount(); row++)
	{
		for (int col = 1; col < m_grid.GetColumnCount(); col++)
		{ 
			if (m_bNumeric)
			{
				m_grid.SetCellType(row,col, RUNTIME_CLASS(CGridCellNumeric));
				CGridCellNumeric *pGridCell = (CGridCellNumeric *) m_grid.GetCell(row,col);
				pGridCell->SetSignificantDigits(m_significantDigits);
				pGridCell->SetAllowNegativeValues(m_bAllowNegativeValues);
			}

			CString str;

			GV_ITEM Item;

			item.mask = GVIF_TEXT;
			item.row = row;
			item.col = col;
			if (index<numElements)
			{
				item.strText = (*m_pValues)[index];
			}
			else
			{
				item.mask = GVIF_TEXT | GVIF_STATE;
				item.strText = "";
				item.nState |= GVIS_READONLY;
			}

			m_grid.SetItem(&item);
			index++;
		}
	}
}

void OptionTreeListDlg::SetNumColumns(int numColumns)
{
	m_nCols = numColumns;
}

void OptionTreeListDlg::SetAdjustableColumns(bool bAdjustColumns)
{
	m_bAdjustColumns = bAdjustColumns;
}

void OptionTreeListDlg::SetNumeric(bool bNumeric)
{
	m_bNumeric = bNumeric;
}

void OptionTreeListDlg::SetSignificantDigits(int significantDigits)
{
	m_significantDigits = significantDigits;
}

void OptionTreeListDlg::SetAllowNegativeValues(bool bAllowNegativeValues)
{
	m_bAllowNegativeValues = bAllowNegativeValues;
}

void OptionTreeListDlg::SetStringData(CStringArray *values)
{
	m_pValues = values;
}

void OptionTreeListDlg::OnCbnSelchangeNumColumnsCombo()
{
	m_nCols = m_numColumnsCombo.GetCurSel()+1;
	PopulateGrid();
}

void OptionTreeListDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	const int GRID_RIGHT = 100;
	const int GRID_BOTTOM = 12;
	const int OK_BUTTON_RIGHT = 10;
	const int CANCEL_BUTTON_RIGHT = 10;
	const int NUM_COLUMNS_STATIC_RIGHT = 22;
	const int NUM_COLUMNS_COMBO_RIGHT = 10;

	CRect rect;
	GetClientRect(&rect);
	// grid
	if (::IsWindow(m_grid.m_hWnd))
	{
		CRect myRect;
		m_grid.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_grid.MoveWindow(myRect.left,myRect.top,(rect.right-GRID_RIGHT)-myRect.left,(rect.bottom-GRID_BOTTOM)-myRect.top);
	}
	// ok Button
	CWnd *pCWnd = GetDlgItem(IDOK);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		CRect myRect;
		pCWnd->GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		pCWnd->MoveWindow(rect.right-(OK_BUTTON_RIGHT+myRect.Width()),myRect.top,myRect.Width(),myRect.Height());
	}
	// cancel Button
	pCWnd = GetDlgItem(IDCANCEL);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		CRect myRect;
		pCWnd->GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		pCWnd->MoveWindow(rect.right-(CANCEL_BUTTON_RIGHT+myRect.Width()),myRect.top,myRect.Width(),myRect.Height());
	}
	// num columns static
	if (::IsWindow(m_numColumnsStatic.m_hWnd))
	{
		CRect myRect;
		m_numColumnsStatic.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_numColumnsStatic.MoveWindow(rect.right-(NUM_COLUMNS_STATIC_RIGHT+myRect.Width()),myRect.top,myRect.Width(),myRect.Height());
	}
	// num columns static
	if (::IsWindow(m_numColumnsCombo.m_hWnd))
	{
		CRect myRect;
		m_numColumnsCombo.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_numColumnsCombo.MoveWindow(rect.right-(NUM_COLUMNS_COMBO_RIGHT+myRect.Width()),myRect.top,myRect.Width(),myRect.Height());
	}
}

void OptionTreeListDlg::OnGridCellChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	// Declare variables
	NM_GRIDVIEW *pGridView = (NM_GRIDVIEW*) pNotifyStruct;
	int row = pGridView->iRow;
	int column = pGridView->iColumn;
	if (row>0 && column>0)
	{
		CGridCellBase *pGridCell = m_grid.GetCell(row,column);
		if (pGridCell)
		{
			CString text = pGridCell->GetText();

			int listIndex = ((row-1)*m_nCols)+column-1;
			if (listIndex>=0&& listIndex<m_pValues->GetSize())
			{
				if (strcmp(text,(*m_pValues)[listIndex]))
				{
					(*m_pValues)[listIndex] = text;
				}
			}
		}
	}
}

BOOL OptionTreeListDlg::PreTranslateMessage(MSG* pMsg)
{
//	if(pMsg->message == WM_KEYDOWN)
//	{
//		if(pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
//		{
//			return TRUE;
//		}
//	}
	return CDialog::PreTranslateMessage(pMsg);
}
