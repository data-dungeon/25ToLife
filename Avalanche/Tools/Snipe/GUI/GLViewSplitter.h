
#pragma once

#include "GLControls.h"

#define DEFAULTHANDLESIZE 4

class CGLSplitterControl : public CGLControl
{
	DECLARE_SERIAL(CGLSplitterControl)
public:
   static GLImageList m_glImageListKnob;

   CRuntimeClass   *m_pDynamicControlClass;
   CArray<float>    m_colwidtharray;
   CArray<float>    m_rowheightarray;
   int              m_nHandleSize;
   UINT             m_bFixedSize : 1;

            CGLSplitterControl();
            CGLSplitterControl( CRuntimeClass *pDynamicControlClass, int nHandleSize=DEFAULTHANDLESIZE );
            CGLSplitterControl( CGLControl *pStarterCell, int nInitialWidth, int nInitialHeight, int nHandleSize=DEFAULTHANDLESIZE );
   virtual ~CGLSplitterControl();

   void SetHandleSize( int nHandleSize )
   {
      m_nHandleSize = nHandleSize;
   }
   int GetColumnCount()
   {
      return m_colwidtharray.GetCount();
   }
   int GetRowCount()
   {
      return m_rowheightarray.GetCount();
   }
   float GetColumnWidth( int nIndex )
   {
      return m_colwidtharray[nIndex];
   }
   float GetRowHeight( int nIndex )
   {
      return m_rowheightarray[nIndex];
   }
   void SetSize( int nCols, int nRows );
   void AddColumn();
   void AddRow();
   void RemoveColumn();
   void RemoveRow();
   void SetAt( int nCol, int nRow, CGLControl *pPane )
   {
      if (nCol >= m_colwidtharray.GetCount() || nRow >= m_rowheightarray.GetCount())
         return;
      m_childcontrolarray.SetAt( nRow * m_colwidtharray.GetCount() + nCol, pPane );
   }
   CGLControl *GetAt( int nCol, int nRow )
   {
      return m_childcontrolarray.GetAt( nRow * m_colwidtharray.GetCount() + nCol );
   }
   BOOL HitTest( const CPoint &point, int &nColHit, int &nRowHit, bool bSetTopLeft = false ); // not the virtual one

   DECLARE_MESSAGE_MAP()
   afx_msg void OnSplitterAddHorizontalSplit();
   afx_msg void OnSplitterAddVerticalSplit();
   afx_msg void OnSplitterRemoveHorizontalSplit();
   afx_msg void OnSplitterRemoveVerticalSplit();

public:
// CGLControl overrides
   virtual void PostInit();
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
   virtual BOOL OnLButtonDown( UINT nFlags, CPoint point );
   virtual BOOL OnLButtonUp( UINT nFlags, CPoint point );
   virtual BOOL OnMouseMove( UINT nFlags, CPoint point );
   virtual BOOL OnSetCursor( UINT message, CPoint point );
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL ParseData( BaseStream &bs, const DataHeader &dataheader );
};

