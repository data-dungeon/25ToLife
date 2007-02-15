//

#ifndef __STRCLASS_H
#include "StrClass.h"
#endif

class HModel;
class CExportMultiActionDialog : public CDialog
{  
   DECLARE_DYNAMIC(CExportMultiActionDialog)
   String m_filename;
   BOOL m_exportflattened;
   BOOL m_exportbaked;
   BOOL m_exportkfm;
   BOOL m_includemodelbone;
public:

   CExportMultiActionDialog(HModel *hmodel);
private:
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   virtual void DoDataExchange(CDataExchange* pDX);

	afx_msg void OnBrowse();
	DECLARE_MESSAGE_MAP()
};
