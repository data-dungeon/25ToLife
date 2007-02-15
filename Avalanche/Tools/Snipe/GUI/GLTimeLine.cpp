//

#include "stdafx.h"
#include "Interface/MainInterface.h"
#include "GLTimeLine.h"
#include "Properties/MotionCurveDriver.h"
#include "Interface/SnipeNavigator.h"
#include "resource.h"

#define TRANSPORTHEIGHT 36

////////////////////////////////////
// CTimeLineDoc

IMPLEMENT_DYNCREATE(CTimeLineDoc, CBaseDoc)

BEGIN_MESSAGE_MAP(CTimeLineDoc, CBaseDoc)
END_MESSAGE_MAP()

CTimeLineDoc::CTimeLineDoc()
{
   m_SelectableObjectTypeArray.Add( SOT_MotionKey );

//   POSITION pos = GetApp().GetDocList().Find( this );
//   if (pos)
//      GetApp().GetDocList().RemoveAt( pos );
}

CTimeLineDoc::~CTimeLineDoc()
{
}

HierObject *CTimeLineDoc::GetCommandObject() const
{
   if (m_pObjectHead)
      return (HierObject *)m_pObjectHead->GetCommandObject();
   else
      return NULL;
}

void CTimeLineDoc::SetObjectHead( HierObject *pObjectHead )
{
   CBaseDoc::SetObjectHead( pObjectHead );

   for (POSITION pos=m_viewlist.GetHeadPosition(); pos; )
   {
      CGLTimeLineView *pView = (CGLTimeLineView *)m_viewlist.GetNext( pos );
      if (pView->m_bAutoZoomFit)
         pView->ZoomFitAll();
   }
}

////////////////////////////////////
// CGLTimeLineView

IMPLEMENT_DYNAMIC(CGLTimeLineView, CGL2DView)

BEGIN_MESSAGE_MAP(CGLTimeLineView, CGL2DView)
END_MESSAGE_MAP()

BoolProperty::Info CGLTimeLineView::m_bAutoZoomFitInfo( "Auto Zoom Fit", "AutoZoomFit", TRUE );

CGLTimeLineView::CGLTimeLineView()
{
   m_bAutoZoomFit.Init( &m_bAutoZoomFitInfo, this );

   m_bDisplayFrameRate.StoreValue( FALSE );

   m_pDocument = &m_timelinedoc;
   m_pDocument->AddView( this );
}

CGLTimeLineView::~CGLTimeLineView()
{
   MainInterface::m_pCurrentView = NULL;
}

void CGLTimeLineView::DrawContents()
{
   SetProjectionMatrix();
   glPushMatrix();
//   glScalef( 100.0f, 100.0f, 1.0f );

   if (!m_bDoPick && GetManipulatorInstance())
      GetManipulatorInstance()->PreDraw(this);

   if (!m_bDoPick && GetNavigatorInstance())
      GetNavigatorInstance()->PreDraw(this);

   if (!m_bDoPick)
   {
      float fFocusDistance = m_pCurCamera->m_fFocusDistance;
      float fFOV = m_pCurCamera->m_fFOV * DEGTORAD;
      float fVerticalSpan = fFocusDistance * (float)(tan(fFOV/2.0f));
      float fControlRatio = (float)m_szControl.cx / (float)m_szControl.cy;
      float fHorizontalSpan = fVerticalSpan * m_pCurCamera->m_fAspectRatio * fControlRatio;
      float fTargetX = m_pCurCamera->m_transform.m_vTranslate.GetValue().x;
      float fTargetY = m_pCurCamera->m_transform.m_vTranslate.GetValue().y;
      float fMinExtentY = fTargetY-fVerticalSpan;
      float fMaxExtentY = fTargetY+fVerticalSpan;
      float fMinExtentX = fTargetX-fHorizontalSpan;
      float fMaxExtentX = fTargetX+fHorizontalSpan;

      float verticalGridMin = abs((fVerticalSpan*120.0)/(float)m_szControl.cy);
      float verticalGridUnit = 1.0f;
      if (verticalGridMin < 1.0f)
      {
         while (verticalGridUnit>verticalGridMin)
         {
            verticalGridUnit /= 2.0f;
         }
      }
      else if (verticalGridMin > 1.0f)
      {
         while (verticalGridUnit*2.0<verticalGridMin)
         {
            verticalGridUnit *= 2.0f;
         }
      }
      float startGridY = ((int)(fMinExtentY/verticalGridUnit))*verticalGridUnit;
      float endGridY = ((int)(fMaxExtentY/verticalGridUnit))*verticalGridUnit;

      float horizontalGridMin = abs((fHorizontalSpan*250.0)/(float)m_szControl.cx);
      float horizontalGridUnit = 1.0f;
      if (horizontalGridMin < 1.0f)
      {
         while (horizontalGridUnit>horizontalGridMin)
         {
            horizontalGridUnit /= 2.0f;
         }
      }
      else if (horizontalGridMin > 1.0f)
      {
         while (horizontalGridUnit*2.0<horizontalGridMin)
         {
            horizontalGridUnit *= 2.0f;
         }
      }
      float startGridX = ((int)(fMinExtentX/horizontalGridUnit))*horizontalGridUnit;
      float endGridX = ((int)(fMaxExtentX/horizontalGridUnit))*horizontalGridUnit;


      // draw grid
      glLineWidth( 1.0f );
      glColor4f( 1,1,1,0.1f );

      float x,y;
      glBegin( GL_LINES );
         for (y=startGridY; y<=endGridY; y+=verticalGridUnit)
         {
            glVertex2f( fMinExtentX, y );
            glVertex2f( fMaxExtentX, y );
         }
         for (x=startGridX; x<=endGridX; x+=horizontalGridUnit)
         {
            glVertex2f( x, fMinExtentY );
            glVertex2f( x, fMaxExtentY );
         }
      glEnd();
      glLineWidth( 3.0f );
      glBegin( GL_LINES );
      glVertex2f( fMinExtentX, 0 );
      glVertex2f( fMaxExtentX, 0 );
      glVertex2f( 0, fMinExtentY );
      glVertex2f( 0, fMaxExtentY );
      glEnd();

      glColor4f( 0,0,0,1.0f );
      char text[32];
      float textYRatio = (float)m_szControl.cy/(fMaxExtentY-fMinExtentY);
      for (y=startGridY; y<=endGridY; y+=verticalGridUnit)
      {
         float newYPos = ((float)m_szControl.cy-((y-fMinExtentY))*textYRatio) + 4;
         if (newYPos < (float)m_szControl.cy-15)
         {
            sprintf( text, "%g", y );
            DrawText( 6,newYPos, text );
         }
      }
      float textXRatio = (float)m_szControl.cx/(fMaxExtentX-fMinExtentX);
      for (x=startGridX; x<=endGridX; x+=horizontalGridUnit)
      {
         Time xTime((double)x);
         String strTime = xTime.ToString( GetFPS(), GetTimeUnits() );
         int numChars = strTime.Length();
         float newXPos = ((x-fMinExtentX))*textXRatio-(numChars*3);
         DrawText( newXPos, m_szControl.cy - 6, strTime );
      }
   }

   glLineWidth( 2.0f );
   glPointSize( 5.0f );

   MotionCurveDriver *pMotionCurveDriver = GetDocument()->GetMotionCurveDriver();
   if (pMotionCurveDriver)
      pMotionCurveDriver->Draw( this );

   if (!m_bDoPick && GetNavigatorInstance())
      GetNavigatorInstance()->PostDraw(this);

   if (!m_bDoPick && GetManipulatorInstance())
      GetManipulatorInstance()->PostDraw(this);

   if (!m_bDoPick)
      DrawViewControls();

   glPopMatrix();
}

void CGLTimeLineView::DefaultZoomFit(SnipeObjectList *pList)
{
   ASSERT(pList);

   float fov = m_pCurCamera->m_fFOV*DEGTORAD;
   float fMinValue=-100.0f, fMaxValue=100.0f;
   float fStartTime=0.0f, fStopTime=5.0f;

   BoundingBox bbox;
   pList->ComputeBoundingBox( bbox );
   if (!bbox.IsEmpty())
   {
      fMinValue = bbox.m_miny;
      fMaxValue = bbox.m_maxy;
      fStartTime = bbox.m_minx;
      fStopTime = bbox.m_maxx;
   }
   else // use entire curve
   {
      MotionCurveDriver *pMotionCurveDriver = GetDocument()->GetMotionCurveDriver();
      if (pMotionCurveDriver)
      {
         fMinValue = pMotionCurveDriver->GetMinValue();
         fMaxValue = pMotionCurveDriver->GetMaxValue();
         fStartTime = pMotionCurveDriver->GetStartTime().GetSeconds();
         fStopTime = pMotionCurveDriver->GetStopTime().GetSeconds();
      }
   }
   if (fMaxValue-fMinValue < 0.0001f)
   {
      fMaxValue += 0.0001f;
      fMinValue -= 0.0001f;
   }
   if (fStopTime-fStartTime < 0.0001f)
   {
      fStopTime += 0.0001f;
      fStartTime -= 0.0001f;
   }

   float fTargetHeight = (fMaxValue-fMinValue)*1.1f;
   float fTargetWidth = (fStopTime-fStartTime)*1.05f;
   float fVDistance = (fTargetHeight/2.0f)/(float)tan(fov/2.0f);
   float fHDistance = (fTargetWidth/2.0f)/(float)tan(fov/2.0f);
   float fControlRatio = 1.0f;
   if (m_szControl.cx)
      fControlRatio = (float)m_szControl.cy / (float)m_szControl.cx;
   float fAspectRatio = (fHDistance*fControlRatio)/fVDistance;

   Vector newtarget;
   newtarget.x = (fStartTime + fStopTime) * 0.5f;
   newtarget.y = (fMaxValue + fMinValue) * 0.5f;
   newtarget.z = 0.0f;

   Matrix44 cameramatrix;
   cameramatrix.Identity();
   Vector translate = Vector( 0.0f, 0.0f, fVDistance );
   translate += newtarget;
   m_pCurCamera->m_fFocusDistance.StoreValue(fVDistance);
   m_pCurCamera->m_transform.m_vTranslate.StoreValue( translate );
   Vector rotate( 0.0f, 0.0f, 0.0f );
   m_pCurCamera->m_transform.m_vRotate.StoreValue( rotate );
   m_pCurCamera->m_fAspectRatio.StoreValue(fAspectRatio);
   Invalidate();
}

////////////////////////////////////
// CGLTimeLineBar - Actually it is the whole bar for the output/history/timeline

CGLTimeLineBar::CGLTimeLineBar() :
 m_glOutputTabControl( FALSE, TRUE )
{
}

void CGLTimeLineBar::PostInit()
{
   CGLTransparentControl::PostInit();

   m_glTransportControl.Init( this );
   m_glOutputTabControl.Init( this );
   m_glOutputTabControl.m_bCenter = TRUE;
   m_glOutputTabControl.AddTab( "Output", (LPARAM)&m_glOutputFrame );
   m_glOutputTabControl.AddTab( "History", (LPARAM)&m_glHistoryControl );
   m_glOutputTabControl.AddTab( "Timeline", (LPARAM)&m_glTimeLineFrame );
}

void CGLTimeLineBar::Move( const CPoint &ptControl, const CSize &szControl, Justify justification/*=JUST_NOCHANGE*/ )
{
   CGLTransparentControl::Move( ptControl, szControl, justification );

   m_glTransportControl.Move( CPoint(0,0), CSize(szControl.cx, TRANSPORTHEIGHT) );
   m_glOutputTabControl.Move( CPoint(0,TRANSPORTHEIGHT), CSize(m_szControl.cx, m_szControl.cy-TRANSPORTHEIGHT) );
}

////////////////////////////////////
// CGLTimeLineFrame

CGLTimeLineFrame::CGLTimeLineFrame()
{
}

void CGLTimeLineFrame::PostInit()
{
   CGLBorderedControl::PostInit();

   m_glSplitter.Init( this );
   m_glSplitter.m_bFixedSize = TRUE;
   m_glSplitter.SetSize( 2, 1 );
   m_glSplitter.SetHandleSize( 9 );
   m_glSplitter.m_colwidtharray[0] = GetApp().GetProfileInt("ToolBars", "Timeline Tree Width", 200);
   m_glSplitter.m_colwidtharray[1] = 0.0f;
      m_glTimeLineTreeFrame.Init( &m_glSplitter );
      m_glTimeLineView.Init( &m_glSplitter );
}

void CGLTimeLineFrame::Move( const CPoint &ptControl, const CSize &szControl, Justify justification/*=JUST_NOCHANGE*/ )
{
   CGLBorderedControl::Move( ptControl, szControl, justification );

   m_glSplitter.Move( CPoint(0,0), szControl );
}

////////////////////////////////////
// CGLTimeLineTreeFrame

CGLTimeLineTreeFrame::CGLTimeLineTreeFrame() :
 m_glTab( TRUE, TRUE )
{
}

void CGLTimeLineTreeFrame::PostInit()
{
   CGLControl::PostInit();

   m_glTab.Init( this );

   m_glTab.AddTab( "Timeline", (LPARAM)&m_glTimeLineTree );
   m_glTab.AddTab( "Keyframe Editor", (LPARAM)&m_glKeyframeEditorTree );
}

void CGLTimeLineTreeFrame::Move( const CPoint &ptControl, const CSize &szControl, Justify justification/*=JUST_NOCHANGE*/ )
{
   CGLControl::Move( ptControl, szControl, justification );

   m_glTab.Move( CPoint(2,2), CSize( m_szControl.cx-4, m_szControl.cy-4 ));
}

////////////////////////////////////
// CGLTransportControl

CGLTransportControl::CGLTransportControl() :
   m_glCurrentFrameEdit( NULL ),
   m_glStartFrameEdit( NULL ),
   m_glEndFrameEdit( NULL )
{
}

void CGLTransportControl::PostInit()
{
   CGLTransparentControl::PostInit();

   m_glImageListButton.Create( IDB_TRANSPORTBUTTON, CRect(0,0,28,28), 28 );

   m_glStartFrameEdit.Init( this, NULL, CPoint(0,0), CSize(100,18), ID_STARTFRAMEEDIT );
   m_glEndFrameEdit.Init( this, NULL, CPoint(0,0), CSize(100,18), ID_ENDFRAMEEDIT );
   m_glCurrentFrameEdit.Init( this, NULL, CPoint(0,0), CSize(100,18), ID_CURRENTFRAMEEDIT, JUST_RIGHT );

   m_glRecordButton.SetButtonImageList( &m_glImageListButton );
   m_glRecordButton.Init( this, NULL, CPoint(0,0), CSize(28,28), ID_RECORD );
   m_glRecordButton.m_bCheckBox = TRUE;
   m_glGoToStartButton.SetButtonImageList( &m_glImageListButton );
   m_glGoToStartButton.Init( this, NULL, CPoint(0,0), CSize(28,28), ID_GOTOSTART );
   m_glGoToEndButton.SetButtonImageList( &m_glImageListButton );
   m_glGoToEndButton.Init( this, NULL, CPoint(0,0), CSize(28,28), ID_GOTOEND );
   m_glPlayFromStartButton.SetButtonImageList( &m_glImageListButton );
   m_glPlayFromStartButton.Init( this, NULL, CPoint(0,0), CSize(28,28), ID_PLAYFROMSTART );
   m_glPlayPauseButton.LoadButtonImageList( IDB_PLAYBUTTON, CRect(0,0,32,32), 32 );
   m_glPlayPauseButton.Init( this, NULL, CPoint(0,0), CSize(32,32), ID_PLAY );
   m_glPlayPauseButton.m_bCheckBox = TRUE;
   m_glLoopPlaybackButton.SetButtonImageList( &m_glImageListButton );
   m_glLoopPlaybackButton.Init( this, NULL, CPoint(0,0), CSize(28,28), ID_LOOPPLAYBACK );
   m_glLoopPlaybackButton.m_bCheckBox = TRUE;
   m_glGoToPreviousFrameButton.SetButtonImageList( &m_glImageListButton );
   m_glGoToPreviousFrameButton.Init( this, NULL, CPoint(0,0), CSize(28,28), ID_GOTOPREVIOUSFRAME );
   m_glGoToNextFrameButton.SetButtonImageList( &m_glImageListButton );
   m_glGoToNextFrameButton.Init( this, NULL, CPoint(0,0), CSize(28,28), ID_GOTONEXTFRAME );
   m_glPlayMuteAudioButton.SetButtonImageList( &m_glImageListButton );
   m_glPlayMuteAudioButton.Init( this, NULL, CPoint(0,0), CSize(28,28), ID_PLAYMUTEAUDIO );
   m_glPlayMuteAudioButton.m_bCheckBox = TRUE;
}

void CGLTransportControl::Move( const CPoint &ptControl, const CSize &szControl, Justify justification/*=JUST_NOCHANGE*/ )
{
   CGLTransparentControl::Move( ptControl, szControl, justification );

   int nWidth = 342;
   int nLeft = (szControl.cx - nWidth) / 2;
   int nTop = 2;

   m_glStartFrameEdit.Move( CPoint(4,4), m_glStartFrameEdit.m_szControl );
   m_glEndFrameEdit.Move( CPoint(108,4), m_glEndFrameEdit.m_szControl );
   m_glCurrentFrameEdit.Move( CPoint(104,4), m_glCurrentFrameEdit.m_szControl );

   m_glRecordButton.Move( CPoint(nLeft, nTop), m_glRecordButton.m_szControl );
   m_glGoToStartButton.Move( CPoint(m_glRecordButton.m_ptControl.x+m_glRecordButton.m_szControl.cx+16, nTop), m_glGoToStartButton.m_szControl );
   m_glGoToEndButton.Move( CPoint(m_glGoToStartButton.m_ptControl.x+m_glGoToStartButton.m_szControl.cx+4, nTop), m_glGoToEndButton.m_szControl );
   m_glPlayFromStartButton.Move( CPoint(m_glGoToEndButton.m_ptControl.x+m_glGoToEndButton.m_szControl.cx+16, nTop), m_glPlayFromStartButton.m_szControl );
   m_glPlayPauseButton.Move( CPoint(m_glPlayFromStartButton.m_ptControl.x+m_glPlayFromStartButton.m_szControl.cx+6, nTop-2), m_glPlayPauseButton.m_szControl );
   m_glLoopPlaybackButton.Move( CPoint(m_glPlayPauseButton.m_ptControl.x+m_glPlayPauseButton.m_szControl.cx+6, nTop), m_glLoopPlaybackButton.m_szControl );
   m_glGoToPreviousFrameButton.Move( CPoint(m_glLoopPlaybackButton.m_ptControl.x+m_glLoopPlaybackButton.m_szControl.cx+16, nTop), m_glGoToPreviousFrameButton.m_szControl );
   m_glGoToNextFrameButton.Move( CPoint(m_glGoToPreviousFrameButton.m_ptControl.x+m_glGoToPreviousFrameButton.m_szControl.cx+4, nTop), m_glGoToNextFrameButton.m_szControl );
   m_glPlayMuteAudioButton.Move( CPoint(m_glGoToNextFrameButton.m_ptControl.x+m_glGoToNextFrameButton.m_szControl.cx+16, nTop), m_glPlayMuteAudioButton.m_szControl );
}

/////////////////////////////////////////////////////////////////////////////
// CGLToolBarControl idle update through CToolCmdUI class

void CGLTransportControl::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
   CGLButtonCmdUI btnstate;

   btnstate.m_pOther = (CWnd *)&m_glRecordButton;
   btnstate.m_nID = ((CGLButtonControl *)btnstate.m_pOther)->m_nID;
   btnstate.DoUpdate(pTarget, bDisableIfNoHndler);

   btnstate.m_pOther = (CWnd *)&m_glGoToStartButton;
   btnstate.m_nID = ((CGLButtonControl *)btnstate.m_pOther)->m_nID;
   btnstate.DoUpdate(pTarget, bDisableIfNoHndler);

   btnstate.m_pOther = (CWnd *)&m_glGoToEndButton;
   btnstate.m_nID = ((CGLButtonControl *)btnstate.m_pOther)->m_nID;
   btnstate.DoUpdate(pTarget, bDisableIfNoHndler);

   btnstate.m_pOther = (CWnd *)&m_glPlayFromStartButton;
   btnstate.m_nID = ((CGLButtonControl *)btnstate.m_pOther)->m_nID;
   btnstate.DoUpdate(pTarget, bDisableIfNoHndler);

   btnstate.m_pOther = (CWnd *)&m_glPlayPauseButton;
   btnstate.m_nID = ((CGLButtonControl *)btnstate.m_pOther)->m_nID;
   btnstate.DoUpdate(pTarget, bDisableIfNoHndler);

   btnstate.m_pOther = (CWnd *)&m_glLoopPlaybackButton;
   btnstate.m_nID = ((CGLButtonControl *)btnstate.m_pOther)->m_nID;
   btnstate.DoUpdate(pTarget, bDisableIfNoHndler);

   btnstate.m_pOther = (CWnd *)&m_glGoToPreviousFrameButton;
   btnstate.m_nID = ((CGLButtonControl *)btnstate.m_pOther)->m_nID;
   btnstate.DoUpdate(pTarget, bDisableIfNoHndler);

   btnstate.m_pOther = (CWnd *)&m_glGoToNextFrameButton;
   btnstate.m_nID = ((CGLButtonControl *)btnstate.m_pOther)->m_nID;
   btnstate.DoUpdate(pTarget, bDisableIfNoHndler);

   btnstate.m_pOther = (CWnd *)&m_glPlayMuteAudioButton;
   btnstate.m_nID = ((CGLButtonControl *)btnstate.m_pOther)->m_nID;
   btnstate.DoUpdate(pTarget, bDisableIfNoHndler);
}
