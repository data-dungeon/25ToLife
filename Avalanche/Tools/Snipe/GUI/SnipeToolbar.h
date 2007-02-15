#pragma once

class CSnipeControlBar;
class String;

class CSnipeToolBar : public CExtToolControlBar
{
public:
	DECLARE_DYNCREATE(CSnipeToolBar);
   
   CSnipeToolBar() {
      m_dwMRUDockingState = 0;
   }

   void InitialEnableDocking(DWORD dwDockStyle) 
   {
      m_dwMRUDockingState = dwDockStyle;
      EnableDocking(dwDockStyle);
   }

   void UpdateButton(UINT cmdid);
   void SetAllCommandsBasic();
   
protected:
   DWORD m_dwMRUDockingState;

   virtual CExtBarContentExpandButton *OnCreateBarRightBtn();
   CSize _CalcLayout(DWORD dwMode, int nLength = -1 );
   CSize CalcDynamicLayout(int nLength, DWORD dwMode);
	CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz );

   afx_msg void OnToggleAllowDocking();

   DECLARE_MESSAGE_MAP()
};

class CSnipeControlBar : public CExtControlBar // This class can only have one child
{
	DECLARE_DYNAMIC(CSnipeControlBar)

public:
   DWORD m_dwMRUDockingState;

   CSnipeControlBar () {
      m_dwMRUDockingState = 0;
   }

   void InitialEnableDocking(DWORD dwDockStyle) 
   {
      m_dwMRUDockingState = dwDockStyle;
      EnableDocking(dwDockStyle);
   }
   void OnToggleAllowDocking();

   DECLARE_MESSAGE_MAP()
};

//***********************************************************************
//***********************************************************************
//***********************************************************************

class CPluginControlBar : public CExtControlBar
{
	DECLARE_DYNAMIC(CPluginControlBar)

public:
   DWORD m_dwMRUDockingState;
   BOOL m_bInstalledSuccessful;

   CPluginControlBar () {
      m_dwMRUDockingState = 0;
      m_bInstalledSuccessful = FALSE;
   }
   CPluginControlBar (HWND hwndChild) {
      m_dwMRUDockingState = 0;
      m_bInstalledSuccessful = FALSE;
   }
   ~CPluginControlBar() {
   }
   void OnToggleAllowDocking();
//   BOOL DoCreate(CFrameWnd *pFrame, UINT nCommandID, PluginReg *entry, //Not sure if Plugin Reg is correct yet 
    //UINT pluginindex, BOOL bCreateInvisible=FALSE);

protected:
   afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
   DECLARE_MESSAGE_MAP()
};
