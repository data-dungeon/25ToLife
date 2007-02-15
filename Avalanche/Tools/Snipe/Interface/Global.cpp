#include "stdafx.h"
#include "Global.h"
#include "..\Gui\MainFrm.h"
#include "..\Gui\OutputView.h"
#include "..\Gui\ProgressBar.h"

OutputWindowStream g_outputWindow;

////////////////////////////////////////////////////////////////////////////////////////////
// Output Dialog Stream Overrides
////////////////////////////////////////////////////////////////////////////////////////////

extern OutputWindowStream &operator << (OutputWindowStream &os,const char *line)
{
   CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
   if (pMainFrame)
      pMainFrame->GetMainRenderWnd()->m_glTimeLineBar.m_glOutputFrame.m_glOutputControl.AppendLine(line);
   return os;
}

extern OutputWindowStream &operator << (OutputWindowStream &os,const String &line)
{
   CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
   if (pMainFrame)
      pMainFrame->GetMainRenderWnd()->m_glTimeLineBar.m_glOutputFrame.m_glOutputControl.AppendLine(line.Get());
   return os;
}

extern OutputWindowStream &operator << (OutputWindowStream &os,const int &value)
{
   CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
   char line[256];
   if (pMainFrame && sprintf(line,"%i",value))
      pMainFrame->GetMainRenderWnd()->m_glTimeLineBar.m_glOutputFrame.m_glOutputControl.AppendLine(line);
   return os;
}

extern OutputWindowStream &operator << ( OutputWindowStream &os, const UINT &value )
{
   CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
   char line[256];
   if (pMainFrame && sprintf(line,"%i",value))
      pMainFrame->GetMainRenderWnd()->m_glTimeLineBar.m_glOutputFrame.m_glOutputControl.AppendLine(line);
   return os;
}

extern OutputWindowStream &operator << (OutputWindowStream &os,const long &value)
{
   CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
   char line[256];
   if (pMainFrame && sprintf(line,"%i",value))
      pMainFrame->GetMainRenderWnd()->m_glTimeLineBar.m_glOutputFrame.m_glOutputControl.AppendLine(line);
   return os;
}

extern OutputWindowStream &operator << (OutputWindowStream &os,const float &value)
{
   CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
   char line[256];
   if (pMainFrame && sprintf(line,"%f",value))
      pMainFrame->GetMainRenderWnd()->m_glTimeLineBar.m_glOutputFrame.m_glOutputControl.AppendLine(line);
   return os;
}

extern OutputWindowStream &operator << (OutputWindowStream &os,const double &value)
{
   CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
   char line[256];
   if (pMainFrame && sprintf(line,"%f",value))
      pMainFrame->GetMainRenderWnd()->m_glTimeLineBar.m_glOutputFrame.m_glOutputControl.AppendLine(line);
   return os;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Timer Functions
////////////////////////////////////////////////////////////////////////////////////////////

extern void TimerStart(LARGE_INTEGER &starttime)
{
   QueryPerformanceCounter(&starttime);
}

extern float TimerStop(LARGE_INTEGER &starttime)
{
   LARGE_INTEGER frequency,stoptime;
   QueryPerformanceFrequency(&frequency);

   QueryPerformanceCounter(&stoptime);
   return ((float)stoptime.QuadPart - starttime.QuadPart)/frequency.QuadPart;
}

////////////////////////////////////////////////////////////////////////////////////////////
// ProgressBar Functions
////////////////////////////////////////////////////////////////////////////////////////////

CProgressBar *pProgressBar=NULL;

extern void StartProgressBar( const String &strmessage, int totalitems )
{
//   GetMainFrame()->GetMainRenderWnd()->m_glStatusBarControl.m_glProgressControl.Start( strmessage, totalitems );
#if 0
   if (pProgressBar)
      delete pProgressBar;
   pProgressBar = new CProgressBar;
   pProgressBar->Create(strmessage,40,totalitems);
   pProgressBar->SetFactor(totalitems/100);
   pProgressBar->SetStep(pProgressBar->GetFactor());
#endif
}

extern void StepProgressBar()
{
//   GetMainFrame()->GetMainRenderWnd()->m_glStatusBarControl.m_glProgressControl.Step();
#if 0
   pProgressBar->SetVirtualPos(pProgressBar->GetVirtualPos()+1);
   if ((pProgressBar->GetFactor()) && (pProgressBar->GetVirtualPos()%pProgressBar->GetFactor() == 0))
      pProgressBar->StepIt();
#endif
}

extern void StopProgressBar()
{
//   GetMainFrame()->GetMainRenderWnd()->m_glStatusBarControl.m_glProgressControl.Stop();
#if 0
   pProgressBar->Clear();
   delete pProgressBar;
   pProgressBar = NULL;
#endif
}