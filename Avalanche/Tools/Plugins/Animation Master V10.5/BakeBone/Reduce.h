//

#define __REDUCE_H

class CReduceDialog : public CDialog
{
   DECLARE_DYNAMIC(CReduceDialog)
   float m_errortolerance;

public:
   CReduceDialog( CWnd* pParentWnd=NULL );
private:
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   virtual void DoDataExchange(CDataExchange* pDX);
};
