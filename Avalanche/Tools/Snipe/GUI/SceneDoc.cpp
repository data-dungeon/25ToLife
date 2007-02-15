//
#include "stdafx.h"
#include "Snipe.h"
#include "SceneDoc.h"
#include "Scene.h"
#include "GLRenderWnd.h"
#include "MainFrm.h"
#include "resource.h"

IMPLEMENT_DYNCREATE(CSceneDoc, CModelDoc)

BEGIN_MESSAGE_MAP(CSceneDoc, CModelDoc)
   ON_COMMAND(ID_GOTOSTART, OnGotoStart)
   ON_COMMAND(ID_GOTOEND, OnGotoEnd)
   ON_COMMAND(ID_PLAYFROMSTART, OnPlayFromStart)
   ON_UPDATE_COMMAND_UI(ID_PLAY, OnUpdatePlay)
   ON_COMMAND(ID_PLAY, OnPlay)
   ON_UPDATE_COMMAND_UI(ID_LOOPPLAYBACK, OnUpdateLoopPlayback)
   ON_COMMAND(ID_LOOPPLAYBACK, OnLoopPlayback)
   ON_COMMAND(ID_GOTONEXTFRAME, OnGoToNextFrame)
   ON_UPDATE_COMMAND_UI(ID_GOTONEXTFRAME, OnUpdateGoToNextFrame)
   ON_COMMAND(ID_GOTOPREVIOUSFRAME, OnGoToPreviousFrame)
   ON_UPDATE_COMMAND_UI(ID_GOTOPREVIOUSFRAME, OnUpdateGoToPreviousFrame)
END_MESSAGE_MAP()

CSceneDoc::CSceneDoc()
{
   m_BoundingSphere.m_fRadius = 100.0f;
   m_bPlaying = FALSE;
   m_bLoopPlayback = TRUE;

   m_SelectableObjectTypeArray.Add( SOT_TransformableObject );

   m_SelectableObjectTypeArray.SetCurrentIndex( m_SelectableObjectTypeArray.GetNumInfos()-1 );
}

CSceneDoc::~CSceneDoc()
{
}

void CSceneDoc::SetObjectHead( HierObject *pObjectHead )
{
   CModelDoc::SetObjectHead( pObjectHead );

   m_pScene = (Scene *)pObjectHead->IsKindOf( SOT_Scene );
}

BOOL CSceneDoc::UsesObject( SnipeObject *pObject )
{
   if (pObject == GetApp().GetTimeProperty())
      return TRUE;

   return CModelDoc::UsesObject( pObject );
}

BOOL CSceneDoc::OnSnipeIdle()
{
   BOOL bMoreIdles = CModelDoc::OnSnipeIdle();

   if (!m_bPlaying)
      return bMoreIdles;

   GetApp().DrawInvalidControls();

   Time tPlaybackStart = GetPlaybackStartProperty()->GetCurrentValue();
   Time tPlaybackEnd = GetPlaybackEndProperty()->GetCurrentValue();
   Time tTime = GetApp().GetTime();

   if (TRUE) // play actual speed
   {
      LARGE_INTEGER liCurTime;
      QueryPerformanceCounter( &liCurTime );
      LARGE_INTEGER liFrequency;
      QueryPerformanceFrequency( &liFrequency );
      float fTimeSinceStart = (float)(liCurTime.QuadPart - m_liPlaybackStartTime.QuadPart) / (float)liFrequency.QuadPart;
      float fLength = (tPlaybackEnd - tPlaybackStart).GetSeconds();
      if (!m_bLoopPlayback && fTimeSinceStart>fLength)
      {
         m_bPlaying = FALSE;
         GetMainFrame()->m_glMainRenderWnd.m_glTimeLineBar.m_glTransportControl.m_glPlayPauseButton.SetChecked( FALSE );
         OnGotoEnd();
         return bMoreIdles;
      }
      fTimeSinceStart = fmod( fTimeSinceStart, fLength );

      Time tPreferred = tPlaybackStart + fTimeSinceStart;
      tPreferred.SnapToFrame( GetFPS() );
      if (tPreferred == tTime)
         return TRUE;
      tTime = tPreferred;
   }
   else // step one frame every time regardless
   {
      tTime.StepFrame( GetFPS() );

      if (tTime < tPlaybackStart)
         tTime = tPlaybackStart;
      else if (tTime > tPlaybackEnd)
      {
         if (m_bLoopPlayback)
            tTime = tPlaybackStart;
         else
         {
            m_bPlaying = FALSE;
            GetMainFrame()->m_glMainRenderWnd.m_glTimeLineBar.m_glTransportControl.m_glPlayPauseButton.SetChecked( FALSE );
            OnGotoEnd();
            return bMoreIdles;
         }
      }
   }

   GetApp().SetTime( tTime );

   return TRUE; // more idles please
}

void CSceneDoc::OnGotoStart()
{
   QueryPerformanceCounter( &m_liPlaybackStartTime );
   GetApp().SetTime( GetPlaybackStartProperty()->GetCurrentValue() );
   GetApp().DrawInvalidControls();
}

void CSceneDoc::OnGotoEnd()
{
   QueryPerformanceCounter( &m_liPlaybackStartTime );
   GetApp().SetTime( GetPlaybackEndProperty()->GetCurrentValue() );
   GetApp().DrawInvalidControls();
}

void CSceneDoc::OnPlayFromStart()
{
   m_bPlaying = TRUE;
   QueryPerformanceCounter( &m_liPlaybackStartTime );
   GetApp().SetTime( GetPlaybackStartProperty()->GetCurrentValue() );
   GetApp().DrawInvalidControls();
}

void CSceneDoc::OnUpdatePlay( CCmdUI *pCmdUI )
{
   pCmdUI->SetCheck( m_bPlaying );
}

void CSceneDoc::OnPlay()
{
   m_bPlaying = !m_bPlaying;

   if (m_bPlaying)
   {
      if (GetApp().GetTime() >= GetPlaybackEndProperty()->GetCurrentValue())
        OnPlayFromStart();
      else
      {
         Time tTimeSinceStart = GetApp().GetTime() - GetPlaybackStartProperty()->GetCurrentValue();

         LARGE_INTEGER liFrequency;
         QueryPerformanceFrequency( &liFrequency );
         QueryPerformanceCounter( &m_liPlaybackStartTime );
         m_liPlaybackStartTime.QuadPart -= liFrequency.QuadPart * tTimeSinceStart.GetSeconds();
      }
   }
}

void CSceneDoc::OnUpdateLoopPlayback( CCmdUI *pCmdUI )
{
   pCmdUI->SetCheck( m_bLoopPlayback );
}

void CSceneDoc::OnLoopPlayback()
{
   m_bLoopPlayback = !m_bLoopPlayback;
}

void CSceneDoc::OnUpdateGoToNextFrame( CCmdUI *pCmdUI )
{
   pCmdUI->Enable( !m_bPlaying );
}

void CSceneDoc::OnGoToNextFrame()
{
   Time oneframe( 1.0, GetFPS() );
   Time newframe = GetTime() + oneframe;
   GetApp().SetTime( newframe );
   GetApp().DrawInvalidControls();
}

void CSceneDoc::OnUpdateGoToPreviousFrame( CCmdUI *pCmdUI )
{
   pCmdUI->Enable( !m_bPlaying );
}

void CSceneDoc::OnGoToPreviousFrame()
{
   static Time zero( 0.0f );
   Time oneframe( 1.0, GetFPS() );
   Time newframe = GetTime() - oneframe;
   if (newframe < zero)
      newframe = zero;
   GetApp().SetTime( newframe );
   GetApp().DrawInvalidControls();
}

TimeProperty *CSceneDoc::GetPlaybackStartProperty()
{
   return &m_pScene->m_tPlaybackStart;
}

TimeProperty *CSceneDoc::GetPlaybackEndProperty()
{
   return &m_pScene->m_tPlaybackEnd;
}
