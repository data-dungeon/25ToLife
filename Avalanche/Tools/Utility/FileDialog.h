#pragma once

#include "AString.h"

#ifndef UTILITYEXPORT
#define UTILITYEXPORT
#endif

#define WM_MYINITDONE WM_USER+6

class UTILITYEXPORT SnipeFileDialog : public CFileDialog 
{
public:
   String m_strTitleBar;
   StringArray *m_filterarray;

            SnipeFileDialog( BOOL bOpenFileDialog, const String strInitFileName, const String &strFilter, const char *szTitleBar=NULL, int nInitFilterIndex=0, const char *szDefExt=NULL );
   virtual ~SnipeFileDialog();

   DECLARE_MESSAGE_MAP()
   afx_msg LRESULT OnMyInitDone( WPARAM wParam, LPARAM lParam );
   afx_msg void OnDestroy();

// CFileDialog Overrides
   virtual void OnTypeChange();
   virtual void OnInitDone();
};

BOOL UTILITYEXPORT GetOpenFileName( const String strInitFileName, const String &strFilter, String &strOutFileName, const char *szTitleBar=NULL, int *nOutIndex=NULL );
BOOL UTILITYEXPORT GetOpenFileName( const String strInitFileName, const String &strFilter, StringList &strOutFileNames, const char *szTitleBar=NULL, int *nOutIndex=NULL );
BOOL UTILITYEXPORT GetSaveFileName( const String strInitFileName, const String &strFilter, String &strOutFileName, const char *szTitleBar=NULL, int nInitFilterIndex=0, int *nOutIndex=NULL, const String &strAppenExt="" );


