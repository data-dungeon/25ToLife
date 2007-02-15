//

#pragma once

#include "GLTabControl.h"
#include "OutputView.h"
#include "GLHistoryListControl.h"
#include "PropertyBar.h"
#include "GLViewSplitter.h"
#include "GL2DView.h"
#include "BaseDoc.h"

class MotionCurveDriver;

class CTimeLineDoc : public CBaseDoc
{
protected:
	DECLARE_BASEDOC(CTimeLineDoc, CBaseDoc)

public:
         	CTimeLineDoc();
	virtual ~CTimeLineDoc();

// Operations
   MotionCurveDriver *GetMotionCurveDriver() { return (MotionCurveDriver *)m_pObjectHead; }

// CBaseDoc overrides
   virtual SnipeDocType GetDocType() { return SDT_CTimeLineDoc; }
   virtual HierObject *GetCommandObject() const;
   virtual void SetObjectHead( HierObject *pObjectHead );
};

class CGLTimeLineView : public CGL2DView
{
   DECLARE_BASEVIEW(CGLTimeLineView, CGL2DView)

public:
   static BoolProperty::Info m_bAutoZoomFitInfo;

   BoolProperty m_bAutoZoomFit;

   CTimeLineDoc m_timelinedoc;

            CGLTimeLineView();
   virtual ~CGLTimeLineView();

   CTimeLineDoc *GetDocument() const { return (CTimeLineDoc *)CGLBaseView::GetDocument(); }

// CGLBaseView overrides
   virtual void DefaultZoomFit( SnipeObjectList *pList );
   virtual void DrawContents();
};

class CGLTransportControl : public CGLTransparentControl
{
public:
   GLImageList      m_glImageListButton;

   CGLBasicPropertyEdit<Time> m_glCurrentFrameEdit;
   CGLBasicPropertyEdit<Time> m_glStartFrameEdit;
   CGLBasicPropertyEdit<Time> m_glEndFrameEdit;

   CGLButtonControl m_glRecordButton;
   CGLButtonControl m_glGoToStartButton;
   CGLButtonControl m_glGoToEndButton;
   CGLButtonControl m_glPlayFromStartButton;
   CGLButtonControl m_glPlayPauseButton;
   CGLButtonControl m_glLoopPlaybackButton;
   CGLButtonControl m_glGoToPreviousFrameButton;
   CGLButtonControl m_glGoToNextFrameButton;
   CGLButtonControl m_glPlayMuteAudioButton;

   CGLTransportControl();

// CGLControl overrides
   virtual void PostInit();
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );

// CCmdTarget overrides
   virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
};

class CGLTimeLineTreeFrame : public CGLControl
{
public:
   CGLTabControl             m_glTab;
      CGLObjectTreeControl      m_glTimeLineTree;
      CGLObjectTreeControl      m_glKeyframeEditorTree;

   CGLTimeLineTreeFrame();

// CGLControl overrides
   virtual void PostInit();
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
};

class CGLTimeLineFrame : public CGLBorderedControl
{
public:
   CGLSplitterControl      m_glSplitter;
      CGLTimeLineTreeFrame    m_glTimeLineTreeFrame;
      CGLTimeLineView         m_glTimeLineView;

   CGLTimeLineFrame();

// CGLControl overrides
   virtual void PostInit();
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
};

class CGLTimeLineBar : public CGLTransparentControl
{
public:
   CGLTransportControl      m_glTransportControl;
   CGLTabControl            m_glOutputTabControl;
      CGLOutputFrame           m_glOutputFrame;
      CGLHistoryListControl    m_glHistoryControl;
      CGLTimeLineFrame         m_glTimeLineFrame;

   CGLTimeLineBar();

// CGLControl overrides
   virtual void PostInit();
   virtual void Move( const CPoint &ptControl, const CSize &szControl, Justify justification=JUST_NOCHANGE );
};

