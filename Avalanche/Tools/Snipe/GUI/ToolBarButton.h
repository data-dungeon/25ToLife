#pragma once

class CSnipeExtBarTextFieldButton : public CExtBarTextFieldButton
{
public:
   CSnipeExtBarTextFieldButton(INT nTextFieldWidth,
    CExtToolControlBar *pBar, UINT nCmdID, UINT nStyle)
    : CExtBarTextFieldButton(FALSE, nTextFieldWidth, pBar, nCmdID, nStyle) {}
    
   virtual void OnInplaceControlSessionEnd();
   virtual CWnd *OnInplaceControlCreate();
};

//*****************************************************************

class CSnipeExtBarSliderButton : public CExtBarSliderButton
{
public:
   CSnipeExtBarSliderButton(
    CExtToolControlBar * pBar = NULL,
    UINT nCmdID = ID_SEPARATOR,
    UINT nStyle = 0,
    ULONG nScrollTotalRange = 0L,
    ULONG nScrollPos = 0L,
    ULONG nScrollPageSize = 0L,
    INT nScrollButtonExtentH = 0,
    INT nScrollButtonExtentV = 0,
    INT nScrollControlExtentH = 50,
    INT nScrollControlExtentV = 50) 
    : CExtBarSliderButton(pBar, nCmdID, nStyle, nScrollTotalRange, nScrollPos,
     nScrollPageSize, nScrollButtonExtentH, nScrollButtonExtentV, 
     nScrollControlExtentH, nScrollControlExtentV) 
   {}
          
   ULONG ScrollPosSetNoSetTime( ULONG nScrollPos );// call this one from code to just update slider to proper time
   virtual ULONG ScrollPosSet( ULONG nScrollPos );
   virtual CExtSafeString GetText() const;
};
