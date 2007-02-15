//

#ifndef __STRCLASS_H
#include "StrClass.h"
#endif

class HActionCache;
class CExportActionDialog : public CDialog
{  
   DECLARE_DYNAMIC(CExportActionDialog)
   int m_exportframerange;
   float m_errortolerance;
   int m_beginframe,m_endframe;
   String m_filename;
public:
   CExportActionDialog(HActionCache *hac);
private:
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   virtual void DoDataExchange(CDataExchange* pDX);
	afx_msg void OnBrowse();
	DECLARE_MESSAGE_MAP()
};
